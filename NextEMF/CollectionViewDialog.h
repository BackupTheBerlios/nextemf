#pragma once
#include "afxcmn.h"
#include "CollectionListCtrl.h"
#include "afxwin.h"

// CCollectionViewDialog dialog

class CCollection;

class CCollectionViewDialog : public CDialog
{
	DECLARE_DYNAMIC(CCollectionViewDialog)

public:
	CCollectionViewDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCollectionViewDialog();

// Dialog Data
	enum { IDD = IDD_COLLECTIONVIEWDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CCollection* m_pCollection;
public:
	void SetCollection(CCollection* pCollection);
	virtual BOOL OnInitDialog(void);
private:
	CCollectionListCtrl m_CollectionViewList;
public:
	afx_msg void OnNMDblclkCollectionvewlist(NMHDR *pNMHDR, LRESULT *pResult);
	void DownloadSelected(void);
	afx_msg void OnBnClickedViewcollectiondl();
	afx_msg void OnBnClickedOk();
	CButton m_AddNewCatagory;
	CStatic m_CollectionViewListLabel;
	CStatic m_CollectionViewListIcon;
	CButton m_CollectionDownload;
	CButton m_CollectionExit;
	CEdit m_CollectionViewAuthor;
	CEdit m_CollectionViewAuthorKey;
};
