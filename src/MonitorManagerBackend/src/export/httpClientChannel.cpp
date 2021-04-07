//
// Created by lf729 on 2020/9/18.
//
#include "httpClientChannel.h"




void HttpClientChannel::AsyncPostMsgWithSpecHeaders(const std::string& path, const std::string & target,const std::string& msg, std::map<std::string, std::string> headers,
                                                    std::function<void(std::string, const SimpleWeb::error_code)> cb_) {

    // Asynchronous request example
    {
        HttpClient client(path);
        SimpleWeb::CaseInsensitiveMultimap header;
        header.insert(headers.begin(), headers.end());

        client.request("POST", target, msg, header, [cb_](shared_ptr<HttpClient::Response> response, const SimpleWeb::error_code &ec) {
            if (!ec) {
                //cout << "Response content: " << response->content.rdbuf() << endl;
                cb_(std::move(response->content.string()), ec);
            } else {
                cout << "error is " << ec.message() << endl;
                cb_(response->content.string(), ec);
            }

        });
        client.io_service->run();
    }

}


void HttpClientChannel::AsyncPostMsgWithSpecHeadersAndUserData(const std::string &path, const std::string &target,
                                                               const std::string &msg,
                                                               std::map<std::string, std::string> headers,
                                                               std::map<std::string, std::string> UserData,
                                                               std::function<void(std::string, std::map<std::string, std::string>,
                                                                                  const SimpleWeb::error_code)> cb_) {

    // Asynchronous request example
        HttpClient client(path);
        SimpleWeb::CaseInsensitiveMultimap header;
        header.insert(headers.begin(), headers.end());

        client.request("POST", target, msg, header, [&, UserData](shared_ptr<HttpClient::Response> response, const SimpleWeb::error_code &ec) {
            if (!ec) {
                //cout << "Response content: " << response->content.rdbuf() << endl;
                cb_(std::move(response->content.string()), UserData, ec);
            } else {
                cb_(response->content.string(), UserData, ec);
            }
        });

    client.io_service->run();

}


void HttpClientChannel::AsyncPostMsg(const std::string& path, const std::string & target,const std::string& msg,
                                                    std::function<void(std::string, const SimpleWeb::error_code)> cb_) {

    // Asynchronous request example
    {
        HttpClient client(path);

        client.request("POST", target, msg, [cb_](shared_ptr<HttpClient::Response> response, const SimpleWeb::error_code &ec) {
            if (!ec) {
                //cout << "Response content: " << response->content.rdbuf() << endl;
                cb_(std::move(response->content.string()), ec);
            } else {
                cout << "error is " << ec.message() << endl;
                cb_(response->content.string(), ec);
            }

        });
        client.io_service->run();
    }

}