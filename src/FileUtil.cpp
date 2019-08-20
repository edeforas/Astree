#include "FileUtil.h"

//sicne there is now portable way to know the executable path, one version by OS:
#ifdef _WIN32
#include <Windows.h>
//////////////////////////////////////////////////////////////////////////////
std::string FileUtil::get_executable_path()
{
	 char szEXEPath[ MAX_PATH  ];
     DWORD nChars = GetModuleFileNameA( nullptr, szEXEPath, MAX_PATH  );
    return std::string(szEXEPath,nChars);
}
//////////////////////////////////////////////////////////////////////////////
#endif


#ifdef __unix__ //linux , using c++17
#include <limits.h>
#include <unistd.h>
//////////////////////////////////////////////////////////////////////////////
std::string FileUtil::get_executable_path()
{
	char result[PATH_MAX];
	ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
	return std::string(result, (count > 0) ? count : 0);
}
//////////////////////////////////////////////////////////////////////////////
#endif


#include <filesystem>
using namespace std;

//////////////////////////////////////////////////////////////////////////////
vector<string> FileUtil::list(string sPathAndMask)
{
    vector<string> vsResult;

    for (const auto & entry : std::filesystem::directory_iterator(sPathAndMask))
        vsResult.push_back(entry.path().string());

    return vsResult;
}
//////////////////////////////////////////////////////////////////////////////
string FileUtil::get_path(string sFile)
{
    size_t iPos = sFile.find_last_of("\\");
    return sFile.substr(0, iPos);
}
//////////////////////////////////////////////////////////////////////////////
