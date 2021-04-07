using System;
using System.Net;
using System.Text;

namespace ConsoleApp
{
    class MyHttpServer : HttpServerBase
    {
        public MyHttpServer(int maxThreads) : base(maxThreads)
        { }

        protected override void ProcessHttpRequest(HttpListenerContext context)
        {
            Console.WriteLine("You can do anything......");

            string response = "Hello World!";
            byte[] buffer = Encoding.UTF8.GetBytes(response);
            context.Response.StatusCode = 200;
            context.Response.ContentLength64 = buffer.Length;
            context.Response.OutputStream.Write(buffer, 0, buffer.Length);
            context.Response.OutputStream.Close();
            context.Response.Close();
        }
    }
}