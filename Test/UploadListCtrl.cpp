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
#include "UploadListCtrl.h"
#include "TransferWnd.h"
#include "otherfunctions.h"
#include "MenuCmds.h"
#include "ClientDetailDialog.h"
#include "KademliaWnd.h"
#include "emuledlg.h"
#include "friendlist.h"
#include "MemDC.h"
#include "KnownFile.h"
#include "SharedFileList.h"
#include "UpDownClient.h"
#include "ClientCredits.h"
#include "ChatWnd.h"
#include "kademlia/kademlia/Kademlia.h"
#include "kademlia/net/KademliaUDPListener.h"
#include "UploadQueue.h"
//==>Modversion [cyrex2001]
#ifdef MODVERSION
#include "DownloadQueue.h"
#endif //Modversion
//<==Modversion [cyrex2001]

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// CUploadListCtrl

IMPLEMENT_DYNAMIC(CUploadListCtrl, CMuleListCtrl)
CUploadListCtrl::CUploadListCtrl(){
}

void CUploadListCtrl::Init()
{
	CImageList ilDummyImageList; //dummy list for getting the proper height of listview entries
	ilDummyImageList.Create(1, theApp.GetSmallSytemIconSize().cy,theApp.m_iDfltImageListColorFlags|ILC_MASK, 1, 1); 
	SetImageList(&ilDummyImageList, LVSIL_SMALL);
	ASSERT( (GetStyle() & LVS_SHAREIMAGELISTS) == 0 );
	ilDummyImageList.Detach();

	SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

	CToolTipCtrl* tooltip = GetToolTips();
	if (tooltip){
		tooltip->ModifyStyle(0, TTS_NOPREFIX);
		tooltip->SetDelayTime(TTDT_AUTOPOP, 20000);
		tooltip->SetDelayTime(TTDT_INITIAL, thePrefs.GetToolTipDelay()*1000);
	}

	InsertColumn(0,GetResString(IDS_QL_USERNAME),LVCFMT_LEFT,150,0);
	InsertColumn(1,GetResString(IDS_FILE),LVCFMT_LEFT,275,1);
	InsertColumn(2,GetResString(IDS_DL_SPEED),LVCFMT_LEFT,60,2);
	InsertColumn(3,GetResString(IDS_DL_TRANSF),LVCFMT_LEFT,65,3);
	InsertColumn(4,GetResString(IDS_WAITED),LVCFMT_LEFT,60,4);
	InsertColumn(5,GetResString(IDS_UPLOADTIME),LVCFMT_LEFT,60,5);
	InsertColumn(6,GetResString(IDS_STATUS),LVCFMT_LEFT,110,6);
	InsertColumn(7,GetResString(IDS_UPSTATUS),LVCFMT_LEFT,100,7);
//==>Modversion [cyrex2001]
#ifdef MODVERSION
	InsertColumn(8,GetResString(IDS_CLIENTSOFTWARE),LVCFMT_LEFT,100,8);
#endif //Modversion
//<==Modversion [cyrex2001]


	SetAllIcons();
	Localize();
	LoadSettings(CPreferences::tableUpload);

	// Barry - Use preferred sort order from preferences
	int sortItem = thePrefs.GetColumnSortItem(CPreferences::tableUpload);
	bool sortAscending = thePrefs.GetColumnSortAscending(CPreferences::tableUpload);
	SetSortArrow(sortItem, sortAscending);
	SortItems(SortProc, sortItem + (sortAscending ? 0:100));
}

CUploadListCtrl::~CUploadListCtrl(){
}

void CUploadListCtrl::OnSysColorChange()
{
	CMuleListCtrl::OnSysColorChange();
	SetAllIcons();
}

