#include "stdafx.h"
#include "GameObject.h"
#define BALLSPEED 1.6f
#pragma warning (disable :4996)

GameObject::GameObject()
{
	startstate = 1;
	t = new Texture();

	//system = new GameSystem(&height_buffer, &position, b, &ballbottom, &ballpower, &startstate, &ballspeed_switch, &balldisappear, &fog_switch, &stopmotion, &finish, &ballstate);
	p = new Particle();
	s = new GameSound();
}

GameObject::~GameObject()
{
}

void  GameObject::Draw_Menu()
{
	GLfloat White[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	float position[3] = { -80,805,25 };
	glRasterPos3f(position[0], position[1], position[2]);
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glPopMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, White);
	t->SetBind(15);
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(-300, 600, 300);
	glTexCoord2f(1, 0);
	glVertex3f(300, 600, 300);
	glTexCoord2f(1, 1);
	glVertex3f(300, 600, -300);
	glTexCoord2f(0, 1);
	glVertex3f(-300, 600, -300);
	glEnd();
	glPopMatrix();
}

void GameObject::SetSBuffer(SERVER_STRUCT&  SBUFFER)
{
	Sbuffer = SBUFFER;
	
}

void GameObject::PlayerMove()
{
	
	if (Sbuffer.A[Sbuffer.pIndex].state)
	{
		//if (GetFocus() != NULL)
		//{
		if (GetAsyncKeyState(VK_LEFT)) 
		{
			Sbuffer.A[Sbuffer.pIndex].x -= BALLSPEED;
		}
		if (GetAsyncKeyState(VK_RIGHT))
		{
			Sbuffer.A[Sbuffer.pIndex].x += BALLSPEED;
		}
		if (GetAsyncKeyState(VK_UP)) 
		{
			Sbuffer.A[Sbuffer.pIndex].z -= BALLSPEED;
		}
		if (GetAsyncKeyState(VK_DOWN)) 
		{
			Sbuffer.A[Sbuffer.pIndex].z += BALLSPEED;
		}
		//}
	}
}

void GameObject::Draw_Fog()
{
	GLfloat fog_color[4] = { 0.905, 0.905, 0.905, 1.0 };
	GLfloat density = 1.0;
	GLfloat start = 0.0f;
	GLfloat end = 230.0f;
	glFogf(GL_FOG_MODE, GL_LINEAR);
	glFogfv(GL_FOG_COLOR, fog_color);
	glFogf(GL_FOG_START, start);
	glFogf(GL_FOG_END, end);
	glFogf(GL_FOG_DENSITY, density);
}

