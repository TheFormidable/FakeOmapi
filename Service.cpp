#include "Service.h"

namespace aidl::android::se::omapi {
    ::ndk::ScopedAStatus Omapi::getReaders(std::vector<std::string>* _aidl_return) {
        LOG(INFO) << __func__;
        std::cout << "getReaders";
        return ndk::ScopedAStatus::ok();
    };
    ::ndk::ScopedAStatus Omapi::getReader(const std::string& in_reader, std::shared_ptr<ISecureElementReader>* _aidl_return) {
        LOG(INFO) << __func__;
        std::cout << "getReader";
        return ndk::ScopedAStatus::ok();
    };
    ::ndk::ScopedAStatus Omapi::isNfcEventAllowed(const std::string& in_reader, const std::vector<uint8_t>& in_aid, const std::vector<std::string>& in_packageNames, int32_t in_userId, std::vector<bool>* _aidl_return) {
        LOG(INFO) << __func__;
        std::cout << "isNfcEventAllowed";
        return ndk::ScopedAStatus::ok();
    };
}