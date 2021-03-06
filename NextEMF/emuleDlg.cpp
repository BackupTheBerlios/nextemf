//this file is part of eMule
//Copyright (C)2002 Merkur ( devs@emule-project.net / http://www.emule-project.net )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#include "stdafx.h"
#include <math.h>
#include <afxinet.h>
#define MMNODRV			// mmsystem: Installable driver support
//#define MMNOSOUND		// mmsystem: Sound support
#define MMNOWAVE		// mmsystem: Waveform support
#define MMNOMIDI		// mmsystem: MIDI support
#define MMNOAUX			// mmsystem: Auxiliary audio support
#define MMNOMIXER		// mmsystem: Mixer support
#define MMNOTIMER		// mmsystem: Timer support
#define MMNOJOY			// mmsystem: Joystick support
#define MMNOMCI			// mmsystem: MCI support
#define MMNOMMIO		// mmsystem: Multimedia file I/O support
#define MMNOMMSYSTEM	// mmsystem: General MMSYSTEM functions
#include <Mmsystem.h>
#include <HtmlHelp.h>
#include <share.h>
#include "emule.h"
#include "emuleDlg.h"
#include "ServerWnd.h"
#include "KademliaWnd.h"
#include "TransferWnd.h"
#include "SearchResultsWnd.h"
#include "SearchDlg.h"
#include "SharedFilesWnd.h"
#include "ChatWnd.h"
#include "StatisticsDlg.h"
#include "CreditsDlg.h"
#include "PreferencesDlg.h"
#include "Sockets.h"
#include "KnownFileList.h"
#include "ServerList.h"
#include "Opcodes.h"
#include "SharedFileList.h"
#include "ED2KLink.h"
//==> SplashScreen [Xman]
#include "SplashScreenEx.h"
//<== SplashScreen [Xman]
#include "PartFileConvert.h"
#include "EnBitmap.h"
#include "Wizard.h"
#include "Exceptions.h"
#include "SearchList.h"
#include "HTRichEditCtrl.h"
#include "FrameGrabThread.h"
#include "kademlia/kademlia/kademlia.h"
#include "kademlia/kademlia/SearchManager.h"
#include "kademlia/routing/RoutingZone.h"
#include "kademlia/routing/contact.h"
#include "kademlia/kademlia/prefs.h"
#include "KadSearchListCtrl.h"
#include "KadContactListCtrl.h"
#include "PerfLog.h"
#include "DropTarget.h"
#include "LastCommonRouteFinder.h"
#include "WebServer.h"
#include "DownloadQueue.h"
#include "ClientUDPSocket.h"
#include "UploadQueue.h"
#include "ClientList.h"
#include "UploadBandwidthThrottler.h"
#include "FriendList.h"
#include "IPFilter.h"
#include "Statistics.h"
//==> Toolbar [shadow2004]
//#include "MuleToolbarCtrl.h"
//<== Toolbar [shadow2004]
#include "TaskbarNotifier.h"
#include "MuleStatusbarCtrl.h"
#include "ListenSocket.h"
#include "Server.h"
#include "PartFile.h"
#include "Scheduler.h"
#include "ClientCredits.h"
#include "MenuCmds.h"
#include "MuleSystrayDlg.h"
#include "IPFilterDlg.h"
#include "DirectDownloadDlg.h"
#include "PeerCacheFinder.h"
#include "Statistics.h"
#include "FirewallOpener.h"
#include "StringConversion.h"
#include "aichsyncthread.h"
#include "Log.h"
#include "MiniMule.h"
#include "UserMsgs.h"
#include "Collection.h"
#include "CollectionViewDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define	SYS_TRAY_ICON_COOKIE_FORCE_UPDATE	(UINT)-1

BOOL (WINAPI *_TransparentBlt)(HDC, int, int, int, int, HDC, int, int, int, int, UINT) = NULL;
const static UINT UWM_ARE_YOU_EMULE = RegisterWindowMessage(EMULE_GUID);
UINT _uMainThreadId = 0;


///////////////////////////////////////////////////////////////////////////
// CemuleDlg Dialog

IMPLEMENT_DYNAMIC(CMsgBoxException, CException)

BEGIN_MESSAGE_MAP(CemuleDlg, CTrayDialog)
	///////////////////////////////////////////////////////////////////////////
	// Windows messages
	//
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ENDSESSION()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_MENUCHAR()
	ON_WM_QUERYENDSESSION()
	ON_WM_SYSCOLORCHANGE()
	ON_MESSAGE(WM_COPYDATA, OnWMData)
	ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
	ON_WM_SHOWWINDOW()
	ON_WM_DESTROY()
	ON_WM_SETTINGCHANGE()

	///////////////////////////////////////////////////////////////////////////
	// WM_COMMAND messages
	//
	ON_COMMAND(MP_CONNECT, StartConnection)
	ON_COMMAND(MP_DISCONNECT, CloseConnection)
	ON_COMMAND(MP_EXIT, OnClose)
	ON_COMMAND(MP_RESTORE, RestoreWindow)
	// quick-speed changer -- 
	ON_COMMAND_RANGE(MP_QS_U10, MP_QS_UP10, QuickSpeedUpload)
	ON_COMMAND_RANGE(MP_QS_D10, MP_QS_DC, QuickSpeedDownload)
	//--- quickspeed - paralize all ---
	ON_COMMAND_RANGE(MP_QS_PA, MP_QS_UA, QuickSpeedOther)
	// quick-speed changer -- based on xrmb	
	ON_NOTIFY_EX_RANGE(RBN_CHEVRONPUSHED, 0, 0xFFFF, OnChevronPushed)

	ON_REGISTERED_MESSAGE(UWM_ARE_YOU_EMULE, OnAreYouEmule)
	ON_BN_CLICKED(IDC_HOTMENU, OnBnClickedHotmenu)

	///////////////////////////////////////////////////////////////////////////
	// WM_USER messages
	//
	ON_MESSAGE(UM_TASKBARNOTIFIERCLICKED, OnTaskbarNotifierClicked)
	ON_MESSAGE(UM_CLOSE_MINIMULE, OnCloseMiniMule)
	
	// Webserver messages
	ON_MESSAGE(WEB_GUI_INTERACTION, OnWebGUIInteraction)
	ON_MESSAGE(WEB_CLEAR_COMPLETED, OnWebServerClearCompleted)
	ON_MESSAGE(WEB_FILE_RENAME, OnWebServerFileRename)
	ON_MESSAGE(WEB_ADDDOWNLOADS, OnWebAddDownloads)
	ON_MESSAGE(WEB_CATPRIO, OnWebSetCatPrio)
	ON_MESSAGE(WEB_ADDREMOVEFRIEND, OnAddRemoveFriend)

	// Version Check DNS
	ON_MESSAGE(UM_VERSIONCHECK_RESPONSE, OnVersionCheckResponse)

	// PeerCache DNS
	ON_MESSAGE(UM_PEERCHACHE_RESPONSE, OnPeerCacheResponse)

	///////////////////////////////////////////////////////////////////////////
	// WM_APP messages
	//
	ON_MESSAGE(TM_FINISHEDHASHING, OnFileHashed)
	ON_MESSAGE(TM_FILEOPPROGRESS, OnFileOpProgress)
	ON_MESSAGE(TM_HASHFAILED, OnHashFailed)
	ON_MESSAGE(TM_FRAMEGRABFINISHED, OnFrameGrabFinished)
	ON_MESSAGE(TM_FILEALLOCEXC, OnFileAllocExc)
	ON_MESSAGE(TM_FILECOMPLETED, OnFileCompleted)
//==> Toolbar [shadow2004]
	ON_WM_ERASEBKGND()

	ON_BN_CLICKED(IDC_BTN_CONNECT, OnBnClickedBtnConnect)
	ON_BN_CLICKED(IDC_BTN_KADEMLIA, OnBnClickedBtnKademlia)
	ON_BN_CLICKED(IDC_BTN_SERVER, OnBnClickedBtnServer)
	ON_BN_CLICKED(IDC_BTN_TRANSFER, OnBnClickedBtnTransfer)
	ON_BN_CLICKED(IDC_BTN_SEARCH, OnBnClickedBtnSearch)
	ON_BN_CLICKED(IDC_BTN_FILES, OnBnClickedBtnShared)
	ON_BN_CLICKED(IDC_BTN_MESSAGES, OnBnClickedBtnMessage)
	ON_BN_CLICKED(IDC_BTN_STATISTIC, OnBnClickedBtnStatistic)
	ON_BN_CLICKED(IDC_BTN_PREFERENCES, OnBnClickedBtnPreferences)
	ON_BN_CLICKED(IDC_BTN_TOOLS, OnBnClickedBtnTools)

//<== Toolbar [shadow2004]

END_MESSAGE_MAP()

CemuleDlg::CemuleDlg(CWnd* pParent /*=NULL*/)
	: CTrayDialog(CemuleDlg::IDD, pParent)
{
	_uMainThreadId = GetCurrentThreadId();
	preferenceswnd = new CPreferencesDlg;
	serverwnd = new CServerWnd;
	kademliawnd = new CKademliaWnd;
	transferwnd = new CTransferWnd;
	sharedfileswnd = new CSharedFilesWnd;
	searchwnd = new CSearchDlg;
	chatwnd = new CChatWnd;
	statisticswnd = new CStatisticsDlg;
//==> Toolbar [shadow2004]
//	toolbar = new CMuleToolbarCtrl;
//<== Toolbar [shadow2004]
	statusbar = new CMuleStatusBarCtrl;
	m_wndTaskbarNotifier = new CTaskbarNotifier;

	m_hIcon = NULL;
	theApp.m_app_state = APP_STATE_RUNNING;
	ready = false; 
	m_bStartMinimizedChecked = false;
	m_bStartMinimized = false;
//==> Optimizer [shadow2004]
#ifdef OPTIM
	memzero(&m_wpFirstRestore, sizeof m_wpFirstRestore);
#else
	memset(&m_wpFirstRestore, 0, sizeof m_wpFirstRestore);
#endif
//<== Optimizer [shadow2004]
	m_uUpDatarate = 0;
	m_uDownDatarate = 0;
	status = 0;
	activewnd = NULL;
	for (int i = 0; i < ARRSIZE(connicons); i++)
		connicons[i] = NULL;
	transicons[0] = NULL;
	transicons[1] = NULL;
	transicons[2] = NULL;
	transicons[3] = NULL;
	imicons[0] = NULL;
	imicons[1] = NULL;
	imicons[2] = NULL;
	m_iMsgIcon = 0;
	m_icoSysTrayConnected = NULL;
	m_icoSysTrayDisconnected = NULL;
	m_icoSysTrayLowID = NULL;
	usericon = NULL;
	m_icoSysTrayCurrent = NULL;
	m_hTimer = 0;
	notifierenabled = false;
	m_pDropTarget = new CMainFrameDropTarget;
	m_pSplashWnd = NULL;
	m_dwSplashTime = (DWORD)-1;
	m_pSystrayDlg = NULL;
	m_pMiniMule = NULL;
	m_uLastSysTrayIconCookie = SYS_TRAY_ICON_COOKIE_FORCE_UPDATE;
}

CemuleDlg::~CemuleDlg()
{
	DestroyMiniMule();
	if (m_icoSysTrayCurrent) VERIFY( DestroyIcon(m_icoSysTrayCurrent) );
	if (m_hIcon) VERIFY( ::DestroyIcon(m_hIcon) );
	for (int i = 0; i < ARRSIZE(connicons); i++){
		if (connicons[i]) VERIFY( ::DestroyIcon(connicons[i]) );
	}
	if (transicons[0]) VERIFY( ::DestroyIcon(transicons[0]) );
	if (transicons[1]) VERIFY( ::DestroyIcon(transicons[1]) );
	if (transicons[2]) VERIFY( ::DestroyIcon(transicons[2]) );
	if (transicons[3]) VERIFY( ::DestroyIcon(transicons[3]) );
	if (imicons[0]) VERIFY( ::DestroyIcon(imicons[0]) );
	if (imicons[1]) VERIFY( ::DestroyIcon(imicons[1]) );
	if (imicons[2]) VERIFY( ::DestroyIcon(imicons[2]) );
	if (m_icoSysTrayConnected) VERIFY( ::DestroyIcon(m_icoSysTrayConnected) );
	if (m_icoSysTrayDisconnected) VERIFY( ::DestroyIcon(m_icoSysTrayDisconnected) );
	if (m_icoSysTrayLowID) VERIFY( ::DestroyIcon(m_icoSysTrayLowID) );
	if (usericon) VERIFY( ::DestroyIcon(usericon) );

	// already destroyed by windows?
	//VERIFY( m_menuUploadCtrl.DestroyMenu() );
	//VERIFY( m_menuDownloadCtrl.DestroyMenu() );
	//VERIFY( m_SysMenuOptions.DestroyMenu() );

	delete preferenceswnd;
	delete serverwnd;
	delete kademliawnd;
	delete transferwnd;
	delete sharedfileswnd;
	delete chatwnd;
	delete statisticswnd;
//==> Toolbar [shadow2004]
//	delete toolbar;
//<== Toolbar [shadow2004]
	delete statusbar;
	delete m_wndTaskbarNotifier;
	delete m_pDropTarget;
}

void CemuleDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrayDialog::DoDataExchange(pDX);
//==> Toolbar [shadow2004]
	DDX_Control(pDX, IDC_BTN_CONNECT, m_co_ConnectBtn);
	DDX_Control(pDX, IDC_BTN_KADEMLIA, m_co_KademliaBtn);
	DDX_Control(pDX, IDC_BTN_TRANSFER, m_co_TransferBtn);
	DDX_Control(pDX, IDC_BTN_SERVER, m_co_ServerBtn);
	DDX_Control(pDX, IDC_BTN_SEARCH, m_co_SearchBtn);
	DDX_Control(pDX, IDC_BTN_FILES, m_co_SharedBtn);
	DDX_Control(pDX, IDC_BTN_MESSAGES, m_co_MessagesBtn);
	DDX_Control(pDX, IDC_BTN_STATISTIC, m_co_StatisticBtn);
	DDX_Control(pDX, IDC_BTN_PREFERENCES, m_co_PreferencesBtn);
	DDX_Control(pDX, IDC_BTN_TOOLS, m_co_ToolsBtn);
//<== Toolbar [shadow2004]
}

LRESULT CemuleDlg::OnAreYouEmule(WPARAM, LPARAM)
{
	return UWM_ARE_YOU_EMULE;
} 

BOOL CemuleDlg::OnInitDialog()
{
//==> Toolbar [shadow2004]
	m_co_ToolLeft.LoadImage(IDR_TOOLBAR_LEFT,_T("JPG"));
	m_co_ToolMid.LoadImage(IDR_TOOLBAR_CENTER,_T("JPG"));
	m_co_ToolRight.LoadImage(IDR_TOOLBAR_RIGHT,_T("JPG"));
//<== Toolbar [shadow2004]
	m_bStartMinimized = thePrefs.GetStartMinimized();
	if (!m_bStartMinimized)
		m_bStartMinimized = theApp.DidWeAutoStart();

	// temporary disable the 'startup minimized' option, otherwise no window will be shown at all
	if (thePrefs.IsFirstStart())
		m_bStartMinimized = false;

	// show splashscreen as early as possible to "entertain" user while starting emule up
	if (thePrefs.UseSplashScreen() && !m_bStartMinimized)
		ShowSplash();

	// Create global GUI objects
	theApp.CreateAllFonts();
	theApp.CreateBackwardDiagonalBrush();

	CTrayDialog::OnInitDialog();
	InitWindowStyles(this);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL){
		pSysMenu->AppendMenu(MF_SEPARATOR);

		ASSERT( (MP_ABOUTBOX & 0xFFF0) == MP_ABOUTBOX && MP_ABOUTBOX < 0xF000);
		pSysMenu->AppendMenu(MF_STRING, MP_ABOUTBOX, GetResString(IDS_ABOUTBOX));

		ASSERT( (MP_VERSIONCHECK & 0xFFF0) == MP_VERSIONCHECK && MP_VERSIONCHECK < 0xF000);
		pSysMenu->AppendMenu(MF_STRING, MP_VERSIONCHECK, GetResString(IDS_VERSIONCHECK));

		// remaining system menu entries are created later...
	}

//==> Toolbar [shadow2004]
/*	CWnd* pwndToolbarX = toolbar;
	if (toolbar->Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, IDC_TOOLBAR))
	{
		toolbar->Init();
		if (thePrefs.GetUseReBarToolbar())
		{
		    if (m_ctlMainTopReBar.Create(WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
									     RBS_BANDBORDERS | RBS_AUTOSIZE | CCS_NODIVIDER, 
									     CRect(0, 0, 0, 0), this, AFX_IDW_REBAR))
		    {
			    CSize sizeBar;
			    VERIFY( toolbar->GetMaxSize(&sizeBar) );
			    REBARBANDINFO rbbi = {0};
			    rbbi.cbSize = sizeof(rbbi);
				rbbi.fMask = RBBIM_STYLE | RBBIM_SIZE | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_IDEALSIZE | RBBIM_ID;
			    rbbi.fStyle = RBBS_NOGRIPPER | RBBS_BREAK | RBBS_USECHEVRON;
			    rbbi.hwndChild = toolbar->m_hWnd;
			    rbbi.cxMinChild = sizeBar.cy;
			    rbbi.cyMinChild = sizeBar.cy;
			    rbbi.cxIdeal = sizeBar.cx;
			    rbbi.cx = rbbi.cxIdeal;
				rbbi.wID = 0;
			    VERIFY( m_ctlMainTopReBar.InsertBand((UINT)-1, &rbbi) );
				toolbar->SaveCurHeight();
    
			    pwndToolbarX = &m_ctlMainTopReBar;
		    }
		}
	}*/
