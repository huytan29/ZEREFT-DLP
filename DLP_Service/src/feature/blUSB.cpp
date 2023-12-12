#include<stdio.h>
#include<cstdlib>
#include "../headers/main.h"


int blockUSB(){

	system("COLOR 9");
     printf("1: Block Port USB \n");
     printf("2: Unblock Port USB \n");
     printf("3: Exit \n");

     printf(" Put your choice: \n");
     char c;
     scanf("%c",&c);
	 system("cls");

     switch (c){
	 
       case '1': system("reg add HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\USBSTOR \/v Start \/t REG_DWORD \/d 4 \/f");
            break;
			
       case '2': system("reg add HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\USBSTOR \/v Start \/t REG_DWORD \/d 3 \/f");
            break;
			
       case '3': system("EXIT");
            break;
			
      default :printf(" Error , choose again \n" );
	 }
      return 0;
}
