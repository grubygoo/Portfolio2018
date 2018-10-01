#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <sqlext.h>
#include <stdlib.h>
#include <conio.h>
#include <windowsx.h>
#include <mswsock.h>
#include <vector>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <thread>
#include <algorithm>
#include <chrono>
#include <math.h>
#include <mutex>
#include <time.h>
#include <tbb/mutex.h>
#include <tbb/tbbmalloc_proxy.h>
#include <tbb/queuing_rw_mutex.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"mswsock.lib") 

using namespace std; 
using namespace chrono;
using namespace tbb;

#include "Define.h"
#include "Struct.h"
#include "protocol.h"


inline bool IsNPC(unsigned int id)
{
	return (id < NUM_OF_NPC);
}

inline bool InMySight(Vec2i me, Vec2i other, BYTE radius)
{
	int Dist_Sqare = (me.x - other.x)*(me.x - other.x) + (me.y - other.y)*(me.y - other.y);
	return (Dist_Sqare <= radius * radius);
}