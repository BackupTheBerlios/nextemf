//==>Reask sourcen after ip change [cyrex2001]
//==>Quickstart [cyrex2001]
// PPgSecurity2.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "emule.h"
#include "PPgSecurity2.h"
#include "emuledlg.h"
#include "Preferences.h"
#include "OtherFunctions.h"
#include "preferencesdlg.h"
#include "IPFilter.h"



// CPPgSecurity2-Dialogfeld

IMPLEMENT_DYNAMIC(CPPgSecurity2, CPropertyPage)

BEGIN_MESSAGE_MAP(CPPgSecurity2, CPropertyPage)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_SECURITY2, OnTcnSelchangeTabSecurity2)
	ON_BN_CLICKED(IDC_SEC2_IPFSERVER, OnSettingsChange)
	ON_EN_CHANGE(IDC_SEC2_IPFLEVEL, OnSettingsChange)
	ON_BN_CLICKED(IDC_SEC2_IPFRELOAD,OnReloadIPFilter)
	ON_BN_CLICKED(IDC_SEC2_IPFEDIT, OnEditIPFilter)
	ON_EN_CHANGE(IDC_SEC2_IPFURL, OnSettingsChange)
	ON_BN_CLICKED(IDC_SEC2_IPFRESET,OnResetIPFilterURL)
	ON_BN_CLICKED(IDC_SEC2_IPFLOAD, OnLoadIPFilter)
	ON_BN_CLICKED(IDC_SEC2_IPFAUTOUPDATE, OnBnClickedChkEnableAutoupdate)
	ON_BN_CLICKED(IDC_SEC2_IPFAUTOUPDATEAIPC, OnSettingsChange)
END_MESSAGE_MAP()

CPPgSecurity2::CPPgSecurity2()
	: CPropertyPage(CPPgSecurity2::IDD)
{
	m_imageList.DeleteImageList();
	m_imageList.Create(16, 16, theApp.m_iDfltImageListColorFlags | ILC_MASK, 14+1, 0);
	m_imageList.Add(CTempIconLoader(_T("SECURITY")));
	m_imageList.Add(CTempIconLoader(_T("IPFILTER")));
}

CPPgSecurity2::~CPPgSecurity2()
{
}

void CPPgSecurity2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_SECURITY2, m_tabCtr);
}

// CPPgSecurity2-Meldungshandler

BOOL CPPgSecurity2::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	InitTab(true,1);
	m_tabCtr.SetCurSel(theApp.emuledlg->preferenceswnd->StartPageSecurity);
	InitWindowStyles(this);

	LoadSettings();
	Localize();

	return TRUE;

}

void CPPgSecurity2::Localize(void)
{
	if(m_hWnd)
	{
		CString strBuffer;

		strBuffer.Format(_T("%s %d)"),GetResString(IDS_SEC2_IPFILTER), thePrefs.GetIPfilterVersion());
		GetDlgItem(IDC_SEC2_IPFILTER)->SetWindowText(strBuffer);
		GetDlgItem(IDC_SEC2_IPFSERVER)->SetWindowText(GetResString(IDS_SEC2_IPFSERVER));
		GetDlgItem(IDC_SEC2_IPFLEVEL_LBL1)->SetWindowText(GetResString(IDS_SEC2_IPFLEVEL_LBL1));
		GetDlgItem(IDC_SEC2_IPFRELOAD)->SetWindowText(GetResString(IDS_SEC2_IPFRELOAD));
		GetDlgItem(IDC_SEC2_IPFEDIT)->SetWindowText(GetResString(IDS_SEC2_IPFEDIT));
		GetDlgItem(IDC_SEC2_IPFUPDATE)->SetWindowText(GetResString(IDS_SEC2_IPFUPDATE));
		GetDlgItem(IDC_SEC2_IPFURL_LBL1)->SetWindowText(GetResString(IDS_SEC2_IPFURL_LBL1));
		GetDlgItem(IDC_SEC2_IPFRESET)->SetWindowText(GetResString(IDS_SEC2_IPFRESET));
		GetDlgItem(IDC_SEC2_IPFLOAD)->SetWindowText(GetResString(IDS_SEC2_IPFLOAD));
		GetDlgItem(IDC_SEC2_IPFAUTOUPDATE)->SetWindowText(GetResString(IDS_SEC2_IPFAUTOUPDATE));
		GetDlgItem(IDC_SEC2_IPFAUTOUPDATEAIPC)->SetWindowText(GetResString(IDS_SEC2_IPFAUTOUPDATEAIPC));
	}
}

