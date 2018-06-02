#include "FileUtil.h"

// dep a verifier
#include <stdio.h>
//#include <types.h>
#include <sys/stat.h>
#include <windows.h>

//////////////////////////////////////////////////////////////////////////////
int FileUtil::file_size(char* sFileName)
{
    struct stat stat_p;

    if (stat(sFileName, &stat_p) == -1)
    {
        return -1;
    }
    return stat_p.st_size;
}
//////////////////////////////////////////////////////////////////////////////
bool FileUtil::file_delete(char* sFileName)
{
    remove(sFileName);
    return true;
}
//////////////////////////////////////////////////////////////////////////////
vector<string> FileUtil::list(string sPathAndMask) //TODO faire qq chose portable
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
            //     if( wfd.cFileName[0] != '.')
            {
                vsResult.push_back( wfd.cFileName);
            }

        }
        while(FindNextFileA( hfind, &wfd));
        FindClose(hfind);
    }

    return vsResult;
}
//////////////////////////////////////////////////////////////////////////////
string FileUtil::get_executable_path()
{
    char szEXEPath[ 2048 ];
    DWORD nChars = GetModuleFileNameA( NULL, szEXEPath, 2048 );

    return string(szEXEPath,nChars);    
}
//////////////////////////////////////////////////////////////////////////////
string FileUtil::get_path(string sFile)
{
    size_t iPos=sFile.find_last_of("\\");
    return sFile.substr(0,iPos);
}
//////////////////////////////////////////////////////////////////////////////

