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
//==>Reask sourcen after ip chnage or Quickstart [cyrex2001]
#if defined (RSAIC) || defined (QUICKSTART) //Reask sourcen after ip chnage or Quickstart

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
		CImageList* piml = m_ctrlTreeOptions.GetImageList(TVSIL_NORMAL);
		if (piml)
		{
			iImgOpt = piml->Add(CTempIconLoader(_T("PREF_CONNECTION")));
		}
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

		m_ctrlTreeOptions.SendMessage(WM_VSCROLL, SB_TOP);
		m_bInitializedTreeOpts = true;
	}
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiIsreaskSourceAfterIPChange, m_bIsreaskSourceAfterIPChange);
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiQuickStart, m_bQuickStart);
	DDX_TreeEdit(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiQuickStartMaxTime, m_iQuickStartMaxTime);
	DDV_MinMaxInt(pDX, m_iQuickStartMaxTime, 8, 18);
	DDX_TreeEdit(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiQuickStartMaxConnPerFive, m_iQuickStartMaxConnPerFive);
	DDV_MinMaxInt(pDX, m_iQuickStartMaxConnPerFive, 5, 200);
	DDX_TreeEdit(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiQuickStartMaxConn, m_iQuickStartMaxConn);
	DDV_MinMaxInt(pDX, m_iQuickStartMaxConn, 200, 2000);
	DDX_TreeCheck(pDX, IDC_PPG_NEXTEMF_OPTS, m_htiQuickStartAfterIPChange, m_bQuickStartAfterIPChange);
}

BOOL CPPgNextEMF::OnInitDialog()
{
	m_bIsreaskSourceAfterIPChange = thePrefs.isreaskSourceAfterIPChange;
	m_bQuickStart = thePrefs.isQuickStart;
	m_iQuickStartMaxTime = (int)(thePrefs.QuickStartMaxTime);
	m_iQuickStartMaxConnPerFive = (int)(thePrefs.QuickStartMaxConnPerFive);
	m_iQuickStartMaxConn = (int)(thePrefs.QuickStartMaxConn);
	m_bQuickStartAfterIPChange = thePrefs.isQuickStartAfterIPChange;

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

	thePrefs.isreaskSourceAfterIPChange = m_bIsreaskSourceAfterIPChange;
	thePrefs.isQuickStart = m_bQuickStart;
	thePrefs.QuickStartMaxTime = m_iQuickStartMaxTime;
	thePrefs.QuickStartMaxConnPerFive = m_iQuickStartMaxConnPerFive;
	thePrefs.QuickStartMaxConn = m_iQuickStartMaxConn;
	thePrefs.isQuickStartAfterIPChange = m_bQuickStartAfterIPChange;

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
		SetWindowText(_T("NextEMF"));
		GetDlgItem(IDC_WARNING)->SetWindowText(GetResString(IDS_TWEAKS_WARNING));

		if (m_htiIsreaskSourceAfterIPChange) m_ctrlTreeOptions.SetItemText(m_htiIsreaskSourceAfterIPChange, GetResString(IDS_PPG_NEXTEMF_REASK_SOURCE_AFTER_IP_CHANGE_CHECK ));
		if (m_htiQuickStart) m_ctrlTreeOptions.SetItemText(m_htiQuickStart, GetResString(IDS_QUICK_START));
		if (m_htiQuickStartMaxTime) m_ctrlTreeOptions.SetEditLabel(m_htiQuickStartMaxTime, GetResString(IDS_QUICK_START_MAX_TIME));
		if (m_htiQuickStartMaxConnPerFive) m_ctrlTreeOptions.SetEditLabel(m_htiQuickStartMaxConnPerFive, GetResString(IDS_QUICK_START_MAX_CONN_PER_FIVE));
		if (m_htiQuickStartMaxConn) m_ctrlTreeOptions.SetEditLabel(m_htiQuickStartMaxConn, GetResString(IDS_QUICK_START_MAX_CONN));
		if (m_htiQuickStartAfterIPChange) m_ctrlTreeOptions.SetItemText(m_htiQuickStartAfterIPChange, GetResString(IDS_QUICK_START_AFTER_IP_CHANGE));
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
#endif //Reask sourcen after ip chnage or Quickstart
//<==Reask sourcen after ip chnage or Quickstart [cyrex2001]