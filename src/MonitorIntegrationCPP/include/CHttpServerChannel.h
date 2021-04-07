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
#include "CMonitor.h"
#include "singleton.h"


class HttpServerChannel {

public:

    using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
    HttpServerChannel(int port);

    ~HttpServerChannel();
    void Start();
    void Init();

    void JsonMsgHandler(std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request);

private:
    int port_;
    std::shared_ptr<std::thread> server_thread_ptr;
    std::atomic<bool> is_running_;
    HttpServer server;
};




#endif //MONITORINTEGRATION_CHTTPSERVERCHANNEL_H
