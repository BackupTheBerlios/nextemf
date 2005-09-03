#pragma once


// CPPgFiles2-Dialogfeld

class CPPgFiles2 : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgFiles2)

public:
	CPPgFiles2();
	virtual ~CPPgFiles2();
	void Localize(void);

// Dialogfelddaten
	enum { IDD = IDD_PPG_FILES2 };

protected:
	CSliderCtrl		m_ctrlNH;
	uint16			NHash;

	virtual void	DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	virtual BOOL	OnInitDialog();
	void			LoadSettings(void);
	virtual BOOL	OnApply();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSettingsChange()					{ SetModified(); }
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

private:
	enum eTab	{NONE, Files1,Files2};
	CTabCtrl	m_tabCtr;
	eTab		m_currentTab;
	CImageList	m_imageList;
	void		SetTab(eTab tab);
	
public:
	void	InitTab(bool firstinit, int Page = 0);
	afx_msg void OnTcnSelchangeTabFiles2(NMHDR *pNMHDR, LRESULT *pResult);
};
