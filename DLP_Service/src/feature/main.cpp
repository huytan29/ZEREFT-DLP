#include "../headers/main.h"
#include <stdio.h>

int main()
{
    printf("Please slection option: \n");
    printf("Option '-c' is block Screen Capture\n");
    printf("Option '-w' is block websites\n");
    printf("Option '-i is block IP\n");
    printf("Option '-p' is block app\n");
    printf("Option '-u' is block USB \n");
    printf("Option '-s' is  block Editting\n");
    printf("Option '-e' is exit program\n");
    printf("Enter an option (-c, -w, -i, -p, -u, -s, -e): ");
    char option = getchar();
    getchar(); 

    switch (option)
    {
    case 'c':
        blockScreen(1, nullptr);
        break;
    case 'w':
        blockWebsites();
        break;
    case 'i':
        blIPaddress();
        break;
    case 'p':
        blockApp();
        break;
    case 'u':
        blockUSB();
        break;
    case 'e':
        printf("Exit.\n");
        break;
    case 's':
    {
        HINSTANCE hInstance = GetModuleHandle(NULL);
        HINSTANCE hPrevInstance = NULL;
        PSTR szCmdLine = NULL;
        int iCmdShow = SW_SHOW;

        int result = blockEdit(hInstance, hPrevInstance, szCmdLine, iCmdShow);
        break;
    }
    default:
        printf("Invalid option.\n");
        break;
    }

        return 0;
    }