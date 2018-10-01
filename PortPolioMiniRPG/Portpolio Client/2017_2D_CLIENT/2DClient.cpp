// PROG14_1_16b.CPP - DirectInput keyboard demo

// INCLUDES ///////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN  
#define INITGUID
//#pragma comment(linker,"/entry:WinMainCRTStartup /subsystem:console")

#include <WinSock2.h>
#include <windows.h>   // include important windows stuff
#include <windowsx.h>
#include <stdio.h>
#include <wchar.h>
#include <d3d9.h>     // directX includes
#include "d3dx9tex.h"     // directX includes
#include "gpdumb1.h"
#include "Protocol.h"
#include "../2017_2D_CLIENT/Input/Input.h"
#pragma comment (lib, "ws2_32.lib")


#define KEYDOWN(Key)  CInput::GET_SINGLE()->KeyDown(Key)
#define KEYPRESS(Key) CInput::GET_SINGLE()->KeyPress(Key)
#define KEYUP(Key)    CInput::GET_SINGLE()->KeyUp(Key)
// DEFINES ////////////////////////////////////////////////

#define MAX(a,b)	((a)>(b))?(a):(b)
#define	MIN(a,b)	((a)<(b))?(a):(b)
// defines for windows 
#define WINDOW_CLASS_NAME L"WINXCLASS"  // class name

#define WINDOW_WIDTH    800// size of window
#define WINDOW_HEIGHT   800

//// 748  680
// 801	  730
#define	BUF_SIZE				1024
#define	WM_SOCKET				WM_USER + 1

// PROTOTYPES /////////////////////////////////////////////
HWND hEditName, hEditStory, hEnter, hStory;
// game console
int Game_Init(void *parms = NULL);
int Game_Shutdown(void *parms = NULL);
int Game_Main(void *parms = NULL);

void game_UI();
void game_Draw();
void game_Input();
// GLOBALS ////////////////////////////////////////////////


HWND main_window_handle = NULL; // save the window handle
HINSTANCE main_instance = NULL; // save the instance

/*PLAYER ID*/
WCHAR     USER_ID[MAX_STR_SIZE];


// demo globals
//BOB			player;				    // 플레이어 Unit
BOB         skelaton[MAX_USER];     // the other player skelaton
BOB			npc[NUM_OF_NPC];        // NPC Unit


BITMAP_IMAGE reactor;      // the background   
BITMAP_IMAGE black_tile;
BITMAP_IMAGE white_tile;
BITMAP_IMAGE AttackSign;
D3DXVECTOR3 AttackPos;

bool IsAttack = false;

#define UNIT_TEXTURE  0

SOCKET  g_mysocket;
WSABUF	send_wsabuf;
char 	send_buffer[BUF_SIZE];
WSABUF	recv_wsabuf;
char	recv_buffer[BUF_SIZE];
char	packet_buffer[BUF_SIZE];
DWORD		in_packet_size = 0;
int		saved_packet_size = 0;



unsigned int g_myid;
unsigned int g_myindex;
int		g_left_x = 0;
int     g_top_y = 0;

WCHAR str[256];
bool  ischatt = false;

