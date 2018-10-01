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


////////////////////////  me �׸��� 

struct Character {
	GLfloat posX, posZ;		// ��ü���� ���� ��ġ�� ��Ÿ���� ����(��� �κп� �����ұ�)
	GLfloat speed = 5;		// ������ ��ȭ�� ���� �ӵ��� �޸� �ϱ����� ����
	GLfloat xx;				// ��ü�� x�� �̵��� ���� ��ǥ �� (glTranslatef() �Լ��� ����)
	GLfloat yy;				// ��ü�� y�� �̵��� ���� ��ǥ �� (glTranslatef() �Լ��� ����)
	GLfloat zz;				// ��ü�� z�� �̵��� ���� ��ǥ �� (glTranslatef() �Լ��� ����)
	bool Get = false;		// false - non_get , true - get (ī�带 ����°������� bool����)
	bool Reverse = false;	// false - uo , true -down ( ī��ִϸ��̼ǿ� ���̴� bool ����)
	GLfloat RotY = 0.0;		// ��ü�� Y�� ȸ���� ���� ����
	GLfloat rotLeg = 0;		// �����ϴ� ĳ������ �ȴٸ� �ִϸ��̼��� ���� ���� 
	GLfloat rotArm = 0;		// �����ϴ� ĳ������ �ȴٸ� �ִϸ��̼��� ���� ����
	GLfloat increseRad = 3; // ������ ���� Ű���������� ���� ���� �޸� �ϱ����� ����
	TrumpNumber grade;		// ī���� ������ �������� ��Ÿ���� ���� (Ace~~~ Joker ����)
};

typedef struct MapSource {
	int x = rand() % 800 - 400;		//��Ÿ������ �ϴ� ��ֹ� Ȥ�� ���� ��ü�� ���� ������ ����
	int y = rand() % 400 + 300;		//��Ÿ������ �ϴ� ��ֹ� Ȥ�� ���� ��ü�� ���� ������ ����
	int z = rand() % 800 - 400;		//��Ÿ������ �ϴ� ��ֹ� Ȥ�� ���� ��ü�� ���� ������ ����
	int speed = rand() % 3 + 1;		//���� ��ü�� �̵� �ӵ��� ��Ÿ���� ���� ����
	bool Crush = false;				// ��ֹ��� �浹üũ�� Ȯ�ν��� �ֱ� ���� ����
}MapSource;

///////////////////////////////////////////////////
// �ִϸ��̼� �Լ� 
GLvoid move(int value);
GLvoid Cliff_Check(int value);		// ���� üũ
GLvoid Get_Check(int value);		// ����ٴϰ��ϱ�
GLvoid Crush_Check(int value);		// ī�� �浹 üũ
GLvoid Ob_Crush_Check(int value);	// ��ֹ� �浹 üũ
GLvoid Win_Check(int value);		// ���� ���������� ���� ���� �¸� üũ
GLvoid obs_Rock_Timer(int value);	// ������ �ε����� ȭ�鰡���� ���� �Լ�
GLvoid ob_Setting();				// ���ο� ������������ ��ֹ� �÷��ֱ�
GLvoid Ob_Screen();					// ȭ�� ������ �׸���

void initsetting();					// ��Ʈ�� �ҷ�����

GLvoid makeLand();					// �������
GLvoid makeTree();					// ���������
GLvoid makeRock();					// �������
GLvoid makeSnow();					// �������
GLvoid mkitRain();					// �񸸵��
GLvoid madCloud();					// ���������
GLvoid Enemy_setting();				// ī��� ����
GLvoid setting();					// ī�޶� ����
GLvoid enemy_Rotate(int value);		// ī��� �����ֱ� 
GLvoid Atmosphere(int value);		// ���� ��ȭ
GLvoid timeCheck(int value);		// �㳷 �ٲ������
GLvoid clouding();					// �����̵�
GLvoid raining();					// �񳻸���
GLvoid snowing();					// ��������
GLvoid drawCharacter();				// ���ΰ��׸���
GLvoid Draw_enemy();				// ī��� �׸���
GLvoid drawTitle();					// Ÿ��Ʋ �׸���

// �߰� ���� ����� ����Ʈ ����Ʈ �Լ�
GLvoid settingLight();


///////// �������� ���� 
int Stage = 1;
int Trees_Ob_Num = 20;
int rocks_Ob_Num = 10;

// ������ ���� 
MapSource *trees;
MapSource *rocks;
MapSource snows[300];
MapSource rains[300];
MapSource clouds[7];
Weather weatherMan = sunny;
bool obs_rock = false;

