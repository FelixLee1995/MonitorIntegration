using System;
using MonitorIntegration;

namespace Caller
{
    class Program
    {


        static void Main(string[] args)
        {
            Console.WriteLine("Hello World!");

            var c = Decimal.Parse(args[0]);
            var ws = new WsTest();

            Monitor.Instance("monitorTestApp", (int)c, false);

            Monitor.RegisterIndicator("websocket_check", ws.websocket_check);
            Monitor.RegisterIndicator("websocket_check", ws.websocket_check);
            Monitor.RegisterIndicator("redis_check", ws.redis_check);
            Monitor.RegisterIndicator("mysql_check", ws.mysql_check);

            // 监控输出的api，  在需要监控内容处调用
            Monitor.MonitorOut("ws disconnected", "websocket client channel disconnected!", MonitorSeverity.Fatal);

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
                Key = "ws_client",
                StatusCode = (int)HealthStatus.UP,
                Level = (int)HealthSeverityLevel.NORMAL,
                Desc = "connection normal!"
            };
            return detail;

        }



        public HealthDetail mysql_check()
        {
            var detail = new HealthDetail
            {
                Key = "mysql_db",
                StatusCode = (int)HealthStatus.UP,
                Level = (int)HealthSeverityLevel.NORMAL,
                Desc = "connection normal!",
            };
            return detail;

        }




        public HealthDetail redis_check()
        {
            var detail = new HealthDetail
            {
                Key = "redis_db",
                StatusCode = (int)HealthStatus.DOWN,
                Level = (int)HealthSeverityLevel.FATAL,
                Desc = "connection down!"
            };
            return detail;

        }
    }
}