// FUNCTIONS //////////////////////////////////////////////
void ProcessPacket(char *ptr)
{
	static bool first_time = true;
	switch (ptr[1])
	{
	case SC_LOG_SUCC:
	{
		sc_packet_logsucc* LogPacket = reinterpret_cast<sc_packet_logsucc*>(ptr);
		first_time = false;
		g_myid = LogPacket->id;
		g_myindex = LogPacket->index;
		skelaton[g_myindex].ID = g_myid;
		skelaton[g_myindex].HP = LogPacket->hp;
		skelaton[g_myindex].MP = LogPacket->mp;
		skelaton[g_myindex].LEV = LogPacket->lev;
		skelaton[g_myindex].ATK = LogPacket->atk;
		skelaton[g_myindex].EXP = LogPacket->exp;
		skelaton[g_myindex].x = LogPacket->x;
		skelaton[g_myindex].y = LogPacket->y;
		g_left_x = skelaton[g_myindex].x - 5;
		g_top_y = skelaton[g_myindex].y - 5;
		skelaton[g_myindex].attr |= BOB_ATTR_VISIBLE;
		break;
	}
	case SC_LOG_FAIL:
	{
		printf("이미 접속되어있거나.. 없는 아이디입니다!\n");
		break;
	}
	case SC_PUT_PLAYER:
	{
		sc_packet_put_player *my_packet = reinterpret_cast<sc_packet_put_player *>(ptr);
		unsigned int id = my_packet->id;
		unsigned int index = my_packet->index;
		//printf("ID: %d, X:%d Y:%d \n", my_packet->id, my_packet->x, my_packet->y);
		if (id == g_myid) {
			skelaton[index].x = my_packet->x;
			skelaton[index].y = my_packet->y;
			skelaton[index].attr |= BOB_ATTR_VISIBLE;
			g_left_x = skelaton[index].x - 5;
			g_top_y = skelaton[index].y - 5;
		}
		else
		{
			skelaton[index].ID = my_packet->id;
			skelaton[index].x = my_packet->x;
			skelaton[index].y = my_packet->y;
			skelaton[index].attr |= BOB_ATTR_VISIBLE;
		}
		break;
	}
	case SC_MOVE_PLAYER:
	{
		sc_packet_move_player *my_packet = reinterpret_cast<sc_packet_move_player *>(ptr);
		unsigned int index = my_packet->index;
		if (index == g_myindex)
		{
			skelaton[g_myindex].x = my_packet->x;
			skelaton[g_myindex].y = my_packet->y;
			g_left_x = my_packet->x - 5;
			g_top_y = my_packet->y - 5;
		}
		else
		{
			skelaton[index].x = my_packet->x;
			skelaton[index].y = my_packet->y;
		}
		break;
	}
	case SC_REMOVE_PLAYER:
	{
		sc_packet_remove_player *my_packet = reinterpret_cast<sc_packet_remove_player *>(ptr);
		int index = my_packet->index;
		if (index == g_myindex) skelaton[g_myindex].attr &= ~BOB_ATTR_VISIBLE;
		else skelaton[index].attr &= ~BOB_ATTR_VISIBLE;
		break;
	}
	case SC_PUT_NPC:
	{  sc_packet_put_npc *my_packet = reinterpret_cast<sc_packet_put_npc *>(ptr);
	unsigned int id = my_packet->id;
	//printf("Put NPC\n");
	npc[id].ID = my_packet->id;
	npc[id].x = my_packet->x;
	npc[id].y = my_packet->y;
	npc[id].attr |= BOB_ATTR_VISIBLE;
	// npc[id].curr_frame = PutNPCPacket->obtype;
//printf("NPC ID : %d  X :%d Y:%d \n", id, npc[id - NPC_START].x, npc[id - NPC_START].y);
	break;
	}
	case SC_MOVE_NPC:
	{
		sc_packet_move_npc *Packet = reinterpret_cast<sc_packet_move_npc *>(ptr);
		npc[Packet->id].x = Packet->x;
		npc[Packet->id].y = Packet->y;
		break;
	}
	case SC_REMOVE_NPC:
	{
		sc_packet_remove_npc *Packet = reinterpret_cast<sc_packet_remove_npc *>(ptr);
		//printf("Remove NPC\n");
		npc[Packet->id].attr &= ~BOB_ATTR_VISIBLE;
		//printf("remove npc %d\n", Packet->id);
		break;
	}
	case SC_HEATED_PLAYER:
	{
		sc_packet_heated_player * Packet = reinterpret_cast<sc_packet_heated_player*>(ptr);
		skelaton[g_myindex].HP = Packet->hp;


		printf("HP : %d \n", Packet->hp);
		break;
	}
	case SC_EXP_UP_PLAYER:
	{
		sc_packet_exp_up_player* Packet = reinterpret_cast<sc_packet_exp_up_player*>(ptr);
		skelaton[g_myindex].EXP = Packet->exp;
		break;
	}
	case SC_LEVEL_UP_PLAYER:
	{
		sc_packet_level_up_player* Packet = reinterpret_cast<sc_packet_level_up_player*>(ptr);
		skelaton[g_myindex].LEV += 1;
	}
	default: printf("Unknown PACKET type [%d]\n", ptr[1]);
	}
}

void ReadPacket(SOCKET sock)
{
	DWORD iobyte, ioflag = 0;

	int ret = WSARecv(sock, &recv_wsabuf, 1, &iobyte, &ioflag, NULL, NULL);
	if (ret) {
		int err_code = WSAGetLastError();
		printf("Recv Error [%d]\n", err_code);
	}

	BYTE *ptr = reinterpret_cast<BYTE *>(recv_buffer);

	while (0 != iobyte) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (iobyte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			iobyte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, iobyte);
			saved_packet_size += iobyte;
			iobyte = 0;
		}
	}
}

