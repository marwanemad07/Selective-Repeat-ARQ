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

#ifndef UTILS_H_
#define UTILS_H_

#include "../src/Message_m.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <bitset>

class Utils {
public:
    Utils();
    static std::string readFile(std::string path);
    static std::vector<std::pair<std::string, std::string>> readLines(std::string path);
    static Message_Base* castMessage(cMessage *msg);
    static std::string createFrame(std:: string payload);
    static std::string deframe(std::string frame);
    static std::string createCRC(const std::string& data, const std::string& generator);
    static bool validateCRC(const std::string& dataWithCRC, const std::string& generator);
    static std::string convertToBitStream(const std::string &s);
    static std::string charToBits(char character);
    static char bitsToChar(const std::string& bits);
    virtual ~Utils();
};

#endif /* UTILS_H_ */
