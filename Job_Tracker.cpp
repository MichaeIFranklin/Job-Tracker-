// Job Tracker.cpp : Defines the entry point for the application.
//

#include "Job_Tracker.h"
#include "NewCompanyWindow.h"
#include "EditCompanyWindow.h"
#include "NewPositionWindow.h" 
#include "PositionDetailsWindow.h"
#include "SaveLoadProgressWindow.h"


// Constants
enum SortBy {
	Name,
	CompanyName,
	ApplyDate,
	ReminderDate,
	LogDate
};
#define IDC_POSITION_LIST_MAIN_WINDOW 10035
#define IDC_OPEN_POSITION_BUTTON 10036
#define IDC_NEW_POSITION_BUTTON 10038
#define MAX_LOADSTRING 100
const std::string ListHeaders[] = {
	"Position Title",
	"Current Status",
	"Date Applied",
	"Recent Log",
	"Reminder Date",
	"Company Name",
	"Company Location"
};

// Global Variables:
UINT UPDATE_LIST;
HINSTANCE hInst = NULL;
CHAR szTitle[MAX_LOADSTRING];                  // The title bar text
CHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
											   // Globals
struct PositionInfo
{
	int ID;
	std::string Name;
	std::string Company;
	std::string Location;
	std::string Status;
	std::string AppliedDate;
	std::string LogDate;
	std::string ReminderDate;
};

std::vector<PositionInfo> g_Positions;
HWND MWPositionListHwnd;
HWND OpenPositionButton;
HWND NewPositionButton;
int SelectedPositionID;
SortBy sortBy;
bool SortAscending;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// Main Window Methods ////////////////////////////////////////////////
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
	
    // Initialize global strings
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_JOBTRACKER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_JOBTRACKER));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_JOBTRACKER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_MENU+1);
    wcex.lpszMenuName   = MAKEINTRESOURCE(IDC_JOBTRACKER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_CLIPCHILDREN,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);

   UpdateWindow(hWnd);

   return TRUE;
}
// End of Main Window Methods ////////////////////////////////////////////////

// Child Control Methods ////////////////////////////////////////////////
// Private Methods
PositionInfo GetPositionInfo(int ID)
{
	// find company with correct ID
	PositionInfo TempInfo;
	for (int b = 0; b < g_Positions.size(); b++)
	{
		if (g_Positions[b].ID == ID)
		{
			TempInfo = g_Positions[b];
			break;
		}
	}
	return TempInfo;
}
int Partition(int *Array, const int Start, const int End, bool Compare(int CurrentItem, int Pivot)) {
	const int Mid = Start + (End - Start) / 2;
	const int Pivot = Array[Mid];
	// move the mid point value to the front.
	std::swap(Array[Mid], Array[Start]);
	int CurrentIndex = Start + 1;
	int CurrentEnd = End;
	while (CurrentIndex <= CurrentEnd) {
		while (CurrentIndex <= CurrentEnd && (*Compare)(Array[CurrentIndex], Pivot)) {
			CurrentIndex++;
		}

		while (CurrentIndex <= CurrentEnd && !(*Compare)(Array[CurrentIndex], Pivot)) {
			CurrentEnd--;
		}

		if (CurrentIndex < CurrentEnd) {
			std::swap(Array[CurrentIndex], Array[CurrentEnd]);
		}
	}
	std::swap(Array[CurrentIndex - 1], Array[Start]);
	return CurrentIndex - 1;
}
void Quicksort(int *Array, const int Start, const int End, bool Compare(int CurrentItem, int Pivot), const int SortIndex) {

	if (Start >= End) {
		return;
	}


	int part = Partition(Array, Start, End, Compare);

	Quicksort(Array, Start, part - 1, Compare, SortIndex);
	Quicksort(Array, part + 1, End, Compare, SortIndex);
}

