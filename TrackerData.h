#pragma once
#include "Job Tracker.h"
#include <string>
#include <vector>
#include <ctime>
#include <fstream>

extern UINT TD_DIALOG_UPDATE; // message sent from the background to the save load dialog to force an update
extern UINT TD_DIALOG_SETUP; // message sent from the background to the save load dialog to indicate dialog setup values are set
struct Company
{
	int ID;
	bool Selectable;
	std::string Name;
	std::string Location;
	std::string Phone;
	std::string Description;
};
struct Log
{
	std::string Title;
	std::string Description;
	std::string Reminder;
	std::string Date;
	std::string ReminderDate;
};
struct Position
{
	int ID;
	std::string Name;
	std::string Description;
	int CompanyID;
	std::vector<Log> Logs;
};


class TrackerData
{
private:
	const int ID_MAX = 10000; // highest a Data ID can go
	
	std::vector<Company> Companies;
	std::vector<Position> Positions;

	
	std::string ReadInMultiLineData(std::ifstream *File, std::string line, HWND hwnd);

	int CreateID();
	int Partition(int *Array, const int Start, const int End, bool (TrackerData::*Compare)(int CurrentItem, int Pivot));
	void Quicksort(int *Array, const int Start, const int End, bool (TrackerData::*Compare)(int CurrentItem, int Pivot), const int SortIndex);
	void UpdateSaveLoadDialog(HWND hwnd);


	// Company Methods
	int FindCompanyIndex(int ID); // finds vector index of Company based on given ID
	bool ValdiateCompanyID(int ID); // checks for no duplication in Company IDs
	bool CompareCompanyNames(int CurrentItem, int Pivot); // compares two Companies to see if Pivot's Name is first alphabetically

	// Position Methods
	int FindPositionIndex(int ID); // finds vector index of Position based on given ID
	bool ValdiatePositionID(int ID); // checks for no duplication in Position IDs
	
	

public:
	const std::string LogDateFormater = "%02d/%02d/%d"; // Used to Convert Dates to and from strings
	const std::string LogAppliedTitle = "Applied"; // Title that denotes Application Log

	int LoadMax;
	int LoadCurrent;

	TrackerData()
	{
		// seed rand
		srand(time(NULL));

		/*// load data from file
		LoadTrackerData();*/

		// register custom messsages
		TD_DIALOG_UPDATE = RegisterWindowMessage("Save Load Dialog Update");
		TD_DIALOG_SETUP = RegisterWindowMessage("Save Load Dialog Setup");

		// debug stuff
		/*Company tempComp;
		tempComp.ID = 0;
		tempComp.Description = "dafs";
		tempComp.Location = "asdfasd asdf";
		tempComp.Name = "test";
		tempComp.Phone = "123";
		tempComp.Selectable = false;
		Companies.push_back(tempComp);
		tempComp.ID = 1;
		tempComp.Description = "daaaasdffs";
		tempComp.Location = "asdfassssd asdf";
		tempComp.Name = "asd";
		tempComp.Phone = "14423";
		tempComp.Selectable = true;
		Companies.push_back(tempComp);

		/*Position tempPos;
		Log tempLog;
		tempPos.ID = 0;
		tempPos.Description = "test1 des";
		tempPos.Name = "test1";
		tempPos.CompanyID = 1;

		tempLog.Title = LogAppliedTitle;
		tempLog.Date = "02/03/2018";
		tempLog.Description = "I\r\nApplied\r\nfor\r\nthis\r\nPosition.";
		tempLog.ReminderDate = "02/10/2018";
		tempLog.Reminder = "Remember\r\nto\r\nCheck Up\r\nfor\r\nthis\r\nPosition.";
		tempPos.Logs.push_back(tempLog);
		Positions.push_back(tempPos);

		tempPos.ID = 1;
		tempPos.Description = "testb des";
		tempPos.Name = "testb";
		tempPos.CompanyID = 0;
		tempPos.Logs.clear();

		tempLog.Title = LogAppliedTitle;
		tempLog.Date = "02/04/2018";
		tempLog.Description = "I\r\nApplied\r\nfor\r\nthis\r\nPosition.";
		tempLog.ReminderDate = "02/11/2018";
		tempLog.Reminder = "Remember\r\nto\r\nCheck Up\r\nfor\r\nthis\r\nPosition.";
		tempPos.Logs.push_back(tempLog);
		tempLog.Title = "Custom1";
		tempLog.Date = "02/12/2018";
		tempLog.Description = "Do\r\nSomething\r\nfor\r\nthis\r\nPosition.";
		tempLog.ReminderDate = "02/23/2018";
		tempLog.Reminder = "Remember\r\nto\r\nCheck Up\r\nfor\r\nthis\r\nPosition.";
		tempPos.Logs.push_back(tempLog);
		Positions.push_back(tempPos);*/
	}
	~TrackerData()
	{
		/*// save data to file
		SaveTrackerData();*/
		
	}

	// General Methods
	void SaveTrackerData(HWND hwnd = NULL);
	void LoadTrackerData(HWND hwnd = NULL);
	inline std::string ConvertDateToString(SYSTEMTIME Date)
	{
		// Convert Date into String
		char buffer[256];
		sprintf_s(buffer, LogDateFormater.c_str(),
			Date.wMonth,
			Date.wDay,
			Date.wYear);
		std::string StringDate = buffer;
		return StringDate;
	}
	inline SYSTEMTIME ConvertStringToDate(char* StringDate)
	{
		// Convert String into Date
		SYSTEMTIME Date;
		sscanf_s(StringDate, LogDateFormater.c_str(),
			&Date.wMonth,
			&Date.wDay,
			&Date.wYear);
		return Date;
	}

	// Company Methods
	inline std::vector<Company>* GetCompanies()
	{
		return &Companies;
	}
	Company GetCompany(int ID);
	int CreateCompany(std::string Name, std::string Location, std::string Phone, std::string Description);
	void UpdateCompany(int ID, std::string Name, std::string Location, std::string Phone, std::string Description, bool Selectable = true);
	void SortCompanies(); // sorts the companies list data
	void SortCompanies(std::vector<Company>* PCompanies); // copies the companies list into the passed list and sorts that

	// Position Methods
	inline std::vector<Position>* GetPositions()
	{
		return &Positions;
	}
	Position GetPosition(int ID);
	int CreatePosition(std::string Name, std::string Description, int CompanyID);
	void UpdatePosition(int ID, std::string Name, std::string Description, int CompanyID);
	void DeletePosition(int ID);
	void CreateLog(int PositionID, std::string LogTitle, std::string LogDescription, std::string LogReminder, std::string LogDate, std::string LogReminderDate);
	std::vector<Log>* GetPositionLogs(int ID);
	std::string GetPositionRecentLogDate(int ID);
	std::string GetPositionRecentLogTitle(int ID);
	std::string GetPositionRecentReminderDate(int ID);
	std::string GetPositionApplyDate(int ID);

};

extern TrackerData trackerData;					// Handle to backend data storage