//<== Toolbar [shadow2004]

	//set title
        SetWindowText(_T("eMule v") + theApp.m_strCurVersionLong
//==>Modversion [shadow2004]
#ifdef MODVERSION
        +_T(" [") + theApp.m_strModLongVersion + _T("]")
#endif //Modversion
        );
        
	// Init taskbar notifier
	m_wndTaskbarNotifier->Create(this);
	if (thePrefs.GetNotifierConfiguration().IsEmpty()) {
		CString defaultTBN;
		defaultTBN.Format(_T("%sNotifier.ini"), thePrefs.GetConfigDir());
		LoadNotifier(defaultTBN);
		thePrefs.SetNotifierConfiguration(defaultTBN);
	}
	else
		LoadNotifier(thePrefs.GetNotifierConfiguration());

	// set statusbar
	// the statusbar control is created as a custom control in the dialog resource,
	// this solves font and sizing problems when using large system fonts
	statusbar->SubclassWindow(GetDlgItem(IDC_STATUSBAR)->m_hWnd);
	statusbar->EnableToolTips(true);
	SetStatusBarPartsSize();

	// create main window dialog pages
	serverwnd->Create(IDD_SERVER);
	sharedfileswnd->Create(IDD_FILES);
	searchwnd->Create(this);
	chatwnd->Create(IDD_CHAT);
	transferwnd->Create(IDD_TRANSFER);
	statisticswnd->Create(IDD_STATISTICS);
	kademliawnd->Create(IDD_KADEMLIAWND);

	// optional: restore last used main window dialog
	if (thePrefs.GetRestoreLastMainWndDlg()){
		switch (thePrefs.GetLastMainWndDlgID()){
		case IDD_SERVER:
			{
//==> Toolbar [shadow2004]
				m_co_KademliaBtn.EnableWindow(TRUE);
				m_co_TransferBtn.EnableWindow(TRUE);
				m_co_SearchBtn.EnableWindow(TRUE);
				m_co_SharedBtn.EnableWindow(TRUE);
				m_co_MessagesBtn.EnableWindow(TRUE);
				m_co_StatisticBtn.EnableWindow(TRUE);
				m_co_PreferencesBtn.EnableWindow(TRUE);				

				m_co_ServerBtn.EnableWindow(FALSE);
//<== Toolbar [shadow2004]
			SetActiveDialog(serverwnd);
			break;
			}
		case IDD_FILES:
			{
//==> Toolbar [shadow2004]
				m_co_KademliaBtn.EnableWindow(TRUE);
				m_co_ServerBtn.EnableWindow(TRUE);
				m_co_TransferBtn.EnableWindow(TRUE);
				m_co_SearchBtn.EnableWindow(TRUE);
				m_co_MessagesBtn.EnableWindow(TRUE);
				m_co_StatisticBtn.EnableWindow(TRUE);
				m_co_PreferencesBtn.EnableWindow(TRUE);				

				m_co_SharedBtn.EnableWindow(FALSE);
//<== Toolbar [shadow2004]
			SetActiveDialog(sharedfileswnd);
			break;
			}
		case IDD_SEARCH:
			{
//==> Toolbar [shadow2004]
				m_co_KademliaBtn.EnableWindow(TRUE);
				m_co_ServerBtn.EnableWindow(TRUE);
				m_co_TransferBtn.EnableWindow(TRUE);
				m_co_SharedBtn.EnableWindow(TRUE);
				m_co_MessagesBtn.EnableWindow(TRUE);
				m_co_StatisticBtn.EnableWindow(TRUE);
				m_co_PreferencesBtn.EnableWindow(TRUE);				

				m_co_SearchBtn.EnableWindow(FALSE);
//<== Toolbar [shadow2004]
			SetActiveDialog(searchwnd);
			break;
			}
		case IDD_CHAT:
			{
//==> Toolbar [shadow2004]
				m_co_KademliaBtn.EnableWindow(TRUE);
				m_co_ServerBtn.EnableWindow(TRUE);
				m_co_TransferBtn.EnableWindow(TRUE);
				m_co_SearchBtn.EnableWindow(TRUE);
				m_co_SharedBtn.EnableWindow(TRUE);
				m_co_StatisticBtn.EnableWindow(TRUE);
				m_co_PreferencesBtn.EnableWindow(TRUE);				

				m_co_MessagesBtn.EnableWindow(FALSE);
//<== Toolbar [shadow2004]
			SetActiveDialog(chatwnd);
			break;
			}
		case IDD_TRANSFER:
			{
//==> Toolbar [shadow2004]
				m_co_KademliaBtn.EnableWindow(TRUE);
				m_co_ServerBtn.EnableWindow(TRUE);
				m_co_SearchBtn.EnableWindow(TRUE);
				m_co_SharedBtn.EnableWindow(TRUE);
				m_co_MessagesBtn.EnableWindow(TRUE);
				m_co_StatisticBtn.EnableWindow(TRUE);
				m_co_PreferencesBtn.EnableWindow(TRUE);				

				m_co_TransferBtn.EnableWindow(FALSE);
//<== Toolbar [shadow2004]
			SetActiveDialog(transferwnd);
			break;
			}
		case IDD_STATISTICS:
			{
//==> Toolbar [shadow2004]
				m_co_KademliaBtn.EnableWindow(TRUE);
				m_co_ServerBtn.EnableWindow(TRUE);
				m_co_TransferBtn.EnableWindow(TRUE);
				m_co_SearchBtn.EnableWindow(TRUE);
				m_co_SharedBtn.EnableWindow(TRUE);
				m_co_MessagesBtn.EnableWindow(TRUE);
				m_co_PreferencesBtn.EnableWindow(TRUE);				

				m_co_StatisticBtn.EnableWindow(FALSE);
//<== Toolbar [shadow2004]
			SetActiveDialog(statisticswnd);
			break;
			}
		case IDD_KADEMLIAWND:
			{
//==> Toolbar [shadow2004]
				m_co_ServerBtn.EnableWindow(TRUE);
				m_co_TransferBtn.EnableWindow(TRUE);
				m_co_SearchBtn.EnableWindow(TRUE);
				m_co_SharedBtn.EnableWindow(TRUE);
				m_co_MessagesBtn.EnableWindow(TRUE);
				m_co_StatisticBtn.EnableWindow(TRUE);
				m_co_PreferencesBtn.EnableWindow(TRUE);				

				m_co_KademliaBtn.EnableWindow(FALSE);
//<== Toolbar [shadow2004]
			SetActiveDialog(kademliawnd);
			break;
			}
		}
	}

	// if still no active window, activate server window
	if (activewnd == NULL)
			{
//==> Toolbar [shadow2004]
		m_co_KademliaBtn.EnableWindow(TRUE);
		m_co_TransferBtn.EnableWindow(TRUE);
		m_co_SearchBtn.EnableWindow(TRUE);
		m_co_SharedBtn.EnableWindow(TRUE);
		m_co_MessagesBtn.EnableWindow(TRUE);
		m_co_StatisticBtn.EnableWindow(TRUE);
		m_co_PreferencesBtn.EnableWindow(TRUE);				

		m_co_ServerBtn.EnableWindow(FALSE);
//<== Toolbar [shadow2004]
		SetActiveDialog(serverwnd);
			}

	SetAllIcons();
	Localize();

	// set updateintervall of graphic rate display (in seconds)
	//ShowConnectionState(false);

//==> Toolbar [shadow2004]
	RECT rect;
	GetClientRect(&rect);

	CRect srect;

	statusbar->GetClientRect(&srect);
	statusbar->SetWindowPos(NULL,0,rect.bottom-(srect.bottom-srect.top),rect.right-rect.left,rect.bottom-rect.top,SWP_NOZORDER);
	statusbar->GetClientRect(&srect);
//<== Toolbar [shadow2004]

	CWnd* apWnds[] =
	{
		serverwnd,
		kademliawnd,
		transferwnd,
		sharedfileswnd,
		searchwnd,
		chatwnd,
		statisticswnd
	};
	for (int i = 0; i < ARRSIZE(apWnds); i++)
//==> Toolbar [shadow2004]
		apWnds[i]->SetWindowPos(NULL,0,66,rect.right-rect.left,rect.bottom-rect.top-66-(srect.bottom-srect.top),SWP_NOZORDER);

	GetDlgItem(IDC_BTN_CONNECT)->SetWindowPos(NULL,16,4,56,56,SWP_NOZORDER);
	GetDlgItem(IDC_BTN_KADEMLIA)->SetWindowPos(NULL,73,4,56,56,SWP_NOZORDER);
	GetDlgItem(IDC_BTN_SERVER)->SetWindowPos(NULL,130,4,56,56,SWP_NOZORDER);
	GetDlgItem(IDC_BTN_TRANSFER)->SetWindowPos(NULL,187,4,56,56,SWP_NOZORDER);
	GetDlgItem(IDC_BTN_SEARCH)->SetWindowPos(NULL,244,4,56,56,SWP_NOZORDER);
	GetDlgItem(IDC_BTN_FILES)->SetWindowPos(NULL,301,4,56,56,SWP_NOZORDER);
	GetDlgItem(IDC_BTN_MESSAGES)->SetWindowPos(NULL,358,4,56,56,SWP_NOZORDER);
	GetDlgItem(IDC_BTN_STATISTIC)->SetWindowPos(NULL,415,4,56,56,SWP_NOZORDER);
	GetDlgItem(IDC_BTN_PREFERENCES)->SetWindowPos(NULL,472,4,56,56,SWP_NOZORDER);
	GetDlgItem(IDC_BTN_TOOLS)->SetWindowPos(NULL,529,4,56,56,SWP_NOZORDER);


	RECT trect4={0,39,56,55};
	m_co_ConnectBtn.Set_TextPos(trect4);
	m_co_KademliaBtn.Set_TextPos(trect4);
	m_co_ServerBtn.Set_TextPos(trect4);
	m_co_TransferBtn.Set_TextPos(trect4);
	m_co_SearchBtn.Set_TextPos(trect4);
	m_co_SharedBtn.Set_TextPos(trect4);
	m_co_MessagesBtn.Set_TextPos(trect4);
	m_co_StatisticBtn.Set_TextPos(trect4);
	m_co_PreferencesBtn.Set_TextPos(trect4);
	m_co_ToolsBtn.Set_TextPos(trect4);


	m_co_ConnectBtn.SetSkin(IDB_CONNECT_NORMAL,IDB_CONNECT_CLICK,IDB_CONNECT_OVER,IDB_CONNECT_CLICK,0,0,0,0,0);
	m_co_KademliaBtn.SetSkin(IDB_KADEMLIA_NORMAL,IDB_KADEMLIA_CLICK,IDB_KADEMLIA_OVER,IDB_KADEMLIA_CLICK,0,0,0,0,0);
	m_co_ServerBtn.SetSkin(IDB_SERVER_NORMAL,IDB_SERVER_CLICK,IDB_SERVER_OVER,IDB_SERVER_CLICK,0,0,0,0,0);
	m_co_TransferBtn.SetSkin(IDB_TRANSFER_NORMAL,IDB_TRANSFER_CLICK,IDB_TRANSFER_OVER,IDB_TRANSFER_CLICK,0,0,0,0,0);
	m_co_SearchBtn.SetSkin(IDB_SEARCH_NORMAL,IDB_SEARCH_CLICK,IDB_SEARCH_OVER,IDB_SEARCH_CLICK,0,0,0,0,0);
	m_co_SharedBtn.SetSkin(IDB_SHARED_NORMAL,IDB_SHARED_CLICK,IDB_SHARED_OVER,IDB_SHARED_CLICK,0,0,0,0,0);

	m_co_MessagesBtn.SetSkin(IDB_MESSAGES_NORMAL,IDB_MESSAGES_CLICK,IDB_MESSAGES_OVER,IDB_MESSAGES_CLICK,0,0,0,0,0);
	m_co_StatisticBtn.SetSkin(IDB_STATISTIC_NORMAL,IDB_STATISTIC_CLICK,IDB_STATISTIC_OVER,IDB_STATISTIC_CLICK,0,0,0,0,0);
	m_co_PreferencesBtn.SetSkin(IDB_PREFERENCES_NORMAL,IDB_PREFERENCES_CLICK,IDB_PREFERENCES_OVER,IDB_PREFERENCES_CLICK,0,0,0,0,0);
	m_co_ToolsBtn.SetSkin(IDB_TOOLS_NORMAL,IDB_TOOLS_CLICK,IDB_TOOLS_OVER,IDB_TOOLS_CLICK,0,0,0,0,0);