//�ؽ�ó ����
GLuint textures[14];
GLubyte* bitmap;
BITMAPINFO* bitmapInfo;


//ĳ���� ���� 
TrumpNumber grade;
Character me;
Character* enemy;
bool LegArmrev = false;
int G = 0;


// Ÿ��Ʋ ���� ���� ���� �����ϱ� ���� ���� 
bool TitleScene = true;
// Ÿ��Ʋ���� ���ξ� ������ ī�޶� ������ ���� ���� 
int firstCameraz = 800;
// ���� ���� ��ȭ�� ���� ���� - by Hong
GLfloat r = 0, g = 0, b = 0;
// ���� ������ �ٲ��ֱ� ���� ���� - by Hong
GLfloat nightC = 0.8, nightC2 = 0.3;
// ���� ������ ��� �ٲ��ֱ� ���� ������ 
// red up, down green up, down blue up, down - by Hong
bool ru = true, gu = false, bu = false;
bool rd = false, gd = false, bd = false;
// ������ �ƴ��� ǥ�� 
bool night = false;
// Ÿ��Ʋȭ�� ���� ǥ�� 
char* string1 = { "Press any key Start" };




void main()															// ������ ����ϰ� ����Լ� ���� 
{

	PlaySound(TEXT("ȫ���ı�.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NOSTOP || SND_LOOP);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);		// ���÷��� ��� ����       
	glutInitWindowPosition(400, 100);								// �������� ��ġ ����        
	glutInitWindowSize(800, 600);									// �������� ũ�� ����        
	glutCreateWindow("Example1");									// ������ ���� (������ �̸�)   
	glutDisplayFunc(drawScene);										// ��� �Լ��� ����           
	glutSpecialFunc(SpecialKeyboard);								// ����� Ű����
	glutKeyboardFunc(Keyboard);
	Enemy_setting();
	// ��Ʈ�� ����
	initsetting();
	ob_Setting();

	glutTimerFunc(50, obs_Rock_Timer, 1);
	glutTimerFunc(50, Cliff_Check, 1);	// ������ ���������� üũ�Ѵ�
	glutTimerFunc(50, Crush_Check, 1);	// ī��� ĳ���Ͱ� ��Ҵ��� üũ�Ѵ�
	glutTimerFunc(50, enemy_Rotate, 1); // ī�� �ִϸ��̼� 
	glutTimerFunc(50, move, 1);         // ĳ���� ������ �ִϸ��̼� 
	glutTimerFunc(50, Get_Check, 1);    // ī�带 ������� üũ�Ѵ�.
	glutTimerFunc(50, Ob_Crush_Check, 1);  // ��ֹ��� ��Ҵ��� üũ�Ѵ� 
	glutTimerFunc(50, Win_Check, 1);	 //  ī�带 �� ���������� �������������� �Ѿ�� �Ѵ�.  --> ��ֹ������� �̷��� ���� �й� ���ߴ�. 									 
	glutTimerFunc(5000, Atmosphere, 1); // ���ȭ 5�ʿ� �ѹ��� �������� ��ȭ������
	// ��ħ�� �ð�üũ�� ���� ���� - by Hong
	glutTimerFunc(1500, timeCheck, 1);
	// Ÿ�̸�
	glutReshapeFunc(Reshape);
	glutPostRedisplay();                                            // �ٽ� �׸��� �Լ� ����    
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();										            // �̺�Ʈ ó�� ���� 

}

GLvoid drawScene()										            // ��� �Լ� 
{   //ī�޶� ����
	setting();

	makeLand(); //Map
	makeTree(); //Tree
	makeRock(); // Rock
	madCloud(); // cloud 

	if (weatherMan == rainy)		mkitRain();
	if (weatherMan == snowy)		makeSnow();

	drawCharacter(); // ĳ���� �׸��� �Լ�

	Draw_enemy(); //   ī��� �׸��� �Լ� 
	Ob_Screen();

	//Ÿ��Ʋ �� ������ Ʈ�� �̸� Ÿ��Ʋ ȭ���� �׷��ش�
	if (TitleScene == true) drawTitle();
	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)							// �ٽ� �׸��� �Լ� 
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, w / h, 1.0, 2000.0);			// ���� ��ȯ ����(��������)
	glMatrixMode(GL_MODELVIEW);

}

