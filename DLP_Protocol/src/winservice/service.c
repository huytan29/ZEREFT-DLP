#ifdef WIN32
#include <windows.h>
#include <winsvc.h>
#include <stdio.h>
#include "service.h"
static LPTSTR serviceName = "LancsDLP";
static LPTSTR serviceDisplayName = "LancsDLP";
static LPTSTR serviceDescription = "Lancs Windows DLP Agent";

static SERVICE_STATUS_HANDLE serviceHandle;
static SERVICE_STATUS serviceStatus;

#define UNUSED(A) (void)(A)
/*A callback function that will be called by the SCM to handle SCM requests */
void stop_module()
{
    printf("Module is stopped\n");
}
/*Start run main code*/
void start()
{
    printf("service is start\n"); 
}
int stop_service()
{
    int rc = 0;
    SC_HANDLE scManager,scService;
    scManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
    if(scManager)
    {
        scService = OpenService(scManager,serviceName,SC_MANAGER_ALL_ACCESS);
        if(scService)
        {
            SERVICE_STATUS serviceStatus;
            if(ControlService(scService,SERVICE_CONTROL_STOP,&serviceStatus))
            {
                rc = 1;
            }
            CloseServiceHandle(scService);
        }
        CloseServiceHandle(scManager);
    }
    /*Sleep for a short period of time to avoid possible race condition with newer instance of dlp-agent*/
    Sleep(300);
    return (rc);
}
int start_service()
{
    int rc = 0;
    SC_HANDLE scManager,scService;
    scManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
    if(scManager)
    {
        scService = OpenService(scManager,serviceName,SC_MANAGER_ALL_ACCESS);
        if(scService){
            if(StartService(scService,0,NULL)){
                rc = 1;
            }else{
                if(GetLastError() == ERROR_SERVICE_ALREADY_RUNNING)
                {
                    rc = -1;
                }
            }
            CloseServiceHandle(scService);
        }
        CloseServiceHandle(scManager);
    }
    return (rc);
}



/*Checking if agent service is running
  Return 1 on running or 0 if not running
*/
int CheckingServiceRunning()
{
    int rc = 0;
    SC_HANDLE scService,scManager;
    scManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
    if(scManager)
    {
        scService = OpenService(scManager,serviceName,SC_MANAGER_ALL_ACCESS);
        if(scService)
        {
            /*check Status*/
            SERVICE_STATUS serviceStatus;
            if(QueryServiceStatus(scService,&serviceStatus))
            {
                if(serviceStatus.dwCurrentState == SERVICE_RUNNING)
                {
                    rc = 1;
                }
            }
            CloseServiceHandle(scService);
        }
        CloseServiceHandle(scManager);
    }
    return 0;
}


