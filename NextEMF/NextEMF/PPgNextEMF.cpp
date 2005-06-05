// PPgNextEMF.cpp : implementation file
//

#include "stdafx.h"
#include "emule.h"
#include "PPgNextEMF.h"
#include "Preferences.h"
#include "PreferencesDlg.h"
#include "opcodes.h"
#include "otherfunctions.h"
#include "UserMsgs.h"
#include "ModVersion.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW

#endif

IMPLEMENT_DYNAMIC(CPPgNextEMF, CPropertyPage)
BEGIN_MESSAGE_MAP(CPPgNextEMF, CPropertyPage)
	ON_WM_HSCROLL()
    ON_WM_DESTROY()
	ON_MESSAGE(UM_TREEOPTSCTRL_NOTIFY, OnTreeOptsCtrlNotify)
END_MESSAGE_MAP()


CPPgNextEMF::CPPgNextEMF()
	: CPropertyPage(CPPgNextEMF::IDD)
	, m_ctrlTreeOptions(theApp.m_iDfltImageListColorFlags)
{
	m_bInitializedTreeOpts = false;
	m_htiSecurity = NULL;
//==>WiZaRd AntiLeechClass [cyrex2001]
#ifdef ANTI_LEECH_CLASS
	m_htiEnableAntiNickThief = NULL;
	m_htiEnableAntiModThief = NULL;
	m_htiClientBanTime = NULL;
#endif //WiZaRd AntiLeechClass
//<==WiZaRd AntiLeechClass [cyrex2001]

//==> Emulate others by WiZaRd & Spike [shadow2004]
#ifdef EMULATE
	m_htiEmulator = NULL;
	m_htiEnableMLDonkey = NULL;
	m_htiEnableeDonkey = NULL;
	m_htiEnableeDonkeyHybrid = NULL;
	m_htiEnableShareaza = NULL;
#endif
//<== Emulate others by WiZaRd & Spike [shadow2004]
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
		int iImgSecurity = 8;
//==> Emulate others by WiZaRd & Spike [shadow2004]
#ifdef EMULATE
		int iImgEmulator = 8;
#endif
//<== Emulate others by WiZaRd & Spike [shadow2004]

		CImageList* piml = m_ctrlTreeOptions.GetImageList(TVSIL_NORMAL);
		if (piml)
		{
			iImgSecurity = piml->Add(CTempIconLoader(_T("SECURITY")));
//==> Emulate others by WiZaRd & Spike [shadow2004]
#ifdef EMULATE
			iImgEmulator = piml->Add(CTempIconLoader(_T("EMULATOR")));
#endif
//<== Emulate others by WiZaRd & Spike [shadow2004]
		}
        m_htiSecurity = m_ctrlTreeOptions.InsertGroup(GetResString(IDS_SECURITY), iImgSecurity, TVI_ROOT);
//==>WiZaRd AntiLeechClass [cyrex2001]
#ifdef ANTI_LEECH_CLASS
		m_htiEnableAntiNickThief = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_ANTINICKTHIEF), m_htiSecurity, m_bEnableAntiNickThief);
		m_htiEnableAntiModThief = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_ANTIMODTHIEF), m_htiSecurity, m_bEnableAntiModThief);
		m_htiClientBanTime = m_ctrlTreeOptions.InsertItem(GetResString(IDS_CLIENTBANTIME), TREEOPTSCTRLIMG_EDIT, TREEOPTSCTRLIMG_EDIT, m_htiSecurity);
		m_ctrlTreeOptions.AddEditBox(m_htiClientBanTime, RUNTIME_CLASS(CNumTreeOptionsEdit));
#endif //WiZaRd AntiLeechClass
//<==WiZaRd AntiLeechClass [cyrex2001]
        m_ctrlTreeOptions.Expand(m_htiSecurity, TVE_EXPAND);
//==> Emulate others by WiZaRd & Spike [shadow2004]
#ifdef EMULATE
		m_htiEmulator = m_ctrlTreeOptions.InsertGroup(GetResString(IDS_EMULATOR), iImgEmulator, TVI_ROOT);
		m_htiEnableMLDonkey = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_EMUMLDONKEY), m_htiEmulator, EmuMLDonkey);
		m_htiEnableeDonkey = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_EMUEDONKEY), m_htiEmulator, EmueDonkey);
		m_htiEnableeDonkeyHybrid = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_EMUEDONKEYHYBRID), m_htiEmulator, EmueDonkeyHybrid);
		m_htiEnableShareaza = m_ctrlTreeOptions.InsertCheckBox(GetResString(IDS_EMUSHAREAZA2), m_htiEmulator, EmuShareaza);
		m_ctrlTreeOptions.Expand(m_htiEmulator, TVE_EXPAND);
#endif
//<== Emulate others by WiZaRd & Spike [shadow2004]
		m_ctrlTreeOptions.SendMessage(WM_VSCROLL, SB_TOP);
		m_bInitializedTreeOpts = true;
	}
//==>WiZaRd AntiLeechClass [cyrex2001]
#ifdef ANTI_LEECH_CLASS
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiEnableAntiNickThief, m_bEnableAntiNickThief);
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiEnableAntiModThief, m_bEnableAntiModThief);
	DDX_TreeEdit (pDX, IDC_PPG_NEXTEMF_OPTS, m_htiClientBanTime, m_iClientBanTime);
	DDV_MinMaxInt(pDX, m_iClientBanTime, 2, 12);// 2-12h [cyrex2001]
