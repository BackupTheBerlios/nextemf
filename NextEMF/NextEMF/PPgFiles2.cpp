// PPgFiles2.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "emule.h"
#include "PPgFiles2.h"
#include "emuledlg.h"
#include "Preferences.h"
#include "OtherFunctions.h"
#include "preferencesdlg.h"



// CPPgSecurity2-Dialogfeld

IMPLEMENT_DYNAMIC(CPPgFiles2, CPropertyPage)

CPPgFiles2::CPPgFiles2()
	: CPropertyPage(CPPgFiles2::IDD)
{
	m_imageList.DeleteImageList();
	m_imageList.Create(16, 16, theApp.m_iDfltImageListColorFlags | ILC_MASK, 14+1, 0);
	m_imageList.Add(CTempIconLoader(_T("TRANSFER")));
	m_imageList.Add(CTempIconLoader(_T("TRANSFER")));
}

CPPgFiles2::~CPPgFiles2()
{
}

void CPPgFiles2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_FILES2, m_tabCtr);
	DDX_Control(pDX, IDC_FIL2_NH_SLIDER, m_ctrlNH);
}

BEGIN_MESSAGE_MAP(CPPgFiles2, CPropertyPage)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_FILES2, OnTcnSelchangeTabFiles2)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()
// CPPgSecurity2-Meldungshandler

BOOL CPPgFiles2::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	InitTab(true,1);
	m_tabCtr.SetCurSel(theApp.emuledlg->preferenceswnd->StartPageFiles);
	InitWindowStyles(this);

	LoadSettings();
	Localize();

	return TRUE;

}

void CPPgFiles2::Localize(void)
{
	if(m_hWnd)
	{
		GetDlgItem(IDC_FIL2_NH)->SetWindowText(GetResString(IDS_FIL2_NH));
		GetDlgItem(IDC_FIL2_NH_LBL1)->SetWindowText(GetResString(IDS_FIL2_NH_LBL1));
		GetDlgItem(IDC_FIL2_NH_LBL2)->SetWindowText(GetResString(IDS_FIL2_NH_LBL2));
		
		CString strBuffer;
		strBuffer.Format(_T("%d %s"), m_ctrlNH.GetPos(),_T("%"));
		GetDlgItem(IDC_FIL2_NH_LBL3)->SetWindowText(strBuffer);
	}
}

void CPPgFiles2::LoadSettings(void)
{
	CString strBuffer;
	
	NHash = thePrefs.GetNiceHashLoadWeight();
	m_ctrlNH.SetRange(0,100);
	m_ctrlNH.SetPos(NHash);

	strBuffer.Format(_T("%d %s"), m_ctrlNH.GetPos(),_T("%"));
	GetDlgItem(IDC_FIL2_NH_LBL3)->SetWindowText(strBuffer);
}

BOOL CPPgFiles2::OnApply()
{
	thePrefs.m_iNiceHashLoadWeight = (UINT16) m_ctrlNH.GetPos();

	LoadSettings();
	SetModified(FALSE);
	return CPropertyPage::OnApply();
}

void CPPgFiles2::InitTab(bool firstinit, int Page)
{
	if (firstinit) {
		m_tabCtr.DeleteAllItems();
		m_tabCtr.SetImageList(&m_imageList);
		m_tabCtr.InsertItem(TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM, Files1, GetResString(IDS_FIL1_NAME), 0, (LPARAM)Files1); 
		m_tabCtr.InsertItem(TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM, Files2, GetResString(IDS_FIL2_NAME), 1, (LPARAM)Files2); 
	}

	m_tabCtr.SetCurSel(Page);
}
void CPPgFiles2::OnTcnSelchangeTabFiles2(NMHDR *pNMHDR, LRESULT *pResult)
{
	int cur_sel = m_tabCtr.GetCurSel();
	theApp.emuledlg->preferenceswnd->SwitchTab(cur_sel);
	*pResult = 0;
}

void CPPgFiles2::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CString strBuffer;
	if (pScrollBar == GetDlgItem(IDC_FIL2_NH_SLIDER))
	{
		NHash = (uint16)((CSliderCtrl*)pScrollBar)->GetPos();
		strBuffer.Format(_T("%d %s"), m_ctrlNH.GetPos(),_T("%"));
		GetDlgItem(IDC_FIL2_NH_LBL3)->SetWindowText(strBuffer);

		SetModified(TRUE);
	}

	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}