void GameObject::Draw_Board()
{
	GLfloat Blue[] = { 0.980392f, 0.980392f, 0.823529f, 1.0f };
	GLfloat White[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat Red[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	static double stop_timer = 30.0;
		
	for (int i = 0; i < BOARDMAX; i++)
	{
		int bind = Sbuffer.Board[i].texture;

		if (bind == 1) { t->SetBind(0); }
		else if (bind == 2) { t->SetBind(9); }
		else if (bind == 3) { t->SetBind(10); }
		else if (bind == 4) { t->SetBind(11); }
		else if (bind == 5) { t->SetBind(12); }
		else if (bind == 6) { t->SetBind(13); }
		else if (bind == 7) { t->SetBind(14); }
		else { t->SetBind(0); }

		glBegin(GL_POLYGON);
		glTexCoord2f(0, 1);
		glVertex3f(Sbuffer.Board[i].x[0], Sbuffer.Board[i].y - 4, Sbuffer.Board[i].z[0]);
		glTexCoord2f(0, 0);
		glVertex3f(Sbuffer.Board[i].x[0], Sbuffer.Board[i].y - 4, Sbuffer.Board[i].z[1]);
		glTexCoord2f(1, 0);
		glVertex3f(Sbuffer.Board[i].x[1], Sbuffer.Board[i].y - 4, Sbuffer.Board[i].z[1]);
		glTexCoord2f(1, 1);
		glVertex3f(Sbuffer.Board[i].x[1], Sbuffer.Board[i].y - 4, Sbuffer.Board[i].z[0]);
		glEnd();
		if (bind != 3) {
			glBegin(GL_POLYGON);
			glTexCoord2f(1, 1);
			glVertex3f(Sbuffer.Board[i].x[0], Sbuffer.Board[i].y - 8, Sbuffer.Board[i].z[1]);
			glTexCoord2f(0, 1);
			glVertex3f(Sbuffer.Board[i].x[1], Sbuffer.Board[i].y - 8, Sbuffer.Board[i].z[1]);
			glTexCoord2f(0, 0);
			glVertex3f(Sbuffer.Board[i].x[1], Sbuffer.Board[i].y - 4, Sbuffer.Board[i].z[1]);
			glTexCoord2f(1, 0);
			glVertex3f(Sbuffer.Board[i].x[0], Sbuffer.Board[i].y - 4, Sbuffer.Board[i].z[1]);
			glEnd();

			glBegin(GL_POLYGON);
			glTexCoord2f(1, 1);
			glVertex3f(Sbuffer.Board[i].x[0], Sbuffer.Board[i].y - 8, Sbuffer.Board[i].z[0]);
			glTexCoord2f(0, 1);
			glVertex3f(Sbuffer.Board[i].x[0], Sbuffer.Board[i].y - 8, Sbuffer.Board[i].z[1]);
			glTexCoord2f(0, 0);
			glVertex3f(Sbuffer.Board[i].x[0], Sbuffer.Board[i].y - 4, Sbuffer.Board[i].z[1]);
			glTexCoord2f(1, 0);
			glVertex3f(Sbuffer.Board[i].x[0], Sbuffer.Board[i].y - 4, Sbuffer.Board[i].z[0]);
			glEnd();

			glBegin(GL_POLYGON);
			glTexCoord2f(0, 1);
			glVertex3f(Sbuffer.Board[i].x[1], Sbuffer.Board[i].y - 8, Sbuffer.Board[i].z[1]);
			glTexCoord2f(1, 1);
			glVertex3f(Sbuffer.Board[i].x[1], Sbuffer.Board[i].y - 8, Sbuffer.Board[i].z[0]);
			glTexCoord2f(1, 0);
			glVertex3f(Sbuffer.Board[i].x[1], Sbuffer.Board[i].y - 4, Sbuffer.Board[i].z[0]);
			glTexCoord2f(0, 0);
			glVertex3f(Sbuffer.Board[i].x[1], Sbuffer.Board[i].y - 4, Sbuffer.Board[i].z[1]);
			glEnd();
		}

		glPushMatrix();
		glTranslatef((Sbuffer.Board[i].x[0] + Sbuffer.Board[i].x[1]) / 2, Sbuffer.Board[i].y + 0.2, (Sbuffer.Board[i].z[0] + Sbuffer.Board[i].z[1]) / 2);
		if (Sbuffer.Board[i].item != 0) 
		{
			Draw_ItemBox(Sbuffer.Board[i].item);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, White);
		}
		glPopMatrix();
	}
}

void GameObject::Draw_ItemBox(int effect)
{
	static int ry = 0;
	if (effect == 1) {
		GLfloat Red[] = { 0.7f, 0.0f, 0.0f, 1.0f };
		glPushMatrix();
		glTranslatef(0, 4, 0);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Red);
		glRotatef(ry, 0, 1, 0);
		glutSolidCube(8);
		glPopMatrix();
		ry++;
	}
	else if (effect == 2)
	{
		GLfloat Black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glPushMatrix();
		glTranslatef(0, 6, 0);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Black);
		glRotatef(ry, 0, 1, 0);
		glutSolidCube(12);
		glPopMatrix();
		ry++;
	}

}

