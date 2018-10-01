#include "DB.h"

CDB::CDB()
{
}

CDB::~CDB()
{
}


bool CDB::ConnectDB()
{
	// Allocate environment handle
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	// Set the ODBC version environment attribute
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

		// Allocate connection handle
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			// Set login timeout to 5 seconds
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)20, 0);

				// Connect to data source
				retcode = SQLConnect(hdbc, (SQLWCHAR*)L"KANG_ODBC", SQL_NTS, (SQLWCHAR*)L"sa", SQL_NTS, (SQLWCHAR*)L"gameserver", SQL_NTS);

				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
				{
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);

					// Process data
					if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
					{
						cout << "[DB Connection Success!]" << endl << endl;
						return true;
					}
				}
				else
				{
					cout << "[DB Connection Failed]" << endl << endl;
					HandleDiagnosticRecord(hdbc, SQL_HANDLE_DBC, retcode);
					return false;
				}
			}
		}
	}
}

void CDB::DisConnectDB()
{
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	SQLDisconnect(hdbc);
}

void CDB::HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode)
{
	_wsetlocale(LC_ALL, L"Korean");

	SQLSMALLINT iRec = 0;
	SQLINTEGER  iError;
	WCHAR       wszMessage[1000];
	WCHAR       wszState[SQL_SQLSTATE_SIZE + 1];

	if (RetCode == SQL_INVALID_HANDLE) {
		fwprintf(stderr, L"Invalid handle!\n");
		return;
	}
	while (SQLGetDiagRec(hType, hHandle, ++iRec, wszState, &iError, wszMessage,
		(SQLSMALLINT)(sizeof(wszMessage) / sizeof(WCHAR)), (SQLSMALLINT *)NULL) == SQL_SUCCESS) {
		// Hide data truncated..
		if (wcsncmp(wszState, L"01004", 5)) {
			fwprintf(stderr, L"[%5.5s] %s (%d)\n", wszState, wszMessage, iError);
		}
	}
}

bool CDB::GetUserData(CUser& user, char*name)
{
	int retries = 0;
retry:
	WCHAR EXECORDER[256];
	OBJECT_DATA Data;
	SQLLEN NameLength, HpLength, MpLength, AtkLength, XLength, YLength, ExpLength;
	SQLWCHAR  Name[50] = { NULL, };
	SQLUINTEGER X, Y, Hp, Mp, Atk, Exp;
	wsprintf(EXECORDER, L"EXEC dbo.LoadUserData %S", name);
	if (SQLExecDirect(hstmt, (SQLWCHAR*)EXECORDER, SQL_NTS) == SQL_ERROR) {
		printf("쿼리 실행 실패\n");
		return false;
	}

	if (retcode != SQL_ERROR)
	{
		retcode = SQLBindCol(hstmt, 1, SQL_C_WCHAR, &Name, 100, &NameLength);
		retcode = SQLBindCol(hstmt, 2, SQL_C_LONG, &X, 100, &XLength);
		retcode = SQLBindCol(hstmt, 3, SQL_C_LONG, &Y, 100, &YLength);
		retcode = SQLBindCol(hstmt, 4, SQL_C_LONG, &Hp, 100, &HpLength);
		retcode = SQLBindCol(hstmt, 5, SQL_C_LONG, &Mp, 100, &MpLength);
		retcode = SQLBindCol(hstmt, 6, SQL_C_LONG, &Atk, 100, &AtkLength);
		retcode = SQLBindCol(hstmt, 7, SQL_C_LONG, &Exp, 100, &ExpLength);

		for (int i = 0; ; i++) {
			retcode = SQLFetch(hstmt);
			if (retcode == SQL_NO_DATA) {
				printf("데이터 정보가 없는데요?");
				return false;
			}
			else if (SQL_ERROR != retcode || SQL_SUCCESS == retcode)
			{
				HandleDiagnosticRecord(hdbc, SQL_HANDLE_DBC, retcode);
				break;
			}
		}

	}
	else {
		printf("실패했는데 다시 시도해보도록 하겠습니다.");
		retries++;
		if (retries >= 5) return false;
			goto retry;
	}

	if (retcode != SQL_ERROR)
	{
		user.SetName(name);
		user.SetPos(X, Y);
		user.SetHP(Hp);
		user.SetMP(Mp);
		user.SetAtk(Atk);
		user.SetExp(Exp);
		printf("success Get USerData\n");
		return true;
	}
	SQLCloseCursor(hstmt);
}

bool CDB::SaveUserData(CUser& user)
{
	WCHAR EXECORDER[256];
	wsprintf(EXECORDER, TEXT("EXEC dbo.SaveUserData %S,%d,%d,%d,%f,%f,%f "),
		user.GetName(), user.GetHP(), user.GetMP(), user.GetAtk(),
		user.GetPos().x, user.GetPos().y);

	if (SQLExecDirect(hstmt, (SQLWCHAR*)EXECORDER, SQL_NTS) == SQL_ERROR)
		HandleDiagnosticRecord(hdbc, SQL_HANDLE_DBC, retcode);

	if (retcode != SQL_ERROR)
	{
		while (SQLFetch(hstmt) == SQL_SUCCESS)
		{
		}
	}
	else printf("DB:: UserData Save Faild....\n");

	SQLCloseCursor(hstmt);
	return true;
}
