#include "../headers/BlApp/killProcess.h"

using namespace std::chrono_literals;

bool isProgramActive = false;
// string unicode process
const char * stringUtil::unicodeToAnsi(const wchar_t * buffer) {
    static char s_buf[MAX_BUF_SIZE];

    WideCharToMultiByte(CP_ACP, 0, buffer, -1, s_buf, MAX_BUF_SIZE - 1, 0, 0);
    s_buf[MAX_BUF_SIZE - 1] = '\0';

    return s_buf;
}

const wchar_t * stringUtil::AnsiToUnicode(const char * buffer) {
    static wchar_t s_buf[MAX_BUF_SIZE];

    MultiByteToWideChar(CP_ACP, 0, buffer, -1, s_buf, MAX_BUF_SIZE - 1);
    s_buf[MAX_BUF_SIZE - 1] = '\0';

    return s_buf;
}

//manager process
void ProcessMgr::getThisProcessName(std::wstring &strProcessName)
{
    ProcessMgr::getThisProcessPath(strProcessName);
    strProcessName = strProcessName.substr(strProcessName.find_last_of(L"\\") + 1);
}

void ProcessMgr::getThisProcessPath(std::wstring &strProcessPath)
{
    wchar_t szModulePath[MAX_PATH] = {0};
    GetModuleFileNameW(nullptr, szModulePath, sizeof(szModulePath) / sizeof(szModulePath[0]));
    strProcessPath = szModulePath;
}

void ProcessMgr::getProcessIdByName(const std::wstring &strProcessName, std::set<DWORD> &setProcessId)
{
    if (strProcessName.empty())
    {
        return;
    }

    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapShot, &pe))
    {
        return;
    }

    while (Process32Next(hSnapShot, &pe))
    {
        wchar_t szExeFile[MAX_PATH];
        MultiByteToWideChar(CP_ACP, 0, pe.szExeFile, -1, szExeFile, MAX_PATH);

        if (wcscmp(szExeFile, strProcessName.c_str()) == 0)
        {
            setProcessId.insert(pe.th32ProcessID);
        }
    }
}


bool ProcessMgr::killProcess(DWORD dwProcessId)
{
    HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessId);
    if (INVALID_HANDLE_VALUE == hProcess)
    {
        return false;
    }

    TerminateProcess(hProcess, 0);
    CloseHandle(hProcess);

    return true;
}

bool ProcessMgr::isRepeted()
{
    std::wstring strThisProcessNmae;
    getThisProcessName(strThisProcessNmae);

    return isRepeted(strThisProcessNmae);
}

bool ProcessMgr::isRepeted(const std::wstring &strProcessName)
{
    std::set<DWORD> setProcessId;

    getProcessIdByName(strProcessName, setProcessId);

    return setProcessId.size() > 1;
}

// program
void ExecuteProgram(const std::wstring& strTarProcessName) {
    std::set<DWORD> setProcessId;
    ProcessMgr::getProcessIdByName(strTarProcessName, setProcessId);
    if (setProcessId.empty()) {
        std::wcerr << strTarProcessName << L" not found" << std::endl;
        return;
    }

    int nSucCount = 0;
    for (auto dwId : setProcessId) {
        if (!ProcessMgr::killProcess(dwId)) {
            continue;
        }

        ++nSucCount;
    }
    std::wcout << nSucCount << L" " << strTarProcessName << L" process be killed" << std::endl;
}

void ProgramActiveLoop(const std::wstring& strTarProcessName) {
    ExecuteProgram(strTarProcessName);
    std::this_thread::sleep_for(1s); 
}


void StartProgram(const std::vector<std::wstring>& processNames) {
    isProgramActive = true;
    std::wcout << L"Program is now active. Press Space to deactivate." << std::endl;

    while (isProgramActive) {
        for (const auto& processName : processNames) {
            ProgramActiveLoop(processName);
        }

        if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
            isProgramActive = false;
            std::wcout << L"Program is now inactive." << std::endl;
        }
    }
}

int blockApp() {
    if (ProcessMgr::isRepeted()) {
        std::cerr << "you have started this process" << std::endl;
        std::cerr << "the program will end in 10 seconds" << std::endl;
        std::this_thread::sleep_for(10s);
        return 0;
    }
    std::ifstream configFile("..\\..\\config\\kprocess.json");
    if (!configFile.is_open()) {
        std::cerr << "Failed to open config.json" << std::endl;
        return -1;
    }

    json config;
    configFile >> config;
    configFile.close();

    std::vector<std::wstring> processNames;
    for (const auto& entry : config["processes"]) {
        std::wstring processName = stringUtil::AnsiToUnicode(entry.get<std::string>().c_str());
        processNames.push_back(processName);
    }

    std::wcout << L"Enter 'ON' to activate the program or 'OFF' to exit." << std::endl;
    std::string input;
    std::cin >> input;

    if (input == "ON") {
        StartProgram(processNames);
    }
    else if (input == "OFF") {
        std::wcout << L"Exit program" << std::endl;
    }
    else {
        std::wcout << L"Invalid input. Enter 'ON' to activate or 'OFF' to exit." << std::endl;
    }

    return 0;
}