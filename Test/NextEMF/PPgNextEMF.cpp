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
#if defined (QUICKSTART)|| defined (SIVKA_BAN) || defined (RSAIC_MAELLA) //Reask sourcen after ip chnage or Quickstart or Sivka-Ban

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
	m_htiSecurity = NULL;
	m_htiSivkaBanGroup = NULL;
	m_htiEnableSivkaBan = NULL;
	m_htiSivkaAskTime = NULL;
	m_htiSivkaAskCounter = NULL;
	m_htiSivkaAskLog = NULL;
	m_htiAntiFakeRank = NULL;
//==>Anti-Leecher [cyrex2001]
#ifdef ANTI_LEECHER
	m_htiEnableALF = NULL;
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
#ifdef NULLFILLED
	m_htiEnableZeroFilledTest = NULL;
#endif
//<==defeat 0-filled partsenders [shadow2004]
//==>Drop maunal [cyrex2001]
#ifdef DROP
    m_htiDropSources = NULL;
    m_htiHqrBox = NULL;
    m_htiDropTimer = NULL;
#endif //Drop maunal
//<==Drop maunal [cyrex2001]
//==>timestamp in chatwindow [shadow2004]
	m_htiEnableTimestamp = NULL; 
//<==timestamp in chatwindow [shadow2004]
//==>Lowid retry by SlugFiller [cyrex2001]
#ifdef LOWID
	m_htiLowIdRetry = NULL;
	m_iLowIdRetry = 0;
#endif //Lowid retry
//<==Lowid retry [cyrex2001]
//==> Spooky Mode [cyrex2001]
#ifdef SPOOKY // Spooky Mode [eWombat]
	m_htiSpooky = NULL;
	m_htiEnableSpookyMode = NULL;
	m_htiReconSpooky = NULL;
	m_htiSpookyFailed = NULL;
	m_htiSpookyFailedCount = NULL;
#endif // Spooky Mode [eWombat] 
//<== Spooky Mode [cyrex2001]
//==> Spooky Mode ConChecker [cyrex2001]
#ifdef CONCHECKER //>>>WiZaRd: Spooky Mode ConChecker [eWombat] 
	m_htiCheckConGroup = NULL;
	m_htiCheckCon = NULL;
	m_htiICMP = NULL;
	m_htiPingTimeOut = NULL;
	m_htiPingTTL = NULL;
#endif //<<<WiZaRd: Spooky Mode ConChecker [eWombat] 
//<== Spooky Mode ConChecker [cyrex2001]
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
#ifdef DROP
		int iImgDrop = 8;
#endif //Drop maunal
//<==Drop maunal [cyrex2001]
//==> Spooky Mode [cyrex2001]
#ifdef SPOOKY // Spooky Mode [eWombat]
		int iImgSpooky = 8;
#endif // Spooky Mode [eWombat] 
//<== Spooky Mode [cyrex2001]
		CImageList* piml = m_ctrlTreeOptions.GetImageList(TVSIL_NORMAL);
		if (piml)
		{
			iImgSecurity = piml->Add(CTempIconLoader(_T("SECURITY")));
//==>Drop maunal [cyrex2001]
#ifdef DROP
			iImgDrop = piml->Add(CTempIconLoader(_T("DROP")));
#endif //Drop maunal
//<==Drop maunal [cyrex2001]
//==> Spooky Mode [cyrex2001]
#ifdef SPOOKY // Spooky Mode [eWombat]
			iImgSpooky = piml->Add(CTempIconLoader(_T("IDI_SPOOKY")));
#endif // Spooky Mode [eWombat] 
//<== Spooky Mode [cyrex2001]
//==> Spooky Mode ConChecker [cyrex2001]
#ifdef CONCHECKER //>>>WiZaRd: Spooky Mode ConChecker [eWombat] 
			iImgOpt = piml->Add(CTempIconLoader(_T("CONNECTION")));
#endif //<<<WiZaRd: Spooky Mode ConChecker [eWombat] 
//<== Spooky Mode ConChecker [cyrex2001]
		}