void clienterror()
{
	exit(-1);
}

LRESULT CALLBACK WindowProc(HWND hwnd,
	UINT msg,
	WPARAM wparam,
	LPARAM lparam)
{
	// this is the main message handler of the system
	PAINTSTRUCT	ps;		   // used in WM_PAINT
	HDC			hdc;	   // handle to a device context


	// what is the message 
	switch (msg)
	{
	case WM_KEYDOWN:
	{
		if (wparam == VK_ESCAPE)
			PostMessage(main_window_handle, WM_DESTROY, 0, 0);
		IsAttack = false;
		int x = 0, y = 0;
		if (wparam == VK_RIGHT)	x += 1;
		if (wparam == VK_LEFT)	x -= 1;
		if (wparam == VK_UP)	y -= 1;
		if (wparam == VK_DOWN)	y += 1;
		cs_packet_move *MovePacket = reinterpret_cast<cs_packet_move *>(send_buffer);
		MovePacket->size = sizeof(MovePacket);
		MovePacket->type = CS_MOVE;
		send_wsabuf.len = sizeof(MovePacket);
		DWORD iobyte;
		if (0 != x)
		{
			if (1 == x) MovePacket->dir = DIR_RIGHT;
			else MovePacket->dir = DIR_LEFT;
			WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		}
		if (0 != y)
		{
			if (1 == y) MovePacket->dir = DIR_DOWN;
			else MovePacket->dir = DIR_UP;
			WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		}
	}
	case WM_CREATE:
	{
		// do initialization stuff here
		return(0);
	} break;
	case WM_PAINT:
	{
		// start painting
		hdc = BeginPaint(hwnd, &ps);
		// end painting
		EndPaint(hwnd, &ps);
		return(0);
	} break;

	case WM_DESTROY:
	{
		// kill the application			
		PostQuitMessage(0);
		return(0);
	} break;
	case WM_SOCKET:
	{
		if (WSAGETSELECTERROR(lparam)) {
			closesocket((SOCKET)wparam);
			clienterror();
			break;
		}
		switch (WSAGETSELECTEVENT(lparam)) {
		case FD_READ:
			ReadPacket((SOCKET)wparam);
			break;
		case FD_CLOSE:
			closesocket((SOCKET)wparam);
			clienterror();
			break;
		}
	}

	default:break;

	} // end switch


// process any messages that we didn't take care of 
	return (DefWindowProc(hwnd, msg, wparam, lparam));

} // end WinProc

// WINMAIN ////////////////////////////////////////////////

int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE hprevinstance,
	LPSTR lpcmdline,
	int ncmdshow)
{
	// this is the winmain function

	WNDCLASS winclass;	// this will hold the class we create
	HWND	 hwnd;		// generic window handle
	MSG		 msg;		// generic message


	// first fill in the window class stucture
	winclass.style = CS_DBLCLKS | CS_OWNDC |
		CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = hinstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = WINDOW_CLASS_NAME;

	// register the window class
	if (!RegisterClass(&winclass))
		return(0);

	// create the window, note the use of WS_POPUP
	if (!(hwnd = CreateWindow(WINDOW_CLASS_NAME, // class
		L"Chess Client",	 // title
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		0, 0,	   // x,y
		WINDOW_WIDTH,  // width
		WINDOW_HEIGHT, // height
		NULL,	   // handle to parent 
		NULL,	   // handle to menu
		hinstance,// instance
		NULL)))	// creation parms
		return(0);

	// save the window handle and instance in a global
	main_window_handle = hwnd;
	main_instance = hinstance;

	// perform all game console specific initialization
	Game_Init();

	/***************************************로그인 패킷 전송**/
	//cs_packet_login* LogInPacket = reinterpret_cast<cs_packet_login*>(send_buffer);
	//DWORD IoByte;
	//LogInPacket->type = CS_LOGIN;
	//LogInPacket->size = sizeof(cs_packet_login);
	//send_wsabuf.len = sizeof(cs_packet_login);
	//wcscpy(LogInPacket->name, USER_ID);
	//int retval = WSASend(g_mysocket, &send_wsabuf, 1, &IoByte, 0, NULL, NULL);
	//memset(send_buffer, 0, sizeof(send_buffer));
	/********************************************************/
	// enter main event loop
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// test if this is a quit
			if (msg.message == WM_QUIT)
				break;

			// translate any accelerator keys
			TranslateMessage(&msg);

			// send the message to the window proc
			DispatchMessage(&msg);
		} // end if

	// main game processing goes here
		Game_Main();

	} // end while

