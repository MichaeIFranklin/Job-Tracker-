#include "EditCompanyWindow.h"


// Globals
Company g_company;
std::vector<Company>* g_companies;
HWND CEParentHwnd;
HWND CompanyNameHwnd;
HWND CompanyLocationHwnd;
HWND CompanyPhoneHwnd;
HWND CompanyDescriptionHwnd;
HWND CompanySelectableHwnd;
HWND CompanyListHwnd;
bool CEUpdateList; // true if main window Positions List needs to update when window closes
// Function Declorations
INT_PTR CALLBACK    EditCompany(HWND, UINT, WPARAM, LPARAM);


// Public Methods
void ShowEditCompanyWindow(HWND hWnd)
{
	CEParentHwnd = hWnd;
	DialogBox(hInst, MAKEINTRESOURCE(IDD_EDIT_COMPANY), hWnd, EditCompany);
}

// Private Methods
void CESetupList()
{

	ListView_SetExtendedListViewStyle(CompanyListHwnd, LVS_EX_FULLROWSELECT);
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
	GetWindowRect(CompanyListHwnd, &listRect);
	int Width = listRect.right - listRect.left-1;
	for (iCol = 0; iCol < 2; iCol++)
	{
		lvc.iSubItem = iCol;
		lvc.pszText = szText;
		lvc.cx = Width / 2;     // Width of column in pixels.

		lvc.fmt = LVCFMT_LEFT;  // Left-aligned column.

		// Load the names of the column headings from the string resources.
		LoadString(hInst,
			IDS_EDIT_COMPANY_COLUMN1 + iCol,
			szText,
			sizeof(szText) / sizeof(szText[0]));

		// Insert the columns into the list view.
		if (ListView_InsertColumn(CompanyListHwnd, iCol, &lvc) == -1)
		{ }
	}
}
void CEPopulateList()
{
	// clear list view
	SendMessage(CompanyListHwnd, LVM_DELETEALLITEMS, NULL, NULL);

	// setup List Item parameters
	LVITEM lvI;
	lvI.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
	lvI.stateMask = 0;
	lvI.iSubItem = 0;
	lvI.state = 0;
	
	// for each item in g_companies
	for (int i = 0 ; i < (*g_companies).size(); i++)
	{
		// add current item to list
		lvI.iItem = i;
		lvI.pszText = (LPSTR)(*g_companies)[i].Name.c_str();
		lvI.lParam = (LPARAM)&(*g_companies)[i]; // to be used when user clicks on item

		if (ListView_InsertItem(CompanyListHwnd, &lvI) == -1)
			MessageBox(NULL,"Couldn't add list view item " + i+1,"Error",MB_OK | MB_ICONERROR);
		ListView_SetItemText(CompanyListHwnd, i, 1, (LPSTR)(*g_companies)[i].Location.c_str());
	}
}
void CEPopulateData(int ID)
{
	g_company = trackerData.GetCompany(ID);
	SetWindowText(CompanyNameHwnd, g_company.Name.c_str());
	SetWindowText(CompanyLocationHwnd, g_company.Location.c_str());
	SetWindowText(CompanyPhoneHwnd, g_company.Phone.c_str());
	SetWindowText(CompanyDescriptionHwnd, g_company.Description.c_str());
	if (g_company.Selectable)
	{
		// check box
		SendMessage(CompanySelectableHwnd, BM_SETCHECK, BST_CHECKED, NULL);
	}
	else
	{
		// uncheck box
		SendMessage(CompanySelectableHwnd, BM_SETCHECK, BST_UNCHECKED, NULL);
	}
	
}

int SetupEditCompanyWindow(HWND hWnd, WPARAM wParam)
{
	// get Company Info
	g_companies = trackerData.GetCompanies();

	// Get Windows
	CompanyNameHwnd = GetDlgItem(hWnd, IDC_COMPANY_NAME_EDIT);
	CompanyLocationHwnd = GetDlgItem(hWnd, IDC_COMPANY_LOCATION_EDIT);
	CompanyPhoneHwnd = GetDlgItem(hWnd, IDC_COMPANY_PHONE_EDIT);
	CompanyDescriptionHwnd = GetDlgItem(hWnd, IDC_COMPANY_DESCRIPTION_EDIT);
	CompanySelectableHwnd = GetDlgItem(hWnd, IDC_COMPANY_SELECTABLE);
	CompanyListHwnd = GetDlgItem(hWnd, IDC_COMPANY_LIST);

	// Set Window Data
	CEUpdateList = false;
	// if there are no companies to edit
	if ((*g_companies).size() == 0)
	{
		// show message and exit dialog
		EndDialog(hWnd, LOWORD(wParam));
		MessageBox(NULL, "No Companies to Edit. Use Company -> Add to add a new Company.", "No Companies", MB_OK);
	}
	else
	{
		// Setup List
		CESetupList();
		// populate list
		CEPopulateList();
		// select first item
		ListView_SetItemState(CompanyListHwnd,
			0,         // index to listview item
			LVIS_FOCUSED | LVIS_SELECTED, // item state
			0x000F // Specify to the macro that 4 bits are changing 
		);

		// get First Item in list
		LVITEM SelectedItem = { 0 };
		SelectedItem.iItem = 0;
		SelectedItem.mask = LVIF_PARAM;

		if (ListView_GetItem(CompanyListHwnd, &SelectedItem) == TRUE)
		{
			// get data item from first Item
			Company *item = (Company*)SelectedItem.lParam;

			// Populate window with data from new data item
			CEPopulateData(item->ID);
		}
	}

	return (INT_PTR)TRUE;
}



