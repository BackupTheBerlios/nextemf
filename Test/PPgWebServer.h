#pragma once
#include "HypertextCtrl.h"
#include "loggable.h"

class CPPgWebServer : public CPropertyPage, public CLoggable
{
	DECLARE_DYNAMIC(CPPgWebServer)

public:
	CPPgWebServer();
	virtual ~CPPgWebServer();

	enum { IDD = IDD_PPG_WEBSRV };

	void Localize(void);

protected:
	bool m_bModified;
	bool bCreated;
//==> remove MobileMule [shadow2004]
#if defined(MM)
	CHyperTextCtrl m_wndMobileLink;
#endif //MM
//<== remove MobileMule [shadow2004]

	void LoadSettings(void);

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void SetModified(BOOL bChanged = TRUE){
		m_bModified = bChanged;
		CPropertyPage::SetModified(bChanged);
	}

	DECLARE_MESSAGE_MAP()
	afx_msg void OnDataChange()		{SetModified();}
	afx_msg void OnEnChangeWSEnabled();
//==> remove MobileMule [shadow2004]
#if defined(MM)
	afx_msg void OnEnChangeMMEnabled();
#endif //MM
//<== remove MobileMule [shadow2004]
	afx_msg void OnReloadTemplates();
	afx_msg void OnBnClickedTmplbrowse();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
};
