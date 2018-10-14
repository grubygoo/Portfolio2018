#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <sqlext.h>
#include <conio.h>
#include <windowsx.h>
#include <mswsock.h>
#include <vector>
#include <iostream>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <tbb/mutex.h>
#include <tbb/queuing_rw_mutex.h>
#include <algorithm>
#include <queue>
#include <chrono>
#include <math.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>
#include <tbb/tbbmalloc_proxy.h>

/*Server Lib*/
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"mswsock.lib") 
using namespace DirectX;
using namespace DirectX::PackedVector;
using namespace std; 
using namespace chrono;

#include "Define.h"
#include "protocol.h"
#include "Struct.h"
#include "Inline.h"

