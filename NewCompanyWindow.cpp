#include "NewCompanyWindow.h"

// Globals
HWND CNParentHwnd; // used to send data back to parent if need be
UINT NCD_NEW_COMPANY;

// Function Declorations
INT_PTR CALLBACK    NewCompany(HWND, UINT, WPARAM, LPARAM);


void ShowNewCompanyWindow(HWND hWnd)
{
	CNParentHwnd = hWnd;
	DialogBox(hInst, MAKEINTRESOURCE(IDD_NEW_COMPANY), hWnd, NewCompany);
}

void SetupNewCompanyWindow()
{
	// Register new log message
	NCD_NEW_COMPANY = RegisterWindowMessage("New_Company_Created");
}
// Message handler
INT_PTR CALLBACK NewCompany(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		SetupNewCompanyWindow();
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDSAVE)
		{
			std::string CompanyName;
			std::string CompanyLocation;
			std::string CompanyPhone;
			std::string CompanyDescription;
			int bufferlen;
			std::vector<char> buffer;
			
			// Process Company
			// Get Windows
			HWND CompnayNameHwnd = GetDlgItem(hDlg, IDC_COMPANY_NAME_EDIT);
			HWND CompnayLocationHwnd = GetDlgItem(hDlg, IDC_COMPANY_LOCATION_EDIT);
			HWND CompnayPhoneHwnd = GetDlgItem(hDlg, IDC_COMPANY_PHONE_EDIT);
			HWND CompnayDescriptionHwnd = GetDlgItem(hDlg, IDC_COMPANY_DESCRIPTION_EDIT);
			// Get Data
			bufferlen = GetWindowTextLength(CompnayNameHwnd) + 1;
			buffer.resize(bufferlen);
			GetWindowText(CompnayNameHwnd, (LPSTR)&buffer[0], bufferlen);
			CompanyName = &buffer[0];
			buffer.clear();
			bufferlen = GetWindowTextLength(CompnayLocationHwnd) + 1;
			buffer.resize(bufferlen);
			GetWindowText(CompnayLocationHwnd, (LPSTR)&buffer[0], bufferlen);
			CompanyLocation = &buffer[0];
			buffer.clear();
			bufferlen = GetWindowTextLength(CompnayPhoneHwnd) + 1;
			buffer.resize(bufferlen);
			GetWindowText(CompnayPhoneHwnd, (LPSTR)&buffer[0], bufferlen);
			CompanyPhone = &buffer[0];
			buffer.clear();
			bufferlen = GetWindowTextLength(CompnayDescriptionHwnd) + 1;
			buffer.resize(bufferlen);
			GetWindowText(CompnayDescriptionHwnd, (LPSTR)&buffer[0], bufferlen);
			CompanyDescription = &buffer[0];

			// validate Data
			if (CompanyName == "" || CompanyLocation == "" || CompanyPhone == "" || CompanyDescription == "")
			{
				// show error
				MessageBox(NULL, "All fields are required", "Info", MB_ICONERROR | MB_OK);
			}
			else
			{
				// Add new Company
				int NewID = trackerData.CreateCompany(CompanyName, CompanyLocation, CompanyPhone, CompanyDescription);
				// send data back to paraent
				SendMessage(CNParentHwnd, NCD_NEW_COMPANY, 0, (LPARAM)NewID);
				// leave Dialog
				EndDialog(hDlg, LOWORD(wParam));
			}
		}
		break;
	}
	return (INT_PTR)FALSE;
}
	