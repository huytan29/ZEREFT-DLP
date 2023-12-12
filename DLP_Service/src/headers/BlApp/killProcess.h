// author: Tan Huy - ZEREFT29

#pragma once
#include <Windows.h>
#include <minwindef.h>
#include <set>
#include <map>
#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <cstdlib>
#include <cwchar>
#include <tlhelp32.h>
#include <cerrno>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <thread>
#include <locale>
#include <psapi.h>
#include "../json.hpp"


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

using json = nlohmann::json;

namespace stringUtil
{
    const unsigned int MAX_BUF_SIZE = 4096;

    const char* unicodeToAnsi(const wchar_t *buffer);
    const wchar_t* AnsiToUnicode(const char *buffer);
};

namespace ProcessMgr {
    void getThisProcessName(std::wstring &strProcessName);
    void getThisProcessPath(std::wstring &strProcessPath);
    void getProcessIdByName(const std::wstring &strProcessName, std::set<DWORD> &setProcessId);

    bool killProcess(DWORD dwProcessId);
    bool isRepeted();
    bool isRepeted(const std::wstring &strProcessName);
}