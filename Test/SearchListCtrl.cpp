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
#include "ResizableLib/ResizableSheet.h"
#include "SearchList.h"
#include "emuledlg.h"
#include "MetaDataDlg.h"
#include "SearchDlg.h"
#include "SearchListCtrl.h"
#include "SearchParams.h"
#include "ClosableTabCtrl.h"
#include "PreviewDlg.h"
#include "UpDownClient.h"
#include "ClientList.h"
#include "MemDC.h"
#include "SharedFileList.h"
#include "DownloadQueue.h"
#include "PartFile.h"
#include "KnownFileList.h"
#include "MenuCmds.h"
#include "OtherFunctions.h"
#include "Opcodes.h"
#include "Packets.h"
//==>removed WebService [shadow2004]
//#include "WebServices.h"
//<==removed WebService [shadow2004]
#include "Log.h"
#include "HighColorTab.hpp"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////////////
// CSearchResultFileDetailSheet

class CSearchResultFileDetailSheet : public CResizableSheet
{
	DECLARE_DYNAMIC(CSearchResultFileDetailSheet)

public:
	CSearchResultFileDetailSheet(const CSearchFile* file);
	virtual ~CSearchResultFileDetailSheet();

protected:
	const CSearchFile* m_file;
	CMetaDataDlg m_wndMetaData;

	static int sm_iLastActivePage;

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
};

int CSearchResultFileDetailSheet::sm_iLastActivePage;

IMPLEMENT_DYNAMIC(CSearchResultFileDetailSheet, CResizableSheet)

BEGIN_MESSAGE_MAP(CSearchResultFileDetailSheet, CResizableSheet)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

CSearchResultFileDetailSheet::CSearchResultFileDetailSheet(const CSearchFile* file)
{
	m_file = file;
	m_psh.dwFlags &= ~PSH_HASHELP;
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
	
	m_wndMetaData.m_psp.dwFlags &= ~PSP_HASHELP;
	m_wndMetaData.m_psp.dwFlags |= PSP_USEICONID;
	m_wndMetaData.m_psp.pszIcon = _T("METADATA");

	if (thePrefs.IsExtControlsEnabled())
		m_wndMetaData.SetFile(file);

	if (thePrefs.IsExtControlsEnabled())
		AddPage(&m_wndMetaData);
}

CSearchResultFileDetailSheet::~CSearchResultFileDetailSheet()
{
}

void CSearchResultFileDetailSheet::OnDestroy()
{
	sm_iLastActivePage = GetActiveIndex();
	CResizableSheet::OnDestroy();
}

BOOL CSearchResultFileDetailSheet::OnInitDialog()
{		
	EnableStackedTabs(FALSE);
	BOOL bResult = CResizableSheet::OnInitDialog();
	HighColorTab::UpdateImageList(*this);
	InitWindowStyles(this);
	EnableSaveRestore(_T("SearchResultFileDetailsSheet")); // call this after(!) OnInitDialog
	SetWindowText(GetResString(IDS_DETAILS) + _T(": ") + m_file->GetFileName());
	if (sm_iLastActivePage < GetPageCount())
		SetActivePage(sm_iLastActivePage);
	return bResult;
}


//////////////////////////////////////////////////////////////////////////////
// CSearchListCtrl
#define DLC_DT_TEXT (DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX|DT_END_ELLIPSIS)

IMPLEMENT_DYNAMIC(CSearchListCtrl, CMuleListCtrl)
CSearchListCtrl::CSearchListCtrl()
{
	searchlist = NULL;
	m_nResultsID = 0;
	SetGeneralPurposeFind(true);
}

