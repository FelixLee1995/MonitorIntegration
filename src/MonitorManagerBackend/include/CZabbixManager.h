//
// Created by felix lee on 2020/10/29.
//

#ifndef MONITORINTEGRATION_CZABBIXMANAGER_H
#define MONITORINTEGRATION_CZABBIXMANAGER_H
#include "httpClientChannel.h"
#include "fmt/format.h"
#include "DBManager.h"
#include <string>


struct ZabbixHost{
    std::string hostid;
    std::string hostname;
    std::string interfaceid;
    std::string itemid;
};


class ZabbixManager{


public:
    ZabbixManager(const std::string& user, const std::string& pwd, const std::string& url, const std::string& target)
    : userid_(user), pwd_(pwd), url_(url), target_(target)
    {

    }

    void Init(){
        map<std::string, string> headers;
        headers.insert(std::make_pair("Content-Type", "application/json-rpc"));
        nlohmann::json j;
        j["jsonrpc"] = "2.0";
        j["method"] = "user.login";
        j["params"]["user"] = userid_;
        j["params"]["password"] = pwd_;
        j["id"] = 1;

        HttpClientChannel::AsyncPostMsgWithSpecHeaders(url_, target_, j.dump(), headers, [&](const std::string& msg, const asio::error_code ec){
            if (ec) {
                std::cout << "get token error: " << ec.message() << std::endl;
                return;
            }
            else {

                try{

                    //todo ���� env ״̬
                    auto json_rsp = nlohmann::json::parse(msg);
                    token_ = json_rsp["result"].get<std::string>();
                    std::cout  <<  "recv  result: " << token_ <<std::endl;

                }
                catch (nlohmann::detail::exception& e) {
                    std::cout << "json_exception: " << e.what() << std::endl;
                }


            }
        });


        GetHostList();
        GetInterfaceList();
    }


    void GetInterfaceList() {

        map<std::string, string> headers;
        headers.insert(std::make_pair("Content-Type", "application/json-rpc"));
        nlohmann::json j;
        j["jsonrpc"] = "2.0";
        j["method"] = "hostinterface.get";
        j["params"]["filter"] = NULL;
        j["id"] = 1;
        j["auth"] = token_;

        HttpClientChannel::AsyncPostMsgWithSpecHeaders(url_, target_, j.dump(), headers, [&](const std::string& msg, const asio::error_code ec){
            if (ec) {
                std::cout << "get token error: " << ec.message() << std::endl;
                return;
            }
            else {

                try{
                    auto json_rsp = nlohmann::json::parse(msg);
                    auto iter = json_rsp["result"].begin();
                    for (;iter!=json_rsp["result"].end();iter++) {
                        auto interfaceid = (*iter)["interfaceid"].get<std::string>();
                        auto hostid = (*iter)["hostid"].get<std::string>();

                        auto it = host_map_.find(hostid);
                        if (it != host_map_.end()) {
                            auto &item =   host_map_[hostid];
                            item.interfaceid = interfaceid;
                            std::cout  <<  "interfaceid: " << interfaceid << " hostid: " << hostid <<std::endl;
                        }


                    }

                }
                catch (nlohmann::detail::exception& e) {
                    std::cout << "json_exception: " << e.what() << std::endl;
                }


            }
        });
    }

    void GetHostList() {

        map<std::string, string> headers;
        headers.insert(std::make_pair("Content-Type", "application/json-rpc"));
        nlohmann::json j;
        j["jsonrpc"] = "2.0";
        j["method"] = "host.get";
        j["params"]["filter"] = NULL;
        j["id"] = 1;
        j["auth"] = token_;

        HttpClientChannel::AsyncPostMsgWithSpecHeaders(url_, target_, j.dump(), headers, [&](const std::string& msg, const asio::error_code ec){
            if (ec) {
                std::cout << "get token error: " << ec.message() << std::endl;
                return;
            }
            else {

                try{
                    auto json_rsp = nlohmann::json::parse(msg);
                    auto iter = json_rsp["result"].begin();
                    for (;iter!=json_rsp["result"].end();iter++) {
                        auto hostname = (*iter)["host"].get<std::string>();
                        auto hostid = (*iter)["hostid"].get<std::string>();
                        ZabbixHost host;
                        host.hostid = hostid;
                        host.hostname = hostname;
                        host_map_.insert(std::make_pair(hostid, host));
                        std::cout  <<  "hostname: " << hostname << " hostid: " << hostid <<std::endl;

                    }

                }
                catch (nlohmann::detail::exception& e) {
                    std::cout << "json_exception: " << e.what() << std::endl;
                }


            }
        });



    }


