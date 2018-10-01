#include "../include/Core/Core.h"
#include "../include/Error/MiniDump.h"

CCore IOCP_SERVER;
int main()
{
	CMiniDump::Begin();
	IOCP_SERVER.Initialize();

	IOCP_SERVER.Run();

	//IOCP_SERVER.Close();
	CMiniDump::End();

	return 0;
}