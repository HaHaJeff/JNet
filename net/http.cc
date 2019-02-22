#include "http.h"
#include "status.h"
#include "log.h"

void HttpMsg::Clear() {
    headers_.clear();
    version_ = "HTTP/1.1";
    body_.clear();
    complete_ = false; 
    contentLen_ = 0;
    scanned_ = 0;
}

std::string HttpMsg::GetValueFromMap_(std::map<std::string, std::string>& m, const std::string& n) {
    auto p = m.find(n);
    return p == m.end() ? "" : p->second;
}

HttpMsg::Result HttpMsg::TryDecode_(std::string& buf, int & lineStart, int& lineEnd) {
   if (complete_) return Complete; 
   if (contentLen_ == 0) {
       // 请求行 header与body之间 \r\b\r\n
       auto p = buf.find("\r\n\r\n");
       if (p == std::string::npos) return NotComplete;
       lineStart = 0;
       lineEnd = buf.find("\r\n")-1;
       DecodeHeader_(buf, lineEnd+3, p-1);
       contentLen_ = atoi(GetHeader("content-length").c_str());
       scanned_ = p+4;
       body_ = buf.substr(scanned_, contentLen_);
       complete_ = true;
   }
   return complete_ ? Complete : NotComplete;
}

static void TrimeSpaceAndToLower(std::string& buf, int& start, int& end) {
    while(1) {
        if (isspace(buf[start])) {
            ++start;
        } else if (isspace(buf[end])) {
            --end;
        } else {
            break;
        }
    }
    int i = start;
    while(i <= end) {
        tolower(buf[i]);
    }
}

void HttpMsg::DecodeHeader_(std::string& buf, int start, int end) {
    while(start < end) {
        int i = buf.find("\r\n", start);
        int oldI = i;
        int kEnd = buf.find(":", start);
        int vStart = kEnd+1;
        TrimeSpaceAndToLower(buf, start, kEnd);
        TrimeSpaceAndToLower(buf, vStart, i);
        headers_[buf.substr(start,kEnd-start)] = buf.substr(vStart, i-vStart);
        start = oldI+2;
    }
}

int HttpRequest::Encode(Buffer& buf) {
    size_t osz = buf.GetSize();
    char conlen[1024], reqln[4096];
    snprintf(reqln, sizeof(reqln), "%s %s %s\r\n", method_.c_str(), query_uri_.c_str(), version_.c_str());
    buf.Append(reqln);
    for (auto& hd: headers_) {
        buf.Append(hd.first).Append(": ").Append(hd.second).Append("\r\n");
    }
    buf.Append("Connection: Keep-Alive\r\n");
    snprintf(conlen, sizeof(conlen), "Content-Length: %lu\r\n", GetBody().size());
    buf.Append(conlen);
    buf.Append("\r\n").Append(GetBody());
    return buf.GetSize() - osz;
}

HttpMsg::Result HttpRequest::TryDecode(std::string& buf, bool copyBody) {
    int start = 0, end = 0;
    Result r = TryDecode_(buf, start, end);
    int i = buf.find(" ", start);        
    method_ = buf.substr(start, i-start);
    int oldI = i;
    i = buf.find(" ", i+1);
    query_uri_ = buf.substr(i+1, i-oldI);
    version_ = buf.substr(i+1, end);
    if(query_uri_.size() == 0 || query_uri_[0] != '/') {
        ERROR("query uri %s '%,*s' should begin with /", query_uri_.size(), query_uri_.data());
        return Error;
    }

    DecodeArgs(query_uri_);
    return r;
}

// args 格式 url?name1=value1&name2=value2...
void HttpRequest::DecodeArgs(std::string& buf) {
    for (size_t i = 0; i <= query_uri_.size(); i++) {
        // 没有args
        if (i == query_uri_.size()) uri_ = query_uri_;

        if (query_uri_[i] == '?') {
            uri_ = buf.substr(0, i); 
            std::string qs = buf.substr(i+1, buf.size()-i-1);
            size_t c, kb, ke, vb, ve;
            ve = vb = ke = kb = c = 0;
            while (c < qs.size()) {
                while (c < qs.size() && qs[c] != '=' && qs[c] != '&') c++;
                ke = c;
                if (c < qs.size() && qs[c] == '=') c++; 
                vb = c;
                while(c < qs.size() && qs[c] != '&') c++;
                ve = c;
                if (c < qs.size() && qs[c] == '&') c++;
                if (kb != ke) {
                    args_[std::string(qs.data()+kb, qs.data()+ke)] = 
                    std::string(qs.data()+vb, qs.data()+ve);
                }
                kb = ke = vb = ve = c;
            }
            break;
        }
    }
}

