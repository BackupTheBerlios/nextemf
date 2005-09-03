//this file is part of eMule
//Copyright (C)2002 Merkur ( devs@emule-project.net / http://www.emule-project.net )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#include "stdafx.h"
#include "emule.h"
#include "PreferencesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(CPreferencesDlg, CTreePropSheet)

BEGIN_MESSAGE_MAP(CPreferencesDlg, CTreePropSheet)
	ON_WM_DESTROY()
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

CPreferencesDlg::CPreferencesDlg()
{
	m_psh.dwFlags &= ~PSH_HASHELP;
	m_wndGeneral.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndDisplay.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndConnection.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndServer.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndDirectories.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndFiles.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndStats.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndWebServer.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndTweaks.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndSecurity.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndScheduler.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndNextEMF.m_psp.dwFlags &= ~PSH_HASHELP;// NextEMF-Pref [cyrex2001]
//==> PPgTabControl [shadow2004]
	m_wndConnection2.m_psp.dwFlags &= ~PSH_HASHELP; // Connection 2
	m_wndConnection3.m_psp.dwFlags &= ~PSH_HASHELP; // Connection 3
	m_wndFiles2.m_psp.dwFlags &= ~PSH_HASHELP;			// Files 2
	m_wndSecurity2.m_psp.dwFlags &= ~PSH_HASHELP;		// Security 2
//<== PPgTabControl [shadow2004]
#if defined(_DEBUG) || defined(USE_DEBUG_DEVICE)
	m_wndDebug.m_psp.dwFlags &= ~PSH_HASHELP;
#endif

	CTreePropSheet::SetPageIcon(&m_wndGeneral, _T("Preferences"));
	CTreePropSheet::SetPageIcon(&m_wndDisplay, _T("DISPLAY"));
	CTreePropSheet::SetPageIcon(&m_wndConnection, _T("CONNECTION"));
	CTreePropSheet::SetPageIcon(&m_wndServer, _T("SERVER"));
	CTreePropSheet::SetPageIcon(&m_wndDirectories, _T("FOLDERS"));
	CTreePropSheet::SetPageIcon(&m_wndFiles, _T("Transfer"));
	CTreePropSheet::SetPageIcon(&m_wndNotify, _T("NOTIFICATIONS"));
	CTreePropSheet::SetPageIcon(&m_wndStats, _T("STATISTICS"));
	CTreePropSheet::SetPageIcon(&m_wndSecurity, _T("SECURITY"));
	CTreePropSheet::SetPageIcon(&m_wndScheduler, _T("SCHEDULER"));
	CTreePropSheet::SetPageIcon(&m_wndWebServer, _T("WEB"));
	CTreePropSheet::SetPageIcon(&m_wndTweaks, _T("TWEAK"));
	CTreePropSheet::SetPageIcon(&m_wndNextEMF, _T("CLIENT_NEXTEMF"));// NextEMF-Pref [cyrex2001]
#if defined(_DEBUG) || defined(USE_DEBUG_DEVICE)
	CTreePropSheet::SetPageIcon(&m_wndDebug, _T("Preferences"));
#endif

	AddPage(&m_wndGeneral);									// 0
	AddPage(&m_wndDisplay);									// 1
	AddPage(&m_wndConnection);								// 2
	AddPage(&m_wndServer);									// 3
	AddPage(&m_wndDirectories);								// 4
	AddPage(&m_wndFiles);									// 5
	AddPage(&m_wndNotify);									// 6
	AddPage(&m_wndStats);									// 7
	AddPage(&m_wndSecurity);								// 8
	AddPage(&m_wndScheduler);								// 9
	AddPage(&m_wndWebServer);								// 10
	AddPage(&m_wndTweaks);									// 11
	AddPage(&m_wndNextEMF);// NextEMF-Pref [cyrex2001]		// 12

//==> PPgTabControl [shadow2004]
	AddPage(&m_wndConnection2);								// 13
	AddPage(&m_wndConnection3);								// 14
	AddPage(&m_wndFiles2);									// 15
	AddPage(&m_wndSecurity2);								// 16
//<== PPgTabControl [shadow2004]
#if defined(_DEBUG) || defined(USE_DEBUG_DEVICE)
	AddPage(&m_wndDebug);									// 17
#endif

	SetTreeViewMode(TRUE, TRUE, TRUE);
	SetTreeWidth(170);

	m_pPshStartPage = NULL;
	m_bSaveIniFile = false;
//==> PPgTabControl [shadow2004]
#ifdef PPGCTRL
/*	ActivePageGeneral			= 0;
	ActivePageDisplay			= 0;*/
	ActivePageConnection		= 0;
/*	ActivePageServer			= 0;
	ActivePageDirectory			= 0;*/
	ActivePageFiles				= 0;
	/*ActivePageNotify			= 0;
	ActivePageStats				= 0;*/
	ActivePageSecurity			= 0;
/*	ActivePageSheduler			= 0;
	ActivePageWebServer			= 0;
	ActivePageTweaks			= 0;
	ActivePageDebug				= 0;

	StartPageGeneral			= 0;
	StartPageDisplay			= 0;*/
	StartPageConnection			= 0;
/*	StartPageServer				= 0;
	StartPageDirectory			= 0;
	StartPageFiles				= 0;
	StartPageNotify				= 0;
	StartPageStats				= 0;*/
	StartPageSecurity			= 0;
/*	StartPageSheduler			= 0;
	StartPageWebServer			= 0;
	StartPageTweaks				= 0;
	StartPageDebug				= 0;*/

#endif
//<== PPgTabControl [shadow2004]
}

