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
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
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
//==> remove IRC [shadow2004]
#if defined(IRC)
	m_wndIRC.m_psp.dwFlags &= ~PSH_HASHELP;
#endif //IRC
//<== remove IRC [shadow2004]
	m_wndWebServer.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndTweaks.m_psp.dwFlags &= ~PSH_HASHELP;
	m_wndSecurity.m_psp.dwFlags &= ~PSH_HASHELP;
//==> remove scheduler [shadow2004]
#if defined(SCHEDULER)
	m_wndScheduler.m_psp.dwFlags &= ~PSH_HASHELP;
#endif //SCHEDULER
//<== remove scheduler [shadow2004]
//==> remove PROXY [shadow2004]
#if defined(PROXY)
	m_wndProxy.m_psp.dwFlags &= ~PSH_HASHELP;
#endif //PROXY
//<== remove PROXY [shadow2004]
//==>Reask sourcen after ip chnage or Quickstart [cyrex2001]
#if defined (RSAIC_SIVKA) || defined (QUICKSTART) //Reask sourcen after ip chnage or Quickstart
	m_wndNextEMF.m_psp.dwFlags &= ~PSH_HASHELP;
#endif //Reask sourcen after ip chnage or Quickstart
//<==Reask sourcen after ip chnage or Quickstart [cyrex2001]
#if defined(_DEBUG) || defined(USE_DEBUG_DEVICE)
	m_wndDebug.m_psp.dwFlags &= ~PSH_HASHELP;
#endif

	CTreePropSheet::SetPageIcon(&m_wndGeneral, _T("Preferences"));
	CTreePropSheet::SetPageIcon(&m_wndDisplay, _T("DISPLAY"));
	CTreePropSheet::SetPageIcon(&m_wndConnection, _T("CONNECTION"));
//==> remove PROXY [shadow2004]
#if defined(PROXY)
	CTreePropSheet::SetPageIcon(&m_wndProxy, _T("PROXY"));
#endif //PROXY
//<== remove PROXY [shadow2004]

	CTreePropSheet::SetPageIcon(&m_wndServer, _T("SERVER"));
	CTreePropSheet::SetPageIcon(&m_wndDirectories, _T("FOLDERS"));
	CTreePropSheet::SetPageIcon(&m_wndFiles, _T("SharedFiles"));
	CTreePropSheet::SetPageIcon(&m_wndNotify, _T("NOTIFICATIONS"));
	CTreePropSheet::SetPageIcon(&m_wndStats, _T("STATISTICS"));
//==> remove IRC [shadow2004]
#if defined(IRC)
	CTreePropSheet::SetPageIcon(&m_wndIRC, _T("IRC"));
#endif //IRC
//<== remove IRC [shadow2004]
	CTreePropSheet::SetPageIcon(&m_wndSecurity, _T("SECURITY"));
//==> remove scheduler [shadow2004]
#if defined(SCHEDULER)
	CTreePropSheet::SetPageIcon(&m_wndScheduler, _T("SCHEDULER"));
#endif //SCHEDULER
//<== remove scheduler [shadow2004]
	CTreePropSheet::SetPageIcon(&m_wndWebServer, _T("WEB"));
	CTreePropSheet::SetPageIcon(&m_wndTweaks, _T("TWEAK"));
#if defined(_DEBUG) || defined(USE_DEBUG_DEVICE)
	CTreePropSheet::SetPageIcon(&m_wndDebug, _T("Preferences"));
#endif
//==>Reask sourcen after ip chnage or Quickstart [cyrex2001]
#if defined (RSAIC_SIVKA) || defined (QUICKSTART)
	CTreePropSheet::SetPageIcon(&m_wndNextEMF, _T("CLIENT_NEXTEMF"));
#endif //Reask sourcen after ip chnage or Quickstart
//<==Reask sourcen after ip chnage or Quickstart [cyrex2001]
	AddPage(&m_wndGeneral);
	AddPage(&m_wndDisplay);
	AddPage(&m_wndConnection);
