#include "MediaFile.h"

MediaFile::MediaFile()
	:m_file(NULL), m_type(-1)
{}

MediaFile::~MediaFile()
{
	Close();
}

int MediaFile::Open(const EBuffer& path, int nType)
{
	m_file = fopen(path, "rb");
	if (m_file == NULL) {
		//TODO:错误提示
		return -1;
	}
	m_type = nType;
	fseek(m_file, 0, SEEK_END);
	m_size = ftell(m_file);
	Reset();
	return 0;
}

EBuffer MediaFile::ReadOneFrame()
{
	switch (m_type) {
	case 96:
		return ReadH264Frame();
	}
	return EBuffer();
}

void MediaFile::Close()
{
	m_type = -1;
	if (m_file != NULL) {
		FILE* file = m_file;
		m_file = NULL;
		fclose(file);
	}
}

void MediaFile::Reset()
{
	if (m_file) {
		fseek(m_file, 0, SEEK_SET);
	}
}

long MediaFile::FindH264Head(int& headsize)
{
	while (!feof(m_file)) {
		char c = 0x7F;
		while (!feof(m_file)) {//feof = file end of file 
			c = fgetc(m_file);
			if (c == 0)break;
		}
		if (!feof(m_file)) {
			c = fgetc(m_file);
			if (c == 0) {
				c = fgetc(m_file);
				if (c == 1) {//找到了一个头
					headsize = 3;
					return ftell(m_file) - 3;
				}
				else if (c == 0) {
					c = fgetc(m_file);
					if (c == 1) {//找到了头部
						headsize = 4;
						return ftell(m_file) - 4;
					}
				}
			}
		}
	}
	return -1;
}

EBuffer MediaFile::ReadH264Frame()
{
	if (m_file) {
		int headsize = 0;
		long off = FindH264Head(headsize);
		if (off == -1)return EBuffer();
		fseek(m_file, off + headsize, SEEK_SET);
		long tail = FindH264Head(headsize);
		if (tail == -1) tail = m_size;
		long size = tail - off;
		fseek(m_file, off, SEEK_SET);
		EBuffer result(size);
		fread(result, 1, size, m_file);
		return result;
	}
	return EBuffer();
}
