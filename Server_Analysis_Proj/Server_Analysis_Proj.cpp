#include<WinSock2.h>
#include "stdafx.h"
#include"CPlusPlus.h"
#include"DataBase.h"
#include<atlconv.h>
#include"WDSocket.h"
#include"Thread_Pool.h"

#pragma comment(lib,"ws2_32.lib")

#define THREAD_STACK_SIZE	1024

/*Socket define*/
#define SERVER_PORT 2018
#define MAX_LISTEN_SZIE 1000
#define SERVER_SIZE 1000
#define RECV_SIZE 32
/*SVM define*/
#define _CHAR_SIZE 1
#define _READ_SIZE 1
#define MODEL_SZIE 1024

#define HEARTRATE 1
#define TEMPERATURE 2
#define WEIGHT 3
#define OFFSET 3

#define SVM_TRAIN_PATH ".\\svm\\svm-train.exe"
#define SVM_SCALE_PATH ".\\svm\\svm-scale.exe"

#define HEARTRATE_PREDICT_SIZE 5
#define TEMPERATURE_PREDICT_SIZE 5
#define WEIGHT_PREDICT_SIZE 1

#define UPDATE_HEARTRATE_VERSION "update oncar_user set hr_version = hr_version+1 where id="+id+""
#define UPDATE_TEMPERATURE_VERSION "update oncar_user set tp_version = tp_version+1 where id="+id+""
#define UPDATE_WEIGHT_VERSION "update oncar_user set model_version = model_version+1 where id="+id+""

#define SQL_CONNECT_STR "Provider=SQLOLEDB;Server=(local);Database=oncar;uid=oncar;pwd=nuaa2018NO1;"



struct svmparam {
	string TableName;
	int DataType;
	sClient *client;
};

ofstream outlog;

SYSTEMTIME systime;

DataBase DB(SQL_CONNECT_STR);

static int Bais_Int[2][5] = { {-5,-7,-9,-11,-13} , { 5, 7, 9, 11, 13 } };

static float Bais_Float[2][5] = { { -0.1,-0.3,-0.5,-0.7,-1.2 } ,{ 0.1, 0.3, 0.5, 0.7, 1.2 } };

/*Server thread*/
LPVOID WINAPI Server_Thread(LPVOID lpParam);

/*Return model*/
DWORD WINAPI Model_Ret(LPVOID lpParam);

/*Train*/
DWORD WINAPI Model_Train(LPVOID lpParam);

/*Set train data*/
DWORD WINAPI SetTrainData(LPVOID lpParam);

/*Initial network environment*/
bool InitNetEnv() {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		cout<<"WSAStartup failed!"<<endl;
		return false;
	}
	return true;
}

/*Log*/
void Write_To_Log(ofstream &outlog, string str) {

	string timeformat;
	GetLocalTime(&systime);
	timeformat = to_string(systime.wYear) + "." + to_string(systime.wMonth) + "." + 
				to_string(systime.wDay) + "." + to_string(systime.wHour) + "." +
				to_string(systime.wMinute) + "." + to_string(systime.wSecond) + "." +
				to_string(systime.wMilliseconds);

	outlog << timeformat <<": "<< str<<endl;

}

/*Simulate bad data*/
void Label_Bais_Int(string Label , int Length , string &BLabel) {

	//label data format : 1 1:76 2:75 3:79 4:84 5:76

	string str_int , streamits;
	stringstream streamsti;
	int Data[5];
	int bais;
	bais = rand() % 2;
	//streamits.assign(to_string((bais ? 1 : -1)));
	streamits.assign("1");
	for (int pt = 0; pt < Length; pt++)
	{
		Label = Label.substr(Label.find_first_of(':') + 1, string::npos);

		if (Label.find(' ') != string::npos)
			str_int = Label.substr(0, Label.find_first_of(' '));
		else
			str_int = Label;

		/*streamsti << str_int;

		streamsti >> Data[pt];*/

		Data[pt] = atoi(str_int.c_str());

		Data[pt] += Bais_Int[bais][pt];

		streamits.append(" ");
		streamits.append(to_string(pt + 1));
		streamits.append(":");
		streamits.append(to_string(Data[pt]));
	}
	BLabel.assign(streamits);
	Write_To_Log(outlog, "Bais_Int -> "+BLabel);
}

string float_to_string(float _Val)
{	// convert float to string
	return (_Floating_to_string("%.2f", _Val));
}