    void HandleMonitorItemCreated(const std::string& msg, std::map<std::string, std::string> UserData, const asio::error_code ec) {

        if (!ec) {
            nlohmann::json j = nlohmann::json::parse(msg);
            if (j.contains("result")) {
                std::cout << "create item success " << std::endl;

                auto itemid = j["result"]["itemids"][0].get<std::string>();
                auto appname = UserData["appname"];

                std::cout << "todo  update  itemid " << itemid << std::endl;
                auto ret = Singleton<DB>::GetInstance()->UpdateMonitorItemByAppname(appname, itemid);
                if (ret == SQLITE_OK) {
                    std::cout << " update itemid success " << appname << std::endl;
                }
            }

        }



    }


    bool AddMoniterItem(std::string appname, std::string address, int port, const std::string &zabbixhost) {
        ZabbixHost host;
        bool found_host = false;
        for (auto iter = host_map_.begin();iter!=host_map_.end(); iter ++) {
            if (iter->second.hostname == zabbixhost) {
                host = iter->second;
                found_host = true;
                break;
            }
        }

        if (!found_host) {
            return false;
        }

        auto url = fmt::format("{}:{}/healthcheck", address, port);


        map<std::string, string> headers;
        headers.insert(std::make_pair("Content-Type", "application/json-rpc"));
        try {
            nlohmann::json j;
            j["jsonrpc"] = "2.0";
            j["method"] = "item.create";
            j["params"]["url"] = url;
            j["params"]["interfaceid"] = host.interfaceid;
            j["params"]["type"] = "19";
            j["params"]["request_method"] = "1";
            j["params"]["hostid"] = host.hostid;
            j["params"]["delay"] = "60s";
            j["params"]["key_"] = appname + "_healthcheck";
            j["params"]["name"] = appname + "_healthcheck";
            j["params"]["value_type"] = "4";
            j["params"]["name"] = appname + "_healthcheck";
            j["params"]["name"] = appname + "_healthcheck";
            j["params"]["post_type"] = "2";
            j["params"]["posts"] = R"({"func_id":10000, "req_id":1})";
            j["auth"] = token_;
            j["id"] = "1";

            std::map<std::string, std::string> userdata;
            userdata["appname"] = appname;

            HttpClientChannel::AsyncPostMsgWithSpecHeadersAndUserData(url_, target_, j.dump(), headers, userdata,
                                                                      std::bind(&ZabbixManager::HandleMonitorItemCreated, this, std::placeholders::_1,  std::placeholders::_2,  std::placeholders::_3));

        }
        catch(nlohmann::detail::exception &exception) {
            std::cout << "json exception: " << exception.what() << std::endl;
        }
        return true;
    }

    bool DeleteMonitorItem(const std::string& itemid){

        bool ret = false;

        map<std::string, string> headers;
        headers.insert(std::make_pair("Content-Type", "application/json-rpc"));
        nlohmann::json j;
        j["jsonrpc"] = "2.0";
        j["method"] = "item.delete";
        j["params"].push_back(itemid);
        j["id"] = 1;
        j["auth"] = token_;

        HttpClientChannel::AsyncPostMsgWithSpecHeaders(url_, target_, j.dump(), headers, [&](const std::string& msg, const asio::error_code ec){
            if (ec) {
                std::cout << "get token error: " << ec.message() << std::endl;
                return;
            }
            else {

                try{
                    auto json_rsp = nlohmann::json::parse(msg);
                    if (json_rsp.contains("result")) {
                        std::cout << "item delete success" << std::endl;
                        ret = true;
                    }
                }
                catch (nlohmann::detail::exception& e) {
                    std::cout << "json_exception: " << e.what() << std::endl;
                }


            }
        });
        std::cout << "delete res is " << ret << std::endl;
        return ret;
    }

private:
    std::string userid_;
    std::string pwd_;
    std::string url_;
    std::string target_;
    std::string token_;

    std::map<std::string, ZabbixHost> host_map_;
};

#endif //MONITORINTEGRATION_CZABBIXMANAGER_H
