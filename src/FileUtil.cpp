#include "FileUtil.h"

//////////////////////////////////////////////////////////////////////////////
string FileUtil::get_path(string sFile) 
{
	size_t iPos = sFile.find_last_of("\\");
	return sFile.substr(0, iPos);
}

#ifdef _WIN32

#include <windows.h>
//////////////////////////////////////////////////////////////////////////////
vector<string> FileUtil::list(string sPathAndMask)
{
	vector<string> vsResult;
	  HANDLE hfind;
	  WIN32_FIND_DATAA wfd;

	  // Liste le contenu du repertoire
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
string FileUtil::get_executable_path()
{
	 char szEXEPath[ MAX_PATH  ];
     DWORD nChars = GetModuleFileNameA( NULL, szEXEPath, MAX_PATH  );
	return string(szEXEPath,nChars);    
}
//////////////////////////////////////////////////////////////////////////////
#endif

#ifdef __unix__ //linux , using c++17

#include <limits.h>
#include <unistd.h>

//#include <filesystem>
//////////////////////////////////////////////////////////////////////////////
vector<string> FileUtil::list(string sPathAndMask)
{
	(void)sPathAndMask; //WIPWIP
	vector<string> vsResult;
/*
	for (const auto & entry : std::filesystem::directory_iterator(sPathAndMask))
		vsResult.push_back(entry.path().string());
*/
    return vsResult;
}
//////////////////////////////////////////////////////////////////////////////
string FileUtil::get_executable_path()
{
	char result[PATH_MAX];
	ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
	return std::string(result, (count > 0) ? count : 0);
}
//////////////////////////////////////////////////////////////////////////////

#endif