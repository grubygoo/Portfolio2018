#include<iostream>
#include <gl/glut.h>   // gl.h glu.h  
#include <math.h>
#include <stdlib.h>

#include <conio.h>
#include <windows.h>
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#define getRadian(me)  (me*(3.14159254 / 180))
#define Me_Radi 10
#define Enemy_Radi 5 
#define Obs_Radi 5 
#define enemy_num  13
using namespace std;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
GLvoid Shape(int key, int x, int y);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid SpecialKeyboard(int key, int x, int y);
GLubyte* LoadDIBitmap(const char *filename, BITMAPINFO **info);

enum Weather {
	sunny, rainy, snowy
};
enum TrumpNumber {
	ace, two, three, four, five, six, seven, eight, nine, ten,
	jack, queen, king, joker
};
GLvoid makeTrumpnumb(TrumpNumber object, int z);


////////////////////////  me 그리기 

struct Character {
	GLfloat posX, posZ;		// 객체들의 생성 위치를 나타내는 변수(어느 부분에 생성할까)
	GLfloat speed = 5;		// 날씨의 변화의 따라 속도를 달리 하기위한 변수
	GLfloat xx;				// 객체의 x축 이동을 위한 좌표 값 (glTranslatef() 함수에 쓰임)
	GLfloat yy;				// 객체의 y축 이동을 위한 좌표 값 (glTranslatef() 함수에 쓰임)
	GLfloat zz;				// 객체의 z축 이동을 위한 좌표 값 (glTranslatef() 함수에 쓰임)
	bool Get = false;		// false - non_get , true - get (카드를 얻었는가에대한 bool변수)
	bool Reverse = false;	// false - uo , true -down ( 카드애니메이션에 쓰이는 bool 변수)
	GLfloat RotY = 0.0;		// 객체의 Y축 회전을 위한 변수
	GLfloat rotLeg = 0;		// 조종하는 캐릭터의 팔다리 애니메이션을 위한 변수 
	GLfloat rotArm = 0;		// 조종하는 캐릭터의 팔다리 애니메이션을 위한 변수
	GLfloat increseRad = 3; // 날씨에 따른 키보드조작의 각도 값을 달리 하기위한 변수
	TrumpNumber grade;		// 카드의 종류가 무엇인지 나타내는 변수 (Ace~~~ Joker 까지)
};

typedef struct MapSource {
	int x = rand() % 800 - 400;		//나타내고자 하는 장애물 혹은 날씨 객체를 랜덤 값으로 지정
	int y = rand() % 400 + 300;		//나타내고자 하는 장애물 혹은 날씨 객체를 랜덤 값으로 지정
	int z = rand() % 800 - 400;		//나타내고자 하는 장애물 혹은 날씨 객체를 랜덤 값으로 지정
	int speed = rand() % 3 + 1;		//날씨 객체의 이동 속도를 나타내기 위한 변수
	bool Crush = false;				// 장애물의 충돌체크를 확인시켜 주기 위한 변수
}MapSource;

///////////////////////////////////////////////////
// 애니메이션 함수 
GLvoid move(int value);
GLvoid Cliff_Check(int value);		// 절벽 체크
GLvoid Get_Check(int value);		// 따라다니게하기
GLvoid Crush_Check(int value);		// 카드 충돌 체크
GLvoid Ob_Crush_Check(int value);	// 장애물 충돌 체크
GLvoid Win_Check(int value);		// 다음 스테이지로 가기 위한 승리 체크
GLvoid obs_Rock_Timer(int value);	// 바위에 부딪히면 화면가리기 위한 함수
GLvoid ob_Setting();				// 새로운 스테이지마다 장애물 늘려주기
GLvoid Ob_Screen();					// 화면 가리개 그리기

void initsetting();					// 비트맵 불러오기

GLvoid makeLand();					// 땅만들기
GLvoid makeTree();					// 나무만들기
GLvoid makeRock();					// 돌만들기
GLvoid makeSnow();					// 눈만들기
GLvoid mkitRain();					// 비만들기
GLvoid madCloud();					// 구름만들기
GLvoid Enemy_setting();				// 카드들 세팅
GLvoid setting();					// 카메라 세팅
GLvoid enemy_Rotate(int value);		// 카드들 돌려주기 
GLvoid Atmosphere(int value);		// 기후 변화
GLvoid timeCheck(int value);		// 밤낮 바꿔버리기
GLvoid clouding();					// 구름이동
GLvoid raining();					// 비내리기
GLvoid snowing();					// 눈내리기
GLvoid drawCharacter();				// 주인공그리기
GLvoid Draw_enemy();				// 카드들 그리기
GLvoid drawTitle();					// 타이틀 그리기

// 야간 조명 씌우기 스포트 라이트 함수
GLvoid settingLight();


///////// 스테이지 변수 
int Stage = 1;
int Trees_Ob_Num = 20;
int rocks_Ob_Num = 10;

// 조형물 변수 
MapSource *trees;
MapSource *rocks;
MapSource snows[300];
MapSource rains[300];
MapSource clouds[7];
Weather weatherMan = sunny;
bool obs_rock = false;

