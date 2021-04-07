//
// Created by felix lee on 2020/10/26.
//

#ifndef MONITORINTEGRATION_CCONFIGMANAGER_H
#define MONITORINTEGRATION_CCONFIGMANAGER_H

#include "nlohmann/json.hpp"
#include <fstream>
#include <regex>


enum ConfigFieldType {
    TypeIPv4 = 0,
    TypeIPv6 = 1,
    TypePort = 2,
    TypeText = 3,
    TypeNumberic = 4,

};





class ConfigManager{
public:
    ConfigManager(const std::string &configPath):field_check_map_() {

        try {
            std::ifstream  ifs(configPath);
            ifs >> json_value_;

            file_path_ = configPath;

        }
        catch (std::exception& e) {
            std::cout << "failed to load config file " << configPath << " reaseon: " << e.what() << std::endl;
        }
    }

    bool IsValidIPv4(const std::string& strIP)
    {
        std::regex regExpress("(?=(\\b|\\D))(((\\d{1,2})|(1\\d{1,2})|(2[0-4]\\d)|(25[0-5]))\\.){3}((\\d{1,2})|(1\\d{1,2})|(2[0-4]\\d)|(25[0-5]))(?=(\\b|\\D))");
        return std::regex_match(strIP, regExpress);
    }

    bool AddFieldCheck(const std::string& field,  ConfigFieldType type) {

        auto iter = field_check_map_.find(field);
        if (iter!=field_check_map_.end()) {
            std::cout << "FieldCheck already exists: " << field << std::endl;
            return false;
        }

        field_check_map_.insert(std::make_pair(field, type));
        return true;
    }


    bool CheckAllField() {
        nlohmann::json j;
        try {
            std::ifstream  ifs(file_path_);
            ifs >> j;

        }
        catch (std::exception& e) {
            std::cout << "failed to load config file " << file_path_ << " reaseon: " << e.what() << std::endl;
            return false;
        }
        bool ret = true;

        for (auto iter = field_check_map_.begin();iter != field_check_map_.end();iter ++) {
            auto res = CheckValid(j, iter->first, iter->second);
            if (!res) {
                ret = false;
            }
        }

        return ret;
    }


    bool CheckValid(const nlohmann::json& j, const std::string& key, ConfigFieldType type){

        auto flat_json = j.flatten();
        if (!flat_json.contains(key)){
            std::cout << "field check failed: " << key << " not found" << std::endl;
            return false;
        }

        try{
            switch (type) {
                case ConfigFieldType::TypeText:{
                    flat_json[key].get<std::string>();
                    break;
                }
                case ConfigFieldType::TypeNumberic:{
                    flat_json[key].get<double>();
                    break;
                }
                case ConfigFieldType::TypePort :{
                    auto port = flat_json[key].get<int>();
                    if (port <0 || port > 65535) {
                        std::cout << "field check failed, field:" << key << ", must be vaild port number" << std::endl;
                        return false;
                    }
                    break;
                }
                case ConfigFieldType::TypeIPv4:{
                    auto ipv4 = flat_json[key].get<std::string>();
                    auto ret = IsValidIPv4(ipv4);
                    if (!ret) {
                        std::cout << "field check failed, field:" << key << ", must be vaild ipv4 address" << std::endl;
                    }
                    return ret;
                }
            }
        }
        catch (nlohmann::detail::exception & e) {
            std::cout << "field check failed, field:" << key <<", json handle exception: " << e.what() << std::endl;
            return false;
        }


        return true;



    }

    template<typename  T>
    T LookupConfig(const std::string& configName, const T& defaultValue) {
        if (json_value_.contains(configName)) {
            return json_value_[configName].get<T>();
        }
        return defaultValue;
    }

private:
    std::string file_path_;
    nlohmann::json json_value_;
    std::map<std::string,  ConfigFieldType> field_check_map_;
};



#endif //MONITORINTEGRATION_CCONFIGMANAGER_H
