#ifndef FileUtil_
#define FileUtil_

#include <string>
#include <vector>

class FileUtil
{
public:
    static std::vector<std::string> list(const std::string& sPathAndMask);
    static std::string get_executable_path();
    static std::string get_path(std::string sFile);
};


#endif
