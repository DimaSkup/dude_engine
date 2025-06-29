// ==================================================================
// Filename:     Filesystem.h
// Description:  utils for working with files paths
// 
// Created:      17.04.2025 by DimaSkup
// ==================================================================
#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "Log.h"

class FileSys
{
public:

    //-----------------------------------------------------
    // Desc:   check if input path is an empty string
    // Args:   - path:   a string with path to file
    // Ret:    true if input str is empty
    //-----------------------------------------------------
    inline static bool IsPathEmpty(const char* path) 
    {   return (!path || path[0] == '\0');  }

    ///////////////////////////////////////////////////////

    static bool Exists(const char* path)
    {
        // check if there is any file by path (relatively to the working directory)

        if (IsPathEmpty(path))
        {
            LogErr(LOG, "input path is empty!");
            return false;
        }

        FILE* pFile = nullptr;

        if ((pFile = fopen(path, "r+")) == nullptr)       
        {
            LogErr(LOG, "there is no texture by path: %s", path);
            return false;
        }

        fclose(pFile);
        return true;
    }
};

#endif

