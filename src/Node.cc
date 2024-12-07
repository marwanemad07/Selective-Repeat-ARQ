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


Define_Module(Node);

void Node::initialize()
{
    // TODO - Generated method body
    const char* moduleName = getName();
    std::string filePath;

    if (std::string(moduleName) == "Node0") {
        filePath = "../input/input0.txt";
    } else if (std::string(moduleName) == "Node1") {
        filePath = "../input/input1.txt";
    }

    // input lines text, to use it later for each node
    setMessageLines(Utils::readLines(filePath));
}

void Node::handleMessage(cMessage *msg)
{
    Message_Base* curMsg = Utils::castMessage(msg);

    if(curMsg->isSelfMessage()){

    }else {
        std::pair<std::string, std::string> messageLine = getNextMessage();
        Message_Base* newMsg = getNewMessage(messageLine.second);
        EV <<  newMsg->getPaylaod() << ' '<<newMsg->getTrailer();
        send(newMsg, "out");
    }
}

void Node::setMessageLines(std::vector<std::pair<std::string, std::string>> lines) {
    messageLines = lines;
}


std::pair<std::string, std::string> Node::getNextMessage() {
    if (lastMessageIndex < 0 || lastMessageIndex >= (int)(messageLines.size())) {
        throw std::out_of_range("Index out of range");
    }
    std::pair<std::string, std::string> message = messageLines[lastMessageIndex];
    incrementMessageIndex();
    return message;
}

void Node::incrementMessageIndex(){
    this->lastMessageIndex++;
}

Message_Base* Node::getNewMessage(std::string message) {
    Message_Base* newMsg = new Message_Base();
    std::string frame = Utils::createFrame(message);
    newMsg->setPaylaod(frame.c_str());

    std::string bitStream = Utils::convertToBitStream(frame);
    std::string crc = Utils::createCRC(bitStream, this->generator);
    newMsg->setTrailer(Utils::bitsToChar(crc));

    return newMsg;
}
