#pragma once
/*
	Author: Alwyn Dippenaar (alwyn.j.dippenaar@gmail.com)
	This class is responsible for providing robot like functionality to simulate keysend
	events to the Desktop.

*/

#ifndef _MYROBOT_H
#define _MYROBOT_H

#pragma once

#include <winsock2.h>


#include <windows.h>
#include <tchar.h>
#include <vector>
#include <ctime>
#include <cctype>

#include <ws2tcpip.h>
#include <iphlpapi.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#ifndef SAFE_DEL
#define SAFE_DEL(obj) if (obj) delete obj; obj = NULL;
#endif

#include <process.h>

///Represents the robot class.
class MyRobot
{
public:
	MyRobot();
	~MyRobot();

	void init();
	void SendKeys(LPSTR KeysToSend);

};



#endif