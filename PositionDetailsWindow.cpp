#include "PositionDetailsWindow.h"
#include "NewLogWindow.h"
#include "NewCompanyWindow.h"

// Globals
int g_PDPositionID;
Position g_PDposition;
std::vector<Log>* Logs;
std::vector<std::string> PECompanyComboList;
HWND PDParentHwnd;
HWND PositionNameHwnd;
HWND PositionDescriptionHwnd;
HWND PositionCompanySelectHwnd;
HWND PDCompanyNameHwnd;
HWND PDCompanyLocationHwnd;
HWND PDCompanyPhoneHwnd;
HWND PDCompanyDescriptionHwnd;
HWND LogTitleHwnd;
HWND LogDateHwnd;
HWND LogReminderHwnd;
HWND LogReminderDateHwnd;
HWND LogDescriptionHwnd;
HWND LogListHwnd;
bool UpdateList; // true if the main window positoins list must be updated once window is closed

// Constants
const std::string ListHeaders[] = {
	"Title",
	"Date",
	"Reminder Date"
};

// Function Declorations
INT_PTR CALLBACK    PositionDetails(HWND, UINT, WPARAM, LPARAM);


// Public Methods
void ShowPositionDetailsWindow(HWND hWnd, int PositionID)
{
	PDParentHwnd = hWnd;
	g_PDPositionID = PositionID;
	DialogBox(hInst, MAKEINTRESOURCE(IDD_POSITION_DETAILS), hWnd, PositionDetails);
}

// Private Methods
void PDSetupList()
{

	ListView_SetExtendedListViewStyle(LogListHwnd, LVS_EX_FULLROWSELECT);
	// build columns
	CHAR szText[256];     // Temporary buffer.
	LVCOLUMN lvc;
	int iCol;

	// Initialize the LVCOLUMN structure.
	// The mask specifies that the format, width, text,
	// and subitem members of the structure are valid.
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	// Add the columns.
	// get width of list box to get width of columns
	RECT listRect;
	GetWindowRect(LogListHwnd, &listRect);
	int Width = listRect.right - listRect.left - 1;
	for (iCol = 0; iCol < 3; iCol++)
	{
		lvc.iSubItem = iCol;
		lvc.pszText = (LPSTR)ListHeaders[iCol].c_str();
		lvc.cx = Width / 2;     // Width of column in pixels.

		lvc.fmt = LVCFMT_LEFT;  // Left-aligned column.

		// Insert the columns into the list view.
		if (ListView_InsertColumn(LogListHwnd, iCol, &lvc) == -1)
		{
		}
	}
}
void PDPopulateList()
{
	// clear list view
	SendMessage(LogListHwnd, LVM_DELETEALLITEMS, NULL, NULL);
	// get updated Position Info
	g_PDposition = trackerData.GetPosition(g_PDPositionID);

	// get updated Logs
	Logs = &g_PDposition.Logs;

	// setup List Item parameters
	LVITEM lvI;
	lvI.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
	lvI.stateMask = 0;
	lvI.iSubItem = 0;
	lvI.state = 0;

	// for each item in g_companies
	for (int i = 0; i < (*Logs).size(); i++)
	{
		// add current item to list
		lvI.iItem = i;
		lvI.pszText = (LPSTR)(*Logs)[i].Title.c_str();

		if (ListView_InsertItem(LogListHwnd, &lvI) == -1)
			MessageBox(NULL, "Couldn't add list view item " + i + 1, "Error", MB_OK | MB_ICONERROR);
		ListView_SetItemText(LogListHwnd, i, 1, (LPSTR)(*Logs)[i].Date.c_str());
		ListView_SetItemText(LogListHwnd, i, 2, (LPSTR)(*Logs)[i].ReminderDate.c_str());
	}
}
void PDPopulateData(int Index)
{
	Log log = (*Logs)[Index];
	SetWindowText(LogTitleHwnd, log.Title.c_str());
	SetWindowText(LogDescriptionHwnd, log.Description.c_str());
	SetWindowText(LogDateHwnd, log.Date.c_str());
	SetWindowText(LogReminderHwnd, log.Reminder.c_str());
	SetWindowText(LogReminderDateHwnd, log.ReminderDate.c_str());
}
void PDPopulateCompanyData(int CompanyID)
{
	Company PositionCompany = trackerData.GetCompany(CompanyID);
	SetWindowText(PDCompanyNameHwnd, PositionCompany.Name.c_str());
	SetWindowText(PDCompanyPhoneHwnd, PositionCompany.Phone.c_str());
	SetWindowText(PDCompanyLocationHwnd, PositionCompany.Location.c_str());
	SetWindowText(PDCompanyDescriptionHwnd, PositionCompany.Description.c_str());
}

