#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <Psapi.h>
#include <fwpmtypes.h>
#include <fwpmu.h>
#include <fwptypes.h>
#include <mstcpip.h>
#include <winapifamily.h>

#include <iostream>
#include <string>
#include <vector>
#include <wchar.h>
#include <stdio.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Fwpuclnt.lib")
#pragma comment(lib, "advapi32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS


void blockIPv4();
void blockIPv6();
void blockIPDv4();
void unblockIPDv4();
void PrintIPAndProcess(DWORD processId, std::string ipAddress, std::string hostName);
int blockWebsites();