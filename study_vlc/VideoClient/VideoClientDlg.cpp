
// VideoClientDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "VideoClient.h"
#include "VideoClientDlg.h"
#include "afxdialogex.h"
#include "VideoClientController.h"		

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVideoClientDlg 对话框



CVideoClientDlg::CVideoClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VIDEOCLIENT_DIALOG, pParent), m_status(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVideoClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PLAY, m_video);
	DDX_Control(pDX, IDC_SLIDER_POS, m_pos);
	DDX_Control(pDX, IDC_SLIDER_VOLUME, m_volume);
	DDX_Control(pDX, IDC_EDIT_URl, m_url);
	DDX_Control(pDX, IDC_BUTTON_PLAY, m_btmPlay);
}

BEGIN_MESSAGE_MAP(CVideoClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CVideoClientDlg::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CVideoClientDlg::OnBnClickedButtonStop)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_POS, &CVideoClientDlg::OnTRBNThumbPosChangingSliderPos)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_VOLUME, &CVideoClientDlg::OnTRBNThumbPosChangingSliderVolume)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()


// CVideoClientDlg 消息处理程序

BOOL CVideoClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	SetTimer(0, 500, NULL);
	//进度条音量键设置为(0, 100)范围
	m_volume.SetRange(0, 100);
	m_pos.SetRange(0, 100);
	m_pos.SetPos(0);
	// Set the play address
	CString url = _T("file:///D:\\edoyun\\study_vlc\\study_vlc\\VideoClient\\123.mp4");
	m_url.SetWindowTextW(url);

	
	
	//音量条每隔10触发一次mark
	m_volume.SetTicFreq(10);
	//将IDC_STATIC_VOLUME设置为100%
	SetDlgItemText(IDC_STATIC_VOLUME, _T("100%"));
	//将IDC_STATIC_TIME设置为0%
	SetDlgItemText(IDC_STATIC_TIME, _T("0%"));
	//设置video为窗口
	m_pController->SetWnd(m_video.GetSafeHwnd());
	//视频铺满整个窗口
	m_video.ModifyStyle(0, WS_MAXIMIZEBOX | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	m_video.SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
	m_video.ShowWindow(SW_SHOW);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CVideoClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		// 使图标在工作区矩形中左上
		/*int x = 0;
		int y = 0;*/
		// // 使图标在工作区矩形中右上
		// int x = rect.Width() - cxIcon;
		// int y = 0;
		// // 使图标在工作区矩形中左下
		// int x = 0;
		// int y = rect.Height() - cyIcon;
		// // 使图标在工作区矩形中右下
		// int x = rect.Width() - cxIcon;
		// int y = rect.Height() - cyIcon;
		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CVideoClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CVideoClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 0)
	{
		float pos = m_pController->VideoCtrl(VLC_GET_POSITION);
		//将进度条的值设置到IDC_STATIC_TIME
		if (pos != -1)
		{
			CString str;
			str.Format(_T("%d%%"), (int)(pos * 100));
			SetDlgItemText(IDC_STATIC_TIME, str);
			m_pos.SetPos((int)(pos * 100));
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CVideoClientDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	KillTimer(0);
}


void CVideoClientDlg::OnBnClickedButtonPlay()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_status == false)
	{
		//播放视频
		CString url;
		m_url.GetWindowTextW(url);
		//url转为string类型
		std::string urlStr = CW2A(url.GetString());
		m_pController->SetMedia(urlStr);

		m_btmPlay.SetWindowTextW(L"暂停");
		m_status = true;
		m_pController->VideoCtrl(VLCPLAY);
	}
	else
	{
		//暂停视频
		m_btmPlay.SetWindowTextW(L"播放");
		m_status = false;
		m_pController->VideoCtrl(VLCPAUSE);
	}
}

void CVideoClientDlg::OnBnClickedButtonStop()
{
	// TODO: 在此添加控件通知处理程序代码
	m_btmPlay.SetWindowTextW(L"播放");
	m_status = false;
	m_pController->VideoCtrl(VLCSTOP);
}


void CVideoClientDlg::OnTRBNThumbPosChangingSliderPos(NMHDR* pNMHDR, LRESULT* pResult)
{
	// 此功能要求 Windows Vista 或更高版本。
	// _WIN32_WINNT 符号必须 >= 0x0600。
	NMTRBTHUMBPOSCHANGING* pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING*>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	TRACE("pos %d reason %d\r\n", pNMTPC->dwPos, pNMTPC->nReason);
	*pResult = 0;
}


void CVideoClientDlg::OnTRBNThumbPosChangingSliderVolume(NMHDR* pNMHDR, LRESULT* pResult)
{
	// 此功能要求 Windows Vista 或更高版本。
	// _WIN32_WINNT 符号必须 >= 0x0600。
	NMTRBTHUMBPOSCHANGING* pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING*>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	TRACE("pos %d reason %d\r\n", pNMTPC->dwPos, pNMTPC->nReason); 
	*pResult = 0;
}


void CVideoClientDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//TRACE("nSBCode %d nPos %d\r\n", nSBCode, nPos);
	//获取进度条的值
	int pos = m_pos.GetPos();
	//将进度条的值设置到IDC_STATIC_TIME
	CString str;
	str.Format(_T("%d%%"), pos);
	SetDlgItemText(IDC_STATIC_TIME, str);
	m_pController->SetPosition((float)pos / 100);

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CVideoClientDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	//获取音量条的值
	int volume = m_volume.GetPos();
	//将音量条的值设置到IDC_STATIC_VOLUME
	CString str;
	str.Format(_T("%d%%"), 100 - volume);
	SetDlgItemText(IDC_STATIC_VOLUME, str);
	m_pController->SetVolume(100 - volume);
	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}