void CPPgSecurity2::LoadSettings(void)
{
	CString strBuffer;

	CheckDlgButton(IDC_SEC2_IPFSERVER,			(UINT)thePrefs.filterserverbyip);

	strBuffer.Format(_T("%i"), thePrefs.filterlevel);
	GetDlgItem(IDC_SEC2_IPFLEVEL)->SetWindowText(strBuffer);

	GetDlgItem(IDC_SEC2_IPFURL)->SetWindowText(thePrefs.GetUpdateURLIPFilter());

	if (thePrefs.AutoUpdateIPFilter)
		GetDlgItem(IDC_SEC2_IPFAUTOUPDATEAIPC)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_SEC2_IPFAUTOUPDATEAIPC)->EnableWindow(FALSE);

	CheckDlgButton(IDC_SEC2_IPFAUTOUPDATE,		(UINT)thePrefs.IsAutoUPdateIPFilterEnabled());
	CheckDlgButton(IDC_SEC2_IPFAUTOUPDATEAIPC,	(UINT)thePrefs.IsAutoUpdateIPFilterAIPCEnabled());
}

BOOL CPPgSecurity2::OnApply()
{
	
	CString sBuffer;
	TCHAR buffer[510];

	thePrefs.filterserverbyip = IsDlgButtonChecked(IDC_SEC2_IPFSERVER)!=0;

	if(GetDlgItem(IDC_SEC2_IPFLEVEL)->GetWindowTextLength())
	{
		GetDlgItem(IDC_SEC2_IPFLEVEL)->GetWindowText(buffer,4);
		thePrefs.filterlevel=_tstoi(buffer);
	}

	if(GetDlgItem(IDC_SEC2_IPFURL)->GetWindowTextLength()) 
	{
		GetDlgItem(IDC_SEC2_IPFURL)->GetWindowText(sBuffer);
		_tcscpy(thePrefs.UpdateURLIPFilter, sBuffer);
	}

	thePrefs.AutoUpdateIPFilter = IsDlgButtonChecked(IDC_SEC2_IPFAUTOUPDATE)!=0;
	thePrefs.AutoUpdateIPFilterAIPC = IsDlgButtonChecked(IDC_SEC2_IPFAUTOUPDATEAIPC)!=0;

	SetModified(FALSE);
	LoadSettings();
	return CPropertyPage::OnApply();
}

void CPPgSecurity2::OnBnClickedChkEnableAutoupdate()
{
	if (IsDlgButtonChecked(IDC_SEC2_IPFAUTOUPDATE))
		GetDlgItem(IDC_SEC2_IPFAUTOUPDATEAIPC)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_SEC2_IPFAUTOUPDATEAIPC)->EnableWindow(FALSE);
	SetModified();
}

void CPPgSecurity2::OnReloadIPFilter()
{
	theApp.ipfilter->LoadFromDefaultFile();
}

void CPPgSecurity2::OnEditIPFilter()
{
	ShellExecute(NULL, _T("open"), thePrefs.GetTxtEditor(),
		_T("\"") + thePrefs.GetConfigDir() + DFLT_IPFILTER_FILENAME _T("\""), NULL, SW_SHOW);
}

void CPPgSecurity2::OnResetIPFilterURL()
{
	CString strBuffer = _T("http://emulepawcio.sourceforge.net/nieuwe_site/Ipfilter_fakes/ipfilter.zip");
	GetDlgItem(IDC_SEC2_IPFURL)->SetWindowText(strBuffer);
	thePrefs.m_IPfilterVersion = 0;
	strBuffer.Format(_T("%s %d)"),GetResString(IDS_SEC2_IPFILTER), thePrefs.GetIPfilterVersion());
	GetDlgItem(IDC_SEC2_IPFILTER)->SetWindowText(strBuffer);
}

void CPPgSecurity2::OnLoadIPFilter()
{
	OnApply();
	theApp.ipfilter->UpdateIPFilterURL();
	CString strBuffer;
	strBuffer.Format(_T("%s %d)"),GetResString(IDS_SEC2_IPFILTER), thePrefs.GetIPfilterVersion());
	GetDlgItem(IDC_SEC2_IPFILTER)->SetWindowText(strBuffer);
}

void CPPgSecurity2::InitTab(bool firstinit, int Page)
{
	if (firstinit) {
		m_tabCtr.DeleteAllItems();
		m_tabCtr.SetImageList(&m_imageList);
		m_tabCtr.InsertItem(TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM, Security1, GetResString(IDS_SEC1_NAME), 0, (LPARAM)Security1); 
		m_tabCtr.InsertItem(TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM, Security2, GetResString(IDS_SEC2_NAME), 1, (LPARAM)Security2); 
	}

	m_tabCtr.SetCurSel(Page);
}
void CPPgSecurity2::OnTcnSelchangeTabSecurity2(NMHDR *pNMHDR, LRESULT *pResult)
{
	int cur_sel = m_tabCtr.GetCurSel();
	theApp.emuledlg->preferenceswnd->SwitchTab(cur_sel);
	*pResult = 0;
}