//텍스처 변수
GLuint textures[14];
GLubyte* bitmap;
BITMAPINFO* bitmapInfo;


//캐릭터 변수 
TrumpNumber grade;
Character me;
Character* enemy;
bool LegArmrev = false;
int G = 0;


// 타이틀 씬과 메인 씬을 구분하기 위한 변수 
bool TitleScene = true;
// 타이틀에서 메인씬 들어갈때의 카메라 구현을 위한 변수 
int firstCameraz = 800;
// 글자 색깔 변화를 위한 변수 - by Hong
GLfloat r = 0, g = 0, b = 0;
// 밤의 색깔을 바꿔주기 위한 변수 - by Hong
GLfloat nightC = 0.8, nightC2 = 0.3;
// 글자 색깔을 계속 바꿔주기 위한 변수들 
// red up, down green up, down blue up, down - by Hong
bool ru = true, gu = false, bu = false;
bool rd = false, gd = false, bd = false;
// 밤인지 아닌지 표현 
bool night = false;
// 타이틀화면 글자 표시 
char* string1 = { "Press any key Start" };




void main()															// 윈도우 출력하고 출력함수 설정 
{

	PlaySound(TEXT("홍구컴그.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NOSTOP || SND_LOOP);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);		// 디스플레이 모드 설정       
	glutInitWindowPosition(400, 100);								// 윈도우의 위치 지정        
	glutInitWindowSize(800, 600);									// 윈도우의 크기 지정        
	glutCreateWindow("Example1");									// 윈도우 생성 (윈도우 이름)   
	glutDisplayFunc(drawScene);										// 출력 함수의 지정           
	glutSpecialFunc(SpecialKeyboard);								// 스페셜 키보드
	glutKeyboardFunc(Keyboard);
	Enemy_setting();
	// 비트맵 셋팅
	initsetting();
	ob_Setting();

	glutTimerFunc(50, obs_Rock_Timer, 1);
	glutTimerFunc(50, Cliff_Check, 1);	// 절벽에 떨어지는지 체크한다
	glutTimerFunc(50, Crush_Check, 1);	// 카드와 캐릭터가 닿았는지 체크한다
	glutTimerFunc(50, enemy_Rotate, 1); // 카드 애니메이션 
	glutTimerFunc(50, move, 1);         // 캐릭터 움직임 애니메이션 
	glutTimerFunc(50, Get_Check, 1);    // 카드를 얻었는지 체크한다.
	glutTimerFunc(50, Ob_Crush_Check, 1);  // 장애물에 닿았는지 체크한다 
	glutTimerFunc(50, Win_Check, 1);	 //  카드를 다 수집했을떄 다음스테이지로 넘어가게 한다.  --> 장애물개수나 이런거 아직 분배 안했다. 									 
	glutTimerFunc(5000, Atmosphere, 1); // 기상변화 5초에 한번씩 랜덤으로 변화시켜줌
	// 아침밤 시간체크를 위한 변수 - by Hong
	glutTimerFunc(1500, timeCheck, 1);
	// 타이머
	glutReshapeFunc(Reshape);
	glutPostRedisplay();                                            // 다시 그리기 함수 지정    
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();										            // 이벤트 처리 시작 

}

GLvoid drawScene()										            // 출력 함수 
{   //카메라를 셋팅
	setting();

	makeLand(); //Map
	makeTree(); //Tree
	makeRock(); // Rock
	madCloud(); // cloud 

	if (weatherMan == rainy)		mkitRain();
	if (weatherMan == snowy)		makeSnow();

	drawCharacter(); // 캐릭터 그리기 함수

	Draw_enemy(); //   카드들 그리는 함수 
	Ob_Screen();

	//타이틀 씬 변수가 트루 이면 타이틀 화면을 그려준다
	if (TitleScene == true) drawTitle();
	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)							// 다시 그리기 함수 
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, w / h, 1.0, 2000.0);			// 투영 변환 적용(원근투영)
	glMatrixMode(GL_MODELVIEW);

}

GLvoid Shape(int key, int x, int y)
{

	glutPostRedisplay();
}
GLvoid Keyboard(unsigned char key, int x, int y)
{

	// 아무키나 누르면 타이틀 씬이 풀린다
	if (key)	TitleScene = false;
	if (key == 'q') exit(0);
	//
	glutPostRedisplay();
}
GLvoid SpecialKeyboard(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT)
	{
		if (me.RotY < 360)
		{

			me.RotY += me.increseRad;
			

			if (me.RotY >= 360)
				me.RotY = 0;
		}
	}

	else if (key == GLUT_KEY_RIGHT)
	{
		if (me.RotY > -360)
		{

			me.RotY -= me.increseRad;
			

			if (me.RotY <= -360)
				me.RotY = 0;
		}
	}
	glutPostRedisplay();
}

//비트맵읽기

