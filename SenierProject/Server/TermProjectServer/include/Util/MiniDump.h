#define WIN32_LEAN_AND_MEAN		// ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <DbgHelp.h>
class CMiniDump
{
public:
	static BOOL Begin(VOID);
	static BOOL End(VOID);
};