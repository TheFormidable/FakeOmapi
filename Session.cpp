#include "Session.h"
#include "Channel.h"
#include "Reader.h"

#include "ByteArrayConverter.h"

namespace aidl::android::se::omapi {
using aidl::android::se::SecureElementReader;

SecureElementSession::SecureElementSession(SecureElementReader* reader) {
    mReader = std::shared_ptr<SecureElementReader>(reader);
    mAtr = mReader->getAtr();
    mIsClosed = false;
}

::ndk::ScopedAStatus SecureElementSession::getReader(std::shared_ptr<ISecureElementReader>* outReader) {
    *outReader = std::static_pointer_cast<ISecureElementReader>(mReader);
    return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus SecureElementSession::getAtr(std::vector<uint8_t>* outAtr) {
    *outAtr = mAtr;
    return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus SecureElementSession::close() {
    LOG(INFO) << __func__;
    closeChannels();
    mReader->removeSession(this);
    std::lock_guard<std::mutex> lock(mLock);
    mIsClosed = true;
    return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus SecureElementSession::removeChannel(Channel* channel) {
    LOG(INFO) << __func__;
    std::lock_guard<std::mutex> lock(mLock);
    mChannels.erase(
        std::remove(mChannels.begin(), mChannels.end(), channel),
        mChannels.end()
    );
    LOG(INFO) << "Removed channel: " << channel->getChannelNumber();
    return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus SecureElementSession::closeChannels() {
    LOG(INFO) << __func__;
    std::lock_guard<std::mutex> lock(mLock);
    for (auto channel : mChannels) {
        channel->close();
        LOG(INFO) << "Closed channel: " << channel->getChannelNumber();
    }
    return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus SecureElementSession::isClosed(bool* isClosed) {
    LOG(INFO) << __func__;
    std::lock_guard<std::mutex> lock(mLock);
    *isClosed = mIsClosed;
    return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus SecureElementSession::openBasicChannel(const std::vector<uint8_t>& aid, int8_t p2,
    const std::shared_ptr<ISecureElementListener>& listener, std::shared_ptr<ISecureElementChannel>* outChannel) {
        LOG(INFO) << __func__ << " AID = " << p2;
        if(mIsClosed) {
            LOG(ERROR) << "Session is closed";
        } else if (listener == nullptr) {
            LOG(ERROR) << "Listener must not be null";
        } else if ((p2 != 0x00) && (p2 != 0x04) && (p2 != 0x08) && (p2 != 0x0C)) {
            LOG(ERROR) << "p2 not supported: " << (p2 & 0xFF);
        }
        std::string packageName;
        // const std::vector<uint8_t>& uuid
        /* Ignore getting package name */
        LOG(INFO) << "openBasicChannel() trying to find mapping uuid";
        /* hardcode uuid, refer to /vendor/etc/hal_uuid_map.xml */

        /* Skip judging of equal of mReader and ESE_TERMINAL */
        // int uid = AIBinder_getCallingUid();
        Terminal& terminal = mReader->getTerminal();
        Channel* channel = terminal.openBasicChannel(this, aid, p2, listener,
                                                                    "" /* package name */, mUuid, AIBinder_getCallingPid());
        if (channel == nullptr) {
            LOG(ERROR) << "OpenBasicChannel() - returning null";
        }
        LOG(INFO) << "Open basic channel success. Channel: " << channel->getChannelNumber();
        
        std::lock_guard<std::mutex> lock(mLock);
        mChannels.push_back(channel);
        auto sChannel = std::shared_ptr<Channel>(channel); 
        *outChannel = ndk::SharedRefBase::make<SecureElementChannel>(sChannel);
        return ::ndk::ScopedAStatus::ok();
    }

    ::ndk::ScopedAStatus SecureElementSession::openLogicalChannel(const std::vector<uint8_t>& aid, int8_t p2,
        const std::shared_ptr<ISecureElementListener>& listener, std::shared_ptr<ISecureElementChannel>* outChannel) {
        LOG(INFO) << __func__ << " AID = " << hex2string(aid) << ", P2 = " << p2;
        if(mIsClosed) {
            LOG(ERROR) << __func__ << ": Session is closed!";
        } else if(listener == nullptr) {
            LOG(ERROR) << __func__ << ": listener is null!";
        } else if ((p2 != 0x00) && (p2 != 0x04) && (p2 != 0x08) && (p2 != 0x0C)) {
            LOG(ERROR) << __func__ << ": Unsupported p2 operation: " << (p2 & 0xFF);
        }
        std::string packageName = ""; /* getPackageNameFromCallingUid, empty on native env */

        // if(mReader->getTerminal().getName().starts_with(SecureElementService::ESE_TERMINAL)) {
            // getUUID logic, hardcode it
        // }

        Terminal& terminal = mReader->getTerminal();
        Channel* channel = terminal.openLogicalChannel(this, aid, p2, listener, packageName, mUuid, AIBinder_getCallingPid());

        if(channel == nullptr) {
            LOG(ERROR) << __func__ << ": openLogicalChannel() - returning null";
            return ::ndk::ScopedAStatus::ok();
        }

        LOG(INFO) << __func__ << ": openLogicalChannel() Success. Channel: " << channel->getChannelNumber();

        std::lock_guard<std::mutex> lock(mLock);
        mChannels.push_back(channel);

        auto sChannel = std::shared_ptr<Channel>(channel); 
        *outChannel = ndk::SharedRefBase::make<SecureElementChannel>(sChannel);
        return ::ndk::ScopedAStatus::ok();
    }

}