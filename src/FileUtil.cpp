#include "FileUtil.h"

// one version by OS:
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
std::vector<std::string> FileUtil::list(std::string sPathAndMask)
{
    std::vector<std::string> vsResult;
	  HANDLE hfind;
	  WIN32_FIND_DATAA wfd;

	  // List the folder content
	  hfind = FindFirstFileA(sPathAndMask.c_str(), &wfd );
	  if (hfind != INVALID_HANDLE_VALUE)
	  {
		  do
		  {
				  vsResult.push_back( wfd.cFileName);
		  }
		  while(FindNextFileA( hfind, &wfd));
		  FindClose(hfind);
	  }
	  
	return vsResult;
}
//////////////////////////////////////////////////////////////////////////////
#endif

#ifdef __unix__ //linux
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
vector<string> FileUtil::list(string sPathAndMask)
{
    vector<string> vsResult;

//    for (const auto & entry : std::filesystem::directory_iterator(sPathAndMask))
//        vsResult.push_back(entry.path().string());

    return vsResult;
}
//////////////////////////////////////////////////////////////////////////////
#endif

//////////////////////////////////////////////////////////////////////////////
std::string FileUtil::get_path(std::string sFile)
{
    size_t iPos = sFile.find_last_of("\\");
    return sFile.substr(0, iPos);
}
//////////////////////////////////////////////////////////////////////////////
