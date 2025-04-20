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
    inline static bool IsPathEmpty(const char* path) { return (!path || path[0] == '\0'); }

    ///////////////////////////////////////////////////////

    static bool Exists(const char* path)
    {
        // check if there is any file by path (relatively to the working directory)

        if (IsPathEmpty(path))
        {
            LogErr("input path is empty!");
            return false;
        }

        FILE* pFile = nullptr;

        if ((pFile = fopen(path, "r+")) == nullptr)       
        {
            sprintf(g_String, "there is no texture by path: %s", path);
            LogErr(g_String);
            return false;
        }

        fclose(pFile);
        return true;
    }
};

#endif