//==>Lowid retry by SlugFiller [cyrex2001]
#ifdef LOWID
		m_htiLowIdRetry = m_ctrlTreeOptions.InsertItem(GetResString(IDS_RECONNECTONLOWID), TREEOPTSCTRLIMG_EDIT, TREEOPTSCTRLIMG_EDIT, TVI_ROOT);
		m_ctrlTreeOptions.AddEditBox(m_htiLowIdRetry, RUNTIME_CLASS(CNumTreeOptionsEdit));
#endif //Lowid retry
//<==Lowid retry [cyrex2001]
//==> Spooky Mode [cyrex2001]
#ifdef SPOOKY // Spooky Mode [eWombat]
		m_htiSpooky = m_ctrlTreeOptions.InsertGroup(GetResString(IDS_SPOOKY), iImgSpooky, TVI_ROOT);
		m_htiEnableSpookyMode = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_ENABLE_SPOOKY_MODE), m_htiSpooky, m_bEnableSpookyMode);
		m_htiReconSpooky = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_RECON_SPOOKY), m_htiSpooky, m_bReconSpooky);
		m_htiSpookyFailed = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_FAILED_SPOOKY), m_htiSpooky, m_bSpookyFailed);
		m_htiSpookyFailedCount = m_ctrlTreeOptions.InsertItem(GetResString(IDS_FAILED_SPOOKY_COUNT), TREEOPTSCTRLIMG_EDIT, TREEOPTSCTRLIMG_EDIT, m_htiSpooky);
		m_ctrlTreeOptions.AddEditBox(m_htiSpookyFailedCount, RUNTIME_CLASS(CNumTreeOptionsEdit));
#endif // Spooky Mode [eWombat] 
//<== Spooky Mode [cyrex2001]
//==> Spooky Mode ConChecker [cyrex2001]
#ifdef CONCHECKER //>>>WiZaRd: Spooky Mode ConChecker [eWombat] 
		m_htiCheckConGroup = m_ctrlTreeOptions.InsertGroup(GetResString(IDS_CONCHECK), iImgOpt, TVI_ROOT);
		m_htiCheckCon = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_ENABLE_CON_CHECK), m_htiCheckConGroup, m_bCheckCon);
		m_htiICMP = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_ENABLE_ICMP), m_htiCheckConGroup, m_bICMP);
		m_htiPingTimeOut = m_ctrlTreeOptions.InsertItem(GetResString(IDS_PING_TIME_OUT), TREEOPTSCTRLIMG_EDIT, TREEOPTSCTRLIMG_EDIT, m_htiCheckConGroup);
		m_ctrlTreeOptions.AddEditBox(m_htiPingTimeOut, RUNTIME_CLASS(CNumTreeOptionsEdit));
		m_htiPingTTL = m_ctrlTreeOptions.InsertItem(GetResString(IDS_PING_TLL), TREEOPTSCTRLIMG_EDIT, TREEOPTSCTRLIMG_EDIT, m_htiCheckConGroup);
		m_ctrlTreeOptions.AddEditBox(m_htiPingTTL, RUNTIME_CLASS(CNumTreeOptionsEdit));
#endif //<<<WiZaRd: Spooky Mode ConChecker [eWombat] 
//<== Spooky Mode ConChecker [cyrex2001]
//==> Bold Download-Status [shadow2004]
#ifdef BOLDDL
		m_htiEnableDownloadInBold = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_DOWNLOAD_IN_BOLD), TVI_ROOT, m_bEnableDownloadInBold);
#endif //BOLDDL
//<== Bold Download-Status [shadow2004]
//==>timestamp in chatwindow [shadow2004]
		m_htiEnableTimestamp = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_IRC_ADDTIMESTAMP), TVI_ROOT, m_bEnableTimestamp);
