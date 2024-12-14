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
    } else if (std::string(moduleName) == "Node1") {
        filePath = "../input/input1.txt";
    }

    maxSeqNumber = (2 * windowSize) -  1;
    arrived.resize(maxSeqNumber+1);
    bufferLines.resize(maxSeqNumber+1);
    // input lines text, to use it later for each node

}

void Node::handleMessage(cMessage *msg)
{
    if(!strcmp(msg->getName(), "coordinator")) {
        isSender = true;
    }

    Message_Base* curMsg = Utils::castMessage(msg);
    if(curMsg->isSelfMessage()){
        if(isSender){

        } else{

        }
    } else if(isSender) {
        sender(curMsg);
    } else {
        reciever(curMsg);
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
    EV << "trailer before converting to char: " << crc << "\n";
    newMsg->setTrailer(Utils::bitsToChar(crc));

    return newMsg;
}

void Node::sender(Message_Base* msg) {
    EV << "Inside sender\n";
    if(!strcmp(msg->getName(), "coordinator") ) {
        EV << "Before read file\n";
        std::string filePath = "../input/input0.txt";
        setMessageLines(Utils::readLines(filePath));
        EV << "After read file\n";
    }
    int frameType = msg->getFrameType();
    int ackNackNum = msg->getAckNackNumber();


    if(frameType == 0) {
        EV<<"Nack ";
        string messageText = bufferLines[ackNackNum];
        EV << "message text: " <<ackNackNum <<" "<<messageText;

        Message_Base* newMsg = getNewMessage(messageText);

        newMsg->setHeader(ackNackNum);
        newMsg->setFrameType(2);

        EV << newMsg->getTrailer();
        send(newMsg, "out");
    } else {
        EV<<"ACK ";
        // get next message of this index
        moveSenderWindow(ackNackNum);
        std::pair<std::string, std::string> messageLine = getNextMessage();
        std::string errorCode = messageLine.first;
        std::string messageText = messageLine.second;

        EV << "message text: " <<messageText;
        Message_Base* newMsg = getNewMessage(messageText);

        bufferLines[curWindowIndex] = messageText;
        printVector(bufferLines);



        newMsg->setHeader(curWindowIndex);
        newMsg->setFrameType(2);

        EV << newMsg->getTrailer();

        send(newMsg, "out");
        increaseCurIndex();

    }
    cancelAndDelete(msg);

}
void Node::reciever(Message_Base* msg) {
    std::string payload = Utils::deframe(msg->getPaylaod());
    EV << "Revieved payload: " << msg->getPaylaod() << endl;
    printVector(bufferLines);
    std::string trailerBits = Utils::charToBits(msg->getTrailer());
    std::string payloadBits = Utils::convertToBitStream(payload);


    bool validCrc = Utils::validateCRC(payloadBits + trailerBits, this->generator);

    EV << validCrc;
    if(!validCrc)
        return; //

    int seqNumber = msg->getHeader();
    EV << "seqNumber: " <<seqNumber<<" startWindowIndex "<<startWindowIndex<< endl;
    if(seqNumber != startWindowIndex)
        sendAckNack(startWindowIndex,0); //nack
    else{
        moveReciverWindow(seqNumber);
        sendAckNack(seqNumber+1,1);//send next
    }
        cancelAndDelete(msg);
}

void Node::moveSenderWindow(int ackNumber) {
    startWindowIndex = ackNumber;

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
    endWindowIndex = (endWindowIndex + 1) %(maxSeqNumber + 1);
}

void Node::increaseCurIndex() {
    // need to be revise
    int windowSize = par("WS"); // TODO: save these pars to not repeat code
    int maxSeqNumber =(2 * windowSize) - 1;
    if((startWindowIndex <= curWindowIndex and  curWindowIndex < endWindowIndex)
       or (startWindowIndex > endWindowIndex and curWindowIndex >= startWindowIndex)
       or (startWindowIndex > endWindowIndex and curWindowIndex < endWindowIndex)
       ) {
        curWindowIndex = (curWindowIndex + 1) % (maxSeqNumber + 1);
    }
}

void Node::sendAckNack(int seqNumber,int type) {
    Message_Base* msg = new Message_Base();
    msg->setFrameType(type);
    msg->setAckNackNumber((seqNumber) % (this->maxSeqNumber + 1));
    msg->setHeader(seqNumber); //TODO: ask IF WE SHOULD ADD IT

    send(msg, "out");
}


