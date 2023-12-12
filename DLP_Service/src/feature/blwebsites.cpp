#include "../headers/BlWebsites/header.h"
#pragma comment(lib, "Fwpuclnt.lib")


const GUID PROVIDER_KEY =
{
   0x5fb216a8,
   0xe2e8,
   0x4024,
   { 0xb8, 0x53, 0x39, 0x1a, 0x41, 0x68, 0x64, 0x1e }
};

void blockIPv4() {
	FWPM_FILTER0  fwpFilter;
	FWP_IP_VERSION_ fwpIPfilter;
	FWPM_SUBLAYER0 fwpFilterSubLayer;
	HANDLE engineHandle = NULL;
	//DWORD result;

	RtlZeroMemory(&fwpFilter, sizeof(FWPM_FILTER0));

	//fwpFilter.layerKey = ;
	//fwpIPfilter = FWP_IP_VERSION_V4;
	fwpFilter.filterId = FWP_IP_VERSION_V4;
	fwpFilter.action.type = FWP_ACTION_BLOCK;

	if (&fwpFilterSubLayer.subLayerKey != NULL)
	{
		fwpFilter.subLayerKey = fwpFilterSubLayer.subLayerKey;

		fwpFilter.weight.type = FWP_EMPTY;
		fwpFilter.numFilterConditions = 0;
		wchar_t acceptBlock[] = L"Accept Block";
		wchar_t filtertoBlock[] = L"Filter to block all inbound connections";
		fwpFilter.displayData.name = acceptBlock;
		fwpFilter.displayData.description = filtertoBlock;

		printf("Adding filter to block all inbound connection. \n");
		DWORD result = FwpmEngineOpen0(NULL, RPC_C_AUTHN_WINNT, NULL, NULL, &engineHandle);
		if (result != ERROR_SUCCESS)
			printf("FwpmFilterAdd0 failed. Return value: %d.\n", result);
		else
			printf("Filter added successfully.\n");


		// Close the engine handle when done
		FwpmEngineClose0(engineHandle);
	}
}

void blockIPv6() {
	FWPM_FILTER0  fwpFilter;
	FWPM_SUBLAYER0 fwpFilterSubLayer;
	HANDLE engineHandle = NULL;
	//DWORD result;

	RtlZeroMemory(&fwpFilter, sizeof(FWPM_FILTER0));

	fwpFilter.filterId = FWP_IP_VERSION_V6;
	fwpFilter.action.type = FWP_ACTION_BLOCK;

	if (&fwpFilterSubLayer.subLayerKey != NULL)
	{
		fwpFilter.subLayerKey = fwpFilterSubLayer.subLayerKey;

		fwpFilter.weight.type = FWP_EMPTY;
		fwpFilter.numFilterConditions = 0;
		wchar_t acceptBlock[] = L"Accept Block";
		wchar_t filtertoBlock[] = L"Filter to block all inbound connections";
		fwpFilter.displayData.name = acceptBlock;
		fwpFilter.displayData.description = filtertoBlock;

		printf("Adding filter to block all inbound connection. \n");
		DWORD result = FwpmEngineOpen0(NULL, RPC_C_AUTHN_WINNT, NULL, NULL, &engineHandle);
		if (result != ERROR_SUCCESS)
			printf("FwpmFilterAdd0 failed. Return value: %d.\n", result);
		else
			printf("Filter added successfully.\n");


		// Close the engine handle when done
		FwpmEngineClose0(engineHandle);
	}
}


// Hàm để chặn tất cả lưu lượng TCP
void blockIPDv4() {
    // Tạo một rule Windows Firewall để chặn tất cả lưu lượng TCP
    system("netsh advfirewall firewall add rule name=\"Block All TCP\" dir=out action=block protocol=TCP");
    system("netsh advfirewall firewall add rule name=\"Block All TCP\" dir=in action=block protocol=TCP");

	// Chặn tắt Windows Defender Firewall bằng cách thay đổi Registry
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\SharedAccess\\Parameters\\FirewallPolicy\\StandardProfile", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        DWORD value = 0; // 0 là chặn, 1 là mở
        RegSetValueExW(hKey, L"EnableFirewall", 0, REG_DWORD, (BYTE*)&value, sizeof(value));
        RegCloseKey(hKey);
    }
}

