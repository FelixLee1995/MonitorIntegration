//
// Created by felix on 2020/9/28.
//

#include "CHttpServerChannel.h"



HttpServerChannel::HttpServerChannel(int port): server() {
    port_ = port;
}






void HttpServerChannel::Start() {

    server.config.port = port_;



    server.resource["^/healthcheck$"]["POST"] = std::bind(&HttpServerChannel::JsonMsgHandler, this, std::placeholders::_1, std::placeholders::_2);


    server.on_error = [](std::shared_ptr<HttpServer::Request> /*request*/, const SimpleWeb::error_code & ec/*ec*/) {
        // Handle errors here
        // Note that connection timeouts will also call this handle with ec set to SimpleWeb::errc::operation_canceled
        if (ec)
            std::cout << "error: " << ec.message() << std::endl;
    };



    server.start([](unsigned short port) {
        std::cout << "Server listening on port " << port << std::endl;
    });


}

void HttpServerChannel::Init() {

    is_running_.store(true);
    server_thread_ptr = std::make_shared<std::thread>(&HttpServerChannel::Start, this);
}

HttpServerChannel::~HttpServerChannel() {
    is_running_.store(false);
    server_thread_ptr->join();
    server_thread_ptr.reset();
}

void HttpServerChannel::JsonMsgHandler(std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {

    // Set header fields
    SimpleWeb::CaseInsensitiveMultimap header;
    header.emplace("Content-Type", "text/plain");
    header.emplace("Access-Control-Allow-Origin", "*");
    header.emplace("Access-Control-Allow-Methods", "GET, POST, OPTIONS, PUT, DELETE");
    header.emplace("Access-Control-Max-Age", "1728000");
    header.emplace("Access-Control-Allow-Headers", "authorization,content-type");
    try {
        response->close_connection_after_response = true;




        auto j = nlohmann::json::parse(request->content);

        if (j.contains("func_id")&& j["func_id"].get<int>() == 10000) {

            auto ret = Singleton<mi::CMonitor>::GetInstance()->GetTotalHealthStatus();

            auto vec = std::get<1>(ret);
            for (int i = 0; i< vec.size(); ++ i) {
                nlohmann::json item;
                item["key"] = vec[i].Key;
                item["status_code"] = vec[i].StatusCode;
                item["desc"] = vec[i].Desc;
                item["level"] = vec[i].Level;
                j["data"]["Detail"].push_back(std::move(item));
            }

            j["data"]["Health"] = std::get<0>(ret);
            auto str = j.dump();
            response->write(SimpleWeb::StatusCode::success_ok, str, header);
            std::cout << "jsonstr is " << str << std::endl;
        }
    }
    catch(nlohmann::detail::exception &e) {

        response->write(SimpleWeb::StatusCode::client_error_bad_request, e.what(), header);

    }

}
