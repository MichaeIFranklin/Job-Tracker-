#include "TrackerData.h"

TrackerData trackerData;
UINT TD_DIALOG_UPDATE;
UINT TD_DIALOG_SETUP;
void TrackerData::UpdateSaveLoadDialog(HWND hwnd)
{
	if (hwnd != NULL)
	{
		LoadCurrent++;
		// send update message
		SendMessage(hwnd, TD_DIALOG_UPDATE,0,0);
	}
}
void TrackerData::SaveTrackerData(HWND hwnd)
{
	// set dialog vars
	LoadMax = Companies.size() + Positions.size();
	LoadCurrent = 0;

	if (hwnd != NULL)
	{
		// send setup message
		SendMessage(hwnd, TD_DIALOG_SETUP, 0, 0);
	}

	// get output object
	std::ofstream File;
	File.open("./JobData.data", std::ios::trunc);

	// if file open worked
	if (File.is_open())
	{
		// start writing companies to file
		for (int i = 0; i < Companies.size(); i++)
		{
			Company Comp = Companies[i];
			/*char buffer[1000];
			// format string to be written to file
			sprintf_s(buffer., "%d\n%s\n%s\n%s\n}\n%s\n}\n%d\n", Comp.ID, Comp.Name.c_str(), Comp.Phone.c_str(), Comp.Location.c_str(), Comp.Description.c_str(), Comp.Selectable);*/
			std::string buffer = std::to_string(Comp.ID) + "\n" +
				Comp.Name + "\n" +
				Comp.Phone + "\n" +
				Comp.Location + "\n}\n" +
				Comp.Description + "\n}\n" +
				(Comp.Selectable ? "1":"0") + "\n";
			// write formated string to file
			File << buffer;

			// update dialog
			UpdateSaveLoadDialog(hwnd);
		}
		// write a escape character on the next line to show there are no more companies
		File << "~";
		// start writing position to file
		for (int i = 0; i < Positions.size(); i++)
		{

			Position Pos = Positions[i];
			/*char buffer[1000];
			// format string to be written to file
			sprintf_s(buffer, "\n%d\n%s\n%s\n}\n%d\n", Pos.ID, Pos.Name.c_str(), Pos.Description.c_str(), Pos.CompanyID);*/
			std::string buffer = "\n" + std::to_string(Pos.ID) + "\n" +
				Pos.Name + "\n" +
				Pos.Description + "\n}\n" +
				std::to_string(Pos.CompanyID) + "\n";
			// write formated string to file
			File << buffer;

			// write the logs for this Position
			for (int b = 0; b < Pos.Logs.size(); b++)
			{

				Log log = Pos.Logs[b];
				/*char buffer[1000];
				// format string to be written to file
				sprintf_s(buffer, "%s\n%s\n%s\n%s\n}\n%s\n}\n", log.Title.c_str(), log.Date.c_str(), log.ReminderDate.c_str(), log.Description.c_str(), log.Reminder.c_str());*/
				std::string buffer = log.Title + "\n" +
					log.Date + "\n" +
					log.ReminderDate + "\n" +
					log.Description + "\n}\n" +
					log.Reminder + "\n}\n";
				// write formated string to file
				File << buffer;
			}
			// write a escape character on the next line to show there are no more logs for this position
			File << "~";

			// update dialog
			UpdateSaveLoadDialog(hwnd);
		}

		// if there is nothign to save
		if (LoadMax == 0)
		{
			// update the window to close is as this won't have been called yet
			UpdateSaveLoadDialog(hwnd);
		}

		// close file
		File.close();
	}
}
std::string TrackerData::ReadInMultiLineData(std::ifstream *File, std::string line, HWND hwnd)
{
	std::string multiData;

	// while we have not read in the escape character for multiline data - }
	while (line != "}")
	{
		// add current data to item data
		multiData += line;

		// add a line break to the end of this line
		multiData += "\n";

		// read in next line
		getline((*File), line);

		// update dialog
		UpdateSaveLoadDialog(hwnd);
	}

	// remove the final line break of this data
	multiData = multiData.substr(0, multiData.size() - 1);

	// return item data
	return multiData;
}
void TrackerData::LoadTrackerData(HWND hwnd)
{
	// get input object
	std::ifstream File;
	File.open("./JobData.data");
	// if file open worked
	if (File.is_open())
	{
		// set dialog vars
		int lines_count = 0;
		std::string line;
		while (getline(File, line))
			lines_count++;
		LoadMax = lines_count;
		LoadCurrent = 0;

		if (hwnd != NULL)
		{
			// send setup message
			SendMessage(hwnd, TD_DIALOG_SETUP, 0, 0);
		}

		// jump to begining of file
		File.clear();
		File.seekg(0, std::ios::beg);

		line = "";
		/*
		current item to be read:
		0 - ID
		1 - Name
		2 - Phone
		3 - Location
		4 - Description
		5 - Selectable
		*/
		int currentItem = 0; 
		// start reading companies from file
		// while we have not read in the escape character for companies - ~
		Company TempComp; // holder for company data
		while (line != "~")
		{
			// if we have information to process
			if (line != "")
			{
				// process data based on current item
				switch (currentItem)
				{
				case 0: // ID
				{
					TempComp.ID = atoi(line.c_str());
				}
				break;
				case 1: // Name
				{
					TempComp.Name = line;
				}
				break;
				case 2: // Phone
				{
					TempComp.Phone = line;
				}
				break;
				case 3: // Location
				{
					// read in multiline data and set to item data
					TempComp.Location = ReadInMultiLineData(&File, line, hwnd);
				}
				break;
				case 4: // Description
				{
					// read in multiline data and set to item data
					TempComp.Description = ReadInMultiLineData(&File, line, hwnd);
				}
				break;
				case 5: // Selectable
				{
					// set item boolean to data
					TempComp.Selectable = atoi(line.c_str()) == 1;
				}
				break;
				}
				// we have finished processing data for current item. Increment currentItem
				currentItem++;
				// if we have finished reading in all company data
				if (currentItem > 5)
				{
					// add company data to Companies
					Companies.push_back(TempComp);

					// reset currentItem for next company
					currentItem = 0;
				}
			}
			// read in next line
			getline(File, line);

			// update dialog
			UpdateSaveLoadDialog(hwnd);
		}
		
		/*
		current item to be read:
		0 - ID
		1 - Name
		2 - Description
		3 - Company ID
		4 - Logs
		*/
		currentItem = 0;
		line = "";
		// start reading positions from file
		// while we have not reached the end of the file
		Position TempPos; // holder for position data
		while (!File.eof())
		{
			// if we have information to process
			if (line != "")
			{
				// process data based on current item
				switch (currentItem)
				{
				case 0: // ID
				{
					TempPos.ID = atoi(line.c_str());
				}
				break;
				case 1: // Name
				{
					TempPos.Name = line;
				}
				break;
				case 2: // Description
				{
					// read in multiline data and set to item data
					TempPos.Description = ReadInMultiLineData(&File, line, hwnd);
				}
				break;
				case 3: // Company ID
				{
					
					TempPos.CompanyID = atoi(line.c_str());
				}
				break;
				case 4: // Logs
				{
					/*
					current item to be read:
					0 - Title
					1 - Date
					2 - ReminderDate
					3 - Description
					4 - Reminder
					*/
					int currentLogItem = 0;
					// start reading companies from file
					// while we have not read in the escape character for Logs - ~
					Log TempLog; // holder for log data
					while (line != "~")
					{
						// if we have information to process
						if (line != "")
						{
							// process data based on current log item
							switch (currentLogItem)
							{
							case 0: // Title
							{
								TempLog.Title = line;;
							}
							break;
							case 1: // Date
							{
								TempLog.Date = line;
							}
							break;
							case 2: // ReminderDate
							{
								TempLog.ReminderDate = line;
							}
							break;
							case 3: // Description
							{
								// read in multiline data and set to item data
								TempLog.Description = ReadInMultiLineData(&File, line, hwnd);
							}
							break;
							case 4: // Reminder
							{
								// read in multiline data and set to item data
								TempLog.Reminder = ReadInMultiLineData(&File, line, hwnd);
							}
							break;
							}
							// we have finished processing data for current item. Increment currentLogItem
							currentLogItem++;
							// if we have finished reading in all log data
							if (currentLogItem > 4)
							{
								// add log data to Logs
								TempPos.Logs.push_back(TempLog);

								// reset currentLogItem for next log
								currentLogItem = 0;
							}
						}
						// read in next line
						getline(File, line);

						// update dialog
						UpdateSaveLoadDialog(hwnd);
					}
				}
				break;
				}
				// we have finished processing data for current item. Increment currentItem
				currentItem++;
				// if we have finished reading in all position data
				if (currentItem > 4)
				{
					// add company data to Positions
					Positions.push_back(TempPos);

					// reset currentItem for next position
					TempPos.Logs.clear();
					currentItem = 0;
				}
			}
			// read in next line
			getline(File, line);

			// update dialog
			UpdateSaveLoadDialog(hwnd);
		}

		// close file
		File.close();
	}
	else
	{
		// no file create one
		SaveTrackerData(hwnd);
	}
}

