#pragma once
#include "../../ResizableLib/ResizableDialog.h"
#include "IE.h"
#include <atlcoll.h>
// CReleaseWnd dialog

class CReleaseWnd : public CResizableDialog
{
	DECLARE_DYNAMIC(CReleaseWnd)

public:
	CReleaseWnd(CWnd* pParent = NULL);   // standard constructor
	~CReleaseWnd();
	void AddRelease(CString release);

	// Dialog Data
	enum { IDD = IDD_RELEASE };
	//CListCtrl* list;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void UpdateReleaseList(CString strFileName, CString SiteName);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBnGetReleases();
	afx_msg void OnBnClickedBnThisSite();
	afx_msg void OnBnClickedBnDeleteSite();
	afx_msg void OnBnClickedBnAddSite();
	afx_msg void OnBnClickedBnAdvanced(){SetAdvanced(true);};
	afx_msg void OnNMDblclkRelTree(NMHDR *pNMHDR, LRESULT *pResult);

	void Localize();
	void LoadSiteList(void);
	void SaveSiteList(void);
	int	ConnectToSite(CString URL);
	void SetAdvanced(bool clicked);
private:

struct release_data {
	CString site;
	CString category;
	CString title;
	CString description;
	CString link;
};

	CRBMap <HTREEITEM, release_data> rel_list;

	CExplorer m_browser;
};
