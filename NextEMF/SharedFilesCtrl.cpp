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
#include "emuledlg.h"
#include "SharedFilesCtrl.h"
#include "OtherFunctions.h"
#include "ResizableLib/ResizableSheet.h"
#include "KnownFile.h"
#include "MapKey.h"
#include "SharedFileList.h"
#include "MemDC.h"
#include "PartFile.h"
#include "MenuCmds.h"
#include "SharedFilesWnd.h"
#include "Opcodes.h"
#include "InputBox.h"
#include "TransferWnd.h"
#include "ClientList.h"
#include "UpDownClient.h"
#include "Collection.h"
#include "CollectionCreateDialog.h"
#include "CollectionViewDialog.h"
#include "FileDetailDialog.h"
#include "SharedDirsTreeCtrl.h"
#include "SearchParams.h"
#include "SearchDlg.h"
#include "SearchResultsWnd.h"

//////////////////////////////////////////////////////////////////////////////
// CSharedFilesCtrl

IMPLEMENT_DYNAMIC(CSharedFilesCtrl, CMuleListCtrl)

BEGIN_MESSAGE_MAP(CSharedFilesCtrl, CMuleListCtrl)
	ON_WM_SYSCOLORCHANGE()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDblclk)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDispInfo)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

CSharedFilesCtrl::CSharedFilesCtrl()
	: CListCtrlItemWalk(this)
{
//==> Optimizer [shadow2004]
#ifdef OPTIM
	memzero(&sortstat, sizeof(sortstat));
#else
	memset(&sortstat, 0, sizeof(sortstat));
#endif
//<== Optimizer [shadow2004]
	nAICHHashing = 0;
	m_pDirectoryFilter = NULL;
}

CSharedFilesCtrl::~CSharedFilesCtrl()
{
}

void CSharedFilesCtrl::Init()
{
	SetName(_T("SharedFilesCtrl"));
	CImageList ilDummyImageList; //dummy list for getting the proper height of listview entries
	ilDummyImageList.Create(1, theApp.GetSmallSytemIconSize().cy,theApp.m_iDfltImageListColorFlags|ILC_MASK, 1, 1); 
	SetImageList(&ilDummyImageList, LVSIL_SMALL);
	ASSERT( (GetStyle() & LVS_SHAREIMAGELISTS) == 0 );
	ilDummyImageList.Detach();

	SetExtendedStyle(LVS_EX_FULLROWSELECT);
	ModifyStyle(LVS_SINGLESEL,0);

	InsertColumn(0, GetResString(IDS_DL_FILENAME) ,LVCFMT_LEFT,250,0);
	InsertColumn(1,GetResString(IDS_DL_SIZE),LVCFMT_LEFT,100,1);
	InsertColumn(2,GetResString(IDS_TYPE),LVCFMT_LEFT,50,2);
	InsertColumn(3,GetResString(IDS_PRIORITY),LVCFMT_LEFT,70,3);
	InsertColumn(4,GetResString(IDS_FILEID),LVCFMT_LEFT,220,4);
	InsertColumn(5,GetResString(IDS_SF_REQUESTS),LVCFMT_LEFT,100,5);
	InsertColumn(6,GetResString(IDS_SF_ACCEPTS),LVCFMT_LEFT,100,6);
	InsertColumn(7,GetResString(IDS_SF_TRANSFERRED),LVCFMT_LEFT,120,7);
	InsertColumn(8,GetResString(IDS_UPSTATUS),LVCFMT_LEFT,100,8);
	InsertColumn(9,GetResString(IDS_FOLDER),LVCFMT_LEFT,200,9);
	InsertColumn(10,GetResString(IDS_COMPLSOURCES),LVCFMT_LEFT,100,10);
	InsertColumn(11,GetResString(IDS_SHAREDTITLE),LVCFMT_LEFT,200,11);

	SetAllIcons();
	CreateMenues();
	LoadSettings();

	// Barry - Use preferred sort order from preferences
	SetSortArrow();
	SortItems(SortProc, GetSortItem() + (GetSortAscending() ? 0:20));
}

void CSharedFilesCtrl::OnSysColorChange()
{
	CMuleListCtrl::OnSysColorChange();
	SetAllIcons();
	CreateMenues();
}

void CSharedFilesCtrl::SetAllIcons()
{
	m_ImageList.DeleteImageList();
	m_ImageList.Create(16,16,theApp.m_iDfltImageListColorFlags|ILC_MASK,0,1);
	m_ImageList.SetBkColor(CLR_NONE);
	m_ImageList.Add(CTempIconLoader(_T("EMPTY")));
	m_ImageList.Add(CTempIconLoader(_T("FileSharedServer"), 16, 16));
	m_ImageList.Add(CTempIconLoader(_T("FileSharedKad"), 16, 16));
	m_ImageList.Add(CTempIconLoader(_T("Rating_NotRated")));
	m_ImageList.Add(CTempIconLoader(_T("Rating_Fake")));
	m_ImageList.Add(CTempIconLoader(_T("Rating_Poor")));
	m_ImageList.Add(CTempIconLoader(_T("Rating_Fair")));
	m_ImageList.Add(CTempIconLoader(_T("Rating_Good")));
	m_ImageList.Add(CTempIconLoader(_T("Rating_Excellent")));
	m_ImageList.SetOverlayImage(m_ImageList.Add(CTempIconLoader(_T("FileCommentsOvl"))), 1);
}