void CSearchListCtrl::SetStyle()
{
	if (thePrefs.IsDoubleClickEnabled())
		SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	else
		SetExtendedStyle(LVS_EX_ONECLICKACTIVATE | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
}

void CSearchListCtrl::Init(CSearchList* in_searchlist)
{
	CImageList ilDummyImageList; //dummy list for getting the proper height of listview entries
	ilDummyImageList.Create(1, theApp.GetSmallSytemIconSize().cy,theApp.m_iDfltImageListColorFlags|ILC_MASK, 1, 1); 
	SetImageList(&ilDummyImageList, LVSIL_SMALL);
	ASSERT( (GetStyle() & LVS_SHAREIMAGELISTS) == 0 );
	ilDummyImageList.Detach();

	ModifyStyle(LVS_SINGLESEL,0);
	SetStyle();

	CToolTipCtrl* tooltip = GetToolTips();
	if (tooltip){
		tooltip->ModifyStyle(0, TTS_NOPREFIX);
		tooltip->SetDelayTime(TTDT_AUTOPOP, 20000);
		tooltip->SetDelayTime(TTDT_INITIAL, thePrefs.GetToolTipDelay()*1000);
	}
	searchlist = in_searchlist;

	InsertColumn(0,GetResString(IDS_DL_FILENAME),LVCFMT_LEFT,250);
	InsertColumn(1,GetResString(IDS_DL_SIZE),LVCFMT_LEFT,70);
	InsertColumn(2,GetResString(IDS_SEARCHAVAIL) + (thePrefs.IsExtControlsEnabled() ? _T(" (") + GetResString(IDS_DL_SOURCES) + _T(')') : _T("")),LVCFMT_LEFT,50);
	InsertColumn(3,GetResString(IDS_COMPLSOURCES),LVCFMT_LEFT,50);
	InsertColumn(4,GetResString(IDS_TYPE),LVCFMT_LEFT,65);
	InsertColumn(5,GetResString(IDS_FILEID),LVCFMT_LEFT,220);
	InsertColumn(6,GetResString(IDS_ARTIST),LVCFMT_LEFT,100);
	InsertColumn(7,GetResString(IDS_ALBUM),LVCFMT_LEFT,100);
	InsertColumn(8,GetResString(IDS_TITLE),LVCFMT_LEFT,100);
	InsertColumn(9,GetResString(IDS_LENGTH),LVCFMT_LEFT,50);
	InsertColumn(10,GetResString(IDS_BITRATE),LVCFMT_LEFT,50);
	InsertColumn(11,GetResString(IDS_CODEC),LVCFMT_LEFT,50);
	InsertColumn(12,GetResString(IDS_FOLDER),LVCFMT_LEFT,220);
	InsertColumn(13,GetResString(IDS_KNOWN),LVCFMT_LEFT,50);

	CreateMenues();

	LoadSettings(CPreferences::tableSearch);

	// Barry - Use preferred sort order from preferences
	int sortItem = thePrefs.GetColumnSortItem(CPreferences::tableSearch);
	if (sortItem != -1){// don't force a sorting if '-1' is specified, so we can better see how the search results are arriving
		bool sortAscending = thePrefs.GetColumnSortAscending(CPreferences::tableSearch);
		SetSortArrow(sortItem, sortAscending);
		SortItems(SortProc, sortItem + (sortAscending ? 0:100));
	}
}

CSearchListCtrl::~CSearchListCtrl(){
}

void CSearchListCtrl::Localize()
{
	CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();
	HDITEM hdi;
	hdi.mask = HDI_TEXT;
	CString strRes;

	for (int icol=0;icol<pHeaderCtrl->GetItemCount();icol++) {
		switch (icol) {
			case 0: strRes = GetResString(IDS_DL_FILENAME); break;
			case 1: strRes = GetResString(IDS_DL_SIZE); break;
			case 2: strRes = GetResString(IDS_SEARCHAVAIL) + (thePrefs.IsExtControlsEnabled() ? _T(" (") + GetResString(IDS_DL_SOURCES) + _T(')') : _T("")); break;
			case 3: strRes = GetResString(IDS_COMPLSOURCES); break;
			case 4: strRes = GetResString(IDS_TYPE); break;
			case 5: strRes = GetResString(IDS_FILEID); break;
			case 6: strRes = GetResString(IDS_ARTIST); break;
			case 7: strRes = GetResString(IDS_ALBUM); break;
			case 8: strRes = GetResString(IDS_TITLE); break;
			case 9: strRes = GetResString(IDS_LENGTH); break;
			case 10: strRes = GetResString(IDS_BITRATE); break;
			case 11: strRes = GetResString(IDS_CODEC); break;
			case 12: strRes = GetResString(IDS_FOLDER); break;
			case 13: strRes = GetResString(IDS_KNOWN); break;
		}
	
		hdi.pszText = strRes.GetBuffer();
		pHeaderCtrl->SetItem(icol, &hdi);
		strRes.ReleaseBuffer();
	}

	CreateMenues();
}

BEGIN_MESSAGE_MAP(CSearchListCtrl, CMuleListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(LVN_GETINFOTIP, OnLvnGetInfoTip)
	ON_NOTIFY_REFLECT(LVN_DELETEALLITEMS, OnLvnDeleteallitems)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT(NM_DBLCLK,OnDblClick)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CSearchListCtrl message handlers

void CSearchListCtrl::AddResult(const CSearchFile* toshow)
{
	// update tab-counter for the given searchfile
	CClosableTabCtrl& searchselect = theApp.emuledlg->searchwnd->GetSearchSelector();
	int iTabItems = searchselect.GetItemCount();
	if (iTabItems > 0)
	{
		TCITEM tabitem;
		tabitem.mask = TCIF_PARAM;
		tabitem.lParam = -1;
		for (int tabCounter = 0; tabCounter < iTabItems; tabCounter++)
		{
			if (searchselect.GetItem(tabCounter, &tabitem) && tabitem.lParam != NULL)
			{
				if (((const SSearchParams*)tabitem.lParam)->dwSearchID == toshow->GetSearchID())
				{
					TCHAR szText[MAX_SEARCH_EXPRESSION_LEN+1];
					tabitem.pszText = szText;
					tabitem.cchTextMax = ARRSIZE(szText);
					tabitem.mask = TCIF_TEXT;
					if (searchselect.GetItem(tabCounter, &tabitem))
					{
						// TODO: Searching for the last '(' is wrong.
						// If the search expression contains '(' and ')' characters, we'll truncate the string.
						LPTSTR psz = _tcsrchr(szText, _T('('));
						if (psz){
							if (psz > szText)
								psz--;
							*psz = _T('\0');
						}

						CString sourceStr;
						sourceStr.Format(_T("%s (%u)"), szText, searchlist->GetFoundFiles(toshow->GetSearchID()));
						tabitem.pszText = const_cast<LPTSTR>((LPCTSTR)sourceStr);
						searchselect.SetItem(tabCounter, &tabitem);

						if (searchselect.GetCurSel() != tabCounter)
							searchselect.HighlightItem(tabCounter);
					}
					break;
				}
			}
		}
	}

	if (toshow->GetSearchID() != m_nResultsID)
		return;

	//TODO: Here we have a problem. Since this listview control is owner drawn the listview items may no longer
	//contain the actual information which is displayed. The usage of the 'Find' command may therefore deal with
	//wrong item contents (which are stored in the listview items right here in this function).

	int itemnr = InsertItem(LVIF_TEXT|LVIF_PARAM,GetItemCount(),toshow->GetFileName(),0,0,0,(LPARAM)toshow);
	
	SetItemText(itemnr,1,CastItoXBytes(toshow->GetFileSize(), false, false));
	
	CString strBuffer;
	uint32 nSources = toshow->GetSourceCount();	
	int iClients = toshow->GetClientsCount();
	if (thePrefs.IsExtControlsEnabled() && iClients > 0)
		strBuffer.Format(_T("%u (%u)"), nSources, iClients);
	else
		strBuffer.Format(_T("%u"), nSources);
	SetItemText(itemnr,2,strBuffer);
	SetItemText(itemnr,3,GetCompleteSourcesDisplayString(toshow, nSources));
	SetItemText(itemnr,4,toshow->GetFileTypeDisplayStr());
	SetItemText(itemnr,5,md4str(toshow->GetFileHash()));
	SetItemText(itemnr,6,toshow->GetStrTagValue(FT_MEDIA_ARTIST));
	SetItemText(itemnr,7,toshow->GetStrTagValue(FT_MEDIA_ALBUM));
	SetItemText(itemnr,8,toshow->GetStrTagValue(FT_MEDIA_TITLE));
	uint32 nMediaLength = toshow->GetIntTagValue(FT_MEDIA_LENGTH);
	TCHAR cbuffer[80];
	if (nMediaLength){
		CString strMediaLength;
		SecToTimeLength(nMediaLength, strMediaLength);
		_sntprintf(cbuffer,ARRSIZE(cbuffer),_T("%s"),strMediaLength);
	}
	else
		cbuffer[0] = _T('\0');
	SetItemText(itemnr,9,cbuffer);
	uint32 nBitrate = toshow->GetIntTagValue(FT_MEDIA_BITRATE);
	if (nBitrate)
		_sntprintf(cbuffer,ARRSIZE(cbuffer),_T("%u kBit/s"),nBitrate);
	else
		cbuffer[0] = _T('\0');
	SetItemText(itemnr,10,cbuffer);
	SetItemText(itemnr,11,toshow->GetStrTagValue(FT_MEDIA_CODEC));

	if (toshow->GetDirectory())
		SetItemText(itemnr,12,toshow->GetDirectory());

	if (toshow->m_eKnown == CSearchFile::Shared)
		SetItemText(itemnr,13,GetResString(IDS_SHARED));
	else if (toshow->m_eKnown == CSearchFile::Downloading)
		SetItemText(itemnr,13,GetResString(IDS_DOWNLOADING));
	else if (toshow->m_eKnown == CSearchFile::Downloaded)
		SetItemText(itemnr,13,GetResString(IDS_DOWNLOADED));
}

void CSearchListCtrl::UpdateSources(const CSearchFile* toupdate)
{
	LVFINDINFO find;
	find.flags = LVFI_PARAM;
	find.lParam = (LPARAM)toupdate;
	int index = FindItem(&find);
	if (index != (-1)){

		CString strBuffer;
		uint32 nSources = toupdate->GetSourceCount();	
		int iClients = toupdate->GetClientsCount();
		if ( thePrefs.IsExtControlsEnabled() && iClients > 0)
			strBuffer.Format(_T("%u (%u)"), nSources, iClients);
		else
			strBuffer.Format(_T("%u"), nSources);
		SetItemText(index,2,strBuffer);
		SetItemText(index,3,GetCompleteSourcesDisplayString(toupdate, nSources));

		uint16 maxhitsname = (uint16)-1;
		bool change=false;
		CString strFileName = toupdate->GetFileName();
		// update expanded childs
		if (toupdate->GetListIsExpanded()) {
			for (POSITION pos = theApp.searchlist->list.GetHeadPosition(); pos != NULL; ){
				const CSearchFile* cur_file = theApp.searchlist->list.GetNext(pos);
				if (cur_file->GetListParent()==toupdate) {
					if (maxhitsname<cur_file->GetListChildCount()) {
						maxhitsname=cur_file->GetListChildCount();
						strFileName=cur_file->GetFileName();
						change=true;
					}
					// i am your child, darth! but do I exist? Do I?
					LVFINDINFO find;
					find.flags = LVFI_PARAM;
					find.lParam = (LPARAM)cur_file;
					int index2 = FindItem(&find);
					if (index2!=-1)
						// yes, I see you...
						Update(index2);
					else
						// no, but I create you now for display
						InsertItem(LVIF_PARAM|LVIF_TEXT,index+1,cur_file->GetFileName(),0,0,0,(LPARAM)cur_file);
				}
			}
		}

		// set name in list to the most used filename
		if (change)
			SetItemText(index,0,strFileName);
		Update(index);
	}
}

CString CSearchListCtrl::GetCompleteSourcesDisplayString(const CSearchFile* pFile, UINT uSources, bool* pbComplete) const
{
	UINT uCompleteSources = pFile->GetIntTagValue(FT_COMPLETE_SOURCES);
	int iComplete = pFile->IsComplete(uSources, uCompleteSources);

	CString str;
	if (iComplete < 0)			// '< 0' ... unknown
	{
		str = _T("?");
		if (pbComplete)
			*pbComplete = true;	// treat 'unknown' as complete
	}
	else if (iComplete > 0)		// '> 0' ... we know it's complete
	{
		ASSERT( uSources );
		if (uSources)
//==> Search Complete Sources XT [shadow2004]
#ifdef SourceXT
			str.Format(_T("%u (%u%%)"), uCompleteSources, (uCompleteSources*100)/uSources);
#else //SourceXT
			str.Format(_T("%u%%"), (uCompleteSources*100)/uSources);
#endif //SourcesXT
//<== Search Complete Sources XT [shadow2004]
            
		if (pbComplete)
			*pbComplete = true;
	}
	else						// '= 0' ... we know it's not complete
	{
//==> Search Complete Sources XT [shadow2004]
#ifdef SourceXT
				str = _T("0 (0%)");
#else //SourceXT
                str = _T("0%");
#endif //SourcesXT
//<== Search Complete Sources XT [shadow2004]
		
		if (pbComplete)
			*pbComplete = false;
	}
	return str;
}

void CSearchListCtrl::RemoveResult(const CSearchFile* toremove)
{
	LVFINDINFO find;
	find.flags = LVFI_PARAM;
	find.lParam = (LPARAM)toremove;
	sint32 result = FindItem(&find);
	if(result != (-1) )
		DeleteItem(result);
}

void CSearchListCtrl::ShowResults(uint32 nResultsID)
{
	DeleteAllItems();
	m_nResultsID = nResultsID;
	searchlist->ShowResults(m_nResultsID);
}

void CSearchListCtrl::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// Barry - Store sort order in preferences
	// Determine ascending based on whether already sorted on this column
	int sortItem = thePrefs.GetColumnSortItem(CPreferences::tableSearch);
	bool m_oldSortAscending = thePrefs.GetColumnSortAscending(CPreferences::tableSearch);
	bool sortAscending = (sortItem != pNMListView->iSubItem) ? true : !m_oldSortAscending;

	// Item is column clicked
	sortItem = pNMListView->iSubItem;

	// Save new preferences
	thePrefs.SetColumnSortItem(CPreferences::tableSearch, sortItem);
	thePrefs.SetColumnSortAscending(CPreferences::tableSearch, sortAscending);

	// Sort table
	SetSortArrow(sortItem, sortAscending);
	SortItems(SortProc, sortItem + (sortAscending ? 0:100));

	*pResult = 0;
}

int CSearchListCtrl::SortProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	const CSearchFile* item1 = (CSearchFile*)lParam1;
	const CSearchFile* item2 = (CSearchFile*)lParam2;

	int sortMod = 1;
	if(lParamSort >= 100) {
		sortMod = -1;
		lParamSort -= 100;
	}

	int comp;

	if (item1->GetListParent()==NULL && item2->GetListParent()!=NULL){
		if (item1 == item2->GetListParent())
			return -1;
		comp = Compare(item1, item2->m_list_parent, lParamSort) * sortMod;
	}
	else if (item2->GetListParent()==NULL && item1->GetListParent()!=NULL){
		if (item1->m_list_parent == item2)
			return 1;
		comp = Compare(item1->GetListParent(), item2, lParamSort) * sortMod;
	}
	else if (item1->GetListParent()==NULL){
		comp = Compare(item1, item2, lParamSort) * sortMod;
	}
	else{
		comp = Compare(item1->GetListParent(), item2->GetListParent(), lParamSort);
		if (comp != 0)
			return sortMod * comp;

		if ((item1->GetListParent()==NULL && item2->GetListParent()!=NULL) || (item2->GetListParent()==NULL && item1->GetListParent()!=NULL)){
			if (item1->GetListParent()==NULL)
				return -1;
			else
				return 1;
		}
		comp = CompareChild(item1, item2, lParamSort);
	}

	return comp;
}

