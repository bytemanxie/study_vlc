#include "pch.h"
#include "VideoClientController.h"

VideoClientController::VideoClientController()
{
	m_videoClientDlg.m_pController = this;
}

VideoClientController::~VideoClientController()
{
}

int VideoClientController::Init(CWnd*& pWnd)
{
	pWnd = &m_videoClientDlg;
	return 0;
}

int VideoClientController::Invoke()
{
	INT_PTR nResponse = m_videoClientDlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "����: �Ի��򴴽�ʧ�ܣ�Ӧ�ó���������ֹ��\n");
		TRACE(traceAppMsg, 0, "����: ������ڶԻ�����ʹ�� MFC �ؼ������޷� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS��\n");
	}
	return nResponse;
}

int VideoClientController::SetMedia(const std::string& url)
{
	m_vlc.SetMedia(url);
	return 0;
}

float VideoClientController::VideoCtrl(VLcCommand cmd)
{
	switch (cmd)
	{
	case VLCPLAY:
		return float(m_vlc.Play());
		break;
	case VLCSTOP:
		return float(m_vlc.Stop());
		break;
	case VLCPAUSE:
		return float(m_vlc.Pause());
		break;
	case VLC_GET_VOLUME:
		return float(m_vlc.GetVolume());
		break;
	case VLC_GET_POSITION:
		return float(m_vlc.GetPosition());
		break;
	default:
		break;
	}
	return -1.0f;
}

int VideoClientController::SetVolume(int volume)
{
	return m_vlc.SetVolume(volume);
	return 0;
}

int VideoClientController::SetPosition(float pos)
{
	return m_vlc.SetPosition(pos); 
	return 0;
}

int VideoClientController::SetWnd(HWND pWnd)
{
	return m_vlc.SetHwnd(pWnd);
	return 0;
}

VlcSize VideoClientController::GetMediaInfo()
{
	return m_vlc.GetVideoSize();
	return VlcSize();
}
 