void CSharedFilesCtrl::Localize()
{
	CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();
	HDITEM hdi;
	hdi.mask = HDI_TEXT;
	CString strRes;

	strRes = GetResString(IDS_DL_FILENAME);
	hdi.pszText = strRes.GetBuffer();
	pHeaderCtrl->SetItem(0, &hdi);
	strRes.ReleaseBuffer();

	strRes = GetResString(IDS_DL_SIZE);
	hdi.pszText = strRes.GetBuffer();
	pHeaderCtrl->SetItem(1, &hdi);
	strRes.ReleaseBuffer();

	strRes = GetResString(IDS_TYPE);
	hdi.pszText = strRes.GetBuffer();
	pHeaderCtrl->SetItem(2, &hdi);
	strRes.ReleaseBuffer();

	strRes = GetResString(IDS_PRIORITY);
	hdi.pszText = strRes.GetBuffer();
	pHeaderCtrl->SetItem(3, &hdi);
	strRes.ReleaseBuffer();

	strRes = GetResString(IDS_FILEID);
	hdi.pszText = strRes.GetBuffer();
	pHeaderCtrl->SetItem(4, &hdi);
	strRes.ReleaseBuffer();

	strRes = GetResString(IDS_SF_REQUESTS);
	hdi.pszText = strRes.GetBuffer();
	pHeaderCtrl->SetItem(5, &hdi);
	strRes.ReleaseBuffer();

	strRes = GetResString(IDS_SF_ACCEPTS);
	hdi.pszText = strRes.GetBuffer();
	pHeaderCtrl->SetItem(6, &hdi);
	strRes.ReleaseBuffer();

	strRes = GetResString(IDS_SF_TRANSFERRED);
	hdi.pszText = strRes.GetBuffer();
	pHeaderCtrl->SetItem(7, &hdi);
	strRes.ReleaseBuffer();

	strRes = GetResString(IDS_SHARED_STATUS);
	hdi.pszText = strRes.GetBuffer();
	pHeaderCtrl->SetItem(8, &hdi);
	strRes.ReleaseBuffer();

	strRes = GetResString(IDS_FOLDER);
	hdi.pszText = strRes.GetBuffer();
	pHeaderCtrl->SetItem(9, &hdi);
	strRes.ReleaseBuffer();

	strRes = GetResString(IDS_COMPLSOURCES);
	hdi.pszText = strRes.GetBuffer();
	pHeaderCtrl->SetItem(10, &hdi);
	strRes.ReleaseBuffer();

	strRes = GetResString(IDS_SHAREDTITLE);
	hdi.pszText = strRes.GetBuffer();
	pHeaderCtrl->SetItem(11, &hdi);
	strRes.ReleaseBuffer();

	CreateMenues();

	int iItems = GetItemCount();
	for (int i = 0; i < iItems; i++)
		Update(i);
}

void CSharedFilesCtrl::AddFile(const CKnownFile* file)
{
	if (!theApp.emuledlg->IsRunning())
		return;
	// check filter conditions if we should show this file right now
	if (m_pDirectoryFilter != NULL){
		CString strFilePath = file->GetPath();
		if (strFilePath.Right(1) == "\\"){
			strFilePath = strFilePath.Left(strFilePath.GetLength()-1);
		}
		switch(m_pDirectoryFilter->m_eItemType){
			case SDI_ALL:
				// No filter
				break;
			case SDI_FILESYSTEMPARENT:
				return; // no files
				break;
			case SDI_NO:
				// some shared directory
			case SDI_CATINCOMING:
				// Categories with special incoming dirs
			case SDI_UNSHAREDDIRECTORY:
				// Items from the whole filesystem tree
				if (strFilePath.CompareNoCase(m_pDirectoryFilter->m_strFullPath) != 0)
					return;
				break;
			case SDI_TEMP:
				// only tempfiles
				if (!file->IsPartFile())
					return;
				else if (m_pDirectoryFilter->m_nCatFilter != -1 && m_pDirectoryFilter->m_nCatFilter != ((CPartFile*)file)->GetCategory())
					return;
				break;
			case SDI_DIRECTORY:
				// any userselected shared dir but not incoming or temp
				if (file->IsPartFile())
					return;
				if (strFilePath.CompareNoCase(thePrefs.GetIncomingDir()) == 0)
					return;
				break;
			case SDI_INCOMING:
				// Main incoming directory
				if (strFilePath.CompareNoCase(thePrefs.GetIncomingDir()) != 0)
					return;
				// Hmm should we show all incoming files dirs or only those from the main incoming dir here?
				// hard choice, will only show the main for now
				break;

		}
	}
	if (FindFile(file) != -1)
		return;
	int iItem = InsertItem(LVIF_TEXT|LVIF_PARAM, GetItemCount(), LPSTR_TEXTCALLBACK, 0, 0, 0, (LPARAM)file);
	if (iItem >= 0)
		Update(iItem);
}

void CSharedFilesCtrl::RemoveFile(const CKnownFile* file)
{
	int iItem = FindFile(file);
	if (iItem != -1)
	{
		DeleteItem(iItem);
		ShowFilesCount();
	}
}

void CSharedFilesCtrl::UpdateFile(const CKnownFile* file)
{
	if (!file || !theApp.emuledlg->IsRunning())
		return;
	int iItem = FindFile(file);
	if (iItem != -1)
	{
		Update(iItem);
		if (GetItemState(iItem, LVIS_SELECTED))
			theApp.emuledlg->sharedfileswnd->ShowSelectedFilesSummary();
	}
}

int CSharedFilesCtrl::FindFile(const CKnownFile* pFile)
{
	LVFINDINFO find;
	find.flags = LVFI_PARAM;
	find.lParam = (LPARAM)pFile;
	return FindItem(&find);
}

void CSharedFilesCtrl::ReloadFileList()
{
	DeleteAllItems();
	theApp.emuledlg->sharedfileswnd->ShowSelectedFilesSummary();
	
	CCKey bufKey;
	CKnownFile* cur_file;
	for (POSITION pos = theApp.sharedfiles->m_Files_map.GetStartPosition(); pos != 0; ){
		theApp.sharedfiles->m_Files_map.GetNextAssoc(pos, bufKey, cur_file);
		AddFile(cur_file);
	}
	ShowFilesCount();
}

void CSharedFilesCtrl::ShowFilesCount()
{
	CString str;
	if (theApp.sharedfiles->GetHashingCount() + nAICHHashing)
		str.Format(_T(" (%i, %s %i)"), theApp.sharedfiles->GetCount(), GetResString(IDS_HASHING), theApp.sharedfiles->GetHashingCount() + nAICHHashing);
	else
		str.Format(_T(" (%i)"), theApp.sharedfiles->GetCount());
	theApp.emuledlg->sharedfileswnd->GetDlgItem(IDC_TRAFFIC_TEXT)->SetWindowText(GetResString(IDS_SF_FILES) + str);
}

#define DLC_DT_TEXT (DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX|DT_END_ELLIPSIS)

void CSharedFilesCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if( !theApp.emuledlg->IsRunning() )
		return;
	if( !lpDrawItemStruct->itemData)
		return;
	CDC* odc = CDC::FromHandle(lpDrawItemStruct->hDC);
	BOOL bCtrlFocused = ((GetFocus() == this ) || (GetStyle() & LVS_SHOWSELALWAYS));
	if (lpDrawItemStruct->itemState & ODS_SELECTED) {
		if(bCtrlFocused)
			odc->SetBkColor(m_crHighlight);
		else
			odc->SetBkColor(m_crNoHighlight);
	}
	else
		odc->SetBkColor(GetBkColor());
	/*const*/ CKnownFile* file = (CKnownFile*)lpDrawItemStruct->itemData;
	CMemDC dc(odc, &lpDrawItemStruct->rcItem);
	CFont* pOldFont = dc.SelectObject(GetFont());
	CRect cur_rec(lpDrawItemStruct->rcItem);
	COLORREF crOldTextColor = dc.SetTextColor((lpDrawItemStruct->itemState & ODS_SELECTED) ? m_crHighlightText : m_crWindowText);

	int iOldBkMode;
	if (m_crWindowTextBk == CLR_NONE){
		DefWindowProc(WM_ERASEBKGND, (WPARAM)(HDC)dc, 0);
		iOldBkMode = dc.SetBkMode(TRANSPARENT);
	}
	else
		iOldBkMode = OPAQUE;

	CHeaderCtrl *pHeaderCtrl = GetHeaderCtrl();
	int iCount = pHeaderCtrl->GetItemCount();
	const int iMarginX = 4;
	cur_rec.right = cur_rec.left - iMarginX*2;
	cur_rec.left += iMarginX;
	CString buffer;
	int iIconDrawWidth = theApp.GetSmallSytemIconSize().cx + 3;
	for(int iCurrent = 0; iCurrent < iCount; iCurrent++){
		int iColumn = pHeaderCtrl->OrderToIndex(iCurrent);
		if( !IsColumnHidden(iColumn) ){
			UINT uDTFlags = DLC_DT_TEXT;
			cur_rec.right += GetColumnWidth(iColumn);
			switch(iColumn){
				case 0:{
					int iImage = theApp.GetFileTypeSystemImageIdx(file->GetFileName());
					if (theApp.GetSystemImageList() != NULL)
						::ImageList_Draw(theApp.GetSystemImageList(), iImage, dc.GetSafeHdc(), cur_rec.left, cur_rec.top, ILD_NORMAL|ILD_TRANSPARENT);
					if (!file->GetFileComment().IsEmpty() || file->GetFileRating())
						m_ImageList.Draw(dc, 0, CPoint(cur_rec.left, cur_rec.top), ILD_NORMAL | ILD_TRANSPARENT | INDEXTOOVERLAYMASK(1));
					cur_rec.left += (iIconDrawWidth - 3);

					if (thePrefs.ShowRatingIndicator() && (file->HasComment() || file->HasRating()))
					{
						m_ImageList.Draw(dc, file->UserRating()+3, CPoint(cur_rec.left, cur_rec.top), ILD_NORMAL);
						cur_rec.left += 16;
						iIconDrawWidth += 16;
					}

					cur_rec.left += 3;

					buffer = file->GetFileName();
					break;
				}
				case 1:
					buffer = CastItoXBytes(file->GetFileSize(), false, false);
					uDTFlags |= DT_RIGHT;
					break;
				case 2:
					buffer = file->GetFileTypeDisplayStr();
					break;
				case 3:{
					switch (file->GetUpPriority()) {
						case PR_VERYLOW :
							buffer = GetResString(IDS_PRIOVERYLOW);
							break;
						case PR_LOW :
							if( file->IsAutoUpPriority() )
								buffer = GetResString(IDS_PRIOAUTOLOW);
							else
								buffer = GetResString(IDS_PRIOLOW);
							break;
						case PR_NORMAL :
							if( file->IsAutoUpPriority() )
								buffer = GetResString(IDS_PRIOAUTONORMAL);
							else
								buffer = GetResString(IDS_PRIONORMAL);
							break;
						case PR_HIGH :
							if( file->IsAutoUpPriority() )
								buffer = GetResString(IDS_PRIOAUTOHIGH);
							else
								buffer = GetResString(IDS_PRIOHIGH);
							break;
						case PR_VERYHIGH :
							buffer = GetResString(IDS_PRIORELEASE);
							break;
						default:
							buffer.Empty();
					}
					break;
				}
				case 4:
					buffer = md4str(file->GetFileHash());
					break;
				case 5:
                    buffer.Format(_T("%u (%u)"), file->statistic.GetRequests(), file->statistic.GetAllTimeRequests());
					break;
				case 6:
					buffer.Format(_T("%u (%u)"), file->statistic.GetAccepts(), file->statistic.GetAllTimeAccepts());
					break;
				case 7:
					buffer.Format(_T("%s (%s)"), CastItoXBytes(file->statistic.GetTransferred(), false, false), CastItoXBytes(file->statistic.GetAllTimeTransferred(), false, false));
					break;
				case 8:
					if (file->GetPartCount()){
						cur_rec.bottom--;
						cur_rec.top++;
						file->DrawShareStatusBar(dc,&cur_rec,false);
						cur_rec.bottom++;
						cur_rec.top--;
					}
					break;
				case 9:
					buffer = file->GetPath();
					PathRemoveBackslash(buffer.GetBuffer());
					buffer.ReleaseBuffer();
					break;
				case 10:
                    if (file->m_nCompleteSourcesCountLo == file->m_nCompleteSourcesCountHi)
						buffer.Format(_T("%u"), file->m_nCompleteSourcesCountLo);
                    else if (file->m_nCompleteSourcesCountLo == 0)
						buffer.Format(_T("< %u"), file->m_nCompleteSourcesCountHi);
					else
						buffer.Format(_T("%u - %u"), file->m_nCompleteSourcesCountLo, file->m_nCompleteSourcesCountHi);
					break;
				case 11:{
					CPoint pt(cur_rec.left, cur_rec.top);
					m_ImageList.Draw(dc, file->GetPublishedED2K() ? 1 : 0, pt, ILD_NORMAL | ILD_TRANSPARENT);
					pt.x += 16;
					bool bSharedInKad;
					if ((uint32)time(NULL) < file->GetLastPublishTimeKadSrc())
					{
						if (theApp.IsFirewalled() && theApp.IsConnected())
						{
							if (theApp.clientlist->GetBuddy() && (file->GetLastPublishBuddy() == theApp.clientlist->GetBuddy()->GetIP()))
								bSharedInKad = true;
							else
								bSharedInKad = false;
						}
						else
							bSharedInKad = true;
					}
					else
						bSharedInKad = false;
					m_ImageList.Draw(dc, bSharedInKad ? 2 : 0, pt, ILD_NORMAL | ILD_TRANSPARENT);
					buffer.Empty();
					break;
				}
			}

			if (iColumn != 8)
				dc.DrawText(buffer, buffer.GetLength(), &cur_rec, uDTFlags);
			if (iColumn == 0)
				cur_rec.left -= iIconDrawWidth;
			cur_rec.left += GetColumnWidth(iColumn);
		}
	}
	ShowFilesCount();
	if (lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		RECT outline_rec = lpDrawItemStruct->rcItem;

		outline_rec.top--;
		outline_rec.bottom++;
		dc.FrameRect(&outline_rec, &CBrush(m_crWindow));
		outline_rec.top++;
		outline_rec.bottom--;
		outline_rec.left++;
		outline_rec.right--;

		if (lpDrawItemStruct->itemID > 0 && GetItemState(lpDrawItemStruct->itemID - 1, LVIS_SELECTED))
			outline_rec.top--;

		if (lpDrawItemStruct->itemID + 1 < (UINT)GetItemCount() && GetItemState(lpDrawItemStruct->itemID + 1, LVIS_SELECTED))
			outline_rec.bottom++;

		if(bCtrlFocused)
			dc.FrameRect(&outline_rec, &CBrush(m_crFocusLine));
		else
			dc.FrameRect(&outline_rec, &CBrush(m_crNoFocusLine));
	}
	
	if (m_crWindowTextBk == CLR_NONE)
		dc.SetBkMode(iOldBkMode);
	dc.SelectObject(pOldFont);
	dc.SetTextColor(crOldTextColor);
}

void CSharedFilesCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// get merged settings
	bool bFirstItem = true;
	int iSelectedItems = GetSelectedCount();
	int iCompleteFileSelected = -1;
	UINT uPrioMenuItem = 0;
	const CKnownFile* pSingleSelFile = NULL;
	POSITION pos = GetFirstSelectedItemPosition();
	while (pos)
	{
		const CKnownFile* pFile = (CKnownFile*)GetItemData(GetNextSelectedItem(pos));
		if (bFirstItem)
			pSingleSelFile = pFile;
		else
			pSingleSelFile = NULL;

		int iCurCompleteFile = pFile->IsPartFile() ? 0 : 1;
		if (bFirstItem)
			iCompleteFileSelected = iCurCompleteFile;
		else if (iCompleteFileSelected != iCurCompleteFile)
			iCompleteFileSelected = -1;

		UINT uCurPrioMenuItem = 0;
		if (pFile->IsAutoUpPriority())
			uCurPrioMenuItem = MP_PRIOAUTO;
		else if (pFile->GetUpPriority() == PR_VERYLOW)
			uCurPrioMenuItem = MP_PRIOVERYLOW;
		else if (pFile->GetUpPriority() == PR_LOW)
			uCurPrioMenuItem = MP_PRIOLOW;
		else if (pFile->GetUpPriority() == PR_NORMAL)
			uCurPrioMenuItem = MP_PRIONORMAL;
		else if (pFile->GetUpPriority() == PR_HIGH)
			uCurPrioMenuItem = MP_PRIOHIGH;
		else if (pFile->GetUpPriority() == PR_VERYHIGH)
			uCurPrioMenuItem = MP_PRIOVERYHIGH;
		else
			ASSERT(0);

		if (bFirstItem)
			uPrioMenuItem = uCurPrioMenuItem;
		else if (uPrioMenuItem != uCurPrioMenuItem)
			uPrioMenuItem = 0;

		bFirstItem = false;
	}

	m_SharedFilesMenu.EnableMenuItem((UINT_PTR)m_PrioMenu.m_hMenu, iSelectedItems > 0 ? MF_ENABLED : MF_GRAYED);
	m_PrioMenu.CheckMenuRadioItem(MP_PRIOVERYLOW, MP_PRIOAUTO, uPrioMenuItem, 0);

	bool bSingleCompleteFileSelected = (iSelectedItems == 1 && iCompleteFileSelected == 1);
	m_SharedFilesMenu.EnableMenuItem(MP_OPEN, bSingleCompleteFileSelected ? MF_ENABLED : MF_GRAYED);
	UINT uInsertedMenuItem = 0;
	m_SharedFilesMenu.EnableMenuItem(MP_OPENFOLDER, bSingleCompleteFileSelected ? MF_ENABLED : MF_GRAYED);
	m_SharedFilesMenu.EnableMenuItem(MP_RENAME, bSingleCompleteFileSelected ? MF_ENABLED : MF_GRAYED);
	m_SharedFilesMenu.EnableMenuItem(MP_REMOVE, iCompleteFileSelected > 0 ? MF_ENABLED : MF_GRAYED);
	m_SharedFilesMenu.SetDefaultItem(bSingleCompleteFileSelected ? MP_OPEN : -1);
	m_SharedFilesMenu.EnableMenuItem(MP_CMT, iSelectedItems > 0 ? MF_ENABLED : MF_GRAYED);
	m_SharedFilesMenu.EnableMenuItem(MP_DETAIL, iSelectedItems > 0 ? MF_ENABLED : MF_GRAYED);
	m_SharedFilesMenu.EnableMenuItem(thePrefs.GetShowCopyEd2kLinkCmd() ? MP_GETED2KLINK : MP_SHOWED2KLINK, iSelectedItems > 0 ? MF_ENABLED : MF_GRAYED);

	m_CollectionsMenu.EnableMenuItem(MP_MODIFYCOLLECTION, ( pSingleSelFile != NULL && pSingleSelFile->m_pCollection != NULL ) ? MF_ENABLED : MF_GRAYED);
	m_CollectionsMenu.EnableMenuItem(MP_VIEWCOLLECTION, ( pSingleSelFile != NULL && pSingleSelFile->m_pCollection != NULL ) ? MF_ENABLED : MF_GRAYED);
	m_CollectionsMenu.EnableMenuItem(MP_SEARCHAUTHOR, ( pSingleSelFile != NULL && pSingleSelFile->m_pCollection != NULL && !pSingleSelFile->m_pCollection->GetAuthorKeyHashString().IsEmpty()) ? MF_ENABLED : MF_GRAYED);
#if defined(_DEBUG)
	if (thePrefs.IsExtControlsEnabled()){
	//JOHNTODO: Not for release as we need kad lowID users in the network to see how well this work work. Also, we do not support these links yet.
	if (iSelectedItems > 0 && theApp.IsConnected() && theApp.IsFirewalled() && theApp.clientlist->GetBuddy())
		m_SharedFilesMenu.EnableMenuItem(MP_GETKADSOURCELINK, MF_ENABLED);
	else
		m_SharedFilesMenu.EnableMenuItem(MP_GETKADSOURCELINK, MF_GRAYED);
	}