//==> remove PROXY [shadow2004]
#if defined(PROXY)
	AddPage(&m_wndProxy);
#endif //PROXY
//<== remove PROXY [shadow2004]
	AddPage(&m_wndServer);
	AddPage(&m_wndDirectories);
	AddPage(&m_wndFiles);
	AddPage(&m_wndNotify);
	AddPage(&m_wndStats);
//==> remove IRC [shadow2004]
#if defined(IRC)
	AddPage(&m_wndIRC);
#endif //IRC
//<== remove IRC [shadow2004]
	AddPage(&m_wndSecurity);
//==> remove scheduler [shadow2004]
#if defined(SCHEDULER)
	AddPage(&m_wndScheduler);
#endif //SCHEDULER
//<== remove scheduler [shadow2004]
	AddPage(&m_wndWebServer);
	AddPage(&m_wndTweaks);
//==>Reask sourcen after ip chnage or Quickstart [cyrex2001]
#if defined (RSAIC_SIVKA) || defined (QUICKSTART) //Reask sourcen after ip chnage or Quickstart
	AddPage(&m_wndNextEMF);
#endif //Reask sourcen after ip chnage or Quickstart
//<==Reask sourcen after ip chnage or Quickstart [cyrex2001]
#if defined(_DEBUG) || defined(USE_DEBUG_DEVICE)
	AddPage(&m_wndDebug);
#endif

	SetTreeViewMode(TRUE, TRUE, TRUE);
	SetTreeWidth(170);

	m_nActiveWnd = 0;
	m_iPrevPage = -1;
}

CPreferencesDlg::~CPreferencesDlg()
{
}

void CPreferencesDlg::OnDestroy()
{
	CTreePropSheet::OnDestroy();
//==> Save Preferences only if needed by WiZaRd [shadow2004]
#ifndef FIX04
	thePrefs.Save();
#endif
//<== Save Preferences only if needed by WiZaRd [shadow2004]
	m_nActiveWnd = GetActiveIndex();
}

