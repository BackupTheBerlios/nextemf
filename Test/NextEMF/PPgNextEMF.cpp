// PPgNextEMF.cpp : implementation file
//

#include "stdafx.h"
#include "emule.h"
#include "PPgNextEMF.h"
#include "preferences.h"
#include "PreferencesDlg.h"
#include "opcodes.h"
#include "otherfunctions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW

#endif
//==>Reask sourcen after ip chnage or Quickstart or Sivka-Ban [cyrex2001]
#if defined (QUICKSTART)|| defined (SIVKA_BAN) || defined (RSAIC_SIVKA) //Reask sourcen after ip chnage or Quickstart or Sivka-Ban

IMPLEMENT_DYNAMIC(CPPgNextEMF, CPropertyPage)
BEGIN_MESSAGE_MAP(CPPgNextEMF, CPropertyPage)
	ON_WM_HSCROLL()
    ON_WM_DESTROY()
	ON_MESSAGE(WM_TREEOPTSCTRL_NOTIFY, OnTreeOptsCtrlNotify)
END_MESSAGE_MAP()


CPPgNextEMF::CPPgNextEMF()
	: CPropertyPage(CPPgNextEMF::IDD)
	, m_ctrlTreeOptions(theApp.m_iDfltImageListColorFlags)
{

	m_bInitializedTreeOpts = false;
	m_htiIsreaskSourceAfterIPChange = NULL; 
	m_htiQuickStart = NULL;
	m_htiQuickStartMaxTime = NULL;
	m_htiQuickStartMaxConnPerFive = NULL;
	m_htiQuickStartMaxConn = NULL;
	m_htiQuickStartAfterIPChange = NULL;
	m_htiSivkaAskTime = NULL;
	m_htiSivkaAskCounter = NULL;
	m_htiSivkaAskLog = NULL;
	m_htiAntiFakeRank = NULL;
//==>Anti-Leecher [cyrex2001]
#ifdef ANTI_LEECHER
	m_htiEnableAntiLeecher = NULL;
	m_htiEnableAntiBadComunity = NULL;
	m_htiEnableAntiGplBreaker = NULL;
        m_htiEnableAntiCreditHack = NULL;
#endif //Anti-Leecher
//<==Anti-Leecher [cyrex2001]
//==> Bold Download-Status [shadow2004]
#ifdef BOLDDL
	m_htiEnableDownloadInBold = NULL;
#endif //BOLDDL
//<== Bold Download-Status [shadow2004]
//==>AntiNickThief [shadow2004]
	m_htiEnableAntiNickThief = NULL;
//<==AntiNickThief [shadow2004]
//==>defeat 0-filled partsenders [shadow2004]
	m_htiEnableZeroFilledTest = NULL;
//<==defeat 0-filled partsenders [shadow2004]
//==>Drop maunal [cyrex2001]
#ifdef DROP_MANUAL
    m_htiDropSources = NULL;
    m_htiHqrBox = NULL;
#endif //Drop maunal
//<==Drop maunal [cyrex2001]
}

CPPgNextEMF::~CPPgNextEMF()
{
}

void CPPgNextEMF::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PPG_NEXTEMF_OPTS, m_ctrlTreeOptions);
	if (!m_bInitializedTreeOpts)
	{
		int iImgOpt = 8;
		int iImgSecurity = 8;
//==>Drop maunal [cyrex2001]
#ifdef DROP_MANUAL
		int iImgDrop = 8;
#endif //Drop maunal
//<==Drop maunal [cyrex2001]
		CImageList* piml = m_ctrlTreeOptions.GetImageList(TVSIL_NORMAL);
		if (piml)
		{
			iImgOpt = piml->Add(CTempIconLoader(_T("CONNECTION")));
			iImgSecurity = piml->Add(CTempIconLoader(_T("SECURITY")));
//==>Drop maunal [cyrex2001]
#ifdef DROP_MANUAL
			iImgDrop = piml->Add(CTempIconLoader(_T("DROP")));
#endif //Drop maunal
//<==Drop maunal [cyrex2001]
		}
//==> Bold Download-Status [shadow2004]
#ifdef BOLDDL
		m_htiEnableDownloadInBold = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_DOWNLOAD_IN_BOLD), TVI_ROOT, m_bEnableDownloadInBold);