#endif

	GetPopupMenuPos(*this, point);
	m_SharedFilesMenu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON,point.x,point.y,this);
}

BOOL CSharedFilesCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	CTypedPtrList<CPtrList, CKnownFile*> selectedList;
	POSITION pos = GetFirstSelectedItemPosition();
	while (pos != NULL){
		int index = GetNextSelectedItem(pos);
		if (index >= 0)
			selectedList.AddTail((CKnownFile*)GetItemData(index));
	}

	if (wParam==MP_CREATECOLLECTION || selectedList.GetCount() > 0)
	{
		CKnownFile* file = NULL;
		if (selectedList.GetCount() == 1)
			file = selectedList.GetHead();

		switch (wParam){
			case MP_GETED2KLINK:{
				CString str;
				POSITION pos = selectedList.GetHeadPosition();
				while (pos != NULL)
				{
					file = selectedList.GetNext(pos);
					if (!str.IsEmpty())
						str += _T("\r\n");
					str += CreateED2kLink(file);
				}
				theApp.CopyTextToClipboard(str);
				break;
			}
#if defined(_DEBUG)
			//JOHNTODO: Not for release as we need kad lowID users in the network to see how well this work work. Also, we do not support these links yet.
			case MP_GETKADSOURCELINK:{
				CString str;
				POSITION pos = selectedList.GetHeadPosition();
				while (pos != NULL)
				{
					file = selectedList.GetNext(pos);
					if (!str.IsEmpty())
						str += _T("\r\n");
					str += theApp.CreateKadSourceLink(file);
				}
				theApp.CopyTextToClipboard(str);
				break;
			}
#endif
			// file operations
			case MP_OPEN:
				if (file && !file->IsPartFile())
					OpenFile(file);
				break; 
			case MP_OPENFOLDER:
				if (file && !file->IsPartFile()){
					CString path = file->GetPath();
					int bspos = path.ReverseFind(_T('\\'));
					ShellExecute(NULL, _T("open"), path.Left(bspos), NULL, NULL, SW_SHOW);
				}
				break; 
			case MP_RENAME:
			case MPG_F2:
				if (file && !file->IsPartFile()){
					InputBox inputbox;
					CString title = GetResString(IDS_RENAME);
					title.Remove(_T('&'));
					inputbox.SetLabels(title, GetResString(IDS_DL_FILENAME), file->GetFileName());
					inputbox.SetEditFilenameMode();
					inputbox.DoModal();
					CString newname = inputbox.GetInput();
					if (!inputbox.WasCancelled() && newname.GetLength()>0)
					{
						// at least prevent users from specifying something like "..\dir\file"
						static const TCHAR _szInvFileNameChars[] = _T("\\/:*?\"<>|");
						if (newname.FindOneOf(_szInvFileNameChars) != -1){
							AfxMessageBox(GetErrorMessage(ERROR_BAD_PATHNAME));
							break;
						}

						CString newpath;
						PathCombine(newpath.GetBuffer(MAX_PATH), file->GetPath(), newname);
						newpath.ReleaseBuffer();
						if (_trename(file->GetFilePath(), newpath) != 0){
							CString strError;
							strError.Format(GetResString(IDS_ERR_RENAMESF), file->GetFilePath(), newpath, strerror(errno));
							AfxMessageBox(strError);
							break;
						}
						
						if (file->IsKindOf(RUNTIME_CLASS(CPartFile)))
							file->SetFileName(newname);
						else
						{
							theApp.sharedfiles->RemoveKeywords(file);
							file->SetFileName(newname);
							theApp.sharedfiles->AddKeywords(file);
						}
						file->SetFilePath(newpath);
						UpdateFile(file);
					}
				}
				else
					MessageBeep(MB_OK);
				break;
			case MP_REMOVE:
			case MPG_DELETE:{
				if (IDNO == AfxMessageBox(GetResString(IDS_CONFIRM_FILEDELETE),MB_ICONWARNING | MB_ICONQUESTION | MB_DEFBUTTON2 | MB_YESNO))
					return TRUE;

				SetRedraw(FALSE);
				bool bRemovedItems = false;
				while (!selectedList.IsEmpty())
				{
					CKnownFile* myfile = selectedList.RemoveHead();
					if (!myfile || myfile->IsPartFile())
						continue;
					
					BOOL delsucc = FALSE;
					if (!PathFileExists(myfile->GetFilePath()))
						delsucc = TRUE;
					else{
						// Delete
						if (!thePrefs.GetRemoveToBin()){
							delsucc = DeleteFile(myfile->GetFilePath());
						}
						else{
							// delete to recycle bin :(
							TCHAR todel[MAX_PATH+1];
//==> Optimizer [shadow2004]
#ifdef OPTIM
							memzero(todel, sizeof todel);
#else
							memset(todel, 0, sizeof todel);
#endif
//<== Optimizer [shadow2004]
							_tcsncpy(todel, myfile->GetFilePath(), ARRSIZE(todel)-2);

							SHFILEOPSTRUCT fp = {0};
							fp.wFunc = FO_DELETE;
							fp.hwnd = theApp.emuledlg->m_hWnd;
							fp.pFrom = todel;
							fp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SILENT;// | FOF_NOERRORUI
							delsucc = (SHFileOperation(&fp) == 0);
						}
					}
					if (delsucc){
						theApp.sharedfiles->RemoveFile(myfile);
						bRemovedItems = true;
						if (myfile->IsKindOf(RUNTIME_CLASS(CPartFile)))
							theApp.emuledlg->transferwnd->downloadlistctrl.ClearCompleted(static_cast<CPartFile*>(myfile));
					}
					else{
						CString strError;
						strError.Format( GetResString(IDS_ERR_DELFILE) + _T("\r\n\r\n%s"), myfile->GetFilePath(), GetErrorMessage(GetLastError()));
						AfxMessageBox(strError);
					}
				}
				SetRedraw(TRUE);
				if (bRemovedItems)
					AutoSelectItem();
				break; 
			}
			case MP_CMT:
				ShowFileDialog(selectedList, IDD_COMMENT);
                break; 
			case MPG_ALTENTER:
			case MP_DETAIL:
				ShowFileDialog(selectedList);
				break;
			case MP_CREATECOLLECTION:
			{
				CCollection* pCollection = new CCollection();
				POSITION pos = selectedList.GetHeadPosition();
				while (pos != NULL)
				{
					pCollection->AddFileToCollection(selectedList.GetNext(pos),true);
				}
				CCollectionCreateDialog dialog;
				dialog.SetCollection(pCollection);
				dialog.DoModal();
				//We delete this collection object because when the newly created
				//collection file is added to the sharedfile list, it is read and verified
				//and which creates the colleciton object that is attached to that file..
				delete pCollection;
				break;
			}
			case MP_SEARCHAUTHOR:
			{
				if (selectedList.GetCount() == 1 && file->m_pCollection)
				{
					SSearchParams* pParams = new SSearchParams;
					pParams->strExpression = file->m_pCollection->GetCollectionAuthorKeyString();
					pParams->eType = SearchTypeKademlia;
					pParams->strFileType = ED2KFTSTR_EMULECOLLECTION;
					pParams->strSpecialTitle = file->m_pCollection->m_sCollectionAuthorName;
					if (pParams->strSpecialTitle.GetLength() > 50){
						pParams->strSpecialTitle = pParams->strSpecialTitle.Left(50) + _T("...");
					}
					theApp.emuledlg->searchwnd->m_pwndResults->StartSearch(pParams);
				}
				break;
			}
			case MP_VIEWCOLLECTION:
			{
				if (selectedList.GetCount() == 1 && file->m_pCollection)
				{
					CCollectionViewDialog dialog;
					dialog.SetCollection(file->m_pCollection);
					dialog.DoModal();
				}
				break;
			}
			case MP_MODIFYCOLLECTION:
			{
				if (selectedList.GetCount() == 1 && file->m_pCollection)
				{
					CCollectionCreateDialog dialog;
					CCollection* pCollection = new CCollection(file->m_pCollection);
					dialog.SetCollection(pCollection);
					dialog.DoModal();
					delete pCollection;				
				}
				break;
			}
			case MP_SHOWED2KLINK:
				ShowFileDialog(selectedList, IDD_ED2KLINK);
				break;
			case MP_PRIOVERYLOW:
			case MP_PRIOLOW:
			case MP_PRIONORMAL:
			case MP_PRIOHIGH:
			case MP_PRIOVERYHIGH:
			case MP_PRIOAUTO:
				{
					POSITION pos = selectedList.GetHeadPosition();
					while (pos != NULL)
					{
						CKnownFile* file = selectedList.GetNext(pos);
						switch (wParam) {
							case MP_PRIOVERYLOW:
								file->SetAutoUpPriority(false);
								file->SetUpPriority(PR_VERYLOW);
								UpdateFile(file);
								break;
							case MP_PRIOLOW:
								file->SetAutoUpPriority(false);
								file->SetUpPriority(PR_LOW);
								UpdateFile(file);
								break;
							case MP_PRIONORMAL:
								file->SetAutoUpPriority(false);
								file->SetUpPriority(PR_NORMAL);
								UpdateFile(file);
								break;
							case MP_PRIOHIGH:
								file->SetAutoUpPriority(false);
								file->SetUpPriority(PR_HIGH);
								UpdateFile(file);
								break;
							case MP_PRIOVERYHIGH:
								file->SetAutoUpPriority(false);
								file->SetUpPriority(PR_VERYHIGH);
								UpdateFile(file);
								break;	
							case MP_PRIOAUTO:
								file->SetAutoUpPriority(true);
								file->UpdateAutoUpPriority();
								UpdateFile(file); 
								break;
						}
					}
					break;
				}
			default:
				break;
		}
	}
	return TRUE;
}

