#pragma once

#include <iostream>
#include <android-base/logging.h>

#include <aidl/android/se/omapi/ISecureElementService.h>
#include <aidl/android/se/omapi/BnSecureElementService.h>

namespace aidl::android::se::omapi {
class Omapi : public BnSecureElementService {
    public:
        virtual ::ndk::ScopedAStatus getReaders(std::vector<std::string>* _aidl_return);
        virtual ::ndk::ScopedAStatus getReader(const std::string& in_reader, std::shared_ptr<ISecureElementReader>* _aidl_return);
        virtual ::ndk::ScopedAStatus isNfcEventAllowed(const std::string& in_reader, const std::vector<uint8_t>& in_aid, const std::vector<std::string>& in_packageNames, int32_t in_userId, std::vector<bool>* _aidl_return);
};
}