#include "myrobot.h"

//To debug, or not to debug...
bool bDebug = true;
bool quit = false;
HHOOK g_hKeyboardHook;


MyRobot		*myRobot	= NULL;

bool process = false;


bool is_bit_set(unsigned value, unsigned bitindex)
{
	return (value & (1 << bitindex)) != 0;
}

void sendkeys(void *cBuf2)
{
	if (myRobot)
	{
		myRobot->SendKeys((char*) cBuf2);
	}

	_endthread();
}

/**
This function handles the key events.
**/
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{

	char	tBuf[2048];
	char    cBuf[2048];
	char    cBuf2[2048];

	ZeroMemory(tBuf, sizeof(char) * 2048);
	ZeroMemory(cBuf, sizeof(char) * 2048);
	ZeroMemory(cBuf2, sizeof(char) * 2048);

	LPKBDLLHOOKSTRUCT str = NULL;
	str = (LPKBDLLHOOKSTRUCT)lParam;

	if (str->vkCode == VK_ESCAPE)
	{
		//esc = (wParam == WM_KEYDOWN);
		if (g_hKeyboardHook) UnhookWindowsHookEx(g_hKeyboardHook);
		g_hKeyboardHook = NULL;

		printf("quitting...\n"); fflush(stdout);
		quit = true;
		exit(0);
		return true;
	}

	if (process)
	{
		return false;
	}

	if (wParam == WM_KEYDOWN)
	{
		char cPressed[3];
		ZeroMemory(cPressed, sizeof(char) * 3);

		cPressed[0] = (char)MapVirtualKey(str->vkCode, MAPVK_VK_TO_CHAR);

		switch (str->vkCode)
		{
		case VK_F1:
			strcpy_s(cPressed, "F1\0");
			break;
		case VK_F2:
			strcpy_s(cPressed, "F2\0");
			break;
		case VK_F3:
			strcpy_s(cPressed, "F3\0");
			break;
		case VK_F4:
			strcpy_s(cPressed, "F4\0");
			break;
		case VK_F5:
			strcpy_s(cPressed, "F5\0");
			break;
		case VK_F6:
			strcpy_s(cPressed, "F6\0");
			break;
		case VK_F7:
			strcpy_s(cPressed, "F7\0");
			break;
		case VK_F8:
			strcpy_s(cPressed, "F8\0");
			break;
		case VK_F9:
			strcpy_s(cPressed, "F9\0");
			break;
		case VK_F10:
			strcpy_s(cPressed, "F10\0");
			break;
		case VK_F11:
			strcpy_s(cPressed, "F11\0");
			break;
		case VK_F12:
			strcpy_s(cPressed, "F12\0");
			break;
		default:
			break;
		}

		sprintf_s(cBuf, "Pressed a Vcode: '%i' : '%s'\n", str->vkCode, cPressed);
		printf(cBuf); fflush(stdout);

		sprintf_s(cBuf, "%s\0", cPressed);
		

		GetPrivateProfileStringA("MyRobot", cBuf, "", cBuf2, sizeof(char) * 2048, ".\\MyRobot.ini");
		if (strlen(cBuf2) > 0)
		{
			sprintf_s(cBuf, "\tProcessing...key..Value: '%s'\n", cBuf2);
			printf(cBuf); fflush(stdout);
			process = true;
		}


		//NOW DO SENDKEYS...
		if (process)
		{
			if (g_hKeyboardHook) UnhookWindowsHookEx(g_hKeyboardHook);
			g_hKeyboardHook = NULL;
			
			_beginthread(sendkeys, 0, cBuf2);

			printf("processing done...\n"); fflush(stdout);
			quit = true;
			return true;
		}
	}
	return false;
}


int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	myRobot = new MyRobot();

	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);


	//keybd_event((byte) 'P', 0, 0, 0);
	//Sleep(10);
	//keybd_event((byte) 'P', 0, KEYEVENTF_KEYUP, 0);

	g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);

	MSG msg;
	int bRet = 0;
	while (!quit && (bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		Sleep(1);

		if (bRet == -1)
		{
			// handle the error and possibly exit
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	ShowCursor(TRUE);

	if (g_hKeyboardHook) UnhookWindowsHookEx(g_hKeyboardHook);

	SAFE_DEL(myRobot);
	return 0;

}

void MyRobot::SendKeys(LPSTR KeysToSend)
{
	char tBuf[2048];
	char tempChar = '\0';

	ZeroMemory(tBuf, sizeof(char) * 2048);
	sprintf_s(tBuf, "MyRobot::SendKeys:-- START:, KeysToSend:= '%s'\n", KeysToSend);

	for (int i = 0; i < strlen(KeysToSend); i++)
	{
		char toSend = KeysToSend[i];
		SHORT vCode = VkKeyScanA(toSend);
		byte lowBit = LOBYTE(vCode);
		byte hiBit = HIBYTE(vCode);

		bool processKey = true;

		if (toSend == '{')
		{
			tempChar = KeysToSend[i + 1];
			i += 2;
			if (tempChar == 'E')
			{
				vCode = VK_RETURN;
				lowBit = VK_RETURN;
				hiBit = 0;
			}
			if (tempChar == 'T')
			{
				vCode = VK_TAB;
				lowBit = VK_TAB;
				hiBit = 0;
			}

			//Sleep window, then send keys... for sneak VM's with no COPY/PASTE  mechanism.
			if (tempChar == 'S')
			{
				processKey = false;
				Sleep(1000);
			}
		}


		if (processKey)
		{

			bool shift = ((hiBit & 0x00000001) == 1);
			sprintf_s(tBuf, "\t (LowBit: '%i', HiBit: '%i') vKey: '%c', vCode: '%i', shift: '%s'\n", lowBit, hiBit, toSend, vCode, (shift ? "Yes" : "No"));
			printf(tBuf); fflush(stdout);

			if (shift) keybd_event(VK_SHIFT, 0, 0, 0);
			keybd_event(lowBit, 0, 0, 0);
			Sleep(1);
			keybd_event(lowBit, 0, KEYEVENTF_KEYUP, 0);
			if (shift) keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
		}

	}

	printf(tBuf); fflush(stdout);
}


void MyRobot::init()
{
}


MyRobot::MyRobot()
{
	init();
	//Create here...
	printf("MyRobot::MyRobot:-- Constructor.\n"); fflush(stdout);
}

MyRobot::~MyRobot()
{
	//Destroy here...
	printf("MyRobot::~MyRobot:-- Destructor.\n"); fflush(stdout);
	init();
}