int TrackerData::Partition(int *Array, const int Start, const int End, bool (TrackerData::*Compare)(int CurrentItem, int Pivot)) {
	const int Mid = Start + (End - Start) / 2;
	const int Pivot = Array[Mid];
	// move the mid point value to the front.
	std::swap(Array[Mid], Array[Start]);
	int CurrentIndex = Start + 1;
	int CurrentEnd = End;
	while (CurrentIndex <= CurrentEnd) {
		while (CurrentIndex <= CurrentEnd && ((*this).*Compare)(Array[CurrentIndex],Pivot)) {
			CurrentIndex++;
		}

		while (CurrentIndex <= CurrentEnd && !((*this).*Compare)(Array[CurrentIndex], Pivot)) {
			CurrentEnd--;
		}

		if (CurrentIndex < CurrentEnd) {
			std::swap(Array[CurrentIndex], Array[CurrentEnd]);
		}
	}
	std::swap(Array[CurrentIndex - 1], Array[Start]);
	return CurrentIndex - 1;
}
void TrackerData::Quicksort(int *Array, const int Start, const int End, bool (TrackerData::*Compare)(int CurrentItem, int Pivot), const int SortIndex) {

	if (Start >= End) {
		return;
	}


	int part = Partition(Array, Start, End, Compare);

	Quicksort(Array, Start, part - 1, Compare, SortIndex);
	Quicksort(Array, part + 1, End, Compare, SortIndex);
}