//<== Toolbar [shadow2004]

	// anchors
	AddAnchor(*serverwnd,		TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(*kademliawnd,		TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(*transferwnd,		TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(*sharedfileswnd,	TOP_LEFT, BOTTOM_RIGHT);
    AddAnchor(*searchwnd,		TOP_LEFT, BOTTOM_RIGHT);
    AddAnchor(*chatwnd,			TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(*statisticswnd,	TOP_LEFT, BOTTOM_RIGHT);
//==> Toolbar [shadow2004]
//	AddAnchor(*pwndToolbarX,	TOP_LEFT, TOP_RIGHT);
//<== Toolbar [shadow2004]
	AddAnchor(*statusbar,		BOTTOM_LEFT, BOTTOM_RIGHT);
//==> Toolbar [shadow2004]
//	AddAnchor(IDC_BTN_TOOLS,TOP_RIGHT);
//	AddAnchor(IDC_BTN_PREFERENCES,TOP_RIGHT);
//<== Toolbar [shadow2004]
	statisticswnd->ShowInterval();

	// tray icon
	TraySetMinimizeToTray(thePrefs.GetMinTrayPTR());
	TrayMinimizeToTrayChange();

	ShowTransferRate(true);
    ShowPing();
	searchwnd->UpdateCatTabs();

	///////////////////////////////////////////////////////////////////////////
	// Restore saved window placement
	//
	WINDOWPLACEMENT wp = {0};
	wp.length = sizeof(wp);
	wp = thePrefs.GetEmuleWindowPlacement();
	if (m_bStartMinimized)
	{
		// To avoid the window flickering during startup we try to set the proper window show state right here.
		if (*thePrefs.GetMinTrayPTR())
		{
			// Minimize to System Tray
			//
			// Unfortunately this does not work. The eMule main window is a modal dialog which is invoked
			// by CDialog::DoModal which eventually calls CWnd::RunModalLoop. Look at 'MLF_SHOWONIDLE' and
			// 'bShowIdle' in the above noted functions to see why it's not possible to create the window
			// right in hidden state.

			//--- attempt #1
			//wp.showCmd = SW_HIDE;
			//TrayShow();
			//--- doesn't work at all

			//--- attempt #2
			//if (wp.showCmd == SW_SHOWMAXIMIZED)
			//	wp.flags = WPF_RESTORETOMAXIMIZED;
			//m_bStartMinimizedChecked = false; // post-hide the window..
			//--- creates window flickering

			//--- attempt #3
			// Minimize the window into the task bar and later move it into the tray bar
			if (wp.showCmd == SW_SHOWMAXIMIZED)
				wp.flags = WPF_RESTORETOMAXIMIZED;
			wp.showCmd = SW_MINIMIZE;
			m_bStartMinimizedChecked = false;

			// to get properly restored from tray bar (after attempt #3) we have to use a patched 'restore' window cmd..
			m_wpFirstRestore = thePrefs.GetEmuleWindowPlacement();
			m_wpFirstRestore.length = sizeof(m_wpFirstRestore);
			if (m_wpFirstRestore.showCmd != SW_SHOWMAXIMIZED)
				m_wpFirstRestore.showCmd = SW_SHOWNORMAL;
		}
		else {
			// Minimize to System Taskbar
			//
			if (wp.showCmd == SW_SHOWMAXIMIZED)
				wp.flags = WPF_RESTORETOMAXIMIZED;
			wp.showCmd = SW_MINIMIZE; // Minimize window but do not activate it.
			m_bStartMinimizedChecked = true;
		}
	}
	else
	{
		// Allow only SW_SHOWNORMAL and SW_SHOWMAXIMIZED. Ignore SW_SHOWMINIMIZED to make sure the window
		// becomes visible. If user wants SW_SHOWMINIMIZED, we already have an explicit option for this (see above).
		if (wp.showCmd != SW_SHOWMAXIMIZED)
			wp.showCmd = SW_SHOWNORMAL;
		m_bStartMinimizedChecked = true;
	}
	SetWindowPlacement(&wp);

	if (thePrefs.GetWSIsEnabled())
		theApp.webserver->StartServer();

	VERIFY( (m_hTimer = ::SetTimer(NULL, NULL, 300, StartupTimer)) != NULL );
	if (thePrefs.GetVerbose() && !m_hTimer)
		AddDebugLogLine(true,_T("Failed to create 'startup' timer - %s"),GetErrorMessage(GetLastError()));

	theStats.starttime = GetTickCount();

	if (thePrefs.IsFirstStart())
	{
		// temporary disable the 'startup minimized' option, otherwise no window will be shown at all
		m_bStartMinimized = false;
		DestroySplash();

		extern BOOL FirstTimeWizard();
		if (FirstTimeWizard()){
			// start connection wizard
			CConnectionWizardDlg conWizard;
			conWizard.DoModal();
		}
	}

	VERIFY( m_pDropTarget->Register(this) );

	// initalize PeerCache
	theApp.m_pPeerCache->Init(thePrefs.GetPeerCacheLastSearch(), thePrefs.WasPeerCacheFound(), thePrefs.IsPeerCacheDownloadEnabled(), thePrefs.GetPeerCachePort());
	
	// start aichsyncthread
	AfxBeginThread(RUNTIME_CLASS(CAICHSyncThread), THREAD_PRIORITY_BELOW_NORMAL,0);

	return TRUE;
}

// modders: dont remove or change the original versioncheck! (additionals are ok)
void CemuleDlg::DoVersioncheck(bool manual) {

	if (!manual && thePrefs.GetLastVC()!=0) {
		CTime last(thePrefs.GetLastVC());
		time_t tLast=safe_mktime(last.GetLocalTm());
		time_t tNow=safe_mktime(CTime::GetCurrentTime().GetLocalTm());

		if ( (difftime(tNow,tLast) / 86400)<thePrefs.GetUpdateDays() )
			return;
	}
	if (WSAAsyncGetHostByName(m_hWnd, UM_VERSIONCHECK_RESPONSE, "vcdns2.emule-project.org", m_acVCDNSBuffer, sizeof(m_acVCDNSBuffer)) == 0){
		AddLogLine(true,GetResString(IDS_NEWVERSIONFAILED));
	}
}


void CALLBACK CemuleDlg::StartupTimer(HWND hwnd, UINT uiMsg, UINT idEvent, DWORD dwTime)
{
	// NOTE: Always handle all type of MFC exceptions in TimerProcs - otherwise we'll get mem leaks
	try
	{
		switch(theApp.emuledlg->status){
			case 0:
				theApp.emuledlg->status++;
				theApp.emuledlg->ready = true;
				theApp.sharedfiles->SetOutputCtrl(&theApp.emuledlg->sharedfileswnd->sharedfilesctrl);
				theApp.emuledlg->status++;
				break;
			case 1:
				break;
			case 2:
				theApp.emuledlg->status++;
				try{
					theApp.serverlist->Init();
				}
				catch(...){
					ASSERT(0);
					LogError(LOG_STATUSBAR,_T("Failed to initialize server list - Unknown exception"));
				}
				theApp.emuledlg->status++;
				break;
			case 3:
				break;
			case 4:{
				bool bError = false;
				theApp.emuledlg->status++;

				// NOTE: If we have an unhandled exception in CDownloadQueue::Init, MFC will silently catch it
				// and the creation of the TCP and the UDP socket will not be done -> client will get a LowID!
				try{
					theApp.downloadqueue->Init();
				}
				catch(...){
					ASSERT(0);
					LogError(LOG_STATUSBAR,_T("Failed to initialize download queue - Unknown exception"));
					bError = true;
				}
				if(!theApp.listensocket->StartListening()){
					LogError(LOG_STATUSBAR, GetResString(IDS_MAIN_SOCKETERROR),thePrefs.GetPort());
					bError = true;
				}
				if(!theApp.clientudp->Create()){
				    LogError(LOG_STATUSBAR, GetResString(IDS_MAIN_SOCKETERROR),thePrefs.GetUDPPort());
					bError = true;
				}
				
				if (!bError) // show the success msg, only if we had no serious error
//==> WINSOCK2 [shadow2004]
#ifdef WINSOCK2 //WINSOCK2
					{
					AddLogLine(false,_T("*********   WinSock   *********"));
					AddLogLine(false,_T("Version %d.%d [%.40s] %.40s"), HIBYTE( theApp.m_wsaData.wVersion ),LOBYTE(theApp.m_wsaData.wVersion ),
					(CString)theApp.m_wsaData.szDescription, (CString)theApp.m_wsaData.szSystemStatus);
					if (theApp.m_wsaData.iMaxSockets!=0)
						AddLogLine(false,_T("max. sockets %d"), theApp.m_wsaData.iMaxSockets);
					else
						AddLogLine(false,_T("unlimited sockets"));
					AddLogLine(false,_T("*********   WinSock   *********"));
#endif //WINSOCK2
//<== WINSOCK2 [shadow2004]
					AddLogLine(true, GetResString(IDS_MAIN_READY),theApp.m_strCurVersionLong);
//==> WINSOCK2 [shadow2004]
#ifdef WINSOCK2 //WINSOCK2
					}
#endif //WINSOCK2
//<== WINSOCK2 [shadow2004]
				if(thePrefs.DoAutoConnect())
					theApp.emuledlg->OnBnClickedButton2();
				theApp.emuledlg->status++;
				break;
			}
			case 5:
				break;
			default:
				theApp.emuledlg->StopTimer();
		}
	}
	CATCH_DFLT_EXCEPTIONS(_T("CemuleDlg::StartupTimer"))
}

void CemuleDlg::StopTimer()
{
	if (m_hTimer){
		VERIFY( ::KillTimer(NULL, m_hTimer) );
		m_hTimer = 0;
	}
	
	if (thePrefs.UpdateNotify())
		DoVersioncheck(false);
	
	if (theApp.pstrPendingLink != NULL){
		OnWMData(NULL, (LPARAM)&theApp.sendstruct);
		delete theApp.pstrPendingLink;
	}
}

void CemuleDlg::OnSysCommand(UINT nID, LPARAM lParam){
	// Systemmenu-Speedselector
	if (nID>=MP_QS_U10 && nID<=10512) {
		QuickSpeedUpload(nID);
		return;
	}
	if (nID>=MP_QS_D10 && nID<=10531) {
		QuickSpeedDownload(nID);
		return;
	}
	if (nID==MP_QS_PA || nID==MP_QS_UA) {
		QuickSpeedOther(nID);
		return;
	}
	
	switch (nID /*& 0xFFF0*/){
		case MP_ABOUTBOX : {
			CCreditsDlg dlgAbout;
			dlgAbout.DoModal();
			break;
		}
		case MP_VERSIONCHECK:
			DoVersioncheck(true);
			break;
		case MP_CONNECT : {
			StartConnection();
			break;
		}
		case MP_DISCONNECT : {
			CloseConnection();
			break;
		}
		default:{
			CTrayDialog::OnSysCommand(nID, lParam);
		}
	}

	if (
		(nID & 0xFFF0) == SC_MINIMIZE ||
		(nID & 0xFFF0) == SC_MINIMIZETRAY ||
		(nID & 0xFFF0) == SC_RESTORE ||
		(nID & 0xFFF0) == SC_MAXIMIZE ) { 
		ShowTransferRate(true);
		ShowPing();
		transferwnd->UpdateCatTabTitles();
	}
}

void CemuleDlg::PostStartupMinimized()
{
	if (!m_bStartMinimizedChecked)
	{
		//TODO: Use full initialized 'WINDOWPLACEMENT' and remove the 'OnCancel' call...
		// Isn't that easy.. Read comments in OnInitDialog..
		m_bStartMinimizedChecked = true;
		if (m_bStartMinimized)
		{
			if (theApp.DidWeAutoStart())
			{
				if (!thePrefs.mintotray) {
					thePrefs.mintotray = true;
					OnCancel();
					thePrefs.mintotray = false;
				}
				else
					OnCancel();
			}
			else
				OnCancel();
		}
	}
}

void CemuleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
		CTrayDialog::OnPaint();
}

HCURSOR CemuleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CemuleDlg::OnBnClickedButton2(){
	if (!theApp.IsConnected())
		//connect if not currently connected
		if (!theApp.serverconnect->IsConnecting() && !Kademlia::CKademlia::isRunning() ){
			StartConnection();
		}
		else {
			CloseConnection();
		}
	else{
		//disconnect if currently connected
		CloseConnection();
	}
}

void CemuleDlg::ResetServerInfo(){
	serverwnd->servermsgbox->Reset();
}
void CemuleDlg::ResetLog(){
	serverwnd->logbox->Reset();
}

//==>Anti-Leecher-Log [cyrex2001]
#ifdef ANTI_LEECHER_LOG
void CemuleDlg::ResetLeecherLog()
{
	serverwnd->leecherlog->Reset();
}
#endif
//<== Anti-Leecher-Log [cyrex2001]

void CemuleDlg::ResetDebugLog(){
	serverwnd->debuglog->Reset();
}

void CemuleDlg::AddLogText(UINT uFlags, LPCTSTR pszText)
{
	if (GetCurrentThreadId() != _uMainThreadId)
	{
		theApp.QueueLogLineEx(uFlags, _T("%s"), pszText);
		return;
	}

	if (uFlags & LOG_STATUSBAR)
	{
        if (statusbar->m_hWnd /*&& ready*/)
		{
			if (theApp.m_app_state != APP_STATE_SHUTINGDOWN)
				statusbar->SetText(pszText, SBarLog, 0);
		}
		else
			AfxMessageBox(pszText);
	}
#if defined(_DEBUG) || defined(USE_DEBUG_DEVICE)
	Debug(_T("%s\n"), pszText);
#endif

	if ((uFlags & LOG_DEBUG) && !thePrefs.GetVerbose())
		return;

//==>Anti-Leecher-Log [cyrex2001]
#ifdef ANTI_LEECHER_LOG
	if ((uFlags & LOG_LEECHER) && !thePrefs.GetVerbose())
		return;
#endif
//<== Anti-Leecher-Log [cyrex2001]

	TCHAR temp[1060];
	int iLen = _sntprintf(temp, ARRSIZE(temp), _T("%s: %s\r\n"), CTime::GetCurrentTime().Format(thePrefs.GetDateTimeFormat4Log()), pszText);
	if (iLen >= 0)
	{
//==>Anti-Leecher-Log [cyrex2001]
#ifdef ANTI_LEECHER_LOG
		if (!(uFlags & LOG_DEBUG) && !(uFlags & LOG_LEECHER))
#else //Anti-Leecher-Log
		if (!(uFlags & LOG_DEBUG))
#endif
//<== Anti-Leecher-Log [cyrex2001]
		{
			serverwnd->logbox->AddTyped(temp, iLen, uFlags);
			if (IsWindow(serverwnd->StatusSelector) && serverwnd->StatusSelector.GetCurSel() != CServerWnd::PaneLog)
				serverwnd->StatusSelector.HighlightItem(CServerWnd::PaneLog, TRUE);
			if (!(uFlags & LOG_DONTNOTIFY) && ready)
				ShowNotifier(pszText, TBN_LOG);
			if (thePrefs.GetLog2Disk())
				theLog.Log(temp, iLen);
		}
//==>Anti-Leecher-Log [cyrex2001]
#ifdef ANTI_LEECHER_LOG
		else
		if (thePrefs.GetVerbose() && (uFlags & LOG_LEECHER) )
		{
			serverwnd->leecherlog->AddTyped(temp, iLen, uFlags);
			if (IsWindow(serverwnd->StatusSelector) && serverwnd->StatusSelector.GetCurSel() != CServerWnd::PaneLeecherLog)
				serverwnd->StatusSelector.HighlightItem(CServerWnd::PaneLeecherLog, TRUE);

			if (thePrefs.GetDebug2Disk())
				theVerboseLog.Log(temp, iLen);
		}
		else
#endif
//<== Anti-Leecher-Log [cyrex2001]
		if (thePrefs.GetVerbose() && ((uFlags & LOG_DEBUG) || thePrefs.GetFullVerbose()))
		{
			serverwnd->debuglog->AddTyped(temp, iLen, uFlags);
			if (IsWindow(serverwnd->StatusSelector) && serverwnd->StatusSelector.GetCurSel() != CServerWnd::PaneVerboseLog)
				serverwnd->StatusSelector.HighlightItem(CServerWnd::PaneVerboseLog, TRUE);

			if (thePrefs.GetDebug2Disk())
				theVerboseLog.Log(temp, iLen);
		}
	}
}

CString CemuleDlg::GetLastLogEntry()
{
	return serverwnd->logbox->GetLastLogEntry();
}

CString CemuleDlg::GetAllLogEntries()
{
	return serverwnd->logbox->GetAllLogEntries();
}

CString CemuleDlg::GetLastDebugLogEntry()
{
	return serverwnd->debuglog->GetLastLogEntry();
}

CString CemuleDlg::GetAllDebugLogEntries()
{
	return serverwnd->debuglog->GetAllLogEntries();
}
CString CemuleDlg::GetServerInfoText()
{
	return serverwnd->servermsgbox->GetText();
}

void CemuleDlg::AddServerMessageLine(LPCTSTR pszLine)
{
	serverwnd->servermsgbox->AppendText(pszLine + CString(_T('\n')));
	if (IsWindow(serverwnd->StatusSelector) && serverwnd->StatusSelector.GetCurSel() != CServerWnd::PaneServerInfo)
		serverwnd->StatusSelector.HighlightItem(CServerWnd::PaneServerInfo, TRUE);
}

UINT CemuleDlg::GetConnectionStateIconIndex() const
{
	if (theApp.serverconnect->IsConnected() && !Kademlia::CKademlia::isConnected())
	{
		if (theApp.serverconnect->IsLowID())
			return 3; // LowNot
		else
			return 6; // HighNot
	}
	else if (!theApp.serverconnect->IsConnected() && Kademlia::CKademlia::isConnected())
	{
		if (Kademlia::CKademlia::isFirewalled())
			return 1; // NotLow
		else
			return 2; // NotHigh
	}
	else if (theApp.serverconnect->IsConnected() && Kademlia::CKademlia::isConnected())
	{
		if (theApp.serverconnect->IsLowID() && Kademlia::CKademlia::isFirewalled())
			return 4; // LowLow
		else if (theApp.serverconnect->IsLowID())
			return 5; // LowHigh
		else if (Kademlia::CKademlia::isFirewalled())
			return 7; // HighLow
		else
			return 8; // HighHigh
	}
	else
	{
		return 0; // NotNot
	}
}

void CemuleDlg::ShowConnectionStateIcon()
{
	UINT uIconIdx = GetConnectionStateIconIndex();
	if (uIconIdx >= ARRSIZE(connicons)){
		ASSERT(0);
		uIconIdx = 0;
	}
	statusbar->SetIcon(SBarConnected, connicons[uIconIdx]);
}

CString CemuleDlg::GetConnectionStateString()
{
	CString status;
	if (theApp.serverconnect->IsConnected())
		status = _T("eD2K:") + GetResString(IDS_CONNECTED);
	else if (theApp.serverconnect->IsConnecting())
		status = _T("eD2K:") + GetResString(IDS_CONNECTING);
	else
		status = _T("eD2K:") + GetResString(IDS_NOTCONNECTED);

	if (Kademlia::CKademlia::isConnected())
		status += _T("|Kad:") + GetResString(IDS_CONNECTED);
	else if (Kademlia::CKademlia::isRunning())
		status += _T("|Kad:") + GetResString(IDS_CONNECTING);
	else
		status += _T("|Kad:") + GetResString(IDS_NOTCONNECTED);
	return status;
}

void CemuleDlg::ShowConnectionState()
{
	theApp.downloadqueue->OnConnectionState(theApp.IsConnected());
	serverwnd->UpdateMyInfo();
	serverwnd->UpdateControlsState();
	kademliawnd->UpdateControlsState();

	ShowConnectionStateIcon();
	statusbar->SetText(GetConnectionStateString(), SBarConnected, 0);

	if (theApp.IsConnected())
	{
//==> Toolbar [shadow2004]
		SetDlgItemText(IDC_BTN_CONNECT,GetResString(IDS_MAIN_BTN_DISCONNECT));
		m_co_ConnectBtn.SetSkin(IDB_CONNECT_CLICK,IDB_CONNECT_CLICK,IDB_DISCONNECT_OVER,IDB_CONNECT_CLICK,0,0,0,0,0);
//<== Toolbar [shadow2004]
	}
	else
	{
		if (theApp.serverconnect->IsConnecting() || Kademlia::CKademlia::isRunning()) 
		{
//==> Toolbar [shadow2004]
			SetDlgItemText(IDC_BTN_CONNECT,GetResString(IDS_MAIN_BTN_CANCEL));
			m_co_ConnectBtn.SetSkin(IDB_CONNECT_CLICK,IDB_CONNECT_CLICK,IDB_DISCONNECT_OVER,IDB_CONNECT_CLICK,0,0,0,0,0);
//<== Toolbar [shadow2004]
			ShowUserCount();
		} 
		else 
		{
//==> Toolbar [shadow2004]
			SetDlgItemText(IDC_BTN_CONNECT,GetResString(IDS_MAIN_BTN_CONNECT));
			m_co_ConnectBtn.SetSkin(IDB_CONNECT_NORMAL,IDB_CONNECT_CLICK,IDB_CONNECT_OVER,IDB_CONNECT_CLICK,0,0,0,0,0);
//<== Toolbar [shadow2004]
			ShowUserCount();
		}

	}
}

void CemuleDlg::ShowUserCount()
{
	uint32 totaluser, totalfile;
	totaluser = totalfile = 0;
	theApp.serverlist->GetUserFileStatus( totaluser, totalfile );
	CString buffer;
	buffer.Format(_T("%s:%s(%s)|%s:%s(%s)"), GetResString(IDS_UUSERS), CastItoIShort(totaluser, false, 1), CastItoIShort(Kademlia::CKademlia::getKademliaUsers(), false, 1), GetResString(IDS_FILES), CastItoIShort(totalfile, false, 1), CastItoIShort(Kademlia::CKademlia::getKademliaFiles(), false, 1));
	statusbar->SetText(buffer, SBarUsers, 0);
}

void CemuleDlg::ShowMessageState(uint8 iconnr)
{
	m_iMsgIcon = iconnr;
	statusbar->SetIcon(SBarChatMsg, imicons[m_iMsgIcon]);
}

void CemuleDlg::ShowTransferStateIcon()
{
	if (m_uUpDatarate && m_uDownDatarate)
		statusbar->SetIcon(SBarUpDown, transicons[3]);
	else if (m_uUpDatarate)
		statusbar->SetIcon(SBarUpDown, transicons[2]);
	else if (m_uDownDatarate)
		statusbar->SetIcon(SBarUpDown, transicons[1]);
	else
		statusbar->SetIcon(SBarUpDown, transicons[0]);
}

CString CemuleDlg::GetUpDatarateString(UINT uUpDatarate)
{
	m_uUpDatarate = uUpDatarate != (UINT)-1 ? uUpDatarate : theApp.uploadqueue->GetDatarate();
	TCHAR szBuff[128];
	if (thePrefs.ShowOverhead())
		_sntprintf(szBuff, ARRSIZE(szBuff), _T("%.1f (%.1f)"), (float)m_uUpDatarate/1024, (float)theStats.GetUpDatarateOverhead()/1024);
	else
		_sntprintf(szBuff, ARRSIZE(szBuff), _T("%.1f"), (float)m_uUpDatarate/1024);
	return szBuff;
}

CString CemuleDlg::GetDownDatarateString(UINT uDownDatarate)
{
	m_uDownDatarate = uDownDatarate != (UINT)-1 ? uDownDatarate : theApp.downloadqueue->GetDatarate();
	TCHAR szBuff[128];
	if (thePrefs.ShowOverhead())
		_sntprintf(szBuff, ARRSIZE(szBuff), _T("%.1f (%.1f)"), (float)m_uDownDatarate/1024, (float)theStats.GetDownDatarateOverhead()/1024);
	else
		_sntprintf(szBuff, ARRSIZE(szBuff), _T("%.1f"), (float)m_uDownDatarate/1024);
	return szBuff;
}

CString CemuleDlg::GetTransferRateString()
{
	TCHAR szBuff[128];
	if (thePrefs.ShowOverhead())
		_sntprintf(szBuff, ARRSIZE(szBuff), GetResString(IDS_UPDOWN),
				  (float)m_uUpDatarate/1024, (float)theStats.GetUpDatarateOverhead()/1024,
				  (float)m_uDownDatarate/1024, (float)theStats.GetDownDatarateOverhead()/1024);
	else
		_sntprintf(szBuff, ARRSIZE(szBuff), GetResString(IDS_UPDOWNSMALL), (float)m_uUpDatarate/1024, (float)m_uDownDatarate/1024);
	return szBuff;
}

void CemuleDlg::ShowTransferRate(bool bForceAll)
{
	if (bForceAll)
		m_uLastSysTrayIconCookie = SYS_TRAY_ICON_COOKIE_FORCE_UPDATE;

	m_uDownDatarate = theApp.downloadqueue->GetDatarate();
	m_uUpDatarate = theApp.uploadqueue->GetDatarate();

	CString strTransferRate = GetTransferRateString();
	if (TrayIsVisible() || bForceAll)
	{
		TCHAR buffer2[100];
		// set trayicon-icon
		int iDownRateProcent = (int)ceil((m_uDownDatarate/10.24) / thePrefs.GetMaxGraphDownloadRate());
		if (iDownRateProcent > 100)
			iDownRateProcent = 100;
		UpdateTrayIcon(iDownRateProcent);

//==>Modversion [shadow2004]
#ifdef MODVERSION
		if (theApp.IsConnected())
			_sntprintf(buffer2,ARRSIZE(buffer2),_T("[%s] (%s)\r\n%s"),theApp.m_strModLongVersion,GetResString(IDS_CONNECTED), strTransferRate);
		else 
			_sntprintf(buffer2,ARRSIZE(buffer2),_T("[%s] (%s)\r\n%s"),theApp.m_strModLongVersion,GetResString(IDS_DISCONNECTED), strTransferRate);
#else //Modversion
		if (theApp.IsConnected()) 
			_sntprintf(buffer2, ARRSIZE(buffer2), _T("eMule v%s (%s)\r\n%s"), theApp.m_strCurVersionLong, GetResString(IDS_CONNECTED), strTransferRate);
		else
			_sntprintf(buffer2, ARRSIZE(buffer2), _T("eMule v%s (%s)\r\n%s"), theApp.m_strCurVersionLong, GetResString(IDS_DISCONNECTED), strTransferRate);
#endif //Modversion
//<==Modversion [shadow2004]

		buffer2[63] = _T('\0');
		TraySetToolTip(buffer2);
	}

	if (IsWindowVisible() || bForceAll)
	{
		statusbar->SetText(strTransferRate, SBarUpDown, 0);
		ShowTransferStateIcon();
	}
	if (IsWindowVisible() && thePrefs.ShowRatesOnTitle())
	{
		TCHAR szBuff[128];
//==>Modversion [shadow2004]
#ifdef MODVERSION
		_sntprintf(szBuff,ARRSIZE(szBuff),_T("(U:%.1f D:%.1f) eMule v%s [%s]"),(float)m_uUpDatarate/1024, (float)m_uDownDatarate/1024,theApp.m_strCurVersionLong,theApp.m_strModLongVersion);
#else //Modversion
		_sntprintf(szBuff, ARRSIZE(szBuff), _T("(U:%.1f D:%.1f) eMule v%s"), (float)m_uUpDatarate/1024, (float)m_uDownDatarate/1024, theApp.m_strCurVersionLong);
#endif //Modversion
//<==Modversion [shadow2004]
		SetWindowText(szBuff);
	}
	if (m_pMiniMule && m_pMiniMule->m_hWnd && m_pMiniMule->IsWindowVisible() && !m_pMiniMule->GetAutoClose())
	{
		m_pMiniMule->UpdateContent(m_uUpDatarate, m_uDownDatarate);
	}
}

void CemuleDlg::ShowPing()
{
    if (IsWindowVisible())
	{
        CString buffer;
        if (thePrefs.IsDynUpEnabled())
		{
			CurrentPingStruct lastPing = theApp.lastCommonRouteFinder->GetCurrentPing();
            if (lastPing.state.GetLength() == 0)
			{
                if (lastPing.lowest > 0 && !thePrefs.IsDynUpUseMillisecondPingTolerance())
                    buffer.Format(_T("%.1f | %ims | %i%%"),lastPing.currentLimit/1024.0f, lastPing.latency, lastPing.latency*100/lastPing.lowest);
                else
                    buffer.Format(_T("%.1f | %ims"),lastPing.currentLimit/1024.0f, lastPing.latency);
            }
			else
                buffer.SetString(lastPing.state);
        }
		statusbar->SetText(buffer, SBarChatMsg, 0);
    }
}

void CemuleDlg::OnOK()
{
}

void CemuleDlg::OnCancel()
{
	if (!thePrefs.GetStraightWindowStyles())
	{
	if (*thePrefs.GetMinTrayPTR())
	{
		TrayShow();
		ShowWindow(SW_HIDE);
	}
	else
		{
		ShowWindow(SW_MINIMIZE);
		}
	ShowTransferRate();
    ShowPing();
}
}

void CemuleDlg::SetActiveDialog(CWnd* dlg)
{
	if (dlg == activewnd)
		return;
	if (activewnd)
		activewnd->ShowWindow(SW_HIDE);
	dlg->ShowWindow(SW_SHOW);
	dlg->SetFocus();
	activewnd = dlg;
	if (dlg == transferwnd)
	{
		if (thePrefs.ShowCatTabInfos()) transferwnd->UpdateCatTabTitles();
//==> Toolbar [shadow2004]
	}
	else if (dlg == chatwnd)	chatwnd->chatselector.ShowChat();
	else if (dlg == statisticswnd)		statisticswnd->ShowStatistics();
//<== Toolbar [shadow2004]
	}

void CemuleDlg::SetStatusBarPartsSize()
{
	CRect rect;
	statusbar->GetClientRect(&rect);
	int ussShift = 0;
	if(thePrefs.IsDynUpEnabled())
	{
        if (thePrefs.IsDynUpUseMillisecondPingTolerance())
            ussShift = 45;
        else
            ussShift = 90;
        }
	
	int aiWidths[5] =
	{ 
		rect.right - 675 - ussShift,
		rect.right - 440 - ussShift,
		rect.right - 250 - ussShift,
		rect.right -  25 - ussShift,
		-1
	};
	statusbar->SetParts(ARRSIZE(aiWidths), aiWidths);
}

void CemuleDlg::OnSize(UINT nType, int cx, int cy)
{
//==> Toolbar [shadow2004]
	CRect rect;
	GetClientRect(&rect);
	rect.bottom=66;

	InvalidateRect(&rect,FALSE);
//<== Toolbar [shadow2004]

	CTrayDialog::OnSize(nType, cx, cy);
	SetStatusBarPartsSize();
	transferwnd->VerifyCatTabSize();
}

void CemuleDlg::ProcessED2KLink(LPCTSTR pszData)
{
	try {
		CString link2;
		CString link;
		link2 = pszData;
		link2.Replace(_T("%7c"),_T("|"));
		link = OptUtf8ToStr(URLDecode(link2));
		CED2KLink* pLink = CED2KLink::CreateLinkFromUrl(link);
		_ASSERT( pLink !=0 );
		switch (pLink->GetKind()) {
		case CED2KLink::kFile:
			{
				CED2KFileLink* pFileLink = pLink->GetFileLink();
				_ASSERT(pFileLink !=0);
				theApp.downloadqueue->AddFileLinkToDownload(pFileLink,searchwnd->GetSelectedCat());
			}
			break;
		case CED2KLink::kServerList:
			{
				CED2KServerListLink* pListLink = pLink->GetServerListLink(); 
				_ASSERT( pListLink !=0 ); 
				CString strAddress = pListLink->GetAddress(); 
				if(strAddress.GetLength() != 0)
					serverwnd->UpdateServerMetFromURL(strAddress);
			}
			break;
		case CED2KLink::kServer:
			{
				CString defName;
				CED2KServerLink* pSrvLink = pLink->GetServerLink();
				_ASSERT( pSrvLink !=0 );
				CServer* pSrv = new CServer(pSrvLink->GetPort(), ipstr(pSrvLink->GetIP()));
				_ASSERT( pSrv !=0 );
				pSrvLink->GetDefaultName(defName);
				pSrv->SetListName(defName.GetBuffer());

				// Barry - Default all new servers to high priority
				if (thePrefs.GetManualAddedServersHighPriority())
					pSrv->SetPreference(SRV_PR_HIGH);

				if (!serverwnd->serverlistctrl.AddServer(pSrv,true)) 
					delete pSrv; 
				else
					AddLogLine(true,GetResString(IDS_SERVERADDED), pSrv->GetListName());
			}
			break;
		default:
			break;
		}
		delete pLink;
	}
	catch(CString strError){
		LogWarning(LOG_STATUSBAR, GetResString(IDS_LINKNOTADDED) + _T(" - ") + strError);
	}
	catch(...){
		LogWarning(LOG_STATUSBAR, GetResString(IDS_LINKNOTADDED));
	}
}

LRESULT CemuleDlg::OnWMData(WPARAM wParam,LPARAM lParam)
{
	PCOPYDATASTRUCT data = (PCOPYDATASTRUCT)lParam;
	if (data->dwData == OP_ED2KLINK)
	{
		if (thePrefs.IsBringToFront())
		{
			FlashWindow(TRUE);
			if (IsIconic())
				ShowWindow(SW_SHOWNORMAL);
			else if (TrayHide())
				RestoreWindow();
			else
				SetForegroundWindow();
		}
		ProcessED2KLink((LPCTSTR)data->lpData);
	}
	else if(data->dwData == OP_COLLECTION){
		FlashWindow(TRUE);
		if (IsIconic())
			ShowWindow(SW_SHOWNORMAL);
		else if (TrayHide())
			RestoreWindow();
		else
			SetForegroundWindow();

		CCollection* pCollection = new CCollection();
		CString strPath = CString((LPCTSTR)data->lpData);
		if (pCollection->InitCollectionFromFile(strPath, strPath.Right((strPath.GetLength()-1)-strPath.ReverseFind('\\')))){
			CCollectionViewDialog dialog;
			dialog.SetCollection(pCollection);
			dialog.DoModal();
		}
		delete pCollection;
	}
	else if (data->dwData == OP_CLCOMMAND){
		// command line command received
		CString clcommand((LPCTSTR)data->lpData);
		clcommand.MakeLower();
		AddLogLine(true,_T("CLI: %s"),clcommand);

		if (clcommand==_T("connect")) {StartConnection(); return true;}
		if (clcommand==_T("disconnect")) {theApp.serverconnect->Disconnect(); return true;}
		if (clcommand==_T("resume")) {theApp.downloadqueue->StartNextFile(); return true;}
		if (clcommand==_T("exit")) {OnClose(); return true;}
		if (clcommand==_T("restore")) {RestoreWindow();return true;}
		if (clcommand==_T("reloadipf")) {theApp.ipfilter->LoadFromDefaultFile(); return true;}
		if (clcommand.Left(7).MakeLower()==_T("limits=") && clcommand.GetLength()>8) {
			CString down;
			CString up=clcommand.Mid(7);
			int pos=up.Find(_T(','));
			if (pos>0) {
				down=up.Mid(pos+1);
				up=up.Left(pos);
			}
//==> Maella [FAF] -Allow Bandwidth Settings in <1KB Incremements-
#ifdef FAF
			if (down.GetLength()>0) thePrefs.SetMaxDownload(_tstof(down));
			if (up.GetLength()>0) thePrefs.SetMaxUpload(_tstof(up));
#else
			if (down.GetLength()>0) thePrefs.SetMaxDownload(_tstoi(down));
			if (up.GetLength()>0) thePrefs.SetMaxUpload(_tstoi(up));
#endif
//<== Maella [FAF] -Allow Bandwidth Settings in <1KB Incremements-

			return true;
		}

		if (clcommand==_T("help") || clcommand==_T("/?")) {
			// show usage
			return true;
		}

		if (clcommand==_T("status")) {
			CString strBuff;
			strBuff.Format(_T("%sstatus.log"),thePrefs.GetAppDir());
			FILE* file = _tfsopen(strBuff, _T("wt"), _SH_DENYWR);
			if (file){
				if (theApp.serverconnect->IsConnected())
					strBuff = GetResString(IDS_CONNECTED);
				else if (theApp.serverconnect->IsConnecting())
					strBuff = GetResString(IDS_CONNECTING);
				else
					strBuff = GetResString(IDS_DISCONNECTED);
				_ftprintf(file, _T("%s\n"), strBuff);

				strBuff.Format(GetResString(IDS_UPDOWNSMALL), (float)theApp.uploadqueue->GetDatarate()/1024, (float)theApp.downloadqueue->GetDatarate()/1024);
				_ftprintf(file, _T("%s"), strBuff); // next string (getTextList) is already prefixed with '\n'!
				_ftprintf(file, _T("%s\n"), transferwnd->downloadlistctrl.getTextList());
				
				fclose(file);
			}
			return true;
		}
		// show "unknown command";
	}
	return true;
}

LRESULT CemuleDlg::OnFileHashed(WPARAM wParam, LPARAM lParam)
{
	if (theApp.m_app_state == APP_STATE_SHUTINGDOWN)
		return FALSE;

	CKnownFile* result = (CKnownFile*)lParam;
	ASSERT( result->IsKindOf(RUNTIME_CLASS(CKnownFile)) );

	if (wParam)
	{
		// File hashing finished for a part file when:
		// - part file just completed
		// - part file was rehashed at startup because the file date of part.met did not match the part file date

		CPartFile* requester = (CPartFile*)wParam;
		ASSERT( requester->IsKindOf(RUNTIME_CLASS(CPartFile)) );

		// SLUGFILLER: SafeHash - could have been canceled
		if (theApp.downloadqueue->IsPartFile(requester))
			requester->PartFileHashFinished(result);
		else
			delete result;
		// SLUGFILLER: SafeHash
	}
	else
	{
		ASSERT( !result->IsKindOf(RUNTIME_CLASS(CPartFile)) );

		// File hashing finished for a shared file (none partfile)
		//	- reading shared directories at startup and hashing files which were not found in known.met
		//	- reading shared directories during runtime (user hit Reload button, added a shared directory, ...)
		theApp.sharedfiles->FileHashingFinished(result);
	}
	return TRUE;
}

LRESULT CemuleDlg::OnFileOpProgress(WPARAM wParam, LPARAM lParam)
{
	if (theApp.m_app_state == APP_STATE_SHUTINGDOWN)
		return FALSE;

	CKnownFile* pKnownFile = (CKnownFile*)lParam;
	ASSERT( pKnownFile->IsKindOf(RUNTIME_CLASS(CKnownFile)) );

	if (pKnownFile->IsKindOf(RUNTIME_CLASS(CPartFile)))
	{
		CPartFile* pPartFile = static_cast<CPartFile*>(pKnownFile);
		pPartFile->SetFileOpProgress(wParam);
		pPartFile->UpdateDisplayedInfo(true);
	}

	return 0;
}

// SLUGFILLER: SafeHash
LRESULT CemuleDlg::OnHashFailed(WPARAM wParam, LPARAM lParam)
{
	theApp.sharedfiles->HashFailed((UnknownFile_Struct*)lParam);
	return 0;
}
// SLUGFILLER: SafeHash

LRESULT CemuleDlg::OnFileAllocExc(WPARAM wParam,LPARAM lParam)
{
	if (lParam == 0)
		((CPartFile*)wParam)->FlushBuffersExceptionHandler();
	else
		((CPartFile*)wParam)->FlushBuffersExceptionHandler((CFileException*)lParam);
	return 0;
}

LRESULT CemuleDlg::OnFileCompleted(WPARAM wParam, LPARAM lParam)
{
	CPartFile* partfile = (CPartFile*)lParam;
	ASSERT( partfile != NULL );
	if (partfile)
		partfile->PerformFileCompleteEnd(wParam);
	return 0;
}

BOOL CemuleDlg::OnQueryEndSession()
{
	AddDebugLogLine(DLP_VERYLOW, _T("%hs: start"), __FUNCTION__);
	if (!CTrayDialog::OnQueryEndSession())
		return FALSE;

	AddDebugLogLine(DLP_VERYLOW, _T("%hs: returning TRUE"), __FUNCTION__);
	return TRUE;
}

void CemuleDlg::OnEndSession(BOOL bEnding)
{
	AddDebugLogLine(DLP_VERYLOW, _T("%hs: bEnding=%d"), __FUNCTION__, bEnding);
	if (bEnding && theApp.m_app_state == APP_STATE_RUNNING)
	{
		theApp.m_app_state	= APP_STATE_SHUTINGDOWN;
		OnClose();
	}

	CTrayDialog::OnEndSession(bEnding);
}

void CemuleDlg::OnDestroy()
{
	AddDebugLogLine(DLP_VERYLOW, _T("%hs"), __FUNCTION__);
	CTrayDialog::OnDestroy();
}

bool CemuleDlg::CanClose()
{
	if (theApp.m_app_state == APP_STATE_RUNNING && thePrefs.IsConfirmExitEnabled())
	{
		if (AfxMessageBox(GetResString(IDS_MAIN_EXIT), MB_YESNO | MB_DEFBUTTON2) == IDNO)
			return false;
	}
	return true;
}

void CemuleDlg::OnClose()
{
	if (!CanClose())
		return;

	Log(_T("Closing eMule"));
	m_pDropTarget->Revoke();
	theApp.m_app_state = APP_STATE_SHUTINGDOWN;
	theApp.serverconnect->Disconnect();
	theApp.OnlineSig(); // Added By Bouc7 

	// get main window placement
	WINDOWPLACEMENT wp;
	wp.length = sizeof(wp);
	GetWindowPlacement(&wp);
	ASSERT( wp.showCmd == SW_SHOWMAXIMIZED || wp.showCmd == SW_SHOWMINIMIZED || wp.showCmd == SW_SHOWNORMAL );
	if (wp.showCmd == SW_SHOWMINIMIZED && (wp.flags & WPF_RESTORETOMAXIMIZED))
		wp.showCmd = SW_SHOWMAXIMIZED;
	wp.flags = 0;
	thePrefs.SetWindowLayout(wp);

	// get active main window dialog
	if (activewnd){
		if (activewnd->IsKindOf(RUNTIME_CLASS(CServerWnd)))
			thePrefs.SetLastMainWndDlgID(IDD_SERVER);
		else if (activewnd->IsKindOf(RUNTIME_CLASS(CSharedFilesWnd)))
			thePrefs.SetLastMainWndDlgID(IDD_FILES);
		else if (activewnd->IsKindOf(RUNTIME_CLASS(CSearchDlg)))
			thePrefs.SetLastMainWndDlgID(IDD_SEARCH);
		else if (activewnd->IsKindOf(RUNTIME_CLASS(CChatWnd)))
			thePrefs.SetLastMainWndDlgID(IDD_CHAT);
		else if (activewnd->IsKindOf(RUNTIME_CLASS(CTransferWnd)))
			thePrefs.SetLastMainWndDlgID(IDD_TRANSFER);
		else if (activewnd->IsKindOf(RUNTIME_CLASS(CStatisticsDlg)))
			thePrefs.SetLastMainWndDlgID(IDD_STATISTICS);
		else if (activewnd->IsKindOf(RUNTIME_CLASS(CKademliaWnd)))
			thePrefs.SetLastMainWndDlgID(IDD_KADEMLIAWND);
		else{
			ASSERT(0);
			thePrefs.SetLastMainWndDlgID(0);
		}
	}

	Kademlia::CKademlia::stop();

	// try to wait untill the hashing thread notices that we are shutting down
	CSingleLock sLock1(&theApp.hashing_mut); // only one filehash at a time
	sLock1.Lock(2000);

	// saving data & stuff
//	theApp.emuledlg->preferenceswnd->m_wndSecurity.DeleteDDB();

	theApp.knownfiles->Save();
	searchwnd->SaveAllSettings();
	serverwnd->SaveAllSettings();
	kademliawnd->SaveAllSettings();
	
	theApp.m_pPeerCache->Save();
	theApp.scheduler->RestoreOriginals();
	thePrefs.Save();
	thePerfLog.Shutdown();

	// explicitly delete all listview items which may hold ptrs to objects which will get deleted
	// by the dtors (some lines below) to avoid potential problems during application shutdown.
	transferwnd->downloadlistctrl.DeleteAllItems();
	chatwnd->chatselector.DeleteAllItems();
	theApp.clientlist->DeleteAll();
	searchwnd->DeleteAllSearchListCtrlItems();
	sharedfileswnd->sharedfilesctrl.DeleteAllItems();
    transferwnd->queuelistctrl.DeleteAllItems();
	transferwnd->clientlistctrl.DeleteAllItems();
	transferwnd->uploadlistctrl.DeleteAllItems();
	serverwnd->serverlistctrl.DeleteAllItems();
	
	CPartFileConvert::CloseGUI();
	CPartFileConvert::RemoveAllJobs();

    theApp.uploadBandwidthThrottler->EndThread();
    theApp.lastCommonRouteFinder->EndThread();

	theApp.sharedfiles->DeletePartFileInstances();

	searchwnd->SendMessage(WM_CLOSE);

    // NOTE: Do not move those dtors into 'CemuleApp::InitInstance' (althought they should be there). The
	// dtors are indirectly calling functions which access several windows which would not be available 
	// after we have closed the main window -> crash!
	delete theApp.listensocket;		theApp.listensocket = NULL;
	delete theApp.clientudp;		theApp.clientudp = NULL;
	delete theApp.sharedfiles;		theApp.sharedfiles = NULL;
	delete theApp.serverconnect;	theApp.serverconnect = NULL;
	delete theApp.serverlist;		theApp.serverlist = NULL;
	delete theApp.knownfiles;		theApp.knownfiles = NULL;
	delete theApp.searchlist;		theApp.searchlist = NULL;
	delete theApp.clientcredits;	theApp.clientcredits = NULL;
	delete theApp.downloadqueue;	theApp.downloadqueue = NULL;
	delete theApp.uploadqueue;		theApp.uploadqueue = NULL;
	delete theApp.clientlist;		theApp.clientlist = NULL;
	delete theApp.friendlist;		theApp.friendlist = NULL;
	delete theApp.scheduler;		theApp.scheduler = NULL;
	delete theApp.ipfilter;			theApp.ipfilter = NULL;
	delete theApp.webserver;		theApp.webserver = NULL;
	delete theApp.m_pPeerCache;		theApp.m_pPeerCache = NULL;
	delete theApp.m_pFirewallOpener;theApp.m_pFirewallOpener = NULL;
	delete theApp.uploadBandwidthThrottler; theApp.uploadBandwidthThrottler = NULL;
	delete theApp.lastCommonRouteFinder; theApp.lastCommonRouteFinder = NULL;

	thePrefs.Uninit();
	theApp.m_app_state = APP_STATE_DONE;
	CTrayDialog::OnCancel();
	AddDebugLogLine(DLP_VERYLOW, _T("Closed eMule"));
}

void CemuleDlg::DestroyMiniMule()
{
	if (m_pMiniMule)
	{
		if (!m_pMiniMule->IsInCallback()) // for safety
		{
			m_pMiniMule->DestroyWindow();
			delete m_pMiniMule;
			m_pMiniMule = NULL;
		}
		else
			ASSERT(0);
	}
}

LRESULT CemuleDlg::OnCloseMiniMule(WPARAM wParam, LPARAM lParam)
{
	DestroyMiniMule();
	if (wParam)
		RestoreWindow();
	return 0;
}

void CemuleDlg::OnTrayLButtonUp(CPoint pt)
{
	if(!IsRunning())
		return;

	// Avoid reentrancy problems with main window, options dialog and mini mule window
	if (IsPreferencesDlgOpen()) {
		MessageBeep(MB_OK);
		preferenceswnd->SetForegroundWindow();
		preferenceswnd->BringWindowToTop();
		return;
	}

	if (m_pMiniMule) {
		m_pMiniMule->ShowWindow(SW_SHOW);
		m_pMiniMule->SetForegroundWindow();
		m_pMiniMule->BringWindowToTop();
		return;
	}

	if (thePrefs.GetEnableMiniMule())
	{
		m_pMiniMule = new CMiniMule(this);
		m_pMiniMule->Create(CMiniMule::IDD, this);
		//m_pMiniMule->ShowWindow(SW_SHOW);	// do not explicitly show the window, it will do that for itself when it's ready..
		m_pMiniMule->SetForegroundWindow();
		m_pMiniMule->BringWindowToTop();
	}
}

void CemuleDlg::OnTrayRButtonUp(CPoint pt)
{
	if(!IsRunning())
		return;

	// Avoid reentrancy problems with main window, options dialog and mini mule window
	if (IsPreferencesDlgOpen()) {
		MessageBeep(MB_OK);
		preferenceswnd->SetForegroundWindow();
		preferenceswnd->BringWindowToTop();
		return;
	}

	if (m_pMiniMule)
	{
		if (m_pMiniMule->GetAutoClose())
			DestroyMiniMule();
		else
		{
			// Avoid reentrancy problems with main window, options dialog and mini mule window
			if (m_pMiniMule->m_hWnd && !m_pMiniMule->IsWindowEnabled()) {
				MessageBeep(MB_OK);
				return;
			}
		}
	}

	if (m_pSystrayDlg) {
		m_pSystrayDlg->BringWindowToTop();
		return;
	}

	m_pSystrayDlg = new CMuleSystrayDlg(this, pt,
										thePrefs.GetMaxGraphUploadRate(true), thePrefs.GetMaxGraphDownloadRate(),
										thePrefs.GetMaxUpload(), thePrefs.GetMaxDownload());
	if (m_pSystrayDlg)
	{
		UINT nResult = m_pSystrayDlg->DoModal();
		delete m_pSystrayDlg;
		m_pSystrayDlg = NULL;
		switch (nResult)
		{
			case IDC_TOMAX:
				QuickSpeedOther(MP_QS_UA);
				break;
			case IDC_TOMIN:
				QuickSpeedOther(MP_QS_PA);
				break;
			case IDC_RESTORE:
				RestoreWindow();
				break;
			case IDC_CONNECT:
				StartConnection();
				break;
			case IDC_DISCONNECT:
				CloseConnection();
				break;
			case IDC_EXIT:
				OnClose();
				break;
			case IDC_PREFERENCES:
				ShowPreferences();
				break;
		}
	}
}

void CemuleDlg::AddSpeedSelectorSys(CMenu* addToMenu)
{
	CString text;

	// creating UploadPopup Menu
	ASSERT( m_menuUploadCtrl.m_hMenu == NULL );
	if (m_menuUploadCtrl.CreateMenu())
	{
		//m_menuUploadCtrl.AddMenuTitle(GetResString(IDS_PW_TIT_UP));
		text.Format(_T("20%%\t%i %s"),  (uint16)(thePrefs.GetMaxGraphUploadRate(true)*0.2),GetResString(IDS_KBYTESPERSEC));	m_menuUploadCtrl.AppendMenu(MF_STRING, MP_QS_U20,  text);
		text.Format(_T("40%%\t%i %s"),  (uint16)(thePrefs.GetMaxGraphUploadRate(true)*0.4),GetResString(IDS_KBYTESPERSEC));	m_menuUploadCtrl.AppendMenu(MF_STRING, MP_QS_U40,  text);
		text.Format(_T("60%%\t%i %s"),  (uint16)(thePrefs.GetMaxGraphUploadRate(true)*0.6),GetResString(IDS_KBYTESPERSEC));	m_menuUploadCtrl.AppendMenu(MF_STRING, MP_QS_U60,  text);
		text.Format(_T("80%%\t%i %s"),  (uint16)(thePrefs.GetMaxGraphUploadRate(true)*0.8),GetResString(IDS_KBYTESPERSEC));	m_menuUploadCtrl.AppendMenu(MF_STRING, MP_QS_U80,  text);
		text.Format(_T("100%%\t%i %s"), (uint16)(thePrefs.GetMaxGraphUploadRate(true)),GetResString(IDS_KBYTESPERSEC));		m_menuUploadCtrl.AppendMenu(MF_STRING, MP_QS_U100, text);
		m_menuUploadCtrl.AppendMenu(MF_SEPARATOR);
		
		if (GetRecMaxUpload()>0) {
			text.Format(GetResString(IDS_PW_MINREC)+GetResString(IDS_KBYTESPERSEC),GetRecMaxUpload());
			m_menuUploadCtrl.AppendMenu(MF_STRING, MP_QS_UP10, text );
		}

		//m_menuUploadCtrl.AppendMenu(MF_STRING, MP_QS_UPC, GetResString(IDS_PW_UNLIMITED));

		text.Format(_T("%s:"), GetResString(IDS_PW_UPL));
		addToMenu->AppendMenu(MF_STRING|MF_POPUP, (UINT_PTR)m_menuUploadCtrl.m_hMenu, text);
	}

	// creating DownloadPopup Menu
	ASSERT( m_menuDownloadCtrl.m_hMenu == NULL );
	if (m_menuDownloadCtrl.CreateMenu())
	{
		//m_menuDownloadCtrl.AddMenuTitle(GetResString(IDS_PW_TIT_DOWN));
		text.Format(_T("20%%\t%i %s"),  (uint16)(thePrefs.GetMaxGraphDownloadRate()*0.2),GetResString(IDS_KBYTESPERSEC));	m_menuDownloadCtrl.AppendMenu(MF_STRING|MF_POPUP, MP_QS_D20,  text);
		text.Format(_T("40%%\t%i %s"),  (uint16)(thePrefs.GetMaxGraphDownloadRate()*0.4),GetResString(IDS_KBYTESPERSEC));	m_menuDownloadCtrl.AppendMenu(MF_STRING|MF_POPUP, MP_QS_D40,  text);
		text.Format(_T("60%%\t%i %s"),  (uint16)(thePrefs.GetMaxGraphDownloadRate()*0.6),GetResString(IDS_KBYTESPERSEC));	m_menuDownloadCtrl.AppendMenu(MF_STRING|MF_POPUP, MP_QS_D60,  text);
		text.Format(_T("80%%\t%i %s"),  (uint16)(thePrefs.GetMaxGraphDownloadRate()*0.8),GetResString(IDS_KBYTESPERSEC));	m_menuDownloadCtrl.AppendMenu(MF_STRING|MF_POPUP, MP_QS_D80,  text);
		text.Format(_T("100%%\t%i %s"), (uint16)(thePrefs.GetMaxGraphDownloadRate()),GetResString(IDS_KBYTESPERSEC));		m_menuDownloadCtrl.AppendMenu(MF_STRING|MF_POPUP, MP_QS_D100, text);
		//m_menuDownloadCtrl.AppendMenu(MF_SEPARATOR);
		//m_menuDownloadCtrl.AppendMenu(MF_STRING, MP_QS_DC, GetResString(IDS_PW_UNLIMITED));

		// Show DownloadPopup Menu
		text.Format(_T("%s:"), GetResString(IDS_PW_DOWNL));
		addToMenu->AppendMenu(MF_STRING|MF_POPUP, (UINT_PTR)m_menuDownloadCtrl.m_hMenu, text);
	}
	addToMenu->AppendMenu(MF_SEPARATOR);
	addToMenu->AppendMenu(MF_STRING, MP_CONNECT, GetResString(IDS_MAIN_BTN_CONNECT));
	addToMenu->AppendMenu(MF_STRING, MP_DISCONNECT, GetResString(IDS_MAIN_BTN_DISCONNECT)); 
}

void CemuleDlg::StartConnection(){
	if (!Kademlia::CKademlia::isRunning() ||
		(!theApp.serverconnect->IsConnecting() && !theApp.serverconnect->IsConnected() )
		){

		AddLogLine(true, GetResString(IDS_CONNECTING));

		// ed2k
		if( thePrefs.GetNetworkED2K() && 
			!theApp.serverconnect->IsConnecting() && !theApp.serverconnect->IsConnected() ){

			if ( serverwnd->serverlistctrl.GetSelectedCount()>1 )
			{
				serverwnd->serverlistctrl.PostMessage(WM_COMMAND,MP_CONNECTTO,0L);
			}
			else
			{
				theApp.serverconnect->ConnectToAnyServer();
			}
		}
		
		// kad
		if( thePrefs.GetNetworkKademlia() && !Kademlia::CKademlia::isRunning())
		{
			Kademlia::CKademlia::start();
		}


		ShowConnectionState();
	}
}

void CemuleDlg::CloseConnection()
{
	if (theApp.serverconnect->IsConnected()){
		theApp.serverconnect->Disconnect();
	}

	if (theApp.serverconnect->IsConnecting()){
		theApp.serverconnect->StopConnectionTry();
	}
	Kademlia::CKademlia::stop();
	theApp.OnlineSig(); // Added By Bouc7 
	ShowConnectionState();
}

void CemuleDlg::RestoreWindow()
{
	if (IsPreferencesDlgOpen()) {
		MessageBeep(MB_OK);
		preferenceswnd->SetForegroundWindow();
		preferenceswnd->BringWindowToTop();
		return;
	}
	if (TrayIsVisible())
		TrayHide();

	DestroyMiniMule();

	if (m_wpFirstRestore.length)
	{
		SetWindowPlacement(&m_wpFirstRestore);
//==> Optimizer [shadow2004]
#ifdef OPTIM
		memzero(&m_wpFirstRestore, sizeof m_wpFirstRestore);
#else
		memset(&m_wpFirstRestore, 0, sizeof m_wpFirstRestore);
#endif
//<== Optimizer [shadow2004]
		SetForegroundWindow();
		BringWindowToTop();
	}
	else
		CTrayDialog::RestoreWindow();
}

void CemuleDlg::UpdateTrayIcon(int iPercent)
{
	// compute an id of the icon to be generated
	UINT uSysTrayIconCookie = (iPercent > 0) ? (16 - ((iPercent*15/100) + 1)) : 0;
	if (theApp.IsConnected()) {
		if (!theApp.IsFirewalled())
			uSysTrayIconCookie += 50;
	}
	else
		uSysTrayIconCookie += 100;
	
	// dont update if the same icon as displayed would be generated
	if (m_uLastSysTrayIconCookie == uSysTrayIconCookie)
		return;
	m_uLastSysTrayIconCookie = uSysTrayIconCookie;

	// prepare it up
	if (theApp.IsConnected()) {
		if (theApp.IsFirewalled())
			m_TrayIcon.Init(m_icoSysTrayLowID, 100, 1, 1, 16, 16, thePrefs.GetStatsColor(11));
		else
			m_TrayIcon.Init(m_icoSysTrayConnected, 100, 1, 1, 16, 16, thePrefs.GetStatsColor(11));
	}
	else
		m_TrayIcon.Init(m_icoSysTrayDisconnected, 100, 1, 1, 16, 16, thePrefs.GetStatsColor(11));

	// load our limit and color info
	static const int aiLimits[1] = { 100 }; // set the limits of where the bar color changes (low-high)
	COLORREF aColors[1] = { thePrefs.GetStatsColor(11) }; // set the corresponding color for each level
	m_TrayIcon.SetColorLevels(aiLimits, aColors, ARRSIZE(aiLimits));

	// generate the icon (do *not* destroy that icon using DestroyIcon(), that's done in 'TrayUpdate')
	int aiVals[1] = { iPercent };
	m_icoSysTrayCurrent = m_TrayIcon.Create(aiVals);
	ASSERT( m_icoSysTrayCurrent != NULL );
	if (m_icoSysTrayCurrent)
		TraySetIcon(m_icoSysTrayCurrent, true);
	TrayUpdate();
}

int CemuleDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return CTrayDialog::OnCreate(lpCreateStruct);
}

void CemuleDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (IsRunning())
		ShowTransferRate(true);
	CTrayDialog::OnShowWindow(bShow, nStatus);
}

