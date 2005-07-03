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
#include "MuleToolbarCtrl.h"
#include "SearchDlg.h"
#include "KademliaWnd.h"
#include "EnBitmap.h"
#include "OtherFunctions.h"
#include "emuledlg.h"
#include "Sockets.h"
#include "MenuCmds.h"
#include "MuleStatusbarCtrl.h"
#include "ServerWnd.h"
#include "TransferWnd.h"
#include "SharedFilesWnd.h"
#include "ChatWnd.h"
#include "StatisticsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define	NUM_BUTTON_BITMAPS	14

// CMuleToolbarCtrl

IMPLEMENT_DYNAMIC(CMuleToolbarCtrl, CToolBarCtrl)
 
CMuleToolbarCtrl::CMuleToolbarCtrl()
{
	m_sizBtnBmp.cx = 32;
	m_sizBtnBmp.cy = 32;
	m_iPreviousHeight = 0;
	m_iLastPressedButton = -1;
	m_buttoncount = 0;
//==> Optimizer [shadow2004]
#ifdef OPTIM
	memzero(TBButtons, sizeof(TBButtons));
	memzero(TBStrings, sizeof(TBStrings));
#else
	memset(TBButtons, 0, sizeof(TBButtons));
	memset(TBStrings, 0, sizeof(TBStrings));
#endif
//<== Optimizer [shadow2004]
}

CMuleToolbarCtrl::~CMuleToolbarCtrl()
{
	if (m_bmpBack.m_hObject)
		VERIFY( m_bmpBack.DeleteObject() );
}

void CMuleToolbarCtrl::Init(void)
{
	ModifyStyle(0, TBSTYLE_FLAT | TBSTYLE_ALTDRAG | CCS_ADJUSTABLE | TBSTYLE_TRANSPARENT | TBSTYLE_TOOLTIPS | CCS_NODIVIDER);
	if (thePrefs.GetUseReBarToolbar())
	{
		ModifyStyle(0, CCS_NORESIZE);
		SetExtendedStyle(GetExtendedStyle() | TBSTYLE_EX_HIDECLIPPEDBUTTONS);
	}

	ChangeToolbarBitmap();

	// add button-text:
	TCHAR cButtonStrings[2000];
	int lLen, lLen2;
	m_buttoncount=0;
	
	_tcscpy(cButtonStrings, GetResString(IDS_MAIN_BTN_CONNECT));
	lLen = _tcslen(GetResString(IDS_MAIN_BTN_CONNECT)) + 1;
	++m_buttoncount;

	lLen2 = _tcslen(GetResString(IDS_EM_KADEMLIA)) + 1;
	memcpy(cButtonStrings+lLen, GetResString(IDS_EM_KADEMLIA), lLen2*sizeof(TCHAR));
	lLen += lLen2;
	++m_buttoncount;

	lLen2 = _tcslen(GetResString(IDS_EM_SERVER)) + 1;
	memcpy(cButtonStrings+lLen, GetResString(IDS_EM_SERVER), lLen2*sizeof(TCHAR));
	lLen += lLen2;
	++m_buttoncount;

	lLen2 = _tcslen(GetResString(IDS_EM_TRANS)) + 1;
	memcpy(cButtonStrings+lLen, GetResString(IDS_EM_TRANS), lLen2*sizeof(TCHAR));
	lLen += lLen2;
	++m_buttoncount;

	lLen2 = _tcslen(GetResString(IDS_EM_SEARCH)) + 1;
	memcpy(cButtonStrings+lLen, GetResString(IDS_EM_SEARCH), lLen2*sizeof(TCHAR));
	lLen += lLen2;
	++m_buttoncount;

	lLen2 = _tcslen(GetResString(IDS_EM_FILES)) + 1;
	memcpy(cButtonStrings+lLen, GetResString(IDS_EM_FILES), lLen2*sizeof(TCHAR));
	lLen += lLen2;
	++m_buttoncount;

	lLen2 = _tcslen(GetResString(IDS_EM_MESSAGES)) + 1;
	memcpy(cButtonStrings+lLen, GetResString(IDS_EM_MESSAGES), lLen2*sizeof(TCHAR));
	lLen += lLen2;
	++m_buttoncount;

	lLen2 = _tcslen(GetResString(IDS_EM_STATISTIC)) + 1;
	memcpy(cButtonStrings+lLen, GetResString(IDS_EM_STATISTIC), lLen2*sizeof(TCHAR));
	lLen += lLen2;
	++m_buttoncount;

	lLen2 = _tcslen(GetResString(IDS_EM_PREFS)) + 1;
	memcpy(cButtonStrings+lLen, GetResString(IDS_EM_PREFS), lLen2*sizeof(TCHAR));
	lLen += lLen2;
	++m_buttoncount;

	lLen2 = _tcslen(GetResString(IDS_TOOLS)) + 1;
	memcpy(cButtonStrings+lLen, GetResString(IDS_TOOLS), lLen2*sizeof(TCHAR));
	lLen += lLen2;
	++m_buttoncount;

	lLen2 = _tcslen(GetResString(IDS_EM_HELP)) + 1;
	memcpy(cButtonStrings+lLen, GetResString(IDS_EM_HELP), lLen2*sizeof(TCHAR));
	lLen += lLen2;
	++m_buttoncount;

	// terminate
	memcpy(cButtonStrings+lLen, _T("\0"), sizeof(TCHAR));

	AddStrings(cButtonStrings);

	// initialize buttons:	
	for(int i = 0; i < m_buttoncount; i++)
	{		
		TBButtons[i].fsState	= TBSTATE_ENABLED;
		TBButtons[i].fsStyle	= TBSTYLE_CHECKGROUP;
		TBButtons[i].idCommand	= IDC_TOOLBARBUTTON + i;
		TBButtons[i].iString	= i;

		switch (TBButtons[i].idCommand)
		{
			case TBBTN_CONNECT:
			case TBBTN_OPTIONS:
			case TBBTN_TOOLS:
			case TBBTN_HELP:
			TBButtons[i].fsStyle = TBSTYLE_BUTTON;
			break;
		}
	}

	// set button image indices
	int iBitmap = 0;
	for(int i = 0; i < m_buttoncount; i++)
	{		
		TBButtons[i].iBitmap = iBitmap;
		if (TBButtons[i].idCommand == TBBTN_CONNECT) // 'Connect' button has 3 states
			iBitmap += 3;
		else
			iBitmap += 1;
	}
	
	TBBUTTON sepButton = {0};
	sepButton.idCommand = 0;
	sepButton.fsStyle = TBSTYLE_SEP;
	sepButton.fsState = TBSTATE_ENABLED;
	sepButton.iString = -1;
	sepButton.iBitmap = -1;
	
	CString config = thePrefs.GetToolbarSettings();
	for (i = 0; i < config.GetLength(); i += 2)
	{
		int index = _tstoi(config.Mid(i, 2));
		if (index == 99)
		{
			AddButtons(1, &sepButton);
			continue;
		}
		AddButtons(1, &TBButtons[index]);
	}

	// recalc toolbar-size
	SetAllButtonsStrings();
	ChangeTextLabelStyle();
	SetAllButtonsWidth();	// then calc and set the button width
	AutoSize();				// and finally call the original (but maybe obsolete) function
	SaveCurHeight();
}

