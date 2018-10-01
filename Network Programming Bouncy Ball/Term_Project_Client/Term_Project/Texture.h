#pragma once

class Texture
{
private:
	GLubyte *pBytes;
	BITMAPINFO *info;
	GLuint textures[16];

public:
	Texture();
	~Texture();
	GLubyte* LoadDIBitmap(const char *filename, BITMAPINFO **info);
	void  InitTexture();
	void SetBind(int n);
};