int CSearchListCtrl::CompareChild(const CSearchFile* item1, const CSearchFile* item2, LPARAM lParamSort)
{
	switch(lParamSort){
		case 0: //filename asc
			return CompareLocaleStringNoCase(item1->GetFileName(),item2->GetFileName());
		case 100: //filename desc
			return CompareLocaleStringNoCase(item2->GetFileName(),item1->GetFileName());
		default:
			// always sort by descending availability
			return CompareUnsigned(item2->GetIntTagValue(FT_SOURCES), item1->GetIntTagValue(FT_SOURCES));
	}
}

int CSearchListCtrl::Compare(const CSearchFile* item1, const CSearchFile* item2, LPARAM lParamSort)
{
	switch(lParamSort){
		case 0: //filename asc
			return CompareLocaleStringNoCase(item1->GetFileName(),item2->GetFileName());
		case 100: //filename desc
			return CompareLocaleStringNoCase(item2->GetFileName(),item1->GetFileName());

		case 1: //size asc
			return CompareUnsigned(item1->GetFileSize(), item2->GetFileSize());
		case 101: //size desc
			return CompareUnsigned(item2->GetFileSize(), item1->GetFileSize());

		case 2: //sources asc
			return CompareUnsigned(item1->GetIntTagValue(FT_SOURCES), item2->GetIntTagValue(FT_SOURCES));
		case 102: //sources desc
			return CompareUnsigned(item2->GetIntTagValue(FT_SOURCES), item1->GetIntTagValue(FT_SOURCES));

		case 3: // complete sources asc
			if (item1->GetIntTagValue(FT_SOURCES) == 0 || item2->GetIntTagValue(FT_SOURCES) == 0 || item1->IsKademlia() || item2->IsKademlia() )
				return 0; // should never happen, just a sanity check
//==> Search Complete Sources XT [shadow2004]
#ifdef SourceXT
				return CompareUnsigned(item1->GetIntTagValue(FT_COMPLETE_SOURCES), item2->GetIntTagValue(FT_COMPLETE_SOURCES));
#else //SourceXT
				return CompareUnsigned((item1->GetIntTagValue(FT_COMPLETE_SOURCES)*100)/item1->GetIntTagValue(FT_SOURCES), (item2->GetIntTagValue(FT_COMPLETE_SOURCES)*100)/item2->GetIntTagValue(FT_SOURCES));
#endif SourceXT
//<== Search Complete Sources XT [shadow2004]
                        
		case 103: //complete sources desc
			if (item1->GetIntTagValue(FT_SOURCES) == 0 || item2->GetIntTagValue(FT_SOURCES) == 0 || item1->IsKademlia() || item2->IsKademlia())
				return 0; // should never happen, just a sanity check
//==> Search Complete Sources XT [shadow2004]
#ifdef SourceXT
                return CompareUnsigned(item2->GetIntTagValue(FT_COMPLETE_SOURCES), item1->GetIntTagValue(FT_COMPLETE_SOURCES));
#else //SourceXT
				return CompareUnsigned((item2->GetIntTagValue(FT_COMPLETE_SOURCES)*100)/item2->GetIntTagValue(FT_SOURCES), (item1->GetIntTagValue(FT_COMPLETE_SOURCES)*100)/item1->GetIntTagValue(FT_SOURCES));
#endif SourceXT
//<== Search Complete Sources XT [shadow2004]

		case 4: //type asc
			return item1->GetFileTypeDisplayStr().Compare(item2->GetFileTypeDisplayStr());
		case 104: //type  desc
			return item2->GetFileTypeDisplayStr().Compare(item1->GetFileTypeDisplayStr());

		case 5: //filehash asc
			return memcmp(item1->GetFileHash(),item2->GetFileHash(),16);
		case 105: //filehash desc
			return memcmp(item2->GetFileHash(),item1->GetFileHash(),16);

		case 6:
			return CompareOptLocaleStringNoCase(item1->GetStrTagValue(FT_MEDIA_ARTIST), item2->GetStrTagValue(FT_MEDIA_ARTIST));
		case 106:
			return -CompareOptLocaleStringNoCase(item1->GetStrTagValue(FT_MEDIA_ARTIST), item2->GetStrTagValue(FT_MEDIA_ARTIST));

		case 7:
			return CompareOptLocaleStringNoCase(item1->GetStrTagValue(FT_MEDIA_ALBUM), item2->GetStrTagValue(FT_MEDIA_ALBUM));
		case 107:
			return -CompareOptLocaleStringNoCase(item1->GetStrTagValue(FT_MEDIA_ALBUM), item2->GetStrTagValue(FT_MEDIA_ALBUM));

		case 8:
			return CompareOptLocaleStringNoCase(item1->GetStrTagValue(FT_MEDIA_TITLE), item2->GetStrTagValue(FT_MEDIA_TITLE));
		case 108:
			return -CompareOptLocaleStringNoCase(item1->GetStrTagValue(FT_MEDIA_TITLE), item2->GetStrTagValue(FT_MEDIA_TITLE));

		case 9:
			return CompareUnsigned(item1->GetIntTagValue(FT_MEDIA_LENGTH), item2->GetIntTagValue(FT_MEDIA_LENGTH));
		case 109:
			return -CompareUnsigned(item1->GetIntTagValue(FT_MEDIA_LENGTH), item2->GetIntTagValue(FT_MEDIA_LENGTH));

		case 10:
			return CompareUnsigned(item1->GetIntTagValue(FT_MEDIA_BITRATE), item2->GetIntTagValue(FT_MEDIA_BITRATE));
		case 110:
			return -CompareUnsigned(item1->GetIntTagValue(FT_MEDIA_BITRATE), item2->GetIntTagValue(FT_MEDIA_BITRATE));

		case 11:
			return CompareOptLocaleStringNoCase(item1->GetStrTagValue(FT_MEDIA_CODEC), item2->GetStrTagValue(FT_MEDIA_CODEC));
		case 111:
			return -CompareOptLocaleStringNoCase(item1->GetStrTagValue(FT_MEDIA_CODEC), item2->GetStrTagValue(FT_MEDIA_CODEC));

		case 12: //path asc
			return CompareOptLocaleStringNoCase(item1->GetDirectory(), item2->GetDirectory());
		case 112: //path desc
			return -CompareOptLocaleStringNoCase(item1->GetDirectory(), item2->GetDirectory());

		case 13:
			return item1->GetKnownType() - item2->GetKnownType();
		case 113:
			return -(item1->GetKnownType() - item2->GetKnownType());

		default:
			return 0;
	}
}

void CSearchListCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
	int iSelected = 0;
	int iToDownload = 0;
	int iToPreview = 0;
	POSITION pos = GetFirstSelectedItemPosition();
	while (pos != NULL)
	{
		const CSearchFile* pFile = (CSearchFile*)GetItemData(GetNextSelectedItem(pos));
		if (pFile)
		{
			iSelected++;
			if (pFile->IsPreviewPossible())
				iToPreview++;
			if (!theApp.downloadqueue->IsFileExisting(pFile->GetFileHash(), false))
				iToDownload++;
		}
	}

	if (thePrefs.IsExtControlsEnabled())
		m_SearchFileMenu.EnableMenuItem(MP_RESUMEPAUSED, iToDownload > 0 ? MF_ENABLED : MF_GRAYED);
	m_SearchFileMenu.EnableMenuItem(MP_RESUME, iToDownload > 0 ? MF_ENABLED : MF_GRAYED);
	m_SearchFileMenu.EnableMenuItem(MP_DETAIL, iSelected == 1 ? MF_ENABLED : MF_GRAYED);
	m_SearchFileMenu.EnableMenuItem(MP_GETED2KLINK, iSelected > 0 ? MF_ENABLED : MF_GRAYED);
	m_SearchFileMenu.EnableMenuItem(MP_GETHTMLED2KLINK, iSelected > 0 ? MF_ENABLED : MF_GRAYED);
	m_SearchFileMenu.EnableMenuItem(MP_REMOVESELECTED, iSelected > 0 ? MF_ENABLED : MF_GRAYED);
	m_SearchFileMenu.EnableMenuItem(MP_PREVIEW, iToPreview == 1 ? MF_ENABLED : MF_GRAYED);
	m_SearchFileMenu.EnableMenuItem(MP_REMOVEALL, theApp.emuledlg->searchwnd->CanDeleteAllSearches() ? MF_ENABLED : MF_GRAYED);
	m_SearchFileMenu.EnableMenuItem(MP_REMOVE, theApp.emuledlg->searchwnd->CanDeleteSearch(m_nResultsID) ? MF_ENABLED : MF_GRAYED);
	m_SearchFileMenu.EnableMenuItem(MP_FIND, GetItemCount() > 0 ? MF_ENABLED : MF_GRAYED);

//==>removed WebService [shadow2004]
	/*CTitleMenu WebMenu;
	WebMenu.CreateMenu();
	WebMenu.AddMenuTitle(NULL, true);
	int iWebMenuEntries = theWebServices.GetFileMenuEntries(&WebMenu);
	UINT flag2 = (iWebMenuEntries == 0 || iSelected != 1) ? MF_GRAYED : MF_STRING;
	m_SearchFileMenu.AppendMenu(MF_POPUP | flag2, (UINT_PTR)WebMenu.m_hMenu, GetResString(IDS_WEBSERVICES), _T("WEB"));
*/
//<==removed WebService [shadow2004]
	
	if (iToDownload > 0)
		m_SearchFileMenu.SetDefaultItem( ( !thePrefs.AddNewFilesPaused() || !thePrefs.IsExtControlsEnabled() )?MP_RESUME:MP_RESUMEPAUSED);
//==>removed WebService [shadow2004]
//	else
//		m_SearchFileMenu.SetDefaultItem((UINT)-1);
//<==removed WebService [shadow2004]
	GetPopupMenuPos(*this, point);
	m_SearchFileMenu.TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON,point.x,point.y,this);
//==>removed WebService [shadow2004]
/*	m_SearchFileMenu.RemoveMenu(m_SearchFileMenu.GetMenuItemCount()-1,MF_BYPOSITION);
	VERIFY( WebMenu.DestroyMenu() );*/
//<==removed WebService [shadow2004]
}


BOOL CSearchListCtrl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (wParam == MP_FIND)
	{
		OnFindStart();
		return TRUE;
	}

	int iSel = GetNextItem(-1, LVIS_SELECTED | LVIS_FOCUSED);
	if (iSel != -1)
	{
		const CSearchFile* file = (CSearchFile*)GetItemData(iSel);
		switch (wParam)
		{
			case MP_GETED2KLINK:{
				CWaitCursor curWait;
				CString clpbrd;
				POSITION pos = GetFirstSelectedItemPosition();
				while (pos!=NULL)
				{
					if (!clpbrd.IsEmpty())
						clpbrd += _T("\r\n");
					clpbrd += CreateED2kLink((CSearchFile*)GetItemData(GetNextSelectedItem(pos)));
				}
				theApp.CopyTextToClipboard(clpbrd);
				return TRUE;
			}
			case MP_GETHTMLED2KLINK:{
				CWaitCursor curWait;
				CString clpbrd;
				POSITION pos = GetFirstSelectedItemPosition();
				while (pos!=NULL)
				{
					if (!clpbrd.IsEmpty())
						clpbrd += _T("<br />\r\n");
					clpbrd += CreateHTMLED2kLink((CSearchFile*)GetItemData(GetNextSelectedItem(pos)));
				}
				theApp.CopyTextToClipboard(clpbrd);
				return TRUE;
			}
			case MP_RESUMEPAUSED:
			case MP_RESUME:
				theApp.emuledlg->searchwnd->DownloadSelected(wParam==MP_RESUMEPAUSED);
				return TRUE;
			case MPG_DELETE:
			case MP_REMOVESELECTED:{
				CWaitCursor curWait;
				SetRedraw(FALSE);
				while (GetFirstSelectedItemPosition()!=NULL) 
				{
					POSITION pos = GetFirstSelectedItemPosition();
					CSearchFile* pItem = (CSearchFile*)GetItemData(GetNextSelectedItem(pos));
					HideSources(pItem);
					theApp.searchlist->RemoveResult(pItem);
				}
				SetRedraw(TRUE);
				return TRUE;
			}
			case MPG_ALTENTER:
			case MP_DETAIL:
				if (file){
					CSearchResultFileDetailSheet sheet(file);
					sheet.DoModal();
				}
				return TRUE;
			case MP_PREVIEW:
				if (file){
					if (file->GetPreviews().GetSize() > 0){
						// already have previews
						(new PreviewDlg())->SetFile(file);
					}
					else{
						CUpDownClient* newclient = new CUpDownClient(NULL, file->GetClientPort(),file->GetClientID(),file->GetClientServerIP(),file->GetClientServerPort(), true);
						if (!theApp.clientlist->AttachToAlreadyKnown(&newclient,NULL)){
							theApp.clientlist->AddClient(newclient);
						}
						newclient->SendPreviewRequest(file);
						// add to res - later
						AddLogLine(true, _T("Preview Requested - Please wait"));
					}
				}
				return TRUE;
			default:
//==>removed WebService [shadow2004]
/*				if (wParam>=MP_WEBURL && wParam<=MP_WEBURL+256){
					theWebServices.RunURL(file, wParam);
					return TRUE;
				}*/
//<==removed WebService [shadow2004]
				break;
		}
	}
	switch (wParam){
		case MP_REMOVEALL:{
			CWaitCursor curWait;
			theApp.emuledlg->searchwnd->DeleteAllSearchs();
			break;
		}
		case MP_REMOVE:{
			CWaitCursor curWait;
			theApp.emuledlg->searchwnd->DeleteSearch(m_nResultsID);
			break;
		}
	}

	return FALSE;
}

void CSearchListCtrl::OnLvnDeleteallitems(NMHDR *pNMHDR, LRESULT *pResult)
{
	// To suppress subsequent LVN_DELETEITEM notification messages, return TRUE.
	*pResult = TRUE;
}

