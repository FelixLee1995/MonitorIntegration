using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SynchronousTCPClientLibrary;
using System.Net.Sockets;
using System.Net;
using System.Threading;
using Newtonsoft.Json;


namespace MonitorIntegration
{


    //	nlohmann::json j;

    public class ZabbixMsg
    {
        public string request { get; set; }
        public List<ZabbixData> data { get; set; }
    }

    public class ZabbixData
    {
        public string host { get; set; }
        public string key { get; set; }
        public string value { get; set; }
    }

    public class ZabbixSender: SingletonBase<ZabbixSender>
    {

        public void SummitMsg(string msg)
        {
            msg_queue_.Enqueue(msg);
        }

        public void SendMsg(string msg)
        {
            try
            {
                Socket tcpClient = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                IPAddress ipaddress = IPAddress.Parse(zbx_config_.zabbix_server);
                EndPoint point = new IPEndPoint(ipaddress, zbx_config_.zabbix_port);
                tcpClient.Connect(point);//通过IP和端口号来定位一个所要连接的服务器端

                if (!tcpClient.Connected)
                {
                    Console.WriteLine("cant connect!");
                    return;
                }


                ZabbixMsg zbx_msg = new ZabbixMsg();
                zbx_msg.request = "sender data";
                zbx_msg.data = new List<ZabbixData>();
                ZabbixData zbx_data = new ZabbixData() { host = zbx_config_.host, key = zbx_config_.key, value = msg };
                zbx_msg.data.Add(zbx_data);




                string json_str = JsonConvert.SerializeObject(zbx_msg);

                byte[] arr = new byte[json_str.Length + 13];
                arr[0] = (byte)'Z';
                arr[1] = (byte)'B';
                arr[2] = (byte)'X';
                arr[3] = (byte)'D';
                arr[4] = 0x01;


                arr[5] = (byte)json_str.Length;
                int i = 0;
                foreach (var c in json_str)
                {
                    arr[13 + i] = (byte)c;
                    i++;
                }

                int send_cnt = tcpClient.Send(arr, json_str.Length + 13, SocketFlags.None);
                Console.WriteLine("send success: cnt " + send_cnt);
                byte[] bytes = new byte[1024];
                int ret = tcpClient.Receive(bytes);
                Console.WriteLine("recv cnt " + ret);

                //todo 输出zabbix上报的应答

                tcpClient.Close();
            }
            catch (ArgumentNullException ex)
            {
                Console.WriteLine(ex.ToString());
            }
            catch (SocketException ex)
            {
                Console.WriteLine(ex.ToString());
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }

        private bool is_running = false;
        private bool disposed = false; // To detect redundant calls

        public string config_path_ { get; set; }

        private Queue<string> msg_queue_;

        private ZabbixConfig zbx_config_;

        private Thread work_thread_;

        public void Start(ZabbixConfig config)
        {
            zbx_config_ = config;


            is_running = true;
            msg_queue_ = new Queue<string>(128);

            ThreadStart childref = new ThreadStart(work);
            work_thread_ = new Thread(childref);
            work_thread_.Start();

        }


        protected virtual void Dispose(bool disposing)
        {
            if (!disposed)
            {
                if (disposing)
                {
                    is_running = false;
                    if (work_thread_!=null)
                        work_thread_.Join();
                }

                disposed = true;
            }
        }


        public void Close()
        {
            Dispose(true);
        }

        public void work()
        {

            while (is_running)
            {
                if (msg_queue_.Count() > 0)
                {
                    string msg = msg_queue_.Dequeue();
                    if (msg != "")
                        SendMsg(msg);
                }

                Thread.Sleep(100);
            }



        }
    }


    public class ZabbixConfig
    {
        public string host { get; set; }
        public string key { get; set; }
        public string monitor_out { get; set; }
        public int zabbix_port { get; set; }
        public string zabbix_server { get; set; }
        public string zabbix_submit_rule { get; set; }
        public int monitor_out_flag { get; set; }
        public int health_check_port { get; set; }
    }
}