void Label_Bais_Float(string Label, int Length, string &BLabel) {

	//label data format : 1 1:37.2 2:36.9 3:37.1 4:37.2 5:37.3

	string str_int, streamits;
	stringstream streamsti;
	float Data[5];
	int bais;
	bais = rand() % 2;
	//streamits.assign(to_string((bais ? 1 : -1)));
	streamits.append("1");
	for (int pt = 0; pt < Length; pt++)
	{
		Label = Label.substr(Label.find_first_of(':') + 1, string::npos);

		if (Label.find(' ') != string::npos)
			str_int = Label.substr(0, Label.find_first_of(' '));
		else
			str_int = Label;

		/*streamsti << str_int;

		streamsti >> Data[pt];*/

		Data[pt] = atof(str_int.c_str());

		Data[pt] += Bais_Float[bais][pt];

		streamits.append(" ");
		streamits.append(to_string(pt + 1));
		streamits.append(":");
		streamits.append(float_to_string(Data[pt]));
	}
	BLabel.assign(streamits);
	Write_To_Log(outlog, "Bais_Float -> " + BLabel);
}

/*Main*/
int main(int argc, char **argv) {

	sockparam tparam;

	pool_init();	//default max_threads = 100

	if (!InitNetEnv()) {
		return -1;
	}

	outlog.open("./Log/log.txt", ios::out);	//start log

	WDSocket sSocket(SERVER_PORT);	//init socket port

	sSocket.WDsocket();	//init server socket

	if (sSocket.WDgetfd() == INVALID_SOCKET) {
		cout<<"Create Socket Failed!"<<endl;
		return -1;
	}
	
	sSocket.WDsa_init();	//init server addr

	if (sSocket.WDbind() == SOCKET_ERROR) {
		cout<<"Socket Bind Failed!"<<endl;
		WSACleanup();	//release network environment 
		sSocket.WDclose();
		return -1;
	}

	Write_To_Log(outlog, "Socket Bind Success!");

	if (sSocket.WDlisten(MAX_LISTEN_SZIE) == SOCKET_ERROR) {

		cout<<"Socket Listen Failed"<<endl;
		WSACleanup();
		sSocket.WDclose();
		return -1;

	}

	Write_To_Log(outlog, "Socket Listen Start!");
	cout << "*------------------------------*" << endl;
	cout << "*-----Copyright @ WD Team------*" << endl;
	cout << "*------------------------------*" << endl;
	cout << "*------------Start!------------*" << endl;
	cout << "*------------------------------*" << endl;
	while (true)
	{

		sClient client;

		sSocket.WDaccept();	//accept request from client

		client = sSocket.WDgetclient();

		if (client.WDvalid() == INVALID_SOCKET) {
			Write_To_Log(outlog, "Socket Accept Failed!");
			continue;
		}

		tparam.client = &client;

		pool_add_task(Server_Thread,(LPVOID)&tparam);	//task pool

	}

	return 0;


}

/*Server thread*/

LPVOID
WINAPI
Server_Thread(LPVOID lpParam) {

	Write_To_Log(outlog, "\n---------------------> Accept!");

	/*Set parameter*/
	sockparam tparam = *(sockparam *)lpParam;	//socket parameter

	sClient client = *tparam.client;

	string recvbuf , tybuf;

	int ret;

	ret = client.WDrecv(recvbuf, RECV_SIZE);	//recv request from tomcat

	if (ret == SOCKET_ERROR) {

		Write_To_Log(outlog, "Socket Recv Failed!");
		client.WDclose();
		ExitThread(1);

	}
	if (ret == 0) {
		Write_To_Log(outlog, "Client Close Connection!");
		client.WDclose();
		ExitThread(1);
	}

	Write_To_Log(outlog, "Recv Str: " + recvbuf);

	/*Analyse recv data*/

	int datatp;

	tybuf = recvbuf.substr(recvbuf.find_first_of("|")+1,string::npos);

	datatp = atoi(tybuf.c_str());

	Write_To_Log(outlog, to_string(datatp));

	recvbuf = recvbuf.substr(0,recvbuf.find_last_of("|"));

	svmparam tp;	//svm parameter

	tp.TableName = recvbuf;	//table name
	tp.DataType  = datatp ;	//user data type
	tp.client    = &client;	//socket

	/*Analyse request type*/

	if		(datatp >= 4 && datatp <= 6) goto RetModel  ;	//return model
	else if (datatp >= 1 && datatp <= 3) goto TrainModel;	//train  model
	else								 goto ErrorProc ;	//error  proc

TrainModel:

	Write_To_Log(outlog, "Model_Train For -> " + tp.TableName);
	Model_Train((LPVOID)&tp);

	goto Exit;

RetModel:

	Write_To_Log(outlog, "Model_Ret For -> " + tp.TableName);
	Model_Ret((LPVOID)&tp);

	goto Exit;

ErrorProc:

	Write_To_Log(outlog, "Unknown DataType -> " + to_string(datatp));
	client.WDclose();

Exit:	//end task

	return 0;

}