//<==timestamp in chatwindow [shadow2004]
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
		m_htiSivkaBanGroup = m_ctrlTreeOptions.InsertGroup(GetResString(IDS_SIVKA_BAN_CONTROL), iImgSecurity, m_htiSecurity);
		m_htiEnableSivkaBan = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_SIVKA_BAN),m_htiSivkaBanGroup, m_bEnableSivkaBan);
		m_htiSivkaAskTime = m_ctrlTreeOptions.InsertItem(GetResString(IDS_SIVKA_ASK_TIME), TREEOPTSCTRLIMG_EDIT, TREEOPTSCTRLIMG_EDIT, m_htiSivkaBanGroup);
		m_ctrlTreeOptions.AddEditBox(m_htiSivkaAskTime, RUNTIME_CLASS(CNumTreeOptionsEdit));
		m_htiSivkaAskCounter = m_ctrlTreeOptions.InsertItem(GetResString(IDS_SIVKA_ASK_COUNTER), TREEOPTSCTRLIMG_EDIT, TREEOPTSCTRLIMG_EDIT, m_htiSivkaBanGroup);
		m_ctrlTreeOptions.AddEditBox(m_htiSivkaAskCounter, RUNTIME_CLASS(CNumTreeOptionsEdit));
		m_htiSivkaAskLog = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_SIVKA_ASK_LOG), m_htiSivkaBanGroup, m_bSivkaAskLog);
		m_htiAntiFakeRank = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_ANTI_FAKE_RANK), m_htiSecurity, m_bAntiFakeRank);
//==>Anti-Leecher [cyrex2001]
#ifdef ANTI_LEECHER
		m_htiEnableALF = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_ALF), m_htiSecurity, m_bEnableALF);
        m_htiEnableAntiCreditHack = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_ANTI_CREDITHACK), m_htiSecurity, m_bEnableAntiCreditHack);
#endif //Anti-Leecher
//<==Anti-Leecher [cyrex2001]
//==>AntiNickThief [shadow2004]
		m_htiEnableAntiNickThief = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_ANTINICKTHIEF), m_htiSecurity, m_bEnableAntiNickThief);
//<==AntiNickThief [shadow2004]
//==>defeat 0-filled partsenders [shadow2004]
#ifdef NULLFILLED
		m_htiEnableZeroFilledTest = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_ZERO_FILLED_TEST), m_htiSecurity, m_bEnableZeroFilledTest);
#endif
//<==defeat 0-filled partsenders [shadow2004]

//==>Drop maunal [cyrex2001]
#ifdef DROP
		m_htiDropSources = m_ctrlTreeOptions.InsertGroup(GetResString(IDS_DROPS), iImgDrop, TVI_ROOT);
		m_htiHqrBox = m_ctrlTreeOptions.InsertItem(GetResString(IDS_DROPHQSLIMIT), TREEOPTSCTRLIMG_EDIT, TREEOPTSCTRLIMG_EDIT, m_htiDropSources);
		m_ctrlTreeOptions.AddEditBox(m_htiHqrBox , RUNTIME_CLASS(CNumTreeOptionsEdit));
		m_htiDropTimer = m_ctrlTreeOptions.InsertItem(GetResString(IDS_DROPTIME), TREEOPTSCTRLIMG_EDIT, TREEOPTSCTRLIMG_EDIT, m_htiDropSources);
		m_ctrlTreeOptions.AddEditBox(m_htiDropTimer , RUNTIME_CLASS(CNumTreeOptionsEdit));
        m_ctrlTreeOptions.Expand(m_htiDropSources, TVE_EXPAND);
#endif //Drop maunal
//<==Drop maunal [cyrex2001]

        m_ctrlTreeOptions.Expand(m_htiSecurity, TVE_EXPAND);

		m_ctrlTreeOptions.SendMessage(WM_VSCROLL, SB_TOP);
		m_bInitializedTreeOpts = true;
	}
//==>Lowid retry by SlugFiller [cyrex2001]
#ifdef LOWID
	DDX_TreeEdit(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiLowIdRetry, m_iLowIdRetry);
	DDV_MinMaxInt(pDX, m_iLowIdRetry, 0, 255);
#endif //Lowid retry
//<==Lowid retry [cyrex2001]
//==> Spooky Mode [cyrex2001]
#ifdef SPOOKY // Spooky Mode [eWombat]
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiEnableSpookyMode,m_bEnableSpookyMode);
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiReconSpooky,m_bReconSpooky);
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiSpookyFailed,m_bSpookyFailed);
	DDX_TreeEdit(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiSpookyFailedCount, m_iSpookyFailedCount);
	DDV_MinMaxInt(pDX, m_iSpookyFailedCount, 3, 25);
