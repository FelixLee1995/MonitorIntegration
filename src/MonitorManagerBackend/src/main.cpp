

#ifdef MYDLL_EXPORTS
#undef MYDLL_EXPORTS
#endif

#include <iostream>

#include "baseheader.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>


#include <CConfigManager.h>
#include "CHttpServerChannel.h"
#include "DBManager.h"
#include "CZabbixManager.h"


std::atomic<bool> g_is_working_;
void setLogger() {
    auto logger = spdlog::daily_logger_mt("daily_logger", "logs/system.log");
    logger->set_level(spdlog::level::debug);

    logger->flush_on(spdlog::level::info);

    auto console_log_ptr = spdlog::stdout_color_mt("console");

    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%f] [%t] [%l] [%s:%#(%!)] %v");

    spdlog::sinks_init_list sink_list = {logger->sinks().front(), console_log_ptr->sinks().front()};

    auto mylogger_ = std::make_shared<spdlog::logger>("my_logger", sink_list);


    spdlog::flush_every(std::chrono::seconds(1));

    spdlog::set_default_logger(mylogger_);

    SPDLOG_INFO("Server Start!");
}



int main() {

    g_is_working_ = true;
    #ifdef unix
    SetCoredump();
    #endif


    std::cout << "Hello World!" << std::endl;


    setLogger();
    auto configIns = Singleton<ConfigManager>::Instance("./config.json");

    auto port = configIns->LookupConfig<int>("api_port", 8001);
    auto check_port = configIns->LookupConfig<int>("healthcheck_port", 8002);


    auto zabbixManagerIns = Singleton<ZabbixManager>::Instance
    (configIns->LookupConfig<std::string>("zabbix_user", "Admin"),
     configIns->LookupConfig<std::string>("zabbix_pwd", "zabbix"),
     configIns->LookupConfig<std::string>("zabbix_server", "http://127.0.0.1"),
     configIns->LookupConfig<std::string>("zabbix_rpc_target", "/api-rpc.php")
     );

    zabbixManagerIns->Init();

    Singleton<DB>::Instance("./data.db");

    // mi::factory_func();

   auto httpServerIns = Singleton<HttpServerChannel>::Instance(port);
   httpServerIns->Init();


    while(g_is_working_.load()) {
        CommonSleep(3000);
    }

    return 0;
}