void CemuleDlg::ShowNotifier(LPCTSTR pszText, int iMsgType, LPCTSTR pszLink, bool bForceSoundOFF)
{
	if (!notifierenabled)
		return;

	LPCTSTR pszSoundEvent = NULL;
	int iSoundPrio = 0;
	bool bShowIt = false;
	switch (iMsgType)
	{
		case TBN_CHAT:
            if (thePrefs.GetNotifierOnChat())
			{
				m_wndTaskbarNotifier->Show(pszText, iMsgType, pszLink);
				bShowIt = true;
				pszSoundEvent = _T("eMule_Chat");
				iSoundPrio = 1;
			}
			break;
		case TBN_DOWNLOADFINISHED:
            if (thePrefs.GetNotifierOnDownloadFinished())
			{
				m_wndTaskbarNotifier->Show(pszText, iMsgType, pszLink);
				bShowIt = true;
				pszSoundEvent = _T("eMule_DownloadFinished");
				iSoundPrio = 1;
				SendNotificationMail(iMsgType, pszText);
			}
			break;
		case TBN_DOWNLOADADDED:
            if (thePrefs.GetNotifierOnNewDownload())
			{
				m_wndTaskbarNotifier->Show(pszText, iMsgType, pszLink);
				bShowIt = true;
				pszSoundEvent = _T("eMule_DownloadAdded");
				iSoundPrio = 1;
			}
			break;
		case TBN_LOG:
            if (thePrefs.GetNotifierOnLog())
			{
				m_wndTaskbarNotifier->Show(pszText, iMsgType, pszLink);
				bShowIt = true;
				pszSoundEvent = _T("eMule_LogEntryAdded");
			}
			break;
		case TBN_IMPORTANTEVENT:
			if (thePrefs.GetNotifierOnImportantError())
			{
				m_wndTaskbarNotifier->Show(pszText, iMsgType, pszLink);
				bShowIt = true;
				pszSoundEvent = _T("eMule_Urgent");
				iSoundPrio = 1;
				SendNotificationMail(iMsgType, pszText);
			}
			break;

		case TBN_NEWVERSION:
			if (thePrefs.GetNotifierOnNewVersion())
			{
				m_wndTaskbarNotifier->Show(pszText, iMsgType, pszLink);
				bShowIt = true;
				pszSoundEvent = _T("eMule_NewVersion");
				iSoundPrio = 1;
			}
			break;
		case TBN_NULL:
            m_wndTaskbarNotifier->Show(pszText, iMsgType, pszLink);
			bShowIt = true;
			break;
	}
	
	if (bShowIt && !bForceSoundOFF && thePrefs.GetNotifierSoundType() != ntfstNoSound)
	{
		bool bNotifiedWithAudio = false;

		if (!bNotifiedWithAudio)
		{
			if (!thePrefs.GetNotifierSoundFile().IsEmpty())
			{
				PlaySound(thePrefs.GetNotifierSoundFile(), NULL, SND_FILENAME | SND_NOSTOP | SND_NOWAIT | SND_ASYNC);
			}
			else if (pszSoundEvent)
			{
				// use 'SND_NOSTOP' only for low priority events, otherwise the 'Log message' event may overrule a more important
				// event which is fired nearly at the same time.
				PlaySound(pszSoundEvent, NULL, SND_APPLICATION | SND_ASYNC | SND_NODEFAULT | SND_NOWAIT | ((iSoundPrio > 0) ? 0 : SND_NOSTOP));
			}
		}
	}
}