void CUploadListCtrl::SetAllIcons()
{
	imagelist.DeleteImageList();
	imagelist.Create(16,16,theApp.m_iDfltImageListColorFlags|ILC_MASK,0,1);
	imagelist.SetBkColor(CLR_NONE);
	imagelist.Add(CTempIconLoader(_T("ClientEDonkey")));//0
	imagelist.Add(CTempIconLoader(_T("ClientCompatible")));//1
	imagelist.Add(CTempIconLoader(_T("ClientEDonkeyPlus")));//2
	imagelist.Add(CTempIconLoader(_T("ClientCompatiblePlus")));//3
	imagelist.Add(CTempIconLoader(_T("Friend")));//4
	imagelist.Add(CTempIconLoader(_T("ClientMLDonkey")));//5
	imagelist.Add(CTempIconLoader(_T("ClientMLDonkeyPlus")));//6
	imagelist.Add(CTempIconLoader(_T("ClientEDonkeyHybrid")));//7
	imagelist.Add(CTempIconLoader(_T("ClientEDonkeyHybridPlus")));//8
	imagelist.Add(CTempIconLoader(_T("ClientShareaza")));//9
	imagelist.Add(CTempIconLoader(_T("ClientShareazaPlus")));//10
	imagelist.Add(CTempIconLoader(_T("ClientAMule")));//11
	imagelist.Add(CTempIconLoader(_T("ClientAMulePlus")));//12
	imagelist.Add(CTempIconLoader(_T("ClientLPhant")));//13
	imagelist.Add(CTempIconLoader(_T("ClientLPhantPlus")));//14
	imagelist.SetOverlayImage(imagelist.Add(CTempIconLoader(_T("ClientSecureOvl"))), 1);//15
//==>Modversion [cyrex2001]
#ifdef MODVERSION
	imagelist.Add(CTempIconLoader(_T("CLIENT_NEXTEMF")));//16
#endif //Modversion
//<==Modversion [cyrex2001]
}	

void CUploadListCtrl::Localize()
{
	CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();
	HDITEM hdi;
	hdi.mask = HDI_TEXT;
	CString strRes;

	strRes = GetResString(IDS_QL_USERNAME);
	hdi.pszText = strRes.GetBuffer();
	pHeaderCtrl->SetItem(0, &hdi);
	strRes.ReleaseBuffer();

	strRes = GetResString(IDS_FILE);
	hdi.pszText = strRes.GetBuffer();
	pHeaderCtrl->SetItem(1, &hdi);
	strRes.ReleaseBuffer();

	strRes = GetResString(IDS_DL_SPEED);
	hdi.pszText = strRes.GetBuffer();
	pHeaderCtrl->SetItem(2, &hdi);
	strRes.ReleaseBuffer();

	strRes = GetResString(IDS_DL_TRANSF);
	hdi.pszText = strRes.GetBuffer();
	pHeaderCtrl->SetItem(3, &hdi);
	strRes.ReleaseBuffer();

	strRes = GetResString(IDS_WAITED);
	hdi.pszText = strRes.GetBuffer();
	pHeaderCtrl->SetItem(4, &hdi);
	strRes.ReleaseBuffer();

	strRes = GetResString(IDS_UPLOADTIME);
	hdi.pszText = strRes.GetBuffer();
	pHeaderCtrl->SetItem(5, &hdi);
	strRes.ReleaseBuffer();

	strRes = GetResString(IDS_STATUS);
	hdi.pszText = strRes.GetBuffer();
	pHeaderCtrl->SetItem(6, &hdi);
	strRes.ReleaseBuffer();

	strRes = GetResString(IDS_UPSTATUS);
	hdi.pszText = strRes.GetBuffer();
	pHeaderCtrl->SetItem(7, &hdi);
	strRes.ReleaseBuffer();
//==>Modversion [cyrex2001]
#ifdef MODVERSION
	strRes = GetResString(IDS_CLIENTSOFTWARE);
	hdi.pszText = strRes.GetBuffer();
	pHeaderCtrl->SetItem(8, &hdi);
	strRes.ReleaseBuffer();
#endif //Modversion
//<==Modversion [cyrex2001]
}

void CUploadListCtrl::AddClient(const CUpDownClient* client)
{
	if (!theApp.emuledlg->IsRunning())
		return;

	int iItemCount = GetItemCount();
	int iItem = InsertItem(LVIF_TEXT|LVIF_PARAM,iItemCount,LPSTR_TEXTCALLBACK,0,0,0,(LPARAM)client);
	Update(iItem);
	theApp.emuledlg->transferwnd->UpdateListCount(1, iItemCount+1);
}

