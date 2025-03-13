#include "ByteArrayConverter.h"

#include <vector>
#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>

std::string hex2string(std::vector<uint8_t> hex) {
    std::stringstream ss;
    ss << "";
    for (size_t i = 0; i < hex.size(); ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') 
           << static_cast<int>(hex[i]);
        if (i < hex.size() - 1) ss << " ";
    }
    return ss.str();
}

std::vector<uint8_t> hexStringToBytes(const std::string& hex) {
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoul(byteString, nullptr, 16));
        bytes.push_back(byte);
    }
    return bytes;
}
