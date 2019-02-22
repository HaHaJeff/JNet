#ifndef HTTP_H
#define HTTP_H

#include <map>
#include "tcpconn.h"
#include "codec.h"
#include "buffer.h"
#include "tcpserver.h"

/*
 * Http请求消息格式
 * 请求方法 空格 URL 协议版本 回车符 换行符    ---> 请求行
 * 头部字段名 : value 回车符 换行符
 * ...............................             ---> 请求头部
 * 头部字段名 : value 回车符 换行符
 * 回车符 换行符
 * 请求数据
 * */

/*
 * Http响应消息格式
 * 协议版本 空格 状态码 状态消息               ---> 状态行
 * 生成响应的日期和时间 MIME类型 编码方式      ---> 消息包头 
 * 回车符 换行符
 * 响应正文
 * */


/*
 * Http 状态码
 * 1xx: 指示信息———表示请求已接收，继续处理
 * 2xx: 成功——表示请求已被成功接收
 * 3xx: 重定向——要完成请求必须进行更进一步的操作
 * 4xx: 客户端错误——请求有语法错误或请求无法实现
 * 5xx: 服务端错误——服务器未能实现合法的请求
 * */

/*
 * Http请求方法
 * GET 请求指定的页面信息，并返回实体主体
 * HEAD 类似于get请求，只不过返回的响应中没有具体的内容，用于获取报头
 * POST 向指定资源提交数据进行处理（例如提交表单或上传文件）。数据被包含在请求体中。POST请求可能会导致新的资源的建立或已有资源的修改
 * PUT 从客户端向服务器传送的数据取代指定的文档的内容
 * DELETE 请求服务器删除指定的页面
 * CONNECT HTTP/1.1协议中预留给能够将连接改为管道方式的代理服务器
 * OPTIONS 允许客户端查看服务器的性能
 * TRACE 回显服务器受到的请求，主要用于测试或诊断
 * */

/*
 * GET和POST的区别？
 * GET提交，请求的数据会附在URL之后（就是把数据放置在HTTP协议头中），以？分割URL和数据，多个参数用&连接，特定浏览器和服务器对URL有长度限制，例如IE对URL的长度限制是2803字节。
 * POST提交，把提交的数据放置在HTTP的body中，理论上数控不受限制，但实际上各个WEB服务器会规定POST提交数据大小的限制
 * */

class HttpMsg {
public:
  enum Result {Error, Complete, NotComplete, Continue};
  HttpMsg() {HttpMsg::Clear();}

  virtual int Encode(Buffer& buf)=0;
  virtual Result TryDecode(std::string& buf, bool copyBody=true)=0;
  virtual void Clear();

  std::string GetHeader(const std::string& n) {
    return GetValueFromMap_(headers_, n);
  }

  std::string GetVersion() {
    return version_;
  }

  std::string GetBody() {
    return body_;
  }

  void SetBody(const std::string& body) {body_ = body;}
  void SetVersion(const std::string& version) {version_ = version;}
  void SetHeaders(const std::map<std::string, std::string>& headers) { headers_ = headers;}

  int GetByte() {
    return scanned_;
  }

protected:
  std::map<std::string, std::string> headers_;
  std::string version_;
  std::string body_;

protected:
  bool complete_;
  size_t contentLen_;
  size_t scanned_;
  Result TryDecode_(std::string& buf, int& lineStart, int& lineEnd);
  // [start, end]
  void DecodeHeader_(std::string& buf, int start, int end);
  std::string GetValueFromMap_(std::map<std::string, std::string>& m, const std::string& n);
};

class HttpRequest : public HttpMsg {
  public:
    HttpRequest() { Clear();}
    std::string GetArg(const std::string& n) { return GetValueFromMap_(args_, n); } 

    virtual int Encode(Buffer& buf);
    virtual Result TryDecode(std::string& buf, bool copyBody=true);
    virtual void Clear() {
      HttpMsg::Clear();
      args_.clear();
      method_ = "Get";  
      uri_.clear();
      query_uri_.clear();
    }
    void SetMethod(const  std::string& method) {method_ = method;}
    void SetUri(const std::string& uri) { uri_ = uri;}
    void SetQueryUri(const std::string& query_uri) { query_uri_ = query_uri;}
    std::string GetMethod() const {return method_;}
    std::string GetUri() const {return uri_;}
    std::string GetQueryUri() const {return query_uri_;}

  private:
    void DecodeArgs(std::string& buf);
    std::map<std::string, std::string> args_;
    std::string method_;
    std::string uri_;
    std::string query_uri_;
};

class HttpResponse : public HttpMsg {
public:
  HttpResponse() { Clear(); }
  void SetStatus(int st, const std::string& msg="") {
    status_ = st;
    statusWord_ = msg;
  }

  int GetStatus() { return status_; }
  std::string GetStatusWord() { return statusWord_; }

  virtual int Encode(Buffer& buf);
  virtual Result TryDecode(std::string& buf, bool copyBody=true);
  virtual void Clear() {
    HttpMsg::Clear();
    status_ = 200;
    statusWord_ = "OK";
  }
private:
  std::string statusWord_;
  int status_;
};

class HttpConnPtr {
public:
  HttpConnPtr(const TcpConnPtr& con): tcp_(con) {}
  TcpConnPtr operator()() const { return tcp_;}
  TcpConn* operator->() const { return tcp_.get();}

  typedef std::function<void(const HttpConnPtr&)> HttpCallBack;

  HttpRequest& GetRequest() const  {
    return tcp_->GetContext().context<HttpContext>().req;
  }
  HttpResponse& GetResponse() const { 
    return tcp_->GetContext().context<HttpContext>().rep;
  }

  void SendRequest() const { SendRequest(GetRequest());}
  void SendResponse() const { SendResponse(GetResponse());}
  void SendRequest(HttpRequest& req) const {
    req.Encode(tcp_->GetOutput()); // tcp_->GetOutput() return &
    LogOutput("http resquest");
    ClearData();
    tcp_->SendOutput();
  }
  //void SendFile(const std::string& filename) const;
  void SendResponse(HttpResponse& rep) const {
    rep.Encode(tcp_->GetOutput());
    LogOutput("http response");
    ClearData();
    tcp_->SendOutput();
  }
  void ClearData() const;
  void OnHttpMsg(const HttpCallBack& cb) const;
  TcpConnPtr GetTcp() {return tcp_;}
private:
  TcpConnPtr tcp_;

  struct HttpContext {
    HttpRequest req;
    HttpResponse rep;
  };
  void HandleRead_(const HttpCallBack& cb) const;
  void LogOutput(const char* title) const;
};

typedef HttpConnPtr::HttpCallBack HttpCallBack;

struct HttpServer: public TcpServer {
  HttpServer(EventLoop* loop);
  template<class Conn = TcpConn> 
  void SetConnType() { conncb_ = []{ return TcpConnPtr(new Conn);};}
  void OnGet(const std::string& uri, const HttpCallBack& cb) { cbs_["GET"][uri] = cb; }
  void OnRequest(const std::string& method, const std::string& uri, const HttpCallBack& cb) { cbs_[method][uri] = cb; }
  void OnDefault(const HttpCallBack& cb) { defcb_ = cb;}
private:
  HttpCallBack defcb_;
  std::function<TcpConnPtr()> conncb_;
  // 为每种method的每个uri定义回调函数
  std::map<std::string, std::map<std::string, HttpCallBack>> cbs_;
};


#endif
