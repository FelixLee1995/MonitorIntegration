//
// Created by felix lee on 2020/10/9.
//

#include "IMonitorApp.h"
#include "CMonitorApp.h"
#include "singleton.h"
#include "fmt/chrono.h"

mi::IMonitorApp * mi::g_monitor_ = nullptr;


mi::IMonitorApp* mi::factory_func() {

    g_monitor_ = Singleton<mi::CMonitorApp>::Instance();
    return g_monitor_;

}