void CSharedFilesCtrl::OnColumnClick( NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// Barry - Store sort order in preferences
	// Determine ascending based on whether already sorted on this column

	bool sortAscending = (GetSortItem() != pNMListView->iSubItem) ? true : !GetSortAscending();

	// Ornis 4-way-sorting
	int adder=0;
	if (pNMListView->iSubItem>=5 && pNMListView->iSubItem<=7)
	{
		ASSERT( pNMListView->iSubItem - 5 < ARRSIZE(sortstat) );
		if (!sortAscending)
			sortstat[pNMListView->iSubItem - 5] = !sortstat[pNMListView->iSubItem - 5];
		adder = sortstat[pNMListView->iSubItem-5] ? 0 : 100;
	}
	else if (pNMListView->iSubItem==11)
	{
		ASSERT( 3 < ARRSIZE(sortstat) );
		if (!sortAscending)
			sortstat[3] = !sortstat[3];
		adder = sortstat[3] ? 0 : 100;
	}

	// Sort table
	if (adder==0)	
		SetSortArrow(pNMListView->iSubItem, sortAscending); 
	else
		SetSortArrow(pNMListView->iSubItem, sortAscending ? arrowDoubleUp : arrowDoubleDown);

	UpdateSortHistory(pNMListView->iSubItem + adder + (sortAscending ? 0:20),20);
	SortItems(SortProc, pNMListView->iSubItem + adder + (sortAscending ? 0:20));

	*pResult = 0;
}