BOOL CPreferencesDlg::OnInitDialog()
{
	BOOL bResult = CTreePropSheet::OnInitDialog();
	InitWindowStyles(this);
	SetActivePage(m_nActiveWnd);

	Localize();	
	m_iPrevPage = GetActiveIndex();
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
//==> remove IRC [shadow2004]
#if defined(IRC)
	m_wndIRC.Localize();
#endif //IRC
//<== remove IRC [shadow2004]
	m_wndSecurity.Localize();
	m_wndTweaks.Localize();
	m_wndWebServer.Localize();
//==> remove scheduler [shadow2004]
#if defined(SCHEDULER)
	m_wndScheduler.Localize();
#endif //SCHEDULER
//<== remove scheduler [shadow2004]
//==> remove PROXY [shadow2004]
#if defined(PROXY)
	m_wndProxy.Localize();
#endif //PROXY
//<== remove PROXY [shadow2004]
//==>Reask sourcen after ip chnage or Quickstart [cyrex2001]
#if defined (RSAIC_SIVKA) || defined (QUICKSTART) //Reask sourcen after ip chnage or Quickstart
	m_wndNextEMF.Localize();
#endif //Reask sourcen after ip chnage or Quickstart
//<==Reask sourcen after ip chnage or Quickstart [cyrex2001]

	CTreeCtrl* pTree = GetPageTreeControl();
	if (pTree)
	{
		pTree->SetItemText(GetPageTreeItem(0), RemoveAmbersand(GetResString(IDS_PW_GENERAL)));
		pTree->SetItemText(GetPageTreeItem(1), RemoveAmbersand(GetResString(IDS_PW_DISPLAY))); 
		pTree->SetItemText(GetPageTreeItem(2), RemoveAmbersand(GetResString(IDS_PW_CONNECTION))); 
//==> remove PROXY [shadow2004]
#if defined(PROXY)
		pTree->SetItemText(GetPageTreeItem(3), RemoveAmbersand(GetResString(IDS_PW_PROXY))); 
#endif //PROXY
//<== remove PROXY [shadow2004]
		pTree->SetItemText(GetPageTreeItem(3), RemoveAmbersand(GetResString(IDS_PW_SERVER))); //4
		pTree->SetItemText(GetPageTreeItem(4), RemoveAmbersand(GetResString(IDS_PW_DIR))); //5
		pTree->SetItemText(GetPageTreeItem(5), RemoveAmbersand(GetResString(IDS_PW_FILES))); //6
		pTree->SetItemText(GetPageTreeItem(6), RemoveAmbersand(GetResString(IDS_PW_EKDEV_OPTIONS))); //7
		pTree->SetItemText(GetPageTreeItem(7), RemoveAmbersand(GetResString(IDS_STATSSETUPINFO))); //8
//==> remove IRC [shadow2004]
#if defined(IRC)
		pTree->SetItemText(GetPageTreeItem(9), RemoveAmbersand(GetResString(IDS_IRC)));
#endif //IRC
//<== remove IRC [shadow2004]
		pTree->SetItemText(GetPageTreeItem(8), RemoveAmbersand(GetResString(IDS_SECURITY))); //10
//==> remove scheduler [shadow2004]
#if defined(SCHEDULER)
		pTree->SetItemText(GetPageTreeItem(9), RemoveAmbersand(GetResString(IDS_SCHEDULER)));//11
#endif //SCHEDULER
//<== remove scheduler [shadow2004]
		pTree->SetItemText(GetPageTreeItem(9), RemoveAmbersand(GetResString(IDS_PW_WS)));//12
		pTree->SetItemText(GetPageTreeItem(10), RemoveAmbersand(GetResString(IDS_PW_TWEAK)));//13
//==>Reask sourcen after ip chnage or Quickstart [cyrex2001]
#if defined (RSAIC_SIVKA) || defined (QUICKSTART) //Reask sourcen after ip chnage or Quickstart
		pTree->SetItemText(GetPageTreeItem(11), RemoveAmbersand(GetResString(IDS_PW_NEXTEMF)/*_T(NextEMF)*/));//14
#endif //Reask sourcen after ip chnage or Quickstart
//<==Reask sourcen after ip chnage or Quickstart [cyrex2001]
	#if defined(_DEBUG) || defined(USE_DEBUG_DEVICE)
		pTree->SetItemText(GetPageTreeItem(12), _T("Debug"));//15
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
//==> Save Preferences only if needed by WiZaRd [shadow2004]
#ifdef FIX04
	// crack message parameters
	UINT nID = LOWORD(wParam);
	HWND hWndCtrl = (HWND)lParam;
	int nCode = HIWORD(wParam);

	int m_nModalResult = 0;
	// set m_nModalResult to ID of button, whenever button is clicked
	if (hWndCtrl && nCode == BN_CLICKED)
	{	
		if (::SendMessage(hWndCtrl, WM_GETDLGCODE, 0, 0) &
			(DLGC_BUTTON|DLGC_DEFPUSHBUTTON))
		{
			LONG lStyle = ::GetWindowLong(hWndCtrl, GWL_STYLE) & 0x0F;
			if (lStyle == BS_PUSHBUTTON || lStyle == BS_DEFPUSHBUTTON ||
				lStyle == BS_USERBUTTON || lStyle == BS_OWNERDRAW)
			{
				m_nModalResult = nID;
			}
		}
	}
    //Save only if needed...
	if(m_nModalResult == IDOK || m_nModalResult == ID_APPLY_NOW)
		thePrefs.Save();
#endif
//<== Save Preferences only if needed by WiZaRd [shadow2004]
	return __super::OnCommand(wParam, lParam);
}

BOOL CPreferencesDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	OnHelp();
	return TRUE;
}
