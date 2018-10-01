#include "stdafx.h"

std::chrono::high_resolution_clock::time_point mill1;
std::chrono::high_resolution_clock::time_point mill2;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void TimerFunction(int value);
void Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void  Light();

int num;
int  speed = 60;
SERVER_STRUCT sbuffer;
CLIENT_STRUCT Cbuffer;
GameObject object = GameObject();
HANDLE hThread;
int N;
bool nbool = true;
bool change = false;
bool timebool = false;
SOCKET client_sock;

//소켓 함수 오류 출력 후 종료
void err_quit(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)&lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

//소켓 함수 오류 출력 
void err_display(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

int recvn(SOCKET s, char* buf, int len, int flags)
{
	// s 는 사용할 소켓
	// buf는 응용프로그램에서 사용하는 데이터를 저장하기위한 버퍼
	//len은 운영체제의 수신버퍼로부터 받을 최대크기의 데이터의 길이를 이야기함 이것은 응용프로그램에서 사용하는 데이터 저장버퍼의 길이보다 길면 안됨 
	//flags 는 그냥 flag값임 
	int received;
	char* ptr = buf;
	int left = len;

	//받은 양을 누적시킨다 
	float Totalrecv = 0;
	float Totallen = len;

	// 렌도 나타내줘야한다. 
	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
		Totalrecv += received;
	}
	// 총길이를 나눈다. 

	return (len - left); // 얼마를 받았다  나타냄 
}


DWORD WINAPI SendRecvThread(LPVOID arg)
{

	int retval;
	// 윈속 초기화 
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	//socket() -> 서버와 연동하기위한 소켓 생성 
	SOCKET client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client_sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	retval = connect(client_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");
	printf("connect()\n");

	while (true)
	{

		mill1 = std::chrono::high_resolution_clock::now();
		retval = recvn(client_sock, (char*)&sbuffer, sizeof(SERVER_STRUCT), 0);
		object.SetSBuffer(sbuffer);
		num = sbuffer.pIndex;

		object.PlayerMove();
		Cbuffer = object.GetCBuffer(num, change);
		if (change) change = false;

		while (true)
		{
			mill2 = std::chrono::high_resolution_clock::now();
			std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds> (mill2 - mill1);
			if (ms > (std::chrono::milliseconds)25) {
				break;
			}
		}
		retval = send(client_sock, (char*)&Cbuffer, sizeof(CLIENT_STRUCT), 0);
	}

	//closesocket()
	closesocket(client_sock);

	//윈속 종료 
	WSACleanup();
	return 0;
}


int main()
{
	CreateThread(NULL, 0, SendRecvThread, NULL, 0, NULL);

	srand((unsigned)time(NULL));
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 800);
	glutTimerFunc(speed, TimerFunction, 1);
	glutCreateWindow("컴그 팀 프로젝트");
	object.t->InitTexture();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	//send()
	glutMainLoop();

	return 0;
}
GLvoid drawScene(GLvoid)
{
	object.SoundPlay();

	static double finish_time = 30.0;
	glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (object.GetStartState() == 0)
	{
		glTranslatef(0.0, -10, -100);
		gluLookAt(0.0, 1000.0, 0.0, 0.0, 10.0, 0.0, 0.0, 0.0, -1.0);
		object.Draw_Menu();
	}
	else if (object.GetStartState() == 1)
	{
		glTranslatef(0.0, -10, -100);
		gluLookAt(object.GetSBuffer().A[object.GetSBuffer().pIndex].x, 100.0, object.GetSBuffer().A[object.GetSBuffer().pIndex].z + 70.0,
			object.GetSBuffer().A[object.GetSBuffer().pIndex].x, 0.0, object.GetSBuffer().A[object.GetSBuffer().pIndex].z,
			0.0, 1.0, 0.0);
	}

	if (object.GetStartState() == 1)
	{
		Light();
		object.Draw_Fog();
		if (object.GetFogSwitch())
		{
			glEnable(GL_FOG);
		}
		else
		{
			glDisable(GL_FOG);
		}
		object.Draw_Inform(num);

		object.Draw_Board();

		for (int i = 0; i < 3; i++)
		{
			if (object.GetBallState(i))
			{
				object.Draw_Ball();
			}
			else
			{
				object.p->Draw();
			}
		}
	}
	glutSwapBuffers();
}
GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	gluPerspective(60.0f, 1.0, 1.0, 2000.0);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
}
void TimerFunction(int value)
{
	glutPostRedisplay();
	glutTimerFunc(speed, TimerFunction, 1);
}
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case VK_TAB:
		change = true;
		break;
	}
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
	}
}

void  Light()
{
	GLfloat ambientLignt[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	GLfloat diffuseLight[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat specular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat lightPos[] = { 0.0f, 100.0f, 0.0f, 1.0f };

	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLignt);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos);
	glEnable(GL_LIGHT1);
}