int CSharedFilesCtrl::SortProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	const CKnownFile* item1 = (CKnownFile*)lParam1;
	const CKnownFile* item2 = (CKnownFile*)lParam2;	
	
	int iResult=0;
	switch(lParamSort){
		case 0: //filename asc
			iResult=CompareLocaleStringNoCase(item1->GetFileName(),item2->GetFileName());
			break;
		case 20: //filename desc
			iResult=CompareLocaleStringNoCase(item2->GetFileName(),item1->GetFileName());
			break;

		case 1: //filesize asc
			iResult=item1->GetFileSize()==item2->GetFileSize()?0:(item1->GetFileSize()>item2->GetFileSize()?1:-1);
			break;

		case 21: //filesize desc
			iResult=item1->GetFileSize()==item2->GetFileSize()?0:(item2->GetFileSize()>item1->GetFileSize()?1:-1);
			break;

		case 2: //filetype asc
			iResult=item1->GetFileTypeDisplayStr().Compare(item2->GetFileTypeDisplayStr());
			break;
		case 22: //filetype desc
			iResult=item2->GetFileTypeDisplayStr().Compare(item1->GetFileTypeDisplayStr());
			break;

		case 3: //prio asc
		{
			uint8 p1=item1->GetUpPriority() +1;
			if(p1==5)
				p1=0;
			uint8 p2=item2->GetUpPriority() +1;
			if(p2==5)
				p2=0;
			iResult=p1-p2;
			break;
		}
		case 23: //prio desc
		{
			uint8 p1=item1->GetUpPriority() +1;
			if(p1==5)
				p1=0;
			uint8 p2=item2->GetUpPriority() +1;
			if(p2==5)
				p2=0;
			iResult=p2-p1;
			break;
		}

		case 4: //fileID asc
			iResult=memcmp(item1->GetFileHash(), item2->GetFileHash(), 16);
			break;
		case 24: //fileID desc
			iResult=memcmp(item2->GetFileHash(), item1->GetFileHash(), 16);
			break;

		case 5: //requests asc
			iResult=item1->statistic.GetRequests() - item2->statistic.GetRequests();
			break;
		case 25: //requests desc
			iResult=item2->statistic.GetRequests() - item1->statistic.GetRequests();
			break;
		
		case 6: //acc requests asc
			iResult=item1->statistic.GetAccepts() - item2->statistic.GetAccepts();
			break;
		case 26: //acc requests desc
			iResult=item2->statistic.GetAccepts() - item1->statistic.GetAccepts();
			break;
		
		case 7: //all transferred asc
			iResult=item1->statistic.GetTransferred()==item2->statistic.GetTransferred()?0:(item1->statistic.GetTransferred()>item2->statistic.GetTransferred()?1:-1);
			break;
		case 27: //all transferred desc
			iResult=item1->statistic.GetTransferred()==item2->statistic.GetTransferred()?0:(item2->statistic.GetTransferred()>item1->statistic.GetTransferred()?1:-1);
			break;

		case 9: //folder asc
			iResult=CompareLocaleStringNoCase(item1->GetPath(),item2->GetPath());
			break;
		case 29: //folder desc
			iResult=CompareLocaleStringNoCase(item2->GetPath(),item1->GetPath());
			break;

		case 10: //complete sources asc
			iResult=CompareUnsigned(item1->m_nCompleteSourcesCount, item2->m_nCompleteSourcesCount);
			break;
		case 30: //complete sources desc
			iResult=CompareUnsigned(item2->m_nCompleteSourcesCount, item1->m_nCompleteSourcesCount);
			break;

		case 11: //ed2k shared asc
			iResult=item1->GetPublishedED2K() - item2->GetPublishedED2K();
			break;
		case 31: //ed2k shared desc
			iResult=item2->GetPublishedED2K() - item1->GetPublishedED2K();
			break;

		case 105: //all requests asc
			iResult=CompareUnsigned(item1->statistic.GetAllTimeRequests(), item2->statistic.GetAllTimeRequests());
			break;
		case 125: //all requests desc
			iResult=CompareUnsigned(item2->statistic.GetAllTimeRequests(), item1->statistic.GetAllTimeRequests());
			break;

		case 106: //all acc requests asc
			iResult=CompareUnsigned(item1->statistic.GetAllTimeAccepts(), item2->statistic.GetAllTimeAccepts());
			break;
		case 126: //all acc requests desc
			iResult=CompareUnsigned(item2->statistic.GetAllTimeAccepts(), item1->statistic.GetAllTimeAccepts());
			break;

		case 107: //all transferred asc
			iResult=item1->statistic.GetAllTimeTransferred()==item2->statistic.GetAllTimeTransferred()?0:(item1->statistic.GetAllTimeTransferred()>item2->statistic.GetAllTimeTransferred()?1:-1);
			break;
		case 127: //all transferred desc
			iResult=item1->statistic.GetAllTimeTransferred()==item2->statistic.GetAllTimeTransferred()?0:(item2->statistic.GetAllTimeTransferred()>item1->statistic.GetAllTimeTransferred()?1:-1);
			break;

		case 111:{ //kad shared asc
			uint32 tNow = time(NULL);
			int i1 = (tNow < item1->GetLastPublishTimeKadSrc()) ? 1 : 0;
			int i2 = (tNow < item2->GetLastPublishTimeKadSrc()) ? 1 : 0;
			iResult=i1 - i2;
			break;
		}
		case 131:{ //kad shared desc
			uint32 tNow = time(NULL);
			int i1 = (tNow < item1->GetLastPublishTimeKadSrc()) ? 1 : 0;
			int i2 = (tNow < item2->GetLastPublishTimeKadSrc()) ? 1 : 0;
			iResult=i2 - i1;
			break;
		}
		default: 
			iResult=0;
			break;
	}
	int dwNextSort;
	//call secondary sortorder, if this one results in equal
	//(Note: yes I know this call is evil OO wise, but better than changing a lot more code, while we have only one instance anyway - might be fixed later)
	if (iResult == 0 && (dwNextSort = theApp.emuledlg->sharedfileswnd->sharedfilesctrl.GetNextSortOrder(lParamSort)) != (-1)){
		iResult= SortProc(lParam1, lParam2, dwNextSort);
	}

	return iResult;

}

void CSharedFilesCtrl::OpenFile(const CKnownFile* file)
{
	if(file->m_pCollection)
	{
		CCollectionViewDialog dialog;
		dialog.SetCollection(file->m_pCollection);
		dialog.DoModal();
	}
	else
	ShellOpenFile(file->GetFilePath(), NULL);
}

void CSharedFilesCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iSel = GetNextItem(-1, LVIS_SELECTED | LVIS_FOCUSED);
	if (iSel != -1)
	{
		CKnownFile* file = (CKnownFile*)GetItemData(iSel);
		if (file)
		{
			if (GetKeyState(VK_MENU) & 0x8000)
			{
				CTypedPtrList<CPtrList, CKnownFile*> aFiles;
				aFiles.AddHead(file);
				ShowFileDialog(aFiles);
			}
			else if (!file->IsPartFile())
				OpenFile(file);
		}
	}
	*pResult = 0;
}

