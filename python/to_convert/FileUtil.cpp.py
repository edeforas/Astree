# self file is covered by the  GNU LESSER GENERAL PUBLIC LICENSE Version 3 or later
# please see LICENSE.txt for more details and licensing issues
# copyright Etienne de Foras ( the author )  mailto: etienne.deforas@gmail.com

#include "FileUtil.h"

# one version by OS:
#ifdef _WIN32
#include <Windows.h>
using namespace std
#######################################
def get_executable_path(self):
	 char szEXEPath[ MAX_PATH  ]
     nChars = GetModuleFileNameA( nullptr, szEXEPath, MAX_PATH  )
    return string(szEXEPath,nChars)

#######################################
def list(self, sPathAndMask):
    vector<string> vsResult
	  HANDLE hfind
	  WIN32_FIND_DATAA wfd

	  # List the folder content
	  hfind = FindFirstFileA(sPathAndMask.c_str(), &wfd )
	  if hfind != INVALID_HANDLE_VALUE:
		  do
				  vsResult.push_back( wfd.cFileName)

		  while(FindNextFileA( hfind, &wfd))
		  FindClose(hfind)

	  
	return vsResult

#######################################
#endif

#ifdef __unix__ #linux
#include <limits.h>
#include <unistd.h>
#include <glob.h>
using namespace std
#######################################
def get_executable_path(self):
	char result[PATH_MAX]
	count = readlink("/proc/self/exe", result, PATH_MAX)
	return string(result, (count > 0) ? count : 0)

#######################################
def list(self, sPathAndMask):
	vector<string> vsResult

	#glob from https:#stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
	glob_t glob_result
	glob(sPathAndMask.c_str(), GLOB_TILDE, NULL, &glob_result)
	for (unsigned i = 0; i < glob_result.gl_pathc; ++i)		vsResult.push_back(string(glob_result.gl_pathv[i]))

	globfree(&glob_result)

    return vsResult

#######################################
#endif

#######################################
def get_path(self, sFile):
    iPos = sFile.find_last_of("\\")
    return sFile.substr(0, iPos)

#######################################