void CSearchListCtrl::CreateMenues()
{
	if (m_SearchFileMenu)
		VERIFY( m_SearchFileMenu.DestroyMenu() );

	m_SearchFileMenu.CreatePopupMenu();
	m_SearchFileMenu.AddMenuTitle(GetResString(IDS_FILE), true);
	m_SearchFileMenu.AppendMenu(MF_STRING,MP_RESUME, GetResString(IDS_DOWNLOAD), _T("RESUME"));

	if (thePrefs.IsExtControlsEnabled())
		m_SearchFileMenu.AppendMenu(MF_STRING, MP_RESUMEPAUSED, GetResString(IDS_DOWNLOAD) + _T(" (") + GetResString(IDS_PAUSED) + _T(")"));

	m_SearchFileMenu.AppendMenu(MF_STRING,MP_PREVIEW, GetResString(IDS_DL_PREVIEW), _T("PREVIEW"));
	if (thePrefs.IsExtControlsEnabled())
		m_SearchFileMenu.AppendMenu(MF_STRING,MP_DETAIL, GetResString(IDS_SHOWDETAILS), _T("FILEINFO"));
	m_SearchFileMenu.AppendMenu(MF_SEPARATOR);
	m_SearchFileMenu.AppendMenu(MF_STRING,MP_GETED2KLINK, GetResString(IDS_DL_LINK1), _T("ED2KLINK"));
	m_SearchFileMenu.AppendMenu(MF_STRING,MP_GETHTMLED2KLINK, GetResString(IDS_DL_LINK2), _T("ED2KLINK"));
	m_SearchFileMenu.AppendMenu(MF_SEPARATOR);
	m_SearchFileMenu.AppendMenu(MF_STRING,MP_REMOVESELECTED, GetResString(IDS_REMOVESELECTED), _T("DELETESELECTED"));
	m_SearchFileMenu.AppendMenu(MF_STRING,MP_REMOVE, GetResString(IDS_REMOVESEARCHSTRING), _T("DELETE"));
	m_SearchFileMenu.AppendMenu(MF_STRING,MP_REMOVEALL, GetResString(IDS_REMOVEALLSEARCH), _T("CLEARCOMPLETE"));

	m_SearchFileMenu.AppendMenu(MF_SEPARATOR);
	m_SearchFileMenu.AppendMenu(MF_STRING, MP_FIND, GetResString(IDS_FIND), _T("Search"));
}

void CSearchListCtrl::OnLvnGetInfoTip(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVGETINFOTIP pGetInfoTip = reinterpret_cast<LPNMLVGETINFOTIP>(pNMHDR);
	if (pGetInfoTip->iSubItem == 0)
	{
		LVHITTESTINFO hti = {0};
		::GetCursorPos(&hti.pt);
		ScreenToClient(&hti.pt);
		bool bOverMainItem = (SubItemHitTest(&hti) != -1 && hti.iItem == pGetInfoTip->iItem && hti.iSubItem == 0);

		// those tooltips are very nice for debugging/testing but pretty annoying for general usage
		// enable tooltips only if Shift+Ctrl is currently pressed
		bool bShowInfoTip = GetSelectedCount() > 1 || ((GetKeyState(VK_SHIFT) & 0x8000) && (GetKeyState(VK_CONTROL) & 0x8000));

		if (!bShowInfoTip){
			if (!bOverMainItem){
				// don' show the default label tip for the main item, if the mouse is not over the main item
				if ((pGetInfoTip->dwFlags & LVGIT_UNFOLDED) == 0 && pGetInfoTip->cchTextMax > 0 && pGetInfoTip->pszText[0] != '\0')
					pGetInfoTip->pszText[0] = '\0';
			}
			return;
		}

		if (GetSelectedCount() == 1)
		{
		    const CSearchFile* file = (CSearchFile*)GetItemData(pGetInfoTip->iItem);
		    if (file && pGetInfoTip->pszText && pGetInfoTip->cchTextMax > 0){
			    CString strInfo;
			    const CArray<CTag*,CTag*>& tags = file->GetTags();
			    for (int i = 0; i < tags.GetSize(); i++){
				    const CTag* tag = tags[i];
				    if (tag){
					    CString strTag;
					    switch (tag->GetNameID()){
						    case FT_FILENAME:
							    strTag.Format(_T("%s: %s"), GetResString(IDS_SW_NAME), tag->GetStr());
							    break;
						    case FT_FILESIZE:
							    strTag.Format(_T("%s: %s"), GetResString(IDS_DL_SIZE), CastItoXBytes(tag->GetInt(), false, false));
							    break;
						    case FT_FILETYPE:
							    strTag.Format(_T("%s: %s"), GetResString(IDS_TYPE), tag->GetStr());
							    break;
						    case FT_FILEFORMAT:
							    strTag.Format(_T("%s: %s"), GetResString(IDS_SEARCHEXTENTION), tag->GetStr());
							    break;
						    case FT_SOURCES:
							    strTag.Format(_T("%s: %u"), GetResString(IDS_SEARCHAVAIL), tag->GetInt());
							    break;
						    case 0x13: // remote client's upload file priority (tested with Hybrid 0.47)
							    if (tag->GetInt() == 0)
								    strTag = GetResString(IDS_PRIORITY) + _T(": ") + GetResString(IDS_PRIONORMAL);
							    else if (tag->GetInt() == 2)
								    strTag = GetResString(IDS_PRIORITY) + _T(": ") + GetResString(IDS_PRIOHIGH);
							    else if (tag->GetInt() == -2)
								    strTag = GetResString(IDS_PRIORITY) + _T(": ") + GetResString(IDS_PRIOLOW);
						    #ifdef _DEBUG
							    else
								    strTag.Format(_T("%s: %d (***Unknown***)"), GetResString(IDS_PRIORITY), tag->GetInt());
						    #endif
							    break;
						    default:{
							    bool bUnkTag = false;
							    if (tag->GetName()){
								    strTag.Format(_T("%s: "), tag->GetName());
								    strTag = strTag.Left(1).MakeUpper() + strTag.Mid(1);
							    }
							    else{
								    extern CString GetName(const CTag* pTag);
								    CString strTagName = GetName(tag);
								    if (!strTagName.IsEmpty()){
									    strTag.Format(_T("%s: "), strTagName);
								    }
								    else{
								    #ifdef _DEBUG
									    strTag.Format(_T("Unknown tag #%02X: "), tag->GetNameID());
								    #else
									    bUnkTag = true;
								    #endif
								    }
							    }
							    if (!bUnkTag){
								    if (tag->IsStr())
									    strTag += tag->GetStr();
								    else if (tag->IsInt()){
									    if (tag->GetNameID() == FT_MEDIA_LENGTH){
										    CString strTemp;
										    SecToTimeLength(tag->GetInt(), strTemp);
										    strTag += strTemp;
									    }
									    else{
										    TCHAR szBuff[16];
										    _itot(tag->GetInt(), szBuff, 10);
										    strTag += szBuff;
									    }
								    }
								    else if (tag->IsFloat()){
									    TCHAR szBuff[32];
									    _sntprintf(szBuff, ARRSIZE(szBuff), _T("%f"), tag->GetFloat());
									    strTag += szBuff;
								    }
								    else{
								    #ifdef _DEBUG
									    CString strBuff;
									    strBuff.Format(_T("Unknown value type=#%02X"), tag->GetType());
									    strTag += strBuff;
								    #else
									    strTag.Empty();
								    #endif
								    }
							    }
						    }
					    }
					    if (!strTag.IsEmpty()){
						    if (!strInfo.IsEmpty())
							    strInfo += _T("\n");
						    strInfo += strTag;
						    if (strInfo.GetLength() >= pGetInfoTip->cchTextMax)
							    break;
					    }
				    }
			    }
    
    #ifdef USE_DEBUG_DEVICE
			    if (file->GetClientsCount()){
				    CString strSource;
				    if (file->GetClientID() && file->GetClientPort()){
					    uint32 uClientIP = file->GetClientID();
					    uint32 uServerIP = file->GetClientServerIP();
					    strSource.Format(_T("Source: %u.%u.%u.%u:%u  Server: %u.%u.%u.%u:%u"), 
						    (uint8)uClientIP,(uint8)(uClientIP>>8),(uint8)(uClientIP>>16),(uint8)(uClientIP>>24), file->GetClientPort(),
						    (uint8)uServerIP,(uint8)(uServerIP>>8),(uint8)(uServerIP>>16),(uint8)(uServerIP>>24), file->GetClientServerPort());
					    if (!strInfo.IsEmpty())
						    strInfo += _T("\n");
					    strInfo += strSource;
				    }
    
				    const CSimpleArray<CSearchFile::SClient>& aClients = file->GetClients();
				    for (int i = 0; i < aClients.GetSize(); i++){
					    uint32 uClientIP = aClients[i].m_nIP;
					    uint32 uServerIP = aClients[i].m_nServerIP;
					    strSource.Format(_T("Source: %u.%u.%u.%u:%u  Server: %u.%u.%u.%u:%u"), 
						    (uint8)uClientIP,(uint8)(uClientIP>>8),(uint8)(uClientIP>>16),(uint8)(uClientIP>>24), aClients[i].m_nPort,
						    (uint8)uServerIP,(uint8)(uServerIP>>8),(uint8)(uServerIP>>16),(uint8)(uServerIP>>24), aClients[i].m_nServerPort);
					    if (!strInfo.IsEmpty())
						    strInfo += _T("\n");
					    strInfo += strSource;
					    if (strInfo.GetLength() >= pGetInfoTip->cchTextMax)
						    break;
				    }
			    }
    
			    if (file->GetServers().GetSize()){
				    CString strServer;
				    const CSimpleArray<CSearchFile::SServer>& aServers = file->GetServers();
				    for (int i = 0; i < aServers.GetSize(); i++){
					    uint32 uServerIP = aServers[i].m_nIP;
					    strServer.Format(_T("Server: %u.%u.%u.%u:%u  Avail: %u"), 
						    (uint8)uServerIP,(uint8)(uServerIP>>8),(uint8)(uServerIP>>16),(uint8)(uServerIP>>24), aServers[i].m_nPort, aServers[i].m_uAvail);
					    if (!strInfo.IsEmpty())
						    strInfo += _T("\n");
					    strInfo += strServer;
					    if (strInfo.GetLength() >= pGetInfoTip->cchTextMax)
						    break;
				    }
			    }
    #endif
			    _tcsncpy(pGetInfoTip->pszText, strInfo, pGetInfoTip->cchTextMax);
			    pGetInfoTip->pszText[pGetInfoTip->cchTextMax-1] = _T('\0');
		    }
	    }
		else
		{
			int iSelected = 0;
			ULONGLONG ulTotalSize = 0;
			POSITION pos = GetFirstSelectedItemPosition();
			while (pos)
			{
				const CSearchFile* pFile = (CSearchFile*)GetItemData(GetNextSelectedItem(pos));
				if (pFile)
				{
					iSelected++;
					ulTotalSize += pFile->GetFileSize();
				}
			}

			if (iSelected > 0)
			{
				CString strInfo;
				strInfo.Format(_T("%s: %u\r\n%s: %s"), GetResString(IDS_FILES), iSelected, GetResString(IDS_DL_SIZE), CastItoXBytes(ulTotalSize));

				_tcsncpy(pGetInfoTip->pszText, strInfo, pGetInfoTip->cchTextMax);
				pGetInfoTip->pszText[pGetInfoTip->cchTextMax-1] = _T('\0');
			}
		}
	}
	
	*pResult = 0;
}

