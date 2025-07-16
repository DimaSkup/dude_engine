// ==================================================================
// Filename:    log.cpp
// Description: implementation of logger
//
// Created:     07.04.2025 by DimaSkup
// ==================================================================
#include "Log.h"
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>

#define BUF_SIZE 512

// static ptr to the logger file
FILE* s_pLogFile = NULL;

// global string buffer
char g_String[BUF_SIZE]{'\0'};

// static string buffer for logger internal purposes
char s_StrTempBuf[BUF_SIZE]{'\0'};


//---------------------------------------------------------
// Desc:   set console color to some particular by input code
// Args:   - keyColor: key code to change color
//---------------------------------------------------------
void SetConsoleColor(const char* keyColor)
{
    printf("%s", keyColor);
}

//---------------------------------------------------------
// Desc:   a helper for printing messages into the console
//         and into the logger file
// Args:   - fileName:  path to the caller file
//         - funcName:  name of the caller function/method
//         - type:      type of the log message
//         - text:      log message content
//         - codeLine:  line of code where logger was called
//---------------------------------------------------------
void PrintHelper(
    const char* fileName,
    const char* funcName,
    const char* type,
    const char* text,
    const int codeLine)
{
    const char* fmt = "[%05ld] %s %s: %s() (line: %d): %s\n";
    const time_t t = clock();

    printf(fmt, t, type, fileName, funcName, codeLine, text);

    if (s_pLogFile)
        fprintf(s_pLogFile, fmt, t, type, fileName, funcName, codeLine, text);
}

//---------------------------------------------------------
// Desc:   create a logger file into which we will write messages
// Ret:    1 if everything is OK, and 0 if something went wrong
//---------------------------------------------------------
int InitLogger(void)
{
    s_pLogFile = fopen("dude_engine_log.txt", "w");

    if (s_pLogFile != NULL)
    {
        printf("the log file is created!\n");

        time_t rawTime;
        struct tm* info = NULL;
        char buffer[80];

        time(&rawTime);
        info = localtime(&rawTime);
        strftime(buffer, 80, "%x - %I:%M%p", info);

        fprintf(s_pLogFile, "%s| the log file is create!\n", buffer);
        fprintf(s_pLogFile, "-------------------------\n\n");

        return 1;
    }
    else
    {
        printf("CRITICAL_ERROR: can't create a log file");
        return 0;
    }
}

//---------------------------------------------------------
// Desc:   print msg about closing of the log file and close it
//---------------------------------------------------------
void CloseLogger(void)
{
    time_t rawTime;
    struct tm* info = NULL;
    char buffer[80];

    time(&rawTime);
    info = localtime(&rawTime);
    strftime(buffer, 80, "%x -%I:%M%p", info);

    fprintf(s_pLogFile, "\n--------------------------------\n");
    fprintf(s_pLogFile, "%s| this is the end, my only friend, the end\n");

    fclose(s_pLogFile);
}

//---------------------------------------------------------
// Desc:   print a usual message into console but without
//         info about the caller
// Args:   - format:   format string for variadic aruments
//         - ...:      variadic arguments
//---------------------------------------------------------
void LogMsg(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    // reset the buffer
    memset(s_StrTempBuf, 0, BUF_SIZE);

    // make a string with input log-message
    vsnprintf(s_StrTempBuf, BUF_SIZE-1, format, args);

    // print a message into the console and log-file
    const char* fmt = "[%05ld] %s\n";
    const time_t t = clock();

    printf(fmt, t, s_StrTempBuf);

    if (s_pLogFile)
        fprintf(s_pLogFile, fmt, t, s_StrTempBuf);

    va_end(args);
}

//---------------------------------------------------------
// Desc:   print a usual message into console
// Args:   - fileName:   path to the caller file
//         - funcName:   name of the caller function
//         - codeLine:   line of code where logger was called
//         - format:     format string for variadic arguments
//         - ...:        variadic arguments
//---------------------------------------------------------
void LogMsg(
    const char* fileName,
    const char* funcName,
    const int codeLine,
    const char* format,
    ...)
{
    va_list args;
    va_start(args, format);

    // reset the buffer
    memset(s_StrTempBuf, 0, BUF_SIZE);

    // make a string with input log-message
    vsnprintf(s_StrTempBuf, BUF_SIZE-1, format, args);

    // print a message into the console and log file
    SetConsoleColor(GREEN);
    PrintHelper(fileName, funcName, "", s_StrTempBuf, codeLine);
    SetConsoleColor(RESET);

    va_end(args);
}

//---------------------------------------------------------
// Desc:   print a debug message into console
// Args:   - fileName:   path to the caller file
//         - funcName:   name of the caller function
//         - codeLine:   line of code where logger was called
//         - format:     format string for variadic arguments
//         - ...:        variadic arguments
//---------------------------------------------------------
void LogDbg(
    const char* fileName,
    const char* funcName,
    const int codeLine,
    const char* format,
    ...)
{
    va_list args;
    va_start(args, format);

    // reset the buffer
    memset(s_StrTempBuf, 0, BUF_SIZE);

    // make a string with input log-message
    vsnprintf(s_StrTempBuf, BUF_SIZE-1, format, args);

    // print a message into the console and log file
    SetConsoleColor(RESET);
    PrintHelper(fileName, funcName, "DEBUG:", s_StrTempBuf, codeLine);

    va_end(args);
}

//---------------------------------------------------------
// Desc:   print an error message into console
// Args:   - fileName:   path to the caller file
//         - funcName:   name of the caller function
//         - codeLine:   line of code where logger was called
//         - format:     format string for variadic arguments
//         - ...:        variadic arguments
//---------------------------------------------------------
void LogErr(
    const char* fileName,
    const char* funcName,
    const int codeLine,
    const char* format,
    ...)
{
    va_list args;
    va_start(args, format);

    // reset the buffer
    memset(s_StrTempBuf, 0, BUF_SIZE);

    // make a string with input log-message
    vsnprintf(s_StrTempBuf, BUF_SIZE-1, format, args);

    // print a message into the console and log file
    SetConsoleColor(RED);
    PrintHelper(fileName, funcName, "ERROR:", s_StrTempBuf, codeLine);
    SetConsoleColor(RESET);

    va_end(args);
}

