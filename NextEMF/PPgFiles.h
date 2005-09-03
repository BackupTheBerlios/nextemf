#pragma once

class CPPgFiles : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgFiles)

public:
	CPPgFiles();
	virtual ~CPPgFiles();

// Dialog Data
	enum { IDD = IDD_PPG_FILES };

	void Localize(void);

protected:
	CListBox m_uncfolders;

	void LoadSettings(void);
	void OnSettingsChangeCat(uint8 index);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	afx_msg void OnSetCleanupFilter();
	afx_msg void BrowseVideoplayer();
	afx_msg void OnSettingsChange();
	afx_msg void OnSettingsChangeCat1() {OnSettingsChangeCat(1);}
	afx_msg void OnSettingsChangeCat2()	{OnSettingsChangeCat(2);}
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);

//==> PPgTabControl [shadow2004]
#ifdef PPGCTRL
private:
	enum eTab	{NONE, Files1,Files2};
	CTabCtrl	m_tabCtr;
	eTab		m_currentTab;
	CImageList	m_imageList;
	void		SetTab(eTab tab);

public:
	afx_msg void OnTcnSelchangeTabFiles1(NMHDR *pNMHDR, LRESULT *pResult);
	void	InitTab(bool firstinit, int Page = 0);
#endif
//<== PPgTabControl [shadow2004]
};