/*Return model*/

DWORD
WINAPI
Model_Ret(LPVOID lpParam) {

	/*Get parameter*/
	svmparam tp = *(svmparam *)lpParam;
	sClient client = *tp.client;
	string ModelFile;
	string ModelStr;
	stringstream streambuffer;
	ifstream OpenFile;
	int i = 0;
	switch (tp.DataType - OFFSET)
	{
		case HEARTRATE:
			ModelFile = tp.TableName;
			ModelFile = ModelFile.substr(ModelFile.find_last_of('_') + 1, string::npos);
			ModelFile = "./Train/heartrate/heartrate_model_" + ModelFile;
			break;
		case TEMPERATURE:
			ModelFile = tp.TableName;
			ModelFile = ModelFile.substr(ModelFile.find_last_of('_') + 1, string::npos);
			ModelFile = "./Train/temperature/temperature_model_" + ModelFile;
			break;
		case WEIGHT:
			ModelFile = tp.TableName;
			ModelFile = ModelFile.substr(ModelFile.find_last_of('_') + 1, string::npos);
			ModelFile = "./Train/weight/weight_model_" + ModelFile;
			break;
		default:
			break;
	}

	/*Get user model*/

	OpenFile.open(ModelFile.c_str(), ios::in);
	if (!OpenFile.is_open()) {
		Write_To_Log(outlog, "File Open ERROR! "+ModelFile);
		client.WDclose();
		return -1;
	}
	
	streambuffer << OpenFile.rdbuf();

	ModelStr = streambuffer.str();

	OpenFile.close();

	int ret;

	ret = client.WDsend(ModelStr.c_str(), ModelStr.size());	//send model to tomcat

	if (ret == SOCKET_ERROR) {
		Write_To_Log(outlog, "Socket Send Failed!");
		client.WDclose();
		return -1;
	}

	Write_To_Log(outlog, "Send Model Success!");

	client.WDclose();

	return 0;
}

/*Model train*/

