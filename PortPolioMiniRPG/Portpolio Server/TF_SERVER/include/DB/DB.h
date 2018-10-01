#pragma once
#include "../SingleTone.h"
#include "../Include.h"
#include "../Object/User.h"
class CDB :public CSingleTone<CDB>
{
private:
	SQLHENV   henv;
	SQLHDBC   hdbc;
	SQLHSTMT  hstmt;
	SQLRETURN retcode;
public:
	CDB();
	virtual ~CDB();

public:
	bool ConnectDB();
	void DisConnectDB();
	void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);

public:
	bool GetUserData(CUser& user, char*name);
	bool SaveUserData(CUser& user);
};