#endif //BOLDDL
//<== Bold Download-Status [shadow2004]
		m_htiIsreaskSourceAfterIPChange = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_PPG_NEXTEMF_REASK_SOURCE_AFTER_IP_CHANGE_CHECK ), TVI_ROOT, m_bIsreaskSourceAfterIPChange);
		m_htiQuickStart = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_QUICK_START), TVI_ROOT, m_bQuickStart);
		m_htiQuickStartMaxTime = m_ctrlTreeOptions.InsertItem(GetResString(IDS_QUICK_START_MAX_TIME), TREEOPTSCTRLIMG_EDIT, TREEOPTSCTRLIMG_EDIT, m_htiQuickStart);
		m_ctrlTreeOptions.AddEditBox(m_htiQuickStartMaxTime, RUNTIME_CLASS(CNumTreeOptionsEdit));
		m_htiQuickStartMaxConnPerFive = m_ctrlTreeOptions.InsertItem(GetResString(IDS_QUICK_START_MAX_CONN_PER_FIVE), TREEOPTSCTRLIMG_EDIT, TREEOPTSCTRLIMG_EDIT, m_htiQuickStart);
		m_ctrlTreeOptions.AddEditBox(m_htiQuickStartMaxConnPerFive, RUNTIME_CLASS(CNumTreeOptionsEdit));
		m_htiQuickStartMaxConn = m_ctrlTreeOptions.InsertItem(GetResString(IDS_QUICK_START_MAX_CONN), TREEOPTSCTRLIMG_EDIT, TREEOPTSCTRLIMG_EDIT, m_htiQuickStart);
		m_ctrlTreeOptions.AddEditBox(m_htiQuickStartMaxConn, RUNTIME_CLASS(CNumTreeOptionsEdit));
                m_ctrlTreeOptions.Expand(m_htiQuickStart, TVE_EXPAND);
		m_htiQuickStartAfterIPChange = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_QUICK_START_AFTER_IP_CHANGE), TVI_ROOT, m_bQuickStartAfterIPChange);
		m_htiSecurity = m_ctrlTreeOptions.InsertGroup(GetResString(IDS_SECURITY), iImgSecurity, TVI_ROOT);
		m_htiSivkaAskTime = m_ctrlTreeOptions.InsertItem(GetResString(IDS_SIVKA_ASK_TIME), TREEOPTSCTRLIMG_EDIT, TREEOPTSCTRLIMG_EDIT, m_htiSecurity);
		m_ctrlTreeOptions.AddEditBox(m_htiSivkaAskTime, RUNTIME_CLASS(CNumTreeOptionsEdit));
		m_htiSivkaAskCounter = m_ctrlTreeOptions.InsertItem(GetResString(IDS_SIVKA_ASK_COUNTER), TREEOPTSCTRLIMG_EDIT, TREEOPTSCTRLIMG_EDIT, m_htiSecurity);
		m_ctrlTreeOptions.AddEditBox(m_htiSivkaAskCounter, RUNTIME_CLASS(CNumTreeOptionsEdit));
		m_htiSivkaAskLog = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_SIVKA_ASK_LOG), m_htiSecurity, m_bSivkaAskLog);
		m_htiAntiFakeRank = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_ANTI_FAKE_RANK), m_htiSecurity, m_bAntiFakeRank);
//==>Anti-Leecher [cyrex2001]
#ifdef ANTI_LEECHER
		m_htiEnableAntiLeecher = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_ANTI_LEECHER), m_htiSecurity, m_bEnableAntiLeecher);
		m_htiEnableAntiBadComunity = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_ANTI_BADCOMUNITY), m_htiSecurity, m_bEnableAntiBadComunity);
		m_htiEnableAntiGplBreaker = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_ANTI_GPLBREAKER), m_htiSecurity, m_bEnableAntiGplBreaker);
                m_htiEnableAntiCreditHack = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_ANTI_CREDITHACK), m_htiSecurity, m_bEnableAntiCreditHack);
