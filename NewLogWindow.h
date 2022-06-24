#pragma once
#include "Job Tracker.h"

// Global Functions
extern UINT NLD_NEW_LOG; // message sent from this dialog to tell a parent that a new Log has been created with a LPARAM of a boolean identifying if the creation was a success or not
void ShowNewLogWindow(HWND hWnd, int PositionID);