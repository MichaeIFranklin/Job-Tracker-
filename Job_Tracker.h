#pragma once
#pragma comment(lib,"comctl32.lib")
#include <windows.h>

#include "resource.h"
#include <string>
#include "TrackerData.h"
#include <Commctrl.h>

extern HINSTANCE hInst;  // current instance
extern UINT UPDATE_LIST; // message sent to the main window to update positions list