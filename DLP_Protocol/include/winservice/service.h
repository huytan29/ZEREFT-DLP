#ifndef SERVICE_H
#define SERVICE_H
/*A callback function that will be called by the SCM to handle SCM requests */
void stop_module();
/*Start run main code*/
void start();
int CheckingServiceRunning();
/*Install LancsDLP service*/
int InstallService(char *path);
int UninstallService();
int start_service();
int stop_service();
int run_service(int argc,char **argv);

#endif