// Compare Methods
bool ComparePositionNames(int CurrentItem, int Pivot)
{
	// get position info of ID
	PositionInfo CurrentItemInfo = GetPositionInfo(CurrentItem);
	PositionInfo PivotInfo = GetPositionInfo(Pivot);

	// compare item names
	if (SortAscending)
	{
		return CurrentItemInfo.Name.compare(PivotInfo.Name) <= 0;
	}
	else
	{
		return CurrentItemInfo.Name.compare(PivotInfo.Name) > 0;
	}

}
bool ComparePositionCompanies(int CurrentItem, int Pivot)
{
	// get position info of ID
	PositionInfo CurrentItemInfo = GetPositionInfo(CurrentItem);
	PositionInfo PivotInfo = GetPositionInfo(Pivot);

	// compare item names
	if (SortAscending)
	{
		return CurrentItemInfo.Company.compare(PivotInfo.Company) <= 0;
	}
	else
	{
		return CurrentItemInfo.Company.compare(PivotInfo.Company) > 0;
	}

}
bool ComparePositionLogDates(int CurrentItem, int Pivot)
{
	// get position info of ID
	PositionInfo CurrentItemInfo = GetPositionInfo(CurrentItem);
	PositionInfo PivotInfo = GetPositionInfo(Pivot);

	// compare item names
	if (!SortAscending)
	{
		return CurrentItemInfo.LogDate.compare(PivotInfo.LogDate) <= 0;
	}
	else
	{
		return CurrentItemInfo.LogDate.compare(PivotInfo.LogDate) > 0;
	}

}
bool ComparePositionApplyDates(int CurrentItem, int Pivot)
{
	// get position info of ID
	PositionInfo CurrentItemInfo = GetPositionInfo(CurrentItem);
	PositionInfo PivotInfo = GetPositionInfo(Pivot);

	// compare item names
	if (!SortAscending)
	{
		return CurrentItemInfo.AppliedDate.compare(PivotInfo.AppliedDate) <= 0;
	}
	else
	{
		return CurrentItemInfo.AppliedDate.compare(PivotInfo.AppliedDate) > 0;
	}

}
bool ComparePositionReminderDates(int CurrentItem, int Pivot)
{
	// get position info of ID
	PositionInfo CurrentItemInfo = GetPositionInfo(CurrentItem);
	PositionInfo PivotInfo = GetPositionInfo(Pivot);

	// compare item names
	return CurrentItemInfo.ReminderDate.compare(PivotInfo.ReminderDate) <= 0;

}


// Sorting Methods
void SortPositions()
{
	// create a list of IDs
	std::vector<int> IDList;
	for (int i = 0; i < g_Positions.size(); i++)
	{
		IDList.push_back(g_Positions[i].ID);
	}
	// if we have info to sort
	if (IDList.size() > 0)
	{
		// Sort List IDs based on sorting enum
		switch (sortBy)
		{
		case Name:
			Quicksort(&IDList[0], 0, g_Positions.size() - 1, ComparePositionNames, 0);
			break;
		case CompanyName:
			Quicksort(&IDList[0], 0, g_Positions.size() - 1, ComparePositionCompanies, 0);
			break;
		case LogDate:
			Quicksort(&IDList[0], 0, g_Positions.size() - 1, ComparePositionLogDates, 0);
			break;
		case ReminderDate:
			Quicksort(&IDList[0], 0, g_Positions.size() - 1, ComparePositionReminderDates, 0);
			break;
		case ApplyDate:
			Quicksort(&IDList[0], 0, g_Positions.size() - 1, ComparePositionApplyDates, 0);
			break;
		}

		// Sort List using Sorted IDList
		std::vector<PositionInfo> NewList;
		for (int i = 0; i < g_Positions.size(); i++)
		{
			PositionInfo TempInfo = GetPositionInfo(IDList[i]);
			// add company to NewList
			NewList.push_back(TempInfo);
		}

		// clear old list
		g_Positions.clear();
		// set old list to new list
		g_Positions.swap(NewList);
	}
	}
	


	