void CMuleToolbarCtrl::SetAllButtonsStrings()
{
	static const int TBStringIDs[] =
	{
		IDS_EM_KADEMLIA, 
		IDS_EM_SERVER,
		IDS_EM_TRANS,
		IDS_EM_SEARCH,
		IDS_EM_FILES,
		IDS_EM_MESSAGES,
		IDS_EM_STATISTIC,
		IDS_EM_PREFS,
		IDS_TOOLS,
		IDS_EM_HELP
	};
	TBBUTTONINFO tbi;
	tbi.dwMask = TBIF_TEXT;
	tbi.cbSize = sizeof(TBBUTTONINFO);

	CString buffer;
	if (theApp.serverconnect->IsConnected())
		buffer = GetResString(IDS_MAIN_BTN_DISCONNECT);
	else if (theApp.serverconnect->IsConnecting())
		buffer = GetResString(IDS_MAIN_BTN_CANCEL);
	else
		buffer = GetResString(IDS_MAIN_BTN_CONNECT);

	_sntprintf(TBStrings[0], ARRSIZE(TBStrings[0]), _T("%s"), buffer);
	tbi.pszText = TBStrings[0];
	SetButtonInfo(IDC_TOOLBARBUTTON+0, &tbi);

	for (int i = 1; i < m_buttoncount; i++)
	{
		_sntprintf(TBStrings[i], ARRSIZE(TBStrings[0]), _T("%s"), GetResString(TBStringIDs[i-1]));
		tbi.pszText = TBStrings[i];
		SetButtonInfo(IDC_TOOLBARBUTTON+i, &tbi);
	}
}

void CMuleToolbarCtrl::Localize(void)
{
	if (m_hWnd)
	{
		SetAllButtonsStrings();
		SetAllButtonsWidth();
		AutoSize();
		UpdateIdealSize();
	}
}

void CMuleToolbarCtrl::SetAllButtonsWidth()
{
	if (GetButtonCount() == 0)
		return;

		CDC *pDC = GetDC();
		CFont *pFnt = GetFont();
		CFont *pOldFnt = pDC->SelectObject(pFnt);
		CRect r(0,0,0,0);

		// calculate the max. possible button-size
		int iCalcSize = 0;
		for (int i = 0; i < m_buttoncount ; i++)
		{
			if (!IsButtonHidden(IDC_TOOLBARBUTTON + i))
			{
				pDC->DrawText(TBStrings[i], -1, r, DT_SINGLELINE | DT_CALCRECT);
 				if (r.Width() > iCalcSize)
					iCalcSize = r.Width();
			}
		}
		iCalcSize += 10;

		pDC->SelectObject(pOldFnt);
		ReleaseDC(pDC);

		if (!thePrefs.GetUseReBarToolbar())
		{
			GetClientRect(&r);
			int bc = GetButtonCount();
			if (bc == 0)
				bc = 1;
			int iMaxPossible = r.Width() / bc;

			// if the buttons are to big, reduze their size
			if (iCalcSize > iMaxPossible)
				iCalcSize = iMaxPossible;
		}
		else
		{
			if (iCalcSize < 56)
				iCalcSize = 56;
			else if (iCalcSize > 70)
				iCalcSize = 70;
		}
		SetButtonWidth(iCalcSize, iCalcSize);
}