GLvoid Shape(int key, int x, int y)
{

	glutPostRedisplay();
}
GLvoid Keyboard(unsigned char key, int x, int y)
{

	// �ƹ�Ű�� ������ Ÿ��Ʋ ���� Ǯ����
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

//��Ʈ���б�

GLubyte* LoadDIBitmap(const char *filename, BITMAPINFO **info)
{
	FILE *fp;
	GLubyte *bits;
	int bitsize, infosize;
	BITMAPFILEHEADER header;

	// ���̳ʸ� �б� ���� ������ ����
	if ((fp = fopen(filename, "rb")) == NULL)   return NULL;

	// ��Ʈ�� ���� ����� �д´�.
	if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1)
	{
		fclose(fp);
		return NULL;
	}

	// ������ BMP �������� Ȯ���Ѵ�.
	if (header.bfType != 'MB')
	{
		fclose(fp);
		return NULL;
	}

	// BITMAPINFOHEADER ��ġ�� ����.
	infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);

	// ��Ʈ�� �̹��� �����͸� ���� �޸� �Ҵ��� �Ѵ�.
	if ((*info = (BITMAPINFO *)malloc(infosize)) == NULL)
	{
		fclose(fp);
		exit(0);
		return NULL;
	}

	// ��Ʈ�� ���� ����� �д´�.
	if (fread(*info, 1, infosize, fp) < (unsigned int)infosize)
	{
		free(*info);
		fclose(fp);
		return NULL;
	}

	// ��Ʈ���� ũ�� ����
	if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
		bitsize = ((*info)->bmiHeader.biWidth
			* (*info)->bmiHeader.biBitCount + 7) / 8.0
		* abs((*info)->bmiHeader.biHeight);

	// ��Ʈ���� ũ�⸸ŭ �޸𸮸� �Ҵ��Ѵ�.
	if ((bits = (unsigned char *)malloc(bitsize)) == NULL)
	{
		free(*info);
		fclose(fp);
		return NULL;
	}
	// ��Ʈ�� �����͸� bit(GLubyte Ÿ��)�� �����Ѵ�. 
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

//Ÿ��Ʋ ȭ�� �׷��ֱ� - by Hong
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

//ī�޶� ���� 
GLvoid setting()
{
	//���� �Ǹ� ����Ʈ�� �����ؼ� ����Ʈ����Ʈ�� �ٲ��ش�. - by Hong
	if (night == true)	settingLight();
	// ���̸� ���� ���� - by Hong
	else				glDisable(GL_LIGHTING);
	//
	glClearColor(0.0f, nightC2, nightC, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// ������ ������ ��ü�� ĥ�ϱ�
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();										// �������� ���ֱ�
															// ī�޶� ȭ�� ���� ����ٴϱ� - by Hong
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


// ��ü ���� �Լ� 
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
			//���
			glTranslated(trees[i].x, 15, trees[i].z);
			glScaled(1, 3, 1);
			glColor4f(0.3, 0.1, 0, 0);
			glutSolidCube(10);
			//������
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
			//����
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
		glBegin(GL_QUADS); //�ո� 
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

	//ĳ���� ǥ��
	glPushMatrix();
	glTranslatef(me.posX, 100, me.posZ);
	glRotated(90, 1, 0, 0);
	glColor3f(0, 0, 1);
	glutSolidCone(10, 15, 10, 10);
	glPopMatrix();
	//����
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


	//�ٸ�
	glPushMatrix();
	//�޴ٸ�
	glTranslated(0, 20, 0);
	glRotated(me.rotLeg, 1, 0, 0);
	glTranslated(0, -20, 0);
	glBegin(GL_LINES);
	glVertex3f(me.posX - 5, 20, me.posZ);
	glVertex3f(me.posX - 5, 1, me.posZ);

	//�޹�	
	glVertex3f(me.posX - 5, 1, me.posZ);
	glVertex3f(me.posX - 5, 1, me.posZ - 10);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 20, 0);
	glRotated(-1 * me.rotLeg, 1, 0, 0);
	glTranslated(0, -20, 0);
	//�����ٸ� 
	glBegin(GL_LINES);
	glVertex3f(me.posX + 5, 20, me.posZ);
	glVertex3f(me.posX + 5, 1, me.posZ);
	//������	
	glVertex3f(me.posX + 5, 1, me.posZ);
	glVertex3f(me.posX + 5, 1, me.posZ - 10);
	glEnd();
	glPopMatrix();

	//��
	glPushMatrix();
	glTranslated(0, 30, 0);
	glRotated(-me.rotLeg, 1, 0, 0);
	glTranslated(0, -30, 0);
	glBegin(GL_LINES);
	//����
	glVertex3f(me.posX - 10, 30, me.posZ);
	glVertex3f(me.posX - 14, 10, me.posZ);
	glEnd();
	glPopMatrix();
	//������
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

// ��ü �ִϸ��̼� �Լ� 

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

	// ���� �׷��ֱ� 
	clouding();

	// ����: Rain
	if (weatherMan == rainy) {
		me.speed = 5;
		raining();
	}

	// ���� : Snow
	else if (weatherMan == snowy) {
		me.increseRad = 6;
		snowing();
	}

	//Ÿ��Ʋ ����ٲ��ֱ�
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

	glutPostRedisplay();   // ȭ�� �� ��� 
	glutTimerFunc(50, move, 1); // Ÿ�̸��Լ� �� ����
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

	glutPostRedisplay();   // ȭ�� �� ��� 
	glutTimerFunc(50, Cliff_Check, 1); // Ÿ�̸��Լ� �� ����
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

	glutPostRedisplay();   // ȭ�� �� ��� 
	glutTimerFunc(10, Get_Check, 1); // Ÿ�̸��Լ� �� ����
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

	glutPostRedisplay();   // ȭ�� �� ��� 
	glutTimerFunc(50, enemy_Rotate, 1); // Ÿ�̸��Լ� �� ����

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
			//  me �� x,z ��ǥ��  card ���� x,y ��ǥ������ �Ÿ��� ��� 
			R1 = sqrt((me.xx - (enemy[i].xx - 5.0))* (me.xx - (enemy[i].xx - 5.0)) + (me.zz - enemy[i].zz)* (me.zz - enemy[i].zz));
			R2 = sqrt((me.xx - (enemy[i].xx + 5.0))* (me.xx - (enemy[i].xx + 5.0)) + (me.zz - enemy[i].zz)* (me.zz - enemy[i].zz));

			if (((R1 <= Me_Radi + Enemy_Radi) || (R2 <= Me_Radi + Enemy_Radi)) && enemy[i].grade == G + 1)
			{
				cout << "Crush!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
				cout << "enemy[i].Get" << i << "���� " << G + 1 << endl;
				enemy[i].Get = true;

				++G;
				cout << "������ ã�� ī�� :" << G << endl;
			}

		}
	}

	glutPostRedisplay();   // ȭ�� �� ��� 
	glutTimerFunc(50, Crush_Check, 1); // Ÿ�̸��Լ� �� ����
} // ī�� get 