void CemuleDlg::LoadNotifier(CString configuration)
{
	notifierenabled = m_wndTaskbarNotifier->LoadConfiguration(configuration)!=FALSE;
}

LRESULT CemuleDlg::OnTaskbarNotifierClicked(WPARAM wParam,LPARAM lParam)
{
	if (lParam)
	{
		LPTSTR pszLink = (LPTSTR)lParam;
		ShellOpenFile(pszLink, NULL);
		free(pszLink);
		pszLink = NULL;
	}

	switch (m_wndTaskbarNotifier->GetMessageType())
	{
		case TBN_CHAT:
			RestoreWindow();
			SetActiveDialog(chatwnd);
			break;

		case TBN_DOWNLOADFINISHED:
			// if we had a link and opened the downloaded file, dont restore the app window
			if (lParam==0)
			{
				RestoreWindow();
				SetActiveDialog(transferwnd);
			}
			break;

		case TBN_DOWNLOADADDED:
			RestoreWindow();
			SetActiveDialog(transferwnd);
			break;

		case TBN_IMPORTANTEVENT:
			RestoreWindow();
			SetActiveDialog(serverwnd);	
			break;

		case TBN_LOG:
			RestoreWindow();
			SetActiveDialog(serverwnd);	
			break;

		case TBN_NEWVERSION:
		{
			CString theUrl;
			theUrl.Format(_T("/en/version_check.php?version=%i&language=%i"),theApp.m_uCurVersionCheck,thePrefs.GetLanguageID());
			theUrl = thePrefs.GetVersionCheckBaseURL()+theUrl;
			ShellExecute(NULL, NULL, theUrl, NULL, thePrefs.GetAppDir(), SW_SHOWDEFAULT);
			break;
		}
	}
    return 0;
}

