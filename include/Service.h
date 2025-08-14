#pragma once

#include <map>

#include <iostream>
#include <android-base/logging.h>

#include <android/binder_ibinder.h>

#include <aidl/android/se/omapi/BnSecureElementService.h>
#include <aidl/android/se/omapi/ISecureElementReader.h>

#include <utils/StrongPointer.h>

using aidl::android::se::omapi::ISecureElementReader;

namespace aidl::android::se {
class Terminal;
}

namespace aidl::android::se::omapi {
using aidl::android::se::Terminal;

class SecureElementService : public BnSecureElementService {
    public:
        inline static const std::string UICC_TERMINAL = "SIM";
        inline static const std::string ESE_TERMINAL = "eSE";

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
        std::mutex mTerminalsMutex;
        std::map<const std::string, ::android::sp<Terminal>> mTerminals;
        int mActiveSimCount = 0;
        std::string getPackageNameFromCallingUid(int /*uid*/) {
            // Runing in native environment without PackageManager, return empty string straightly
            return "";
        }

        void createTerminals();
        void addTerminals(const std::string terminalName);
    
        ::android::sp<Terminal> getTerminal(const std::string& readerName);
    };
}