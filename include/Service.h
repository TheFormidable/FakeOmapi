#pragma once

#include <map>

#include <iostream>
#include <android-base/logging.h>

#include <android/binder_ibinder.h>

#include <aidl/android/se/omapi/BnSecureElementService.h>

#include "Terminal.h"
// #include "Reader.h"

namespace aidl::android::se::omapi {
class SecureElementService : public BnSecureElementService {
    public:
        SecureElementService();
        virtual ~SecureElementService() = default;
        virtual ndk::ScopedAStatus getReaders(std::vector<std::string>* readers);
        virtual ndk::ScopedAStatus getReader(const std::string& readerName,
                                                                    std::shared_ptr<ISecureElementReader>* readerObj);
        virtual ndk::ScopedAStatus isNfcEventAllowed(const std::string& readerName,
                                                    const std::vector<uint8_t>& aid,
                                                    const std::vector<std::string>& packageNames,
                                                    int32_t userId,
                                                    std::vector<bool>* isAllowed);
    
    private:
        const std::string ESE_TERMINAL  = "eSE";
        std::mutex mTerminalsMutex;
        // std::map<std::string, sp<Terminal>> mTerminals;
        std::map<std::string, std::string> mTerminals;
        // int mActiveSimCount = 0;
        std::string getPackageNameFromCallingUid(int uid) {
            // Runing in native environment without PackageManager, return empty string straightly
            return "";
        }

        std::shared_ptr<Terminal> getTerminal(const std::string& readerName);
        
    };
}