// shutdown game and release all resources
	Game_Shutdown();

	// return to Windows like this
	return(msg.wParam);

} // end WinMain

///////////////////////////////////////////////////////////

// WINX GAME PROGRAMMING CONSOLE FUNCTIONS ////////////////

int Game_Init(void *parms)
{
	// this function is where you do all the initialization 
	// for your game
	// set up screen dimensions
	CInput::GET_SINGLE()->Initialize(main_window_handle);
	screen_width = WINDOW_WIDTH;
	screen_height = WINDOW_HEIGHT;
	screen_bpp = 32;
	// initialize directdraw
	DD_Init(screen_width, screen_height, screen_bpp);
	// create and load the reactor bitmap image
	Create_Bitmap32(&reactor, 0, 0, 531, 532);
	Create_Bitmap32(&black_tile, 0, 0, 531, 532);
	Create_Bitmap32(&white_tile, 0, 0, 531, 532);
	//Load_Image_Bitmap32(&reactor, L"CHESSMAP.BMP", 0, 0, BITMAP_EXTRACT_MODE_ABS);
	Load_Image_Bitmap32(&black_tile, L"CHESSMAP.BMP", 0, 0, BITMAP_EXTRACT_MODE_ABS);
	black_tile.x = 70;
	black_tile.y = 5;
	black_tile.height = TILE_WIDTH;
	black_tile.width = TILE_WIDTH;
	Load_Image_Bitmap32(&white_tile, L"CHESSMAP.BMP", 0, 0, BITMAP_EXTRACT_MODE_ABS);
	white_tile.x = 6;
	white_tile.y = 6;
	white_tile.height = TILE_WIDTH;
	white_tile.width = TILE_WIDTH;
	Load_Image_Bitmap32(&AttackSign, L"RedX.BMP", 0, 0, BITMAP_EXTRACT_MODE_ABS);
	AttackSign.x = 0;
	AttackSign.y = 0;
	AttackSign.height = TILE_WIDTH;
	AttackSign.width = TILE_WIDTH;
	// now let's load in all the frames for the skelaton!!!
	Load_Texture(L"CHESS2.PNG", UNIT_TEXTURE, 384, 64);

	//player 셋팅 
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (!Create_BOB32(&skelaton[i], 0, 0, 63, 63, 1, BOB_ATTR_SINGLE_FRAME)) return(0);
		Load_Frame_BOB32(&skelaton[i], UNIT_TEXTURE, 0, 3, 0, BITMAP_EXTRACT_MODE_CELL);

		// set up stating state of skelaton
		Set_Animation_BOB32(&skelaton[i], 0);
		Set_Anim_Speed_BOB32(&skelaton[i], 4);
		Set_Vel_BOB32(&skelaton[i], 0, 0);
		Set_Pos_BOB32(&skelaton[i], 0, 0);
	}

	// npc setting
	for (int i = 0; i < NUM_OF_NPC; ++i)
	{
		if (!Create_BOB32(&npc[i], 0, 0, 63, 63, 4, BOB_ATTR_SINGLE_FRAME))return(0);
		Load_Frame_BOB32(&npc[i], UNIT_TEXTURE, 0, 4, 0, BITMAP_EXTRACT_MODE_CELL);
		Load_Frame_BOB32(&npc[i], UNIT_TEXTURE, 1, 0, 0, BITMAP_EXTRACT_MODE_CELL);
		Load_Frame_BOB32(&npc[i], UNIT_TEXTURE, 2, 1, 0, BITMAP_EXTRACT_MODE_CELL);
		Load_Frame_BOB32(&npc[i], UNIT_TEXTURE, 3, 5, 0, BITMAP_EXTRACT_MODE_CELL);
		// set up stating state of skelaton
		Set_Animation_BOB32(&npc[i], 0);
		Set_Anim_Speed_BOB32(&npc[i], 4);
		Set_Vel_BOB32(&npc[i], 0, 0);
		Set_Pos_BOB32(&npc[i], 0, 0);
	}


	// set clipping rectangle to screen extents so mouse cursor
	// doens't mess up at edges
	//RECT screen_rect = {0,0,screen_width,screen_height};
	//lpddclipper = DD_Attach_Clipper(lpddsback,1,&screen_rect);
	// hide the mouse
	//ShowCursor(true);
	/************ID 입력****************************************/
	//printf("ID를 입력해 주세요!:");
	//wscanf(L"%s", USER_ID);
	//getchar();

	/****************************************************/
	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	g_mysocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(MY_SERVER_PORT);
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	int Result = WSAConnect(g_mysocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);
	WSAAsyncSelect(g_mysocket, main_window_handle, WM_SOCKET, FD_CLOSE | FD_READ);
	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = BUF_SIZE;
	recv_wsabuf.buf = recv_buffer;
	recv_wsabuf.len = BUF_SIZE;
	// return success
	return(1);
} // end Game_Init

