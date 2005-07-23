//==>WiZaRd/Max AutoHardLimit [cyrex2001]
// PPgConnection3.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "emule.h"
#include "PPgConnection3.h"
#include "emuledlg.h"
#include "Preferences.h"
#include "OtherFunctions.h"
#include "preferencesdlg.h"


// CPPgConnection2-Dialogfeld

IMPLEMENT_DYNAMIC(CPPgConnection3, CPropertyPage)
CPPgConnection3::CPPgConnection3()
	: CPropertyPage(CPPgConnection3::IDD)
{
	m_imageList.DeleteImageList();
	m_imageList.Create(16, 16, theApp.m_iDfltImageListColorFlags | ILC_MASK, 14+1, 0);
	m_imageList.Add(CTempIconLoader(_T("CONNECTION")));
	m_imageList.Add(CTempIconLoader(_T("HARDLIMIT")));
}

CPPgConnection3::~CPPgConnection3()
{
}

void CPPgConnection3::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_CONNECTION3, m_tabCtr);
	DDX_Control(pDX, IDC_CON3_GHLTIME, m_ctlGHLTime);
	DDX_Control(pDX, IDC_CON3_GHLMINQ, m_ctlGHLMinSrc);
}


BEGIN_MESSAGE_MAP(CPPgConnection3, CPropertyPage)
	ON_BN_CLICKED(IDC_CON3_GHLACTIV, OnBnClickedChkEnablehardlimit)
	ON_EN_KILLFOCUS(IDC_CON3_GHLTIME, OnSettingsChange)
	ON_EN_CHANGE(IDC_CON3_GHLMAXQ, OnSettingsChange)
	ON_EN_KILLFOCUS(IDC_CON3_GHLMINQ, OnSettingsChange)
	ON_WM_HSCROLL()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CONNECTION3, OnTcnSelchangeTabConnection3)
END_MESSAGE_MAP()


// CPPgConnection3-Meldungshandler

BOOL CPPgConnection3::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	InitTab(true,2);
	m_tabCtr.SetCurSel(theApp.emuledlg->preferenceswnd->StartPageConnection);
	InitWindowStyles(this);

	LoadSettings();
	Localize();

	return TRUE; 
}

void CPPgConnection3::Localize(void)
{
	if(m_hWnd)
	{
		GetDlgItem(IDC_CON3_GHLACTIV)->SetWindowText(GetResString(IDS_CON3_GHLACTIV));
		GetDlgItem(IDC_CON3_GHLTIME_LBL1)->SetWindowText(GetResString(IDS_CON3_GHLTIME_LBL1));

		CString strBuffer;
		strBuffer.Format(_T("%d %s"), m_ctlGHLTime.GetPos(),GetResString(IDS_CON3_GHLTIME_LBL2));
		GetDlgItem(IDC_CON3_GHLTIME_LBL2)->SetWindowText(strBuffer);

		GetDlgItem(IDC_CON3_GHLMAXQ_LBL1)->SetWindowText(GetResString(IDS_CON3_GHLMAXQ_LBL1));
		GetDlgItem(IDC_CON3_GHLMINQ_LBL1)->SetWindowText(GetResString(IDS_CON3_GHLMINQ_LBL1));
	}
}

void CPPgConnection3::LoadSettings(void)
{
	CString strBuffer;

	CheckDlgButton(IDC_CON3_GHLACTIV,		(UINT)thePrefs.IsUseAutoHL());

	if (thePrefs.m_bUseAutoHL)
	{
		GetDlgItem(IDC_CON3_GHLTIME)->EnableWindow(TRUE);
		GetDlgItem(IDC_CON3_GHLTIME_LBL1)->EnableWindow(TRUE);
		GetDlgItem(IDC_CON3_GHLTIME_LBL2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CON3_GHLMAXQ)->EnableWindow(TRUE);
		GetDlgItem(IDC_CON3_GHLMAXQ_LBL1)->EnableWindow(TRUE);
		GetDlgItem(IDC_CON3_GHLMINQ)->EnableWindow(TRUE);
		GetDlgItem(IDC_CON3_GHLMINQ_LBL1)->EnableWindow(TRUE);
		GetDlgItem(IDC_CON3_GHLMINQ_LBL2)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_CON3_GHLTIME)->EnableWindow(FALSE);
		GetDlgItem(IDC_CON3_GHLTIME_LBL1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CON3_GHLTIME_LBL2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CON3_GHLMAXQ)->EnableWindow(FALSE);
		GetDlgItem(IDC_CON3_GHLMAXQ_LBL1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CON3_GHLMINQ)->EnableWindow(FALSE);
		GetDlgItem(IDC_CON3_GHLMINQ_LBL1)->EnableWindow(FALSE);
		GetDlgItem(IDC_CON3_GHLMINQ_LBL2)->EnableWindow(FALSE);
	}

	GHLMaxTime = thePrefs.GetAutoHLUpdateTimer();
	m_ctlGHLTime.SetRange(30, 80);
	m_ctlGHLTime.SetPos(GHLMaxTime);
	m_ctlGHLTime.SetTicFreq(1);

	strBuffer.Format(_T("%d %s"), m_ctlGHLTime.GetPos(),GetResString(IDS_CON3_GHLTIME_LBL2));
	GetDlgItem(IDC_CON3_GHLTIME_LBL2)->SetWindowText(strBuffer);

	uint16 m_iMaxQGhl = thePrefs.GetMaxSourcesHL();
	strBuffer.Format(_T("%i"), m_iMaxQGhl);
	GetDlgItem(IDC_CON3_GHLMAXQ)->SetWindowText(strBuffer);

	GHLMinSrc = thePrefs.GetMinFileLimit();
	m_ctlGHLMinSrc.SetRange(10, 50);
	m_ctlGHLMinSrc.SetPos(GHLMinSrc);
	m_ctlGHLMinSrc.SetTicFreq(1);
	
	strBuffer.Format(_T("%d"), m_ctlGHLMinSrc.GetPos());
	GetDlgItem(IDC_CON3_GHLMINQ_LBL2)->SetWindowText(strBuffer);
}