#endif // Spooky Mode [eWombat] 
//<== Spooky Mode [cyrex2001]
//==> Spooky Mode ConChecker [cyrex2001]
#ifdef CONCHECKER //>>>WiZaRd: Spooky Mode ConChecker [eWombat] 
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiCheckCon,m_bCheckCon);
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiICMP,m_bICMP);
	DDX_TreeEdit(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiPingTimeOut, m_uiPingTimeOut);
	DDV_MinMaxInt(pDX, m_uiPingTimeOut, 1, 20);
	DDX_TreeEdit(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiPingTTL, m_uiPingTTL);
	DDV_MinMaxInt(pDX, m_uiPingTTL, 8, 64);
#endif //<<<WiZaRd: Spooky Mode ConChecker [eWombat] 
//<== Spooky Mode ConChecker [cyrex2001]
//==> Bold Download-Status [shadow2004]
#ifdef BOLDDL
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiEnableDownloadInBold, m_bEnableDownloadInBold);
#endif //BOLDDL
//<== Bold Download-Status [shadow2004]
//==>timestamp in chatwindow [shadow2004]
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiEnableTimestamp, m_bEnableTimestamp);
//<==timestamp in chatwindow [shadow2004]
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiIsreaskSourceAfterIPChange, m_bIsreaskSourceAfterIPChange);
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiQuickStart, m_bQuickStart);
	DDX_TreeEdit(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiQuickStartMaxTime, m_iQuickStartMaxTime);
	DDV_MinMaxInt(pDX, m_iQuickStartMaxTime, 8, 18);
	DDX_TreeEdit(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiQuickStartMaxConnPerFive, m_iQuickStartMaxConnPerFive);
	DDV_MinMaxInt(pDX, m_iQuickStartMaxConnPerFive, 5, 200);
	DDX_TreeEdit(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiQuickStartMaxConn, m_iQuickStartMaxConn);
	DDV_MinMaxInt(pDX, m_iQuickStartMaxConn, 200, 2000);
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiQuickStartAfterIPChange, m_bQuickStartAfterIPChange);
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiEnableSivkaBan, m_bEnableSivkaBan);
	DDX_TreeEdit(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiSivkaAskTime, m_iSivkaAskTime);
	DDV_MinMaxInt(pDX, m_iSivkaAskTime, 5, 12);
	DDX_TreeEdit(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiSivkaAskCounter, m_iSivkaAskCounter);
	DDV_MinMaxInt(pDX, m_iSivkaAskCounter, 5, 15);
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiSivkaAskLog, m_bSivkaAskLog);
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiAntiFakeRank, m_bAntiFakeRank);
//==>Anti-Leecher [cyrex2001]
#ifdef ANTI_LEECHER
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiEnableALF, m_bEnableALF);
    DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiEnableAntiCreditHack, m_bEnableAntiCreditHack);
#endif //Anti-Leecher
//<==Anti-Leecher [cyrex2001]
//==>AntiNickThief [shadow2004]
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiEnableAntiNickThief, m_bEnableAntiNickThief);
//<==AntiNickThief [shadow2004]
//==>defeat 0-filled partsenders [shadow2004]
#ifdef NULLFILLED
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiEnableZeroFilledTest, m_bEnableZeroFilledTest);
#endif
//<==defeat 0-filled partsenders [shadow2004]
//==>Drop maunal [cyrex2001]
#ifdef DROP
    DDX_TreeEdit(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiHqrBox, iMaxRemoveQRS);
	DDV_MinMaxInt(pDX, iMaxRemoveQRS, 2500, 100000);
	DDX_TreeEdit(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiDropTimer, m_iDropTimer);
	DDV_MinMaxInt(pDX, m_iDropTimer, 2, 4);
#endif //Drop maunal
//<==Drop maunal [cyrex2001]
}

