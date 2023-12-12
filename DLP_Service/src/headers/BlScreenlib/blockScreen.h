#pragma once
#include <WinSDKVer.h>
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <psapi.h>
#include <locale>
#include <thread>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <locale>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef _WIN64
#define TARGET_BIT 64
// #define my_strtoul strtoull
#else
#define TARGET_BIT 32
// #define my_strtoul strtoul
#endif

#define _STR(s) #s
#define STR(s) _STR(s)