void GameObject::Draw_Ball()
{
	GLfloat AColor[] = { 0.6f, 0.0f, 0.0f, 1.0f };
	GLfloat BColor[] = { 0.0f, 0.6f, 0.0f, 1.0f };
	GLfloat CColor[] = { 0.0f, 0.0f, 0.6f, 1.0f };
	GLfloat Invisible[] = { 0.9f, 0.9f, 0.9f, 0.1f };

	/************************************************************************A*/
	if (Sbuffer.A[A].state)
	{
		glPushMatrix();
		if (Sbuffer.effect[FIRST_NO_ALPHA]) {
			glEnable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Invisible);

		}
		else
		{
			glDisable(GL_CULL_FACE);
			glDisable(GL_BLEND);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, AColor);
		}
		glTranslatef(Sbuffer.A[A].x, Sbuffer.A[A].y, Sbuffer.A[A].z);
		if (Sbuffer.sound[FIRST_PONG])
		{
			glScalef(1.2, 0.8, 1.2);
		}
		glScalef(1, 1.15, 1);
		glutSolidSphere(4, 80, 80);
		glPopMatrix();
	}
	/************************************************************************B*/
	if (Sbuffer.A[B].state)
	{
		glPushMatrix();
		if (Sbuffer.effect[SECOND_NO_ALPHA]) {
			glEnable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Invisible);

		}
		else
		{
			glDisable(GL_CULL_FACE);
			glDisable(GL_BLEND);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, BColor);
		}
		glTranslatef(Sbuffer.A[B].x, Sbuffer.A[B].y, Sbuffer.A[B].z);
		if (Sbuffer.sound[SECOND_PONG]) {
			glScalef(1.2, 0.8, 1.2);
		}
		glScalef(1, 1.15, 1);
		glutSolidSphere(4, 80, 80);
		glPopMatrix();
	}
	/***********************************************************************C*/
	if (Sbuffer.A[C].state)
	{
		glPushMatrix();
		if (Sbuffer.effect[THIRD_NO_ALPHA]) {
			glEnable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Invisible);

		}
		else
		{
			glDisable(GL_CULL_FACE);
			glDisable(GL_BLEND);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, CColor);
		}
		glTranslatef(Sbuffer.A[C].x, Sbuffer.A[C].y, Sbuffer.A[C].z);
		if (Sbuffer.sound[THIRD_PONG]) {
			glScalef(1.2, 0.8, 1.2);
		}
		glScalef(1, 1.15, 1);
		glutSolidSphere(4, 80, 80);
		glPopMatrix();
	}
}

void GameObject::Draw_Inform(int num)
{

	GLfloat White[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, White);
	if (Sbuffer.effect[STOP]) { t->SetBind(4);}
	else if ( Sbuffer.effect[FIRST_NO_DIE] || Sbuffer.effect[SECOND_NO_DIE] || Sbuffer.effect[THIRD_NO_DIE]) { t->SetBind(6); }
	else if (Sbuffer.effect[FIRST_SLOW] || Sbuffer.effect[SECOND_SLOW] || Sbuffer.effect[THIRD_SLOW]) { t->SetBind(5);}
	else if (Sbuffer.effect[FIRST_NO_ALPHA] || Sbuffer.effect[SECOND_NO_ALPHA] || Sbuffer.effect[THIRD_NO_ALPHA]) { t->SetBind(7); }
	else if (Sbuffer.effect[FOG]) { t->SetBind(8); }
	else { return;}


	glPushMatrix();
	glTranslatef(Sbuffer.A[num].x - 75, 120, Sbuffer.A[num].z + 12);
	glRotatef(-55, 1, 0, 0);
	glBegin(GL_POLYGON);
	glTexCoord2f(0, 0);
	glVertex3f(5, -25, -2);
	glTexCoord2f(1, 0);
	glVertex3f(50, -25, -2);
	glTexCoord2f(1, 1);
	glVertex3f(50, 20, -2);
	glTexCoord2f(0, 1);
	glVertex3f(5, 20, -2);
	glEnd();
	glPopMatrix();

}