void CMuleToolbarCtrl::ChangeToolbarBitmap()
{
	CImageList ImageList;
		// load from icon ressources
		ImageList.Create(m_sizBtnBmp.cx, m_sizBtnBmp.cy, theApp.m_iDfltImageListColorFlags | ILC_MASK, 0, 1);
		ImageList.Add(CTempIconLoader(_T("CONNECT"), m_sizBtnBmp.cx, m_sizBtnBmp.cy));
		ImageList.Add(CTempIconLoader(_T("DISCONNECT"), m_sizBtnBmp.cx, m_sizBtnBmp.cy));
		ImageList.Add(CTempIconLoader(_T("STOPCONNECTING"), m_sizBtnBmp.cx, m_sizBtnBmp.cy));
		ImageList.Add(CTempIconLoader(_T("KADEMLIA"), m_sizBtnBmp.cx, m_sizBtnBmp.cy));
		ImageList.Add(CTempIconLoader(_T("SERVER"), m_sizBtnBmp.cx, m_sizBtnBmp.cy));
		ImageList.Add(CTempIconLoader(_T("TRANSFER"), m_sizBtnBmp.cx, m_sizBtnBmp.cy));
		ImageList.Add(CTempIconLoader(_T("SEARCH"), m_sizBtnBmp.cx, m_sizBtnBmp.cy));
		ImageList.Add(CTempIconLoader(_T("SharedFiles"), m_sizBtnBmp.cx, m_sizBtnBmp.cy));
		ImageList.Add(CTempIconLoader(_T("MESSAGES"), m_sizBtnBmp.cx, m_sizBtnBmp.cy));
		ImageList.Add(CTempIconLoader(_T("IRC"), m_sizBtnBmp.cx, m_sizBtnBmp.cy));
		ImageList.Add(CTempIconLoader(_T("STATISTICS"), m_sizBtnBmp.cx, m_sizBtnBmp.cy));
		ImageList.Add(CTempIconLoader(_T("PREFERENCES"), m_sizBtnBmp.cx, m_sizBtnBmp.cy));
		ImageList.Add(CTempIconLoader(_T("TOOLS"), m_sizBtnBmp.cx, m_sizBtnBmp.cy));
		ImageList.Add(CTempIconLoader(_T("HELP"), m_sizBtnBmp.cx, m_sizBtnBmp.cy));
		ASSERT( ImageList.GetImageCount() == NUM_BUTTON_BITMAPS );
		CImageList* pimlOld = SetImageList(&ImageList);
		ImageList.Detach();
		if (pimlOld)
			pimlOld->DeleteImageList();
}

void CMuleToolbarCtrl::ChangeTextLabelStyle()
{
				SetStyle(GetStyle() & ~TBSTYLE_LIST);
				SetMaxTextRows(1);

		for (int i = 0; i < m_buttoncount; i++)
		{	
			TBBUTTONINFO tbbi = {0};
			tbbi.cbSize = sizeof(tbbi);
			tbbi.dwMask = TBIF_STYLE;
			GetButtonInfo(IDC_TOOLBARBUTTON + i, &tbbi);
				tbbi.fsStyle &= ~TBSTYLE_AUTOSIZE;
			SetButtonInfo(IDC_TOOLBARBUTTON + i, &tbbi);
		}
}

void CMuleToolbarCtrl::PressMuleButton(int nID)
{
	// Customization might splits up the button-group, so we have to (un-)press them on our own
	if (m_iLastPressedButton != -1)
		CheckButton(m_iLastPressedButton, FALSE);
	CheckButton(nID, TRUE);
	m_iLastPressedButton = nID;
}

void CMuleToolbarCtrl::UpdateIdealSize()
{
	if (theApp.emuledlg->m_ctlMainTopReBar.m_hWnd)
	{
		// let the rebar know what's our new current ideal size, so the chevron is handled correctly..
		CSize sizeBar;
		GetMaxSize(&sizeBar);
		ASSERT( sizeBar.cx != 0 && sizeBar.cy != 0 );

	    REBARBANDINFO rbbi = {0};
	    rbbi.cbSize = sizeof(rbbi);
	    rbbi.fMask = RBBIM_IDEALSIZE;
		rbbi.cxIdeal = sizeBar.cx;
	    VERIFY( theApp.emuledlg->m_ctlMainTopReBar.SetBandInfo(MULE_TOOLBAR_BAND_NR, &rbbi) );
	}
}

void CMuleToolbarCtrl::AutoSize()
{
	CToolBarCtrl::AutoSize();
}

void CMuleToolbarCtrl::SaveCurHeight()
{
	CRect rcWnd;
	GetWindowRect(&rcWnd);
	m_iPreviousHeight = rcWnd.Height();
}
