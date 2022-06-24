#include "NewPositionWindow.h"
#include "NewCompanyWindow.h"
#include "NewLogWindow.h"

// Globals
std::vector<std::string> PNCompanyComboList;
int PositionID;
HWND PNParentHwnd;
HWND PNDialogHwnd;
WPARAM PNWparam;

// Function Declorations
INT_PTR CALLBACK    NewPosition(HWND, UINT, WPARAM, LPARAM);


// Public Methods
void ShowNewPositionWindow(HWND hWnd)
{
	PNParentHwnd = hWnd;
	DialogBox(hInst, MAKEINTRESOURCE(IDD_NEW_POSITION), hWnd, NewPosition);
}

// Private Methods
void PNPopulateComboBox(HWND hWnd)
{
	HWND PositionCompanySelectHwnd = GetDlgItem(hWnd, IDC_POSITION_COMPANY_SELECT);

	// reset combo box data
	SendMessage(PositionCompanySelectHwnd, CB_RESETCONTENT, NULL, NULL);
	PNCompanyComboList.clear();

	// get company data
	std::vector<Company> CompData = (*trackerData.GetCompanies());

	// create combo box strings
	for (int i = 0; i < CompData.size(); i++)
	{
		if (CompData[i].Selectable)
		{
			PNCompanyComboList.push_back(CompData[i].Name + " @ " + CompData[i].Location);
		}
	}

	// if there are no selectable companies
	if (PNCompanyComboList.size() == 0)
	{
		// show message and exit dialog
		EndDialog(PNDialogHwnd, LOWORD(PNWparam));
		MessageBox(NULL, "No Selectable Companies to add a Position to. Use Company -> Add to add a new Company or use Company -> Edit to make an existing Company Selectable", "No Companies", MB_OK);
		
	}

	for (int i = 0; i < PNCompanyComboList.size(); i++)
	{
		// Add string to combobox.
		SendMessage(PositionCompanySelectHwnd, CB_ADDSTRING, 0, (LPARAM)PNCompanyComboList[i].c_str());
		// set data of item to a pointer to the added company data item. to be used when saving the position
		SendMessage(PositionCompanySelectHwnd, CB_SETITEMDATA, i, (LPARAM)&(*trackerData.GetCompanies())[i]);
	}

	// set selection of combo box to first item
	SendMessage(PositionCompanySelectHwnd, CB_SETCURSEL, 0, NULL);
}

int SetupNewPositionWindow(HWND hWnd, WPARAM wParam)
{
	// Set Window Data
	// if there are no companies to add a position to
	if (trackerData.GetCompanies()->size() == 0)
	{
		// show message and exit dialog
		EndDialog(hWnd, LOWORD(wParam));
		MessageBox(NULL, "No Companies to add a Position to. Use Company -> Add to add a new Company.", "No Companies", MB_OK);
	}
	else
	{
		// setup combo box
		HWND PositionCompanySelectHwnd = GetDlgItem(hWnd, IDC_POSITION_COMPANY_SELECT);


		// populate Combo Box
		PNPopulateComboBox(hWnd);
	}

	return (INT_PTR)TRUE;
}



// Message handler
INT_PTR CALLBACK NewPosition(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	// handle custom messages
	if (message == NCD_NEW_COMPANY && message != 0)
	{
		// New Company Dialog created using this window has been closed
		HWND PositionCompanySelectHwnd = GetDlgItem(hWnd, IDC_POSITION_COMPANY_SELECT);
		// repopulate Company select combo box
		PNPopulateComboBox(hWnd);

		// get the ID of the newly created Company
		int NewID = (int)lParam;
		// select position company
		// get item index of company ID
		std::vector<Company> TempComp = (*trackerData.GetCompanies());
		for (int i = 0; i < TempComp.size(); i++)
		{
			if (NewID == TempComp[i].ID)
			{
				// set combo box to this index
				SendMessage(PositionCompanySelectHwnd, CB_SETCURSEL, i, NULL);
				break;
			}
		}
	}
	else if (message == NLD_NEW_LOG && message != 0)
	{
		// New Log Dialog created using this window has been closed
		// check if Log creation successful
		bool Success = lParam;
		if (Success)
		{
			// Position created successfully
			// send message to parent
			SendMessage(PNParentHwnd, UPDATE_LIST, 0, 0);
				
			// leave Dialog
			EndDialog(hWnd, LOWORD(wParam));
		}
		else
		{
			// Log creation canceled
			// Delete newly created Position
			trackerData.DeletePosition(PositionID);
			// Show Error
			MessageBox(NULL, "You must finish creating the first Log for this Position to complete Position creation.", "Position not saved", MB_OK | MB_ICONINFORMATION);
		}
		
	}
	else
	{
		switch (message)
		{
		case WM_INITDIALOG:
			PNDialogHwnd = hWnd;
			PNWparam = wParam;
			return SetupNewPositionWindow(hWnd, wParam);
			break;
		case WM_COMMAND:
			if (LOWORD(wParam) == IDC_POSITION_NEW_COMPANY)
			{
				// open new Company window
				ShowNewCompanyWindow(hWnd);
			}
			if (LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hWnd, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			if (LOWORD(wParam) == IDSAVE)
			{
				std::string PositionName = "";
				std::string PositionDescription = "";
				int CompanyID = -1;
				int bufferlen;
				std::vector<char> buffer;


				// Process Position

				// Get Windows
				HWND PositionNameHwnd = GetDlgItem(hWnd, IDC_POSITION_NAME_EDIT);
				HWND PositionDescriptionHwnd = GetDlgItem(hWnd, IDC_POSITION_DESCRIPTION_EDIT);
				HWND PositionCompanySelectHwnd = GetDlgItem(hWnd, IDC_POSITION_COMPANY_SELECT);

				// Get Data
				bufferlen = GetWindowTextLength(PositionNameHwnd) + 1;
				buffer.resize(bufferlen);
				GetWindowText(PositionNameHwnd, (LPSTR)&buffer[0], bufferlen);
				PositionName = &buffer[0];
				buffer.clear();
				bufferlen = GetWindowTextLength(PositionDescriptionHwnd) + 1;
				buffer.resize(bufferlen);
				GetWindowText(PositionDescriptionHwnd, &buffer[0], bufferlen);
				PositionDescription = &buffer[0];
				// get current selection index
				int ItemIndex = SendMessage(PositionCompanySelectHwnd, CB_GETCURSEL, NULL, NULL);
				if (ItemIndex != -1)
				{
					// get ID of current selection data
					Company* test = (Company*)SendMessage(PositionCompanySelectHwnd, CB_GETITEMDATA, ItemIndex, NULL);
					CompanyID = (test)->ID;
				}


				// validate Data
				if (PositionName == "" || PositionDescription == "" || CompanyID == -1)
				{
					// show error
					MessageBox(NULL, "All fields are required", "Info", MB_ICONASTERISK | MB_OK);
				}
				else
				{
					// Add new Position
					PositionID = trackerData.CreatePosition(PositionName, PositionDescription, CompanyID);
				
					// Prompt User to make a new Log for the new Position
					ShowNewLogWindow(hWnd, PositionID);
				}
			}
			break;
		}
	}

	return (INT_PTR)FALSE;
}
