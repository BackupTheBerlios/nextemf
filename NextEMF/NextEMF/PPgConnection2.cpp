//==>Reask sourcen after ip change [cyrex2001]
//==>Quickstart [cyrex2001]
// PPgConnection2.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "emule.h"
#include "PPgConnection2.h"
#include "emuledlg.h"
#include "Preferences.h"
#include "OtherFunctions.h"
#include "preferencesdlg.h"



// CPPgConnection2-Dialogfeld

IMPLEMENT_DYNAMIC(CPPgConnection2, CPropertyPage)
CPPgConnection2::CPPgConnection2()
	: CPropertyPage(CPPgConnection2::IDD)
{
	m_imageList.DeleteImageList();
	m_imageList.Create(16, 16, theApp.m_iDfltImageListColorFlags | ILC_MASK, 14+1, 0);
	m_imageList.Add(CTempIconLoader(_T("CONNECTION")));
	m_imageList.Add(CTempIconLoader(_T("HARDLIMIT")));
}

CPPgConnection2::~CPPgConnection2()
{
}

void CPPgConnection2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_CONNECTION2, m_tabCtr);
	DDX_Control(pDX, IDC_CON2_QSTIME, m_ctlQSTime);
}


BEGIN_MESSAGE_MAP(CPPgConnection2, CPropertyPage)
	ON_BN_CLICKED(IDC_CON2_REASK, OnSettingsChange)
	ON_BN_CLICKED(IDC_CON2_ACTIVQS, OnBnClickedChkEnablequickstart)
	ON_BN_CLICKED(IDC_CON2_QSAFTERZT, OnSettingsChange)
	ON_EN_KILLFOCUS(IDC_CON2_QSMAXCON, OnSettingsChange)
	ON_EN_CHANGE(IDC_CON2_QSMAXCON, OnSettingsChange)
	ON_EN_CHANGE(IDC_CON2_QSMAXCON5, OnSettingsChange)
	ON_WM_HSCROLL()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CONNECTION2, OnTcnSelchangeTabConnection2)
END_MESSAGE_MAP()


// CPPgConnection2-Meldungshandler

BOOL CPPgConnection2::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	InitTab(true,1);
	m_tabCtr.SetCurSel(theApp.emuledlg->preferenceswnd->StartPageConnection);
	InitWindowStyles(this);

	LoadSettings();
	Localize();

	return TRUE;

}

void CPPgConnection2::Localize(void)
{
	if(m_hWnd)
	{
		GetDlgItem(IDC_CON2_REASK)->SetWindowText(GetResString(IDS_CON2_REASK));
		GetDlgItem(IDC_CON2_ACTIVQS)->SetWindowText(GetResString(IDS_CON2_ACTIVQS));
		GetDlgItem(IDC_CON2_QSAFTERZT)->SetWindowText(GetResString(IDS_CON2_QSAFTERZT));
		
		CString strBuffer;
		strBuffer.Format(_T("%d %s"), m_ctlQSTime.GetPos(),GetResString(IDS_CON2_QSTIME_LBL2));
		GetDlgItem(IDC_CON2_QSTIME_LBL2)->SetWindowText(strBuffer);

		GetDlgItem(IDC_CON2_QSMAXCON_LBL1)->SetWindowText(GetResString(IDS_CON2_QSMAXCON_LBL1));
		GetDlgItem(IDC_CON2_QSMAXCON5_LBL1)->SetWindowText(GetResString(IDS_CON2_QSMAXCON5_LBL1));
		GetDlgItem(IDC_CON2_QSTIME_LBL1)->SetWindowText(GetResString(IDS_CON2_QSTIME_LBL1));
	}
}

