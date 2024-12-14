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

#include "Utils.h"

Utils::Utils() {
    // TODO Auto-generated constructor stub

}

Utils::~Utils() {
    // TODO Auto-generated destructor stub
}

std::string Utils::readFile(std::string path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::ios_base::failure("Error: Could not open file at path: " + path);
    }

    std::ostringstream content;
    content << file.rdbuf();
    file.close();
    return content.str();
}

std::vector<std::pair<std::string, std::string>> Utils::readLines(std::string path) {
    std::string fileContent = readFile(path);

    std::vector<std::pair<std::string, std::string>> lines;
    std::istringstream stream(fileContent);
    std::string line;

    while (std::getline(stream, line)) {
        std::string errorCode = line.substr(0, 4);
        std::string message = line.substr(5);
        lines.push_back({errorCode, message});
    }

    return lines;
}

Message_Base* Utils::castMessage(cMessage *msg) {
    Message_Base* castedMsg = check_and_cast<Message_Base *>(msg);
    return castedMsg;
}

std::string Utils::createFrame(std::string payload) {
    const char FLAG = '$';
    const char ESCAPE = '/';

    std::string framedMessage = "";
    framedMessage += FLAG;

    for (char c : payload) {
        if (c == FLAG || c == ESCAPE) {
            framedMessage += ESCAPE;
        }
        framedMessage += c;
    }

    framedMessage += FLAG;
    return framedMessage;
}

std::string Utils::deframe(std::string frame) {
    const char FLAG = '$';
    const char ESCAPE = '/';

    std::string payload = "";
    bool escapeNext = false;
    std::cout << "frame: " << frame;
    if (frame.size() < 2 || frame.front() != FLAG || frame.back() != FLAG) {
        throw std::invalid_argument("Invalid frame: Missing start or end FLAG.");
    }

    for (int i = 1; i < frame.size() - 1; ++i) {
        char c = frame[i];

        if (escapeNext) {
            payload += c;
            escapeNext = false;
        } else if (c == ESCAPE) {
            escapeNext = true;
        } else {
            payload += c;
        }
    }

    if (escapeNext) {
        throw std::invalid_argument("Invalid frame: Ends with incomplete escape sequence.");
    }

    return payload;
}

std::string Utils::createCRC(const std::string& data, const std::string& generator) {
    std::string augmentedData = data + std::string(generator.size() - 1, '0');
    EV << "augmented data: " << augmentedData << endl;
    std::string remainder = augmentedData;

    // Perform binary division
    for (size_t i = 0; i <= augmentedData.size() - generator.size(); ++i) {
        if (remainder[i] == '1') {
            // XOR operation for the generator polynomial
            for (size_t j = 0; j < generator.size(); ++j) {
                remainder[i + j] = (remainder[i + j] == generator[j]) ? '0' : '1';
            }
        }
    }

    // The remainder after the division is the CRC
    std::string crc = remainder.substr(augmentedData.size() - generator.size() + 1);
    return crc;
}

bool Utils::validateCRC(const std::string& dataWithCRC, const std::string& generator) {
    std::string remainder = dataWithCRC;
    EV << "Data with crc: " <<dataWithCRC << endl;
    // Perform binary division
    for (size_t i = 0; i <= remainder.size() - generator.size(); ++i) {
        if (remainder[i] == '1') {
            // XOR operation for the generator polynomial
            for (size_t j = 0; j < generator.size(); ++j) {
                remainder[i + j] = (remainder[i + j] == generator[j]) ? '0' : '1';
            }
        }
    }

    // If the remainder is all zeros, the CRC is valid
    return remainder.substr(remainder.size() - generator.size() + 1) == std::string(generator.size() - 1, '0');
}

std::string Utils::convertToBitStream(const std::string &s){
    int charCount = s.size();
    std::string bitStream = std::bitset<8>(charCount).to_string();

    for(auto c : s){
        std::bitset<8> bits(c);
        bitStream += bits.to_string();
    }

    return bitStream;
}

std::string Utils::charToBits(char character) {
    std::string bits = std::bitset<8>(static_cast<unsigned char>(character)).to_string();
    std::string result = "";
    result += bits[6];
    result += bits[7];
    return result;
}


char Utils::bitsToChar(const std::string& bits){
    std::string paddedBits = bits;
    while (paddedBits.size() < 8) {
        paddedBits = "0" + paddedBits;
    }
    return static_cast<char>(std::bitset<8>(paddedBits).to_ulong());
}
