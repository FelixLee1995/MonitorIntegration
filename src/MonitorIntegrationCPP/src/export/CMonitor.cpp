//
// Created by felix lee on 2020/10/9.
//

#include <fmt/chrono.h>
#include "CMonitor.h"
#include "ZbxSender.h"

int mi::CMonitor::RegisterIndicatorFunc(const char *key, const std::function<HealthDetail(const std::string&)> &f) {

    std::lock_guard<std::mutex> guard(indicator_map_mutex);

    if (indicator_map_.count(key) > 0) {
        std::cout << "Indicator key already exists: " << key << std::endl;
        return MI_FAILURE;
    }
    std::string indicator_key = key;
    indicator_map_.insert({indicator_key, f});
    return MI_OK;
}


std::tuple<int, std::vector<mi::HealthDetail>> mi::CMonitor::GetTotalHealthStatus() {

    std::vector<mi::HealthDetail> details;
    int status_code = HealthStatus::UP;

    {
        std::lock_guard<std::mutex> guard(indicator_map_mutex);
        auto iter = indicator_map_.begin();
        for (;iter!= indicator_map_.end(); iter++) {

            auto indicator_f =  iter->second;
            mi::HealthDetail detail = indicator_f(iter->first);
            status_code = detail.StatusCode > status_code ? detail.StatusCode : status_code;
            details.push_back(detail);
        }
    }

    {
        std::lock_guard<std::mutex> guard(health_detail_map_mutex);

        for (auto & iter : healthDetailMap_) {
            details.push_back(iter.second);
            status_code = iter.second.StatusCode > status_code ? iter.second.StatusCode : status_code;
        }

    }


    return std::make_tuple(status_code, std::move(details));
}

mi::HealthDetail mi::CMonitor::GetHealthDetail(const std::string &key) {

    std::lock_guard<std::mutex> guard(indicator_map_mutex);

    auto iter = indicator_map_.find(key);

    if (iter!=indicator_map_.end()) {
        auto indicator_f =  iter->second;
        return indicator_f(iter->first);
    }

    mi::HealthDetail detail{};
    std::strcpy(detail.Key, key.c_str());
    std::strcpy(detail.Desc, "unknown key");
    detail.Level = HealthSeverityLevel::NORMAL;
    detail.StatusCode = HealthStatus::UNKNOWN;

    return detail;
}

mi::CMonitor::CMonitor(bool if_base_check): indicator_map_mutex() {

    if_base_check_ = if_base_check;


    if (if_base_check_) {
        Singleton<ZbxSender>::Instance("monitor/zabbix.json");
    }

}

void mi::CMonitor::MonitorOut(const char *issue, const char *desc, mi::MonitorSeverity severity) {

    std::string sev;

    switch (severity) {
        case MonitorSeverity::Normal: sev = "Normal"; break;
        case MonitorSeverity::Error: sev = "Error"; break;
        case MonitorSeverity::Fatal: sev = "Fatal"; break;
        default: sev = "Unknown";
    }
    std::time_t t = std::time(nullptr);

    auto format_str = fmt::format("{:%Y-%m-%d %T}@{}@{}@{}@MONITORFLAG", fmt::localtime(t), issue, desc, sev);

    std::cout << format_str << std::endl;

    if (if_base_check_) {
        Singleton<ZbxSender>::GetInstance()->send(format_str);
    }


}

int mi::CMonitor::RegisterHealthDetailItem(const char *key) {

    std::lock_guard<std::mutex> guard(health_detail_map_mutex);

    if (healthDetailMap_.count(key) > 0) {
        std::cout << "healthDetail already exists" << std::endl;
        return MI_FAILURE;
    }

    mi::HealthDetail detail;
    strncpy(detail.Key, key, sizeof(detail.Key));
    strncpy(detail.Desc, "STATUS_INIT", sizeof(detail.Desc));
    detail.StatusCode = mi::HealthStatus::DOWN;
    detail.Level = mi::HealthSeverityLevel::FATAL;

    healthDetailMap_[key] = detail;

    return MI_OK;
}

int mi::CMonitor::PostSuccessDetail(const char *key, const std::string & desc) {
    std::lock_guard<std::mutex> guard(health_detail_map_mutex);


    auto iter = healthDetailMap_.find(key);
    if (iter == healthDetailMap_.end()) {
        return MI_FAILURE;
    }

    strncpy(iter->second.Desc, desc.c_str(), sizeof(iter->second.Desc));
    iter->second.Level = mi::HealthSeverityLevel::NORMAL;
    iter->second.StatusCode = mi::HealthStatus::UP;
    return MI_OK;
}

int mi::CMonitor::PostFailDetail(const char *key, const char *desc) {
    std::lock_guard<std::mutex> guard(health_detail_map_mutex);


    auto iter = healthDetailMap_.find(key);
    if (iter == healthDetailMap_.end()) {
        return MI_FAILURE;
    }

    strncpy(iter->second.Desc, desc, sizeof(iter->second.Desc));
    iter->second.Level = mi::HealthSeverityLevel::FATAL;
    iter->second.StatusCode = mi::HealthStatus::DOWN;
    return MI_OK;
}