GLubyte* LoadDIBitmap(const char *filename, BITMAPINFO **info)
{
	FILE *fp;
	GLubyte *bits;
	int bitsize, infosize;
	BITMAPFILEHEADER header;

	// 바이너리 읽기 모드로 파일을 연다
	if ((fp = fopen(filename, "rb")) == NULL)   return NULL;

	// 비트맵 파일 헤더를 읽는다.
	if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1)
	{
		fclose(fp);
		return NULL;
	}

	// 파일이 BMP 파일인지 확인한다.
	if (header.bfType != 'MB')
	{
		fclose(fp);
		return NULL;
	}

	// BITMAPINFOHEADER 위치로 간다.
	infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);

	// 비트맵 이미지 데이터를 넣을 메모리 할당을 한다.
	if ((*info = (BITMAPINFO *)malloc(infosize)) == NULL)
	{
		fclose(fp);
		exit(0);
		return NULL;
	}

	// 비트맵 인포 헤더를 읽는다.
	if (fread(*info, 1, infosize, fp) < (unsigned int)infosize)
	{
		free(*info);
		fclose(fp);
		return NULL;
	}

	// 비트맵의 크기 설정
	if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
		bitsize = ((*info)->bmiHeader.biWidth
			* (*info)->bmiHeader.biBitCount + 7) / 8.0
		* abs((*info)->bmiHeader.biHeight);

	// 비트맵의 크기만큼 메모리를 할당한다.
	if ((bits = (unsigned char *)malloc(bitsize)) == NULL)
	{
		free(*info);
		fclose(fp);
		return NULL;
	}
	// 비트맵 데이터를 bit(GLubyte 타입)에 저장한다. 
	if (fread(bits, 1, bitsize, fp) < (unsigned int)bitsize)
	{
		free(*info);
		free(bits);
		fclose(fp);
		return NULL;
	}
	fclose(fp);
	return bits;
}

//타이틀 화면 그려주기 - by Hong
GLvoid drawTitle()
{
	glPushMatrix();
	glColor3f(r, g, b);
	//Left T
	glTranslated(-120, 300, 300);
	glutSolidCube(20);
	//Left
	glPushMatrix();
	glTranslated(-20, 0, 0);
	glutSolidCube(20);
	glTranslated(-20, 0, 0);
	glutSolidCube(20);
	glTranslated(-20, 0, 0);
	glutSolidCube(20);
	glPopMatrix();
	//Right
	glPushMatrix();
	glTranslated(20, 0, 0);
	glutSolidCube(20);
	glTranslated(20, 0, 0);
	glutSolidCube(20);
	glTranslated(20, 0, 0);
	glutSolidCube(20);
	glPopMatrix();
	//down
	glPushMatrix();
	glTranslated(0, -20, 0);
	glutSolidCube(20);
	glTranslated(0, -20, 0);
	glutSolidCube(20);
	glTranslated(0, -20, 0);
	glutSolidCube(20);
	glTranslated(0, -20, 0);
	glutSolidCube(20);
	glTranslated(0, -20, 0);
	glutSolidCube(20);
	glTranslated(0, -20, 0);
	glutSolidCube(20);
	glTranslated(0, -20, 0);
	glutSolidCube(20);
	glTranslated(0, -20, 0);
	glutSolidCube(20);
	glPopMatrix();
	glPopMatrix();

	//Right TglPushMatrix();
	glTranslated(120, 300, 300);
	glutSolidCube(20);
	//Left
	glPushMatrix();
	glTranslated(-20, 0, 0);
	glutSolidCube(20);
	glTranslated(-20, 0, 0);
	glutSolidCube(20);
	glTranslated(-20, 0, 0);
	glutSolidCube(20);
	glPopMatrix();
	//Right
	glPushMatrix();
	glTranslated(20, 0, 0);
	glutSolidCube(20);
	glTranslated(20, 0, 0);
	glutSolidCube(20);
	glTranslated(20, 0, 0);
	glutSolidCube(20);
	glPopMatrix();
	//down
	glPushMatrix();
	glTranslated(0, -20, 0);
	glutSolidCube(20);
	glTranslated(0, -20, 0);
	glutSolidCube(20);
	glTranslated(0, -20, 0);
	glutSolidCube(20);
	glTranslated(0, -20, 0);
	glutSolidCube(20);
	glTranslated(0, -20, 0);
	glutSolidCube(20);
	glTranslated(0, -20, 0);
	glutSolidCube(20);
	glTranslated(0, -20, 0);
	glutSolidCube(20);
	glTranslated(0, -20, 0);
	glutSolidCube(20);
	glPopMatrix();
	glPopMatrix();

	// press 's' button
	glPushMatrix();
	//glTranslated(0, 100, 300);
	glRasterPos3f(-165, -300, 200);
	for (int i = 0; i < strlen(string1); i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string1[i]);

	glPopMatrix();
}