// Message handler
INT_PTR CALLBACK EditCompany(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return SetupEditCompanyWindow(hWnd, wParam);
	case WM_NOTIFY:
		// if we get a "item changed" notification
		if (((LPNMHDR)lParam)->code == LVN_ITEMCHANGED);
		{
			// get the state change of the item
			NMLISTVIEW* lpStateChange =(NMLISTVIEW*)(lParam);
			// if the state that that was LVIF_STATE and the state is LVIS_SELECTED while the old state is not LVIS_SELECTED
			if ((lpStateChange->uChanged & LVIF_STATE) &&
				(lpStateChange->uNewState & LVIS_SELECTED) != (lpStateChange->uOldState & LVIS_SELECTED))
			{
				// An Item was Selected or Deselected
				// if this notificatoin was about an item being selected
				if (lpStateChange->uNewState & LVIS_SELECTED)
				{
					int iPos = ListView_GetNextItem(CompanyListHwnd, -1, LVNI_SELECTED);
					if (iPos != -1)
					{
						LVITEM SelectedItem = {0};
						SelectedItem.iItem = iPos;
						SelectedItem.mask = LVIF_PARAM;
						if (ListView_GetItem(CompanyListHwnd, &SelectedItem) == TRUE)
						{
							// get data item from selected Item
							Company *item = (Company*)SelectedItem.lParam;
							
							// Populate window with data from new data item
							CEPopulateData(item->ID);
						}	
					}
				}
			}		
		}
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hWnd, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDSAVE)
		{
			std::string CompanyName;
			std::string CompanyLocation;
			std::string CompanyPhone;
			std::string CompanyDescription;
			bool CompanySelectable = true;
			int bufferlen;
			std::vector<char> buffer;

			// Process Company
			// Get Data
			bufferlen = GetWindowTextLength(CompanyNameHwnd) + 1;
			buffer.resize(bufferlen);
			GetWindowText(CompanyNameHwnd, (LPSTR)&buffer[0], bufferlen);
			CompanyName = &buffer[0];
			buffer.clear();
			bufferlen = GetWindowTextLength(CompanyLocationHwnd) + 1;
			buffer.resize(bufferlen);
			GetWindowText(CompanyLocationHwnd, (LPSTR)&buffer[0], bufferlen);
			CompanyLocation = &buffer[0];
			buffer.clear();
			bufferlen = GetWindowTextLength(CompanyPhoneHwnd) + 1;
			buffer.resize(bufferlen);
			GetWindowText(CompanyPhoneHwnd, (LPSTR)&buffer[0], bufferlen);
			CompanyPhone = &buffer[0];
			buffer.clear();
			bufferlen = GetWindowTextLength(CompanyDescriptionHwnd) + 1;
			buffer.resize(bufferlen);
			GetWindowText(CompanyDescriptionHwnd, (LPSTR)&buffer[0], bufferlen);
			CompanyDescription = &buffer[0];
			if (IsDlgButtonChecked(hWnd, IDC_COMPANY_SELECTABLE) == BST_UNCHECKED)
			{
				CompanySelectable = false;
			}

			// validate Data
			if (CompanyName == "" || CompanyLocation == "" || CompanyPhone == "" || CompanyDescription == "")
			{
				// show error
				MessageBox(NULL, "All fields are required", "Info", MB_ICONASTERISK | MB_OK);
			}
			else
			{
				// Update Company
				trackerData.UpdateCompany(g_company.ID, CompanyName, CompanyLocation, CompanyPhone, CompanyDescription, CompanySelectable);

				// repopulate list with updated entries
				CEPopulateList();

				// Main Window Position List must update now
				CEUpdateList = true;

				// Show Message
				//MessageBox(NULL, "Company has been updated.", "Updated", MB_OK);

			}
		}
		break;
	case WM_DESTROY:
		// if the main window position list must be updated
		if (CEUpdateList)
		{
			// send message to parent
			SendMessage(CEParentHwnd, UPDATE_LIST, 0, 0);
		}
		break;
	}
	return (INT_PTR)FALSE;
}
