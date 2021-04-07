//
// Created by felix lee on 2020/10/9.
//

#ifndef MONITORINTEGRATION_CMONITORAPP_H
#define MONITORINTEGRATION_CMONITORAPP_H
#include "CMonitor.h"
#include "CHttpServerChannel.h"
#include "IMonitorApp.h"
#include "singleton.h"
#include "ds.h"
namespace mi {

    class CMonitorApp: public IMonitorApp {

    public:
        CMonitorApp();

        void RegisterIndicatorFunc(const char * key, const std::function<HealthDetail(const std::string& key)> &f) override;

        void MonitorOut(const char * issue, const char* desc, MonitorSeverity ) override;

        int RegisterHealthDetailItem(const char *key) override;

        int PostSuccessDetail(const char *key, const std::string & desc) override;

        int PostFailDetail(const char *key, const char * desc) override;

    private:

        std::shared_ptr<CMonitor> monitor_ptr_;

    };

}
#endif //MONITORINTEGRATION_CMONITORAPP_H