// Hàm để mở chặn tất cả lưu lượng TCP
void unblockIPDv4() {
    // Xóa rule Windows Firewall chặn tất cả lưu lượng TCP
    system("netsh advfirewall firewall delete rule name=\"Block All TCP\"");

	// Mở chặn Windows Defender Firewall bằng cách thay đổi Registry
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services\\SharedAccess\\Parameters\\FirewallPolicy\\StandardProfile", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        DWORD value = 1; // 0 là chặn, 1 là mở
        RegSetValueExW(hKey, L"EnableFirewall", 0, REG_DWORD, (BYTE*)&value, sizeof(value));
        RegCloseKey(hKey);
    }
}


void PrintIPAndProcess(DWORD processId, std::string ipAddress, std::string hostName) {
    char processName[MAX_PATH] = "<unknown>";

    HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
    if (processHandle != NULL) {
        if (GetModuleFileNameExA(processHandle, NULL, processName, MAX_PATH) == 0) {
            std::cerr << "GetModuleFileNameExA failed with error: " << GetLastError() << std::endl;
        }
        CloseHandle(processHandle);
    }

    std::cout << "IP Address: " << ipAddress << " | Host Name: " << hostName << " | Process Name: " << processName << std::endl;
}


int blockWebsites() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }

    bool isRunning = false; // Sử dụng biến này để theo dõi trạng thái chương trình

    while (true) {
        std::string userInput;
        std::cout << "Enter ON to start or OFF to exit: ";
        std::cin >> userInput;
        if (userInput == "ON") {
            isRunning = true;
            blockIPDv4();
            blockIPv6();
            blockIPv4();
        }
        else if (userInput == "OFF") {
            unblockIPDv4();
            break; // Thoát khỏi vòng lặp và kết thúc chương trình
        }

        while (isRunning) {
            MIB_TCPTABLE_OWNER_PID* pTcpTable = NULL;
            DWORD dwSize = 0;
            DWORD dwRetVal = 0;
            blockIPv4();
            blockIPv6();

            if (GetExtendedTcpTable(pTcpTable, &dwSize, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0) == ERROR_INSUFFICIENT_BUFFER) {
                pTcpTable = (MIB_TCPTABLE_OWNER_PID*)malloc(dwSize);
                if (pTcpTable == NULL) {
                    std::cerr << "Memory allocation failed." << std::endl;
                    return 1;
                }
            }

            if ((dwRetVal = GetExtendedTcpTable(pTcpTable, &dwSize, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0)) == NO_ERROR) {
                std::cout << "###################################################################################################################################" << std::endl;
                std::cout << "###################################################################################################################################" << std::endl;
                std::cout << "#################################LIST IP ADDRESS AND PROCESS NAME FOR DLP PROJECT##################################################" << std::endl;
                std::cout << "###################################################################################################################################" << std::endl;
                std::cout << "###################################################################################################################################" << std::endl;
                for (DWORD i = 0; i < pTcpTable->dwNumEntries; i++) {
                    DWORD processId = pTcpTable->table[i].dwOwningPid;
                    char ipAddress[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &(pTcpTable->table[i].dwRemoteAddr), ipAddress, INET_ADDRSTRLEN);

                    // Lấy tên host dựa trên địa chỉ IP
                    char hostName[NI_MAXHOST] = "";
                    struct sockaddr_in sa;
                    sa.sin_family = AF_INET;
                    inet_pton(AF_INET, ipAddress, &(sa.sin_addr));

                    getnameinfo((struct sockaddr*)&sa, sizeof(sa), hostName, sizeof(hostName), NULL, 0, 0);

                    PrintIPAndProcess(processId, ipAddress, hostName);
                }
                Sleep(2000);
            }
            else {
                std::cerr << "GetExtendedTcpTable failed with error: " << dwRetVal << std::endl;
            }

            if (pTcpTable != NULL) {
                free(pTcpTable);
            }

            // Chờ 2 giây trước khi cập nhật lại
            Sleep(2000);
        }
    }

    WSACleanup();
    return 1;
}