void CSharedFilesCtrl::CreateMenues()
{
	if (m_PrioMenu) VERIFY( m_PrioMenu.DestroyMenu() );
	if (m_CollectionsMenu) VERIFY( m_CollectionsMenu.DestroyMenu() );
	if (m_SharedFilesMenu) VERIFY( m_SharedFilesMenu.DestroyMenu() );

	m_PrioMenu.CreateMenu();
	m_PrioMenu.AppendMenu(MF_STRING,MP_PRIOVERYLOW,GetResString(IDS_PRIOVERYLOW));
	m_PrioMenu.AppendMenu(MF_STRING,MP_PRIOLOW,GetResString(IDS_PRIOLOW));
	m_PrioMenu.AppendMenu(MF_STRING,MP_PRIONORMAL,GetResString(IDS_PRIONORMAL));
	m_PrioMenu.AppendMenu(MF_STRING,MP_PRIOHIGH, GetResString(IDS_PRIOHIGH));
	m_PrioMenu.AppendMenu(MF_STRING,MP_PRIOVERYHIGH, GetResString(IDS_PRIORELEASE));
	m_PrioMenu.AppendMenu(MF_STRING,MP_PRIOAUTO, GetResString(IDS_PRIOAUTO));//UAP

	m_CollectionsMenu.CreateMenu();
	m_CollectionsMenu.AddMenuTitle(NULL, true);
	m_CollectionsMenu.AppendMenu(MF_STRING,MP_CREATECOLLECTION, GetResString(IDS_CREATECOLLECTION), _T("COLLECTION_ADD"));
	m_CollectionsMenu.AppendMenu(MF_STRING,MP_MODIFYCOLLECTION, GetResString(IDS_MODIFYCOLLECTION), _T("COLLECTION_EDIT"));
	m_CollectionsMenu.AppendMenu(MF_STRING,MP_VIEWCOLLECTION, GetResString(IDS_VIEWCOLLECTION), _T("COLLECTION_VIEW"));
	m_CollectionsMenu.AppendMenu(MF_STRING,MP_SEARCHAUTHOR, GetResString(IDS_SEARCHAUTHORCOLLECTION), _T("COLLECTION_SEARCH"));

	m_SharedFilesMenu.CreatePopupMenu();
	m_SharedFilesMenu.AddMenuTitle(GetResString(IDS_SHAREDFILES), true);

	m_SharedFilesMenu.AppendMenu(MF_STRING,MP_OPEN, GetResString(IDS_OPENFILE), _T("OPENFILE"));
	m_SharedFilesMenu.AppendMenu(MF_STRING,MP_OPENFOLDER, GetResString(IDS_OPENFOLDER), _T("OPENFOLDER"));
	m_SharedFilesMenu.AppendMenu(MF_STRING,MP_RENAME, GetResString(IDS_RENAME) + _T("..."), _T("FILERENAME"));
	m_SharedFilesMenu.AppendMenu(MF_STRING,MP_REMOVE, GetResString(IDS_DELETE), _T("DELETE"));

	m_SharedFilesMenu.AppendMenu(MF_STRING|MF_SEPARATOR);
	m_SharedFilesMenu.AppendMenu(MF_STRING|MF_POPUP,(UINT_PTR)m_PrioMenu.m_hMenu, GetResString(IDS_PRIORITY) + _T(" (") + GetResString(IDS_PW_CON_UPLBL) + _T(")"), _T("FILEPRIORITY"));
	m_SharedFilesMenu.AppendMenu(MF_STRING|MF_SEPARATOR);
	
	m_SharedFilesMenu.AppendMenu(MF_STRING|MF_POPUP,(UINT_PTR)m_CollectionsMenu.m_hMenu, GetResString(IDS_META_COLLECTION), _T("COLLECTION"));
	m_SharedFilesMenu.AppendMenu(MF_STRING|MF_SEPARATOR); 	

	m_SharedFilesMenu.AppendMenu(MF_STRING,MP_DETAIL, GetResString(IDS_SHOWDETAILS), _T("FILEINFO"));
	m_SharedFilesMenu.AppendMenu(MF_STRING,MP_CMT, GetResString(IDS_CMT_ADD), _T("FILECOMMENTS")); 
	if (thePrefs.GetShowCopyEd2kLinkCmd())
		m_SharedFilesMenu.AppendMenu(MF_STRING,MP_GETED2KLINK, GetResString(IDS_DL_LINK1), _T("ED2KLINK") );
	else
		m_SharedFilesMenu.AppendMenu(MF_STRING,MP_SHOWED2KLINK, GetResString(IDS_DL_SHOWED2KLINK), _T("ED2KLINK") );
//	m_SharedFilesMenu.AppendMenu(MF_STRING|MF_SEPARATOR); 



#if defined(_DEBUG)
	if (thePrefs.IsExtControlsEnabled()){
	//JOHNTODO: Not for release as we need kad lowID users in the network to see how well this work work. Also, we do not support these links yet.
	m_SharedFilesMenu.AppendMenu(MF_STRING,MP_GETKADSOURCELINK, _T("Copy eD2K Links To Clipboard (Kad)"));
	m_SharedFilesMenu.AppendMenu(MF_STRING|MF_SEPARATOR); 	
	}
#endif






}

void CSharedFilesCtrl::ShowComments(CKnownFile* file)
{
	if (file)
	{
		CTypedPtrList<CPtrList, CKnownFile*> aFiles;
		aFiles.AddHead(file);
		ShowFileDialog(aFiles, IDD_COMMENT);
	}
}

void CSharedFilesCtrl::OnGetDispInfo(NMHDR *pNMHDR, LRESULT *pResult)
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
			const CKnownFile* pFile = reinterpret_cast<CKnownFile*>(pDispInfo->item.lParam);
			if (pFile != NULL){
				switch (pDispInfo->item.iSubItem){
					case 0:
						if (pDispInfo->item.cchTextMax > 0){
							_tcsncpy(pDispInfo->item.pszText, pFile->GetFileName(), pDispInfo->item.cchTextMax);
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

void CSharedFilesCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == 'C' && (GetKeyState(VK_CONTROL) & 0x8000))
	{
		// Ctrl+C: Copy listview items to clipboard
		SendMessage(WM_COMMAND, MP_GETED2KLINK);
		return;
	}
	else if (nChar == VK_F5)
		ReloadFileList();

	CMuleListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CSharedFilesCtrl::ShowFileDialog(CTypedPtrList<CPtrList, CKnownFile*>& aFiles, UINT uPshInvokePage)
{
	if (aFiles.GetSize() > 0)
	{
		CFileDetailDialog dialog(aFiles, uPshInvokePage, this);
		dialog.DoModal();
	}
}

void CSharedFilesCtrl::SetDirectoryFilter(CDirectoryItem* pNewFilter, bool bRefresh){
	if (m_pDirectoryFilter == pNewFilter)
		return;
	m_pDirectoryFilter = pNewFilter;
	if (bRefresh)
		ReloadFileList();
}
