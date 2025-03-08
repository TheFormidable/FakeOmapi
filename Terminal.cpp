#include "Terminal.h"
#include "Service.h"

Terminal() {
    mName = ESE_TERMINAL;
    initialize();
}

void Terminal::initialize() {
    std::lock_guard<std::mutex> lock(mLock);
    
    if (mSEAIDLHal == nullptr) {
        mSEAIDLHal = ISecureElement::getService();
        if (mSEAIDLHal != nullptr) {
            mSEAIDLHal->init(mSEAIDLHalCallback);
            AIBinder_linkToDeath(mSEAIDLHal->asBinder().get(),
                                mDeathRecipient.get(), this);
            mIsConnected = true;
            
        }
    }
}

std::vector<uint8_t> Terminal::transmit(const std::vector<uint8_t>& cmd) {
    std::lock_guard<std::mutex> lock(mLock);
    
    if (!mIsConnected) {
        return {};
    }

    std::vector<uint8_t> response;
    if (mSEAIDLHal != nullptr) {
        mSEAIDLHal->transmit(cmd, &response);
    }

    if (response.size() == 0) {
        LOG(ERROR) << "Error in transmit()";
        std::cout << "Error in transmit()" << std::endl;
        return {};
    }

    size_t len = response.size();
    uint8_t sw1 = len >= 2 ? response[len-2] : 0;
    uint8_t sw2 = len >= 1 ? response[len-1] : 0;

    if (sw1 == 0x6C) {
        std::vector<uint8_t> newCmd(cmd);
        newCmd.back() = sw2;
        return transmit(newCmd);
    } else if (sw1 == 0x61) {
        do {
            std::vector<uint8_t> getResponseCmd = {cmd[0], 0xC0, 0x00, 0x00, sw2};
            auto tmp = transmit(getResponseCmd);
            
            response.insert(response.end()-2, tmp.begin(), tmp.end()-2);
            sw1 = tmp[tmp.size()-2];
            sw2 = tmp[tmp.size()-1];
        } while (sw1 == 0x61);
    }

    return response;
}

// sp<Channel> Terminal::openLogicalChannel(const std::vector<uint8_t>& aid, uint8_t p2) {
//     std::lock_guard<std::mutex> lock(mLock);
    
//     LogicalChannelResponse response;
//     if (mSEHal != nullptr) {
//         mSEHal->openLogicalChannel(aid, p2, &response);
//     }
    
//     sp<Channel> channel = new Channel(this, response.channelNumber, response.selectResponse);
//     mChannels[response.channelNumber] = channel;
//     return channel;
// }