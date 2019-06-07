#ifndef FILE_H
#define FILE_H

#include <string>
#include "status.h"
namespace jnet {


class File {
public:
    static Status GetContent(const std::string& name, std::string& result);
    static Status WriteContent(const std::string& name, const std::string& content);
};

}
#endif
