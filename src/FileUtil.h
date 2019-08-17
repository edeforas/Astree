#ifndef _FileUtil_
#define _FileUtil_

#include <string>
#include <vector>
#include <filesystem>
using namespace std;

class FileUtil
{
public:
    static int file_size(char* sFileName);
    static bool file_delete(char* sFileName);
    static vector<string> list(string sPath);
    static string get_executable_path();
    static string get_path(string sFile);
};


#endif