// Setup Methods
void MWSetupList()
{
	// setup List Parameters
	ListView_SetExtendedListViewStyle(MWPositionListHwnd, LVS_EX_FULLROWSELECT);
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
	GetWindowRect(MWPositionListHwnd, &listRect);
	int Width = listRect.right - listRect.left - 1;
	for (iCol = 0; iCol < 7; iCol++)
	{
		lvc.iSubItem = iCol;
		lvc.pszText = (LPSTR)ListHeaders[iCol].c_str();
		// setup column widths
		switch (iCol)
		{
		case 0:
			lvc.cx = Width * 2 / 12;
			break;
		case 1:
			lvc.cx = Width * 2 / 12;
			break;
		case 2:
		case 3:
			lvc.cx = Width / 12;
			break;
		case 4:
			lvc.cx = Width*1.1 / 12;
			break;
		case 5:
			lvc.cx = Width * 2 / 12;
			break;
		case 6:
			lvc.cx = Width*4.9 / 12;
			break;
		}
		if (iCol == 0 || iCol == 1 || iCol == 5 || iCol == 6)
			lvc.fmt = LVCFMT_LEFT;  // Left-aligned column.
		else
			lvc.fmt = LVCFMT_CENTER;  // Center-aligned column.

								// Insert the columns into the list view.
		if (ListView_InsertColumn(MWPositionListHwnd, iCol, &lvc) == -1)
		{
		}
	}
}
void GetPositionInfoData()
{
	// get all Positions
	std::vector<Position>* positions = trackerData.GetPositions();

	// for each position
	for (int i = 0; i < positions->size(); i++)
	{
		// populate position info into PositionInfo list
		PositionInfo TempInfo;
		TempInfo.ID = (*positions)[i].ID;
		TempInfo.Name = (*positions)[i].Name;
		Company PosComp = trackerData.GetCompany((*positions)[i].CompanyID);
		TempInfo.Company = PosComp.Name;
		TempInfo.Location = PosComp.Location;
		TempInfo.Status = trackerData.GetPositionRecentLogTitle(TempInfo.ID);
		TempInfo.LogDate = trackerData.GetPositionRecentLogDate(TempInfo.ID);
		TempInfo.ReminderDate = trackerData.GetPositionRecentReminderDate(TempInfo.ID);
		TempInfo.AppliedDate = trackerData.GetPositionApplyDate(TempInfo.ID);

		// add temp info to list
		g_Positions.push_back(TempInfo);
	}

	// Sort List
	SortPositions();
}
void MWPopulateList()
{
	// clear list view
	SendMessage(MWPositionListHwnd, LVM_DELETEALLITEMS, NULL, NULL);
	// disable position buttons
	EnableWindow(OpenPositionButton, false);

	// get updated Position Info
	g_Positions.clear();
	GetPositionInfoData();



	// setup List Item parameters
	LVITEM lvI;
	lvI.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
	lvI.stateMask = 0;
	lvI.iSubItem = 0;
	lvI.state = 0;

	// for each item in g_companies
	for (int i = 0; i < g_Positions.size(); i++)
	{
		// add current item to list
		lvI.iItem = i;
		lvI.pszText = (LPSTR)g_Positions[i].Name.c_str();
		lvI.lParam = (LPARAM)&g_Positions[i]; // to be used when user double clicks or just clicks on item

		if (ListView_InsertItem(MWPositionListHwnd, &lvI) == -1)
			MessageBox(NULL, "Couldn't add list view item " + i + 1, "Error", MB_OK | MB_ICONERROR);
		ListView_SetItemText(MWPositionListHwnd, i, 1, (LPSTR)g_Positions[i].Status.c_str());
		ListView_SetItemText(MWPositionListHwnd, i, 2, (LPSTR)g_Positions[i].AppliedDate.c_str());
		ListView_SetItemText(MWPositionListHwnd, i, 3, (LPSTR)g_Positions[i].LogDate.c_str());
		ListView_SetItemText(MWPositionListHwnd, i, 4, (LPSTR)g_Positions[i].ReminderDate.c_str());
		ListView_SetItemText(MWPositionListHwnd, i, 5, (LPSTR)g_Positions[i].Company.c_str());
		ListView_SetItemText(MWPositionListHwnd, i, 6, (LPSTR)g_Positions[i].Location.c_str());
	}
}