void CSearchListCtrl::ExpandCollapseItem(int item)
{
	if (item == -1)
		return;

	CSearchFile* searchfile = (CSearchFile*)GetItemData(item);
	if (searchfile->GetListParent() != NULL)
	{
		searchfile = searchfile->GetListParent();
 		LVFINDINFO find;
		find.flags = LVFI_PARAM;
		find.lParam = (LPARAM)searchfile;
		item = FindItem(&find);
		if (item == -1)
			return;
	}
	if (!searchfile)
		return;

	// Check if the source branch is disable
	if (!searchfile->GetListIsExpanded())
	{
		// only expand when more than one child (more than the original entry itself)
		if (searchfile->GetListChildCount() < 2)
			return;

		// Go through the whole list to find out the sources for this file
		SetRedraw(FALSE);
		for (POSITION pos = theApp.searchlist->list.GetHeadPosition(); pos != NULL; ){
			const CSearchFile* cur_file = theApp.searchlist->list.GetNext(pos);
			if(cur_file->GetListParent() == searchfile){
				searchfile->SetListIsExpanded(true);
				InsertItem(LVIF_PARAM|LVIF_TEXT,item+1,cur_file->GetFileName(),0,0,0,(LPARAM)cur_file);
			}
		}
		SetRedraw(TRUE);
	}
	else {
		HideSources(searchfile);
	}

	Update(item);
}

void CSearchListCtrl::HideSources(CSearchFile* toCollapse)
{
	SetRedraw(FALSE);
	int pre,post;
	pre = post = 0;
	for(int i = 0; i < GetItemCount(); i++) {
		const CSearchFile* item = (CSearchFile*)GetItemData(i);
		if(item->GetListParent() == toCollapse) {
			pre++;
			DeleteItem(i--);
			post++;
		}
	}
	if (pre - post == 0)
		toCollapse->SetListIsExpanded(false);
	SetRedraw(TRUE);
}

void CSearchListCtrl::OnClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	POINT point;
	::GetCursorPos(&point);
	CPoint p = point; 
    ScreenToClient(&p);

	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	if (p.x<10) 
		ExpandCollapseItem(pNMIA->iItem);
}

void CSearchListCtrl::OnDblClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	POINT point;
	::GetCursorPos(&point);
	CPoint p = point; 
    ScreenToClient(&p);

	if (p.x > 10){
		if (GetKeyState(VK_MENU) & 0x8000){
			int iSel = GetNextItem(-1, LVIS_SELECTED | LVIS_FOCUSED);
			if (iSel != -1){
				const CSearchFile* file = (CSearchFile*)GetItemData(iSel);
				if (file){
					CSearchResultFileDetailSheet sheet(file);
					sheet.DoModal();
				}
			}
		}
		else
			theApp.emuledlg->searchwnd->DownloadSelected();
	}
}

void CSearchListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (!theApp.emuledlg->IsRunning())
		return;
	if (!lpDrawItemStruct->itemData)
		return;

	CDC* odc = CDC::FromHandle(lpDrawItemStruct->hDC);
	CSearchFile* content = (CSearchFile*)lpDrawItemStruct->itemData;
	BOOL bCtrlFocused = ((GetFocus() == this) || (GetStyle() & LVS_SHOWSELALWAYS));
	if (content->GetListParent()==NULL && (lpDrawItemStruct->itemAction | ODA_SELECT) && (lpDrawItemStruct->itemState & ODS_SELECTED))
	{
		if (bCtrlFocused)
			odc->SetBkColor(m_crHighlight);
		else
			odc->SetBkColor(m_crNoHighlight);
	}
	else
		odc->SetBkColor(GetBkColor());

	CMemDC dc(odc, &lpDrawItemStruct->rcItem);
	CFont *pOldFont = dc->SelectObject(GetFont());
	COLORREF crOldTextColor = dc->SetTextColor(m_crWindowText);

	int iOldBkMode;
	if (m_crWindowTextBk == CLR_NONE)
	{
		DefWindowProc(WM_ERASEBKGND, (WPARAM)(HDC)dc, 0);
		iOldBkMode = dc.SetBkMode(TRANSPARENT);
	}
	else
		iOldBkMode = OPAQUE;

	BOOL notLast = lpDrawItemStruct->itemID + 1 != GetItemCount();
	BOOL notFirst = lpDrawItemStruct->itemID != 0;
	int tree_start = 0;
	int tree_end = 0;

	CRect cur_rec(lpDrawItemStruct->rcItem);

	//offset was 4, now it's the standard 2 spaces
	int iOffset = dc->GetTextExtent(_T(" "), 1 ).cx*2;
	CHeaderCtrl *pHeaderCtrl = GetHeaderCtrl();
	int iCount = pHeaderCtrl->GetItemCount();
	cur_rec.right = cur_rec.left;
	cur_rec.right -= iOffset;
	cur_rec.left += iOffset;

	// icon
	int ofs;
	if (content->GetListParent()!=NULL)
		ofs = 14; // indent child items
	else
		ofs = 6;
	int iImage = theApp.GetFileTypeSystemImageIdx(content->GetFileName());
	ImageList_Draw(theApp.GetSystemImageList(), iImage, dc, cur_rec.left+ofs, cur_rec.top, ILD_NORMAL|ILD_TRANSPARENT);

	// Parent entries
	if (content->GetListParent() == NULL)
	{
		for (int iCurrent = 0; iCurrent < iCount; iCurrent++)
		{
			int iColumn = pHeaderCtrl->OrderToIndex(iCurrent);
			if (!IsColumnHidden(iColumn))
			{
				int cx = CListCtrl::GetColumnWidth(iColumn);
				if (iColumn == 0)
				{
					int iNextLeft = cur_rec.left + cx;
					
					//set up tree vars
					cur_rec.left = cur_rec.right + iOffset;
					cur_rec.right = cur_rec.left + min(8, cx);
					tree_start = cur_rec.left + 1;
					tree_end = cur_rec.right;
					
					//normal column stuff
					cur_rec.left = cur_rec.right + 1;
					cur_rec.right = tree_start + cx - iOffset;
					DrawSourceParent(dc, 0, &cur_rec, content);
					cur_rec.left = iNextLeft;
				}
				else
				{
					cur_rec.right += cx;
					DrawSourceParent(dc, iColumn, &cur_rec, content);
					cur_rec.left += cx;
				}
			}
		}
	}
	else
	{
		for (int iCurrent = 0; iCurrent < iCount; iCurrent++)
		{
			int iColumn = pHeaderCtrl->OrderToIndex(iCurrent);
			if (!IsColumnHidden(iColumn))
			{
				int cx = CListCtrl::GetColumnWidth(iColumn);
				if (iColumn == 0)
				{
					int iNextLeft = cur_rec.left + cx;
					
					//set up tree vars
					cur_rec.left = cur_rec.right + iOffset;
					cur_rec.right = cur_rec.left + min(8, cx);
					tree_start = cur_rec.left + 1;
					tree_end = cur_rec.right;
					
					//normal column stuff
					cur_rec.left = cur_rec.right + 1;
					cur_rec.right = tree_start + cx - iOffset;
					DrawSourceChild(dc, 0, &cur_rec, content);
					cur_rec.left = iNextLeft;
				}
				else
				{
					cur_rec.right += cx;
					DrawSourceChild(dc, iColumn, &cur_rec, content);
					cur_rec.left += cx;
				}
			}
		}
	}

	//draw rectangle around selected item(s)
	if (content->GetListParent()==NULL && (lpDrawItemStruct->itemAction | ODA_SELECT) && (lpDrawItemStruct->itemState & ODS_SELECTED))
	{
		RECT outline_rec = lpDrawItemStruct->rcItem;

		outline_rec.top--;
		outline_rec.bottom++;
		dc->FrameRect(&outline_rec, &CBrush(GetBkColor()));
		outline_rec.top++;
		outline_rec.bottom--;
		outline_rec.left++;
		outline_rec.right--;

		if (notFirst && GetItemState(lpDrawItemStruct->itemID - 1, LVIS_SELECTED))
		{
			const CSearchFile* prev = (CSearchFile*)GetItemData(lpDrawItemStruct->itemID - 1);
			if (prev->GetListParent()==NULL)
				outline_rec.top--;
		} 

		if (notLast && GetItemState(lpDrawItemStruct->itemID + 1, LVIS_SELECTED))
		{
			const CSearchFile* next = (CSearchFile*)GetItemData(lpDrawItemStruct->itemID + 1);
			if (next->GetListParent()==NULL)
				outline_rec.bottom++;
		} 

		if (bCtrlFocused)
			dc->FrameRect(&outline_rec, &CBrush(m_crFocusLine));
		else
			dc->FrameRect(&outline_rec, &CBrush(m_crNoFocusLine));
	}
	//draw focus rectangle around non-highlightable items when they have the focus
	else if (GetFocus() == this && (lpDrawItemStruct->itemState & ODS_FOCUS) == ODS_FOCUS)
	{
		RECT focus_rec;
		focus_rec.top    = lpDrawItemStruct->rcItem.top;
		focus_rec.bottom = lpDrawItemStruct->rcItem.bottom;
		focus_rec.left   = lpDrawItemStruct->rcItem.left + 1;
		focus_rec.right  = lpDrawItemStruct->rcItem.right - 1;
		dc->FrameRect(&focus_rec, &CBrush(m_crNoFocusLine));
	}

	//draw tree last so it draws over selected and focus (looks better)
	if(tree_start < tree_end)
	{
		//set new bounds
		RECT tree_rect;
		tree_rect.top    = lpDrawItemStruct->rcItem.top;
		tree_rect.bottom = lpDrawItemStruct->rcItem.bottom;
		tree_rect.left   = tree_start;
		tree_rect.right  = tree_end;
		dc->SetBoundsRect(&tree_rect, DCB_DISABLE);

		//gather some information
		BOOL hasNext = notLast && ((const CSearchFile*)GetItemData(lpDrawItemStruct->itemID + 1))->GetListParent()!=NULL;
		BOOL isOpenRoot = hasNext && content->GetListParent() == NULL;
		BOOL isChild = content->GetListParent()!= NULL;

		//might as well calculate these now
		int treeCenter = tree_start + 4;
		int middle = (cur_rec.top + cur_rec.bottom + 1) / 2;

		//set up a new pen for drawing the tree
		CPen pn, *oldpn;
		pn.CreatePen(PS_SOLID, 1, RGB(128,128,128)/*m_crWindowText*/);
		oldpn = dc->SelectObject(&pn);

		if(isChild)
		{
			//draw the line to the status bar
			dc->MoveTo(tree_end+10, middle);
			dc->LineTo(tree_start + 4, middle);

			//draw the line to the child node
			if(hasNext)
			{
				dc->MoveTo(treeCenter, middle);
				dc->LineTo(treeCenter, cur_rec.bottom + 1);
			}
		}
		else if (isOpenRoot || (content->GetListParent() == NULL && content->GetListChildCount() > 1))
		{
			//draw box
			RECT circle_rec;
			circle_rec.top    = middle - 5;
			circle_rec.bottom = middle + 4;
			circle_rec.left   = treeCenter - 4;
			circle_rec.right  = treeCenter + 5;
			dc->FrameRect(&circle_rec, &CBrush(RGB(128,128,128)/*m_crWindowText*/));
			CPen penBlack;
			penBlack.CreatePen(PS_SOLID, 1, m_crWindowText);
			CPen* pOldPen2;
			pOldPen2 = dc->SelectObject(&penBlack);
			dc->MoveTo(treeCenter-2,middle - 1);
			dc->LineTo(treeCenter+3,middle - 1);
			
			if (!content->GetListIsExpanded())
			{
				dc->MoveTo(treeCenter,middle-3);
				dc->LineTo(treeCenter,middle+2);
			}
			dc->SelectObject(pOldPen2);
			//draw the line to the child node
			if (hasNext)
			{
				dc->MoveTo(treeCenter, middle + 4);
				dc->LineTo(treeCenter, cur_rec.bottom + 1);
			}
		}

		//draw the line back up to parent node
		if (notFirst && isChild)
		{
			dc->MoveTo(treeCenter, middle);
			dc->LineTo(treeCenter, cur_rec.top - 1);
		}

		//put the old pen back
		dc->SelectObject(oldpn);
		pn.DeleteObject();
	}

	//put the original objects back
	if (m_crWindowTextBk == CLR_NONE)
		dc.SetBkMode(iOldBkMode);
	dc->SelectObject(pOldFont);
	dc->SetTextColor(crOldTextColor);
}

static COLORREF GetSearchItemColor(/*const*/ CSearchFile* src)
{
	// default to black color (may be "corrected" below)
	uint32 red = 0;
	uint32 green = 0;
	uint32 blue = 0;

	const CKnownFile* pFile = theApp.downloadqueue->GetFileByID(src->GetFileHash());
	if (pFile)
	{
		if (pFile->IsPartFile())
			src->SetKnownType(CSearchFile::Downloading);
		else
			src->SetKnownType(CSearchFile::Shared);
		red = 255;
	}
	else if (theApp.sharedfiles->GetFileByID(src->GetFileHash()))
	{
		src->SetKnownType(CSearchFile::Shared);
		red = 255;
	}
	else if (theApp.knownfiles->FindKnownFileByID(src->GetFileHash()))
	{
		src->SetKnownType(CSearchFile::Downloaded);
		green = 128;
	}
	else
	{
		// shade of blue = availability of the file
		blue = src->GetSourceCount() == 0 ? 0 : ((src->GetSourceCount() - 1) * 20);
		if (blue > 255)
			blue = 255;
		else if (blue == 0)
		{
			// avoid the worst case (drawing black text on black background) for users 
			// which are using extreme color schemes by eventually switching to default 
			// windows text color
			COLORREF crText = ::GetSysColor(COLOR_WINDOWTEXT);
			red = GetRValue(crText);
			green = GetGValue(crText);
			blue = GetBValue(crText);
		}
	}

	return RGB(red, green, blue);
}