void GameObject::SoundPlay()
{
	if (Sbuffer.sound[FIRST_PONG] || Sbuffer.sound[SECOND_PONG] || Sbuffer.sound[THIRD_PONG])
	{
		s->Play_GameSound(1);
		Sbuffer.sound[FIRST_PONG] = false;
		Sbuffer.sound[SECOND_PONG] = false;
		Sbuffer.sound[THIRD_PONG] = false;
	}
	if (Sbuffer.sound[FIRST_GET_ITEM] || Sbuffer.sound[SECOND_GET_ITEM] || Sbuffer.sound[THIRD_GET_ITEM])
	{
		s->Play_GameSound(2);
		Sbuffer.sound[FIRST_GET_ITEM] = false;
		Sbuffer.sound[SECOND_GET_ITEM] = false;
		Sbuffer.sound[THIRD_GET_ITEM] = false;
	}

	
}

//BOARD* GameObject::GetBOARD(double x, double z)
//{
//	for (int i = 0; i < b.size(); i++)
//	{
//		if ((b[i].x[0] <= x && b[i].x[1] >= x) && (b[i].z[0] <= z && b[i].z[1] >= z))
//		{
//			return &b[i];
//		}
//	}
//}



//

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Particle::Particle()
{
	Init();
}

Particle::~Particle()
{
}

void Particle::Init()
{
	int x, y, z;
	for (int i = 0; i < PARTICLENUM; i++) {
		x = rand() % 10 - 5;
		y = rand() % 10 - 5;
		z = rand() % 10 - 5;

		p[i].x = x / sqrt(x*x + y*y + z*z);
		p[i].y = y / sqrt(x*x + y*y + z*z);
		p[i].z = z / sqrt(x*x + y*y + z*z);
	}
}

void Particle::Draw()
{
	GLfloat Green[] = { 0.0f, 0.3f, 0.0f, 1.0f };
	for (int i = 0; i < PARTICLENUM; i++)
	{
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, Green);
		glTranslatef(p[i].x, p[i].y, p[i].z);
		p[i].x += p[i].x / 10;
		p[i].y += p[i].y / 10;
		p[i].z += p[i].z / 10;
		glutSolidCube(1);
		glPopMatrix();
	}
}

void Particle::Update()
{

}


//BOARD* Etc::GetBOARD(double x, double z)
//{
//	for (int i = 0; i < b.size(); i++)
//	{
//		if ((b[i].x[0] <= x && b[i].x[1] >= x) && (b[i].z[0] <= z && b[i].z[1] >= z))
//		{
//			return &b[i];
//		}
//	}
//	return NULL;
//}

//-----------------------------------------------------------------sound
GameSound::GameSound()
{
	System_Create(&pSystem);
	pSystem->init(6, FMOD_INIT_NORMAL, NULL);
	pSystem->createSound("SOUND\\BGM.mp3",
		FMOD_LOOP_NORMAL | FMOD_HARDWARE, NULL, &pSound[0]);
	pSystem->createSound("SOUND\\drip.wav",
		FMOD_LOOP_OFF | FMOD_HARDWARE, NULL, &pSound[1]);
	pSystem->createSound("SOUND\\tring.wav",
		FMOD_LOOP_OFF | FMOD_HARDWARE, NULL, &pSound[2]);
	pSystem->createSound("SOUND\\TADA.wav",
		FMOD_LOOP_OFF | FMOD_HARDWARE, NULL, &pSound[3]);
	pSystem->createSound("SOUND\\Click.wav",
		FMOD_LOOP_OFF | FMOD_HARDWARE, NULL, &pSound[4]);
	pSystem->createSound("SOUND\\explosion6.wav",
		FMOD_LOOP_OFF | FMOD_HARDWARE, NULL, &pSound[5]);
}


GameSound::~GameSound()
{
}


void GameSound::Play_GameSound(int i)
{
	pSystem->playSound(FMOD_CHANNEL_REUSE, pSound[i], false, &pChannel[i]);
}