void CPPgConnection2::LoadSettings(void)
{
	CString strBuffer;

	CheckDlgButton(IDC_CON2_REASK,		(UINT)thePrefs.IsReaskSourceAfterIPChange());
	CheckDlgButton(IDC_CON2_ACTIVQS,	(UINT)thePrefs.GetQuickStart());
	CheckDlgButton(IDC_CON2_QSAFTERZT,	(UINT)thePrefs.GetQuickStartAfterIPChange());

	if (thePrefs.m_bQuickStart)
	{
		GetDlgItem(IDC_CON2_QSMAXCON)->EnableWindow(TRUE);
		GetDlgItem(IDC_CON2_QSMAXCON_LBL1)->EnableWindow(TRUE);
		GetDlgItem(IDC_CON2_QSMAXCON5)->EnableWindow(TRUE);
		GetDlgItem(IDC_CON2_QSMAXCON5_LBL1)->EnableWindow(TRUE);
		GetDlgItem(IDC_CON2_QSTIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_CON2_QSTIME_LBL1)->EnableWindow(TRUE);
		GetDlgItem(IDC_CON2_QSTIME_LBL2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CON2_QSAFTERZT)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_CON2_QSMAXCON)->EnableWindow(FALSE);
		GetDlgItem(IDC_CON2_QSMAXCON_LBL1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CON2_QSMAXCON5)->EnableWindow(FALSE);
		GetDlgItem(IDC_CON2_QSMAXCON5_LBL1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CON2_QSTIME)->EnableWindow(FALSE);
		GetDlgItem(IDC_CON2_QSTIME_LBL1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CON2_QSTIME_LBL2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CON2_QSAFTERZT)->EnableWindow(FALSE);
	}

	uint16 m_iMaxConPerFile = thePrefs.GetQuickStartMaxConn();
	uint16 m_iMaxConPerFive = thePrefs.GetQuickStartMaxConnPerFive();

	strBuffer.Format(_T("%i"), m_iMaxConPerFile);
	GetDlgItem(IDC_CON2_QSMAXCON)->SetWindowText(strBuffer);

	strBuffer.Format(_T("%i"), m_iMaxConPerFive);
	GetDlgItem(IDC_CON2_QSMAXCON5)->SetWindowText(strBuffer);

	QuickStartMaxTime = thePrefs.QuickStartMaxTime;
	m_ctlQSTime.SetRange(8, 18);
	m_ctlQSTime.SetPos(QuickStartMaxTime);
	m_ctlQSTime.SetTicFreq(1);

	strBuffer.Format(_T("%d %s"), m_ctlQSTime.GetPos(),GetResString(IDS_CON2_QSTIME_LBL2));
	GetDlgItem(IDC_CON2_QSTIME_LBL2)->SetWindowText(strBuffer);
}

BOOL CPPgConnection2::OnApply()
{
	
	CString sBuffer;

	GetDlgItem(IDC_CON2_QSMAXCON)->GetWindowText(sBuffer);
	thePrefs.SetQuickStartMaxConn(_tstoi(sBuffer));
	if(thePrefs.GetQuickStartMaxConn() < 100 || thePrefs.GetQuickStartMaxConn() > 2000)
	{ 
		if (thePrefs.GetMaxCon() > 666)
			thePrefs.SetQuickStartMaxConn(2000);
		else
			thePrefs.SetQuickStartMaxConn(thePrefs.GetMaxCon()*3);
		sBuffer.Format(_T("%d"),thePrefs.GetQuickStartMaxConn()); 
		GetDlgItem(IDC_CON2_QSMAXCON)->SetWindowText(sBuffer); 
	} 

	GetDlgItem(IDC_CON2_QSMAXCON5)->GetWindowText(sBuffer);
	thePrefs.SetQuickStartMaxConnPerFive(_tstoi(sBuffer));
	if(thePrefs.GetQuickStartMaxConnPerFive() < 5 || thePrefs.GetQuickStartMaxConnPerFive() > 200)
	{ 
		if (thePrefs.GetMaxConperFive() > 66)
			thePrefs.SetQuickStartMaxConnPerFive(200);
		else
			thePrefs.SetQuickStartMaxConnPerFive(thePrefs.GetMaxConperFive()*3);
		sBuffer.Format(_T("%d"),thePrefs.GetQuickStartMaxConnPerFive()); 
		GetDlgItem(IDC_CON2_QSMAXCON5)->SetWindowText(sBuffer); 
	} 

	thePrefs.m_bReaskSourceAfterIPChange	=	(UINT)IsDlgButtonChecked(IDC_CON2_REASK) ? TRUE : FALSE;
	thePrefs.m_bQuickStart			=	(UINT)IsDlgButtonChecked(IDC_CON2_ACTIVQS) ? TRUE : FALSE;
	thePrefs.m_bQuickStartAfterIPChange	=	(UINT)IsDlgButtonChecked(IDC_CON2_QSAFTERZT) ? TRUE : FALSE;
	thePrefs.QuickStartMaxTime              =       (uint16)m_ctlQSTime.GetPos();

	LoadSettings();
	SetModified(FALSE);
	return CPropertyPage::OnApply();
}

