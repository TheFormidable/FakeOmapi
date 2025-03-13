#include "Service.h"
#include "Terminal.h"

namespace aidl::android::se::omapi {
    SecureElementService::SecureElementService() {
        createTerminals();
    }

    ndk::ScopedAStatus SecureElementService::getReaders(std::vector<std::string>* readers) {
            int callingUid = AIBinder_getCallingUid();
            LOG(INFO) << "getReaders() for uid: " << callingUid;
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

                for (const auto& pair : mTerminals) {
                    if (pair.first.find(prefix) == 0) {
                        readers->push_back(pair.first);
                        LOG(INFO) << "Find eseReader: " << pair.first;
                    }
                }
        }
        return ndk::ScopedAStatus::ok();
    };

    ndk::ScopedAStatus SecureElementService::getReader(const std::string& readerName,
                                                        std::shared_ptr<ISecureElementReader>* readerObj) {
        LOG(INFO) << __func__;
        LOG(INFO) << "getReader for " << readerName.c_str() << std::endl;
        for (const auto& pair : mTerminals) {
            if (pair.first.compare(readerName) == 0) {
                LOG(INFO) << "Find reader for: " << readerName.c_str() << std::endl;
                ::android::sp<Terminal> terminal = getTerminal(readerName);
                if (terminal != nullptr) {
                    LOG(INFO) << __func__ << ": Getting reader";
                    *readerObj = terminal->newSecureElementReader(ndk::SharedRefBase::make<SecureElementService>());
                }
                return ndk::ScopedAStatus::ok();
            }
        }
        return ndk::ScopedAStatus::ok();
    }

    ndk::ScopedAStatus SecureElementService::isNfcEventAllowed(const std::string& readerName,
                                            const std::vector<uint8_t>& aid,
                                            const std::vector<std::string>& packageNames,
                                            int32_t userId,
                                            std::vector<bool>* isAllowed) {
        LOG(INFO) << __func__;
        std::cout << "isNfcEventAllowed" << std::endl;
        // TODO: implement isNfcEventAllowed, return false for now
        for (auto const packageName : packageNames) {
            isAllowed->push_back(false);
        }
        return ndk::ScopedAStatus::ok();
    }

    void SecureElementService::createTerminals() {
        addTerminals(SecureElementService::ESE_TERMINAL);
        /* We don't use sim card in native environment */
        // addTerminals(UICC_TERMINAL);
    }

    void SecureElementService::addTerminals(std::string terminalName) {
        int index = 1;
        if (terminalName.find(SecureElementService::UICC_TERMINAL) == 0) {
            index += mActiveSimCount + 1;
        }
        /* Ignore add for UICC */
        const std::string name = terminalName + std::to_string(index);
        ::android::sp<Terminal> terminal = new Terminal(name);
        terminal->initialize(index == 1);
        mTerminals.insert({name, terminal});
    }

    ::android::sp<Terminal> SecureElementService::getTerminal(const std::string& terminalName) {
        LOG(INFO) << __func__;
        auto it = mTerminals.find(terminalName);
        if (it != mTerminals.end()) {
            LOG(INFO) << __func__ << "Find terminal for " << terminalName;
            return it->second; // Valid: 'it->second' is the value
        }
        return nullptr;
    }
}