void PEPopulateComboBox()
{
	// reset combo box data
	SendMessage(PositionCompanySelectHwnd, CB_RESETCONTENT, NULL, NULL);
	PECompanyComboList.clear();

	// get company data
	std::vector<Company> CompData = (*trackerData.GetCompanies());

	// create combo box strings
	for (int i = 0; i < CompData.size(); i++)
	{
		if (CompData[i].Selectable)
		{
			PECompanyComboList.push_back(CompData[i].Name + " @ " + CompData[i].Location);
		}
	}

	// if there are no selectable companies
	if (PECompanyComboList.size() == 0)
	{
		// disable selector
		EnableWindow(PositionCompanySelectHwnd, false);
	}
	else
	{
		for (int i = 0; i < PECompanyComboList.size(); i++)
		{
			// Add string to combobox.
			SendMessage(PositionCompanySelectHwnd, CB_ADDSTRING, 0, (LPARAM)PECompanyComboList[i].c_str());
			// set data of item to a pointer to the added company data item. to be used when saving the position
			SendMessage(PositionCompanySelectHwnd, CB_SETITEMDATA, i, (LPARAM)&(*trackerData.GetCompanies())[i]);

		}


		// set selection of combo box to first item
		SendMessage(PositionCompanySelectHwnd, CB_SETCURSEL, 0, NULL);
	}
}

int SetupPositionDetailsWindow(HWND hWnd)
{
	// get Position Info
	g_PDposition = trackerData.GetPosition(g_PDPositionID);

	// Get Windows
	PDCompanyNameHwnd = GetDlgItem(hWnd, IDC_COMPANY_NAME_TEXT);
	PDCompanyLocationHwnd = GetDlgItem(hWnd, IDC_COMPANY_LOCATION_TEXT);
	PDCompanyPhoneHwnd = GetDlgItem(hWnd, IDC_COMPANY_PHONE_TEXT);
	PDCompanyDescriptionHwnd = GetDlgItem(hWnd, IDC_COMPANY_DESCRIPTION_TEXT);
	PositionNameHwnd = GetDlgItem(hWnd, IDC_POSITION_NAME_TEXT);
	PositionDescriptionHwnd = GetDlgItem(hWnd, IDC_POSITION_DESCRIPTION_TEXT);
	LogTitleHwnd = GetDlgItem(hWnd, IDC_LOG_TITLE_TEXT);
	LogDescriptionHwnd = GetDlgItem(hWnd, IDC_LOG_DESCRIPTION_TEXT);
	LogReminderHwnd = GetDlgItem(hWnd, IDC_LOG_REMINDER_TEXT);
	LogReminderDateHwnd = GetDlgItem(hWnd, IDC_LOG_REMINDER_DATE_TEXT);
	LogDateHwnd = GetDlgItem(hWnd, IDC_LOG_DATE_TEXT);
	LogListHwnd = GetDlgItem(hWnd, IDC_LOG_LIST);
	PositionCompanySelectHwnd = GetDlgItem(hWnd, IDC_POSITION_COMPANY_SELECT);

	// Set Window Data
	UpdateList = false;
	// Position data
	SetWindowText(PositionNameHwnd, g_PDposition.Name.c_str());
	SetWindowText(PositionDescriptionHwnd, g_PDposition.Description.c_str());
	
	// Setup Combo Box
	PEPopulateComboBox();
	// select Position Company
	// get item index of company ID
	std::vector<Company> TempComp = (*trackerData.GetCompanies());
	for (int i = 0; i < TempComp.size(); i++)
	{
		if (g_PDposition.CompanyID == TempComp[i].ID)
		{
			// set combo box to this index
			SendMessage(PositionCompanySelectHwnd, CB_SETCURSEL, i, NULL);
			break;
		}
	}
	// Set Company data
	PDPopulateCompanyData(g_PDposition.CompanyID);

	// Setup List
	PDSetupList();
	// populate list
	PDPopulateList();
	// select last item
	ListView_SetItemState(LogListHwnd,
		Logs->size()-1,         // index to listview item
		LVIS_FOCUSED | LVIS_SELECTED, // item state
		0x000F // Specify to the macro that 4 bits are changing 
	);

	// Populate window with data from new data item
	PDPopulateData(Logs->size() - 1);

	return (INT_PTR)TRUE;
}