//카메라 셋팅 
GLvoid setting()
{
	//밤이 되면 라이트를 세팅해서 스포트라이트로 바꿔준다. - by Hong
	if (night == true)	settingLight();
	// 낮이면 조명 오프 - by Hong
	else				glDisable(GL_LIGHTING);
	//
	glClearColor(0.0f, nightC2, nightC, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// 설정된 색으로 젂체를 칠하기
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();										// 원점으로 가주기
															// 카메라 화면 같이 따라다니기 - by Hong
	gluLookAt(me.xx, 300.0, firstCameraz + me.zz,
		me.xx, 0.0, me.zz,
		0.0, 1.0, 0.0);
}
GLvoid ob_Setting()
{
	trees = new MapSource[Trees_Ob_Num];
	rocks = new MapSource[rocks_Ob_Num];
}

GLvoid Enemy_setting()
{
	enemy = new Character[13];

	for (int i = 0; i < 13; ++i)
	{
		switch (i) {
		case 0:
			enemy[i].grade = two;
			break;
		case 1:
			enemy[i].grade = three;
			break;
		case 2:
			enemy[i].grade = four;
			break;
		case 3:
			enemy[i].grade = five;
			break;
		case 4:
			enemy[i].grade = six;
			break;
		case 5:
			enemy[i].grade = seven;
			break;
		case 6:
			enemy[i].grade = eight;
			break;
		case 7:
			enemy[i].grade = nine;
			break;
		case 8:
			enemy[i].grade = ten;
			break;
		case 9:
			enemy[i].grade = jack;
			break;
		case 10:
			enemy[i].grade = queen;
			break;
		case 11:
			enemy[i].grade = king;
			break;
		case 12:
			enemy[i].grade = joker;
			break;
		}
	}

	for (int i = 0; i < 13; ++i)
	{
		enemy[i].xx = rand() % 800 - 400;
		enemy[i].zz = rand() % 800 - 400;
		enemy[i].yy = 20;
	}
}


// 객체 생성 함수 
GLvoid makeLand()
{
	glColor4f(0.0f, 0.5f, 0.0f, 1.0f);
	GLfloat ctrlPoints[4][4][3] = {
		{ { -400.0, 0.0 , -400.0 },{ -150.0, 0.0 , -400.0 },{ 150.0, 0.0 , -400.0 },{ 400.0, 0.0 , -400.0 } },
		{ { -400.0, 0.0 , -150.0 },{ -150.0, 0.0 , -150.0 },{ 150.0, 0.0 , -150.0 },{ 400.0, 0.0 , -150.0 } },
		{ { -400.0, 0.0 ,  150.0 },{ -150.0, 0.0 ,  150.0 },{ 150.0, 0.0 ,  150.0 },{ 400.0, 0.0 ,  150.0 } },
		{ { -400.0, 0.0 ,  400.0 },{ -150.0, 0.0 ,  400.0 },{ 150.0, 0.0 ,  400.0 },{ 400.0, 0.0 ,  400.0 } }
	};

	glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 4, 0.0, 1.0, 12, 4, &ctrlPoints[0][0][0]);
	glEnable(GL_MAP2_VERTEX_3);
	glMapGrid2f(10, 0.0, 1.0, 10, 0.0, 1.0);
	glEvalMesh2(GL_FILL, 0, 10, 0, 10);
	glPointSize(2.0);

}

GLvoid makeTree()
{
	for (int i = 0; i < Trees_Ob_Num; ++i) {
		if (trees[i].Crush == false)
		{

			glPushMatrix();
			//기둥
			glTranslated(trees[i].x, 15, trees[i].z);
			glScaled(1, 3, 1);
			glColor4f(0.3, 0.1, 0, 0);
			glutSolidCube(10);
			//나뭇잎
			glPushMatrix();
			glScaled(1, 0.5, 1);
			glTranslated(0, 10, 0);
			glColor3f(0, 1, 0);
			glutSolidCube(20);
			if (weatherMan == snowy) {
				glPushMatrix();
				glScaled(1.5, 1, 1);
				glTranslated(0, 10, 0);
				glColor3f(1, 1, 1);
				glutSolidCube(10);
				glPopMatrix();
			}

			glPopMatrix();
			glPopMatrix();
		}
	}
}

GLvoid makeRock()
{
	for (int i = 0; i < rocks_Ob_Num; ++i) {
		if (rocks[i].Crush == false)
		{
			glPushMatrix();
			//바위
			glTranslated(rocks[i].x, 5, rocks[i].z);
			glColor4f(0.5, 0.5, 0.5, 0);
			glutSolidCube(10);
			glPopMatrix();
		}
	}
}

GLvoid makeSnow()
{
	glColor3f(1, 1, 1);
	for (int i = 0; i < 100; ++i) {
		glPushMatrix();
		glTranslated(snows[i].x, snows[i].y, snows[i].z);
		glutSolidSphere(5, 10, 10);
		glPopMatrix();
	}
}

GLvoid mkitRain()
{
	glColor3f(0.2f, 1.0f, 1.0f);
	for (int i = 0; i < 250; ++i) {
		glPushMatrix();
		glTranslated(rains[i].x, rains[i].y, rains[i].z);
		glLineWidth(1);
		glBegin(GL_LINES);
		glVertex3f(rains[i].x, rains[i].y, rains[i].z);
		glVertex3f(rains[i].x, rains[i].y + 10, rains[i].z);
		glEnd();
		glPopMatrix();
	}
}

