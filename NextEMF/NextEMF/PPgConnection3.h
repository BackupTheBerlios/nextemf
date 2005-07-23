//==>WiZaRd/Max AutoHardLimit [cyrex2001]
#pragma once


// CPPgConnection3-Dialogfeld

class CPPgConnection3 : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgConnection3)

public:
	CPPgConnection3();
	virtual ~CPPgConnection3();
	void Localize(void);

// Dialogfelddaten
	enum { IDD = IDD_PPG_CONNECTION3 };

protected:
	CSliderCtrl m_ctlGHLTime;
	CSliderCtrl m_ctlGHLMinSrc;
	uint16      GHLMaxTime;
	uint16      GHLMinSrc;

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
	afx_msg void OnTcnSelchangeTabConnection3(NMHDR *pNMHDR, LRESULT *pResult);
	void	InitTab(bool firstinit, int Page = 0);

	afx_msg void OnBnClickedChkEnablehardlimit();
};
