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
#pragma once
#include "ResizableLib\ResizableDialog.h"
#include "SplitterControl.h"
#include "BtnST.h"
#include "TabCtrl.hpp"
#include "UploadListCtrl.h"
#include "DownloadListCtrl.h"
#include "QueueListCtrl.h"
#include "ClientListCtrl.h"

class CUploadListCtrl;
class CDownloadListCtrl;
class CQueueListCtrl;
class CClientListCtrl;

class CTransferWnd : public CResizableDialog
{
	DECLARE_DYNAMIC(CTransferWnd)

public:
	CTransferWnd(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTransferWnd();

	void ShowQueueCount(uint32 number);
	void UpdateListCount(uint8 listindex, int iCount = -1);
	void Localize();
	void UpdateCatTabTitles(bool force=true);
	void VerifyCatTabSize();
	void SwitchUploadList();

	// Dialog Data
	enum { IDD = IDD_TRANSFER };
	CUploadListCtrl		uploadlistctrl;
	CDownloadListCtrl	downloadlistctrl;
	CQueueListCtrl		queuelistctrl;
	CClientListCtrl		clientlistctrl;
	CToolTipCtrl		m_tooltipCats;

protected:
	CSplitterControl m_wndSplitter;
	uint8 m_uWnd2;
	bool downloadlistactive;
	CButtonST m_uplBtn;
	TabControl m_dlTab;
	int	rightclickindex;
	int m_nDragIndex;
	int m_nDropIndex;
	int m_nLastCatTT;
	int	m_isetcatmenu;
	bool m_bIsDragging;
	CImageList* m_pDragImage;
	HICON icon_download;
	POINT m_pLastMousePoint;

	void ShowWnd2(uint8 uList);
	void SetWnd2(uint8 uWnd2);
	void DoResize(int delta);
	void UpdateSplitterRange();
	void SetInitLayout();
	void DoSplitResize(int delta);
	void SetAllIcons();
	void SetWnd2Icon();
	void UpdateTabToolTips() {UpdateTabToolTips(-1);}
	void UpdateTabToolTips(int tab);
	CString GetTabStatistic(uint8 tab);
	int GetTabUnderMouse(CPoint* point);
	int GetItemUnderMouse(CListCtrl* ctrl);
	CString GetCatTitle(int catid);
	int AddCategorie(CString newtitle,CString newincoming,CString newcomment,CString newautocat,bool addTab=true);
	void EditCatTabLabel(int index,CString newlabel);
	void EditCatTabLabel(int index);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()
	afx_msg void OnHoverUploadList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHoverDownloadList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangeDltab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickDltab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTabMovement(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLvnKeydownDownloadlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSysColorChange();
	afx_msg void OnDblclickDltab();
	afx_msg void OnBnClickedQueueRefreshButton();
};
