// PPgConnection2.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "emule.h"
#include "PPgConnection2.h"
#include "emuledlg.h"
#include "Preferences.h"
#include "otherfunctions.h"

// eF-Mod :: PPgTabControl
#include "preferencesdlg.h"
// eF-Mod end


// CPPgConnection2-Dialogfeld

IMPLEMENT_DYNAMIC(CPPgConnection2, CPropertyPage)
CPPgConnection2::CPPgConnection2()
	: CPropertyPage(CPPgConnection2::IDD)
{
	// eF-Mod :: PPgTabControl
	m_imageList.DeleteImageList();
	m_imageList.Create(16, 16, theApp.m_iDfltImageListColorFlags | ILC_MASK, 14+1, 0);
	m_imageList.Add(CTempIconLoader(_T("CONNECTION")));
	// eF-Mod end
}

CPPgConnection2::~CPPgConnection2()
{
}

void CPPgConnection2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	// eF-Mod :: PPgTabControl
	DDX_Control(pDX, IDC_TAB_CONNECTION2, m_tabCtr);
	// eF-Mod end
}


BEGIN_MESSAGE_MAP(CPPgConnection2, CPropertyPage)
	// eF-Mod :: PPgTabControl
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CONNECTION2, OnTcnSelchangeTabConnection2)
	// eF-Mod end
END_MESSAGE_MAP()


// CPPgConnection2-Meldungshandler

BOOL CPPgConnection2::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// eF-Mod :: PPgTabControl
	InitTab(true,1);
	m_tabCtr.SetCurSel(theApp.emuledlg->preferenceswnd->StartPageConnection);
	// eF-Mod end

	InitWindowStyles(this);

	LoadSettings();
	Localize();

	return TRUE;  // return TRUE unless you set the focus to a control

}

void CPPgConnection2::Localize(void)
{
	if(m_hWnd)
	{
	}
}

void CPPgConnection2::LoadSettings(void)
{
}

BOOL CPPgConnection2::OnApply()
{
	SetModified(FALSE);
	return CPropertyPage::OnApply();
}

// eF-Mod :: PPgTabControl
void CPPgConnection2::InitTab(bool firstinit, int Page)
{
	if (firstinit) {
		m_tabCtr.DeleteAllItems();
		m_tabCtr.SetImageList(&m_imageList);
		m_tabCtr.InsertItem(TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM, Connection1, _T("Connection 1"), 0, (LPARAM)Connection1); 
		m_tabCtr.InsertItem(TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM, Connection2, _T("Connection 2"), 0, (LPARAM)Connection2); 
	}

	m_tabCtr.SetCurSel(Page);
}
void CPPgConnection2::OnTcnSelchangeTabConnection2(NMHDR *pNMHDR, LRESULT *pResult)
{
	int cur_sel = m_tabCtr.GetCurSel();
	theApp.emuledlg->preferenceswnd->SwitchTab(cur_sel);
	*pResult = 0;
}
// eF-Mod end
