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
#include "SharedFilesWnd.h"
#include "OtherFunctions.h"
#include "SharedFileList.h"
#include "KnownFileList.h"
#include "KnownFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CSharedFilesWnd dialog

IMPLEMENT_DYNAMIC(CSharedFilesWnd, CDialog)
CSharedFilesWnd::CSharedFilesWnd(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CSharedFilesWnd::IDD, pParent)
{
	icon_files = NULL;
}

CSharedFilesWnd::~CSharedFilesWnd()
{
	if (icon_files)
		VERIFY( DestroyIcon(icon_files) );
}

void CSharedFilesWnd::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SFLIST, sharedfilesctrl);
	DDX_Control(pDX, IDC_POPBAR, pop_bar);
	DDX_Control(pDX, IDC_POPBAR2, pop_baraccept);
	DDX_Control(pDX, IDC_POPBAR3, pop_bartrans);
	DDX_Control(pDX, IDC_STATISTICS, m_ctrlStatisticsFrm);
}

BOOL CSharedFilesWnd::OnInitDialog()
{
	CResizableDialog::OnInitDialog();
	InitWindowStyles(this);
	SetAllIcons();
	sharedfilesctrl.Init();
	
	pop_bar.SetGradientColors(RGB(255,255,240),RGB(255,255,0));
	pop_bar.SetTextColor(RGB(20,70,255));
	pop_baraccept.SetGradientColors(RGB(255,255,240),RGB(255,255,0));
	pop_baraccept.SetTextColor(RGB(20,70,255));
	pop_bartrans.SetGradientColors(RGB(255,255,240),RGB(255,255,0));
	pop_bartrans.SetTextColor(RGB(20,70,255));

	LOGFONT lf;
	GetFont()->GetLogFont(&lf);
	lf.lfWeight = FW_BOLD;
	bold.CreateFontIndirect(&lf);
	m_ctrlStatisticsFrm.Init(_T("StatsDetail"));
    m_ctrlStatisticsFrm.SetFont(&bold); // should run Init() *first* before setting font bold. 
    m_ctrlStatisticsFrm.SetText(GetResString(IDS_SF_STATISTICS)); // i_a: XXX: moved here from 'Localize()' 
    
	Localize();

	GetDlgItem(IDC_CURSESSION_LBL)->SetFont(&bold);
	GetDlgItem(IDC_TOTAL_LBL)->SetFont(&bold);
	
	AddAnchor(IDC_TRAFFIC_TEXT, TOP_LEFT);
	AddAnchor(IDC_SFLIST,TOP_LEFT,BOTTOM_RIGHT);
	AddAnchor(IDC_RELOADSHAREDFILES, BOTTOM_RIGHT);
	AddAnchor(IDC_STATISTICS,BOTTOM_LEFT);
	AddAnchor(IDC_CURSESSION_LBL, BOTTOM_LEFT);
	AddAnchor(IDC_TOTAL_LBL, BOTTOM_LEFT);
	AddAnchor(IDC_FSTATIC4, BOTTOM_LEFT);
	AddAnchor(IDC_SREQUESTED,BOTTOM_LEFT);
	AddAnchor(IDC_POPBAR,BOTTOM_LEFT);
	AddAnchor(IDC_FSTATIC7,BOTTOM_LEFT);
	AddAnchor(IDC_SREQUESTED2,BOTTOM_LEFT);
	AddAnchor(IDC_FSTATIC5,BOTTOM_LEFT);
	AddAnchor(IDC_SACCEPTED,BOTTOM_LEFT);
	AddAnchor(IDC_POPBAR2,BOTTOM_LEFT);
	AddAnchor(IDC_FSTATIC8,BOTTOM_LEFT);
	AddAnchor(IDC_SACCEPTED2,BOTTOM_LEFT);
	AddAnchor(IDC_FSTATIC6,BOTTOM_LEFT);
	AddAnchor(IDC_STRANSFERED,BOTTOM_LEFT);
	AddAnchor(IDC_POPBAR3,BOTTOM_LEFT);
	AddAnchor(IDC_FSTATIC9,BOTTOM_LEFT);
	AddAnchor(IDC_STRANSFERED2,BOTTOM_LEFT);
	
	return TRUE;
}