void CPPgConnection2::OnBnClickedChkEnablequickstart()
{
	if (IsDlgButtonChecked(IDC_CON2_ACTIVQS))
	{
		GetDlgItem(IDC_CON2_QSMAXCON)->EnableWindow(TRUE);
		GetDlgItem(IDC_CON2_QSMAXCON_LBL1)->EnableWindow(TRUE);
		GetDlgItem(IDC_CON2_QSMAXCON5)->EnableWindow(TRUE);
		GetDlgItem(IDC_CON2_QSMAXCON5_LBL1)->EnableWindow(TRUE);
		GetDlgItem(IDC_CON2_QSTIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_CON2_QSTIME_LBL1)->EnableWindow(TRUE);
		GetDlgItem(IDC_CON2_QSTIME_LBL2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CON2_QSAFTERZT)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_CON2_QSMAXCON)->EnableWindow(FALSE);
		GetDlgItem(IDC_CON2_QSMAXCON_LBL1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CON2_QSMAXCON5)->EnableWindow(FALSE);
		GetDlgItem(IDC_CON2_QSMAXCON5_LBL1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CON2_QSTIME)->EnableWindow(FALSE);
		GetDlgItem(IDC_CON2_QSTIME_LBL1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CON2_QSTIME_LBL2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CON2_QSAFTERZT)->EnableWindow(FALSE);
	}
	SetModified();
}

void CPPgConnection2::InitTab(bool firstinit, int Page)
{
	if (firstinit) {
		m_tabCtr.DeleteAllItems();
		m_tabCtr.SetImageList(&m_imageList);
		m_tabCtr.InsertItem(TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM, Connection1, GetResString(IDS_CON1_NAME), 0, (LPARAM)Connection1); 
		m_tabCtr.InsertItem(TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM, Connection2, GetResString(IDS_CON2_NAME), 0, (LPARAM)Connection2); 
		m_tabCtr.InsertItem(TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM, Connection3, GetResString(IDS_CON3_NAME), 1, (LPARAM)Connection3); 
	}

	m_tabCtr.SetCurSel(Page);
}
void CPPgConnection2::OnTcnSelchangeTabConnection2(NMHDR *pNMHDR, LRESULT *pResult)
{
	int cur_sel = m_tabCtr.GetCurSel();
	theApp.emuledlg->preferenceswnd->SwitchTab(cur_sel);
	*pResult = 0;
}

void CPPgConnection2::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CString strBuffer;
	if (pScrollBar == GetDlgItem(IDC_CON2_QSTIME))
	{
		QuickStartMaxTime = (uint16)((CSliderCtrl*)pScrollBar)->GetPos();
		strBuffer.Format(_T("%d %s"), m_ctlQSTime.GetPos(),GetResString(IDS_CON2_QSTIME_LBL2));
		GetDlgItem(IDC_CON2_QSTIME_LBL2)->SetWindowText(strBuffer);

		SetModified(TRUE);
	}

	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}