/*Install LancsDLP service*/
int InstallService(char *path)
{
    int ret;
    SC_HANDLE scManager,scService;
    LPCTSTR BinaryPathName = NULL;
    SERVICE_DESCRIPTION svdesc;
    /*Uninstall service if it exists*/
    /*
    if(!UninstallService())
    {
      
        fprintf(stderr,"Failure running UninstallService.");
        return 0;
    }
    */
    /*Executable path - must be called with full path*/
    BinaryPathName = path;
    /*Open service database*/
    scManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
    if(scManager == NULL){
        goto install_error;
    }
    /*Create new service*/
    scService = CreateService( scManager,
                               serviceName,
                               serviceDisplayName,
                               SERVICE_ALL_ACCESS,
                               SERVICE_WIN32_OWN_PROCESS,
                               SERVICE_AUTO_START,
                               SERVICE_ERROR_NORMAL,
                               BinaryPathName,
                               NULL,NULL,NULL,NULL,NULL

    );
    if(scService == NULL)
    {
        CloseServiceHandle(scManager);
        goto install_error;
    }
    /*If create service successfully, we set description for service*/
    svdesc.lpDescription = serviceDescription;
    ret = ChangeServiceConfig2(scService,SERVICE_CONFIG_DESCRIPTION,&svdesc);
    CloseServiceHandle(scService);
    CloseServiceHandle(scManager);
    /*Check for error*/
    if(!ret)
    {
        goto install_error;
    }
    /*Log system*/
    printf("Succesfully added to the service database.");
    return 1;


install_error: {
    char local_msg[1025];
    LPVOID msgBuf;
    memset(local_msg,0,1025);
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | // allocate buffer to msg
                  FORMAT_MESSAGE_FROM_SYSTEM | // retrieve system error msg
                  FORMAT_MESSAGE_IGNORE_INSERTS, //ignore msg insert sequences.
                  NULL,
                  GetLastError(),//error code or msg id 
                  MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
                  (LPTSTR)&msgBuf,
                  0,
                  NULL
                  );
    /*Log system*/
    fprintf(stderr,"Unable to create service: %s",(LPTSTR)msgBuf);
    return 0;
    }

}
VOID WINAPI serviceHandler(DWORD dwOpcode)
{
     if(serviceHandle)
     {
        switch(dwOpcode)
        {
            case SERVICE_CONTROL_STOP: // receive stop signal from SCM
                serviceStatus.dwWin32ExitCode = 0;
                serviceStatus.dwCheckPoint = 0;
                serviceStatus.dwWaitHint = 0;
                /*Write a notificaition to log*/
                /*Set current status to pending in order to stop modules before set status to stop*/
                serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
                SetServiceStatus(serviceHandle,&serviceStatus);
                /*Stop process, childprocess and cleanup */
                stop_module();
                /*Set current status to stop. Completely stop service*/
                serviceStatus.dwCurrentState = SERVICE_STOPPED;
                SetServiceStatus(serviceHandle,&serviceStatus);
            break;
        }
     }
}
void WINAPI serviceStart(DWORD argc,LPTSTR *argv)
{
    UNUSED(argc);
    UNUSED(argv);
    serviceStatus.dwServiceType = SERVICE_WIN32;
    serviceStatus.dwCurrentState = SERVICE_START_PENDING;
    serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP; //
    serviceStatus.dwWin32ExitCode = 0; //set 0 for success and non-zero for an error.
    serviceStatus.dwServiceSpecificExitCode = 0; 
    serviceStatus.dwCheckPoint = 0;
    serviceStatus.dwWaitHint = 0;
    serviceHandle = RegisterServiceCtrlHandler(serviceName,serviceHandler);//will be called by SCM to handle control requests: start,stop,pause,..
    if(serviceHandle == (SERVICE_STATUS_HANDLE)0)
    {
        /*error log*/
        fprintf(stderr,"RegisterServiceCtrlHandler failed.");
        return;
    }
    serviceStatus.dwCurrentState = SERVICE_RUNNING;
    serviceStatus.dwCheckPoint = 0;
    serviceStatus.dwWaitHint = 0;
    /*set status*/
    if(!(SetServiceStatus(serviceHandle,&serviceStatus)))
    {
         /*error log*/
        fprintf(stderr,"SetServiceStatus Error.");
        return;
    }
    /*Entry point to start execute process*/
    start();
}
/*Uninstalled the DLP agent service
success return 1 or fail return 0;
*/
int UninstallService()
{
    int ret;
    int rc = 0;
    SC_HANDLE scManager,scService;
    SERVICE_STATUS ServiceStatus;
    /*Remove from the service database*/
    scManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
    if(scManager)
    {
        scService = OpenService(scManager,serviceName,SERVICE_STOP | DELETE);
        if(scService)
        {
            if(CheckingServiceRunning())
            {
                /*log infor*/
                printf("Find (%s) service is running and going to stop it!\n",serviceName);
                ret = ControlService(scService,SERVICE_CONTROL_STOP,&ServiceStatus);
                if(!ret){
                    printf("Fail to stop service (%s) before removing it!\n",serviceName);/*Log info*/
                }else{
                    printf("Successfully stopped (%s)!\n",serviceName);
                }
            }else{
                printf("Found (%s) service is not running!\n",serviceName);
                ret = 1;
            }
            if(ret && DeleteService(scService))
            {
                printf("Successfully removed (%s) from the service database!\n",serviceName);
                rc = 1;
            }
            CloseServiceHandle(scService);
        }
        else{
            printf("Service does not exist (%s) nothing to delete!\n",serviceName);
            rc = 1;
        }
        CloseServiceHandle(scService);
    }
    if(!rc)
    {
        printf("Failure removing (%s) from the service database\n");
    }
    return rc;
}
/*This is the first part that run in service*/
int run_service(int argc,char **argv)
{
    UNUSED(argc);
    UNUSED(argv);
    SERVICE_TABLE_ENTRY ste[] = {
        {serviceName,serviceStart},
        {NULL,NULL}
    };
    if(!(StartServiceCtrlDispatcher(ste)))
    {
        /*error log*/
        fprintf(stderr,"Unable to set service information.");
        return 1;
    }
    return 1;
}
#endif
/*
#define MAXSTR 1024
int main(int argc,char** argv)
{
    char exe_path[MAXSTR + 1];
    char service_str[MAXSTR + 20];
    memset(exe_path,0,sizeof(exe_path));
    if(GetModuleFileName(NULL,exe_path,MAXSTR) == MAXSTR)
    {
        fprintf(stderr,"Error: Path too long\n");
        return -1;
    }
    snprintf(service_str,sizeof(service_str),"\"%s\"",exe_path );
    printf("path: %s\n",service_str);
    if(argc > 1)
    {
        if(!strcmp(argv[1],"run")){
            start();
            return 0;
        }else if(!strcmp(argv[1],"install")){
            return (InstallService(service_str));
        }else if(!strcmp(argv[1],"uninstall")){
            return (UninstallService());
        }else if(!strcmp(argv[1],"-h")){
           printf("agent-help\n");
        }else{
            printf("Unknown Options: %s\n",argv[1]);
            exit(1);
        }
    }
    if(!run_service(argc,argv))
    {
        fprintf(stderr,"Unable to start run_service");
    }
    return 0;
}
*/