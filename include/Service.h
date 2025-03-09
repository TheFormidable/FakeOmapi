#pragma once

#include <map>

#include <iostream>
#include <android-base/logging.h>

#include <android/binder_ibinder.h>

#include <aidl/android/se/omapi/BnSecureElementService.h>

// #include "Terminal.h"

namespace aidl::android::se::omapi {
class SecureElementService : public BnSecureElementService {
    public:
        // SecureElementService();
        // virtual ~SecureElementService();
        virtual ndk::ScopedAStatus getReaders(std::vector<std::string>* readers);
        virtual ndk::ScopedAStatus getReader(const std::string& in_reader, std::shared_ptr<ISecureElementReader>* out_reader);
        virtual ndk::ScopedAStatus isNfcEventAllowed(const std::string& in_reader,
                                                    const std::vector<uint8_t>& in_aid,
                                                    const std::vector<std::string>& in_packageNames,
                                                    int32_t in_userId,
                                                    std::vector<bool>* isAllowed);
    
    private:
        const std::string ESE_TERMINAL  = "eSE";
        std::mutex mTerminalsMutex;
        // std::map<std::string, sp<Terminal>> mTerminals;
        std::map<std::string, std::string> mTerminals;
        int mActiveSimCount = 0;
        std::string getPackageNameFromCallingUid(int uid) {
            // Runing in native environment without PackageManager, return empty string straightly
            return "";
        }

        // sp<Terminal> getTerminal(const std::string& reader);
        
    };
}