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
#pragma once
#include "TrayDialog.h"
#include "MeterIcon.h"
#include "TitleMenu.h"

//==> Toolbar [shadow2004]
#include "EnBitmap.h"
#include "NextEMF/xSkinButton.h"
//<== Toolbar [shadow2004]

namespace Kademlia {
	class CSearch;
	class CContact;
	class CEntry;
	class CUInt128;
};

class CChatWnd;
class CKademliaWnd;
class CKnownFileList; 
class CMainFrameDropTarget;
class CMuleStatusBarCtrl;
//==> Toolbar [shadow2004]
//class CMuleToolbarCtrl;
//<== Toolbar [shadow2004]
class CPreferencesDlg;
class CSearchDlg;
class CServerWnd;
class CSharedFilesWnd;
class CStatisticsDlg;
class CTaskbarNotifier;
class CTransferWnd;
struct Status;
//==> SplashScreen [Xman]
class CSplashScreenEx;
//<== SplashScreen [Xman]
class CMuleSystrayDlg;
class CMiniMule;

// emuleapp <-> emuleapp
#define OP_ED2KLINK				12000
#define OP_CLCOMMAND			12001
#define OP_COLLECTION			12002

#define	EMULE_HOTMENU_ACCEL		'x'

class CemuleDlg : public CTrayDialog
{
//==> Toolbar [shadow2004]
//	friend class CMuleToolbarCtrl;
//<== Toolbar [shadow2004]
	friend class CMiniMule;

public:
	CemuleDlg(CWnd* pParent = NULL);
	~CemuleDlg();

	enum { IDD = IDD_EMULE_DIALOG };

	bool IsRunning();
	void ShowConnectionState();
	void ShowNotifier(LPCTSTR pszText, int iMsgType, LPCTSTR pszLink = NULL, bool bForceSoundOFF = false);
	void SendNotificationMail(int iMsgType, LPCTSTR pszText);
	void ShowUserCount();
	void ShowMessageState(uint8 iconnr);
	void SetActiveDialog(CWnd* dlg);
	void ShowTransferRate(bool forceAll=false);
    void ShowPing();
	void Localize();

	// Logging
	void AddLogText(UINT uFlags, LPCTSTR pszText);
	void AddServerMessageLine(LPCTSTR pszText);
	void ResetLog();
	void ResetDebugLog();
//==>Anti-Leecher-Log [cyrex2001]
#ifdef ANTI_LEECHER_LOG
	void ResetLeecherLog();
#endif
//<== Anti-Leecher-Log [cyrex2001]
	void ResetServerInfo();
	CString GetLastLogEntry();
	CString	GetLastDebugLogEntry();
	CString	GetAllLogEntries();
	CString	GetAllDebugLogEntries();
	CString GetServerInfoText();
	CString	GetConnectionStateString();
	UINT GetConnectionStateIconIndex() const;
	CString	GetTransferRateString();
	CString	GetUpDatarateString(UINT uUpDatarate = -1);
	CString	GetDownDatarateString(UINT uDownDatarate = -1);

	void StopTimer();
	void DoVersioncheck(bool manual);
	void ApplyHyperTextFont(LPLOGFONT pFont);
	void ApplyLogFont(LPLOGFONT pFont);
	void ProcessED2KLink(LPCTSTR pszData);
	void SetStatusBarPartsSize();
	int ShowPreferences(UINT uStartPageID = (UINT)-1);
	bool IsPreferencesDlgOpen() const;

	virtual void RestoreWindow();
	virtual void HtmlHelp(DWORD_PTR dwData, UINT nCmd = 0x000F);

