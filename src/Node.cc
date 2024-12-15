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
    std::string filePath;

    if (std::string(moduleName) == "Node0") {
        filePath = "../input/input0.txt";
        nodeId = 0;
    } else if (std::string(moduleName) == "Node1") {
        filePath = "../input/input1.txt";
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
        std::string filePath = "../input/input0.txt";
        setMessageLines(Utils::readLines(filePath));
    }

    int frameType = msg->getFrameType();
    int ackNackNum = msg->getAckNackNumber();

    if(frameType == 0) {
        string messageText = bufferLines[ackNackNum];

        Message_Base* newMsg = getNewMessage(messageText);

        newMsg->setHeader(ackNackNum);
        newMsg->setFrameType(2);

        send(newMsg, "out");
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
        newMsg->setKind(0);
        newMsg->setName(errorCode.c_str());
        scheduleAt(simTime() + PT, newMsg);
    }
    cancelAndDelete(msg);
}


void Node::senderSelfMessage(Message_Base* msg) {
    int seqNumber = msg->getHeader();
    int kind = msg->getKind();
    if(kind == 0) {
        Utils::logFrameTransmission(simTime(), nodeId, seqNumber, msg->getPaylaod(),
                msg->getTrailer(), 0, 0, 0, 0);

        sendDelayed(msg, TD, "out");
        increaseCurIndex();

        // this is for handling timeout
        Message_Base* dupMsg = msg->dup();
        dupMsg->setKind(1);
        scheduleAt(simTime() + TD + TO, dupMsg);
        return;
    }

    if(not isSended[seqNumber]) {
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
        Utils::logControlFrame(simTime(), nodeId, 1, seqNumber + 1, 0);
        return;
    }

    std::string payload = Utils::deframe(msg->getPaylaod());
    std::string trailerBits = Utils::charToBits(msg->getTrailer());
    std::string payloadBits = Utils::convertToBitStream(payload);

    bool validCrc = Utils::validateCRC(payloadBits + trailerBits, this->generator);

    if(!validCrc)
        return; //

    Utils::logPayloadUpload(payload, seqNumber);

    if(seqNumber != startWindowIndex) {
        sendAckNack(startWindowIndex, 0); //nack
    }
    else{
        moveReciverWindow(seqNumber);
        sendAckNack(seqNumber+1, 1);//send next
    }
    cancelAndDelete(msg);
}

void Node::recieverSelfMessage(Message_Base* msg) {
    int ackNackNumber = msg->getAckNackNumber();
    int lossProb = PT / 100.0;
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
