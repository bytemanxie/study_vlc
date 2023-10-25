#pragma once
#include "VideoClientDlg.h"
#include "CVlc.h"

enum VLcCommand
{
	VLCPLAY,
	VLCSTOP,
	VLCPAUSE,
	VLC_GET_VOLUME,
	VLC_GET_POSITION
};

class VideoClientController
{
protected:
	CVideoClientDlg m_videoClientDlg;
	CVlc m_vlc;
public:
	VideoClientController();
	~VideoClientController();

	int Init(CWnd*& pWnd);

	int Invoke();
	int SetMedia(const std::string& url);

	float VideoCtrl(VLcCommand cmd);
	int SetVolume(int volume);
	int SetPosition(float pos);
	int SetWnd(HWND pWnd);
	VlcSize GetMediaInfo();
};

