using System;
using System.Collections.Generic;


namespace MonitorIntegration
{


    using Newtonsoft.Json;



    public enum MonitorSeverity
    {
        Normal = 0,
        Error = 1,
        Fatal = 2
    }


    public class Monitor
    { 
        private static bool init_flag_ = false;

        private static bool zabbix_monitor_flag_ = false;

        private static ZabbixConfig zbx_config_;

        //appName: 应用的名称， 尽量做到唯一, port: 对外开启的端口， if_base_check: 保留参数， 填入false
        public static  void Instance(string appName)
        {

            try
            {
                string str = System.IO.File.ReadAllText("monitor/zabbix.json");
                zbx_config_ = JsonConvert.DeserializeObject<ZabbixConfig>(str);

            }
            catch (Exception e)
            {
                Console.WriteLine("failed to open zabbix config file, error: " + e.Message);
                //todo exit
            }


            zabbix_monitor_flag_ = zbx_config_.monitor_out_flag == 1;

            if (init_flag_ == false)
            {
                init_flag_ = true;

                Indicator.Instance.SetAppName(appName);
                HttpServer.Instance.SetApiPort(zbx_config_.health_check_port);
                HttpServer.Instance.Start();

                if (zabbix_monitor_flag_)
                {
                    ZabbixSender.Instance.config_path_ = "monitor/zabbix.json";
                    ZabbixSender.Instance.Start(zbx_config_);
                }

           

            }
            else
            {
                Console.WriteLine("Monitor Error: Can only init once!");
            }

          
        }


        private static bool disposed = false; // To detect redundant calls


        protected virtual void Dispose(bool disposing)
        {
            if (!disposed)
            {
                if (disposing)
                {
                    ZabbixSender.Instance.Close();
                    HttpServer.Instance.Stop();
                }

                disposed = true;
            }
        }

        public static void Dispose()
        {
            // Do not change this code. Put cleanup code in Dispose(bool disposing) above.
            if (!disposed)
            {

                ZabbixSender.Instance.Close();
                HttpServer.Instance.Stop();
                disposed = true;
            }
        }


        public static bool RegisterIndicator(string indicatorKey, Indicator.HealthCheckFuncDelegate indicatorFunc)
        {
            
            if (funcDicts.ContainsKey(indicatorKey))
            {
                Console.WriteLine("funcKey already exists: " + indicatorKey);
                return false;
            }
            funcDicts.Add(indicatorKey, indicatorFunc);
            Indicator.Instance.exer += indicatorFunc;

            return true;
        }

        private static Dictionary<string, Indicator.HealthCheckFuncDelegate> funcDicts = new Dictionary<string, Indicator.HealthCheckFuncDelegate>();


        public static void MonitorOut(string issue, string desc, MonitorSeverity severity) {

            string sev;

            switch (severity)
            {
                case MonitorSeverity.Normal: sev = "Normal"; break;
                case MonitorSeverity.Error: sev = "Error"; break;
                case MonitorSeverity.Fatal: sev = "Fatal"; break;
                default: sev = "Unknown"; break;
            }
            DateTime dt = DateTime.Now;

            string msg = string.Format("{0}@{1}@{2}@{3}@MONITORFLAG", dt.ToString("yyyy-MM-dd HH:mm:ss"), issue, desc, sev);

            if (zabbix_monitor_flag_)
            {
                ZabbixSender.Instance.SummitMsg(msg);
            }

            Console.WriteLine(string.Format("{0}@{1}@{2}@{3}@MONITORFLAG", dt.ToString("yyyy-MM-dd HH:mm:ss"), issue, desc, sev));
        }

    }

    public enum MI_RET
    {
        MI_FAILURE = -1,
        MI_OK = 0,
    };



    public enum HealthStatus
    {
        UP = 0,
        UNKNOWN,
        OUT_OF_SERVICE,
        DOWN,
    };

    public enum HealthSeverityLevel
    {
        NORMAL = 0,
        ERR = 1,
        FATAL = 2,
    };




    public class Indicator: SingletonBase<Indicator>
    {

        public void SetAppName(string name)
        {
            AppName_ = name;
        }


        public delegate HealthDetail HealthCheckFuncDelegate();

        public HealthCheckFuncDelegate exer;

        public string RunHealthCheck()
        {

            var totalHealth = new TotalHealth();
            int code = 0;
            if (exer != null)
            {

      
                totalHealth.AppName = AppName_;
                foreach (HealthCheckFuncDelegate delegater in exer.GetInvocationList())
                {
                    var res = delegater();
                    totalHealth.Detail.Add(res);
                    if (res.status_code != (int)HealthStatus.UP)
                    {
                        code += res.status_code;
                    }

                }

                totalHealth.Health = code;

            }
            else
            {
                totalHealth.AppName = AppName_;
                code = 0;
            }
            HealcheckRet ret = new HealcheckRet();

            ret.data = totalHealth;

            return JsonConvert.SerializeObject(ret);

        }

        private string AppName_;


    }


    public class HealthDetail
    {
        public string key;
        public string desc;
        public int status_code;
        public int level;
    }

    public class HealcheckRet {

        public TotalHealth data;
        public int func_id = 10000;
    }

    public class TotalHealth
    {
        public TotalHealth()
        {
            Detail = new List<HealthDetail>();
        }


        public string AppName;
        public int Health;
        public List<HealthDetail> Detail;
    }

}
