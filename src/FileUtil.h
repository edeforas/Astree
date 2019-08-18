#ifndef _FileUtil_
#define _FileUtil_

#include <string>
#include <vector>
using namespace std;

class FileUtil
{
public:
    static vector<string> list(string sPath);
    static string get_executable_path();
    static string get_path(string sFile);
};


#endif
