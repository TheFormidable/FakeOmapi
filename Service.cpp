#include "Service.h"

namespace aidl::android::se::omapi {
    ndk::ScopedAStatus SecureElementService::getReaders(std::vector<std::string>* readers) {
            int callingUid = AIBinder_getCallingUid();
            LOG(INFO) << "getReaders() for uid: " << callingUid << std::endl;
            std::string packageName = getPackageNameFromCallingUid(callingUid);

            if (packageName != "") {
                LOG(INFO) << "getReaders() find package name for uid[" << callingUid << "]: " << packageName.c_str() << std::endl;

                std::lock_guard<std::mutex> lock(mTerminalsMutex);
                for (const auto& pair : mTerminals) {
                    readers->push_back(pair.first);
                }
            } else {
                LOG(INFO) << "Can't find package name using PackageManager, find eseReader instead" << std::endl;
                const std::string& prefix = ESE_TERMINAL;
                std::lock_guard<std::mutex> lock(mTerminalsMutex);

                // for (const auto& pair : mTerminals) {
                //     if (pair.first.find(prefix) == 0) {
                //         readers->push_back(pair.first);
                //     }
                // }
                readers->push_back("eSE1");
        }
        return ndk::ScopedAStatus::ok();
    };

    ndk::ScopedAStatus SecureElementService::getReader(const std::string& in_reader,
                                                        std::shared_ptr<ISecureElementReader>* out_reader) {
        LOG(INFO) << __func__;
        std::cout << "getReader for " << in_reader.c_str() << std::endl;
        // for (const auto& pair : mTerminals) {
        //     if (pair.first.compare(in_reader) == 0) {
        //         out_reader = pair.second;
        //         return ndk::ScopedAStatus::ok();
        //     }
        // }
        return ndk::ScopedAStatus::ok();
    };
    ndk::ScopedAStatus SecureElementService::isNfcEventAllowed(const std::string& reader,
                                            const std::vector<uint8_t>& aid,
                                            const std::vector<std::string>& packageNames,
                                            int32_t userId,
                                            std::vector<bool>* isAllowed) {
        LOG(INFO) << __func__;
        std::cout << "isNfcEventAllowed" << std::endl;
        // TODO: implement isNfcEventAllowed, return false for now
        isAllowed->resize(packageNames.size(), false);
        return ndk::ScopedAStatus::ok();
    };
}