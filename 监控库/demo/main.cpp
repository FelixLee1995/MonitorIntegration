

#ifdef MYDLL_EXPORTS
#undef MYDLL_EXPORTS
#endif

#include <iostream>
#include <IMonitorApp.h>





int main() {
    std::cout << "Hello World!" << std::endl;

    auto f =  []() {
        mi::HealthDetail detail{};
        std::strcpy(detail.Key, "test");
        std::strcpy(detail.Desc, "unknown key");
        detail.Level = mi::HealthSeverityLevel::NORMAL;
        detail.StatusCode = mi::HealthStatus::UNKNOWN;
        return detail;
    };

    char key[128] = "ws_client";
    mi::factory_func(8067, true);
    mi::g_monitor_->RegisterIndicatorFunc(key, f);



    getchar();

    return 0;
}