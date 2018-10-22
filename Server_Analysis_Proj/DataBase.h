#pragma once
#include"stdafx.h"
#include <sql.h>       
#include <sqlext.h>       
#include <sqltypes.h>       
#include <odbcss.h>   
#include "iomanip"
#include"CPlusPlus.h"

#import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","EndOfFile")

struct oncar{
	string username ;
	string nickname	;
	string password	;
	string EMERcontact_1;
	string EMERcontact_2;
	int height;
};

struct heartrate {
	string	time;
	string	HeartrateStr;
};

struct temperature {
	string time;
	string TemperatureStr;
};
struct weight {
	string time;
	string WeightStr;
};

class DataBase
{
public:
	DataBase(_bstr_t StrConnect);					//init with database connection string
	~DataBase();
	void	DB_Select_UserInfo(_bstr_t	bstrSQL);	//select user info
	void	DB_Select_TempInfo(_bstr_t bstrSQL);	//select user temperature data
	void	DB_Select_HeartInfo(_bstr_t bstrSQL);	//select user heartrate data
	void	DB_Select_WeightInfo(_bstr_t bstrSQL);	//select user weight data
	void	DB_Select_Version_Hr(_bstr_t bstrSQL);	//select heartrate model version
	void	DB_Select_Version_Tp(_bstr_t bstrSQL);	//select temperature model version
	void	DB_Update(_bstr_t	bstrSQL);			//update data label
	void	DB_Delete(_bstr_t	bstrSQL);			//delete data label
	void	DB_Insert(_bstr_t	bstrSQL);			//insert new data label
	void	DB_NewTable(_bstr_t	bstrSQL);			//create a new table
	void	DB_DeleteTable(_bstr_t	bstrSQL);		//delete a table
	/*return variable*/
	vector<oncar>		DB_ReturnOnCar();
	vector<temperature> DB_ReturnTemp();
	vector<heartrate>	DB_ReturnHeartRate();
	vector<weight>		DB_ReturnWeight();
	int					DB_ReturnVersion_Hr();
	int					DB_ReturnVersion_Tp();

private:
	vector<oncar>		OnCar;
	vector<heartrate>	HeartRate;
	vector<temperature> Temperature;
	vector<weight>		Weight;
	_bstr_t				strConnect;
	int					Tp_Version;
	int					Hr_Version;
};