GLvoid madCloud()
{
	glColor3f(1, 1, 1);
	if (weatherMan == rainy || weatherMan == snowy)	glColor3f(0.35, 0.35, 0.35);
	for (int i = 0; i < 5; ++i) {

		glPushMatrix();
		glTranslated(clouds[i].x, clouds[i].y, clouds[i].z);
		glPushMatrix();
		glScaled(1.2, 1, 1.5);
		glutSolidCube(50);
		glPushMatrix();
		glTranslated(40, 0, 20);
		glScaled(1.0, 0.7, 0.7);
		glutSolidCube(50);
		glPushMatrix();
		glTranslated(-80, 0, 20);
		glutSolidCube(50);
		glPushMatrix();
		glTranslated(40, 0, 30);
		glutSolidCube(50);
		glPopMatrix();
		glPopMatrix();
		glPopMatrix();
		glPopMatrix();
		glPopMatrix();
	}
}
//
GLvoid Ob_Screen()
{
	if (obs_rock == true)
	{

		glPushMatrix();
		glColor3f(1.0, 1.0, 1.0f);
		glTranslatef(0.0, 100, 0.0);
		glBegin(GL_QUADS); //앞면 
		glColor3f(1.0f, 1.0f, 1.0f); // White

		glVertex3f(me.xx - 100.0, me.yy + 200.0, me.zz + 100.0);

		glColor3f(1.0f, 0.0, 1.0f); // Magenta

		glVertex3f(me.xx - 100.0, me.yy - 200.0, me.zz + 100.0);

		glColor3f(0.0f, 0.0f, 1.0f); // Blue

		glVertex3f(me.xx + 100.0, me.yy - 200.0, me.zz + 100.0);

		glColor3f(0.0f, 1.0f, 1.0f); // Cyan

		glVertex3f(me.xx + 100.0, me.yy + 200.0, me.zz + 100.0);
		glEnd();
		glPopMatrix();

	}
}


GLvoid drawCharacter()
{

	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslatef(me.xx, me.yy, me.zz);
	glRotatef(me.RotY, 0.0, 1.0, 0.0);
	//glRotatef(180, 0, 1, 0);

	//캐릭터 표시
	glPushMatrix();
	glTranslatef(me.posX, 100, me.posZ);
	glRotated(90, 1, 0, 0);
	glColor3f(0, 0, 1);
	glutSolidCone(10, 15, 10, 10);
	glPopMatrix();
	//몸통
	glPushMatrix();
	glScaled(1.0, 2.0, 1.0);
	glTranslated(me.posX, 20, me.posZ);
	makeTrumpnumb(ace, 10);
	glColor3f(1, 1, 1);
	glutSolidCube(20);
	glColor3f(0, 0, 0);
	glLineWidth(2);
	glutWireCube(20);
	glPopMatrix();


	//다리
	glPushMatrix();
	//왼다리
	glTranslated(0, 20, 0);
	glRotated(me.rotLeg, 1, 0, 0);
	glTranslated(0, -20, 0);
	glBegin(GL_LINES);
	glVertex3f(me.posX - 5, 20, me.posZ);
	glVertex3f(me.posX - 5, 1, me.posZ);

	//왼발	
	glVertex3f(me.posX - 5, 1, me.posZ);
	glVertex3f(me.posX - 5, 1, me.posZ - 10);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 20, 0);
	glRotated(-1 * me.rotLeg, 1, 0, 0);
	glTranslated(0, -20, 0);
	//오른다리 
	glBegin(GL_LINES);
	glVertex3f(me.posX + 5, 20, me.posZ);
	glVertex3f(me.posX + 5, 1, me.posZ);
	//오른발	
	glVertex3f(me.posX + 5, 1, me.posZ);
	glVertex3f(me.posX + 5, 1, me.posZ - 10);
	glEnd();
	glPopMatrix();

	//팔
	glPushMatrix();
	glTranslated(0, 30, 0);
	glRotated(-me.rotLeg, 1, 0, 0);
	glTranslated(0, -30, 0);
	glBegin(GL_LINES);
	//왼팔
	glVertex3f(me.posX - 10, 30, me.posZ);
	glVertex3f(me.posX - 14, 10, me.posZ);
	glEnd();
	glPopMatrix();
	//오른팔
	glPushMatrix();
	glTranslated(0, 30, 0);
	glRotated(me.rotLeg, 1, 0, 0);
	glTranslated(0, -30, 0);
	glBegin(GL_LINES);
	glVertex3f(me.posX + 10, 30, me.posZ);
	glVertex3f(me.posX + 14, 10, me.posZ);
	glEnd();
	glPopMatrix();

	glPopMatrix();

}
GLvoid Draw_enemy()
{
	for (int i = 0; i < 13; ++i)
	{
		glPushMatrix();
		glPushMatrix();
		glTranslated(enemy[i].xx, enemy[i].yy, enemy[i].zz);
		glRotatef(enemy[i].RotY, 0.0, 1.0, 0.0);
		glScaled(1.0, 2.0, 0.5);
		makeTrumpnumb(enemy[i].grade, 10);
		glColor3f(1, 1, 1);
		glutSolidCube(20);
		glColor3f(0, 0, 0);
		glLineWidth(2);
		glutWireCube(20);
		glPopMatrix();
		glPopMatrix();
	}
}
GLvoid makeTrumpnumb(TrumpNumber object, int z)
{
	switch (object) {
	case ace: {
		glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		glColor3f(1, 1, 1);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, z);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, textures[0]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, -z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, -z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, -z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, -z);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}
			  break;
	case two: {
		glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		glColor3f(1, 1, 1);
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, +z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, +z);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, textures[1]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, -z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, -z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, -z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, -z);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}
			  break;
	case three: {
		glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		glColor3f(1, 1, 1);
		glBindTexture(GL_TEXTURE_2D, textures[2]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, +z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, +z);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, textures[2]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, -z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, -z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, -z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, -z);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}
				break;
	case four: {
		glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		glColor3f(1, 1, 1);
		glBindTexture(GL_TEXTURE_2D, textures[3]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, +z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, +z);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, textures[3]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, -z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, -z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, -z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, -z);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}
			   break;
	case five: {
		glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		glColor3f(1, 1, 1);
		glBindTexture(GL_TEXTURE_2D, textures[4]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, +z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, +z);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, textures[4]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, -z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, -z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, -z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, -z);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}
			   break;
	case six: {
		glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		glColor3f(1, 1, 1);
		glBindTexture(GL_TEXTURE_2D, textures[5]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, +z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, +z);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, textures[5]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, -z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, -z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, -z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, -z);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}
			  break;
	case seven: {glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		glColor3f(1, 1, 1);
		glBindTexture(GL_TEXTURE_2D, textures[6]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, +z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, +z);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, textures[6]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, -z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, -z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, -z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, -z);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}
				break;
	case eight: {
		glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		glColor3f(1, 1, 1);
		glBindTexture(GL_TEXTURE_2D, textures[7]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, +z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, +z);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, textures[7]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, -z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, -z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, -z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, -z);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}
				break;
	case nine: {
		glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		glColor3f(1, 1, 1);
		glBindTexture(GL_TEXTURE_2D, textures[8]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, +z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, +z);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, textures[8]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, -z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, -z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, -z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, -z);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}
			   break;
	case ten: {glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		glColor3f(1, 1, 1);
		glBindTexture(GL_TEXTURE_2D, textures[9]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, +z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, +z);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, textures[9]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, -z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, -z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, -z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, -z);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}
			  break;
	case jack: {glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		glColor3f(1, 1, 1);
		glBindTexture(GL_TEXTURE_2D, textures[10]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, +z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, +z);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, textures[10]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, -z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, -z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, -z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, -z);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}
			   break;
	case queen: {glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		glColor3f(1, 1, 1);
		glBindTexture(GL_TEXTURE_2D, textures[11]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, +z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, +z);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, textures[11]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, -z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, -z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, -z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, -z);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}
				break;
	case king: {glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		glColor3f(1, 1, 1);
		glBindTexture(GL_TEXTURE_2D, textures[12]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, +z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, +z);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, textures[12]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, -z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, -z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, -z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, -z);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}
			   break;
	case joker: {
		glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		glColor3f(1, 1, 1);
		glBindTexture(GL_TEXTURE_2D, textures[13]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, +z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, +z);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, textures[13]);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-10, 10, -z);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-10, -10, -z);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(+10, -10, -z);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(+10, 10, -z);
		glEnd();
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
	}
				break;
	}
}