CPreferencesDlg::~CPreferencesDlg()
{
}

void CPreferencesDlg::OnDestroy()
{
	CTreePropSheet::OnDestroy();
	if (m_bSaveIniFile)
	{
		thePrefs.Save();
		m_bSaveIniFile = false;
	}
	m_pPshStartPage = GetPage(GetActiveIndex())->m_psp.pszTemplate;
}

BOOL CPreferencesDlg::OnInitDialog()
{
	ASSERT( !m_bSaveIniFile );
	BOOL bResult = CTreePropSheet::OnInitDialog();
	InitWindowStyles(this);

//==> PPgTabControl [shadow2004]
#ifdef PPGCTRL
	int nPage = 0;
#endif
//<== PPgTabControl [shadow2004]
	for (int i = 0; i < m_pages.GetSize(); i++)
	{
		if (GetPage(i)->m_psp.pszTemplate == m_pPshStartPage)
		{

//==> PPgTabControl [shadow2004]
#ifdef PPGCTRL

			if (i>=13)
			{
				if (i == 13) 
				{	// Connection2 / Reask&Quickstart
					SetActivePage(2);
			    }
				if (i == 14) 
				{	// Connection3 / Hardlimit
					SetActivePage(2);
				}
				if (i == 15) 
				{	// Files 2 / Nicehash
					SetActivePage(5);
				}
				if (i == 16) 
				{	// Security2 / IP-Filter
					SetActivePage(8);
				}


					m_wndConnection.InitTab(false,StartPageConnection);
					m_wndConnection2.InitTab(false,StartPageConnection);
					m_wndConnection3.InitTab(false,StartPageConnection);
					m_wndFiles.InitTab(false,StartPageFiles);
					m_wndFiles2.InitTab(false,StartPageFiles);
					m_wndSecurity.InitTab(false,StartPageSecurity);
					m_wndSecurity2.InitTab(false,StartPageSecurity);
					break;
				}
				else
				{
			SetActivePage(i);
			break;
		}

#endif
//<== PPgTabControl [shadow2004]

			}
	}

	Localize();	
	return bResult;
}

void CPreferencesDlg::Localize()
{
	SetTitle(RemoveAmbersand(GetResString(IDS_EM_PREFS))); 

	m_wndGeneral.Localize();
	m_wndDisplay.Localize();
	m_wndConnection.Localize();
	m_wndServer.Localize();
	m_wndDirectories.Localize();
	m_wndFiles.Localize();
	m_wndStats.Localize();
	m_wndNotify.Localize();
	m_wndSecurity.Localize();
	m_wndTweaks.Localize();
	m_wndWebServer.Localize();
	m_wndScheduler.Localize();
	m_wndNextEMF.Localize();// NextEMF-Pref [cyrex2001]

	CTreeCtrl* pTree = GetPageTreeControl();
	if (pTree)
	{
		pTree->SetItemText(GetPageTreeItem(0), RemoveAmbersand(GetResString(IDS_PW_GENERAL)));
		pTree->SetItemText(GetPageTreeItem(1), RemoveAmbersand(GetResString(IDS_PW_DISPLAY))); 
		pTree->SetItemText(GetPageTreeItem(2), RemoveAmbersand(GetResString(IDS_PW_CONNECTION))); 
		pTree->SetItemText(GetPageTreeItem(3), RemoveAmbersand(GetResString(IDS_PW_SERVER))); 
		pTree->SetItemText(GetPageTreeItem(4), RemoveAmbersand(GetResString(IDS_PW_DIR))); 
		pTree->SetItemText(GetPageTreeItem(5), RemoveAmbersand(GetResString(IDS_PW_FILES))); 
		pTree->SetItemText(GetPageTreeItem(6), RemoveAmbersand(GetResString(IDS_PW_EKDEV_OPTIONS))); 
		pTree->SetItemText(GetPageTreeItem(7), RemoveAmbersand(GetResString(IDS_STATSSETUPINFO))); 
		pTree->SetItemText(GetPageTreeItem(8), RemoveAmbersand(GetResString(IDS_SECURITY))); 
		pTree->SetItemText(GetPageTreeItem(9), RemoveAmbersand(GetResString(IDS_SCHEDULER)));
		pTree->SetItemText(GetPageTreeItem(10), RemoveAmbersand(GetResString(IDS_PW_WS)));
		pTree->SetItemText(GetPageTreeItem(11), RemoveAmbersand(GetResString(IDS_PW_TWEAK)));
		pTree->SetItemText(GetPageTreeItem(12), RemoveAmbersand(GetResString(IDS_NEXTEMF)));// NextEMF-Pref [cyrex200]
//==> PPgTabControl [shadow2004]
		pTree->SetItemText(GetPageTreeItem(13), RemoveAmbersand(_T(" "))); // Connection 2
		pTree->SetItemText(GetPageTreeItem(14), RemoveAmbersand(_T(" "))); // Connection 3
		pTree->SetItemText(GetPageTreeItem(15), RemoveAmbersand(_T(" "))); // Files 2
		pTree->SetItemText(GetPageTreeItem(16), RemoveAmbersand(_T(" "))); // Security 2
//<== PPgTabControl [shadow2004]
	#if defined(_DEBUG) || defined(USE_DEBUG_DEVICE)
		pTree->SetItemText(GetPageTreeItem(17), _T("Debug"));
	#endif
	}

	UpdateCaption();
}