void CemuleDlg::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	theApp.UpdateDesktopColorDepth();
	CTrayDialog::OnSettingChange(uFlags, lpszSection);
}

void CemuleDlg::OnSysColorChange()
{
	theApp.UpdateDesktopColorDepth();
	CTrayDialog::OnSysColorChange();
	SetAllIcons();
}

void CemuleDlg::SetAllIcons()
{
	// application icon (although it's not customizable, we may need to load a different color resolution)
	if (m_hIcon)
		VERIFY( ::DestroyIcon(m_hIcon) );
	// NOTE: the application icon name is prefixed with "AAA" to make sure it's alphabetically sorted by the
	// resource compiler as the 1st icon in the resource table!
	m_hIcon = AfxGetApp()->LoadIcon(_T("AAAEMULEAPP"));
	SetIcon(m_hIcon, TRUE);
	// this scales the 32x32 icon down to 16x16, does not look nice at least under WinXP
	//SetIcon(m_hIcon, FALSE);

	// connection state
	for (int i = 0; i < ARRSIZE(connicons); i++){
		if (connicons[i]) VERIFY( ::DestroyIcon(connicons[i]) );
	}
	connicons[0] = theApp.LoadIcon(_T("ConnectedNotNot"), 16, 16);
	connicons[1] = theApp.LoadIcon(_T("ConnectedNotLow"), 16, 16);
	connicons[2] = theApp.LoadIcon(_T("ConnectedNotHigh"), 16, 16);
	connicons[3] = theApp.LoadIcon(_T("ConnectedLowNot"), 16, 16);
	connicons[4] = theApp.LoadIcon(_T("ConnectedLowLow"), 16, 16);
	connicons[5] = theApp.LoadIcon(_T("ConnectedLowHigh"), 16, 16);
	connicons[6] = theApp.LoadIcon(_T("ConnectedHighNot"), 16, 16);
	connicons[7] = theApp.LoadIcon(_T("ConnectedHighLow"), 16, 16);
	connicons[8] = theApp.LoadIcon(_T("ConnectedHighHigh"), 16, 16);
	ShowConnectionStateIcon();

	// transfer state
	if (transicons[0]) VERIFY( ::DestroyIcon(transicons[0]) );
	if (transicons[1]) VERIFY( ::DestroyIcon(transicons[1]) );
	if (transicons[2]) VERIFY( ::DestroyIcon(transicons[2]) );
	if (transicons[3]) VERIFY( ::DestroyIcon(transicons[3]) );
	transicons[0] = theApp.LoadIcon(_T("UP0DOWN0"), 16, 16);
	transicons[1] = theApp.LoadIcon(_T("UP0DOWN1"), 16, 16);
	transicons[2] = theApp.LoadIcon(_T("UP1DOWN0"), 16, 16);
	transicons[3] = theApp.LoadIcon(_T("UP1DOWN1"), 16, 16);
	ShowTransferStateIcon();

	// users state
	if (usericon) VERIFY( ::DestroyIcon(usericon) );
	usericon = theApp.LoadIcon(_T("StatsClients"), 16, 16);
	ShowUserStateIcon();

	// traybar icons
	if (m_icoSysTrayConnected) VERIFY( ::DestroyIcon(m_icoSysTrayConnected) );
	if (m_icoSysTrayDisconnected) VERIFY( ::DestroyIcon(m_icoSysTrayDisconnected) );
	if (m_icoSysTrayLowID) VERIFY( ::DestroyIcon(m_icoSysTrayLowID) );
	m_icoSysTrayConnected = theApp.LoadIcon(_T("TrayConnected"), 16, 16);
	m_icoSysTrayDisconnected = theApp.LoadIcon(_T("TrayNotConnected"), 16, 16);
	m_icoSysTrayLowID = theApp.LoadIcon(_T("TrayLowID"), 16, 16);
	ShowTransferRate(true);

	if (imicons[0]) VERIFY( ::DestroyIcon(imicons[0]) );
	if (imicons[1]) VERIFY( ::DestroyIcon(imicons[1]) );
	if (imicons[2]) VERIFY( ::DestroyIcon(imicons[2]) );
	imicons[0] = NULL;
	imicons[1] = theApp.LoadIcon(_T("Message"), 16, 16);
	imicons[2] = theApp.LoadIcon(_T("MessagePending"), 16, 16);
	ShowMessageState(m_iMsgIcon);
}

void CemuleDlg::Localize()
{
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu)
	{
		VERIFY( pSysMenu->ModifyMenu(MP_ABOUTBOX, MF_BYCOMMAND | MF_STRING, MP_ABOUTBOX, GetResString(IDS_ABOUTBOX)) );
		VERIFY( pSysMenu->ModifyMenu(MP_VERSIONCHECK, MF_BYCOMMAND | MF_STRING, MP_VERSIONCHECK, GetResString(IDS_VERSIONCHECK)) );

		switch (thePrefs.GetWindowsVersion()){
		case _WINVER_98_:
		case _WINVER_95_:
		case _WINVER_ME_:
			// NOTE: I think the reason why the old version of the following code crashed under Win9X was because
			// of the menus were destroyed right after they were added to the system menu. New code should work
			// under Win9X too but I can't test it.
			break;
		default:{
			// localize the 'speed control' sub menus by deleting the current menus and creating a new ones.

			// remove any already available 'speed control' menus from system menu
			UINT uOptMenuPos = pSysMenu->GetMenuItemCount() - 1;
			CMenu* pAccelMenu = pSysMenu->GetSubMenu(uOptMenuPos);
			if (pAccelMenu)
			{
				ASSERT( pAccelMenu->m_hMenu == m_SysMenuOptions.m_hMenu );
				VERIFY( pSysMenu->RemoveMenu(uOptMenuPos, MF_BYPOSITION) );
				pAccelMenu = NULL;
			}

			// destroy all 'speed control' menus
			if (m_menuUploadCtrl)
				VERIFY( m_menuUploadCtrl.DestroyMenu() );
			if (m_menuDownloadCtrl)
				VERIFY( m_menuDownloadCtrl.DestroyMenu() );
			if (m_SysMenuOptions)
				VERIFY( m_SysMenuOptions.DestroyMenu() );

			// create new 'speed control' menus
			if (m_SysMenuOptions.CreateMenu())
			{
				AddSpeedSelectorSys(&m_SysMenuOptions);
				pSysMenu->AppendMenu(MF_STRING|MF_POPUP, (UINT_PTR)m_SysMenuOptions.m_hMenu, GetResString(IDS_EM_PREFS));
			}
		  }
		}
	}

	ShowUserStateIcon();
//==> Toolbar [shadow2004]
	SetDlgItemText(IDC_BTN_KADEMLIA,GetResString(IDS_EM_KADEMLIA));
	SetDlgItemText(IDC_BTN_SERVER,GetResString(IDS_EM_SERVER));
	SetDlgItemText(IDC_BTN_TRANSFER,GetResString(IDS_EM_TRANS));
	SetDlgItemText(IDC_BTN_SEARCH,GetResString(IDS_EM_SEARCH));
	SetDlgItemText(IDC_BTN_FILES,GetResString(IDS_EM_FILES));
	SetDlgItemText(IDC_BTN_MESSAGES,GetResString(IDS_EM_MESSAGES));
	SetDlgItemText(IDC_BTN_STATISTIC,GetResString(IDS_EM_STATISTIC));
	SetDlgItemText(IDC_BTN_PREFERENCES,GetResString(IDS_EM_PREFS));
	SetDlgItemText(IDC_BTN_TOOLS,GetResString(IDS_TOOLS));
//<== Toolbar [shadow2004]
	ShowConnectionState();
	ShowTransferRate(true);
	ShowUserCount();
	CPartFileConvert::Localize();
	if (m_pMiniMule)
		m_pMiniMule->Localize();
}

void CemuleDlg::ShowUserStateIcon()
{
	statusbar->SetIcon(SBarUsers, usericon);
}

void CemuleDlg::QuickSpeedOther(UINT nID)
{
	switch (nID) {
//==> Maella [FAF] -Allow Bandwidth Settings in <1KB Incremements-
#ifdef FAF
		case MP_QS_PA: thePrefs.SetMaxUpload((float)(3));
			thePrefs.SetMaxDownload((float)(1));
			break ;
		case MP_QS_UA: 
			thePrefs.SetMaxUpload((float)(thePrefs.GetMaxGraphUploadRate(true)-2));
			thePrefs.SetMaxDownload((float)(thePrefs.GetMaxGraphDownloadRate()));
			break ;
#else
		case MP_QS_PA: thePrefs.SetMaxUpload((uint16)(1));
			thePrefs.SetMaxDownload((uint16)(1));
			break ;
		case MP_QS_UA: 
			thePrefs.SetMaxUpload((uint16)(thePrefs.GetMaxGraphUploadRate(true)));
			thePrefs.SetMaxDownload((uint16)(thePrefs.GetMaxGraphDownloadRate()));
			break ;
#endif
//<== Maella [FAF] -Allow Bandwidth Settings in <1KB Incremements-
	}
}


void CemuleDlg::QuickSpeedUpload(UINT nID)
{
	switch (nID) {
//==> Maella [FAF] -Allow Bandwidth Settings in <1KB Incremements-
#ifdef FAF
		case MP_QS_U10: thePrefs.SetMaxUpload((float)(thePrefs.GetMaxGraphUploadRate(true)*0.1)); break ;
		case MP_QS_U20: thePrefs.SetMaxUpload((float)(thePrefs.GetMaxGraphUploadRate(true)*0.2)); break ;
		case MP_QS_U30: thePrefs.SetMaxUpload((float)(thePrefs.GetMaxGraphUploadRate(true)*0.3)); break ;
		case MP_QS_U40: thePrefs.SetMaxUpload((float)(thePrefs.GetMaxGraphUploadRate(true)*0.4)); break ;
		case MP_QS_U50: thePrefs.SetMaxUpload((float)(thePrefs.GetMaxGraphUploadRate(true)*0.5)); break ;
		case MP_QS_U60: thePrefs.SetMaxUpload((float)(thePrefs.GetMaxGraphUploadRate(true)*0.6)); break ;
		case MP_QS_U70: thePrefs.SetMaxUpload((float)(thePrefs.GetMaxGraphUploadRate(true)*0.7)); break ;
		case MP_QS_U80: thePrefs.SetMaxUpload((float)(thePrefs.GetMaxGraphUploadRate(true)*0.8)); break ;
		case MP_QS_U90: thePrefs.SetMaxUpload((float)(thePrefs.GetMaxGraphUploadRate(true)*0.9)); break ;
#else
		case MP_QS_U10: thePrefs.SetMaxUpload((uint16)(thePrefs.GetMaxGraphUploadRate(true)*0.1)); break ;
		case MP_QS_U20: thePrefs.SetMaxUpload((uint16)(thePrefs.GetMaxGraphUploadRate(true)*0.2)); break ;
		case MP_QS_U30: thePrefs.SetMaxUpload((uint16)(thePrefs.GetMaxGraphUploadRate(true)*0.3)); break ;
		case MP_QS_U40: thePrefs.SetMaxUpload((uint16)(thePrefs.GetMaxGraphUploadRate(true)*0.4)); break ;
		case MP_QS_U50: thePrefs.SetMaxUpload((uint16)(thePrefs.GetMaxGraphUploadRate(true)*0.5)); break ;
		case MP_QS_U60: thePrefs.SetMaxUpload((uint16)(thePrefs.GetMaxGraphUploadRate(true)*0.6)); break ;
		case MP_QS_U70: thePrefs.SetMaxUpload((uint16)(thePrefs.GetMaxGraphUploadRate(true)*0.7)); break ;
		case MP_QS_U80: thePrefs.SetMaxUpload((uint16)(thePrefs.GetMaxGraphUploadRate(true)*0.8)); break ;
		case MP_QS_U90: thePrefs.SetMaxUpload((uint16)(thePrefs.GetMaxGraphUploadRate(true)*0.9)); break ;
#endif
//<== Maella [FAF] -Allow Bandwidth Settings in <1KB Incremements-
		case MP_QS_U100: thePrefs.SetMaxUpload(thePrefs.GetMaxGraphUploadRate(true)); break ;
//		case MP_QS_UPC: thePrefs.SetMaxUpload(UNLIMITED); break ;
		case MP_QS_UP10: thePrefs.SetMaxUpload(GetRecMaxUpload()); break ;
	}
}

void CemuleDlg::QuickSpeedDownload(UINT nID)
{
	switch (nID) {
//==> Maella [FAF] -Allow Bandwidth Settings in <1KB Incremements-
#ifdef FAF
		case MP_QS_D10: thePrefs.SetMaxDownload(0.1f * thePrefs.GetMaxGraphDownloadRate()); break ;
		case MP_QS_D20: thePrefs.SetMaxDownload(0.2f * thePrefs.GetMaxGraphDownloadRate()); break ;
		case MP_QS_D30: thePrefs.SetMaxDownload(0.3f * thePrefs.GetMaxGraphDownloadRate()); break ;
		case MP_QS_D40: thePrefs.SetMaxDownload(0.4f * thePrefs.GetMaxGraphDownloadRate()); break ;
		case MP_QS_D50: thePrefs.SetMaxDownload(0.5f * thePrefs.GetMaxGraphDownloadRate()); break ;
		case MP_QS_D60: thePrefs.SetMaxDownload(0.6f * thePrefs.GetMaxGraphDownloadRate()); break ;
		case MP_QS_D70: thePrefs.SetMaxDownload(0.7f * thePrefs.GetMaxGraphDownloadRate()); break ;
		case MP_QS_D80: thePrefs.SetMaxDownload(0.8f * thePrefs.GetMaxGraphDownloadRate()); break ;
		case MP_QS_D90: thePrefs.SetMaxDownload(0.9f * thePrefs.GetMaxGraphDownloadRate()); break ;
#else
		case MP_QS_D10: thePrefs.SetMaxDownload((uint16)(thePrefs.GetMaxGraphDownloadRate()*0.1)); break ;
		case MP_QS_D20: thePrefs.SetMaxDownload((uint16)(thePrefs.GetMaxGraphDownloadRate()*0.2)); break ;
		case MP_QS_D30: thePrefs.SetMaxDownload((uint16)(thePrefs.GetMaxGraphDownloadRate()*0.3)); break ;
		case MP_QS_D40: thePrefs.SetMaxDownload((uint16)(thePrefs.GetMaxGraphDownloadRate()*0.4)); break ;
		case MP_QS_D50: thePrefs.SetMaxDownload((uint16)(thePrefs.GetMaxGraphDownloadRate()*0.5)); break ;
		case MP_QS_D60: thePrefs.SetMaxDownload((uint16)(thePrefs.GetMaxGraphDownloadRate()*0.6)); break ;
		case MP_QS_D70: thePrefs.SetMaxDownload((uint16)(thePrefs.GetMaxGraphDownloadRate()*0.7)); break ;
		case MP_QS_D80: thePrefs.SetMaxDownload((uint16)(thePrefs.GetMaxGraphDownloadRate()*0.8)); break ;
		case MP_QS_D90: thePrefs.SetMaxDownload((uint16)(thePrefs.GetMaxGraphDownloadRate()*0.9)); break ;
#endif
//<== Maella [FAF] -Allow Bandwidth Settings in <1KB Incremements-
		case MP_QS_D100: thePrefs.SetMaxDownload(thePrefs.GetMaxGraphDownloadRate()); break ;
//		case MP_QS_DC: thePrefs.SetMaxDownload(UNLIMITED); break ;
	}
}
// quick-speed changer -- based on xrmb

//==> Maella [FAF] -Allow Bandwidth Settings in <1KB Incremements-
#ifdef FAF
float CemuleDlg::GetRecMaxUpload() {
	
	if (thePrefs.GetMaxGraphUploadRate(true)<7) return 0;
	if (thePrefs.GetMaxGraphUploadRate(true)<15) return thePrefs.GetMaxGraphUploadRate(true)-1.5f;
	return (thePrefs.GetMaxGraphUploadRate(true)-2.5f);

}
#else
int CemuleDlg::GetRecMaxUpload() {
	
	if (thePrefs.GetMaxGraphUploadRate(true)<7) return 0;
	if (thePrefs.GetMaxGraphUploadRate(true)<15) return thePrefs.GetMaxGraphUploadRate(true)-3;
	return (thePrefs.GetMaxGraphUploadRate(true)-4);

}
#endif
//<== Maella [FAF] -Allow Bandwidth Settings in <1KB Incremements-

BOOL CemuleDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{	
//		case TBBTN_CONNECT:
//==> Toolbar [shadow2004]
//			OnBnClickedButton2();
			break;
		case MP_HM_KAD:
//		case TBBTN_KAD:
			SetActiveDialog(kademliawnd);
			break;
//		case TBBTN_SERVER:
		case MP_HM_SRVR:
			SetActiveDialog(serverwnd);
			break;
//		case TBBTN_TRANSFERS:
		case MP_HM_TRANSFER:
			SetActiveDialog(transferwnd);
			break;
//		case TBBTN_SEARCH:
		case MP_HM_SEARCH:
			SetActiveDialog(searchwnd);
			break;
//		case TBBTN_SHARED:
		case MP_HM_FILES:
			SetActiveDialog(sharedfileswnd);
			break;
//		case TBBTN_MESSAGES:
		case MP_HM_MSGS:
			SetActiveDialog(chatwnd);
			break;
//		case TBBTN_STATS:
		case MP_HM_STATS:
			SetActiveDialog(statisticswnd);
			break;
//		case TBBTN_OPTIONS:
		case MP_HM_PREFS:
//==> Toolbar [shadow2004]
//			toolbar->CheckButton(TBBTN_OPTIONS, TRUE);
			ShowPreferences();
//			toolbar->CheckButton(TBBTN_OPTIONS, FALSE);
			break;
//		case TBBTN_TOOLS:
//			ShowToolPopup(true);
//			break;
		case MP_HM_OPENINC:
			ShellExecute(NULL, _T("open"), thePrefs.GetIncomingDir(),NULL, NULL, SW_SHOW); 
			break;
		case MP_HM_HELP:
//		case TBBTN_HELP:
			wParam = ID_HELP;
			break;
		case MP_HM_CON:
//==> Toolbar [shadow2004]
//			OnBnClickedButton2();
			break;
		case MP_HM_EXIT:
			OnClose();
			break;
		case MP_HM_LINK1: // MOD: dont remove!
			ShellExecute(NULL, NULL, thePrefs.GetHomepageBaseURL(), NULL, thePrefs.GetAppDir(), SW_SHOWDEFAULT);
			break;
		case MP_HM_LINK2:
			ShellExecute(NULL, NULL, thePrefs.GetHomepageBaseURL()+ CString(_T("/faq/")), NULL, thePrefs.GetAppDir(), SW_SHOWDEFAULT);
			break;
		case MP_HM_LINK3: {
			CString theUrl;
			theUrl.Format( thePrefs.GetVersionCheckBaseURL() + CString(_T("/en/version_check.php?version=%i&language=%i")),theApp.m_uCurVersionCheck,thePrefs.GetLanguageID());
			ShellExecute(NULL, NULL, theUrl, NULL, thePrefs.GetAppDir(), SW_SHOWDEFAULT);
			break;
		}
		case MP_HM_CONVERTPF:
			CPartFileConvert::ShowGUI();
			break;
		case MP_HM_SCHEDONOFF:
			thePrefs.SetSchedulerEnabled(!thePrefs.IsSchedulerEnabled());
			theApp.scheduler->Check(true);
			break;
		case MP_HM_1STSWIZARD:
			extern BOOL FirstTimeWizard();
			if (FirstTimeWizard()){
				// start connection wizard
				CConnectionWizardDlg conWizard;
				conWizard.DoModal();
			}
			break;
		case MP_HM_IPFILTER:{
			CIPFilterDlg dlg;
			dlg.DoModal();
			break;
		}
		case MP_HM_DIRECT_DOWNLOAD:{
			CDirectDownloadDlg dlg;
			dlg.DoModal();
			break;
		}
	}	
        if (wParam>=MP_SCHACTIONS && wParam<=MP_SCHACTIONS+99) {
		theApp.scheduler->ActivateSchedule(wParam-MP_SCHACTIONS);
		theApp.scheduler->SaveOriginals(); // use the new settings as original
	}

	return CTrayDialog::OnCommand(wParam, lParam);
}

