#pragma once
#include "vlc.h"
#include <string>
class VlcSize
{
public:
	int width;
	int height;
	VlcSize(int w = 0, int h = 0) :width(w), height(h) {}
	~VlcSize() {}

	VlcSize(const VlcSize& other)
	{
		width = other.width;
		height = other.height;
	}

	VlcSize& operator=(const VlcSize& other)
	{
		if (this != &other)
		{
			width = other.width;
			height = other.height;
		}
		
		return *this;
	}
};

class CVlc
{
public:
	CVlc();
	~CVlc();

	//若URL含有中文，传入时使用utf-8编码
	int SetMedia(const std::string& URL);
	int Play();
	int Pause();
	int Stop();
	int GetVolume();
	int SetVolume(int volume);
	float GetPosition();
	int SetPosition(float pos);
	VlcSize GetVideoSize();
	int SetHwnd(void* hwnd);

protected:
	libvlc_instance_t* m_vlcInstance;
	libvlc_media_player_t* m_vlcMediaPlayer;
	libvlc_media_t* m_vlcMedia;
};