void SetupMainWindow(HWND hWnd)
{
	// Register new log message
	UPDATE_LIST = RegisterWindowMessage("Update List");

	// Setup Windows
	// List Window //////////////////////////////
	// Initalize List Control Classes
	INITCOMMONCONTROLSEX icex;
	icex.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex);

	// Get Size of Parent Window
	RECT rcClient;
	GetClientRect(hWnd, &rcClient);
	// Create List window
	MWPositionListHwnd = CreateWindowEx(
		LVS_EX_FULLROWSELECT,
		WC_LISTVIEW,
		"",
		WS_CHILD | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS,
		0, 0,
		rcClient.right - rcClient.left,
		(rcClient.bottom - rcClient.top)-30,
		hWnd,
		(HMENU)IDC_POSITION_LIST_MAIN_WINDOW,
		hInst,
		NULL);

	if (!hWnd)
	{
		MessageBox(NULL, "Failed to create List HWND", "HWND Failed", MB_OK | MB_ICONERROR);
		return;
	}

	// Show List Window
	ShowWindow(MWPositionListHwnd, true);
	// End of List Window //////////////////////////////

	// Open Position Button //////////////////////////////
	// Get Size of Parent Window
	rcClient;
	GetClientRect(hWnd, &rcClient);
	// Create List window
	OpenPositionButton = CreateWindowEx(
		NULL,
		WC_BUTTON,
		"View Position Details",
		WS_CHILD,
		rcClient.right-153, rcClient.bottom - 28,
		150,
		25,
		hWnd,
		(HMENU)IDC_OPEN_POSITION_BUTTON,
		hInst,
		NULL);

	if (!hWnd)
	{
		MessageBox(NULL, "Failed to create Open Position Button", "HWND Failed", MB_OK | MB_ICONERROR);
		return;
	}

	// Show List Window
	ShowWindow(OpenPositionButton, true);
	EnableWindow(OpenPositionButton, false);
	// End of Open Position Button //////////////////////////////

	// New Position Button //////////////////////////////
	// Get Size of Parent Window
	rcClient;
	GetClientRect(hWnd, &rcClient);
	// Create List window
	NewPositionButton = CreateWindowEx(
		NULL,
		WC_BUTTON,
		"New Position",
		WS_CHILD,
		rcClient.right - 256, rcClient.bottom - 28,
		100,
		25,
		hWnd,
		(HMENU)IDC_NEW_POSITION_BUTTON,
		hInst,
		NULL);

	if (!hWnd)
	{
		MessageBox(NULL, "Failed to create New Position Button", "HWND Failed", MB_OK | MB_ICONERROR);
		return;
	}

	// Show Window
	ShowWindow(NewPositionButton, true);
	// End of New Position Button //////////////////////////////

	// Set Window Data
	SelectedPositionID = -1;
	SortAscending = true;
	sortBy = Name;

	// Setup List
	MWSetupList();
	// populate list
	MWPopulateList();
}

