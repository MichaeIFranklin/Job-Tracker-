#pragma once
#include "Job Tracker.h"

// external globals
extern UINT NCD_NEW_COMPANY; // message sent from this dialog to tell a parent that a new Company has been created with a LPARAM of the ID of the newly created Company
// Global Functions
void ShowNewCompanyWindow(HWND hWnd);