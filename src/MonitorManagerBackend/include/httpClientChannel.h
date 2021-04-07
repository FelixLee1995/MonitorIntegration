//
// Created by lf729 on 2020/9/18.
//

#ifndef HTTPCLIENTCHANNEL_H
#define HTTPCLIENTCHANNEL_H


#include "simpleweb/client_http.hpp"
#include <future>

// Added for the json-example
#define BOOST_SPIRIT_THREADSAFE

// Added for the default_resource example
#include <algorithm>
#include <vector>
#include <nlohmann/json.hpp>
#include <map>
#include <functional>

using namespace std;
// Added for the json-example:

using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;


class HttpClientChannel{

public:

    static void AsyncPostMsgWithSpecHeaders(const std::string &path, const std::string &target,
                                            const std::string &msg, std::map<std::string, std::string> headers,
                                            std::function<void(std::string resp, const SimpleWeb::error_code)>);

    static void AsyncPostMsg(const std::string &path, const std::string &target,
                                            const std::string &msg,
                                            std::function<void(std::string resp, const SimpleWeb::error_code)>);


    static void AsyncPostMsgWithSpecHeadersAndUserData(const std::string &path, const std::string &target,
                                            const std::string &msg, std::map<std::string, std::string> headers,
                                            std::map<std::string, std::string> UserData,
                                                       std::function<void(std::string resp, std::map<std::string, std::string>, const SimpleWeb::error_code)>);



private:


};


#endif //HTTPCLIENTCHANNEL_H