DWORD
WINAPI
Model_Train(LPVOID	lpParam) {

	/*Get parameter*/
	svmparam tp = *(svmparam *)lpParam;
	sClient client = *tp.client;

	int Cnt;

	string strSQL = "select * from " + (string)tp.TableName + " order by id DESC";	//select data order by desc

	vector<heartrate>	HeartRate;
	vector<temperature> Temperature;
	vector<weight>		Weight;
	vector<heartrate>::iterator		hit;
	vector<temperature>::iterator	tit;
	vector<weight>::iterator		wit;

	string InputFile , GridFile , ModelFile , ParamLine , Tstr;
	ofstream WriteFile;
	ifstream ReadFile;

	double c, g;	//train parameter

	SHELLEXECUTEINFO ShExecInfo = { 0 };
	USES_CONVERSION;	//convert asii to unicode

	SetTrainData((LPVOID)&tp);	

	switch (tp.DataType)
	{
		/*Train hreatrate_model*/
		case HEARTRATE:
			/*Select DataBase*/
			try
			{
				DB.DB_Select_HeartInfo(strSQL.c_str());
				HeartRate = DB.DB_ReturnHeartRate();
			}
			catch (string &Exception)
			{
				Write_To_Log(outlog, "DataBase ERROR -> "+ Exception);
				client.WDclose();
				return -1;
			}

			InputFile = tp.TableName;
			InputFile = InputFile.substr(InputFile.find_last_of('_') + 1, string::npos);
			ModelFile = "./Train/heartrate/heartrate_model_" + InputFile;
			GridFile = "./Input/heartrate/heartrate_input_" + InputFile+".out";
			InputFile = "./Input/heartrate/heartrate_input_" + InputFile;

			/*Write train data into file*/
			WriteFile.open(InputFile.c_str(),ios::out);
			for (Cnt = 0, hit = HeartRate.begin();Cnt < 25 && hit != HeartRate.end(); hit++ , Cnt ++) {
				
				WriteFile << hit->HeartrateStr <<endl;

			}
			WriteFile.close();
			break;
		/*Train temperature_model*/
		case TEMPERATURE:
			/*Select DataBase*/
			try
			{
				DB.DB_Select_TempInfo(strSQL.c_str());
				Temperature = DB.DB_ReturnTemp();
			}
			catch (string &Exception)
			{
				Write_To_Log(outlog, "DataBase ERROR -> " + Exception);
				client.WDclose();
				return -1;
			}
			
			InputFile = tp.TableName;
			InputFile = InputFile.substr(InputFile.find_last_of('_') + 1, string::npos);
			ModelFile = "./Train/temperature/temperature_model_" + InputFile;
			GridFile = "./Input/temperature/temperature_input_" + InputFile + ".out";
			InputFile = "./Input/temperature/temperature_input_" + InputFile;

			/*Write train data into file*/
			WriteFile.open(InputFile.c_str(), ios::out);
			for (Cnt = 0 , tit = Temperature.begin();Cnt < 25 && tit != Temperature.end(); tit++ , Cnt++) {
				
				WriteFile << tit->TemperatureStr<<endl;
			}
			WriteFile.close();
			break;
		/*Train weight_model*/
		case WEIGHT:
			try
			{
				DB.DB_Select_WeightInfo(strSQL.c_str());
				Weight = DB.DB_ReturnWeight();
			}
			catch (string &Exception)
			{
				Write_To_Log(outlog, "DataBase ERROR -> " + Exception);
				client.WDclose();
				return -1;
			}
		
			InputFile = tp.TableName;
			InputFile = InputFile.substr(InputFile.find_last_of('_') + 1, string::npos);
			ModelFile = "./Train/weight/weight_model_" + InputFile;
			GridFile = "./Input/weight/weight_input_" + InputFile + ".out";
			InputFile = "./Input/weight/weight_input_" + InputFile;

			/*Write train data into file*/
			WriteFile.open(InputFile.c_str(), ios::out);
			for (Cnt = 0 , wit = Weight.begin();Cnt < 25 && wit != Weight.end(); wit++ , Cnt++) {
				
				WriteFile << wit->WeightStr.c_str() << endl;
			}
			WriteFile.close();
			break;
		default:
			break;
	}

	//python arguments.py -svmtrain svm-train.exe inputfile

	Write_To_Log(outlog,"Start Grid");

	Tstr.append("Grid\\arguments.py -gnuplot \"null\" -out \"null\" -svmtrain svm\\svm-train.exe ");
	Tstr.append(InputFile);

	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = _T("open");
	ShExecInfo.lpFile = _T("python");
	ShExecInfo.lpParameters = A2W(Tstr.c_str());
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;

	ShellExecuteEx(&ShExecInfo);	//shell exec

	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);	//wait grid finish

	//Deal with best parameter

	ReadFile.open(GridFile.c_str(),ios::in);

	ReadFile >> c >> g;

	ReadFile.close();


	//svm-train.exe [options] training_set_file [model_file]

	Write_To_Log(outlog, "Start Train");

	/*Set Parameter*/
	ParamLine.append(" ");
	ParamLine.append("-s");
	ParamLine.append(" ");
	ParamLine.append("0");
	ParamLine.append(" ");
	ParamLine.append("-c");
	ParamLine.append(" ");
	ParamLine.append(to_string(c));
	ParamLine.append(" ");
	ParamLine.append("-g");
	ParamLine.append(" ");
	ParamLine.append(to_string(g));
	ParamLine.append(" " + InputFile + " " + ModelFile);

	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = _T("open");
	ShExecInfo.lpFile = _T(SVM_TRAIN_PATH);
	ShExecInfo.lpParameters = A2W(ParamLine.c_str());
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;

	ShellExecuteEx(&ShExecInfo);	//shell exec

	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);	//wait training finish

	/*Update model version*/

	Write_To_Log(outlog, "Update Model Version");

	string id = tp.TableName;
	id = id.substr(id.find_last_of('_') + 1, string::npos);

	switch (tp.DataType)
	{
		case HEARTRATE:
			strSQL.assign(UPDATE_HEARTRATE_VERSION);//update heartrate model version
			break;
		case TEMPERATURE:
			strSQL.assign(UPDATE_TEMPERATURE_VERSION);//update temperature model version
			break;
		default:
			break;
	}

	try
	{
		DB.DB_Update(strSQL.c_str());	//update
	}
	catch (string &Exception)
	{
		Write_To_Log(outlog, "DataBase ERROR -> " + Exception);
		client.WDclose();
		return -1;
	}

	/*Return ack*/

	int ret;

	ret = client.WDsend("1",2);	//return ack

	if (ret == SOCKET_ERROR) {
		Write_To_Log(outlog, "Ack Send Failed!");
		client.WDclose();
		return -1;
	}

	Write_To_Log(outlog, "Ret Ack");

	client.WDclose();

	return 0;
}

/*Set train data*/

