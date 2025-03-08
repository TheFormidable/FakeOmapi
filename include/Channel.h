#pragma once

#include "Terminal.h"

class Channel : public RefBase {
    public:
        Channel(Terminal* terminal, int channelNumber, 
                const std::vector<uint8_t>& selectResponse);
        virtual ~Channel();
    
        status_t transmit(const std::vector<uint8_t>& command,
                          std::vector<uint8_t>& response);
        void close();
    
    private:
        Terminal* mTerminal;
        int mChannelNumber;
        std::vector<uint8_t> mSelectResponse;
    };