// compare functions
bool TrackerData::CompareCompanyNames(int CurrentItem, int Pivot)
{
	// passed paramenters are IDs
	Company PivotCompany = GetCompany(Pivot);
	Company CurrentCompany = GetCompany(CurrentItem);

	// compare company names
	return CurrentCompany.Name.compare(PivotCompany.Name) <= 0;
}


// General Methods
int TrackerData::CreateID()
{
	// randomly select an ID between 0 and ID_MAX
	return rand() % ID_MAX;
}
bool TrackerData::ValdiateCompanyID(int ID)
{
	bool Valid = true;

	// step through all company items checking if ID has already been used
	for (int i = 0; i < Companies.size(); i++)
	{
		// if item ID is equal to passed ID
		if (Companies[i].ID == ID)
		{
			// ID not valid
			Valid = false;
			break;
		}
	}

	return Valid;
}
int TrackerData::FindCompanyIndex(int ID)
{
	// step through all company items to find passed it
	for (int i = 0; i < Companies.size(); i++)
	{
		// if item ID is equal to passed ID
		if (Companies[i].ID == ID)
		{
			// return item index
			return i;
		}
	}

	return -1;
}
bool TrackerData::ValdiatePositionID(int ID)
{
	bool Valid = true;

	// step through all position items checking if ID has already been used
	for (int i = 0; i < Positions.size(); i++)
	{
		// if item ID is equal to passed ID
		if (Positions[i].ID == ID)
		{
			// ID not valid
			Valid = false;
			break;
		}
	}

	return Valid;
}
int TrackerData::FindPositionIndex(int ID)
{
	// step through all position items to find passed it
	for (int i = 0; i < Positions.size(); i++)
	{
		// if item ID is equal to passed ID
		if (Positions[i].ID == ID)
		{
			// return item index
			return i;
		}
	}

	return -1;
}

// Company Methods
Company TrackerData::GetCompany(int ID)
{
	// Get Index of Company based on passed ID and return item
	return Companies[FindCompanyIndex(ID)];
}
int TrackerData::CreateCompany(std::string Name, std::string Location, std::string Phone, std::string Description)
{
	// create a temp Item to populate data
	Company TempComp;

	// populate temp Item with passed data
	TempComp.Name = Name;
	TempComp.Location = Location;
	TempComp.Phone = Phone;
	TempComp.Description = Description;
	TempComp.Selectable = true;

	// generate a new ID for Company
	int ID = -1;
	// while we do not have a valid ID
	while (ID == -1)
	{
		// create a new ID
		ID = CreateID();
		// check ID validity
		if (!ValdiateCompanyID(ID))
		{
			// ID was not valid
			ID = -1;
		}
	}

	// give temp Item new ID
	TempComp.ID = ID;

	// add populated temp Item to Data List
	Companies.push_back(TempComp);

	// Sort Companies
	SortCompanies();

	return ID;
}
void TrackerData::UpdateCompany(int ID, std::string Name, std::string Location, std::string Phone, std::string Description, bool Selectable)
{
	bool Sort = false; // true if we must sort companies after update

	// find index of passed item ID
	int Index = FindCompanyIndex(ID);
	
	// create temp Item to populate data with
	Company TempComp = Companies[Index];

	// check if sort is needed
	if (TempComp.Name != Name)
		Sort = true; // sort after we a done updating

	// populate temp Item with passed data
	TempComp.Name = Name;
	TempComp.Location = Location;
	TempComp.Phone = Phone;
	TempComp.Description = Description;
	TempComp.Selectable = Selectable;

	// update populated temp Item to Data List
	Companies[Index] = (TempComp);

	if (Sort)
		SortCompanies();
}
void TrackerData::SortCompanies()
{
	// create a list of IDs
	std::vector<int> IDList;
	for (int i = 0; i < Companies.size(); i++)
	{
		IDList.push_back(Companies[i].ID);
	}

	// Sort List IDs
	Quicksort(&IDList[0], 0, Companies.size()-1, &TrackerData::CompareCompanyNames, 0);

	// Sort List using Sorted IDList
	std::vector<Company> NewList;
	for (int i = 0; i < Companies.size(); i++)
	{
		// find company with correct ID
		Company TempComp = GetCompany(IDList[i]);

		// add company to NewList
		NewList.push_back(TempComp);
	}

	// clear old list
	Companies.clear();
	// set old list to new list
	Companies.swap(NewList);
}