void CSearchListCtrl::DrawSourceChild(CDC *dc, int nColumn, LPRECT lpRect, /*const*/ CSearchFile* src)
{
	if (!src)
		return;
	if (lpRect->left < lpRect->right)
	{
		CString buffer;
		COLORREF crOldTextColor = dc->SetTextColor(GetSearchItemColor(src));
		switch (nColumn)
		{
			case 0:			// file name
				lpRect->left += 30;
				dc->DrawText(src->GetFileName(), src->GetFileName().GetLength(), lpRect, DLC_DT_TEXT);
				lpRect->left -= 30;
				break;
			case 1:			// file size
				break;
			case 2:			// number of same filenames
				buffer.Format(_T("%u"), src->GetListChildCount());
				dc->DrawText(buffer,buffer.GetLength(), lpRect, DLC_DT_TEXT | DT_RIGHT);
				break;
			case 3:
				break;
			case 4:			// file type
				break;
			case 5:			// file hash
				break;
			case 6:
				buffer = src->GetStrTagValue(FT_MEDIA_ARTIST);
				dc->DrawText(buffer, buffer.GetLength(), lpRect, DLC_DT_TEXT);
				break;
			case 7:
				buffer = src->GetStrTagValue(FT_MEDIA_ALBUM);
				dc->DrawText(buffer, buffer.GetLength(), lpRect, DLC_DT_TEXT);
				break;
			case 8:
				buffer = src->GetStrTagValue(FT_MEDIA_TITLE);
				dc->DrawText(buffer, buffer.GetLength(), lpRect, DLC_DT_TEXT);
				break;
			case 9:{
				uint32 nMediaLength = src->GetIntTagValue(FT_MEDIA_LENGTH);
				if (nMediaLength){
					SecToTimeLength(nMediaLength, buffer);
					dc->DrawText(buffer, buffer.GetLength(), lpRect, DLC_DT_TEXT | DT_RIGHT);
				}
				break;
			}
			case 10:{
				uint32 nBitrate = src->GetIntTagValue(FT_MEDIA_BITRATE);
				if (nBitrate){
					buffer.Format(_T("%u kBit/s"), nBitrate);
					dc->DrawText(buffer, buffer.GetLength(), lpRect, DLC_DT_TEXT | DT_RIGHT);
				}
				break;
			}
			case 11:
				buffer = src->GetStrTagValue(FT_MEDIA_CODEC);
				dc->DrawText(buffer, buffer.GetLength(), lpRect, DLC_DT_TEXT);
				break;
			case 12:		// dir
				if (src->GetDirectory()){
					buffer = src->GetDirectory();
					dc->DrawText(buffer, buffer.GetLength(), lpRect, DLC_DT_TEXT);
				}
				break;
			case 13:
				if (src->m_eKnown == CSearchFile::Shared)
					buffer = GetResString(IDS_SHARED);
				else if (src->m_eKnown == CSearchFile::Downloading)
					buffer = GetResString(IDS_DOWNLOADING);
				else if (src->m_eKnown == CSearchFile::Downloaded)
					buffer = GetResString(IDS_DOWNLOADED);
				else
					buffer.Empty();
				dc->DrawText(buffer, buffer.GetLength(), lpRect, DLC_DT_TEXT);
				break;
		}
		dc->SetTextColor(crOldTextColor);
	}
}

void CSearchListCtrl::DrawSourceParent(CDC *dc, int nColumn, LPRECT lpRect, /*const*/ CSearchFile* src)
{
	if (!src)
		return;

	if (lpRect->left < lpRect->right)
	{
		CString buffer;
		COLORREF crOldTextColor = dc->SetTextColor(GetSearchItemColor(src));
		switch (nColumn)
		{
			case 0:			// file name
				lpRect->left += 22;
				dc->DrawText(src->GetFileName(), src->GetFileName().GetLength(), lpRect, DLC_DT_TEXT);
				lpRect->left -= 22;
				break;
			case 1:			// file size
				buffer = CastItoXBytes(src->GetFileSize(), false, false);
				dc->DrawText(buffer, buffer.GetLength(), lpRect, DLC_DT_TEXT | DT_RIGHT);
				break;
			case 2:{ // avail
				uint32 nSources = src->GetSourceCount();	
				int iClients = src->GetClientsCount();
				if ( thePrefs.IsExtControlsEnabled() && iClients > 0)
					buffer.Format(_T("%u (%u)"), nSources, iClients);
				else
					buffer.Format(_T("%u"), nSources);
				dc->DrawText(buffer, buffer.GetLength(), lpRect, DLC_DT_TEXT | DT_RIGHT);
				break;

			}
			case 3:{		// complete sources
				bool bComplete = false;
				buffer = GetCompleteSourcesDisplayString(src, src->GetSourceCount(), &bComplete);
				COLORREF crOldTextColor;
				if (!bComplete)
					crOldTextColor = dc->SetTextColor(RGB(255, 0, 0));
				dc->DrawText(buffer, buffer.GetLength(), lpRect, DLC_DT_TEXT | DT_RIGHT);
				if (!bComplete)
					dc->SetTextColor(crOldTextColor);
				break;
			}
			case 4:			// file type
				dc->DrawText(src->GetFileTypeDisplayStr(), src->GetFileTypeDisplayStr().GetLength(), lpRect, DLC_DT_TEXT);
				break;
			case 5:			// file hash
				buffer = md4str(src->GetFileHash());
				dc->DrawText(buffer, buffer.GetLength(), lpRect, DLC_DT_TEXT);
				break;
			case 6:
				buffer = src->GetStrTagValue(FT_MEDIA_ARTIST);
				dc->DrawText(buffer, buffer.GetLength(), lpRect, DLC_DT_TEXT);
				break;
			case 7:
				buffer = src->GetStrTagValue(FT_MEDIA_ALBUM);
				dc->DrawText(buffer, buffer.GetLength(), lpRect, DLC_DT_TEXT);
				break;
			case 8:
				buffer = src->GetStrTagValue(FT_MEDIA_TITLE);
				dc->DrawText(buffer, buffer.GetLength(), lpRect, DLC_DT_TEXT);
				break;
			case 9:{
				uint32 nMediaLength = src->GetIntTagValue(FT_MEDIA_LENGTH);
				if (nMediaLength){
					SecToTimeLength(nMediaLength, buffer);
					dc->DrawText(buffer, buffer.GetLength(), lpRect, DLC_DT_TEXT | DT_RIGHT);
				}
				break;
			}
			case 10:{
				uint32 nBitrate = src->GetIntTagValue(FT_MEDIA_BITRATE);
				if (nBitrate){
					buffer.Format(_T("%u kBit/s"), nBitrate);
					dc->DrawText(buffer, buffer.GetLength(), lpRect, DLC_DT_TEXT | DT_RIGHT);
				}
				break;
			}
			case 11:
				buffer = src->GetStrTagValue(FT_MEDIA_CODEC);
				dc->DrawText(buffer, buffer.GetLength(), lpRect, DLC_DT_TEXT);
				break;
			case 12:		// dir
				if (src->GetDirectory()){
					buffer = src->GetDirectory();
					dc->DrawText(buffer, buffer.GetLength(), lpRect, DLC_DT_TEXT);
				}
				break;
			case 13:
				if (src->m_eKnown == CSearchFile::Shared)
					buffer = GetResString(IDS_SHARED);
				else if (src->m_eKnown == CSearchFile::Downloading)
					buffer = GetResString(IDS_DOWNLOADING);
				else if (src->m_eKnown == CSearchFile::Downloaded)
					buffer = GetResString(IDS_DOWNLOADED);
				else
					buffer.Empty();
				dc->DrawText(buffer, buffer.GetLength(), lpRect, DLC_DT_TEXT);
				break;
		}
		dc->SetTextColor(crOldTextColor);
	}
}

void CSearchListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == 'C' && (GetKeyState(VK_CONTROL) & 0x8000))
	{
		// Ctrl+C: Copy listview items to clipboard
		SendMessage(WM_COMMAND, MP_GETED2KLINK);
		return;
	}
	CMuleListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}
