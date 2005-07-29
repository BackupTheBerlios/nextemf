//==>Reask sourcen after ip change [cyrex2001]
//==>Quickstart [cyrex2001]
#pragma once


// CPPgSecurity2-Dialogfeld

class CPPgSecurity2 : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgSecurity2)

public:
	CPPgSecurity2();
	virtual ~CPPgSecurity2();
	void Localize(void);

// Dialogfelddaten
	enum { IDD = IDD_PPG_SECURITY2 };

protected:

	virtual void	DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual BOOL	OnInitDialog();
	void			LoadSettings(void);
	virtual BOOL	OnApply();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSettingsChange()					{ SetModified(); }

private:
	enum eTab	{NONE, Security1,Security2};
	CTabCtrl	m_tabCtr;
	eTab		m_currentTab;
	CImageList	m_imageList;
	void		SetTab(eTab tab);
	
public:
	void	InitTab(bool firstinit, int Page = 0);
	afx_msg void OnTcnSelchangeTabSecurity2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedChkEnableAutoupdate();
	afx_msg void OnReloadIPFilter();
	afx_msg void OnEditIPFilter();
	afx_msg void OnResetIPFilterURL();
	afx_msg void OnLoadIPFilter();
};