int HttpResponse::Encode(Buffer& buf) {
    size_t osz = buf.GetSize();
    char conlen[1024], statusln[1024];
    snprintf(statusln, sizeof(statusln), "%s %d %s\r\n", version_.c_str(), status_, statusWord_.c_str());
    buf.Append(statusln);
    for (auto& hd : headers_) {
        buf.Append(hd.first).Append(": ").Append(hd.second).Append("\r\n");
    }
    buf.Append("Connection: Keep-Alive\r\n");
    snprintf(conlen, sizeof(conlen), "Content-Length: %lu\r\n", GetBody().size());
    buf.Append(conlen);
    buf.Append("\r\n").Append(GetBody());
    return buf.GetSize()-osz;
}

HttpMsg::Result HttpResponse::TryDecode(std::string& buf, bool copyBody) {
    int start = 0, end = 0;
    Result r = TryDecode_(buf, start, end);
    if(start < end) {
        int i = buf.find(" ", start);        
        version_ = buf.substr(start, i-start);
        int oldI = i;
        i = buf.find(" ", i+1);
        status_ = atoi(buf.substr(i+1, i-oldI).c_str());
        statusWord_ = buf.substr(i+1, end);
    }
    return r;
}

void HttpConnPtr::OnHttpMsg(const HttpCallBack& cb) const {
    tcp_->OnRead([cb](const TcpConnPtr& con) { HttpConnPtr hcon(con); hcon.HandleRead_(cb);});
}

void HttpConnPtr::HandleRead_(const HttpCallBack& cb) const {
    if (tcp_->IsClient()) {
        HttpRequest& req = GetRequest();
        std::string buf(tcp_->GetInput().GetData(), tcp_->GetInput().GetSize());
        HttpMsg::Result r = req.TryDecode(buf);
        if (r == HttpMsg::Error) {
            tcp_->Close();
            return;
        }
        if (r == HttpMsg::Continue) {
            tcp_->Send("Http/1.1 100 Continue\n\r\n");
        } else if (r == HttpMsg::Complete) {
            INFO("http request: %s %s %s", req.GetMethod().c_str(), req.GetVersion(), req.GetQueryUri());
            TRACE("http request: \n%.*s", buf.size(), buf.c_str());
            cb(*this);
        }
    } else {
        HttpResponse& rep = GetResponse();
        std::string buf(tcp_->GetInput().GetData(), tcp_->GetInput().GetSize());
        HttpMsg::Result r = rep.TryDecode(buf);
        if (r == HttpMsg::Error) {
            tcp_->Close();
            return;
        }
        if (r == HttpMsg::Complete) {
            INFO("http response: %d %s", rep.GetStatus(), rep.GetStatusWord().c_str()); 
            TRACE("http request: \n%.*s", buf.size(), buf.c_str());
            cb(*this);
        }
    }
}

void HttpConnPtr::ClearData() const {
    if (tcp_->IsClient()) {
        tcp_->GetInput().Consume(GetResponse().GetByte());
        GetResponse().Clear();
    } else {
        tcp_->GetInput().Consume(GetRequest().GetByte());
        GetRequest().Clear();
    }
}

void HttpConnPtr::LogOutput(const char* title) const {
    Buffer& o = tcp_->GetOutput();
    TRACE("%s:\n%.*s", title, o.GetSize(), o.GetData());
}

HttpServer::HttpServer(EventLoop* loop): TcpServer(loop) {
    defcb_ = [](const HttpConnPtr& con) {
        HttpResponse& rep = con.GetResponse();
        rep.SetStatus(404, "Not Found");
        rep.SetBody("Not Found");
        con.SendResponse();
    };

    conncb_ = [] { return TcpConnPtr(new TcpConn);};
    OnConnCreate([this]() {
        HttpConnPtr hcon(conncb_());
        hcon.OnHttpMsg([this](const HttpConnPtr& hcon) {
            HttpRequest& req = hcon.GetRequest();
            auto p = cbs_.find(req.GetMethod());
            if(p != cbs_.end()) {
                auto p2 = p->second.find(req.GetUri());
                if (p2 != p->second.end()) {
                    p2->second(hcon);
                    return;
                }
            }
            defcb_(hcon);
         });
        return hcon.GetTcp();
    });
}