///////////////////////////////////////////////////////////

int Game_Shutdown(void *parms)
{
	//this function is where you shutdown your game and
	//release all resources that you allocated
	cs_packet_logout *LogOutPacket = reinterpret_cast<cs_packet_logout *>(send_buffer);
	LogOutPacket->size = sizeof(LogOutPacket);
	LogOutPacket->type = CS_LOGOUT;
	send_wsabuf.len = sizeof(LogOutPacket);
	DWORD iobyte;
	int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
	//if (ret != SOCKET_ERROR)
	//	printf("보냈는ㄷ[?\n");


	// kill the reactor
	Destroy_Bitmap32(&black_tile);
	Destroy_Bitmap32(&white_tile);
	Destroy_Bitmap32(&reactor);
	Destroy_Bitmap32(&AttackSign);

	// kill skelaton
	for (int i = 0; i < MAX_USER; ++i) Destroy_BOB32(&skelaton[i]);
	for (int i = 0; i < NUM_OF_NPC; ++i) Destroy_BOB32(&npc[i]);
	// shutdonw directdraw
	DD_Shutdown();
	WSACleanup();
	// return success
	return(1);
} // end Game_Shutdown

///////////////////////////////////////////////////////////

int Game_Main(void *parms)
{
	// this is the workhorse of your game it will be called
	// continuously in real-time this is like main() in C
	// all the calls for you game go here!
	// check of user is trying to exit
	//if (KEY_DOWN(VK_ESCAPE) /*|| KEY_DOWN(VK_SPACE)*/)
	//	PostMessage(main_window_handle, WM_DESTROY, 0, 0);
	// start the timing clock
	Start_Clock();
	// clear the drawing surface
	DD_Fill_Surface(D3DCOLOR_ARGB(255, 0, 0, 0));
	// get player input
	game_Input();

	g_pd3dDevice->BeginScene();
	g_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);

	game_Draw();

	g_pSprite->End();
	g_pd3dDevice->EndScene();
	DD_Flip();
	return(1);
} // end Game_Main

//////////////////////////////////////////////////////////


void game_Input()
{

	BYTE Dir = -1;
	if (KEYDOWN("Fire")) {

		//printf("Attack\n");
		IsAttack = true;
		cs_packet_attack *AttackPacket = reinterpret_cast<cs_packet_attack *>(send_buffer);
		AttackPacket->size = sizeof(AttackPacket);
		AttackPacket->type = CS_ATTACK;
		send_wsabuf.len = sizeof(AttackPacket);
		DWORD iobyte;
		WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
	}
	if (KEYUP("Fire")) {
		IsAttack = false;
	}

	//if (KEYDOWN("UP"))
	//{   
	//	printf("Up\n");
	//	Dir = DIR_UP;
	//	IsAttack = false;
	//}
	//if (KEYDOWN("DOWN"))
	//{
	//	printf("down\n");
	//	Dir = DIR_DOWN;
	//	IsAttack = false;
	//}
	//if (KEYDOWN("LEFT"))
	//{
	//	printf("left\n");
	//	Dir = DIR_LEFT;
	//	IsAttack = false;
	//}
	//if (KEYDOWN("RIGHT"))
	//{
	//	printf("down\n");
	//	Dir = DIR_RIGHT;
	//	IsAttack = false;
	//}
	//if (Dir != -1)
	//{
	//	cs_packet_move *MovePacket = reinterpret_cast<cs_packet_move *>(send_buffer);
	//	MovePacket->size = sizeof(MovePacket);
	//	MovePacket->type = CS_MOVE;
	//	send_wsabuf.len = sizeof(MovePacket);
	//	MovePacket->dir = Dir;
	//	DWORD iobyte;
	//	WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
	//}


	CInput::GET_SINGLE()->Update();
}

