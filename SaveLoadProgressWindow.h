#pragma once
#include "Job Tracker.h"

// external globals
extern UINT SLP_SAVELOAD_COMPLETE; // message sent from this dialog to tell a parent that the saving or loading of data is complete
								   // Global Functions
void ShowSaveLoadProgressWindow(HWND hWnd, bool Save); // save is true when we are saving data to file, send the SLP_SAVELOAD_COMPLETE message if this is true