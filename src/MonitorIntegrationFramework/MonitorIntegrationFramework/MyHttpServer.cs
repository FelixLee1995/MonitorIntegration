using System;
using System.Collections.Generic;
using System.Text;
using Newtonsoft.Json;
using System.IO;
using System.Linq;
using System.Net;


namespace MonitorIntegration
{


    public class Server : Qoollo.Net.Http.HttpServer
    {
        public Server(int port)
            : base(port)
        {
            string url =  base.BaseUrl;
            Post["/healthcheck"] = Healthcheck;

        }

        private string Healthcheck(HttpListenerRequest arg)
        {
            return Indicator.Instance.RunHealthCheck();
        }
    }




    public class HttpServer: SingletonBase<HttpServer>
    {

        public void SetApiPort(int port)
        {
            port_ = port;
        }

        /// <summary>
        /// Defines the entry point of the application.
        /// </summary>
        /// <param name="args">The arguments.</param>
        public void Start()
        {
  
            server_ = new Server(port_);
            server_.Run();
        }

        public void Stop()
        {
            if (server_ != null) {
                server_.Stop();
            }

        }

        private int port_;
        private Server server_;
    }



}
