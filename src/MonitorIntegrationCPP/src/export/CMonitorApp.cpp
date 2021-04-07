//
// Created by felix lee on 2020/10/9.
//

#include <fstream>
#include "CMonitorApp.h"



mi::CMonitorApp::CMonitorApp() {

    int port;
    int monitor_out_flag;

    nlohmann::json json_obj;
    try {
        std::ifstream i("monitor/zabbix.json");
        if (!i) {
            std::cout << "monitor config file path error!" << std::endl;
            _exit(-1);
        }
        i >> json_obj;
        port = json_obj["health_check_port"].get<int>();
        monitor_out_flag = json_obj["monitor_out_flag"].get<int>();
    }
    catch(std::exception& e) {
        std::cout << "monitor config file parse error: " << e.what() << std::endl;
        _exit(-1);
    }


    monitor_ptr_.reset(Singleton<CMonitor>::Instance(monitor_out_flag == 1));

    auto  ins = Singleton<HttpServerChannel>::Instance(port);
    ins->Init();

}

void mi::CMonitorApp::RegisterIndicatorFunc(const char * key, const std::function<HealthDetail(const std::string& key)> &f) {
    auto instance = Singleton<CMonitor>::GetInstance();
    instance->RegisterIndicatorFunc(key, f);
}

void mi::CMonitorApp::MonitorOut(const char *issue, const char *desc, mi::MonitorSeverity sv) {
    monitor_ptr_->MonitorOut(issue, desc, sv);
}

int mi::CMonitorApp::RegisterHealthDetailItem(const char *key) {
    return monitor_ptr_->RegisterHealthDetailItem(key);
}

int mi::CMonitorApp::PostSuccessDetail(const char *key, const std::string &desc) {
    return monitor_ptr_->PostSuccessDetail(key, desc);
}

int mi::CMonitorApp::PostFailDetail(const char *key, const char *desc) {
    return monitor_ptr_->PostFailDetail(key, desc);
}