void game_Draw()
{

	// draw the background reactor image
	for (int i = 0; i < 11; ++i)
	{
		for (int j = 0; j < 11; ++j)
		{
			int tile_x = i + g_left_x;
			int tile_y = j + g_top_y;
			if ((tile_x < 0) || (tile_y < 0)) continue;
			if ((tile_x > BOARD_HEIGHT - 1) || (tile_y > BOARD_WIDTH - 1)) continue;
			//((tile_x >> 1) % 2) == ((tile_y >> 1) % 2)
			if ((tile_x) % 2 == 0) {
				if (((tile_y) % 2) == 0) Draw_Bitmap32(&black_tile, TILE_WIDTH * i + 7, TILE_WIDTH * j + 7);
				else Draw_Bitmap32(&white_tile, TILE_WIDTH * i + 7, TILE_WIDTH * j + 7);
			}
			else {
				if (((tile_y) % 2) == 0) Draw_Bitmap32(&white_tile, TILE_WIDTH * i + 7, TILE_WIDTH * j + 7);
				else Draw_Bitmap32(&black_tile, TILE_WIDTH * i + 7, TILE_WIDTH * j + 7);
			}
		}
	}

	//	Draw_Bitmap32(&reactor);
	g_pSprite->End();
	g_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE);
	// draw the skelaton
	for (int i = 0; i < MAX_USER; ++i) Draw_BOB32(&skelaton[i]);
	for (int i = 0; i < NUM_OF_NPC; ++i) Draw_BOB32(&npc[i]);

	if (IsAttack)
	{
		//printf("dassd\n");
		Draw_Bitmap32(&AttackSign, (skelaton[g_myindex].x - g_left_x) * static_cast<float>(TILE_WIDTH) + 7,
			(skelaton[g_myindex].y - g_top_y) * static_cast<float>(TILE_WIDTH) + 7);
		Draw_Bitmap32(&AttackSign, (skelaton[g_myindex].x - 1 - g_left_x) * static_cast<float>(TILE_WIDTH) + 7,
			(skelaton[g_myindex].y - g_top_y) * static_cast<float>(TILE_WIDTH) + 7);
		Draw_Bitmap32(&AttackSign, (skelaton[g_myindex].x + 1 - g_left_x) * static_cast<float>(TILE_WIDTH) + 7,
			(skelaton[g_myindex].y - g_top_y) * static_cast<float>(TILE_WIDTH) + 7);
		Draw_Bitmap32(&AttackSign, (skelaton[g_myindex].x - g_left_x) * static_cast<float>(TILE_WIDTH) + 7,
			(skelaton[g_myindex].y - g_top_y - 1) * static_cast<float>(TILE_WIDTH) + 7);
		Draw_Bitmap32(&AttackSign, (skelaton[g_myindex].x - g_left_x) * static_cast<float>(TILE_WIDTH) + 7,
			(skelaton[g_myindex].y - g_top_y + 1) * static_cast<float>(TILE_WIDTH) + 7);

	}
	game_UI();
}

void game_UI()
{
	wchar_t text[300];
	wsprintf(text, L"ID: %s", USER_ID);
	Draw_Text_D3D(text, 0, screen_height - 85, D3DCOLOR_ARGB(255, 255, 255, 0));
	wsprintf(text, L"HP: %d", skelaton[g_myindex].HP);
	Draw_Text_D3D(text, 150, screen_height - 85, D3DCOLOR_ARGB(255, 255, 255, 0));
	wsprintf(text, L"ATK: %d", skelaton[g_myindex].ATK);
	Draw_Text_D3D(text, 300, screen_height - 85, D3DCOLOR_ARGB(255, 255, 255, 0));
	wsprintf(text, L"EXP: %d", skelaton[g_myindex].EXP);
	Draw_Text_D3D(text, 450, screen_height - 85, D3DCOLOR_ARGB(255, 255, 255, 0));
	wsprintf(text, L"LEV: %d", skelaton[g_myindex].LEV);
	Draw_Text_D3D(text, 600, screen_height - 85, D3DCOLOR_ARGB(255, 255, 255, 0));
	wsprintf(text, L"MY POSITION (%3d, %3d)", skelaton[g_myindex].x, skelaton[g_myindex].y);
	Draw_Text_D3D(text, 0, screen_height - 64, D3DCOLOR_ARGB(255, 255, 255, 255));
}



