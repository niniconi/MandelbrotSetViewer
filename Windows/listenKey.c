#pragma once
#include <windows.h>
#include <stdio.h>

HANDLE hStdin;
DWORD fdwSaveOldMode;
DWORD fdwMode;

INPUT_RECORD irInBuf[128];
DWORD cNumRead;
KEY_EVENT_RECORD ker;


void init_listener(){
	// Get the standard input handle. 
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	// Save the current input mode, to be restored on exit. 
	GetConsoleMode(hStdin, &fdwSaveOldMode);
	fdwMode = ENABLE_WINDOW_INPUT;
	SetConsoleMode(hStdin, fdwMode);
}
void close_listener(){
	SetConsoleMode(hStdin, fdwSaveOldMode); 
}
char listener_get_char(){
	ReadConsoleInput(
			hStdin,      // input buffer handle 
			irInBuf,     // buffer to read into 
			128,         // size of read buffer 
			&cNumRead); // number of records read 
	
	for (int i = 0; i < cNumRead; i++)
	{
		switch (irInBuf[i].EventType)
		{
		case KEY_EVENT: // keyboard input
			ker = irInBuf[i].Event.KeyEvent;
			break;
		default:
			break;
		}
	}
	return ker.uChar.UnicodeChar;
}