GLvoid settingLight()
{
	GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat diffuseLight[] = { 0.9f, 0.9f, 0.9f, 1.0 };
	GLfloat specular[] = { 0.5f, 0.5f, 0.5f, 1.0 };
	GLfloat lightPos[] = { 0 + me.xx, 300 , 0 + me.zz, 1.0 };
	GLfloat lightD[] = { 0,-1,0 };

	glEnable(GL_LIGHTING);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 30.0f);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightD);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 1);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHT0);
}

void initsetting()
{
	glGenTextures(14, textures);
	{
		for (int i = 0; i < 14; ++i) {
			glBindTexture(GL_TEXTURE_2D, textures[i]);
			if (i == 0)				bitmap = LoadDIBitmap("ace.bmp", &bitmapInfo);
			else if (i == 1)		bitmap = LoadDIBitmap("two.bmp", &bitmapInfo);
			else if (i == 2)		bitmap = LoadDIBitmap("three.bmp", &bitmapInfo);
			else if (i == 3)		bitmap = LoadDIBitmap("four.bmp", &bitmapInfo);
			else if (i == 4)		bitmap = LoadDIBitmap("five.bmp", &bitmapInfo);
			else if (i == 5)		bitmap = LoadDIBitmap("six.bmp", &bitmapInfo);
			else if (i == 6)		bitmap = LoadDIBitmap("seven.bmp", &bitmapInfo);
			else if (i == 7)		bitmap = LoadDIBitmap("eight.bmp", &bitmapInfo);
			else if (i == 8)		bitmap = LoadDIBitmap("nine.bmp", &bitmapInfo);
			else if (i == 9)		bitmap = LoadDIBitmap("ten.bmp", &bitmapInfo);
			else if (i == 10)		bitmap = LoadDIBitmap("jack.bmp", &bitmapInfo);
			else if (i == 11)		bitmap = LoadDIBitmap("queen.bmp", &bitmapInfo);
			else if (i == 12)		bitmap = LoadDIBitmap("king.bmp", &bitmapInfo);
			else if (i == 13)		bitmap = LoadDIBitmap("joker.bmp", &bitmapInfo);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, 47, 68, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bitmap);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}
	}
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
}