void CUploadListCtrl::RemoveClient(const CUpDownClient* client)
{
	if (!theApp.emuledlg->IsRunning())
		return;

	LVFINDINFO find;
	find.flags = LVFI_PARAM;
	find.lParam = (LPARAM)client;
	sint32 result = FindItem(&find);
	if (result != -1){
		DeleteItem(result);
		theApp.emuledlg->transferwnd->UpdateListCount(1);
	}
}

void CUploadListCtrl::RefreshClient(const CUpDownClient* client)
{
	// There is some type of timing issue here.. If you click on item in the queue or upload and leave
	// the focus on it when you exit the cient, it breaks on line 854 of emuleDlg.cpp
	// I added this IsRunning() check to this function and the DrawItem method and
	// this seems to keep it from crashing. This is not the fix but a patch until
	// someone points out what is going wrong.. Also, it will still assert in debug mode..
	if (!theApp.emuledlg->IsRunning())
		return;

	LVFINDINFO find;
	find.flags = LVFI_PARAM;
	find.lParam = (LPARAM)client;
	sint16 result = FindItem(&find);
	if (result != -1)
		Update(result);
}

#define DLC_DT_TEXT (DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX|DT_END_ELLIPSIS)

void CUploadListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if( !theApp.emuledlg->IsRunning() )
		return;
	if (!lpDrawItemStruct->itemData)
		return;
	CDC* odc = CDC::FromHandle(lpDrawItemStruct->hDC);
	BOOL bCtrlFocused = ((GetFocus() == this ) || (GetStyle() & LVS_SHOWSELALWAYS));
	if( (lpDrawItemStruct->itemAction | ODA_SELECT) && (lpDrawItemStruct->itemState & ODS_SELECTED )){
		if(bCtrlFocused)
			odc->SetBkColor(m_crHighlight);
		else
			odc->SetBkColor(m_crNoHighlight);
	}
	else
		odc->SetBkColor(GetBkColor());
	const CUpDownClient* client = (CUpDownClient*)lpDrawItemStruct->itemData;
	CMemDC dc(CDC::FromHandle(lpDrawItemStruct->hDC), &lpDrawItemStruct->rcItem);
	CFont* pOldFont = dc.SelectObject(GetFont());
	RECT cur_rec = lpDrawItemStruct->rcItem;
	COLORREF crOldTextColor = dc.SetTextColor(m_crWindowText);
    if(client->GetSlotNumber() > theApp.uploadqueue->GetActiveUploadsCount()) {
        dc.SetTextColor(::GetSysColor(COLOR_GRAYTEXT));
    }

	int iOldBkMode;
	if (m_crWindowTextBk == CLR_NONE){
		DefWindowProc(WM_ERASEBKGND, (WPARAM)(HDC)dc, 0);
		iOldBkMode = dc.SetBkMode(TRANSPARENT);
	}
	else
		iOldBkMode = OPAQUE;

	CString Sbuffer;
	CKnownFile* file = theApp.sharedfiles->GetFileByID(client->GetUploadFileID());
	CHeaderCtrl *pHeaderCtrl = GetHeaderCtrl();
	int iCount = pHeaderCtrl->GetItemCount();
	cur_rec.right = cur_rec.left - 8;
	cur_rec.left += 4;

	for(int iCurrent = 0; iCurrent < iCount; iCurrent++){
		int iColumn = pHeaderCtrl->OrderToIndex(iCurrent);
		if( !IsColumnHidden(iColumn) ){
			cur_rec.right += GetColumnWidth(iColumn);
			switch(iColumn){
				case 0:{
					uint8 image;
					if (client->IsFriend())
						image = 4;
					else if (client->GetClientSoft() == SO_EDONKEYHYBRID){
						if (client->credits->GetScoreRatio(client->GetIP()) > 1)
							image = 8;
						else
							image = 7;
					}
					else if (client->GetClientSoft() == SO_MLDONKEY){
						if (client->credits->GetScoreRatio(client->GetIP()) > 1)
							image = 6;
						else
							image = 5;
					}
					else if (client->GetClientSoft() == SO_SHAREAZA){
						if(client->credits->GetScoreRatio(client->GetIP()) > 1)
							image = 10;
						else
							image = 9;
					}
					else if (client->GetClientSoft() == SO_AMULE){
						if(client->credits->GetScoreRatio(client->GetIP()) > 1)
							image = 12;
						else
							image = 11;
					}
					else if (client->GetClientSoft() == SO_LPHANT){
						if(client->credits->GetScoreRatio(client->GetIP()) > 1)
							image = 14;
						else
							image = 13;
					}
					else if (client->ExtProtocolAvailable()){
						if(client->credits->GetScoreRatio(client->GetIP()) > 1)
//==>Modversion [cyrex2001][shadow2004]
#ifdef MODVERSION
						image = (client->IsNextEMF())?16:3;
#else //Modversion
							image = 3;
#endif //Modversion
//<==Modversion [cyrex2001][shadow2004]
						 // image = (client->credits->GetScoreRatio(client->GetIP()) > 1)?3:1 
						else
//==>Modversion [cyrex2001][shadow2004]
#ifdef MODVERSION
							image = (client->IsNextEMF())?16:1;
#else //Modversion
							image = 1;
#endif //Modversion
//<==Modversion [cyrex2001][shadow2004]

					}
					else{
						if (client->credits->GetScoreRatio(client->GetIP()) > 1)
							image = 2;
						else
							image = 0;
					}

					POINT point = {cur_rec.left, cur_rec.top+1};
					imagelist.Draw(dc,image, point, ILD_NORMAL | ((client->Credits() && client->Credits()->GetCurrentIdentState(client->GetIP()) == IS_IDENTIFIED) ? INDEXTOOVERLAYMASK(1) : 0));
					Sbuffer = client->GetUserName();
					cur_rec.left +=20;
					dc->DrawText(Sbuffer,Sbuffer.GetLength(),&cur_rec,DLC_DT_TEXT);
					cur_rec.left -=20;
					break;
				}
				case 1:
					if(file)
						Sbuffer = file->GetFileName();
					else
						Sbuffer = _T("?");
					break;
				case 2:
					Sbuffer.Format(_T("%s"), CastItoXBytes(client->GetDatarate(), false, true));
					break;
				case 3:
					Sbuffer.Format(_T("%s"), CastItoXBytes(client->GetQueueSessionPayloadUp(), false, false));
					break;
				case 4:
					if (client->HasLowID())
						Sbuffer.Format(_T("%s (%s)"),CastSecondsToHM((client->GetWaitTime())/1000),GetResString(IDS_IDLOW) );
					else
						Sbuffer = CastSecondsToHM((client->GetWaitTime())/1000);
					break;
				case 5:
					Sbuffer = CastSecondsToHM((client->GetUpStartTimeDelay())/1000);
					break;
				case 6:
					Sbuffer = client->GetUploadStateDisplayString();
					break;
				case 7:
					//if( client->GetUpPartCount() ){
						cur_rec.bottom--;
						cur_rec.top++;
//==> removed 3D-Bar-display [shadow2004]
#ifdef BAR3D
						client->DrawUpStatusBar(dc,&cur_rec,false,thePrefs.UseFlatBar());
#else
						client->DrawUpStatusBar(dc,&cur_rec,false);
#endif
//<== removed 3D-Bar-display [shadow2004]
						cur_rec.bottom++;
						cur_rec.top--;
					//}
					break;
//==>Modversion [cyrex2001]
#ifdef MODVERSION
				case 8:			
					Sbuffer = client->GetClientSoftVer();
					break;

#endif //Modversion
//<==Modversion [cyrex2001]
			}
			if( iColumn != 7 && iColumn != 0 )
				dc->DrawText(Sbuffer,Sbuffer.GetLength(),&cur_rec,DLC_DT_TEXT);
			cur_rec.left += GetColumnWidth(iColumn);
		}
	}
	//draw rectangle around selected item(s)
	if ((lpDrawItemStruct->itemAction | ODA_SELECT) && (lpDrawItemStruct->itemState & ODS_SELECTED))
	{
		RECT outline_rec = lpDrawItemStruct->rcItem;

		outline_rec.top--;
		outline_rec.bottom++;
		dc->FrameRect(&outline_rec, &CBrush(GetBkColor()));
		outline_rec.top++;
		outline_rec.bottom--;
		outline_rec.left++;
		outline_rec.right--;

		if(bCtrlFocused)
			dc->FrameRect(&outline_rec, &CBrush(m_crFocusLine));
		else
			dc->FrameRect(&outline_rec, &CBrush(m_crNoFocusLine));
	}

	if (m_crWindowTextBk == CLR_NONE)
		dc.SetBkMode(iOldBkMode);
	dc.SelectObject(pOldFont);
	dc.SetTextColor(crOldTextColor);
}

