#include "codec.h"

#include <algorithm>
#include <memory>
#include <stdlib.h>

int LineCode::TryDecode(const std::string& data, std::string& msg) {
    int len = 0;
    auto pos = data.find('\n');
    msg.assign(data.c_str(), pos);

    return pos;
}

void LineCode::Encode(std::string msg, Buffer& buffer) {
    msg.append(1, '\n');
    buffer.Append(msg.c_str(), msg.size());
}

int LengthCode::TryDecode(const std::string& data, std::string& msg) {
    int len = 0;
    auto pos = data.find('\n');
    msg.assign(data.c_str(), pos);

    return pos;
}

void LengthCode::Encode(std::string msg, Buffer& buffer) {

}