// Message handler
INT_PTR CALLBACK PositionDetails(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	// handle custom messages
	if (message == NCD_NEW_COMPANY && message != 0)
	{
		// New Company Dialog created using this window has been closed
		// repopulate Company select combo box
		PEPopulateComboBox();

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
		// Set Company data
		PDPopulateCompanyData(NewID);
	}
	else if (message == NLD_NEW_LOG && message != 0)
	{
		// New Log Dialog created using this window has been closed
		// check if Log creation successful
		bool Success = lParam;
		if (Success)
		{
			UpdateList = true;
			// repopulate new Log list
			PDPopulateList();
			// select last item
			ListView_SetItemState(LogListHwnd,
				Logs->size() - 1,         // index to listview item
				LVIS_FOCUSED | LVIS_SELECTED, // item state
				0x000F // Specify to the macro that 4 bits are changing 
			);

			// Populate window with data from new data item
			PDPopulateData(Logs->size() - 1);
		}
	}
	else
	{
		switch (message)
		{
		case WM_INITDIALOG:
			return SetupPositionDetailsWindow(hWnd);
		case WM_NOTIFY:
			// if we get a "item changed" notification
			if (((LPNMHDR)lParam)->code == LVN_ITEMCHANGED);
			{
				// get the state change of the item
				NMLISTVIEW* lpStateChange = (NMLISTVIEW*)(lParam);
				// if the state that that was LVIF_STATE and the state is LVIS_SELECTED while the old state is not LVIS_SELECTED
				if ((lpStateChange->uChanged & LVIF_STATE) &&
					(lpStateChange->uNewState & LVIS_SELECTED) != (lpStateChange->uOldState & LVIS_SELECTED))
				{
					// An Item was Selected or Deselected
					// if this notificatoin was about an item being selected
					if (lpStateChange->uNewState & LVIS_SELECTED)
					{
						int iPos = ListView_GetNextItem(LogListHwnd, -1, LVNI_SELECTED);
						if (iPos != -1)
						{
							// populate new log data with selected log's data
							PDPopulateData(iPos);
						}
					}
				}
			}
			break;
		case WM_COMMAND:
			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				// new company selected
				int CompanyID = -1;
				// get current selection index
				int ItemIndex = SendMessage(PositionCompanySelectHwnd, CB_GETCURSEL, NULL, NULL);
				if (ItemIndex != -1)
				{
					// get ID of current selection data
					Company* test = (Company*)SendMessage(PositionCompanySelectHwnd, CB_GETITEMDATA, ItemIndex, NULL);
					CompanyID = (test)->ID;
				}
				// Set new Company data
				PDPopulateCompanyData(CompanyID);
			}
			if (LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hWnd, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			if (LOWORD(wParam) == ID_NEW_LOG)
			{
				// Show new Log Window
				ShowNewLogWindow(hWnd, g_PDPositionID);
			}
			if (LOWORD(wParam) == IDC_POSITION_NEW_COMPANY)
			{
				// open new Company window
				ShowNewCompanyWindow(hWnd);
			}
			if (LOWORD(wParam) == IDSAVE)
			{
				std::string PositionName;
				std::string PositionDescription;
				int CompanyID = -1;
				int bufferlen;
				std::vector<char> buffer;

				// Process Position
				// Get Data
				bufferlen = GetWindowTextLength(PositionNameHwnd) +1;
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
					// Update Position
					trackerData.UpdatePosition(g_PDPositionID, PositionName, PositionDescription, CompanyID);

					// send message to parent
					SendMessage(PDParentHwnd, UPDATE_LIST, 0, 0);

					MessageBox(NULL, "Position Info Updated", "Info Updated", MB_ICONINFORMATION | MB_OK);
					/*EndDialog(hWnd, LOWORD(wParam));
					return (INT_PTR)TRUE;*/
				}
			}
			break;
		case WM_DESTROY:
			// if the main window position list must be updated
			if (UpdateList)
			{
				// send message to parent
				SendMessage(PDParentHwnd, UPDATE_LIST, 0, 0);
			}
			break;
		}
	}
	return (INT_PTR)FALSE;
}