void CPreferencesDlg::OnHelp()
{
	int iCurSel = GetActiveIndex();
	if (iCurSel >= 0)
	{
		CPropertyPage* pPage = GetPage(iCurSel);
		if (pPage)
		{
			HELPINFO hi = {0};
			hi.cbSize = sizeof hi;
			hi.iContextType = HELPINFO_WINDOW;
			hi.iCtrlId = 0;
			hi.hItemHandle = pPage->m_hWnd;
			hi.dwContextId = 0;
			pPage->SendMessage(WM_HELP, 0, (LPARAM)&hi);
			return;
		}
	}

	theApp.ShowHelp(0, HELP_CONTENTS);
}

BOOL CPreferencesDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (wParam == ID_HELP)
	{
		OnHelp();
		return TRUE;
	}
	if (wParam == IDOK || wParam == ID_APPLY_NOW)
		m_bSaveIniFile = true;
	return __super::OnCommand(wParam, lParam);
}

BOOL CPreferencesDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	OnHelp();
	return TRUE;
}

void CPreferencesDlg::SetStartPage(UINT uStartPageID)
{
	m_pPshStartPage = MAKEINTRESOURCE(uStartPageID);
}

//==> PPgTabControl [shadow2004]
#ifdef PPGCTRL
void CPreferencesDlg::SwitchTab(int Page)
{
	
	if(m_hWnd && IsWindowVisible()){
		CPropertyPage* activepage = GetActivePage();

		// Connection 1-3
		if (activepage == &m_wndConnection || activepage == &m_wndConnection2 || activepage == &m_wndConnection3){
			if (Page == 0) {
				SetActivePage(&m_wndConnection);
				ActivePageConnection = 0;
				StartPageConnection = 0;
				m_wndConnection.InitTab(false,0);
			}
			if (Page == 1) {
				SetActivePage(&m_wndConnection2);
				ActivePageConnection = 13;
				StartPageConnection = 1;
				m_wndConnection2.InitTab(false,1);
			}
			if (Page == 2) {
				SetActivePage(&m_wndConnection3);
				ActivePageConnection = 14;
				StartPageConnection = 2;
				m_wndConnection3.InitTab(false,2);
			}
		}

		// Files 1-2
		if (activepage == &m_wndFiles || activepage == &m_wndFiles2){
			if (Page == 0) {
				SetActivePage(&m_wndFiles);
				ActivePageFiles = 0;
				StartPageFiles = 0;
				m_wndFiles.InitTab(false,0);
			}
			if (Page == 1) {
				SetActivePage(&m_wndFiles2);
				ActivePageFiles = 15;
				StartPageFiles = 1;
				m_wndFiles2.InitTab(false,1);
			}
		}

		// Security 1-2
		if (activepage == &m_wndSecurity || activepage == &m_wndSecurity2){
			if (Page == 0) {
				SetActivePage(&m_wndSecurity);
				ActivePageSecurity = 0;
				StartPageSecurity = 0;
				m_wndSecurity.InitTab(false,0);
			}
			if (Page == 1) {
				SetActivePage(&m_wndSecurity2);
				ActivePageSecurity = 16;
				StartPageSecurity = 1;
				m_wndSecurity2.InitTab(false,1);
			}
		}
	}
}
#endif
//<== PPgTabControl [shadow2004]
