#include "Core.h"
#include "Util\MiniDump.h"

CCore IOCP_SERVER;
int main()
{
	CMiniDump::Begin();
	IOCP_SERVER.Initialize();

	IOCP_SERVER.Run();
	CMiniDump::End();
	return 0;
}