#include"Process.h"
#include"stdafx.h"
#include<iostream>
#include<fstream>
#include<string>
using	namespace std;

#define PYTHON L"C:\\Users\\ZongHua\\AppData\\Local\\Programs\\Python\\Python36-32\\Lib\\idlelib\\idle.bat"    
#define CMD L"close"
PROCESS_INFORMATION	*	process_py() {
	//LPWSTR	CMD = (LPWSTR)L"open C:\\Users\\ZongHua\Desktop\\Èí¼þ±­\\Server_Analysis_Proj\\Server_Analysis_Proj\\Server_Analysis_Proj\\silk.py";
	//TCHAR	Commandline[] = TEXT("open silk.py");
	STARTUPINFO startup_info;
	GetStartupInfo(&startup_info);
	PROCESS_INFORMATION process_info;
	startup_info.dwFlags = STARTF_USESHOWWINDOW;
	startup_info.wShowWindow = TRUE;
	if (!CreateProcess(PYTHON, NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &startup_info, &process_info))
	{
		int code = GetLastError();
		cout << code << endl;
		ifstream	ifile;
		ifile.open("Windows_Error_Code.txt", ios::in);
		if (ifile.fail()) {
			cout << "open file error!" << endl;
			exit(0);
		}
		int	codes;
		string	code_info;
		while (!ifile.eof()) {
			ifile >> codes >> code_info;
			if (codes == code) {
				//cout << code_info << endl;
				break;
			}
		}
		ifile.close();
		cout << "Create Process Error £º" <<code_info<< endl;
	}
	//Sleep(5000);
	//TerminateProcess(process_info.hProcess, 0);
	return	&process_info;
}