BEGIN_MESSAGE_MAP(CUploadListCtrl, CMuleListCtrl)
	ON_WM_CONTEXTMENU()
	ON_WM_SYSCOLORCHANGE()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDblclk)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDispInfo)
	ON_NOTIFY_REFLECT(LVN_GETINFOTIP, OnLvnGetInfoTip)
END_MESSAGE_MAP()

// CUploadListCtrl message handlers
void CUploadListCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
	int iSel = GetNextItem(-1, LVIS_SELECTED | LVIS_FOCUSED);
	const CUpDownClient* client = (iSel != -1) ? (CUpDownClient*)GetItemData(iSel) : NULL;

	CTitleMenu ClientMenu;
	ClientMenu.CreatePopupMenu();
	ClientMenu.AddMenuTitle(GetResString(IDS_CLIENTS), true);
	ClientMenu.AppendMenu(MF_STRING | (client ? MF_ENABLED : MF_GRAYED), MP_DETAIL, GetResString(IDS_SHOWDETAILS), _T("CLIENTDETAILS"));
	ClientMenu.SetDefaultItem(MP_DETAIL);
	ClientMenu.AppendMenu(MF_STRING | ((client && client->IsEd2kClient() && !client->IsFriend()) ? MF_ENABLED : MF_GRAYED), MP_ADDFRIEND, GetResString(IDS_ADDFRIEND), _T("ADDFRIEND"));
	ClientMenu.AppendMenu(MF_STRING | ((client && client->IsEd2kClient()) ? MF_ENABLED : MF_GRAYED), MP_MESSAGE, GetResString(IDS_SEND_MSG), _T("SENDMESSAGE"));
	ClientMenu.AppendMenu(MF_STRING | ((client && client->IsEd2kClient() && client->GetViewSharedFilesSupport()) ? MF_ENABLED : MF_GRAYED), MP_SHOWLIST, GetResString(IDS_VIEWFILES), _T("VIEWFILES"));
	if (Kademlia::CKademlia::isRunning() && !Kademlia::CKademlia::isConnected())
		ClientMenu.AppendMenu(MF_STRING | ((client && client->IsEd2kClient() && client->GetKadPort()!=0) ? MF_ENABLED : MF_GRAYED), MP_BOOT, GetResString(IDS_BOOTSTRAP));
	GetPopupMenuPos(*this, point);
	ClientMenu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);
}