BOOL CPPgConnection3::OnApply()
{
	
	CString sBuffer;

	thePrefs.m_bUseAutoHL	=	(UINT)IsDlgButtonChecked(IDC_CON3_GHLACTIV) ? TRUE : FALSE;
	thePrefs.SetAutoHLUpdateTimer((uint16)m_ctlGHLTime.GetPos());

	GetDlgItem(IDC_CON3_GHLMAXQ)->GetWindowText(sBuffer);
	thePrefs.SetMaxSourcesHL(_tstoi(sBuffer));
	if(thePrefs.GetMaxSourcesHL() < 1000 || thePrefs.GetMaxSourcesHL() > 5000)
	{
		thePrefs.SetMaxSourcesHL(4000);
		sBuffer.Format(_T("%d"),thePrefs.GetMaxSourcesHL()); 
		GetDlgItem(IDC_CON3_GHLMAXQ)->SetWindowText(sBuffer); 
	}

	thePrefs.SetMinFileLimit((uint16)m_ctlGHLMinSrc.GetPos());

	LoadSettings();
	SetModified(FALSE);
	return CPropertyPage::OnApply();
}

void CPPgConnection3::OnBnClickedChkEnablehardlimit()
{
	if (IsDlgButtonChecked(IDC_CON3_GHLACTIV))
	{
	GetDlgItem(IDC_CON3_GHLTIME)->EnableWindow(TRUE);
	GetDlgItem(IDC_CON3_GHLTIME_LBL1)->EnableWindow(TRUE);
	GetDlgItem(IDC_CON3_GHLTIME_LBL2)->EnableWindow(TRUE);
	GetDlgItem(IDC_CON3_GHLMAXQ)->EnableWindow(TRUE);
	GetDlgItem(IDC_CON3_GHLMAXQ_LBL1)->EnableWindow(TRUE);
	GetDlgItem(IDC_CON3_GHLMINQ)->EnableWindow(TRUE);
	GetDlgItem(IDC_CON3_GHLMINQ_LBL1)->EnableWindow(TRUE);
	GetDlgItem(IDC_CON3_GHLMINQ_LBL2)->EnableWindow(TRUE);
	}
	else
	{
	GetDlgItem(IDC_CON3_GHLTIME)->EnableWindow(FALSE);
	GetDlgItem(IDC_CON3_GHLTIME_LBL1)->EnableWindow(FALSE);
	GetDlgItem(IDC_CON3_GHLTIME_LBL2)->EnableWindow(FALSE);
	GetDlgItem(IDC_CON3_GHLMAXQ)->EnableWindow(FALSE);
	GetDlgItem(IDC_CON3_GHLMAXQ_LBL1)->EnableWindow(FALSE);
	GetDlgItem(IDC_CON3_GHLMINQ)->EnableWindow(FALSE);
	GetDlgItem(IDC_CON3_GHLMINQ_LBL1)->EnableWindow(FALSE);
	GetDlgItem(IDC_CON3_GHLMINQ_LBL2)->EnableWindow(FALSE);
	}	
	SetModified();
}

void CPPgConnection3::InitTab(bool firstinit, int Page)
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
void CPPgConnection3::OnTcnSelchangeTabConnection3(NMHDR *pNMHDR, LRESULT *pResult)
{
	int cur_sel = m_tabCtr.GetCurSel();
	theApp.emuledlg->preferenceswnd->SwitchTab(cur_sel);
	*pResult = 0;
}

void CPPgConnection3::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CString strBuffer;
	if (pScrollBar == GetDlgItem(IDC_CON3_GHLTIME))
	{
		GHLMaxTime = (uint16)((CSliderCtrl*)pScrollBar)->GetPos();
		strBuffer.Format(_T("%d %s"), m_ctlGHLTime.GetPos(),GetResString(IDS_CON3_GHLTIME_LBL2));
		GetDlgItem(IDC_CON3_GHLTIME_LBL2)->SetWindowText(strBuffer);

		SetModified(TRUE);
	}
	if (pScrollBar == GetDlgItem(IDC_CON3_GHLMINQ))
	{
		GHLMinSrc = (uint16)((CSliderCtrl*)pScrollBar)->GetPos();
		strBuffer.Format(_T("%d"), m_ctlGHLMinSrc.GetPos());
		GetDlgItem(IDC_CON3_GHLMINQ_LBL2)->SetWindowText(strBuffer);

		SetModified(TRUE);
	}

	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}