	CTransferWnd*	transferwnd;
	CServerWnd*		serverwnd;
	CPreferencesDlg* preferenceswnd;
	CSharedFilesWnd* sharedfileswnd;
	CSearchDlg*		searchwnd;
	CChatWnd*		chatwnd;
	CMuleStatusBarCtrl* statusbar;
	CStatisticsDlg*  statisticswnd;
	CTaskbarNotifier* m_wndTaskbarNotifier;
	CReBarCtrl		m_ctlMainTopReBar;
//==> Toolbar [shadow2004]
//	CMuleToolbarCtrl* toolbar;
//<== Toolbar [shadow2004]
	CKademliaWnd*	kademliawnd;
	CWnd*			activewnd;
	uint8			status;

//==> Toolbar [shadow2004]
	CEnBitmap m_co_ToolLeft;
	CEnBitmap m_co_ToolMid;
	CEnBitmap m_co_ToolRight;

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	
	afx_msg void OnBnClickedBtnConnect();
	afx_msg void OnBnClickedBtnKademlia();
	afx_msg void OnBnClickedBtnServer();
	afx_msg void OnBnClickedBtnTransfer();
	afx_msg void OnBnClickedBtnSearch();
	afx_msg void OnBnClickedBtnShared();
	afx_msg void OnBnClickedBtnMessage();
	afx_msg void OnBnClickedBtnStatistic();
	afx_msg void OnBnClickedBtnPreferences();
	afx_msg void OnBnClickedBtnTools();
//<== Toolbar [shadow2004]

protected:
	HICON			m_hIcon;
	bool			ready;
	bool			m_bStartMinimizedChecked;
	bool			m_bStartMinimized;
	WINDOWPLACEMENT m_wpFirstRestore;
	HICON			connicons[9];
	HICON			transicons[4];
	HICON			imicons[3];
	HICON			m_icoSysTrayCurrent;
	HICON			usericon;
	CMeterIcon		m_TrayIcon;
	HICON			m_icoSysTrayConnected;		// do not use those icons for anything else than the traybar!!!
	HICON			m_icoSysTrayDisconnected;	// do not use those icons for anything else than the traybar!!!
	HICON			m_icoSysTrayLowID;	// do not use those icons for anything else than the traybar!!!
	int				m_iMsgIcon;
	UINT			m_uLastSysTrayIconCookie;
	uint32			m_uUpDatarate;
	uint32			m_uDownDatarate;
	CImageList		imagelist;
	CTitleMenu		trayPopup;
	CMuleSystrayDlg* m_pSystrayDlg;
	CMainFrameDropTarget* m_pDropTarget;
	CMenu			m_SysMenuOptions;
	CMenu			m_menuUploadCtrl;
	CMenu			m_menuDownloadCtrl;
	char			m_acVCDNSBuffer[MAXGETHOSTSTRUCT];

	// Splash screen
//==> SplashScreen [Xman]
	CSplashScreenEx *m_pSplashWnd;
//<== SplashScreen [Xman]
	DWORD m_dwSplashTime;
	void ShowSplash();
	void DestroySplash();

	// Mini Mule
	CMiniMule* m_pMiniMule;
	void DestroyMiniMule();

	// Startup Timer
	UINT_PTR m_hTimer;
	static void CALLBACK StartupTimer(HWND hwnd, UINT uiMsg, UINT idEvent, DWORD dwTime);