#endif //WiZaRd AntiLeechClass
//<==WiZaRd AntiLeechClass [cyrex2001]

//==> Emulate others by WiZaRd & Spike [shadow2004]
#ifdef EMULATE
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiEnableMLDonkey, EmuMLDonkey);
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiEnableeDonkey, EmueDonkey);
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiEnableeDonkeyHybrid, EmueDonkeyHybrid);
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiEnableShareaza, EmuShareaza);
#endif
//<== Emulate others by WiZaRd & Spike [shadow2004]
}

BOOL CPPgNextEMF::OnInitDialog()
{
//==>WiZaRd AntiLeechClass [cyrex2001]
#ifdef ANTI_LEECH_CLASS
	m_bEnableAntiNickThief = thePrefs.m_bAntiNickThief;
	m_bEnableAntiModThief = thePrefs.m_bAntiModThief;
	m_iClientBanTime = (int) (thePrefs.m_iClientBanTime);
#endif //WiZaRd AntiLeechClass
//<==WiZaRd AntiLeechClass [cyrex2001]

//==> Emulate others by WiZaRd & Spike [shadow2004]
#ifdef EMULATE
	EmuMLDonkey      = thePrefs.EmuMLDonkey;
	EmueDonkey       = thePrefs.EmueDonkey;
	EmueDonkeyHybrid = thePrefs.EmueDonkeyHybrid;
	EmuShareaza      = thePrefs.EmuShareaza;
#endif
//<== Emulate others by WiZaRd & Spike [shadow2004]
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
//==>WiZaRd AntiLeechClass [cyrex2001]
#ifdef ANTI_LEECH_CLASS
	thePrefs.m_bAntiNickThief = m_bEnableAntiNickThief;
	thePrefs.m_bAntiModThief = m_bEnableAntiModThief;
	thePrefs.m_iClientBanTime = m_iClientBanTime;
#endif //WiZaRd AntiLeechClass
//<==WiZaRd AntiLeechClass [cyrex2001]
//==> Emulate others by WiZaRd & Spike [shadow2004]
#ifdef EMULATE
	thePrefs.EmuMLDonkey      = EmuMLDonkey;
	thePrefs.EmueDonkey       = EmueDonkey;
	thePrefs.EmueDonkeyHybrid = EmueDonkeyHybrid;
	thePrefs.EmuShareaza      = EmuShareaza;
#endif
//<== Emulate others by WiZaRd & Spike [shadow2004]
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
//==>WiZaRd AntiLeechClass [cyrex2001]
#ifdef ANTI_LEECH_CLASS
	    if (m_htiEnableAntiNickThief) m_ctrlTreeOptions.SetItemText(m_htiEnableAntiNickThief, GetResString(IDS_ANTINICKTHIEF));
	    if (m_htiEnableAntiModThief) m_ctrlTreeOptions.SetItemText(m_htiEnableAntiModThief, GetResString(IDS_ANTIMODTHIEF));
	    if (m_htiClientBanTime) m_ctrlTreeOptions.SetEditLabel(m_htiClientBanTime, GetResString(IDS_CLIENTBANTIME));
#endif //WiZaRd AntiLeechClass
//<==WiZaRd AntiLeechClass [cyrex2001]
//==> Emulate others by WiZaRd & Spike [shadow2004]
#ifdef EMULATE
		if (m_htiEnableMLDonkey) m_ctrlTreeOptions.SetItemText(m_htiEnableMLDonkey, GetResString(IDS_EMUMLDONKEY));
		if (m_htiEnableeDonkey) m_ctrlTreeOptions.SetItemText(m_htiEnableeDonkey, GetResString(IDS_EMUEDONKEY));
		if (m_htiEnableeDonkeyHybrid) m_ctrlTreeOptions.SetItemText(m_htiEnableeDonkeyHybrid, GetResString(IDS_EMUEDONKEYHYBRID));
		if (m_htiEnableShareaza) m_ctrlTreeOptions.SetItemText(m_htiEnableShareaza, GetResString(IDS_EMUSHAREAZA2));
#endif
//<== Emulate others by WiZaRd & Spike [shadow2004]
	}
}

void CPPgNextEMF::OnDestroy()
{
	m_ctrlTreeOptions.DeleteAllItems();
	m_ctrlTreeOptions.DestroyWindow();
	m_bInitializedTreeOpts = false;
//==>WiZaRd AntiLeechClass [cyrex2001]
#ifdef ANTI_LEECH_CLASS
	m_htiEnableAntiNickThief = NULL;
	m_htiEnableAntiModThief = NULL;
	m_htiClientBanTime = NULL;
#endif //WiZaRd AntiLeechClass
//<==WiZaRd AntiLeechClass [cyrex2001]
//==> Emulate others by WiZaRd & Spike [shadow2004]
#ifdef EMULATE
	m_htiEnableMLDonkey      = NULL;
	m_htiEnableeDonkey       = NULL;
	m_htiEnableeDonkeyHybrid = NULL;
	m_htiEnableShareaza      = NULL;
#endif
//<== Emulate others by WiZaRd & Spike [shadow2004]

	CPropertyPage::OnDestroy();
}

LRESULT CPPgNextEMF::OnTreeOptsCtrlNotify(WPARAM wParam, LPARAM lParam)
{
	if (wParam == IDC_PPG_NEXTEMF_OPTS){
		SetModified();
	}
	return 0;
}
