#include "http.h"
#include "log.h"
#include <iostream>

int main()
{
    SETLOGLEVEL("FATAL");
    EventLoop loop;
    HttpServer server(&loop);
    server.Bind("0.0.0.0", 8080);
    server.OnGet("/ref", [](const HttpConnPtr& con){
        std::cout << "GET" << std::endl;
        std::string b = con.GetRequest().GetBody();
        std::string v = con.GetRequest().GetVersion();
        HttpResponse rep;
        rep.SetBody(b);
        con.SendResponse(rep);
        if (v == "HTTP/1.0") {con->Close();};
    });
    loop.Loop();
    return 0;
}