	void StartConnection();
	void CloseConnection();
	void PostStartupMinimized();
	void UpdateTrayIcon(int iPercent);
	void ShowConnectionStateIcon();
	void ShowTransferStateIcon();
	void ShowUserStateIcon();
	void AddSpeedSelectorSys(CMenu* addToMenu);
//==> Maella [FAF] -Allow Bandwidth Settings in <1KB Incremements-
#ifdef FAF
	float  GetRecMaxUpload();
#else
	int  GetRecMaxUpload();
#endif
//<== Maella [FAF] -Allow Bandwidth Settings in <1KB Incremements-
	void LoadNotifier(CString configuration);
	bool notifierenabled;
	void ShowToolPopup(bool toolsonly = false);
	void SetAllIcons();
	bool CanClose();

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	virtual void OnTrayRButtonUp(CPoint pt);
	virtual void OnTrayLButtonUp(CPoint pt);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType,int cx,int cy);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedButton2();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedHotmenu();
	afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);
	afx_msg void OnSysColorChange();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnEndSession(BOOL bEnding);
	afx_msg LRESULT OnKickIdle(UINT nWhy, long lIdleCount);
	afx_msg void OnShowWindow( BOOL bShow, UINT nStatus );
	afx_msg BOOL OnChevronPushed(UINT id, NMHDR *pnm, LRESULT *pResult);

	// quick-speed changer -- based on xrmb
	afx_msg void QuickSpeedUpload(UINT nID);
	afx_msg void QuickSpeedDownload(UINT nID);
	afx_msg void QuickSpeedOther(UINT nID);
	// end of quick-speed changer
	
	afx_msg LRESULT OnTaskbarNotifierClicked(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnWMData(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnFileHashed(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnHashFailed(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnFileAllocExc(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnFileCompleted(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnFileOpProgress(WPARAM wParam,LPARAM lParam);

	//Framegrabbing
	afx_msg LRESULT OnFrameGrabFinished(WPARAM wParam,LPARAM lParam);

	afx_msg LRESULT OnAreYouEmule(WPARAM, LPARAM);

	//Webinterface
	afx_msg LRESULT OnWebGUIInteraction(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWebServerClearCompleted(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWebServerFileRename(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWebAddDownloads(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnWebSetCatPrio(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAddRemoveFriend(WPARAM wParam, LPARAM lParam);

	// VersionCheck DNS
	afx_msg LRESULT OnVersionCheckResponse(WPARAM wParam, LPARAM lParam);

	// Peercache DNS
	afx_msg LRESULT OnPeerCacheResponse(WPARAM wParam, LPARAM lParam);

	// Mini Mule
	afx_msg LRESULT OnCloseMiniMule(WPARAM wParam, LPARAM lParam);

//==> Toolbar [shadow2004]
	CxSkinButton	m_co_ConnectBtn;
	CxSkinButton	m_co_KademliaBtn;
	CxSkinButton	m_co_ServerBtn;
	CxSkinButton	m_co_TransferBtn;
	CxSkinButton	m_co_SearchBtn;
	CxSkinButton	m_co_SharedBtn;
	CxSkinButton	m_co_MessagesBtn;
	CxSkinButton	m_co_StatisticBtn;
	CxSkinButton	m_co_PreferencesBtn;
	CxSkinButton	m_co_ToolsBtn;
	CxSkinButton	m_co_HelpBtn;
//<== Toolbar [shadow2004]
};


enum EEMuleAppMsgs
{
	//thread messages
	TM_FINISHEDHASHING = WM_APP + 10,
	TM_HASHFAILED,
	TM_FRAMEGRABFINISHED,
	TM_FILEALLOCEXC,
	TM_FILECOMPLETED,
	TM_FILEOPPROGRESS
};

enum EWebinterfaceOrders
{
	WEBGUIIA_UPDATEMYINFO = 1,
	WEBGUIIA_WINFUNC,
	WEBGUIIA_UPD_CATTABS,
	WEBGUIIA_UPD_SFUPDATE,
	WEBGUIIA_UPDATESERVER,
	WEBGUIIA_STOPCONNECTING,
	WEBGUIIA_CONNECTTOSERVER,
	WEBGUIIA_DISCONNECT,
	WEBGUIIA_SERVER_REMOVE,
	WEBGUIIA_SHARED_FILES_RELOAD,
	WEBGUIIA_ADD_TO_STATIC,
	WEBGUIIA_REMOVE_FROM_STATIC,
	WEBGUIIA_UPDATESERVERMETFROMURL,
	WEBGUIIA_SHOWSTATISTICS,
	WEBGUIIA_DELETEALLSEARCHES,
	WEBGUIIA_KAD_BOOTSTRAP,
	WEBGUIIA_KAD_START,
	WEBGUIIA_KAD_STOP,
	WEBGUIIA_KAD_RCFW
};
