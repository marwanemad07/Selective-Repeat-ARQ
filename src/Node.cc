//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "Node.h"
#include "../utils/Utils.cc"

/*
 * Kind bits => 0 -> to know processing or timeout
 *              1 -> to know that a duplicate message is the first one or second
 *
 * */

void printVector(const std::vector<string>& vec) {
    std::cout << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i];
        if (i < vec.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

Define_Module(Node);

void Node::initialize()
{
    // TODO - Generated method body
    windowSize = par("WS").intValue();
    PT = par("PT").doubleValue();
    SN = par("SN").intValue();
    TO = par("TO").doubleValue();
    TD = par("TD").doubleValue();
    ED = par("ED").doubleValue();
    DD = par("DD").doubleValue();
    LP = par("LP").intValue();

    const char* moduleName = getName();

    if (std::string(moduleName) == "Node0") {
        filePath = "../input/input7.txt";
        nodeId = 0;
    } else if (std::string(moduleName) == "Node1") {
        filePath = "../input/input0.txt";
        nodeId = 1;
    }

    endWindowIndex = windowSize - 1;
    maxSeqNumber = (2 * windowSize) -  1;
    arrived.resize(maxSeqNumber+1);
    isSended.resize(maxSeqNumber+1);
    bufferLines.resize(maxSeqNumber+1);
    // input lines text, to use it later for each node
}

void Node::handleMessage(cMessage *msg)
{
    if(!strcmp(msg->getName(), "coordinator")) {
        isSender = true;
    }

    Message_Base* curMsg = Utils::castMessage(msg);
    if(isSender) {
        if(curMsg->isSelfMessage())
            this->senderSelfMessage(curMsg);
        else
            this->sender(curMsg);
    }
    else {
        if(curMsg->isSelfMessage())
            this->recieverSelfMessage(curMsg);
        else
            this->reciever(curMsg);
    }
}

void Node::setMessageLines(std::vector<std::pair<std::string, std::string>> lines) {
    messageLines = lines;
}


std::pair<std::string, std::string> Node::getNextMessage() {
    if (messageIndex < 0 || messageIndex >= (int)(messageLines.size())) {
        throw std::out_of_range("Index out of range");
    }
    std::pair<std::string, std::string> message = messageLines[messageIndex];
    incrementMessageIndex();
    return message;
}

void Node::incrementMessageIndex(){
    this->messageIndex++;
}

Message_Base* Node::getNewMessage(std::string message) {
    Message_Base* newMsg = new Message_Base();
    std::string frame = Utils::createFrame(message);
    newMsg->setPaylaod(frame.c_str());

    std::string bitStream = Utils::convertToBitStream(message);
    std::string crc = Utils::createCRC(bitStream, this->generator);
    newMsg->setTrailer(Utils::bitsToChar(crc));

    return newMsg;
}

void Node::sender(Message_Base* msg) {
    bool isCoordinator = !strcmp(msg->getName(), "coordinator");
    if(isCoordinator) {
        setMessageLines(Utils::readLines(filePath));
    }

    int frameType = msg->getFrameType();
    int ackNackNum = msg->getAckNackNumber();

    if(frameType == 0) {
        string messageText = bufferLines[ackNackNum];

        Message_Base* newMsg = getNewMessage(messageText);

        newMsg->setHeader(ackNackNum);
        newMsg->setFrameType(2);

        send(newMsg, "out"); // i think we need to log for nack
    } else {
        // get next message of this index
        if(!isCoordinator)
            moveSenderWindow(ackNackNum);

        std::pair<std::string, std::string> messageLine = getNextMessage();
        std::string errorCode = messageLine.first;
        std::string messageText = messageLine.second;

        Message_Base* newMsg = getNewMessage(messageText);

        bufferLines[curWindowIndex] = messageText;

        newMsg->setHeader(curWindowIndex);
        newMsg->setFrameType(2);


        Utils::logChannelError(simTime(), nodeId, errorCode);
        // this should be after the end processing

        newMsg->setKind(0);
        newMsg->setName(errorCode.c_str());
        scheduleAt(simTime() + PT, newMsg);
    }
    // cancelAndDelete(msg);
}


void Node::senderSelfMessage(Message_Base* msg) {
    int seqNumber = msg->getHeader();
    int kind = msg->getKind();

    bool discardErrors = ((kind >> 2) & 1);
    std::string errorCode = msg->getName();
    int modifiedBit = 0;

    if((kind & 1) == 0) {
        Message_Base* timOutMsg = msg->dup(); // a pure message without any errors

        if(!discardErrors) {
            if(errorCode[0] == '1')
                modifiedBit = modificationError(msg);


            if(errorCode[1] == '0'){
                // if there's a delay, set the kind
                int delayKind = msg->getKind();
                msg->setKind(delayKind | (1 << 2)); // set the third bit with 1 to discard errors again

                double errorDelay = 0.0;
                if(errorCode[3] == '1') errorDelay = ED;

                if(errorCode[2] == '1') {
                    Message_Base* dupMsg = msg->dup();
                    int dupKind = dupMsg->getKind();
                    dupKind |= (1 << 1);
                    dupMsg->setKind(dupKind);
                    dupMsg->setName(errorCode.c_str());
                    scheduleAt(simTime() + errorDelay + DD, dupMsg);
                }

                if(errorDelay != 0.0){
                    msg->setName(errorCode.c_str());
                    scheduleAt(simTime() + errorDelay, msg->dup());
                }
            }
        }

        int duplicateLog = 0;
        if (errorCode[2] == '1'){
            if( (( msg->getKind() >> 1 ) & 1) == 0) duplicateLog = 1;
            else duplicateLog = 2;
        }

        double errorDelayInterval = 0.0;
        if(errorCode[3] == '1' and !discardErrors) errorDelayInterval = ED;

        Utils::logFrameTransmission(simTime(), nodeId, seqNumber, msg->getPaylaod(),
                                msg->getTrailer(), modifiedBit, errorCode[1] == '1', duplicateLog, errorDelayInterval);

        if(errorDelayInterval == 0.0 and !isSecondMessageVersion(kind)){
            sendDelayed(msg, TD, "out");
            increaseCurIndex();
        }

        // this is for handling timeout

        timOutMsg->setKind(kind | 1);
        scheduleAt(simTime() + TD + TO, timOutMsg);
        return;
    }

    if(not isSended[seqNumber] and !isSecondMessageVersion(kind)) {
        // there will be no error again as said
        double delay = PT + TD;
        Utils::logTimeoutEvent(simTime(), nodeId, seqNumber);
        sendDelayed(msg, delay,"out");
    }
}

void Node::reciever(Message_Base* msg) {
    int seqNumber = msg->getHeader();
    // need to know if this frame not repeated

    if(!isBetween(startWindowIndex, seqNumber, endWindowIndex)) {
        double delay = PT + TD;
        Utils::logControlFrame(simTime(), nodeId, 1, seqNumber + 1, 0); // TODO: i think we shouldn't print this again
        return;
    }

    std::string payload = Utils::deframe(msg->getPaylaod());
    std::string trailerBits = Utils::charToBits(msg->getTrailer());
    std::string payloadBits = Utils::convertToBitStream(payload);

    bool validCrc = Utils::validateCRC(payloadBits + trailerBits, this->generator);

    if(!validCrc)
        return;

    Utils::logPayloadUpload(payload, seqNumber);
    if(seqNumber != startWindowIndex) {
        sendAckNack(startWindowIndex, 0); //nack
    }
    else{
        moveReciverWindow(seqNumber);
        sendAckNack(seqNumber+1, 1);//send next
    }
    // cancelAndDelete(msg);
}

void Node::recieverSelfMessage(Message_Base* msg) {
    int ackNackNumber = msg->getAckNackNumber();
    int lossProb = LP / 100.0;
    if(lossProb <= 0.5) {
        Utils::logControlFrame(simTime(), nodeId, 1, ackNackNumber, 0);
        sendDelayed(msg, TD, "out");
    }
    else {
        // Don't do anything, we have to check in receiving, should i discard the coming message or not
        // i think we should only discard the frames out of the window and ...
        Utils::logControlFrame(simTime(), nodeId, 1, ackNackNumber, 1);
    }
}

void Node::moveSenderWindow(int ackNumber) {
    while(startWindowIndex != ackNumber){
        isSended[startWindowIndex] = true;
        startWindowIndex = (startWindowIndex + 1) % (maxSeqNumber + 1);
    }
    endWindowIndex = (startWindowIndex + par("WS").intValue()) % (maxSeqNumber + 1);
}

void Node::moveReciverWindow(int seqNumber) {
//    if( seqNumber > ( startWindowIndex + windowSize ) )
//        return;
    arrived[seqNumber] = true;
    while(arrived[startWindowIndex]){
        arrived[startWindowIndex] = false;
        startWindowIndex = (startWindowIndex + 1) %(maxSeqNumber + 1);
    }
    endWindowIndex = (startWindowIndex + windowSize) %(maxSeqNumber + 1);
}

void Node::increaseCurIndex() {
    // need to be revise
    int windowSize = par("WS"); // TODO: save these pars to not repeat code
    int maxSeqNumber =(2 * windowSize) - 1;
    if(isBetween(startWindowIndex, curWindowIndex, endWindowIndex)) {
        curWindowIndex = (curWindowIndex + 1) % (maxSeqNumber + 1);
    }
}

void Node::sendAckNack(int seqNumber,int type) {
    Message_Base* msg = new Message_Base();
    msg->setFrameType(type);
    msg->setAckNackNumber((seqNumber) % (this->maxSeqNumber + 1));
    msg->setHeader(seqNumber); //TODO: ask IF WE SHOULD ADD IT

    msg->setKind(0); // 0 means we're waiting for processing, may not be needed here
    scheduleAt(simTime() + PT, msg);
}

bool Node::isBetween(int s, int m, int e) {
    return (s <= m and  m < e)
           or (s > e and m >= s)
           or (s > e and m < e);
}

int Node::modificationError(Message_Base* msg) {

    std::string bitStream = Utils::convertToBitStream(msg->getPaylaod());

    int errorBit = intuniform(8, bitStream.size() - 8);
    bitStream[errorBit] = bitStream[errorBit] == '0' ? '1' : '0';

    std::string payload = Utils::bitsToString(bitStream);
    msg->setPaylaod(payload.c_str());
    // TODO: need to print which value is inverted
    return errorBit;
}

bool Node::isSecondMessageVersion(int kind) {
    return (kind >> 1) & 1;
}