#endif //Anti-Leecher
//<==Anti-Leecher [cyrex2001]
//==>AntiNickThief [shadow2004]
		m_htiEnableAntiNickThief = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_ANTINICKTHIEF), m_htiSecurity, m_bEnableAntiNickThief);
//<==AntiNickThief [shadow2004]
//==>defeat 0-filled partsenders [shadow2004]
		m_htiEnableZeroFilledTest = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_ZERO_FILLED_TEST), m_htiSecurity, m_bEnableZeroFilledTest);
//<==defeat 0-filled partsenders [shadow2004]

//==>Drop maunal [cyrex2001]
#ifdef DROP_MANUAL
		m_htiDropSources = m_ctrlTreeOptions.InsertGroup(GetResString(IDS_DROPS), iImgDrop, TVI_ROOT);
		m_htiHqrBox = m_ctrlTreeOptions.InsertItem(GetResString(IDS_DROPHQSLIMIT), TREEOPTSCTRLIMG_EDIT, TREEOPTSCTRLIMG_EDIT, m_htiDropSources);
		m_ctrlTreeOptions.AddEditBox(m_htiHqrBox , RUNTIME_CLASS(CNumTreeOptionsEdit));
        m_ctrlTreeOptions.Expand(m_htiDropSources, TVE_EXPAND);
#endif //Drop maunal
//<==Drop maunal [cyrex2001]

        m_ctrlTreeOptions.Expand(m_htiSecurity, TVE_EXPAND);

		m_ctrlTreeOptions.SendMessage(WM_VSCROLL, SB_TOP);
		m_bInitializedTreeOpts = true;
	}
//==> Bold Download-Status [shadow2004]
#ifdef BOLDDL
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiEnableDownloadInBold, m_bEnableDownloadInBold);
#endif //BOLDDL
//<== Bold Download-Status [shadow2004]
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiIsreaskSourceAfterIPChange, m_bIsreaskSourceAfterIPChange);
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiQuickStart, m_bQuickStart);
	DDX_TreeEdit(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiQuickStartMaxTime, m_iQuickStartMaxTime);
	DDV_MinMaxInt(pDX, m_iQuickStartMaxTime, 8, 18);
	DDX_TreeEdit(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiQuickStartMaxConnPerFive, m_iQuickStartMaxConnPerFive);
	DDV_MinMaxInt(pDX, m_iQuickStartMaxConnPerFive, 5, 200);
	DDX_TreeEdit(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiQuickStartMaxConn, m_iQuickStartMaxConn);
	DDV_MinMaxInt(pDX, m_iQuickStartMaxConn, 200, 2000);
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiQuickStartAfterIPChange, m_bQuickStartAfterIPChange);
	DDX_TreeEdit(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiSivkaAskTime, m_iSivkaAskTime);
	DDV_MinMaxInt(pDX, m_iSivkaAskTime, 5, 12);
	DDX_TreeEdit(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiSivkaAskCounter, m_iSivkaAskCounter);
	DDV_MinMaxInt(pDX, m_iSivkaAskCounter, 5, 15);
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiSivkaAskLog, m_bSivkaAskLog);
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiAntiFakeRank, m_bAntiFakeRank);
//==>Anti-Leecher [cyrex2001]
#ifdef ANTI_LEECHER
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiEnableAntiLeecher, m_bEnableAntiLeecher);
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiEnableAntiBadComunity, m_bEnableAntiBadComunity);
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiEnableAntiGplBreaker, m_bEnableAntiGplBreaker);
    DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiEnableAntiCreditHack, m_bEnableAntiCreditHack);
#endif //Anti-Leecher
//<==Anti-Leecher [cyrex2001]
//==>AntiNickThief [shadow2004]
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiEnableAntiNickThief, m_bEnableAntiNickThief);
//<==AntiNickThief [shadow2004]
//==>defeat 0-filled partsenders [shadow2004]
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiEnableZeroFilledTest, m_bEnableZeroFilledTest);
//<==defeat 0-filled partsenders [shadow2004]
//==>Drop maunal [cyrex2001]
#ifdef DROP_MANUAL
    DDX_TreeEdit(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiHqrBox, iMaxRemoveQRS);
	DDV_MinMaxInt(pDX, iMaxRemoveQRS, 2500, 100000);