BOOL CPPgNextEMF::OnInitDialog()
{
//==>Reask sourcen after ip change [cyrex2001]
#ifdef RSAIC_MAELLA
	m_bIsreaskSourceAfterIPChange = thePrefs.isreaskSourceAfterIPChange;
#endif //Reask sourcen after ip change
//<==Reask sourcen after ip change [cyrex2001]
	m_bQuickStart = thePrefs.isQuickStart;
	m_iQuickStartMaxTime = (int)(thePrefs.QuickStartMaxTime);
	m_iQuickStartMaxConnPerFive = (int)(thePrefs.QuickStartMaxConnPerFive);
	m_iQuickStartMaxConn = (int)(thePrefs.QuickStartMaxConn);
	m_bQuickStartAfterIPChange = thePrefs.isQuickStartAfterIPChange;
	m_bEnableSivkaBan = thePrefs.enableSivkaBan;
	m_iSivkaAskTime = (int)(thePrefs.SivkaAskTime);
	m_iSivkaAskCounter = (int)(thePrefs.SivkaAskCounter);
	m_bSivkaAskLog = thePrefs.SivkaAskLog;
	m_bAntiFakeRank = thePrefs.AntiFakeRank;
//==>Anti-Leecher [cyrex2001]
#ifdef ANTI_LEECHER
	m_bEnableALF = thePrefs.enableALF;
	m_bEnableAntiCreditHack = thePrefs.enableAntiCreditHack;
#endif //Anti-Leecher
//<==Anti-Leecher [cyrex2001]
//==> Bold Download-Status [shadow2004]
#ifdef BOLDDL
	m_bEnableDownloadInBold = thePrefs.m_bShowActiveDownloadsBold;
#endif //BOLDDL
//<== Bold Download-Status [shadow2004]
//==>timestamp in chatwindow [shadow2004]
	m_bEnableTimestamp = thePrefs.m_bircaddtimestamp;
//<==timestamp in chatwindow [shadow2004]
//==>AntiNickThief [shadow2004]
	m_bEnableAntiNickThief = thePrefs.m_bAntiNickThief;
//<==AntiNickThief [shadow2004]
//==>defeat 0-filled partsenders [shadow2004]
#ifdef NULLFILLED
	m_bEnableZeroFilledTest = thePrefs.enableZeroFilledTest;
#endif
//<==defeat 0-filled partsenders [shadow2004]
//==>Drop maunal [cyrex2001]
#ifdef DROP
    iMaxRemoveQRS = (int) thePrefs.GetMaxRemoveQRS();
    m_iDropTimer = (int) thePrefs.DropTimer;
#endif //Drop maunal
//<==Drop maunal [cyrex2001]
//==>Lowid retry by SlugFiller [cyrex2001]
#ifdef LOWID
	m_iLowIdRetry = thePrefs.GetLowIdRetries();
#endif //Lowid retry
//<==Lowid retry [cyrex2001]
//==> Spooky Mode [cyrex2001]
#ifdef SPOOKY // Spooky Mode [eWombat]
	m_bEnableSpookyMode = thePrefs.m_bEnableSpookyMode;
	m_bReconSpooky = thePrefs.m_bReconSpooky;
	m_bSpookyFailed = thePrefs.m_bSpookyFailed;
	m_iSpookyFailedCount = thePrefs.m_iSpookyFailedCount;
#endif // Spooky Mode [eWombat] 
//<== Spooky Mode [cyrex2001]
//==> Spooky Mode ConChecker [cyrex2001]
#ifdef CONCHECKER //>>>WiZaRd: Spooky Mode ConChecker [eWombat] 
	m_bCheckCon = thePrefs.m_bCheckCon;
	m_bICMP = thePrefs.m_bICMP;
	m_uiPingTimeOut = thePrefs.m_uiPingTimeOut;
	m_uiPingTTL = thePrefs.m_uiPingTTL;
#endif //<<<WiZaRd: Spooky Mode ConChecker [eWombat] 
//<== Spooky Mode ConChecker [cyrex2001]
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
#ifdef RSAIC_MAELLA
	thePrefs.isreaskSourceAfterIPChange = m_bIsreaskSourceAfterIPChange;
#endif //Reask sourcen after ip change
//<==Reask sourcen after ip change [cyrex2001]
	thePrefs.isQuickStart = m_bQuickStart;
	thePrefs.QuickStartMaxTime = m_iQuickStartMaxTime;
	thePrefs.QuickStartMaxConnPerFive = m_iQuickStartMaxConnPerFive;
	thePrefs.QuickStartMaxConn = m_iQuickStartMaxConn;
	thePrefs.isQuickStartAfterIPChange = m_bQuickStartAfterIPChange;
	thePrefs.enableSivkaBan = m_bEnableSivkaBan;
	thePrefs.SivkaAskTime = m_iSivkaAskTime;
	thePrefs.SivkaAskCounter = m_iSivkaAskCounter;
	thePrefs.SivkaAskLog = m_bSivkaAskLog;
	thePrefs.AntiFakeRank = m_bAntiFakeRank;
//==>Anti-Leecher [cyrex2001]
#ifdef ANTI_LEECHER
	thePrefs.enableALF = m_bEnableALF;
	thePrefs.enableAntiCreditHack = m_bEnableAntiCreditHack;
#endif //Anti-Leecher
//<==Anti-Leecher [cyrex2001]
//==> Bold Download-Status [shadow2004]
#ifdef BOLDDL
	thePrefs.m_bShowActiveDownloadsBold = m_bEnableDownloadInBold;
#endif //BOLDDL
//<== Bold Download-Status [shadow2004]
//==>timestamp in chatwindow [shadow2004]
	thePrefs.m_bircaddtimestamp = m_bEnableTimestamp;
//<==timestamp in chatwindow [shadow2004]
//==>AntiNickThief [shadow2004]
	thePrefs.m_bAntiNickThief = m_bEnableAntiNickThief;
//<==AntiNickThief [shadow2004]
//==>defeat 0-filled partsenders [shadow2004]
#ifdef NULLFILLED
	thePrefs.enableZeroFilledTest = m_bEnableZeroFilledTest;
#endif
//<==defeat 0-filled partsenders [shadow2004]
//==>Drop maunal [cyrex2001]
#ifdef DROP
	thePrefs.SetMaxRemoveQRS(iMaxRemoveQRS ? iMaxRemoveQRS : 5000);
	thePrefs.DropTimer = m_iDropTimer;
#endif //Drop maunal
//<==Drop maunal [cyrex2001]
//==>Lowid retry by SlugFiller [cyrex2001]
#ifdef LOWID
	if(m_iLowIdRetry<0)
		m_iLowIdRetry = 0;
	if(m_iLowIdRetry>255)
		m_iLowIdRetry = 255;
	thePrefs.LowIdRetries = m_iLowIdRetry;
#endif //Lowid retry
//<==Lowid retry [cyrex2001]
//==> Spooky Mode [cyrex2001]
#ifdef SPOOKY // Spooky Mode [eWombat]
	thePrefs.m_bEnableSpookyMode = m_bEnableSpookyMode;
	thePrefs.m_bReconSpooky = m_bReconSpooky;
	thePrefs.m_bSpookyFailed = m_bSpookyFailed;
	thePrefs.m_iSpookyFailedCount = m_iSpookyFailedCount;
#endif // Spooky Mode [eWombat] 
//<== Spooky Mode [cyrex2001]
//==> Spooky Mode ConChecker [cyrex2001]
#ifdef CONCHECKER //>>>WiZaRd: Spooky Mode ConChecker [eWombat] 
	thePrefs.m_bCheckCon = m_bCheckCon;
	thePrefs.m_bICMP = m_bICMP;
	thePrefs.m_uiPingTimeOut = m_uiPingTimeOut;
	thePrefs.m_uiPingTTL = m_uiPingTTL;
#endif //<<<WiZaRd: Spooky Mode ConChecker [eWombat] 
//<== Spooky Mode ConChecker [cyrex2001]
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
//==>Lowid retry by SlugFiller [cyrex2001]
#ifdef LOWID
		if(m_htiLowIdRetry)	m_ctrlTreeOptions.SetEditLabel(m_htiLowIdRetry, GetResString(IDS_RECONNECTONLOWID));
#endif //Lowid retry
//<==Lowid retry [cyrex2001]
//==> Spooky Mode [cyrex2001]
#ifdef SPOOKY // Spooky Mode [eWombat]
		if(m_htiEnableSpookyMode) m_ctrlTreeOptions.SetItemText(m_htiSpooky, GetResString(IDS_ENABLE_SPOOKY_MODE));
		if(m_htiReconSpooky) m_ctrlTreeOptions.SetItemText(m_htiReconSpooky, GetResString(IDS_RECON_SPOOKY));
		if(m_htiSpookyFailed) m_ctrlTreeOptions.SetItemText(m_htiSpookyFailed, GetResString(IDS_FAILED_SPOOKY));
		if(m_htiSpookyFailedCount) m_ctrlTreeOptions.SetEditLabel(m_htiSpookyFailedCount, GetResString(IDS_FAILED_SPOOKY_COUNT));
#endif // Spooky Mode [eWombat] 
//<== Spooky Mode [cyrex2001]
//==> Spooky Mode ConChecker [cyrex2001]
#ifdef CONCHECKER //>>>WiZaRd: Spooky Mode ConChecker [eWombat] 
		if(m_htiCheckCon) m_ctrlTreeOptions.SetItemText(m_htiCheckCon, GetResString(IDS_ENABLE_CON_CHECK));
		if(m_htiICMP) m_ctrlTreeOptions.SetItemText(m_htiICMP, GetResString(IDS_ENABLE_ICMP));
		if(m_htiPingTimeOut) m_ctrlTreeOptions.SetEditLabel(m_htiPingTimeOut, GetResString(IDS_PING_TIME_OUT));
		if(m_htiPingTTL) m_ctrlTreeOptions.SetEditLabel(m_htiPingTTL, GetResString(IDS_PING_TLL));
#endif //<<<WiZaRd: Spooky Mode ConChecker [eWombat] 
//<== Spooky Mode ConChecker [cyrex2001]
//==> Bold Download-Status [shadow2004]
#ifdef BOLDDL
		if (m_htiEnableDownloadInBold) m_ctrlTreeOptions.SetItemText(m_htiEnableDownloadInBold, GetResString(IDS_DOWNLOAD_IN_BOLD));
#endif
//<== Bold Download-Status [shadow2004]
//==>timestamp in chatwindow [shadow2004]
		if (m_htiEnableTimestamp) m_ctrlTreeOptions.SetItemText(m_htiEnableTimestamp, GetResString(IDS_IRC_ADDTIMESTAMP));
//<==timestamp in chatwindow [shadow2004]
		if (m_htiIsreaskSourceAfterIPChange) m_ctrlTreeOptions.SetItemText(m_htiIsreaskSourceAfterIPChange, GetResString(IDS_PPG_NEXTEMF_REASK_SOURCE_AFTER_IP_CHANGE_CHECK ));
		if (m_htiQuickStart) m_ctrlTreeOptions.SetItemText(m_htiQuickStart, GetResString(IDS_QUICK_START));
		if (m_htiQuickStartMaxTime) m_ctrlTreeOptions.SetEditLabel(m_htiQuickStartMaxTime, GetResString(IDS_QUICK_START_MAX_TIME));
		if (m_htiQuickStartMaxConnPerFive) m_ctrlTreeOptions.SetEditLabel(m_htiQuickStartMaxConnPerFive, GetResString(IDS_QUICK_START_MAX_CONN_PER_FIVE));
		if (m_htiQuickStartMaxConn) m_ctrlTreeOptions.SetEditLabel(m_htiQuickStartMaxConn, GetResString(IDS_QUICK_START_MAX_CONN));
		if (m_htiQuickStartAfterIPChange) m_ctrlTreeOptions.SetItemText(m_htiQuickStartAfterIPChange, GetResString(IDS_QUICK_START_AFTER_IP_CHANGE));
		if (m_htiEnableSivkaBan) m_ctrlTreeOptions.SetItemText(m_htiEnableSivkaBan, GetResString(IDS_SIVKA_BAN));
		if (m_htiSivkaAskTime) m_ctrlTreeOptions.SetEditLabel(m_htiSivkaAskTime, GetResString(IDS_SIVKA_ASK_TIME));
		if (m_htiSivkaAskCounter) m_ctrlTreeOptions.SetEditLabel(m_htiSivkaAskCounter, GetResString(IDS_SIVKA_ASK_COUNTER));
		if (m_htiSivkaAskLog) m_ctrlTreeOptions.SetItemText(m_htiSivkaAskLog, GetResString(IDS_SIVKA_ASK_LOG));
		if (m_htiAntiFakeRank) m_ctrlTreeOptions.SetItemText(m_htiAntiFakeRank, GetResString(IDS_ANTI_FAKE_RANK));
//==>Anti-Leecher [cyrex2001]
#ifdef ANTI_LEECHER
		if (m_htiEnableALF) m_ctrlTreeOptions.SetItemText(m_htiEnableALF, GetResString(IDS_ALF));
		if (m_htiEnableAntiCreditHack) m_ctrlTreeOptions.SetItemText(m_htiEnableAntiCreditHack, GetResString(IDS_ANTI_CREDITHACK));
#endif //Anti-Leecher
//<==Anti-Leecher [cyrex2001]
//==>AntiNickThief [shadow2004]
	    if (m_htiEnableAntiNickThief) m_ctrlTreeOptions.SetItemText(m_htiEnableAntiNickThief, GetResString(IDS_ANTINICKTHIEF));
//<==AntiNickThief [shadow2004]
//==>defeat 0-filled partsenders [shadow2004]
#ifdef NULLFILLED
	    if (m_htiEnableZeroFilledTest) m_ctrlTreeOptions.SetItemText(m_htiEnableZeroFilledTest, GetResString(IDS_ZERO_FILLED_TEST));
#endif
//<==defeat 0-filled partsenders [shadow2004]
//==>Drop maunal [cyrex2001]
#ifdef DROP
		if (m_htiHqrBox) m_ctrlTreeOptions.SetEditLabel(m_htiHqrBox, GetResString(IDS_DROPHQSLIMIT));
		if (m_htiDropTimer) m_ctrlTreeOptions.SetEditLabel(m_htiDropTimer, GetResString(IDS_DROPTIME));
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
	m_htiSecurity = NULL;
	m_htiSivkaBanGroup = NULL;
	m_htiEnableSivkaBan = NULL;
	m_htiSivkaAskTime = NULL;
	m_htiSivkaAskCounter = NULL;
	m_htiSivkaAskLog = NULL;
	m_htiAntiFakeRank = NULL;
//==> Bold Download-Status [shadow2004]
#ifdef BOLDDL
	m_htiEnableDownloadInBold = NULL;
#endif
//<== Bold Download-Status [shadow2004]
//==>timestamp in chatwindow [shadow2004]
	m_htiEnableTimestamp = NULL;
//<==timestamp in chatwindow [shadow2004]
//==>Anti-Leecher [cyrex2001]
#ifdef ANTI_LEECHER
	m_htiEnableALF = NULL;
	m_htiEnableAntiCreditHack = NULL;
#endif //Anti-Leecher
//<==Anti-Leecher [cyrex2001]
//==>AntiNickThief [shadow2004]
	m_htiEnableAntiNickThief = NULL;
//<==AntiNickThief [shadow2004]
//==>defeat 0-filled partsenders [shadow2004]
#ifdef NULLFILLED
	m_htiEnableZeroFilledTest = NULL;
#endif
//<==defeat 0-filled partsenders [shadow2004]
//==>Drop maunal [cyrex2001]
#ifdef DROP
    m_htiDropSources = NULL;
    m_htiHqrBox = NULL;
    m_htiDropTimer = NULL;
#endif //Drop maunal
//<==Drop maunal [cyrex2001]
//==>Lowid retry by SlugFiller [cyrex2001]
#ifdef LOWID
	m_htiLowIdRetry = NULL;
#endif //Lowid retry
//<==Lowid retry [cyrex2001]
//==> Spooky Mode [cyrex2001]
#ifdef SPOOKY // Spooky Mode [eWombat]
	m_htiSpooky = NULL;
	m_htiEnableSpookyMode = NULL;
	m_htiReconSpooky = NULL;
	m_htiSpookyFailed = NULL;
	m_htiSpookyFailedCount = NULL;
#endif // Spooky Mode [eWombat] 
//<== Spooky Mode [cyrex2001]
//==> Spooky Mode ConChecker [cyrex2001]
#ifdef CONCHECKER //>>>WiZaRd: Spooky Mode ConChecker [eWombat] 
	m_htiCheckConGroup = NULL;
	m_htiCheckCon = NULL;
	m_htiICMP = NULL;
	m_htiPingTimeOut = NULL;
	m_htiPingTTL = NULL;
#endif //<<<WiZaRd: Spooky Mode ConChecker [eWombat] 
//<== Spooky Mode ConChecker [cyrex2001]

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
