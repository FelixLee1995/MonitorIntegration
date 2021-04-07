//
// Created by felix lee on 2020/10/27.
//

#ifndef MONITORINTEGRATION_DBMANAGER_H
#define MONITORINTEGRATION_DBMANAGER_H

#include <string>
#include <fstream>
#include <memory>
#include <iostream>
#include "nlohmann/json.hpp"
#include "sqlite3.h"
#include "fmt/format.h"



class DB {

public:
    DB(const std::string& dbFilePath){
        db_file_path_ = dbFilePath;

        sqlite3_ptr_ = NULL; // һ���򿪵����ݿ�ʵ��

        int result = sqlite3_open_v2(dbFilePath.c_str(), &sqlite3_ptr_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL);

        if (result == SQLITE_OK) {
            load_status = true;
        }
        else {
            load_status = false;
            std::cout << "Failed to create db, error_code: " << result << std::endl;
        }


        auto create_table_sql = "CREATE TABLE IF NOT EXISTS t_monitor_item (appname text PRIMARY KEY, address text NOT NULL, port integer NOT NULL, zabbix_host text NOT NULL, zabbix_itemid text);";
        sqlite3_stmt *stmt = NULL;        //stmt�����
        int res = sqlite3_prepare_v2(sqlite3_ptr_, create_table_sql, -1, &stmt, NULL);
        if (res == SQLITE_OK) {
            sqlite3_step(stmt);
        }

        sqlite3_finalize(stmt);
    }

    ~DB() {

    }


    int InsertData(const std::string& appname,  const std::string & address, int port, const std::string &zabbixhost){
        auto sql = fmt::format("INSERT INTO t_monitor_item(appname, address, port, zabbix_host) VALUES('{}', '{}', {}, '{}'); ",
                               appname, address, port, zabbixhost);
        sqlite3_stmt *stmt = NULL;        //stmt�����

        //���в���ǰ��׼����������������Ϸ���
        //-1����ϵͳ���Զ�����SQL���ĳ���
        int result = sqlite3_prepare_v2(sqlite3_ptr_, sql.c_str(), -1, &stmt, NULL);

        if (result == SQLITE_OK) {
            std::clog << "insert success";
            //ִ�и����
            sqlite3_step(stmt);
        } else {
            std::clog << "insert failed";
        }
        //�����������׼��ִ����һ�����
        sqlite3_finalize(stmt);

        return result;
    };


    int DeleteData(const std::string& appname) {
        auto sql = fmt::format("delete from t_monitor_item where appname = '{}'; ", appname);
        sqlite3_stmt *stmt = NULL;        //stmt�����

        //���в���ǰ��׼����������������Ϸ���
        //-1����ϵͳ���Զ�����SQL���ĳ���
        int result = sqlite3_prepare_v2(sqlite3_ptr_, sql.c_str(), -1, &stmt, NULL);

        if (result == SQLITE_OK) {
            std::clog << "delete success";
            //ִ�и����
            sqlite3_step(stmt);
        } else {
            std::clog << "delete failed";
        }
        //�����������׼��ִ����һ�����
        sqlite3_finalize(stmt);

        return result;
    }


    int ModifyData(const std::string& appname, const std::string & address, int port, const std::string& hostname){
        auto sql = fmt::format("update t_monitor_item  set address = '{}' , port = {}, zabbix_host = '{}' where appname = '{}'; ", address, port, hostname, appname);
        sqlite3_stmt *stmt = NULL;        //stmt�����

        //���в���ǰ��׼����������������Ϸ���
        //-1����ϵͳ���Զ�����SQL���ĳ���
        int result = sqlite3_prepare_v2(sqlite3_ptr_, sql.c_str(), -1, &stmt, NULL);

        if (result == SQLITE_OK) {
            std::clog << "modify success";
            //ִ�и����
            sqlite3_step(stmt);
        } else {
            std::clog << "modify failed";
        }
        //�����������׼��ִ����һ�����
        sqlite3_finalize(stmt);

        return result;
    }

    nlohmann::json QryDataList() {

        nlohmann::json j;

        const char *sqlSentence = "SELECT appname, address, port, zabbix_host, zabbix_itemid FROM t_monitor_item;";    //SQL���



        sqlite3_stmt *stmt = NULL;    // stmt�����

        //���в�ѯǰ��׼����������������Ϸ���
        //-1����ϵͳ���Զ�����SQL���ĳ���
        int result = sqlite3_prepare_v2(sqlite3_ptr_, sqlSentence, -1, &stmt, NULL);

        if (result == SQLITE_OK) {
            std::clog << "OK";
            // ÿ��һ��sqlite3_step()������stmt������ͻ�ָ����һ����¼
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                // ȡ����0���ֶε�ֵ

                auto appname = (const char *)sqlite3_column_text(stmt, 0);
                // ȡ����1���ֶε�ֵ
                auto address = (const char *)sqlite3_column_text(stmt, 1);
                //�����ز�ѯ������
                auto port = sqlite3_column_int(stmt, 2);
                auto hostname = (const char *)sqlite3_column_text(stmt, 3);
                auto itemid = (const char *)sqlite3_column_text(stmt, 4);

                nlohmann::json item;
                item["appname"] = appname;
                item["address"] = address;
                item["port"] = port;
                item["zabbixhost"] = hostname;
                if (itemid != nullptr) {
                    item["itemid"] = itemid;
                }
                else {
                    item["itemid"] = "";
                }
                j.push_back(item);
            }
        } else {
            std::clog << "qry failed";
        }
        //�����������׼��ִ����һ�����
        sqlite3_finalize(stmt);

        return j;

    }

    int UpdateMonitorItemByAppname(const std::string& appname, const std::string& itemID) {
        auto sql = fmt::format("update t_monitor_item  set zabbix_itemid = '{}' where appname = '{}'; ", itemID, appname);
        sqlite3_stmt *stmt = NULL;        //stmt�����

        //���в���ǰ��׼����������������Ϸ���
        //-1����ϵͳ���Զ�����SQL���ĳ���
        int result = sqlite3_prepare_v2(sqlite3_ptr_, sql.c_str(), -1, &stmt, NULL);

        if (result == SQLITE_OK) {
            std::clog << "modify success";
            //ִ�и����
            sqlite3_step(stmt);
        } else {
            std::clog << "modify failed";
        }
        //�����������׼��ִ����һ�����
        sqlite3_finalize(stmt);

        return result;
    }

private:

    std::string db_file_path_;
    bool load_status;
    sqlite3* sqlite3_ptr_;

};




#endif //MONITORINTEGRATION_DBMANAGER_H
