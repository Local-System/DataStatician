#define UNICODE
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <windows.h>
#include <conio.h>
#include <time.h>
using namespace std;
void* hConsole = GetStdHandle(4294967285);
wfstream fsIO;
wstring szDir;
#define KEY_DOWN(VK) ((GetAsyncKeyState(VK) & 0x8000) ? 1:0)
#define KEY_INPUT(VK) (GetAsyncKeyState(VK) == (short)0x8001)
#include "Utils/FileQuery.cpp"
#include "Utils/clrscr.cpp"
#include "Utils/coord.cpp"
#include "Utils/VerifyGUID.cpp"
