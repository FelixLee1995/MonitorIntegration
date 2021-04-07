//
// Created by felix lee on 2020/10/9.
//

#ifndef MONITORINTEGRATION_CHTTPSERVERCHANNEL_H
#define MONITORINTEGRATION_CHTTPSERVERCHANNEL_H


#include <thread>
#include <memory>
#include <atomic>
#include "simpleweb/server_http.hpp"
#include "nlohmann/json.hpp"
#include "monitor/IMonitorApp.h"
#include "singleton.h"
#include "CZabbixManager.h"

class HttpServerChannel {

public:

    using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
    HttpServerChannel(int port);

    ~HttpServerChannel();
    void Start();
    void Init();

    //void JsonMsgHandler(std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request);
    void QryHandler(std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request);
    void InsertHandler(std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request);
    void ModifyHandler(std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request);
    void DeleteHandler(std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request);

    std::string AddHttpCheckItem();

private:
    int port_;
    int zabbix_enable_flag_;
    std::shared_ptr<std::thread> server_thread_ptr;
    std::atomic<bool> is_running_;

};

#endif //MONITORINTEGRATION_CHTTPSERVERCHANNEL_H
