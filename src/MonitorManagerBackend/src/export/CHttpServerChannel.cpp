//
// Created by felix on 2020/9/28.
//

#include "CHttpServerChannel.h"
#include <fstream>
#include <spdlog/spdlog.h>
#include "DBManager.h"
#include "CConfigManager.h"


HttpServerChannel::HttpServerChannel(int port) {
    port_ = port;

    auto configIns = Singleton<ConfigManager>::GetInstance();
    zabbix_enable_flag_ = configIns->LookupConfig<int>("zabbix_enable_flag", 0);

}






void HttpServerChannel::Start() {

    HttpServer server;

    server.config.port = port_;

    server.resource["^/api/qrylist$"]["POST"] = std::bind(&HttpServerChannel::QryHandler, this, std::placeholders::_1, std::placeholders::_2);

    server.resource["^/api/insert$"]["POST"] = std::bind(&HttpServerChannel::InsertHandler, this, std::placeholders::_1, std::placeholders::_2);

    server.resource["^/api/modify$"]["POST"] = std::bind(&HttpServerChannel::ModifyHandler, this, std::placeholders::_1, std::placeholders::_2);

    server.resource["^/api/delete$"]["POST"] = std::bind(&HttpServerChannel::DeleteHandler, this, std::placeholders::_1, std::placeholders::_2);


    server.on_error = [](std::shared_ptr<HttpServer::Request> /*request*/, const SimpleWeb::error_code & ec/*ec*/) {
        // Handle errors here
        // Note that connection timeouts will also call this handle with ec set to SimpleWeb::errc::operation_canceled
        if (ec)
            std::cout << "error: " << ec.message() << std::endl;
    };



    server.start([](unsigned short port) {
        std::cout << "Server listening on port " << port << std::endl;
    });

    std::cout << "after Server listening " << std::endl;

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



void HttpServerChannel::QryHandler(std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {

    try {

        auto j = Singleton<DB>::GetInstance()->QryDataList();
        nlohmann::json item;
        item["code"] = 200;
        item["data"] = j;
        item["op"] = "query";


        auto respstr = item.dump();
        response->close_connection_after_response = true;

// Set header fields
        SimpleWeb::CaseInsensitiveMultimap header;
        header.emplace("Content-Type", "text/plain");
        header.emplace("Access-Control-Allow-Origin", "*");
        header.emplace("Access-Control-Allow-Methods", "GET, POST, OPTIONS, PUT, DELETE");
        header.emplace("Access-Control-Max-Age", "1728000");
        header.emplace("Access-Control-Allow-Headers", "authorization,content-type");

        response->write(SimpleWeb::StatusCode::success_ok, respstr, header);


//        *response << "HTTP/1.1 200 OK\r\n"
//                  << "Content-Length: " << respstr.length() << "\r\n\r\n"
//                  << respstr;
    }
    catch(nlohmann::detail::exception &e) {
        *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n"
                  << e.what();
    }

}

void HttpServerChannel::InsertHandler(std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
    try {
        response->close_connection_after_response = true;

        auto req_j = nlohmann::json::parse(request->content);
        nlohmann::json ret_j;
        ret_j["data"] = req_j["data"];
        auto appname = req_j["data"]["appname"].get<std::string>();
        auto address = req_j["data"]["address"].get<std::string>();
        auto zabbixhost = req_j["data"]["zabbixhost"].get<std::string>();
        auto port = req_j["data"]["port"].get<int>();





        auto ret = Singleton<DB>::GetInstance()->InsertData(appname, address, port, zabbixhost);
        if (ret == SQLITE_OK) {
            ret_j["code"] = 200;
            ret_j["op"] = "insert";
        }
        else {
            ret_j["code"] = -1;
            ret_j["op"] = "insert";
        }

        if (zabbix_enable_flag_ == 1) {
            Singleton<ZabbixManager>::GetInstance()->AddMoniterItem(appname, address, port, zabbixhost);
        }
        else {
            SPDLOG_INFO("zabbix disabled, not to add zabbix item");
        }


// Set header fields
        SimpleWeb::CaseInsensitiveMultimap header;
        header.emplace("Content-Type", "text/plain");
        header.emplace("Access-Control-Allow-Origin", "*");
        header.emplace("Access-Control-Allow-Methods", "GET, POST, OPTIONS, PUT, DELETE");
        header.emplace("Access-Control-Max-Age", "1728000");
        header.emplace("Access-Control-Allow-Headers", "authorization,content-type");

        response->write(SimpleWeb::StatusCode::success_ok, ret_j.dump(), header);


//        *response << "HTTP/1.1 200 OK\r\n"
//                  << "Content-Length: " << respstr.length() << "\r\n\r\n"
//                  << respstr;

    }
    catch(nlohmann::detail::exception &e) {
        *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n"
                  << e.what();
    }

}

void HttpServerChannel::ModifyHandler(std::shared_ptr<HttpServer::Response> response,
                                      std::shared_ptr<HttpServer::Request> request) {


    try {
        response->close_connection_after_response = true;

        auto req_j = nlohmann::json::parse(request->content);
        nlohmann::json ret_j;
        ret_j["data"] = req_j["data"];
        auto appname = req_j["data"]["appname"].get<std::string>();
        auto address = req_j["data"]["address"].get<std::string>();
        auto port = req_j["data"]["port"].get<int>();
        auto zabbixhost = req_j["data"]["zabbixhost"].get<std::string>();

        auto ret = Singleton<DB>::GetInstance()->ModifyData(appname, address, port, zabbixhost);
        if (ret == SQLITE_OK) {
            ret_j["code"] = 200;
            ret_j["op"] = "modify";
        }
        else {
            ret_j["code"] = -1;
            ret_j["op"] = "modify";
        }
// Set header fields
        SimpleWeb::CaseInsensitiveMultimap header;
        header.emplace("Content-Type", "text/plain");
        header.emplace("Access-Control-Allow-Origin", "*");
        header.emplace("Access-Control-Allow-Methods", "GET, POST, OPTIONS, PUT, DELETE");
        header.emplace("Access-Control-Max-Age", "1728000");
        header.emplace("Access-Control-Allow-Headers", "authorization,content-type");

        response->write(SimpleWeb::StatusCode::success_ok, ret_j.dump(), header);


//        *response << "HTTP/1.1 200 OK\r\n"
//                  << "Content-Length: " << respstr.length() << "\r\n\r\n"
//                  << respstr;
    }
    catch(nlohmann::detail::exception &e) {
        *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n"
                  << e.what();
    }

}

void HttpServerChannel::DeleteHandler(std::shared_ptr<HttpServer::Response> response,
                                      std::shared_ptr<HttpServer::Request> request) {


    try {


        response->close_connection_after_response = true;

        auto req_j = nlohmann::json::parse(request->content);
        nlohmann::json ret_j;
        ret_j["data"] = req_j["data"];
        auto appname = req_j["data"]["appname"].get<std::string>();
        auto itemid = req_j["data"]["itemid"].get<std::string>();


        auto ret = Singleton<DB>::GetInstance()->DeleteData(appname);
        if (ret == SQLITE_OK) {
            ret_j["code"] = 200;
            ret_j["op"] = "delete";
        }
        else {
            ret_j["code"] = -1;
            ret_j["op"] = "delete";
        }
        Singleton<ZabbixManager>::GetInstance()->DeleteMonitorItem(itemid);

// Set header fields
        SimpleWeb::CaseInsensitiveMultimap header;
        header.emplace("Content-Type", "text/plain");
        header.emplace("Access-Control-Allow-Origin", "*");
        header.emplace("Access-Control-Allow-Methods", "GET, POST, OPTIONS, PUT, DELETE");
        header.emplace("Access-Control-Max-Age", "1728000");
        header.emplace("Access-Control-Allow-Headers", "authorization,content-type");

        response->write(SimpleWeb::StatusCode::success_ok, ret_j.dump(), header);


//        *response << "HTTP/1.1 200 OK\r\n"
//                  << "Content-Length: " << respstr.length() << "\r\n\r\n"
//                  << respstr;
    }
    catch(nlohmann::detail::exception &e) {
        *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n"
                  << e.what();
    }


}