#include "SaveLoadProgressWindow.h"


// Globals
HWND SLPParentHwnd;
HWND SaveLoadProgressBarHwnd;

UINT SLP_SAVELOAD_COMPLETE;

bool SaveData; // true when we are saving data to file, send the SLP_SAVELOAD_COMPLETE message if this is true
				   
// Function Declorations
INT_PTR CALLBACK    SaveLoadPregress(HWND, UINT, WPARAM, LPARAM);


// Public Methods
void ShowSaveLoadProgressWindow(HWND hWnd, bool Save)
{
	SLPParentHwnd = hWnd;
	SaveData = Save;

	DialogBox(hInst, MAKEINTRESOURCE(IDD_SAVE_LOAD_DIALOG), hWnd, SaveLoadPregress);
}

int SetupSaveLoadProgressWindow(HWND hWnd, WPARAM wParam)
{
	// Get Windows
	SaveLoadProgressBarHwnd = GetDlgItem(hWnd, IDC_SAVE_LOAD_PROGRESS);

	// Register save load completed message
	if (!SLP_SAVELOAD_COMPLETE)
		SLP_SAVELOAD_COMPLETE = RegisterWindowMessage("Save Load Completed");

	// Call appropreate data function
	if (SaveData)
	{
		// save data passing this dialog's hwnd
		trackerData.SaveTrackerData(hWnd);
	}
	else
	{
		// load data passing this dialog's hwnd
		trackerData.LoadTrackerData(hWnd);
	}

	return (INT_PTR)TRUE;
}



// Message handler for new company window
INT_PTR CALLBACK SaveLoadPregress(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	// handle custom messages
	if (message == TD_DIALOG_UPDATE && message != 0)
	{
		// set the new position of the progress bar
		SendMessage(SaveLoadProgressBarHwnd, PBM_SETPOS, trackerData.LoadCurrent, 0);

		// if the progress bar is full, data operations have finished, close the dialog
		if (trackerData.LoadCurrent >= trackerData.LoadMax)
		{
			// leave Dialog
			EndDialog(hWnd, LOWORD(wParam));

			// check if we were saving data
			if (SaveData)
			{
				// send a message to the parent window that the save operation is completed
				SendMessage(SLPParentHwnd, SLP_SAVELOAD_COMPLETE, 0, 0);
			}
			
		}
	}
	else if (message == TD_DIALOG_SETUP && message != 0)
	{
		// set the new range of the progress bar
		SendMessage(SaveLoadProgressBarHwnd, PBM_SETPOS, trackerData.LoadCurrent, 0);
	}
	else
	{
		switch (message)
		{
		case WM_INITDIALOG:
			return SetupSaveLoadProgressWindow(hWnd, wParam);
		}
	}
	
	return (INT_PTR)FALSE;
}
