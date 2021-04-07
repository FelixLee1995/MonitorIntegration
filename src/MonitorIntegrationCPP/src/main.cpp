

#ifdef MYDLL_EXPORTS
#undef MYDLL_EXPORTS
#endif

#include <iostream>
#include <IMonitorApp.h>
#include <CConfigManager.h>



int main() {
    std::cout << "Hello World!" << std::endl;


    ConfigManager configManager("./monitor/zabbix.json");


    configManager.AddFieldCheck("/test/testkey", ConfigFieldType::TypeText);
    configManager.AddFieldCheck("/zabbix_port", ConfigFieldType::TypePort);
    configManager.AddFieldCheck("/zabbix_server", ConfigFieldType::TypeIPv4);

    configManager.CheckAllField();

    auto f =  [](const std::string& key) {
        mi::HealthDetail detail{};
        std::strcpy(detail.Key, key.c_str());
        std::strcpy(detail.Desc, "unknown key");
        detail.Level = mi::HealthSeverityLevel::NORMAL;
        detail.StatusCode = mi::HealthStatus::UNKNOWN;
        return detail;
    };

    char key[128] = "ws_client";
    mi::factory_func();
    mi::g_monitor_->RegisterIndicatorFunc(key, f);


    mi::g_monitor_->MonitorOut("ws disconnect", "websocketchannel disconnected!  please check", mi::MonitorSeverity::Fatal);

    mi::g_monitor_->RegisterHealthDetailItem("redis_db");
    mi::g_monitor_->RegisterHealthDetailItem("mysql_db");
    mi::g_monitor_->RegisterHealthDetailItem("mongodb");
    mi::g_monitor_->PostFailDetail("redis_db", "Failed to connect");
    mi::g_monitor_->PostSuccessDetail("mongodb");
    mi::g_monitor_->PostSuccessDetail("mysql_db", "connect success!");


    getchar();

    return 0;
}