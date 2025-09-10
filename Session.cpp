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

    // Lista de UUIDs candidatos (sin depender de Android.bp)
    mUuidCandidates = {
        hexStringToBytes("109900004540b96e0ea88b62d18ad3fb"),
        hexStringToBytes("30a900007fec9ed7d02dfa85bc499426"),
        hexStringToBytes("60a90000fbd0f332ddeb9af7115ab329")
    };
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

    Terminal& terminal = mReader->getTerminal();
    std::shared_ptr<Channel> channel = nullptr;

    // Probar cada UUID candidato
    for (auto &uuid : mUuidCandidates) {
        channel = terminal.openBasicChannel(this, aid, p2, listener, "" /* package name */, uuid, AIBinder_getCallingPid());
        if (channel != nullptr) {
            LOG(INFO) << "OpenBasicChannel success with UUID " << bytesToHex(uuid);
            break;
        }
    }

    if (channel == nullptr) {
        LOG(ERROR) << "OpenBasicChannel() failed with all UUIDs";
        *outChannel = nullptr;
        return ::ndk::ScopedAStatus::ok();
    }

    std::lock_guard<std::mutex> lock(mLock);
    mChannels.push_back(channel.get());
    auto sChannel = channel;
    *outChannel = ndk::SharedRefBase::make<SecureElementChannel>(sChannel);
    return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus SecureElementSession::openLogicalChannel(const std::vector<uint8_t>& aid, int8_t p2,
    const std::shared_ptr<ISecureElementListener>& listener, std::shared_ptr<ISecureElementChannel>* outChannel) {
    LOG(INFO) << __func__ << " AID = " << hex2string(aid) << ", P2 = " << p2;
    if(mIsClosed) {
        LOG(ERROR) << __func__ << ": Session is closed!";
        *outChannel = nullptr;
        return ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
    } else if(listener == nullptr) {
        LOG(ERROR) << __func__ << ": listener is null!";
        *outChannel = nullptr;
        return ::ndk::ScopedAStatus::fromExceptionCode(EX_NULL_POINTER);
    } else if ((p2 != 0x00) && (p2 != 0x04) && (p2 != 0x08) && (p2 != 0x0C)) {
        LOG(ERROR) << __func__ << ": Unsupported p2 operation: " << (p2 & 0xFF);
        *outChannel = nullptr;
        return ::ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
    }

    Terminal& terminal = mReader->getTerminal();
    std::shared_ptr<Channel> channel = nullptr;

    // Probar cada UUID candidato
    for (auto &uuid : mUuidCandidates) {
        channel = terminal.openLogicalChannel(this, aid, p2, listener, "" /* package name */, uuid, AIBinder_getCallingPid());
        if (channel != nullptr) {
            LOG(INFO) << "openLogicalChannel success with UUID " << bytesToHex(uuid);
            break;
        }
    }

    if(channel == nullptr) {
        LOG(ERROR) << __func__ << ": openLogicalChannel() failed with all UUIDs";
        *outChannel = nullptr;
        return ::ndk::ScopedAStatus::fromServiceSpecificErrorWithMessage(
                -1, "Failed to openLogicalChannel with all UUIDs");
    }

    std::lock_guard<std::mutex> lock(mLock);
    mChannels.push_back(channel.get());
    auto sChannel = channel;
    *outChannel = ndk::SharedRefBase::make<SecureElementChannel>(sChannel);
    return ::ndk::ScopedAStatus::ok();
}

} // namespace aidl::android::se::omapi

