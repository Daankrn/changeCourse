//SOURCE: https://gist.github.com/t-mat/1391291
//adapted by Daan Krijnen
#pragma once
#include <windows.h>
#include <stdio.h>
#include "ofMain.h"
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class DiJoyStick
{public:
    struct Entry {
        DIDEVICEINSTANCE        diDeviceInstance;
        DIDEVCAPS               diDevCaps;
        LPDIRECTINPUTDEVICE8    diDevice;
        DIJOYSTATE2             joystate;
    };
 
    DiJoyStick();
 
    ~DiJoyStick();


	void clear();
	void enumerate(LPDIRECTINPUT, DWORD dwDevType = DI8DEVCLASS_GAMECTRL, LPCDIDATAFORMAT lpdf = &c_dfDIJoystick2, DWORD dwFlags = DIEDFL_ATTACHEDONLY, int maxEntry = 16);
	int getEntryCount();
	const Entry* getEntry(int);
	void update();
	void run();


	//force feedback
	INT CoordToForce( INT , INT);
HRESULT SetDeviceForcesXY(int, int);
INT                     g_nXForce;
LPDIRECTINPUTEFFECT     g_pEffect;


protected:
	 static BOOL CALLBACK DIEnumDevicesCallback_static(LPCDIDEVICEINSTANCE, LPVOID );
	  BOOL DIEnumDevicesCallback(LPCDIDEVICEINSTANCE, LPVOID);
	  Entry*          entry;
    int             maxEntry;
    int             nEntry;
    LPDIRECTINPUT   di;
    LPCDIDATAFORMAT lpdf;


};