DWORD
WINAPI
SetTrainData(LPVOID lpParam) {

	/*Get parameter*/
	svmparam tp = *(svmparam *)lpParam;
	sClient client = *tp.client;
	string id(tp.TableName);

	id = id.substr(id.find_last_of('_') + 1, string::npos);

	string verSQL;
	int Version;

	switch (tp.DataType)
	{
	case HEARTRATE:
		verSQL = "select hr_version from oncar_user where id=" + id;//select hreatrate model version
		DB.DB_Select_Version_Hr(verSQL.c_str());
		Version = DB.DB_ReturnVersion_Hr();
		Write_To_Log(outlog, "HR Model Version -> " + to_string(Version));
		if (Version > 0) return 1;	//version = 0 -> first time
		break;
	case TEMPERATURE:
		verSQL = "select tp_version from oncar_user where id=" + id;//select temperature model version
		DB.DB_Select_Version_Tp(verSQL.c_str());
		Version = DB.DB_ReturnVersion_Tp();
		Write_To_Log(outlog, "TP Model Version -> " + to_string(Version));
		if (Version > 0) return 1;	//version = 0 -> first time
		break;
	default:
		break;
	}

	Write_To_Log(outlog, "Set Train Data -> " + tp.TableName);
	
	/*Set simulated data*/

	srand((unsigned)time(NULL));	//set random seed

	vector<temperature> Temperature;
	vector<heartrate>	HeartRate;
	vector<weight>		Weight;
	vector<heartrate>::iterator		hit;
	vector<temperature>::iterator	tit;
	vector<weight>::iterator		wit;

	string strSQL = "select * from "+ (string)tp.TableName + " order by id DESC";	//select data order by desc

	int Cnt;
	
	switch (tp.DataType)
	{
		case HEARTRATE:
			/*Select DataBase*/
			try{
				DB.DB_Select_HeartInfo(strSQL.c_str());
				HeartRate = DB.DB_ReturnHeartRate();
			}
			catch (string &Exception){
				Write_To_Log(outlog, "DataBase Select ERROR -> " + Exception);
				client.WDclose();
				ExitThread(1);
			}

			for (Cnt = 0 , hit = HeartRate.begin() ;Cnt < 10 && hit != HeartRate.end() ; hit ++ , Cnt ++){
				string BLabel;
				Label_Bais_Int(hit->HeartrateStr,5,BLabel);	//new data label
				string insSQL = "insert into " + tp.TableName + "(heartrate) Values('" + BLabel + "')";
				try{
					DB.DB_Insert(insSQL.c_str());
				}
				catch (string &Exception){
					Write_To_Log(outlog, "DataBase Insert ERROR -> " + Exception);
					client.WDclose();
					ExitThread(1);
				}
			}
			break;
		case TEMPERATURE:
			/*Select DataBase*/
			try {
				DB.DB_Select_TempInfo(strSQL.c_str());
				Temperature = DB.DB_ReturnTemp();
			}
			catch (string &Exception) {
				Write_To_Log(outlog, "DataBase Select ERROR -> " + Exception);
				client.WDclose();
				ExitThread(1);
			}

			for (Cnt = 0 , tit = Temperature.begin();Cnt < 10 && tit != Temperature.end(); tit++ , Cnt ++) {
				string BLabel;
				Label_Bais_Float(tit->TemperatureStr, 5, BLabel);	//new data label
				string insSQL = "insert into " + tp.TableName + "(temperature) Values('" + BLabel + "')";
				try {
					DB.DB_Insert(insSQL.c_str());
				}
				catch (string Exception) {
					Write_To_Log(outlog, "DataBase Insert ERROR -> " + Exception);
					client.WDclose();
					ExitThread(1);
				}
			}
			break;
		case WEIGHT:
			/*Select DataBase*/
			try {
				DB.DB_Select_WeightInfo(strSQL.c_str());
				Weight = DB.DB_ReturnWeight();
			}
			catch (string Exception) {
				Write_To_Log(outlog, "DataBase Select ERROR -> " + Exception);
				client.WDclose();
				ExitThread(1);
			}

			for (Cnt = 0 , wit = Weight.begin();Cnt < 10 && wit != Weight.end(); wit++ , Cnt ++) {
				string BLabel;
				Label_Bais_Int(wit->WeightStr, 5, BLabel);	//new data label
				string insSQL = "insert into " + tp.TableName + "(weight) Values('" + BLabel + "')";
				try {
					DB.DB_Insert(insSQL.c_str());
				}
				catch (string Exception) {
					Write_To_Log(outlog, "DataBase Insert ERROR -> " + Exception);
					client.WDclose();
					ExitThread(1);
				}
			}
			break;
		default:
			break;
	}

	Write_To_Log(outlog, "Set Train Data Finish -> " + tp.TableName);

	return 0;
}