// Position Methods
Position TrackerData::GetPosition(int ID)
{
	// Get Index of Position based on passed ID and return item
	return Positions[FindPositionIndex(ID)];
}
int TrackerData::CreatePosition(std::string Name, std::string Description, int CompanyID)
{
	// create a temp Item to populate data
	Position TempPos;

	// populate temp Item with passed data
	TempPos.Name = Name;
	TempPos.Description = Description;
	TempPos.CompanyID = CompanyID;

	// generate a new ID for Position
	int ID = -1;
	// while we do not have a valid ID
	while (ID == -1)
	{
		// create a new ID
		ID = CreateID();
		// check ID validity
		if (!ValdiatePositionID(ID))
		{
			// ID was not valid
			ID = -1;
		}
	}

	// give temp Item new ID
	TempPos.ID = ID;

	// add populated temp Item to Data List
	Positions.push_back(TempPos);

	return ID;
}
void TrackerData::UpdatePosition(int ID, std::string Name, std::string Description, int CompanyID)
{
	// find index of passed item ID
	int Index = FindPositionIndex(ID);

	// create temp Item to populate data with
	Position TempPos = Positions[Index];

	// populate temp Item with passed data
	TempPos.Name = Name;
	TempPos.Description = Description;
	TempPos.CompanyID = CompanyID;
	
	// update populated temp Item to Data List
	Positions[Index] = (TempPos);
}
void TrackerData::DeletePosition(int ID)
{
	Positions.erase(Positions.begin() + FindPositionIndex(ID));
}
void TrackerData::CreateLog(int PositionID, std::string LogTitle, std::string LogDescription, std::string LogReminder, std::string LogDate, std::string LogReminderDate)
{
	// Get Position Logs to update
	std::vector<Log>* Logs = GetPositionLogs(PositionID);

	// create temp Log
	Log TempLog;

	// populate TempLog
	TempLog.Title = LogTitle;
	TempLog.Description = LogDescription;
	TempLog.Reminder = LogReminder;
	TempLog.Date = LogDate;
	TempLog.ReminderDate = LogReminderDate;

	// add new log to Position Logs
	Logs->push_back(TempLog);
}
std::vector<Log>* TrackerData::GetPositionLogs(int ID)
{
	return &Positions[FindPositionIndex(ID)].Logs;
}
std::string TrackerData::GetPositionRecentLogDate(int ID)
{
	// get Position of passed ID
	Position position = GetPosition(ID);

	// get the most recent record
	return position.Logs[position.Logs.size() - 1].Date;
}
std::string TrackerData::GetPositionRecentLogTitle(int ID)
{
	// get Position of passed ID
	Position position = GetPosition(ID);

	// get the most recent record
	return position.Logs[position.Logs.size() - 1].Title;
}
std::string TrackerData::GetPositionRecentReminderDate(int ID)
{
	// get Position of passed ID
	Position position = GetPosition(ID);

	// get the most recent record
	return position.Logs[position.Logs.size() - 1].ReminderDate;
}
std::string TrackerData::GetPositionApplyDate(int ID)
{
	// get Position of passed ID
	Position position = GetPosition(ID);

	// find the Log of the position that match the LogAppliedTitle string
	for (int i = 0; i < position.Logs.size(); i++)
	{
		// if this log's title matches the LogAppliedTitle string
		if (position.Logs[i].Title == LogAppliedTitle)
		{
			// this is the log we want
			return position.Logs[i].Date;
		}
	}
	return "";
}