BOOL CUploadListCtrl::OnCommand(WPARAM wParam,LPARAM lParam ){
	int iSel = GetNextItem(-1, LVIS_SELECTED | LVIS_FOCUSED);
	if (iSel != -1){
		CUpDownClient* client = (CUpDownClient*)GetItemData(iSel);
		switch (wParam){
			case MP_SHOWLIST:
				client->RequestSharedFileList();
				break;
			case MP_MESSAGE:
				theApp.emuledlg->chatwnd->StartSession(client);
				break;
			case MP_ADDFRIEND:
				if (theApp.friendlist->AddFriend(client))
					Update(iSel);
				break;
			case MPG_ALTENTER:
			case MP_DETAIL:{
				CClientDetailDialog dialog(client);
				dialog.DoModal();
				break;
			}
			case MP_BOOT:
				if (client->GetKadPort())
					Kademlia::CKademlia::bootstrap(ntohl(client->GetIP()), client->GetKadPort());
				break;
		}
	}
	return true;
}


void CUploadListCtrl::OnColumnClick( NMHDR* pNMHDR, LRESULT* pResult){

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// if it's a second click on the same column then reverse the sort order,
	// otherwise sort the new column in ascending order.

	// Barry - Store sort order in preferences
	// Determine ascending based on whether already sorted on this column
	int sortItem = thePrefs.GetColumnSortItem(CPreferences::tableUpload);
	bool m_oldSortAscending = thePrefs.GetColumnSortAscending(CPreferences::tableUpload);
	bool sortAscending = (sortItem != pNMListView->iSubItem) ? true : !m_oldSortAscending;
	// Item is column clicked
	sortItem = pNMListView->iSubItem;
	// Save new preferences
	thePrefs.SetColumnSortItem(CPreferences::tableUpload, sortItem);
	thePrefs.SetColumnSortAscending(CPreferences::tableUpload, sortAscending);
	// Sort table
	SetSortArrow(sortItem, sortAscending);
	SortItems(SortProc, sortItem + (sortAscending ? 0:100));

	*pResult = 0;
}

