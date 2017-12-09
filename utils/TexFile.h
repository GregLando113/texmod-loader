#pragma once

class TexFile
{
public:

	struct Texture
	{
		unsigned long long hash;
		char* filename;
		void* base;
		unsigned int size;
	};

	// valid types are .zip, .tpf
	void LoadFromFile(char* fileName);

	Texture& operator[](int idx);

	void Release();

	unsigned int error() { return m_lastError; }
	unsigned int count() { return m_texturecount; }
private:

	char           m_fileName[0x100];
	unsigned char* m_base;
	unsigned int   m_size;
	unsigned int   m_lastError;

	char*          m_defbase;

	Texture*       m_textures;
	unsigned int   m_texturecount;
};