#endif //Drop maunal
//<==Drop maunal [cyrex2001]
}

BOOL CPPgNextEMF::OnInitDialog()
{
//==>Reask sourcen after ip change [cyrex2001]
#ifdef RSAIC_SIVKA
	m_bIsreaskSourceAfterIPChange = thePrefs.isreaskSourceAfterIPChange;
#endif //Reask sourcen after ip change
//<==Reask sourcen after ip change [cyrex2001]
	m_bQuickStart = thePrefs.isQuickStart;
	m_iQuickStartMaxTime = (int)(thePrefs.QuickStartMaxTime);
	m_iQuickStartMaxConnPerFive = (int)(thePrefs.QuickStartMaxConnPerFive);
	m_iQuickStartMaxConn = (int)(thePrefs.QuickStartMaxConn);
	m_bQuickStartAfterIPChange = thePrefs.isQuickStartAfterIPChange;
	m_iSivkaAskTime = (int)(thePrefs.SivkaAskTime);
	m_iSivkaAskCounter = (int)(thePrefs.SivkaAskCounter);
	m_bSivkaAskLog = thePrefs.SivkaAskLog;
	m_bAntiFakeRank = thePrefs.AntiFakeRank;
//==>Anti-Leecher [cyrex2001]
#ifdef ANTI_LEECHER
	m_bEnableAntiLeecher = thePrefs.enableAntiLeecher;
	m_bEnableAntiBadComunity = thePrefs.enableAntiBadComunity;
	m_bEnableAntiGplBreaker = thePrefs.enableAntiGplBreaker;
	m_bEnableAntiCreditHack = thePrefs.enableAntiCreditHack;
#endif //Anti-Leecher
//<==Anti-Leecher [cyrex2001]
//==> Bold Download-Status [shadow2004]
#ifdef BOLDDL
	m_bEnableDownloadInBold = thePrefs.m_bShowActiveDownloadsBold;
#endif //BOLDDL
//<== Bold Download-Status [shadow2004]
//==>AntiNickThief [shadow2004]
	m_bEnableAntiNickThief = thePrefs.m_bAntiNickThief;
//<==AntiNickThief [shadow2004]
//==>defeat 0-filled partsenders [shadow2004]
	m_bEnableZeroFilledTest = thePrefs.enableZeroFilledTest;
//<==defeat 0-filled partsenders [shadow2004]
//==>Drop maunal [cyrex2001]
#ifdef DROP_MANUAL
    iMaxRemoveQRS = thePrefs.GetMaxRemoveQRS();
#endif //Drop maunal
//<==Drop maunal [cyrex2001]
	CPropertyPage::OnInitDialog();
	Localize();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPPgNextEMF::OnKillActive()
{
	// if prop page is closed by pressing ENTER we have to explicitly commit any possibly pending
	// data from an open edit control
	m_ctrlTreeOptions.HandleChildControlLosingFocus();
	return CPropertyPage::OnKillActive();
}

BOOL CPPgNextEMF::OnApply()
{
	// if prop page is closed by pressing ENTER we have to explicitly commit any possibly pending
	// data from an open edit control
	m_ctrlTreeOptions.HandleChildControlLosingFocus();
	
	if (!UpdateData())
		return FALSE;
//==>Reask sourcen after ip change [cyrex2001]
#ifdef RSAIC_SIVKA
	thePrefs.isreaskSourceAfterIPChange = m_bIsreaskSourceAfterIPChange;
#endif //Reask sourcen after ip change
//<==Reask sourcen after ip change [cyrex2001]
	thePrefs.isQuickStart = m_bQuickStart;
	thePrefs.QuickStartMaxTime = m_iQuickStartMaxTime;
	thePrefs.QuickStartMaxConnPerFive = m_iQuickStartMaxConnPerFive;
	thePrefs.QuickStartMaxConn = m_iQuickStartMaxConn;
	thePrefs.isQuickStartAfterIPChange = m_bQuickStartAfterIPChange;
	thePrefs.SivkaAskTime = m_iSivkaAskTime;
	thePrefs.SivkaAskCounter = m_iSivkaAskCounter;
	thePrefs.SivkaAskLog = m_bSivkaAskLog;
	thePrefs.AntiFakeRank = m_bAntiFakeRank;
//==>Anti-Leecher [cyrex2001]
#ifdef ANTI_LEECHER
	thePrefs.enableAntiLeecher = m_bEnableAntiLeecher;
	thePrefs.enableAntiBadComunity = m_bEnableAntiBadComunity;
	thePrefs.enableAntiGplBreaker = m_bEnableAntiGplBreaker;
	thePrefs.enableAntiCreditHack = m_bEnableAntiCreditHack;
#endif //Anti-Leecher
//<==Anti-Leecher [cyrex2001]
//==> Bold Download-Status [shadow2004]
#ifdef BOLDDL
	thePrefs.m_bShowActiveDownloadsBold = m_bEnableDownloadInBold;
#endif //BOLDDL
//<== Bold Download-Status [shadow2004]
//==>AntiNickThief [shadow2004]
	thePrefs.m_bAntiNickThief = m_bEnableAntiNickThief;
//<==AntiNickThief [shadow2004]
//==>defeat 0-filled partsenders [shadow2004]
	thePrefs.enableZeroFilledTest = m_bEnableZeroFilledTest;
//<==defeat 0-filled partsenders [shadow2004]
//==>Drop maunal [cyrex2001]
#ifdef DROP_MANUAL
	thePrefs.SetMaxRemoveQRS(iMaxRemoveQRS ? iMaxRemoveQRS : 5000);
#endif //Drop maunal
//<==Drop maunal [cyrex2001]
	SetModified(FALSE);
	return CPropertyPage::OnApply();
}

void CPPgNextEMF::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SetModified(TRUE);
	UpdateData(false); 
	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPPgNextEMF::Localize(void)
{	
	if(m_hWnd)
	{
		SetWindowText(MOD_VERSION);
		GetDlgItem(IDC_WARNING)->SetWindowText(GetResString(IDS_TWEAKS_WARNING));

		if (m_htiIsreaskSourceAfterIPChange) m_ctrlTreeOptions.SetItemText(m_htiIsreaskSourceAfterIPChange, GetResString(IDS_PPG_NEXTEMF_REASK_SOURCE_AFTER_IP_CHANGE_CHECK ));
		if (m_htiQuickStart) m_ctrlTreeOptions.SetItemText(m_htiQuickStart, GetResString(IDS_QUICK_START));
		if (m_htiQuickStartMaxTime) m_ctrlTreeOptions.SetEditLabel(m_htiQuickStartMaxTime, GetResString(IDS_QUICK_START_MAX_TIME));
		if (m_htiQuickStartMaxConnPerFive) m_ctrlTreeOptions.SetEditLabel(m_htiQuickStartMaxConnPerFive, GetResString(IDS_QUICK_START_MAX_CONN_PER_FIVE));
		if (m_htiQuickStartMaxConn) m_ctrlTreeOptions.SetEditLabel(m_htiQuickStartMaxConn, GetResString(IDS_QUICK_START_MAX_CONN));
		if (m_htiQuickStartAfterIPChange) m_ctrlTreeOptions.SetItemText(m_htiQuickStartAfterIPChange, GetResString(IDS_QUICK_START_AFTER_IP_CHANGE));
		if (m_htiSivkaAskTime) m_ctrlTreeOptions.SetEditLabel(m_htiSivkaAskTime, GetResString(IDS_SIVKA_ASK_TIME));
		if (m_htiSivkaAskCounter) m_ctrlTreeOptions.SetEditLabel(m_htiSivkaAskCounter, GetResString(IDS_SIVKA_ASK_COUNTER));
		if (m_htiSivkaAskLog) m_ctrlTreeOptions.SetItemText(m_htiSivkaAskLog, GetResString(IDS_SIVKA_ASK_LOG));
		if (m_htiAntiFakeRank) m_ctrlTreeOptions.SetItemText(m_htiAntiFakeRank, GetResString(IDS_ANTI_FAKE_RANK));
//==>Anti-Leecher [cyrex2001]
#ifdef ANTI_LEECHER
		if (m_htiEnableAntiLeecher) m_ctrlTreeOptions.SetItemText(m_htiEnableAntiLeecher, GetResString(IDS_ANTI_LEECHER));
		if (m_htiEnableAntiBadComunity) m_ctrlTreeOptions.SetItemText(m_htiEnableAntiBadComunity, GetResString(IDS_ANTI_BADCOMUNITY));
		if (m_htiEnableAntiGplBreaker) m_ctrlTreeOptions.SetItemText(m_htiEnableAntiGplBreaker, GetResString(IDS_ANTI_GPLBREAKER));
		if (m_htiEnableAntiCreditHack) m_ctrlTreeOptions.SetItemText(m_htiEnableAntiCreditHack, GetResString(IDS_ANTI_CREDITHACK));
#endif //Anti-Leecher
//<==Anti-Leecher [cyrex2001]
//==>AntiNickThief [shadow2004]
	    if (m_htiEnableAntiNickThief) m_ctrlTreeOptions.SetItemText(m_htiEnableAntiNickThief, GetResString(IDS_ANTINICKTHIEF));
//<==AntiNickThief [shadow2004]
//==>defeat 0-filled partsenders [shadow2004]
	    if (m_htiEnableZeroFilledTest) m_ctrlTreeOptions.SetItemText(m_htiEnableZeroFilledTest, GetResString(IDS_ZERO_FILLED_TEST));
//<==defeat 0-filled partsenders [shadow2004]
//==>Drop maunal [cyrex2001]
#ifdef DROP_MANUAL
		if (m_htiHqrBox) m_ctrlTreeOptions.SetEditLabel(m_htiHqrBox, GetResString(IDS_DROPHQSLIMIT));
		if (m_htiDropSources) m_ctrlTreeOptions.SetItemText(m_htiDropSources, GetResString(IDS_DROPS)); 
#endif //Drop maunal
//<==Drop maunal [cyrex2001]
	}
}

void CPPgNextEMF::OnDestroy()
{
	m_ctrlTreeOptions.DeleteAllItems();
	m_ctrlTreeOptions.DestroyWindow();
	m_bInitializedTreeOpts = false;

	m_htiIsreaskSourceAfterIPChange = NULL; 
	m_htiQuickStart = NULL;
	m_htiQuickStartMaxTime = NULL;
	m_htiQuickStartMaxConnPerFive = NULL;
	m_htiQuickStartMaxConn = NULL;
	m_htiQuickStartAfterIPChange = NULL;
	m_htiSivkaAskTime = NULL;
	m_htiSivkaAskCounter = NULL;
	m_htiSivkaAskLog = NULL;
	m_htiAntiFakeRank = NULL;
//==>Anti-Leecher [cyrex2001]
#ifdef ANTI_LEECHER
	m_htiEnableAntiLeecher = NULL;
	m_htiEnableAntiBadComunity = NULL;
	m_htiEnableAntiGplBreaker = NULL;
	m_htiEnableAntiCreditHack = NULL;
#endif //Anti-Leecher
//<==Anti-Leecher [cyrex2001]
//==>AntiNickThief [shadow2004]
	m_htiEnableAntiNickThief = NULL;
//<==AntiNickThief [shadow2004]
//==>defeat 0-filled partsenders [shadow2004]
	m_htiEnableZeroFilledTest = NULL;
//<==defeat 0-filled partsenders [shadow2004]
//==>Drop maunal [cyrex2001]
#ifdef DROP_MANUAL
    m_htiDropSources = NULL;
    m_htiHqrBox = NULL;
#endif //Drop maunal
//<==Drop maunal [cyrex2001]

	CPropertyPage::OnDestroy();
}

LRESULT CPPgNextEMF::OnTreeOptsCtrlNotify(WPARAM wParam, LPARAM lParam)
{
	if (wParam == IDC_PPG_NEXTEMF_OPTS){
		TREEOPTSCTRLNOTIFY* pton = (TREEOPTSCTRLNOTIFY*)lParam;
		SetModified();
	}
	return 0;
}
#endif //Reask sourcen after ip chnage or Quickstart or Sivka-Ban
//<==Reask sourcen after ip chnage or Quickstart or Sivka-Ban [cyrex2001]
