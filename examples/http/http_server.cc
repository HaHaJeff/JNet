#include "http.h"
#include "net/log.h"
#include <iostream>

using namespace jnet;
int main()
{
    SETLOGLEVEL("FATAL");
    EventLoop loop;
    HttpServer server(&loop);
    server.Bind("0.0.0.0", 8080);
    server.OnGet("/ref", [](const HttpConnPtr& con){
        std::string b = con.GetRequest().GetBody();
        std::string v = con.GetRequest().GetVersion();
        HttpResponse rep;
        rep.SetBody(b);
        con.SendResponse(rep);
        if (v == "HTTP/1.0") {
        con->Close();
        };
    });
    loop.Loop();
    return 0;
}
