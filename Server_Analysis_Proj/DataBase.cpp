#include "stdafx.h"
#include "DataBase.h"
/*Init DataBase*/
DataBase::DataBase(_bstr_t StrConnect)
{
	strConnect = StrConnect;
}
/*Free DataBase*/
DataBase::~DataBase()
{
}
/*Select from OnCar table*/
void	DataBase::DB_Select_UserInfo(_bstr_t	bstrSQL) {
	
	::CoInitialize(NULL);//初始化OLE/COM库环境，为访问ADO接口做准备
	_RecordsetPtr m_pRecordset("ADODB.Recordset");
	_ConnectionPtr m_pConnection("ADODB.Connection");
	try
	{
		//创建Connection对象
		m_pConnection.CreateInstance("ADODB.Connection");
		//连接数据库
		m_pConnection->Open(strConnect, "", "", adModeUnknown);
		if (m_pConnection == NULL)
		{
			cerr << "Lind data ERROR!\n";
		}
		//创建记录集  
		m_pRecordset.CreateInstance(_uuidof(Recordset));
		//取得表中的记录  
		m_pRecordset->Open(bstrSQL, m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
		_variant_t UserName, NickName,PassWord,EMERContact_1,EMERContact_2;
		int height;
		while (!m_pRecordset->EndOfFile)
		{
			UserName = m_pRecordset->GetCollect("username");
			NickName = m_pRecordset->GetCollect("nickname");
			PassWord = m_pRecordset->GetCollect("password");
			EMERContact_1 = m_pRecordset->GetCollect("EMERcontact_1");
			EMERContact_2 = m_pRecordset->GetCollect("EMERcontact_2");
			height = m_pRecordset->GetCollect("height");
			if (UserName.vt != VT_NULL && NickName.vt != VT_NULL)
			{
				oncar	TOnCar;
				TOnCar.username = (char*)(_bstr_t)UserName;
				TOnCar.nickname = (char*)(_bstr_t)NickName;
				TOnCar.password = (char *)(_bstr_t)PassWord;
				TOnCar.EMERcontact_1 = (char *)(_bstr_t)EMERContact_1;
				TOnCar.EMERcontact_2 = (char *)(_bstr_t)EMERContact_2;
				TOnCar.height = height;
				OnCar.push_back(TOnCar);
			}
			m_pRecordset->MoveNext(); ///移到下一条记录  
		}
		m_pRecordset->Close(); // 关闭记录集  
	}
	catch (_com_error c)
	{
		cerr << "\nDataBase ERROR:" << (char*)c.Description();
	}
	if (m_pConnection->State)
		m_pConnection->Close();

	::CoUninitialize();
}
/*Select from Temperature table*/
void	DataBase::DB_Select_TempInfo(_bstr_t bstrSQL) {

	::CoInitialize(NULL);//初始化OLE/COM库环境，为访问ADO接口做准备
	_RecordsetPtr m_pRecordset("ADODB.Recordset");
	_ConnectionPtr m_pConnection("ADODB.Connection");
	try
	{
		//创建Connection对象
		m_pConnection.CreateInstance("ADODB.Connection");
		//连接数据库
		m_pConnection->Open(strConnect, "", "", adModeUnknown);
		if (m_pConnection == NULL)
		{
			cerr << "Lind data ERROR!\n";
		}
		//创建记录集  
		m_pRecordset.CreateInstance(_uuidof(Recordset));
		//取得表中的记录  
		m_pRecordset->Open(bstrSQL, m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
		temperature STemperature;
		_variant_t vtemperature , time;
		while (!m_pRecordset->EndOfFile)
		{
			time = m_pRecordset->GetCollect("id");

			STemperature.time.assign((char *)(_bstr_t)time);

			vtemperature = m_pRecordset->GetCollect("temperature");

			STemperature.TemperatureStr.assign((_bstr_t)vtemperature);
			
			Temperature.push_back(STemperature);

			m_pRecordset->MoveNext(); ///移到下一条记录  
		}
		m_pRecordset->Close(); // 关闭记录集  
	}
	catch (_com_error c)
	{
		cerr << "\nDataBase ERROR:" << (char*)c.Description();
		throw("DataBase Select Temperature ERROR!");
	}
	if (m_pConnection->State)
		m_pConnection->Close();

	::CoUninitialize();

}
/*Select from HeartRate teble*/
void	DataBase::DB_Select_HeartInfo(_bstr_t bstrSQL) {

	::CoInitialize(NULL);//初始化OLE/COM库环境，为访问ADO接口做准备
	_RecordsetPtr m_pRecordset("ADODB.Recordset");
	_ConnectionPtr m_pConnection("ADODB.Connection");
	try
	{
		//创建Connection对象
		m_pConnection.CreateInstance("ADODB.Connection");
		//连接数据库
		m_pConnection->Open(strConnect, "", "", adModeUnknown);
		if (m_pConnection == NULL)
		{
			cerr << "Lind data ERROR!\n";
		}
		//创建记录集  
		m_pRecordset.CreateInstance(_uuidof(Recordset));
		//取得表中的记录  
		m_pRecordset->Open(bstrSQL, m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
		heartrate SHeartrate;
		_variant_t time;
		_variant_t vheartrate;
		while (!m_pRecordset->EndOfFile)
		{
			time = m_pRecordset->GetCollect("id");
			SHeartrate.time.assign((char *)(_bstr_t)time);
			vheartrate = m_pRecordset->GetCollect("heartrate");
			SHeartrate.HeartrateStr.assign((char *)(_bstr_t)vheartrate);
			HeartRate.push_back(SHeartrate);

			m_pRecordset->MoveNext(); ///移到下一条记录  
		}
		m_pRecordset->Close(); // 关闭记录集  
	}
	catch (_com_error c)
	{
		cerr << "\nDataBase ERROR:" << (char*)c.Description();
		throw("DataBase Select HeartRate ERROR!");
	}
	if (m_pConnection->State)
		m_pConnection->Close();

	::CoUninitialize();

}
/*Select from Weight table*/
void	DataBase::DB_Select_WeightInfo(_bstr_t bstrSQL) {

	::CoInitialize(NULL);//初始化OLE/COM库环境，为访问ADO接口做准备
	_RecordsetPtr m_pRecordset("ADODB.Recordset");
	_ConnectionPtr m_pConnection("ADODB.Connection");
	try
	{
		//创建Connection对象
		m_pConnection.CreateInstance("ADODB.Connection");
		//连接数据库
		m_pConnection->Open(strConnect, "", "", adModeUnknown);
		if (m_pConnection == NULL)
		{
			cerr << "Lind data ERROR!\n";
		}
		//创建记录集  
		m_pRecordset.CreateInstance(_uuidof(Recordset));
		//取得表中的记录  
		m_pRecordset->Open(bstrSQL, m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
		weight SWeight;
		_variant_t vwight , time ;
		while (!m_pRecordset->EndOfFile)
		{
			time = m_pRecordset->GetCollect("id");

			SWeight.time.assign((char *)(_bstr_t)time);

			vwight = m_pRecordset->GetCollect("weight");

			SWeight.WeightStr.assign((_bstr_t)vwight);
			
			Weight.push_back(SWeight);
			
			m_pRecordset->MoveNext(); ///移到下一条记录  
		}
		m_pRecordset->Close(); // 关闭记录集  
	}
	catch (_com_error c)
	{
		cerr << "\nDataBase ERROR:" << (char*)c.Description();
		throw("DataBase Select Weight ERROR!");
	}
	if (m_pConnection->State)
		m_pConnection->Close();

	::CoUninitialize();
}
/*Select from Oncar table*/
void	DataBase::DB_Select_Version_Hr(_bstr_t bstrSQL) {
	::CoInitialize(NULL);//初始化OLE/COM库环境，为访问ADO接口做准备
	_RecordsetPtr m_pRecordset("ADODB.Recordset");
	_ConnectionPtr m_pConnection("ADODB.Connection");
	try
	{
		//创建Connection对象
		m_pConnection.CreateInstance("ADODB.Connection");
		//连接数据库
		m_pConnection->Open(strConnect, "", "", adModeUnknown);
		if (m_pConnection == NULL)
		{
			cerr << "Lind data ERROR!\n";
		}
		//创建记录集  
		m_pRecordset.CreateInstance(_uuidof(Recordset));
		//取得表中的记录  
		m_pRecordset->Open(bstrSQL, m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
		while (!m_pRecordset->EndOfFile)
		{
			Hr_Version = m_pRecordset->GetCollect("hr_version");
			m_pRecordset->MoveNext(); ///移到下一条记录  
		}
		m_pRecordset->Close(); // 关闭记录集  
	}
	catch (_com_error c)
	{
		cerr << "\nDataBase ERROR:" << (char*)c.Description();
		throw("DataBase SELECT ERROR!" + bstrSQL);
	}
	if (m_pConnection->State)
		m_pConnection->Close();

	::CoUninitialize();
}
/*Select from Oncar  table*/
void	DataBase::DB_Select_Version_Tp(_bstr_t bstrSQL) {
	::CoInitialize(NULL);//初始化OLE/COM库环境，为访问ADO接口做准备
	_RecordsetPtr m_pRecordset("ADODB.Recordset");
	_ConnectionPtr m_pConnection("ADODB.Connection");
	try
	{
		//创建Connection对象
		m_pConnection.CreateInstance("ADODB.Connection");
		//连接数据库
		m_pConnection->Open(strConnect, "", "", adModeUnknown);
		if (m_pConnection == NULL)
		{
			cerr << "Lind data ERROR!\n";
		}
		//创建记录集  
		m_pRecordset.CreateInstance(_uuidof(Recordset));
		//取得表中的记录  
		m_pRecordset->Open(bstrSQL, m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
		while (!m_pRecordset->EndOfFile)
		{
			Tp_Version = m_pRecordset->GetCollect("tp_version");
			m_pRecordset->MoveNext(); ///移到下一条记录  
		}
		m_pRecordset->Close(); // 关闭记录集  
	}
	catch (_com_error c)
	{
		cerr << "\nDataBase ERROR:" << (char*)c.Description();
		throw("DataBase SELECT ERROR!" + bstrSQL);
	}
	if (m_pConnection->State)
		m_pConnection->Close();

	::CoUninitialize();
}
/*update form table*/
void	DataBase::DB_Update(_bstr_t	bstrSQL) {

	::CoInitialize(NULL);//初始化OLE/COM库环境，为访问ADO接口做准备
	_ConnectionPtr m_pConnection("ADODB.Connection");
	try
	{
		//创建Connection对象
		m_pConnection.CreateInstance("ADODB.Connection");
		//连接数据库
		m_pConnection->Open(strConnect, "", "", adModeUnknown);
		if (m_pConnection == NULL)
		{
			cerr << "Lind data ERROR!\n";
		}
		m_pConnection->Execute(bstrSQL, NULL, adCmdText);
	}
	catch (_com_error c)
	{
		cerr << "\nUPDATE	ERROR:" << (char*)c.Description();
		throw("DataBase UPDATE ERROR!" + bstrSQL);
	}
	if (m_pConnection->State)
		m_pConnection->Close();

	::CoUninitialize();
}
/*delete from table*/
void	DataBase::DB_Delete(_bstr_t	bstrSQL) {

	::CoInitialize(NULL);//初始化OLE/COM库环境，为访问ADO接口做准备
	_ConnectionPtr m_pConnection("ADODB.Connection");
	try
	{
		//创建Connection对象
		m_pConnection.CreateInstance("ADODB.Connection");
		//连接数据库
		m_pConnection->Open(strConnect, "", "", adModeUnknown);
		if (m_pConnection == NULL)
		{
			cerr << "Lind data ERROR!\n";
		}
		m_pConnection->Execute(bstrSQL, NULL, adCmdText);
	}
	catch (_com_error c)
	{
		cerr << "\nDELETE	ERROR:" << (char*)c.Description();
		throw("DataBase Delete ERROR!" + bstrSQL);
	}
	if (m_pConnection->State)
		m_pConnection->Close();

	::CoUninitialize();
}
/*insert into table*/
void	DataBase::DB_Insert(_bstr_t	bstrSQL) {

	::CoInitialize(NULL);//初始化OLE/COM库环境，为访问ADO接口做准备
	_ConnectionPtr m_pConnection("ADODB.Connection");
	try
	{
		//创建Connection对象
		m_pConnection.CreateInstance("ADODB.Connection");
		//连接数据库
		m_pConnection->Open(strConnect, "", "", adModeUnknown);
		if (m_pConnection == NULL)
		{
			cerr << "Lind data ERROR!\n";
		}
		m_pConnection->Execute(bstrSQL, NULL, adCmdText);
	}
	catch (_com_error c)
	{
		cerr << "\nINSERT	ERROR:" << (char*)c.Description();
		throw("DataBase Insert ERROR!" + bstrSQL);
	}
	if (m_pConnection->State)
		m_pConnection->Close();

	::CoUninitialize();
}
/*new table*/
void	DataBase::DB_NewTable(_bstr_t	bstrSQL) {

	::CoInitialize(NULL);//初始化OLE/COM库环境，为访问ADO接口做准备
	_ConnectionPtr m_pConnection("ADODB.Connection");
	try
	{
		//创建Connection对象
		m_pConnection.CreateInstance("ADODB.Connection");
		//连接数据库
		m_pConnection->Open(strConnect, "", "", adModeUnknown);
		if (m_pConnection == NULL)
		{
			cerr << "Lind data ERROR!\n";
		}
		m_pConnection->Execute(bstrSQL, NULL, adCmdText);
	}
	catch (_com_error c)
	{
		cerr << "\nCREATE TABLE	ERROR:" << (char*)c.Description();
	}
	if (m_pConnection->State)
		m_pConnection->Close();

	::CoUninitialize();
}
/*delete table*/
void	DataBase::DB_DeleteTable(_bstr_t	bstrSQL) {

	::CoInitialize(NULL);//初始化OLE/COM库环境，为访问ADO接口做准备
	_ConnectionPtr m_pConnection("ADODB.Connection");
	try
	{
		//创建Connection对象
		m_pConnection.CreateInstance("ADODB.Connection");
		//连接数据库
		m_pConnection->Open(strConnect, "", "", adModeUnknown);
		if (m_pConnection == NULL)
		{
			cerr << "Lind data ERROR!\n";
		}
		m_pConnection->Execute(bstrSQL, NULL, adCmdText);
	}
	catch (_com_error c)
	{
		cerr << "\nDELETE TABLE	ERROR:" << (char*)c.Description();
	}
	if (m_pConnection->State)
		m_pConnection->Close();

	::CoUninitialize();
}
/*return OnCar Info*/
vector<oncar>	DataBase::DB_ReturnOnCar() {
	return	OnCar;
}
/*return Remperature Info*/
vector<temperature>	DataBase::DB_ReturnTemp() {
	return	Temperature;
}
/*return HeartRate Info*/
vector<heartrate>	DataBase::DB_ReturnHeartRate() {
	return	HeartRate;
}
/*return Weight Info*/
vector<weight>	DataBase::DB_ReturnWeight() {
	return	Weight;
}
/*return Rols Cnt*/
int DataBase::DB_ReturnVersion_Hr() {
	return Hr_Version;
}
int DataBase::DB_ReturnVersion_Tp() {
	return Tp_Version;
}