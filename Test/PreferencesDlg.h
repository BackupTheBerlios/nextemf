#pragma once
#include "PPgGeneral.h"
#include "PPgConnection.h"
#include "PPgServer.h"
#include "PPgDirectories.h"
#include "PPgFiles.h"
#include "PPgStats.h"
#include "PPgNotify.h"
//==> remove IRC [shadow2004]
#if defined(IRC)
#include "PPgIRC.h"
#else
  #include "Preferences.h"
#endif //IRC
//<== remove IRC [shadow2004]
#include "PPgTweaks.h"
#include "PPgDisplay.h"
#include "PPgSecurity.h"
#include "PPgWebServer.h"
#include "PPgScheduler.h"
//==> remove PROXY [shadow2004]
#if defined(PROXY)
#include "PPgProxy.h"
#endif //PROXY
//<== remove PROXY [shadow2004]
#if defined(_DEBUG) || defined(USE_DEBUG_DEVICE)
#include "PPgDebug.h"
#endif
#include "otherfunctions.h"
#include "TreePropSheet.h"

//==>Reask sourcen after ip chnage or Quickstart [cyrex2001]
#if defined (RSAIC_SIVKA) || defined (QUICKSTART) //Reask sourcen after ip chnage or Quickstart
#include ".\NextEMF\PPgNextEMF.h"
#endif //Reask sourcen after ip chnage or Quickstart
//<==Reask sourcen after ip chnage or Quickstart [cyrex2001]

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
//==> remove IRC [shadow2004]
#if defined(IRC)
	CPPgIRC			m_wndIRC;
#endif // IRC
//<== remove IRC [shadow2004]
	CPPgTweaks		m_wndTweaks;
	CPPgDisplay		m_wndDisplay;
	CPPgSecurity	m_wndSecurity;
	CPPgWebServer	m_wndWebServer;
	CPPgScheduler	m_wndScheduler;
//==> remove PROXY [shadow2004]
#if defined(PROXY)
	CPPgProxy		m_wndProxy;
#endif //PROXY
//<== remove PROXY [shadow2004]
#if defined(_DEBUG) || defined(USE_DEBUG_DEVICE)
	CPPgDebug		m_wndDebug;
#endif

//==>Reask sourcen after ip chnage or Quickstart [cyrex2001]
#if defined (RSAIC_SIVKA) || defined (QUICKSTART) //Reask sourcen after ip chnage or Quickstart
	CPPgNextEMF		m_wndNextEMF;
#endif //Reask sourcen after ip chnage or Quickstart
//<==Reask sourcen after ip chnage or Quickstart [cyrex2001]

	void Localize();

protected:
	int m_iPrevPage;
	UINT m_nActiveWnd;

	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
