#include "NewLogWindow.h"

// Globals
int g_LNPositionID;
HWND LNParentHwnd; // used to send data back to parent if need be
UINT NLD_NEW_LOG;

// Function Declorations
INT_PTR CALLBACK    NewLog(HWND, UINT, WPARAM, LPARAM);


void ShowNewLogWindow(HWND hWnd, int PositionID)
{
	LNParentHwnd = hWnd;
	g_LNPositionID = PositionID;
	DialogBox(hInst, MAKEINTRESOURCE(IDD_NEW_LOG), hWnd, NewLog);
}

void SetupNewLogWindow(HWND hWnd)
{
	// Register new company message
	NLD_NEW_LOG = RegisterWindowMessage("New_Log_Created");
}

// Message handler
INT_PTR CALLBACK NewLog(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		SetupNewLogWindow(hWnd);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)
		{
			// send data back to paraent
			SendMessage(LNParentHwnd, NLD_NEW_LOG, 0, (LPARAM)false);
			SetupNewLogWindow(hWnd);
			EndDialog(hWnd, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDSAVE)
		{
			std::string LogTitle;
			std::string LogDescription;
			std::string LogReminder;
			std::string LogDate;
			std::string LogReminderDate;

			int bufferlen;
			std::vector<char> buffer;


			// Process Company
			// Get Windows
			HWND LogTitleHwnd = GetDlgItem(hWnd, IDC_LOG_TITLE_EDIT);
			HWND LogReminderHwnd = GetDlgItem(hWnd, IDC_LOG_REMINDER_EDIT);
			HWND LogDescriptionHwnd = GetDlgItem(hWnd, IDC_LOG_DESCRIPTION_EDIT);
			HWND LogDateHwnd = GetDlgItem(hWnd, IDC_LOG_DATE_PICKER);
			HWND LogReminderDateHwnd = GetDlgItem(hWnd, IDC_LOG_REMINDER_DATE_PICKER);

			// Get Data	
			bufferlen = GetWindowTextLength(LogDescriptionHwnd) + 1;
			buffer.resize(bufferlen);
			GetWindowText(LogDescriptionHwnd, (LPSTR)&buffer[0], bufferlen);
			LogDescription = &buffer[0];
			buffer.clear();
			bufferlen = GetWindowTextLength(LogReminderHwnd) + 1;
			buffer.resize(bufferlen);
			GetWindowText(LogReminderHwnd, (LPSTR)&buffer[0], bufferlen);
			LogReminder = &buffer[0];
			buffer.clear();
			if (IsDlgButtonChecked(hWnd, IDC_LOG_TITLE_APPLY_CHECKBOX) == BST_CHECKED)
			{
				// Set Title to Application Log Title
				LogTitle = trackerData.LogAppliedTitle;
			}
			else
			{
				// Set Title to User Defined Title
				bufferlen = GetWindowTextLength(LogTitleHwnd) + 1;
				buffer.resize(bufferlen);
				GetWindowText(LogTitleHwnd, (LPSTR)&buffer[0], bufferlen);
				LogTitle = &buffer[0];
			}

			// get Dates
			SYSTEMTIME DateBuffer;
			if (SendMessage(LogDateHwnd, DTM_GETSYSTEMTIME, 0, (LPARAM)&DateBuffer) == GDT_VALID)
			{
				// get String from date
				LogDate = trackerData.ConvertDateToString(DateBuffer);
			}
			else
			{
				LogDate = "";
			}
			if (SendMessage(LogReminderDateHwnd, DTM_GETSYSTEMTIME, 0, (LPARAM)&DateBuffer) == GDT_VALID)
			{
				// get String from date
				LogReminderDate = trackerData.ConvertDateToString(DateBuffer);
			}
			else
			{
				LogReminderDate = "";
			}
			// validate Data
			if (LogTitle == "" || LogDescription == "" || LogReminder == "" || LogDate == "" || LogReminderDate == "")
			{
				// show error
				MessageBox(NULL, "All fields are required", "Info", MB_ICONERROR | MB_OK);
			}
			else
			{
				// Add new Log
				trackerData.CreateLog(g_LNPositionID, LogTitle, LogDescription, LogReminder, LogDate, LogReminderDate);
				// send data back to paraent
				SendMessage(LNParentHwnd, NLD_NEW_LOG, 0, (LPARAM)true);

				// leave Dialog
				EndDialog(hWnd, LOWORD(wParam));
			}
		}
		break;
	}
	return (INT_PTR)FALSE;
}