int CUploadListCtrl::SortProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	const CUpDownClient* item1 = (CUpDownClient*)lParam1;
	const CUpDownClient* item2 = (CUpDownClient*)lParam2;
	switch(lParamSort){
		case 0: 
			if(item1->GetUserName() && item2->GetUserName())
				return CompareLocaleStringNoCase(item1->GetUserName(), item2->GetUserName());
			else if(item1->GetUserName())
				return 1;
			else
				return -1;
		case 100:
			if(item1->GetUserName() && item2->GetUserName())
				return CompareLocaleStringNoCase(item2->GetUserName(), item1->GetUserName());
			else if(item2->GetUserName())
				return 1;
			else
				return -1;
		case 1: {
			CKnownFile* file1 = theApp.sharedfiles->GetFileByID(item1->GetUploadFileID());
			CKnownFile* file2 = theApp.sharedfiles->GetFileByID(item2->GetUploadFileID());
			if( (file1 != NULL) && (file2 != NULL))
				return CompareLocaleStringNoCase(file1->GetFileName(), file2->GetFileName());
			else if( file1 == NULL )
				return 1;
			else
				return -1;
		}
		case 101:{
			CKnownFile* file1 = theApp.sharedfiles->GetFileByID(item1->GetUploadFileID());
			CKnownFile* file2 = theApp.sharedfiles->GetFileByID(item2->GetUploadFileID());
			if( (file1 != NULL) && (file2 != NULL))
				return CompareLocaleStringNoCase(file2->GetFileName(), file1->GetFileName());
			else if( file1 == NULL )
				return 1;
			else
				return -1;
		}
		case 2: 
			return CompareUnsigned(item1->GetDatarate(), item2->GetDatarate());
		case 102:
			return CompareUnsigned(item2->GetDatarate(), item1->GetDatarate());

		case 3: 
			return CompareUnsigned(item1->GetSessionUp(), item2->GetSessionUp());
		case 103: 
			return CompareUnsigned(item2->GetSessionUp(), item1->GetSessionUp());

		case 4: 
			return item1->GetWaitTime() - item2->GetWaitTime();
		case 104: 
			return item2->GetWaitTime() - item1->GetWaitTime();
		case 5: 
			return item1->GetUpStartTimeDelay() - item2->GetUpStartTimeDelay();
		case 105: 
			return item2->GetUpStartTimeDelay() - item1->GetUpStartTimeDelay();
		case 6: 
			return item1->GetUploadState() - item2->GetUploadState();
		case 106: 
			return item2->GetUploadState() - item1->GetUploadState();
		case 7:
			return item1->GetUpPartCount() - item2->GetUpPartCount();
		case 107: 
			return item2->GetUpPartCount() - item1->GetUpPartCount();
//==>Modversion [cyrex2001]
#ifdef MODVERSION
		case 8:
			return item2->GetClientSoftVer().CompareNoCase(item1->GetClientSoftVer());
		case 108:
			return item1->GetClientSoftVer().CompareNoCase(item2->GetClientSoftVer());
#endif //Modversion
//<==Modversion [cyrex2001]
		default:
			return 0;
	}
}

