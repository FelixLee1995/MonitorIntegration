using System;
using MonitorIntegration;
using System.Net.Sockets;
using System.Net;
using System.Threading;
using Monitor = MonitorIntegration.Monitor;
using System.Collections.Generic;


namespace Caller
{
    class Program
    {


        static void Main(string[] args)
        {
            Console.WriteLine("Hello World!");

            var ws = new WsTest();

            Monitor.Instance("monitorTestApp");

            Monitor.RegisterIndicator("websocket_check", ws.websocket_check);
            Monitor.RegisterIndicator("websocket_check", ws.websocket_check);
            Monitor.RegisterIndicator("redis_check", ws.redis_check);
            Monitor.RegisterIndicator("mysql_check", ws.mysql_check);

            Thread.Sleep(200);

            // 监控上报的api，  在需要监控内容处调用
            Monitor.MonitorOut("ws disconnected", "websocket client channel disconnected!", MonitorSeverity.Fatal);
            Monitor.MonitorOut("ws connected", "websocket client channel reconnected!", MonitorSeverity.Normal);

            Monitor.Dispose();

            Console.ReadKey();

            return;
        }
    }


    public class WsTest
    {


        public HealthDetail websocket_check()
        {
            var detail = new HealthDetail
            {
                key = "ws_client",
                status_code = (int)HealthStatus.UP,
                level = (int)HealthSeverityLevel.NORMAL,
                desc = "connection normal!"
            };
            return detail;

        }



        public HealthDetail mysql_check()
        {
            var detail = new HealthDetail
            {
                key = "mysql_db",
                status_code = (int)HealthStatus.UP,
                level = (int)HealthSeverityLevel.NORMAL,
                desc = "connection normal!",
            };
            return detail;

        }




        public HealthDetail redis_check()
        {
            var detail = new HealthDetail
            {
                key = "redis_db",
                status_code = (int)HealthStatus.DOWN,
                level = (int)HealthSeverityLevel.FATAL,
                desc = "connection down!"
            };
            return detail;

        }
    }
}