// 객체 애니메이션 함수 

GLvoid snowing()
{
	for (int i = 0; i < 300; ++i) {
		snows[i].y -= snows[i].speed * 3;
		if (snows[i].y <= 0)		snows[i].y = 600;
	}
}

GLvoid raining()
{
	for (int i = 0; i < 300; ++i) {
		rains[i].y -= rains[i].speed * 5;
		if (rains[i].y <= 0)		rains[i].y = 600;
	}
}

GLvoid clouding()
{
	for (int i = 0; i < 7; ++i) {

		clouds[i].x += clouds[i].speed;
		if (clouds[i].x >= 600) {
			clouds[i].x = -600;
			cout << "cloud reset" << endl;
		}
	}

}

GLvoid move(int value)
{
	if (TitleScene == false) {
		me.xx -= me.speed * sin(getRadian(me.RotY));
		me.zz -= me.speed * cos(getRadian(me.RotY));
		if (LegArmrev == false) {
			me.rotLeg += 5;
			me.rotArm += 5;
			if (me.rotLeg >= 30) LegArmrev = true;
		}
		else if (LegArmrev == true) {
			me.rotLeg -= 5;
			me.rotArm -= 5;
			if (me.rotLeg <= -30) LegArmrev = false;
		}
		if (firstCameraz >= 300)
			firstCameraz -= 10;
	}

	// 구름 그려주기 
	clouding();

	// 날씨: Rain
	if (weatherMan == rainy) {
		me.speed = 5;
		raining();
	}

	// 날씨 : Snow
	else if (weatherMan == snowy) {
		me.increseRad = 6;
		snowing();
	}

	//타이틀 색깔바꿔주기
	if (ru == true) {
		r += 0.1;
		if (r >= 1.0) { r = 1.0; ru = false; gu = true; }
	}
	else if (rd == true) {
		r -= 0.1;
		if (r <= 0.0) { r = 0.0; rd = false; gd = true; }
	}
	else if (gu == true) {
		g += 0.1;
		if (g >= 1.0) { g = 1.0; gu = false; bu = true; }
	}
	else if (gd == true) {
		g -= 0.1;
		if (g <= 0.0) { g = 0.0; gd = false; bd = true; }
	}
	else if (bu == true) {
		b += 0.1;
		if (b >= 1.0) { b = 1.0; bu = false; rd = true; }
	}
	else if (bd == true) {
		b -= 0.1;
		if (b <= 0.0) { b = 0.0; bd = false; ru = true; }
	}

	//if (firstCameraz > 300)	firstCameraz -= 10;

	glutPostRedisplay();   // 화면 재 출력 
	glutTimerFunc(50, move, 1); // 타이머함수 재 설정
}

GLvoid Cliff_Check(int value)
{

	if (me.xx < -400.0 || me.xx > 400)
	{

		me.yy -= 10;
		if (me.yy <= -700)
		{
			me.xx = 0.0;
			me.yy = 0.0;
			me.zz = 0.0;
		}
	}
	if (me.zz < -400.0 || me.zz > 400)
	{

		me.yy -= 10;
		if (me.yy <= -700)
		{
			me.xx = 0.0;
			me.yy = 0.0;
			me.zz = 0.0;
		}
	}

	glutPostRedisplay();   // 화면 재 출력 
	glutTimerFunc(50, Cliff_Check, 1); // 타이머함수 재 설정
}

GLvoid Get_Check(int value)
{

	for (int i = 0; i < 13; ++i)
	{
		if (enemy[i].Get == true)
		{
			enemy[i].xx = me.xx + (10 * (3 * sin(getRadian(me.RotY))));
			enemy[i].zz = me.zz + (10 * (3 * cos(getRadian(me.RotY))));
			enemy[i].yy = me.yy + 30 + i * 20;
			enemy[i].RotY = me.RotY;
		}
	}

	glutPostRedisplay();   // 화면 재 출력 
	glutTimerFunc(10, Get_Check, 1); // 타이머함수 재 설정
}

GLvoid enemy_Rotate(int value)
{


	for (int i = 0; i < 13; ++i)
	{
		if (enemy[i].Get == false)
		{

			if (enemy[i].Reverse == false)
				enemy[i].yy += 2;
			else
				enemy[i].yy -= 2;

			enemy[i].RotY += 3;

			if (enemy[i].RotY == 360.0)
				enemy[i].RotY = 0;
			if (enemy[i].yy == 40)
				enemy[i].Reverse = true;
			else if (enemy[i].yy == 20)
				enemy[i].Reverse = false;
		}
	}

	glutPostRedisplay();   // 화면 재 출력 
	glutTimerFunc(50, enemy_Rotate, 1); // 타이머함수 재 설정

}

GLvoid Atmosphere(int value)
{
	int weaV = rand() % 3;
	switch (weaV) {
	case 0:
		weatherMan = sunny;
		cout << "sunny" << endl;
		break;
	case 1:
		weatherMan = rainy;
		cout << "rainy" << endl;
		break;
	case 2:
		weatherMan = snowy;
		cout << "snowy" << endl;
		break;
	}
	glutPostRedisplay();
	glutTimerFunc(5000, Atmosphere, 1);
}


