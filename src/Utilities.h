#ifndef UTILITIES_H
#define UTILITIES_H

#define RUNNING_LINUX 
#include <algorithm>
#include <iostream>
#include <string>
namespace Utils {

const auto get_second = [](const auto& it) { return it.second; };
const auto get_first = [](const auto& it) { return it.first; };

#ifdef RUNNING_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#pragma comment(lib, "Shlwapi.lib")
#include <Shlwapi.h>
#include <windows.h>
inline std::string GetExecutableDirectory()
{
    char path[MAX_PATH];
    GetModuleFileNameA(nullptr, path, sizeof(path));
    PathRemoveFileSpecA(path);
    strcat_s(path, "\\");
    return std::string(path);
}
#elif defined RUNNING_LINUX
#include <unistd.h>
inline std::string GetExecutableDirectory()
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        return std::string(cwd) + std::string("/");
    }
    return "";
}
#endif
}

#endif
