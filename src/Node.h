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

#ifndef __PROJECT_NODE_H_
#define __PROJECT_NODE_H_

#include <omnetpp.h>
#include <vector>
#include "Message_m.h"

using namespace omnetpp;
using namespace std;
/**
 * TODO - Generated class
 */
class Node : public cSimpleModule
{
  private:
    // a vector of pair for {error code, message to send}
    std::vector<std::pair<std::string, std::string>> messageLines;
    std::vector<bool> arrived;
    std::vector<std::string> bufferLines;
    std::vector<bool> isSended;
    int startWindowIndex = 0;
    int curWindowIndex = 0;
    int endWindowIndex = 0;
    const std::string generator = "101";
    bool isSender = false;
    int messageIndex = 0 ;

    int nodeId;
    std::string filePath;

    // .ini parameters
    int windowSize;
    int maxSeqNumber = 0;
    double PT;
    int SN;
    double TO;
    double TD;
    double ED;
    double DD;
    int LP;

    Message_Base* getNewMessage(std::string message);
    void sender(Message_Base* msg);
    void senderSelfMessage(Message_Base* msg);
    void reciever(Message_Base* msg);
    void recieverSelfMessage(Message_Base* msg);
    void moveSenderWindow(int ackNumber);
    void moveReciverWindow(int seqNumber);
    void increaseCurIndex();
    void sendAckNack(int seqNumber,int type);
    bool isBetween(int l, int m, int r);
    int modificationError(Message_Base* msg);
    bool isSecondMessageVersion(int kind);
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

  public:
    void setMessageLines(std::vector<std::pair<std::string, std::string>> lines);
    std::pair<std::string, std::string> getNextMessage();
    void incrementMessageIndex();
};

#endif