void CUploadListCtrl::ShowSelectedUserDetails()
{
	POINT point;
	::GetCursorPos(&point);
	CPoint p = point; 
    ScreenToClient(&p); 
    int it = HitTest(p); 
    if (it == -1)
		return;

	SetItemState(-1, 0, LVIS_SELECTED);
	SetItemState(it, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	SetSelectionMark(it);   // display selection mark correctly!

	const CUpDownClient* client = (CUpDownClient*)GetItemData(GetSelectionMark());
	if (client){
		CClientDetailDialog dialog(client);
		dialog.DoModal();
	}
}

void CUploadListCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iSel = GetNextItem(-1, LVIS_SELECTED | LVIS_FOCUSED);
	if (iSel != -1){
		const CUpDownClient* client = (CUpDownClient*)GetItemData(iSel);
		if (client){
			CClientDetailDialog dialog(client);
			dialog.DoModal();
		}
	}
	*pResult = 0;
}

void CUploadListCtrl::OnGetDispInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	if (theApp.emuledlg->IsRunning()){
		// Although we have an owner drawn listview control we store the text for the primary item in the listview, to be
		// capable of quick searching those items via the keyboard. Because our listview items may change their contents,
		// we do this via a text callback function. The listview control will send us the LVN_DISPINFO notification if
		// it needs to know the contents of the primary item.
		//
		// But, the listview control sends this notification all the time, even if we do not search for an item. At least
		// this notification is only sent for the visible items and not for all items in the list. Though, because this
		// function is invoked *very* often, no *NOT* put any time consuming code here in.

		if (pDispInfo->item.mask & LVIF_TEXT){
			const CUpDownClient* pClient = reinterpret_cast<CUpDownClient*>(pDispInfo->item.lParam);
			if (pClient != NULL){
				switch (pDispInfo->item.iSubItem){
					case 0:
						if (pClient->GetUserName() && pDispInfo->item.cchTextMax > 0){
							_tcsncpy(pDispInfo->item.pszText, pClient->GetUserName(), pDispInfo->item.cchTextMax);
							pDispInfo->item.pszText[pDispInfo->item.cchTextMax-1] = _T('\0');
						}
						break;
					default:
						// shouldn't happen
						pDispInfo->item.pszText[0] = _T('\0');
						break;
				}
			}
		}
	}
	*pResult = 0;
}

void CUploadListCtrl::OnLvnGetInfoTip(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVGETINFOTIP pGetInfoTip = reinterpret_cast<LPNMLVGETINFOTIP>(pNMHDR);

	if (pGetInfoTip->iSubItem == 0)
	{
		LVHITTESTINFO hti = {0};
		::GetCursorPos(&hti.pt);
		ScreenToClient(&hti.pt);
		if (SubItemHitTest(&hti) == -1 || hti.iItem != pGetInfoTip->iItem || hti.iSubItem != 0){
			// don' show the default label tip for the main item, if the mouse is not over the main item
			if ((pGetInfoTip->dwFlags & LVGIT_UNFOLDED) == 0 && pGetInfoTip->cchTextMax > 0 && pGetInfoTip->pszText[0] != '\0')
				pGetInfoTip->pszText[0] = '\0';
			return;
		}

		const CUpDownClient* client = (CUpDownClient*)GetItemData(pGetInfoTip->iItem);
		if (client && pGetInfoTip->pszText && pGetInfoTip->cchTextMax > 0)
		{
			CString info;

			CKnownFile* file = theApp.sharedfiles->GetFileByID(client->GetUploadFileID());
			// build info text and display it
			info.Format(GetResString(IDS_USERINFO), client->GetUserName());
			if (file)
			{
				info += GetResString(IDS_SF_REQUESTED) + _T(" ") + CString(file->GetFileName()) + _T("\n");
				CString stat;
				stat.Format(GetResString(IDS_FILESTATS_SESSION)+GetResString(IDS_FILESTATS_TOTAL),
							file->statistic.GetAccepts(), file->statistic.GetRequests(), CastItoXBytes(file->statistic.GetTransferred(), false, false),
							file->statistic.GetAllTimeAccepts(), file->statistic.GetAllTimeRequests(), CastItoXBytes(file->statistic.GetAllTimeTransferred(), false, false) );
				info += stat;
			}
			else
			{
				info += GetResString(IDS_REQ_UNKNOWNFILE);
			}

			_tcsncpy(pGetInfoTip->pszText, info, pGetInfoTip->cchTextMax);
			pGetInfoTip->pszText[pGetInfoTip->cchTextMax-1] = _T('\0');
		}
	}
	*pResult = 0;
}