LRESULT CemuleDlg::OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu)
{
//==> Toolbar [shadow2004]
/*	UINT nCmdID;
	if (toolbar->MapAccelerator(nChar, &nCmdID)){
		OnCommand(nCmdID, 0);
		return MAKELONG(0,MNC_CLOSE);
	}*/
//<== Toolbar [shadow2004]
	return CTrayDialog::OnMenuChar(nChar, nFlags, pMenu);
}

// Barry - To find out if app is running or shutting/shut down
bool CemuleDlg::IsRunning()
{
	return (theApp.m_app_state == APP_STATE_RUNNING);
}


void CemuleDlg::OnBnClickedHotmenu()
{
	ShowToolPopup(false);
}

void CemuleDlg::ShowToolPopup(bool toolsonly)
{
	POINT point;
	::GetCursorPos(&point);

	CTitleMenu menu;
	menu.CreatePopupMenu();
	if (!toolsonly)
		menu.AddMenuTitle(GetResString(IDS_HOTMENU), true);
	else
		menu.AddMenuTitle(GetResString(IDS_TOOLS), true);

	CTitleMenu Links;
	Links.CreateMenu();
	Links.AddMenuTitle(NULL, true);
	Links.AppendMenu(MF_STRING, MP_HM_LINK1, GetResString(IDS_HM_LINKHP), _T("WEB"));
	Links.AppendMenu(MF_STRING, MP_HM_LINK2, GetResString(IDS_HM_LINKFAQ), _T("WEB"));
	Links.AppendMenu(MF_STRING, MP_HM_LINK3, GetResString(IDS_HM_LINKVC), _T("WEB"));

	CMenu scheduler;
	scheduler.CreateMenu();
	CString schedonoff= (!thePrefs.IsSchedulerEnabled())?GetResString(IDS_HM_SCHED_ON):GetResString(IDS_HM_SCHED_OFF);

	scheduler.AppendMenu(MF_STRING,MP_HM_SCHEDONOFF, schedonoff);
	if (theApp.scheduler->GetCount()>0) {
		scheduler.AppendMenu(MF_SEPARATOR);
		for (int i=0; i<theApp.scheduler->GetCount();i++)
			scheduler.AppendMenu(MF_STRING,MP_SCHACTIONS+i, theApp.scheduler->GetSchedule(i)->title );
	}

	if (!toolsonly) {
		if (theApp.serverconnect->IsConnected())
			menu.AppendMenu(MF_STRING,MP_HM_CON,GetResString(IDS_MAIN_BTN_DISCONNECT), _T("DISCONNECT"));
		else if (theApp.serverconnect->IsConnecting())
			menu.AppendMenu(MF_STRING,MP_HM_CON,GetResString(IDS_MAIN_BTN_CANCEL), _T("STOPCONNECTING"));
		else
			menu.AppendMenu(MF_STRING,MP_HM_CON,GetResString(IDS_MAIN_BTN_CONNECT), _T("CONNECT"));

		menu.AppendMenu(MF_STRING,MP_HM_KAD, GetResString(IDS_EM_KADEMLIA), _T("KADEMLIA") );
		menu.AppendMenu(MF_STRING,MP_HM_SRVR, GetResString(IDS_EM_SERVER), _T("SERVER") );
		menu.AppendMenu(MF_STRING,MP_HM_TRANSFER, GetResString(IDS_EM_TRANS),_T("TRANSFER") );
		menu.AppendMenu(MF_STRING,MP_HM_SEARCH, GetResString(IDS_EM_SEARCH), _T("SEARCH"));
		menu.AppendMenu(MF_STRING,MP_HM_FILES, GetResString(IDS_EM_FILES), _T("SharedFiles"));
		menu.AppendMenu(MF_STRING,MP_HM_MSGS, GetResString(IDS_EM_MESSAGES), _T("MESSAGES"));
		menu.AppendMenu(MF_STRING,MP_HM_STATS, GetResString(IDS_EM_STATISTIC), _T("STATISTICS"));
		menu.AppendMenu(MF_STRING,MP_HM_PREFS, GetResString(IDS_EM_PREFS), _T("PREFERENCES"));
		menu.AppendMenu(MF_STRING,MP_HM_HELP, GetResString(IDS_EM_HELP), _T("HELP"));
		menu.AppendMenu(MF_SEPARATOR);
	}

	menu.AppendMenu(MF_STRING,MP_HM_OPENINC, GetResString(IDS_OPENINC) + _T("..."), _T("OPENFOLDER"));
	menu.AppendMenu(MF_STRING,MP_HM_CONVERTPF, GetResString(IDS_IMPORTSPLPF) + _T("..."), _T("CONVERT"));
	menu.AppendMenu(MF_STRING,MP_HM_1STSWIZARD, GetResString(IDS_WIZ1) + _T("..."), _T("WIZARD"));
	menu.AppendMenu(MF_STRING,MP_HM_IPFILTER, GetResString(IDS_IPFILTER) + _T("..."), _T("IPFILTER"));
	menu.AppendMenu(MF_STRING,MP_HM_DIRECT_DOWNLOAD, GetResString(IDS_SW_DIRECTDOWNLOAD) + _T("..."), _T("PASTELINK"));

	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING|MF_POPUP,(UINT_PTR)Links.m_hMenu, GetResString(IDS_LINKS), _T("WEB") );
	menu.AppendMenu(MF_STRING|MF_POPUP,(UINT_PTR)scheduler.m_hMenu, GetResString(IDS_SCHEDULER), _T("SCHEDULER") );

	if (!toolsonly) {
		menu.AppendMenu(MF_SEPARATOR);
		menu.AppendMenu(MF_STRING,MP_HM_EXIT, GetResString(IDS_EXIT), _T("EXIT"));
	}
	menu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);
	VERIFY( Links.DestroyMenu() );
	VERIFY( scheduler.DestroyMenu() );
	VERIFY( menu.DestroyMenu() );
}


void CemuleDlg::ApplyHyperTextFont(LPLOGFONT plf)
{
	theApp.m_fontHyperText.DeleteObject();
	if (theApp.m_fontHyperText.CreateFontIndirect(plf))
	{
		thePrefs.SetHyperTextFont(plf);
		serverwnd->servermsgbox->SetFont(&theApp.m_fontHyperText);
		chatwnd->chatselector.UpdateFonts(&theApp.m_fontHyperText);
	}
}

void CemuleDlg::ApplyLogFont(LPLOGFONT plf)
{
	theApp.m_fontLog.DeleteObject();
	if (theApp.m_fontLog.CreateFontIndirect(plf))
	{
		thePrefs.SetLogFont(plf);
		serverwnd->logbox->SetFont(&theApp.m_fontLog);
		serverwnd->debuglog->SetFont(&theApp.m_fontLog);
//==>Anti-Leecher-Log [cyrex2001]
#ifdef ANTI_LEECHER_LOG
		serverwnd->leecherlog->SetFont(&theApp.m_fontLog);
#endif
//<== Anti-Leecher-Log [cyrex2001]
	}
}

LRESULT CemuleDlg::OnFrameGrabFinished(WPARAM wParam,LPARAM lParam){
	CKnownFile* pOwner = (CKnownFile*)wParam;
	FrameGrabResult_Struct* result = (FrameGrabResult_Struct*)lParam;
	
	if (theApp.knownfiles->IsKnownFile(pOwner) || theApp.downloadqueue->IsPartFile(pOwner) ){
		pOwner->GrabbingFinished(result->imgResults,result->nImagesGrabbed, result->pSender);
	}
	else{
		ASSERT ( false );
	}

	delete result;
	return 0;
}

void StraightWindowStyles(CWnd* pWnd)
{
	CWnd* pWndChild = pWnd->GetWindow(GW_CHILD);
	while (pWndChild)
	{
		StraightWindowStyles(pWndChild);
		pWndChild = pWndChild->GetNextWindow();
	}

	CHAR szClassName[MAX_PATH];
	if (::GetClassNameA(*pWnd, szClassName, ARRSIZE(szClassName)))
	{
		bool bButton = (__ascii_stricmp(szClassName, "Button") == 0);

		if (   (__ascii_stricmp(szClassName, "EDIT") == 0 && (pWnd->GetExStyle() & WS_EX_STATICEDGE))
			|| __ascii_stricmp(szClassName, "SysListView32") == 0
			|| __ascii_stricmp(szClassName, "msctls_trackbar32") == 0
			)
		{
			pWnd->ModifyStyleEx(WS_EX_STATICEDGE, WS_EX_CLIENTEDGE);
		}

		if (bButton)
			pWnd->ModifyStyle(BS_FLAT, 0);
	}
}

void FlatWindowStyles(CWnd* pWnd)
{
	CWnd* pWndChild = pWnd->GetWindow(GW_CHILD);
	while (pWndChild)
	{
		FlatWindowStyles(pWndChild);
		pWndChild = pWndChild->GetNextWindow();
	}

	CHAR szClassName[MAX_PATH];
	if (::GetClassNameA(*pWnd, szClassName, ARRSIZE(szClassName)))
	{
		bool bButton = (__ascii_stricmp(szClassName, "Button") == 0);

//		if (   !bButton
//			//|| (__ascii_stricmp(szClassName, "SysListView32") == 0 && (pWnd->GetStyle() & WS_BORDER) == 0)
//			|| __ascii_stricmp(szClassName, "msctls_trackbar32") == 0
//			)
//		{
//			pWnd->ModifyStyleEx(WS_EX_CLIENTEDGE, WS_EX_STATICEDGE);
//		}

		if (bButton)
			pWnd->ModifyStyle(0, BS_FLAT);
	}
}

void InitWindowStyles(CWnd* pWnd)
{
	if (thePrefs.GetStraightWindowStyles() < 0)
		return;
	else if (thePrefs.GetStraightWindowStyles() > 0)
		StraightWindowStyles(pWnd);
	else
		FlatWindowStyles(pWnd);
}

LRESULT CemuleDlg::OnVersionCheckResponse(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETASYNCERROR(lParam) == 0)
	{
		int iBufLen = WSAGETASYNCBUFLEN(lParam);
		if (iBufLen >= sizeof(HOSTENT))
		{
			LPHOSTENT pHost = (LPHOSTENT)m_acVCDNSBuffer;
			if (pHost->h_length == 4 && pHost->h_addr_list && pHost->h_addr_list[0])
			{
				uint32 dwResult = ((LPIN_ADDR)(pHost->h_addr_list[0]))->s_addr;		
				// last byte contains informations about mirror urls, to avoid effects of future DDoS Attacks against eMules Homepage
				thePrefs.SetWebMirrorAlertLevel((uint8)(dwResult >> 24));
				uint8 abyCurVer[4] = { CemuleApp::m_nVersionBld + 1, CemuleApp::m_nVersionUpd, CemuleApp::m_nVersionMin, 0};
				dwResult &= 0x00FFFFFF;
				if (dwResult > *(uint32*)abyCurVer){
					thePrefs.UpdateLastVC();
					SetActiveWindow();
					Log(LOG_SUCCESS|LOG_STATUSBAR,GetResString(IDS_NEWVERSIONAVL));
					ShowNotifier(GetResString(IDS_NEWVERSIONAVLPOPUP), TBN_NEWVERSION);
					if (!thePrefs.GetNotifierOnNewVersion()){
						if (AfxMessageBox(GetResString(IDS_NEWVERSIONAVL)+GetResString(IDS_VISITVERSIONCHECK),MB_YESNO)==IDYES) {
							CString theUrl;
							theUrl.Format(_T("/en/version_check.php?version=%i&language=%i"),theApp.m_uCurVersionCheck,thePrefs.GetLanguageID());
							theUrl = thePrefs.GetVersionCheckBaseURL()+theUrl;
							ShellExecute(NULL, NULL, theUrl, NULL, thePrefs.GetAppDir(), SW_SHOWDEFAULT);
						}
					}
				}
				else{
					thePrefs.UpdateLastVC();
					AddLogLine(true,GetResString(IDS_NONEWERVERSION));
				}
				return 0;
			}
		}
	}
	LogWarning(LOG_STATUSBAR,GetResString(IDS_NEWVERSIONFAILED));
	return 0;
}