GLvoid Ob_Crush_Check(int value)
{
	//trees[20]
	//rocks[20]

	GLfloat R1 = 0;
	GLfloat R2 = 0;

	for (int i = 0; i < Trees_Ob_Num; ++i) // �������� �浹 üũ 
	{
		if (trees[i].Crush == false)
		{
			R1 = sqrt((me.xx - trees[i].x)* (me.xx - trees[i].x) + (me.zz - trees[i].z)* (me.zz - trees[i].z));

			if (R1 <= Me_Radi + Obs_Radi)
			{
				trees[i].Crush = true;  // ������ �浹 


				cout << "������ Grade" << G << endl;
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
		if (rocks[i].Crush == false) // ������ �浹 üũ 
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


	glutPostRedisplay();   // ȭ�� �� ��� 
	glutTimerFunc(50, Ob_Crush_Check, 1); // Ÿ�̸��Լ� �� ����
}

GLvoid Win_Check(int value) // �ٸԾ������¸� üũ 
{

	int get_num = 0;
	for (int j = 0; j < 13; ++j) // ��� ������ �� �������� Ȯ���Ѵ�. 
	{
		if (enemy[j].Get == true)
			get_num++;
		if (get_num == enemy_num) // �ٸ����ٸ� 
		{

			// ĳ���� ��ġ�� �ʱ�ȭ  
			me.xx = 0.0;
			me.yy = 0.0;
			me.zz = 0.0;

			// ī��� ��й�  
			for (int i = 0; i < 13; ++i)
			{
				enemy[i].grade = static_cast<TrumpNumber>(i + 1);
				enemy[i].Get = false;
				enemy[i].Reverse = false;
				enemy[i].xx = rand() % 800 - 400;
				enemy[i].yy = 20;
				enemy[i].zz = rand() % 800 - 400;
				G = 0;  // ������ �ٽ�  1���� �԰� �Ѵ�  
			}
			Trees_Ob_Num += 10;
			rocks_Ob_Num += 5;
			ob_Setting();
			Stage++;

		}
	}
	glutPostRedisplay();   // ȭ�� �� ��� 
	glutTimerFunc(50, Win_Check, 1); // Ÿ�̸��Լ� �� ����
}

//�㳷 �ٲ��ֱ� 8�ʿ� �ѹ� - by Hong
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