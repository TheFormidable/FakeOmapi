#pragma once

#include <vector>
#include <map>
#include <mutex>

#include <android-base/logging.h>
#include <android/binder_ibinder.h>

#include <utils/RefBase.h>

#include <aidl/android/hardware/secure_element/ISecureElement.h>
#include <aidl/android/hardware/secure_element/BnSecureElementCallback.h>

#include <aidl/android/se/omapi/ISecureElementListener.h>
#include <aidl/android/se/omapi/ISecureElementSession.h>
#include <aidl/android/se/omapi/ISecureElementReader.h>

#include <string>
#include <iostream>
// #include "AccessControl/AccessControlEnforcer.h"

using aidl::android::hardware::secure_element::ISecureElement;
using aidl::android::hardware::secure_element::BnSecureElementCallback;
using aidl::android::se::omapi::ISecureElementListener;
using aidl::android::se::omapi::ISecureElementSession;
using aidl::android::se::omapi::ISecureElementReader;

namespace aidl::android::se {
namespace omapi {
class SecureElementService;
};
class Channel;
class SecureElementReader;

using aidl::android::se::Channel;
// using aidl::android::se::SecureElementReader;
using aidl::android::se::omapi::SecureElementService;

class Terminal : public ::android::RefBase {
public:
    Terminal(const std::string name);
    // virtual ~Terminal() = default;

    void initialize(bool retryOnFail);
    void closeChannel(Channel* channel);
    void closeChannels();
    void close();
    std::string getName() const;
    std::vector<uint8_t> getAtr();
    void selectDefaultApplication();
    std::shared_ptr<Channel> openBasicChannel(ISecureElementSession* session, const std::vector<uint8_t>& aid, uint8_t p2, const std::shared_ptr<ISecureElementListener>& listener, const std::string& packageName, const std::vector<uint8_t>& uuid, int pid);
    std::shared_ptr<Channel> openLogicalChannel(ISecureElementSession* session, const std::vector<uint8_t>& aid, uint8_t p2, const std::shared_ptr<ISecureElementListener>& listener, const std::string& packageName, const std::vector<uint8_t>& uuid, int pid);
    bool isAidSelectable(const std::vector<uint8_t>& aid);
    std::vector<uint8_t> transmit(const std::vector<uint8_t>& cmd);
    bool isSecureElementPresent();
    bool reset();
    void dump(std::ostream& writer);
    std::shared_ptr<ISecureElementReader> newSecureElementReader(std::shared_ptr<SecureElementService> service);

    std::string getName();

private:
    void stateChange(bool state, const std::string& reason);
    void sendStateChangedBroadcast(bool state);
    void initializeAccessControl();
    // ChannelAccess setUpChannelAccess(const std::vector<uint8_t>& aid, const std::string& packageName, const std::vector<uint8_t>& uuid, int pid, bool isBasicChannel);
    bool isPrivilegedApplication(const std::string& packageName);
    void onClientDeath(void* cookie);
    static void onClientDeathWrapper(void* cookie);

    std::string mName;
    std::string mTag;
    std::map<int, std::shared_ptr<Channel>> mChannels;
    std::mutex mLock;
    bool mIsConnected;
    int mGetHalRetryCount = 0;
    // AccessControlEnforcer* mAccessControlEnforcer;
    std::shared_ptr<ISecureElement> mAidlHal;

    class AidlCallback : public BnSecureElementCallback {
        public:
            AidlCallback(Terminal* terminal);
            // Override from ISecureElementCallback
            ::ndk::ScopedAStatus onStateChange(bool state, const std::string& debugReason) override;
            // ::ndk::ScopedAStatus getInterfaceVersion(int& version);
            // ::ndk::ScopedAStatus getInterfaceHash(std::string& hash);
        private:
            Terminal* mTerminal;
        };

    bool mDefaultApplicationSelectedOnBasicChannel = true;
    
    const std::string SECURE_ELEMENT_PRIVILEGED_OPERATION_PERMISSION =
        "android.permission.SECURE_ELEMENT_PRIVILEGED_OPERATION";
    const bool DEBUG = true;

    const int GET_SERVICE_DELAY_MILLIS = 4 * 1000;
    const int EVENT_GET_HAL = 1;
    const int EVENT_NOTIFY_STATE_CHANGE = 2;

    AIBinder_DeathRecipient* mDeathRecipient;
    std::shared_ptr<AidlCallback> mAidlCallback;
    static void onBinderDiedCallback(void* cookie);

    void handler(int event, int msg, int delay);
    
    friend class SecureElementReader;
};
}  // namespace aidl::android::se