//==> SplashScreen [Xman]
void CemuleDlg::ShowSplash()
{
	ASSERT( m_pSplashWnd == NULL );
	if (m_pSplashWnd == NULL)
	{
		m_pSplashWnd = new CSplashScreenEx();
		if (m_pSplashWnd != NULL)
		{
			ASSERT(m_hWnd);

			if (m_pSplashWnd->Create(this,_T("eMule ") + theApp.m_strCurVersionLong,0,CSS_FADE | CSS_CENTERSCREEN | CSS_SHADOW))
			{
				m_pSplashWnd->SetBitmap(IDB_SPLASH,0,255,0);
				m_pSplashWnd->SetTextFont(_T("Tahoma"),100,CSS_TEXT_BOLD);
				//m_pSplashWnd->SetTextRect(CRect(230,150,340,185));
				CRect x=CRect(230,150,340,185); //fix [cyrex2001]
				m_pSplashWnd->SetTextRect(x); //fix [cyrex2001]
				m_pSplashWnd->SetTextColor(RGB(252,191,111));
				m_pSplashWnd->SetTextFormat(DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				m_pSplashWnd->Show();
				Sleep(1000);
				m_dwSplashTime = ::GetCurrentTime();
				m_pSplashWnd->SetText(theApp.m_strModVersion);
			}
			else
			{
				delete m_pSplashWnd;
				m_pSplashWnd = NULL;
			}
		}
	}
}
//<== SplashScreen [Xman]

void CemuleDlg::DestroySplash()
{
	if (m_pSplashWnd != NULL)
	{
		m_pSplashWnd->DestroyWindow();
		delete m_pSplashWnd;
		m_pSplashWnd = NULL;
	}
}

LRESULT CemuleDlg::OnKickIdle(UINT nWhy, long lIdleCount)
{
	LRESULT lResult = 0;

	if (m_pSplashWnd)
	{
		if (::GetCurrentTime() - m_dwSplashTime > 2500)
		{
			// timeout expired, destroy the splash window
			DestroySplash();
			UpdateWindow();
		}
		else
		{
			// check again later...
			lResult = 1;
		}
	}

	if (m_bStartMinimized)
		PostStartupMinimized();

	if (searchwnd && searchwnd->m_hWnd)
	{
		if (theApp.m_app_state != APP_STATE_SHUTINGDOWN)
		{
			//extern void Mfc_IdleUpdateCmdUiTopLevelFrameList(CWnd* pMainFrame);
			//Mfc_IdleUpdateCmdUiTopLevelFrameList(this);
			theApp.OnIdle(0/*lIdleCount*/);	// NOTE: DO **NOT** CALL THIS WITH 'lIdleCount>0'

#ifdef _DEBUG
			// We really should call this to free up the temporary object maps from MFC.
			// It may/will show bugs (wrong usage of temp. MFC data) on couple of (hidden) places,
			// therefore it's right now too dangerous to put this in 'Release' builds..
			// ---
			// The Microsoft Foundation Class (MFC) Libraries create temporary objects that are 
			// used inside of message handler functions. In MFC applications, these temporary 
			// objects are automatically cleaned up in the CWinApp::OnIdle() function that is 
			// called in between processing messages.

			// To slow to be called on each KickIdle. Need a timer
			//extern void Mfc_IdleFreeTempMaps();
			//if (lIdleCount >= 0)
			//	Mfc_IdleFreeTempMaps();
#endif
		}
	}

	return lResult;
}

BOOL CemuleDlg::PreTranslateMessage(MSG* pMsg)
{
	BOOL bResult = CTrayDialog::PreTranslateMessage(pMsg);

	if (m_pSplashWnd && m_pSplashWnd->m_hWnd != NULL &&
		(pMsg->message == WM_KEYDOWN	   ||
		 pMsg->message == WM_SYSKEYDOWN	   ||
		 pMsg->message == WM_LBUTTONDOWN   ||
		 pMsg->message == WM_RBUTTONDOWN   ||
		 pMsg->message == WM_MBUTTONDOWN   ||
		 pMsg->message == WM_NCLBUTTONDOWN ||
		 pMsg->message == WM_NCRBUTTONDOWN ||
		 pMsg->message == WM_NCMBUTTONDOWN))
	{
		DestroySplash();
		UpdateWindow();
	} 
	return bResult;
}

void CemuleDlg::HtmlHelp(DWORD_PTR dwData, UINT nCmd)
{
	CWinApp* pApp = AfxGetApp();
	ASSERT_VALID(pApp);
	ASSERT(pApp->m_pszHelpFilePath != NULL);
	// to call HtmlHelp the m_fUseHtmlHelp must be set in
	// the application's constructor
	ASSERT(pApp->m_eHelpType == afxHTMLHelp);

	CWaitCursor wait;

	PrepareForHelp();

	// need to use top level parent (for the case where m_hWnd is in DLL)
	CWnd* pWnd = GetTopLevelParent();

	TRACE(traceAppMsg, 0, _T("HtmlHelp: pszHelpFile = '%s', dwData: $%lx, fuCommand: %d.\n"), pApp->m_pszHelpFilePath, dwData, nCmd);

	bool bHelpError = false;
	CString strHelpError;
	int iTry = 0;
	while (iTry++ < 2)
	{
		if (!AfxHtmlHelp(pWnd->m_hWnd, pApp->m_pszHelpFilePath, nCmd, dwData))
		{
			bHelpError = true;
			strHelpError.LoadString(AFX_IDP_FAILED_TO_LAUNCH_HELP);

			typedef struct tagHH_LAST_ERROR
			{
				int      cbStruct;
				HRESULT  hr;
				BSTR     description;
			} HH_LAST_ERROR;
			HH_LAST_ERROR hhLastError = {0};
			hhLastError.cbStruct = sizeof hhLastError;
			HWND hwndResult = AfxHtmlHelp(pWnd->m_hWnd, NULL, HH_GET_LAST_ERROR, reinterpret_cast<DWORD>(&hhLastError));
			if (hwndResult != 0)
			{
				if (FAILED(hhLastError.hr))
				{
					if (hhLastError.description)
					{
						USES_CONVERSION;
						strHelpError = OLE2T(hhLastError.description);
						::SysFreeString(hhLastError.description);
					}
					if (   hhLastError.hr == 0x8004020A  /*no topics IDs available in Help file*/
						|| hhLastError.hr == 0x8004020B) /*requested Help topic ID not found*/
					{
						// try opening once again without help topic ID
						if (nCmd != HH_DISPLAY_TOC)
						{
							nCmd = HH_DISPLAY_TOC;
							dwData = 0;
							continue;
						}
					}
				}
			}
			break;
		}
		else
		{
			bHelpError = false;
			strHelpError.Empty();
			break;
		}
	}

	if (bHelpError)
	{
		if (AfxMessageBox(CString(pApp->m_pszHelpFilePath) + _T("\n\n") + strHelpError + _T("\n\n") + GetResString(IDS_ERR_NOHELP), MB_YESNO | MB_ICONERROR) == IDYES)
		{
			CString strUrl = thePrefs.GetHomepageBaseURL() + _T("/home/perl/help.cgi");
			ShellExecute(NULL, NULL, strUrl, NULL, thePrefs.GetAppDir(), SW_SHOWDEFAULT);
		}
	}
}

LRESULT CemuleDlg::OnPeerCacheResponse(WPARAM wParam, LPARAM lParam)
{
	return theApp.m_pPeerCache->OnPeerCacheCheckResponse(wParam,lParam);
}

BOOL CemuleDlg::OnChevronPushed(UINT id, NMHDR* pNMHDR, LRESULT* plResult)
{
	if (!thePrefs.GetUseReBarToolbar())
		return FALSE;

	NMREBARCHEVRON* pnmrc = (NMREBARCHEVRON*)pNMHDR;

	ASSERT( id == AFX_IDW_REBAR );
	ASSERT( pnmrc->uBand == 0 );
	ASSERT( pnmrc->wID == 0 );

//==> Toolbar [shadow2004]
/*	// get visible area of rebar/toolbar
	CRect rcVisibleButtons;
	toolbar->GetClientRect(&rcVisibleButtons);

	// search the first toolbar button which is not fully visible
	int iButtons = toolbar->GetButtonCount();
	for (int i = 0; i < iButtons; i++)
	{
		CRect rcButton;
		toolbar->GetItemRect(i, &rcButton);

		CRect rcVisible;
		if (!rcVisible.IntersectRect(&rcVisibleButtons, &rcButton) || !EqualRect(rcButton, rcVisible))
			break;
	}*/
//<== Toolbar [shadow2004]

	// create menu for all toolbar buttons which are not (fully) visible
	BOOL bLastMenuItemIsSep = TRUE;
	CTitleMenu menu;
	menu.CreatePopupMenu();
	menu.AddMenuTitle(_T("eMule"), true);

//==> Toolbar [shadow2004]
/*	while (i < iButtons)
	{
		TCHAR szString[256];
		szString[0] = _T('\0');
		TBBUTTONINFO tbbi = {0};
		tbbi.cbSize = sizeof tbbi;
		tbbi.dwMask = TBIF_BYINDEX | TBIF_COMMAND | TBIF_STYLE | TBIF_STATE | TBIF_TEXT;
		tbbi.cchText = ARRSIZE(szString);
		tbbi.pszText = szString;
		if (toolbar->GetButtonInfo(i, &tbbi) != -1)
		{
			if (tbbi.fsStyle & TBSTYLE_SEP)
			{
				if (!bLastMenuItemIsSep)
					bLastMenuItemIsSep = menu.AppendMenu(MF_SEPARATOR, 0, (LPCTSTR)NULL);
			}
			else
			{
				if (szString[0] != _T('\0') && menu.AppendMenu(MF_STRING, tbbi.idCommand, szString ))
				{
					bLastMenuItemIsSep = FALSE;
					if (tbbi.fsState & TBSTATE_CHECKED)
						menu.CheckMenuItem(tbbi.idCommand, MF_BYCOMMAND | MF_CHECKED);
					if ((tbbi.fsState & TBSTATE_ENABLED) == 0)
						menu.EnableMenuItem(tbbi.idCommand, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
				}
			}
		}

		i++;
	}*/
//<== Toolbar [shadow2004]

	CPoint ptMenu(pnmrc->rc.left, pnmrc->rc.top);
	ClientToScreen(&ptMenu);
	*plResult = 1;
	return FALSE;
}

bool CemuleDlg::IsPreferencesDlgOpen() const
{
	return (preferenceswnd->m_hWnd != NULL);
}

int CemuleDlg::ShowPreferences(UINT uStartPageID)
{
	if (IsPreferencesDlgOpen())
	{
		preferenceswnd->SetForegroundWindow();
		preferenceswnd->BringWindowToTop();
		return -1;
	}
	else
	{
		if (uStartPageID != (UINT)-1)
			preferenceswnd->SetStartPage(uStartPageID);
		return preferenceswnd->DoModal();
	}
}



//////////////////////////////////////////////////////////////////
// Webserver related

LRESULT CemuleDlg::OnWebAddDownloads(WPARAM wParam, LPARAM lParam)
{
	CString link=CString((TCHAR*)wParam);
	if (link.GetLength()==32 && link.Left(4).CompareNoCase(_T("ed2k"))!=0) {
		uchar fileid[16];
		DecodeBase16(link.GetBuffer(),link.GetLength(),fileid,ARRSIZE(fileid));
		theApp.searchlist->AddFileToDownloadByHash(fileid,(uint8)lParam);

	} else
		theApp.AddEd2kLinksToDownload( (TCHAR*)wParam,(int)lParam);

	return 0;
}

LRESULT CemuleDlg::OnAddRemoveFriend(WPARAM wParam, LPARAM lParam)
{
	if (lParam==0) { // remove
		theApp.friendlist->RemoveFriend((CFriend*)wParam);
	} else {		// add
		theApp.friendlist->AddFriend((CUpDownClient*)wParam);
	}

	return 0;
}

LRESULT CemuleDlg::OnWebSetCatPrio(WPARAM wParam, LPARAM lParam)
{
	theApp.downloadqueue->SetCatPrio(wParam,(uint8)lParam);
	return 0;
}
LRESULT CemuleDlg::OnWebServerClearCompleted(WPARAM wParam, LPARAM lParam)
{
	if(!wParam)
	{
		int cat=(int)lParam;
		transferwnd->downloadlistctrl.ClearCompleted(cat);
	}
	else
	{
		uchar* pFileHash = reinterpret_cast<uchar*>(lParam);
		CKnownFile* file=theApp.knownfiles->FindKnownFileByID(pFileHash);
		if (file)
			transferwnd->downloadlistctrl.RemoveFile((CPartFile*)file);
		delete[] pFileHash;
	}

	return 0;
}

LRESULT CemuleDlg::OnWebServerFileRename(WPARAM wParam, LPARAM lParam)
{
	CString sNewName = ((LPCTSTR)(lParam));

	((CPartFile*)wParam)->SetFileName(sNewName);
	((CPartFile*)wParam)->SavePartFile();
	((CPartFile*)wParam)->UpdateDisplayedInfo();
	sharedfileswnd->sharedfilesctrl.UpdateFile( (CKnownFile*)((CPartFile*)wParam));

	return 0;
}

LRESULT CemuleDlg::OnWebGUIInteraction(WPARAM wParam, LPARAM lParam) {

	switch (wParam) {
		case WEBGUIIA_UPDATEMYINFO:
			serverwnd->UpdateMyInfo();
			break;
		case WEBGUIIA_WINFUNC:{
			if (thePrefs.GetWebAdminAllowedHiLevFunc())
			{
				try {
					HANDLE hToken;
					TOKEN_PRIVILEGES tkp;	// Get a token for this process.
					
					if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
						throw;
					LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
					tkp.PrivilegeCount = 1;  // one privilege to set
					tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;	// Get the shutdown privilege for this process.
					AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

					if (lParam==1) {	// shutdown
						ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0);
					} else 
					if (lParam==2) {
						ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
					}

				} catch(...)
					{
						AddLogLine(true, GetResString(IDS_WEB_REBOOT) + _T(' ') + GetResString(IDS_FAILED));
				}
			}
			else 
				AddLogLine(true, GetResString(IDS_WEB_REBOOT) + _T(' ') + GetResString(IDS_ACCESSDENIED));
			break;
		}
		case WEBGUIIA_UPD_CATTABS:
			theApp.emuledlg->transferwnd->UpdateCatTabTitles();
			break;
		case WEBGUIIA_UPD_SFUPDATE: {
			CKnownFile* kf=(CKnownFile*)lParam;
			if (kf)
				theApp.sharedfiles->UpdateFile(kf);
			}
			break;
		case WEBGUIIA_UPDATESERVER:
			serverwnd->serverlistctrl.RefreshServer((CServer*)lParam);
			break;
		case WEBGUIIA_STOPCONNECTING:
			theApp.serverconnect->StopConnectionTry();
			break;
		case WEBGUIIA_CONNECTTOSERVER: {
			CServer* server=(CServer*)lParam;
			if (server==NULL) 
				theApp.serverconnect->ConnectToAnyServer();
			else 
				theApp.serverconnect->ConnectToServer(server);
			break;
			}
		case WEBGUIIA_DISCONNECT:
			if (lParam!=2)	// !KAD
				theApp.serverconnect->Disconnect();
			if (lParam!=1)	// !ED2K
				Kademlia::CKademlia::stop();
			break;

		case WEBGUIIA_SERVER_REMOVE: {
			serverwnd->serverlistctrl.RemoveServer((CServer*)lParam);
			break;
		}
		case WEBGUIIA_SHARED_FILES_RELOAD: {
			theApp.sharedfiles->Reload();
			break;
		}
		case WEBGUIIA_ADD_TO_STATIC: {
			serverwnd->serverlistctrl.StaticServerFileAppend((CServer*)lParam);
			break;
		}
		case WEBGUIIA_REMOVE_FROM_STATIC: {
			serverwnd->serverlistctrl.StaticServerFileRemove((CServer*)lParam);
			break;
		}
		case WEBGUIIA_UPDATESERVERMETFROMURL:
			theApp.emuledlg->serverwnd->UpdateServerMetFromURL((TCHAR*)lParam);
			break;
		case WEBGUIIA_SHOWSTATISTICS:
			theApp.emuledlg->statisticswnd->ShowStatistics(lParam!=0);
			break;
		case WEBGUIIA_DELETEALLSEARCHES:
			theApp.emuledlg->searchwnd->DeleteAllSearchs();
			break;

		case WEBGUIIA_KAD_BOOTSTRAP:{
			CString dest=CString((TCHAR*)lParam);
			int pos=dest.Find(_T(':'));
			if (pos!=-1) {
				uint16 port=_tstoi(dest.Right( dest.GetLength()-pos-1));
				CString ip=dest.Left(pos);
				Kademlia::CKademlia::bootstrap(ip,port);
			}
			break;
			}
		case WEBGUIIA_KAD_START:
			Kademlia::CKademlia::start();
			break;
		case WEBGUIIA_KAD_STOP:
			Kademlia::CKademlia::stop();
			break;
		case WEBGUIIA_KAD_RCFW:
			Kademlia::CKademlia::RecheckFirewalled();
			break;
	}

	return 0;
}

//==> Toolbar [shadow2004]
BOOL CemuleDlg::OnEraseBkgnd(CDC* pDC)
{
	RECT rect;

	BITMAP bmpInfo;
	CDC dcMemory1,dcMemory2,dcMemory3;

	GetClientRect(&rect);

	m_co_ToolLeft.GetBitmap(&bmpInfo);
	dcMemory1.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap1 = dcMemory1.SelectObject(&m_co_ToolLeft);
	pDC->BitBlt(0,0, bmpInfo.bmWidth, bmpInfo.bmHeight, &dcMemory1, 0, 0, SRCCOPY);

	m_co_ToolRight.GetBitmap(&bmpInfo);
	dcMemory2.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap2 = dcMemory2.SelectObject(&m_co_ToolRight);
	pDC->BitBlt((rect.right-rect.left)-bmpInfo.bmWidth,0, bmpInfo.bmWidth, bmpInfo.bmHeight, &dcMemory2, 0, 0, SRCCOPY);

	m_co_ToolMid.GetBitmap(&bmpInfo);
	dcMemory3.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap3 = dcMemory3.SelectObject(&m_co_ToolMid);
	pDC->StretchBlt(387,0, rect.right-253-387, bmpInfo.bmHeight, &dcMemory3,0,0, bmpInfo.bmWidth, bmpInfo.bmHeight ,SRCCOPY);

	dcMemory1.SelectObject(pOldBitmap1);
	dcMemory2.SelectObject(pOldBitmap2);
	dcMemory3.SelectObject(pOldBitmap3);

  return FALSE;
}

void CemuleDlg::OnBnClickedBtnConnect()
{

	if (!theApp.IsConnected())
		//connect if not currently connected
		if (!theApp.serverconnect->IsConnecting() && !Kademlia::CKademlia::isRunning() ){
			StartConnection();
			SetDlgItemText(IDC_BTN_CONNECT,_T("Disconnect"));
			m_co_ConnectBtn.SetSkin(IDB_CONNECT_CLICK,IDB_CONNECT_CLICK,IDB_DISCONNECT_OVER,IDB_CONNECT_CLICK,0,0,0,0,0);
		}
		else {
			CloseConnection();
			SetDlgItemText(IDC_BTN_CONNECT,_T("Connect"));
			m_co_ConnectBtn.SetSkin(IDB_CONNECT_NORMAL,IDB_CONNECT_CLICK,IDB_CONNECT_OVER,IDB_CONNECT_CLICK,0,0,0,0,0);

		}
	else{
		//disconnect if currently connected
		CloseConnection();
		SetDlgItemText(IDC_BTN_CONNECT,_T("Connect"));
		m_co_ConnectBtn.SetSkin(IDB_CONNECT_NORMAL,IDB_CONNECT_CLICK,IDB_CONNECT_OVER,IDB_CONNECT_CLICK,0,0,0,0,0);

	}
}

void CemuleDlg::OnBnClickedBtnKademlia()
{
	
	m_co_ServerBtn.EnableWindow(TRUE);
	m_co_TransferBtn.EnableWindow(TRUE);
	m_co_SearchBtn.EnableWindow(TRUE);
	m_co_SharedBtn.EnableWindow(TRUE);
	m_co_MessagesBtn.EnableWindow(TRUE);
	m_co_StatisticBtn.EnableWindow(TRUE);
	m_co_PreferencesBtn.EnableWindow(TRUE);


	m_co_KademliaBtn.EnableWindow(FALSE);

	SetActiveDialog(kademliawnd);
}

void CemuleDlg::OnBnClickedBtnServer()
{
	
	m_co_KademliaBtn.EnableWindow(TRUE);
	m_co_TransferBtn.EnableWindow(TRUE);
	m_co_SearchBtn.EnableWindow(TRUE);
	m_co_SharedBtn.EnableWindow(TRUE);
	m_co_MessagesBtn.EnableWindow(TRUE);
	m_co_StatisticBtn.EnableWindow(TRUE);
	m_co_PreferencesBtn.EnableWindow(TRUE);


	m_co_ServerBtn.EnableWindow(FALSE);

	SetActiveDialog(serverwnd);
}

void CemuleDlg::OnBnClickedBtnTransfer()
{
	
	m_co_KademliaBtn.EnableWindow(TRUE);
	m_co_ServerBtn.EnableWindow(TRUE);
	m_co_SearchBtn.EnableWindow(TRUE);
	m_co_SharedBtn.EnableWindow(TRUE);
	m_co_MessagesBtn.EnableWindow(TRUE);
	m_co_StatisticBtn.EnableWindow(TRUE);
	m_co_PreferencesBtn.EnableWindow(TRUE);


	m_co_TransferBtn.EnableWindow(FALSE);
	
	SetActiveDialog(transferwnd);
}

void CemuleDlg::OnBnClickedBtnSearch()
{
	
	m_co_KademliaBtn.EnableWindow(TRUE);
	m_co_ServerBtn.EnableWindow(TRUE);
	m_co_TransferBtn.EnableWindow(TRUE);
	m_co_SharedBtn.EnableWindow(TRUE);
	m_co_MessagesBtn.EnableWindow(TRUE);
	m_co_StatisticBtn.EnableWindow(TRUE);
	m_co_PreferencesBtn.EnableWindow(TRUE);

	m_co_SearchBtn.EnableWindow(FALSE);
	
	SetActiveDialog(searchwnd);
}

void CemuleDlg::OnBnClickedBtnShared()
{
	
	m_co_KademliaBtn.EnableWindow(TRUE);
	m_co_ServerBtn.EnableWindow(TRUE);
	m_co_TransferBtn.EnableWindow(TRUE);
	m_co_SearchBtn.EnableWindow(TRUE);
	m_co_MessagesBtn.EnableWindow(TRUE);
	m_co_StatisticBtn.EnableWindow(TRUE);
	m_co_PreferencesBtn.EnableWindow(TRUE);

	m_co_SharedBtn.EnableWindow(FALSE);
	
	SetActiveDialog(sharedfileswnd);
}


void CemuleDlg::OnBnClickedBtnMessage()
{
	m_co_KademliaBtn.EnableWindow(TRUE);
	m_co_ServerBtn.EnableWindow(TRUE);
	m_co_TransferBtn.EnableWindow(TRUE);
	m_co_SearchBtn.EnableWindow(TRUE);
	m_co_SharedBtn.EnableWindow(TRUE);
	m_co_StatisticBtn.EnableWindow(TRUE);
	m_co_PreferencesBtn.EnableWindow(TRUE);

	
	m_co_MessagesBtn.EnableWindow(FALSE);

	SetActiveDialog(chatwnd);
}

void CemuleDlg::OnBnClickedBtnStatistic()
{
	m_co_KademliaBtn.EnableWindow(TRUE);
	m_co_ServerBtn.EnableWindow(TRUE);
	m_co_TransferBtn.EnableWindow(TRUE);
	m_co_SearchBtn.EnableWindow(TRUE);
	m_co_SharedBtn.EnableWindow(TRUE);
	m_co_MessagesBtn.EnableWindow(TRUE);
	m_co_PreferencesBtn.EnableWindow(TRUE);

	m_co_StatisticBtn.EnableWindow(FALSE);
		
	SetActiveDialog(statisticswnd);
}

void CemuleDlg::OnBnClickedBtnPreferences()
{
	static int iOpen = 0;
	if(!iOpen)
	{
		iOpen = 1;
		ShowPreferences();
		iOpen = 0;

	}	
	
}

void CemuleDlg::OnBnClickedBtnTools()
{
	ShowToolPopup(true);

}
//<== Toolbar [shadow2004]
