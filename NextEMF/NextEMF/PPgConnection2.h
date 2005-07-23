//==>Reask sourcen after ip change [cyrex2001]
//==>Quickstart [cyrex2001]
#pragma once


// CPPgConnection2-Dialogfeld

class CPPgConnection2 : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgConnection2)

public:
	CPPgConnection2();
	virtual ~CPPgConnection2();
	void Localize(void);

// Dialogfelddaten
	enum { IDD = IDD_PPG_CONNECTION2 };

protected:
	CSliderCtrl m_ctlQSTime;
	uint16      QuickStartMaxTime;

	virtual void	DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual BOOL	OnInitDialog();
	void			LoadSettings(void);
	virtual BOOL	OnApply();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSettingsChange()					{ SetModified(); }
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

private:
	enum eTab	{NONE, Connection1,Connection2,Connection3};
	CTabCtrl	m_tabCtr;
	eTab		m_currentTab;
	CImageList	m_imageList;
	void		SetTab(eTab tab);
	
public:
	afx_msg void OnTcnSelchangeTabConnection2(NMHDR *pNMHDR, LRESULT *pResult);
	void	InitTab(bool firstinit, int Page = 0);

	afx_msg void OnBnClickedChkEnablequickstart();
};
