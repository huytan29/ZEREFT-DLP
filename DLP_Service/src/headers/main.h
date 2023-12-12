//#ifndef BLOCK_SCREEN_PROGRAM_H
//#define BLOCK_SCREEN_PROGRAM_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <windows.h>

int blockScreen(int argc, char *argv[]);
int blockWebsites();
int blIPaddress();
int blockApp();
int blockUSB();
int blockEdit(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow);

//#endif // BLOCK_SCREEN_PROGRAM_H