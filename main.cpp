#include "omapi.h"
#include "Service.h"

#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

using aidl::android::se::omapi::Omapi;

int main() {
    ABinderProcess_setThreadPoolMaxThreadCount(0);
    std::shared_ptr<Omapi> hal = ndk::SharedRefBase::make<Omapi>();

    const std::string instance = std::string(Omapi::descriptor) + "/default";
    auto status = AServiceManager_registerLazyService(hal->asBinder().get(), instance.c_str());
    CHECK_EQ(status, STATUS_OK) << "Failed to add service " << instance << " " << status;
    LOG(INFO) << "Omapi AIDL service running...";
    ABinderProcess_joinThreadPool();
    return EXIT_FAILURE;  // should not reach
}