GLvoid Crush_Check(int value)
{
	GLfloat  R1 = 0.0;
	GLfloat  R2 = 0.0;

	for (int i = 0; i < 14; ++i)
	{
		if (enemy[i].Get == false)
		{
			//  me 의 x,z 좌표와  card 들의 x,y 좌표사이의 거리를 계산 
			R1 = sqrt((me.xx - (enemy[i].xx - 5.0))* (me.xx - (enemy[i].xx - 5.0)) + (me.zz - enemy[i].zz)* (me.zz - enemy[i].zz));
			R2 = sqrt((me.xx - (enemy[i].xx + 5.0))* (me.xx - (enemy[i].xx + 5.0)) + (me.zz - enemy[i].zz)* (me.zz - enemy[i].zz));

			if (((R1 <= Me_Radi + Enemy_Radi) || (R2 <= Me_Radi + Enemy_Radi)) && enemy[i].grade == G + 1)
			{
				cout << "Crush!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
				cout << "enemy[i].Get" << i << "번쨰 " << G + 1 << endl;
				enemy[i].Get = true;

				++G;
				cout << "다음에 찾을 카드 :" << G << endl;
			}

		}
	}

	glutPostRedisplay();   // 화면 재 출력 
	glutTimerFunc(50, Crush_Check, 1); // 타이머함수 재 설정
} // 카드 get 

GLvoid Ob_Crush_Check(int value)
{
	//trees[20]
	//rocks[20]

	GLfloat R1 = 0;
	GLfloat R2 = 0;

	for (int i = 0; i < Trees_Ob_Num; ++i) // 나무와의 충돌 체크 
	{
		if (trees[i].Crush == false)
		{
			R1 = sqrt((me.xx - trees[i].x)* (me.xx - trees[i].x) + (me.zz - trees[i].z)* (me.zz - trees[i].z));

			if (R1 <= Me_Radi + Obs_Radi)
			{
				trees[i].Crush = true;  // 나무와 충돌 


				cout << "없어질 Grade" << G << endl;
				if (G == 0)
					continue;
				else {
					--G;
				}
				cout << "enemy[G].grade " << enemy[G].grade << endl;
				enemy[G].xx = rand() % 800 - 400;
				enemy[G].zz = rand() % 800 - 400;
				enemy[G].yy = 20;
				enemy[G].Get = false;
				enemy[G].Reverse = false;
				cout << "Grade" << G << endl;
			}
		}
	}

	for (int i = 0; i < rocks_Ob_Num; ++i)
	{
		if (rocks[i].Crush == false) // 돌과의 충돌 체크 
		{
			R1 = sqrt((me.xx - rocks[i].x)* (me.xx - rocks[i].x) + (me.zz - rocks[i].z)* (me.zz - rocks[i].z));

			if (R1 <= Me_Radi + Obs_Radi)
			{
				rocks[i].Crush = true;

				obs_rock = true;
				////////////////////////////////////////////////////////////////

			}
		}
	}


	glutPostRedisplay();   // 화면 재 출력 
	glutTimerFunc(50, Ob_Crush_Check, 1); // 타이머함수 재 설정
}

GLvoid Win_Check(int value) // 다먹었을떄승리 체크 
{

	int get_num = 0;
	for (int j = 0; j < 13; ++j) // 모든 적들이 다 멱혔는지 확인한다. 
	{
		if (enemy[j].Get == true)
			get_num++;
		if (get_num == enemy_num) // 다먹혔다면 
		{

			// 캐릭터 위치를 초기화  
			me.xx = 0.0;
			me.yy = 0.0;
			me.zz = 0.0;

			// 카드들 재분배  
			for (int i = 0; i < 13; ++i)
			{
				enemy[i].grade = static_cast<TrumpNumber>(i + 1);
				enemy[i].Get = false;
				enemy[i].Reverse = false;
				enemy[i].xx = rand() % 800 - 400;
				enemy[i].yy = 20;
				enemy[i].zz = rand() % 800 - 400;
				G = 0;  // 먹을거 다시  1부터 먹게 한다  
			}
			Trees_Ob_Num += 10;
			rocks_Ob_Num += 5;
			ob_Setting();
			Stage++;

		}
	}
	glutPostRedisplay();   // 화면 재 출력 
	glutTimerFunc(50, Win_Check, 1); // 타이머함수 재 설정
}

//밤낮 바꿔주기 8초에 한번 - by Hong
GLvoid timeCheck(int value)
{
	if (night == false) {
		nightC -= 0.1;
		if (nightC2 >= 0.0)	nightC2 -= 0.1;
	}
	if (nightC <= 0.0) {
		night = true;
	}
	if (night == true) {
		nightC += 0.1;
		if (nightC2 <= 0.3)	nightC2 += 0.1;
		if (nightC >= 0.8) night = false;
	}
	glutPostRedisplay();
	glutTimerFunc(1500, timeCheck, 1);
}

GLvoid obs_Rock_Timer(int value)
{

	if (obs_rock == true)
	{
		obs_rock = false;
	}

	glutPostRedisplay();
	glutTimerFunc(2000, obs_Rock_Timer, 1);
}