#pragma once

#include <vector>
#include <map>
#include <mutex>

#include <android/binder_ibinder.h>

#include <utils/RefBase.h>

#include <aidl/android/se/omapi/BnSecureElementReader.h>
#include <aidl/android/se/omapi/BnSecureElementSession.h>

#include <aidl/android/hardware/secure_element/ISecureElement.h>
#include <aidl/android/hardware/secure_element/BnSecureElement.h>
#include <aidl/android/hardware/secure_element/ISecureElementCallback.h>

// #include "Channel.h"

// using 
using aidl::android::hardware::secure_element::ISecureElement;
using aidl::android::hardware::secure_element::ISecureElementCallback;
using aidl::android::hardware::secure_element::BnSecureElement;

class Terminal : public android::RefBase {
    public:
        Terminal();
        virtual ~Terminal();

        void close();
        bool isSecureElementPresent();
        std::vector<uint8_t> transmit(const std::vector<uint8_t>& data);
        std::vector<uint8_t> getAtr();

        // sp<Channel> openBasicChannel(const std::vector<uint8_t>& aid, uint8_t p2);
        // sp<Channel> openLogicalChannel(const std::vector<uint8_t>& aid, uint8_t p2);
        // void closeChannel(int channelNumber);

    private:
        std::shared_ptr<ISecureElement> mSEAIDLHal;
        std::shared_ptr<ISecureElementCallback> mSEAIDLHalCallback;
    
        std::mutex mLock;
        bool mIsConnected;
        // std::map<int, sp<Channel>> mChannels;
        std::string mName;

        // AIBinder_DeathRecipient mDeathRecipient;

        // TODO
        // sp<AccessControlEnforcer> mAccessControlEnforcer;

        // void handleStateChange(bool state, const std::string& reason);
        // status_t initializeHALInterfaces(bool retryOnFail);
        // void resetInternalState();
};
