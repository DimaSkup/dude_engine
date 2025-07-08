// ==================================================================
// Filename:     Filesystem.h
// Description:  utils for working with files paths
// 
// Created:      17.04.2025 by DimaSkup
// ==================================================================
#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include "Log.h"
#include <string.h>


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

    //-----------------------------------------------------
    // Desc:  get a position of the last slash in the input path
    // Args:  - path: a path to some file
    // Ret:   position of the last slash
    //-----------------------------------------------------
    inline static int GetLastSlashOffset(const char* path)
    {
        const char* offset1 = strrchr(path, '\\');
        const char* offset2 = strrchr(path, '/');

        if ((offset1 == NULL) && (offset2 == NULL))
        {
            // if no slash or backslash
            return 0;
        }
        else if (offset1 == NULL)
        {
            return offset2 - path;
        }
        else
        {
            return offset1 - path;
        }
    }

    //-----------------------------------------------------
    // Desc:  get a filename by input path
    // Args:  - path:  path to some file
    // Out:   an extracted filename
    //-----------------------------------------------------
    inline static void GetFileName(const char* path, char* outFilename)
    {
        // returns the filename path component
        const char* lastSlash = path + GetLastSlashOffset(path) + 1;
        strcpy(outFilename, lastSlash);
        outFilename[lastSlash - path] = '\0';              // put extra NULL
    }
    
    //-----------------------------------------------------
    // Desc:  get a stem by input path 
    // Args:  - path:  path to some file
    // Out:   a filename without extension and parent path
    //-----------------------------------------------------
    inline static void GetFileStem(const char* path, char* outStem)
    {
        // returns the stem path component (filename without the final extension)

        // define where stem starts (what is the offset)
        const int offset = GetLastSlashOffset(path);
        const char* stemStart = path + offset + 1;

        // define where is the dot
        const char* dotPos = strrchr(path, '.');
        int stemLen = 0;

        if (dotPos)
            stemLen = dotPos - stemStart;
        else
            stemLen = strlen(path) - offset;

        // make the output string
        strncpy(outStem, stemStart, stemLen);
        outStem[stemLen] = '\0';                           // put extra NULL
    }


};

#endif

