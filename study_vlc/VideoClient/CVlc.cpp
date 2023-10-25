#include "pch.h"
#include "CVlc.h"

CVlc::CVlc()
{
	m_vlcInstance = libvlc_new(0, NULL);
	m_vlcMediaPlayer = NULL;
	m_vlcMedia = NULL;
	m_hwnd = NULL;
}

CVlc::~CVlc()
{
	if (m_vlcMediaPlayer)
	{
		libvlc_media_player_stop(m_vlcMediaPlayer);
		libvlc_media_player_release(m_vlcMediaPlayer);
		m_vlcMediaPlayer = NULL;
	}

	if (m_vlcMedia)
	{
		libvlc_media_release(m_vlcMedia);
		m_vlcMedia = NULL;
	}

	if (m_vlcInstance)
	{
		libvlc_release(m_vlcInstance);
		m_vlcInstance = NULL;
	}
}

int CVlc::SetMedia(const std::string& URL)
{

	if (m_vlcInstance == NULL || m_hwnd == NULL) return -1;
	if(URL == m_url) return 0;
	m_url = URL;

	if (m_vlcMedia)
	{
		libvlc_media_release(m_vlcMedia);
		m_vlcMedia = NULL;
	}

	if (m_vlcMediaPlayer)
	{
		libvlc_media_player_release(m_vlcMediaPlayer);
		m_vlcMediaPlayer = NULL;
	}

	m_vlcMedia = libvlc_media_new_location(m_vlcInstance, URL.c_str());
	if (!m_vlcMedia) return -2;

	m_vlcMediaPlayer = libvlc_media_player_new_from_media(m_vlcMedia);
	
	if(!m_vlcMediaPlayer) return -3;
	libvlc_media_player_set_hwnd(m_vlcMediaPlayer, m_hwnd);

	return 0;
}

int CVlc::Play()
{
	if (!m_vlcInstance || !m_vlcMedia || !m_vlcMediaPlayer) return -1;
	return libvlc_media_player_play(m_vlcMediaPlayer);
}

int CVlc::Pause()
{
	if (!m_vlcInstance || !m_vlcMedia || !m_vlcMediaPlayer) return -1;
	libvlc_media_player_pause(m_vlcMediaPlayer);
	return 0;
}

int CVlc::Stop()
{
	if (!m_vlcInstance || !m_vlcMedia || !m_vlcMediaPlayer) return -1;
	libvlc_media_player_stop(m_vlcMediaPlayer);

	return 0;
}

int CVlc::GetVolume()
{
	if (!m_vlcInstance || !m_vlcMedia || !m_vlcMediaPlayer) return -1;
	return libvlc_audio_get_volume(m_vlcMediaPlayer);
	return 0;
}

int CVlc::SetVolume(int volume)
{
	if (!m_vlcInstance || !m_vlcMedia || !m_vlcMediaPlayer) return -1;
	return libvlc_audio_set_volume(m_vlcMediaPlayer, volume);
	return 0;
}

float CVlc::GetPosition()
{
	if (!m_vlcInstance || !m_vlcMedia || !m_vlcMediaPlayer) return -1;
	return libvlc_media_player_get_position(m_vlcMediaPlayer);
	return 0.0f;
}

int CVlc::SetPosition(float pos)
{
	if (!m_vlcInstance || !m_vlcMedia || !m_vlcMediaPlayer) return -1;
	libvlc_media_player_set_position(m_vlcMediaPlayer, pos);
	return 0;
}

VlcSize CVlc::GetVideoSize()
{
	if (!m_vlcInstance || !m_vlcMedia || !m_vlcMediaPlayer) return VlcSize();

	return VlcSize(libvlc_video_get_width(m_vlcMediaPlayer), libvlc_video_get_height(m_vlcMediaPlayer));
}

#ifdef WIN32
int CVlc::SetHwnd(void* hwnd)
{
	m_hwnd = (HWND)hwnd;
	return 0;
}
#endif // WIN32


