//
// Created by felix lee on 2020/10/9.
//

#ifndef MONITORINTEGRATION_CMONITOR_H
#define MONITORINTEGRATION_CMONITOR_H

#include <functional>
#include <map>
#include <mutex>
#include <iostream>
#include <tuple>
#include <vector>
#include "CHttpServerChannel.h"
#include "ds.h"

namespace mi{



    class CMonitor{

    public:

        int RegisterHealthDetailItem(const char *key);

        int PostSuccessDetail(const char *key, const std::string & desc = "STATUS SUCCESS");

        int PostFailDetail(const char *key, const char * desc);

        int RegisterIndicatorFunc(const char * key, const std::function<HealthDetail(const std::string& key)>&);

        std::tuple<int, std::vector<HealthDetail>> GetTotalHealthStatus();

        void MonitorOut(const char * issue, const char* desc, MonitorSeverity severity);

        explicit CMonitor(bool if_base_check = false);


    private:


        HealthDetail GetHealthDetail(const std::string & key);


    private:
        bool if_base_check_;


        std::mutex indicator_map_mutex;
        std::map<std::string, std::function<HealthDetail(const std::string& key)>> indicator_map_;

        std::mutex health_detail_map_mutex;
        std::map<std::string, mi::HealthDetail> healthDetailMap_;


    };

}








#endif //MONITORINTEGRATION_CMONITOR_H