void CSharedFilesWnd::Reload()
{
	theApp.sharedfiles->Reload();
	ShowSelectedFilesSummary();
}

BEGIN_MESSAGE_MAP(CSharedFilesWnd, CResizableDialog)
	ON_BN_CLICKED(IDC_RELOADSHAREDFILES, OnBnClickedReloadsharedfiles)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_SFLIST, OnLvnItemActivateSflist)
	ON_NOTIFY(NM_CLICK, IDC_SFLIST, OnNMClickSflist)
	ON_WM_SYSCOLORCHANGE()
END_MESSAGE_MAP()


// CSharedFilesWnd message handlers

void CSharedFilesWnd::OnBnClickedReloadsharedfiles()
{
	Reload();
}

void CSharedFilesWnd::OnLvnItemActivateSflist(NMHDR *pNMHDR, LRESULT *pResult)
{
	ShowSelectedFilesSummary();
}

void CSharedFilesWnd::ShowSelectedFilesSummary()
{
	const CKnownFile* pTheFile = NULL;
	int iFiles = 0;
	uint64 uTransfered = 0;
	UINT uRequests = 0;
	UINT uAccepted = 0;
	uint64 uAllTimeTransfered = 0;
	UINT uAllTimeRequests = 0;
	UINT uAllTimeAccepted = 0;
	POSITION pos = sharedfilesctrl.GetFirstSelectedItemPosition();
	while (pos)
	{
		int iItem = sharedfilesctrl.GetNextSelectedItem(pos);
		const CKnownFile* pFile = (CKnownFile*)sharedfilesctrl.GetItemData(iItem);
		iFiles++;
		if (iFiles == 1)
			pTheFile = pFile;

		uTransfered += pFile->statistic.GetTransferred();
		uRequests += pFile->statistic.GetRequests();
		uAccepted += pFile->statistic.GetAccepts();

		uAllTimeTransfered += pFile->statistic.GetAllTimeTransferred();
		uAllTimeRequests += pFile->statistic.GetAllTimeRequests();
		uAllTimeAccepted += pFile->statistic.GetAllTimeAccepts();
	}

	if (iFiles != 0)
	{
		pop_bartrans.SetRange32(0, theApp.knownfiles->transferred/1024);
		pop_bartrans.SetPos(uTransfered/1024);
		pop_bartrans.SetShowPercent();
		SetDlgItemText(IDC_STRANSFERED, CastItoXBytes(uTransfered, false, false));

		pop_bar.SetRange32(0, theApp.knownfiles->requested);
		pop_bar.SetPos(uRequests);
		pop_bar.SetShowPercent();
		SetDlgItemInt(IDC_SREQUESTED, uRequests, FALSE);

		pop_baraccept.SetRange32(0, theApp.knownfiles->accepted);
		pop_baraccept.SetPos(uAccepted);
		pop_baraccept.SetShowPercent();
		SetDlgItemInt(IDC_SACCEPTED, uAccepted, FALSE);

		SetDlgItemText(IDC_STRANSFERED2, CastItoXBytes(uAllTimeTransfered, false, false));
		SetDlgItemInt(IDC_SREQUESTED2, uAllTimeRequests, FALSE);
		SetDlgItemInt(IDC_SACCEPTED2, uAllTimeAccepted, FALSE);

		CString str(GetResString(IDS_SF_STATISTICS));
		if (iFiles == 1 && pTheFile != NULL)
			str += _T(" (") + MakeStringEscaped(pTheFile->GetFileName()) +_T(")");
		m_ctrlStatisticsFrm.SetText(str);
	}
	else
	{
		pop_bartrans.SetRange32(0, 100);
		pop_bartrans.SetPos(0);
		pop_bartrans.SetTextFormat(_T(""));
		SetDlgItemText(IDC_STRANSFERED, _T("-"));

		pop_bar.SetRange32(0, 100);
		pop_bar.SetPos(0);
		pop_bar.SetTextFormat(_T(""));
		SetDlgItemText(IDC_SREQUESTED, _T("-"));

		pop_baraccept.SetRange32(0, 100);
		pop_baraccept.SetPos(0);
		pop_baraccept.SetTextFormat(_T(""));
		SetDlgItemText(IDC_SACCEPTED, _T("-"));

		SetDlgItemText(IDC_STRANSFERED2, _T("-"));
		SetDlgItemText(IDC_SREQUESTED2, _T("-"));
		SetDlgItemText(IDC_SACCEPTED2, _T("-"));

		m_ctrlStatisticsFrm.SetText(GetResString(IDS_SF_STATISTICS));
	}
}

