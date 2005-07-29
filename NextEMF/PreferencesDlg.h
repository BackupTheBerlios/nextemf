#pragma once
#include "PPgGeneral.h"
#include "PPgConnection.h"
#include "PPgServer.h"
#include "PPgDirectories.h"
#include "PPgFiles.h"
#include "PPgStats.h"
#include "PPgNotify.h"
#include "Preferences.h"
#include "PPgTweaks.h"
#include "PPgDisplay.h"
#include "PPgSecurity.h"
#include "PPgWebServer.h"
#include "PPgScheduler.h"
#if defined(_DEBUG) || defined(USE_DEBUG_DEVICE)
#include "PPgDebug.h"
#endif
#include "otherfunctions.h"
#include "TreePropSheet.h"
#include ".\NextEMF\PPgNextEMF.h"// NextEMF-Pref [cyrex2001]
//==> PPgTabControl [shadow2004]
#include ".\NextEMF\PPgConnection2.h" //Connection 2
#include ".\NextEMF\PPgConnection3.h" //Connection 3
#include ".\NextEMF\PPgSecurity2.h"   //Security 2
//<== PPgTabControl [shadow2004]

class CPreferencesDlg : public CTreePropSheet
{
	DECLARE_DYNAMIC(CPreferencesDlg)

public:
	CPreferencesDlg();
	virtual ~CPreferencesDlg();
	
	CPPgGeneral		m_wndGeneral;
	CPPgConnection	m_wndConnection;
	CPPgServer		m_wndServer;
	CPPgDirectories	m_wndDirectories;
	CPPgFiles		m_wndFiles;
	CPPgStats		m_wndStats;
	CPPgNotify		m_wndNotify;
	CPPgTweaks		m_wndTweaks;
	CPPgDisplay		m_wndDisplay;
	CPPgSecurity	m_wndSecurity;
	CPPgWebServer	m_wndWebServer;
	CPPgScheduler	m_wndScheduler;
	CPPgNextEMF		m_wndNextEMF;// NextEMF-Pref [cyrex2001]
#if defined(_DEBUG) || defined(USE_DEBUG_DEVICE)
	CPPgDebug		m_wndDebug;
#endif

//==> PPgTabControl [shadow2004]
#ifdef PPGCTRL
	CPPgConnection2		m_wndConnection2;
	CPPgConnection3		m_wndConnection3;
	CPPgSecurity2		m_wndSecurity2;
#endif
//<== PPgTabControl [shadow2004]

	void Localize();
	void SetStartPage(UINT uStartPageID);

protected:
	LPCTSTR m_pPshStartPage;
	bool m_bSaveIniFile;

	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);

//==> PPgTabControl [shadow2004]
#ifdef PPGCTRL
public:	
	void SwitchTab(int page);

/*	int ActivePageGeneral;
	int ActivePageDisplay;*/
	int ActivePageConnection;
/*	int ActivePageServer;
	int ActivePageDirectory;
	int ActivePageFiles;
	int ActivePageNotify;
	int ActivePageStats;*/
	int ActivePageSecurity;
/*	int ActivePageSheduler;
	int ActivePageWebServer;
	int	ActivePageTweaks;
	int ActivePageDebug;

	int StartPageGeneral;
	int StartPageDisplay;*/
	int StartPageConnection;
/*	int StartPageServer;
	int StartPageDirectory;
	int StartPageFiles;
	int StartPageNotify;
	int StartPageStats;*/
	int StartPageSecurity;
/*	int StartPageSheduler;
	int StartPageWebServer;
	int	StartPageTweaks;
	int StartPageDebug;*/
#endif
//<== PPgTabControl [shadow2004]
};