// Public Methods
void ShowMainWindow(HWND hWnd)
{
	// setup Child Windows
	SetupMainWindow(hWnd);
}
// End of Child Control Methods ////////////////////////////////////////////////


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// handle custom messages
	if (message == UPDATE_LIST && message != 0)
	{
		// Position Details have changed.. Update Positions List

		// repopulate Positions list
		MWPopulateList();
	}
	else if(message == SLP_SAVELOAD_COMPLETE && message != 0)
	{
		// saving data finished
		// exit
		DestroyWindow(hWnd);
	}
	else
	{

		switch (message)
		{
		case WM_CREATE:
			// load in data
			ShowSaveLoadProgressWindow(hWnd, false);
			// show data
			ShowMainWindow(hWnd);
			break;
		case WM_NOTIFY:
		{
			// get Notify code
			UINT code = ((LPNMHDR)lParam)->code;
			switch (code)
			{
			case LVN_ITEMCHANGED:
			{
				// if we get a "item changed" notification
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
						int iPos = ListView_GetNextItem(MWPositionListHwnd, -1, LVNI_SELECTED);
						if (iPos != -1)
						{
							// get ID of new selected Item
							LVITEM SelectedItem = { 0 };
							SelectedItem.iItem = iPos;
							SelectedItem.mask = LVIF_PARAM;
							if (ListView_GetItem(MWPositionListHwnd, &SelectedItem) == TRUE)
							{
								// get data item from selected Item
								PositionInfo *item = (PositionInfo*)SelectedItem.lParam;

								// set selected ID to item's ID
								SelectedPositionID = item->ID;

								// Enable Position Buttons
								EnableWindow(OpenPositionButton, true);
							}
						}
					}
				}
				break;
			}
			case LVN_COLUMNCLICK:
			{
				// if we get that a column header was clicked
				// get column that was clicked
				int iCol = ((LPNMLISTVIEW)lParam)->iSubItem;

				switch (iCol)
				{
				case 0: // sort by name
					if (sortBy != Name)
					{
						// if we are not already sorting by Company Name
						sortBy = Name;
						SortAscending = true;
					}
					else
					{
						// if we are sorting by Company Name flip the sort order
						SortAscending = !SortAscending;
					}
					// Repopulate List
					MWPopulateList();
					break;
				case 2: // sort by applied date
					if (sortBy != ApplyDate)
					{
						// if we are not already sorting by Company Name
						sortBy = ApplyDate;
						SortAscending = true;
					}
					else
					{
						// if we are sorting by Company Name flip the sort order
						SortAscending = !SortAscending;
					}
					// Repopulate List
					MWPopulateList();
					break;
				case 3: // sort by log date
					if (sortBy != LogDate)
					{
						// if we are not already sorting by Company Name
						sortBy = LogDate;
						SortAscending = true;
					}
					else
					{
						// if we are sorting by Company Name flip the sort order
						SortAscending = !SortAscending;
					}
					// Repopulate List
					MWPopulateList();
					break;
				case 4: // sort by reminder date
					if (sortBy != ReminderDate)
					{
						// if we are not already sorting by Company Name
						sortBy = ReminderDate;
						SortAscending = true;
						// Repopulate List
						MWPopulateList();
					}
					break;
				case 5: // sort by company name
					if (sortBy != CompanyName)
					{
						// if we are not already sorting by Company Name
						sortBy = CompanyName;
						SortAscending = true;
					}
					else
					{
						// if we are sorting by Company Name flip the sort order
						SortAscending = !SortAscending;
					}
					// Repopulate List
					MWPopulateList();
					break;
				}
				break;
			}
			}
			break;
		}
		break;
		case WM_COMMAND:
			/*if (LOWORD(wParam) == IDCANCEL)
			{
			EndDialog(hWnd, LOWORD(wParam));
			return (INT_PTR)TRUE;
			}
			if (LOWORD(wParam) == ID_NEW_LOG)
			{
			// Show new Log Window
			ShowNewLogWindow(hWnd, g_PDPositionID);
			}*/
		{
			int wmId = LOWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
			case IDM_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			case IDM_COMPANY_NEW:
				ShowNewCompanyWindow(hWnd);
				break;
			case IDM_COMPANY_EDIT:
				ShowEditCompanyWindow(hWnd);
				break;
			case IDC_NEW_POSITION_BUTTON:
				ShowNewPositionWindow(hWnd);
				break;
			case IDC_OPEN_POSITION_BUTTON:
				ShowPositionDetailsWindow(hWnd, SelectedPositionID);
				break;
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code that uses hdc here...
			EndPaint(hWnd, &ps);
		}
		break;
		case WM_CLOSE:
			// save data
			ShowSaveLoadProgressWindow(hWnd, true);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