void CSharedFilesWnd::OnNMClickSflist(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnLvnItemActivateSflist(pNMHDR,pResult);
	*pResult = 0;
}

BOOL CSharedFilesWnd::PreTranslateMessage(MSG* pMsg) 
{
   if(pMsg->message == WM_KEYUP){
	   if (pMsg->hwnd == GetDlgItem(IDC_SFLIST)->m_hWnd)
			OnLvnItemActivateSflist(0,0);
   }
   if (pMsg->message == WM_MBUTTONUP){
		POINT point;
		::GetCursorPos(&point);
		CPoint p = point; 
		sharedfilesctrl.ScreenToClient(&p); 
		int it = sharedfilesctrl.HitTest(p); 
		if (it == -1)
			return FALSE;

		sharedfilesctrl.SetItemState(-1, 0, LVIS_SELECTED);
		sharedfilesctrl.SetItemState(it, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		sharedfilesctrl.SetSelectionMark(it);   // display selection mark correctly! 
		sharedfilesctrl.ShowComments((CKnownFile*)sharedfilesctrl.GetItemData(it));
		return TRUE;
   }

   return CResizableDialog::PreTranslateMessage(pMsg);
}

void CSharedFilesWnd::OnSysColorChange()
{
	CResizableDialog::OnSysColorChange();
	SetAllIcons();
	sharedfilesctrl.CreateMenues();
}

void CSharedFilesWnd::SetAllIcons()
{
	if (icon_files)
		VERIFY( DestroyIcon(icon_files) );
	icon_files = theApp.LoadIcon(_T("SharedFilesList"), 16, 16);
	((CStatic*)GetDlgItem(IDC_FILES_ICO))->SetIcon(icon_files);
}

void CSharedFilesWnd::Localize()
{
	sharedfilesctrl.Localize();
	GetDlgItem(IDC_TRAFFIC_TEXT)->SetWindowText(GetResString(IDS_SF_FILES));
	GetDlgItem(IDC_RELOADSHAREDFILES)->SetWindowText(GetResString(IDS_SF_RELOAD));

	m_ctrlStatisticsFrm.SetWindowText(GetResString(IDS_SF_STATISTICS));
	m_ctrlStatisticsFrm.SetText(GetResString(IDS_SF_STATISTICS));	
	
	GetDlgItem(IDC_CURSESSION_LBL)->SetWindowText(GetResString(IDS_SF_CURRENT));
	GetDlgItem(IDC_TOTAL_LBL)->SetWindowText(GetResString(IDS_SF_TOTAL));
	GetDlgItem(IDC_FSTATIC6)->SetWindowText(GetResString(IDS_SF_TRANS));
	GetDlgItem(IDC_FSTATIC5)->SetWindowText(GetResString(IDS_SF_ACCEPTED));
	GetDlgItem(IDC_FSTATIC4)->SetWindowText(GetResString(IDS_SF_REQUESTS)+_T(":"));
	GetDlgItem(IDC_FSTATIC9)->SetWindowText(GetResString(IDS_SF_TRANS));
	GetDlgItem(IDC_FSTATIC8)->SetWindowText(GetResString(IDS_SF_ACCEPTED));
	GetDlgItem(IDC_FSTATIC7)->SetWindowText(GetResString(IDS_SF_REQUESTS)+_T(":"));
}
