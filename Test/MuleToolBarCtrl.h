#pragma once

#define IDC_TOOLBAR			16127
#define IDC_TOOLBARBUTTON	16129

class CMuleToolbarCtrl : public CToolBarCtrl
{
	DECLARE_DYNAMIC(CMuleToolbarCtrl)

public:
	CMuleToolbarCtrl();
	virtual ~CMuleToolbarCtrl();

	void Init(void);
	void Localize(void);
	void ReloadConfig();

	// Customization might splits up the button-group, so we have to (un-)press them on our own
	void PressMuleButton(int nID)
	{
		if(m_iLastPressedButton != -1)
			CheckButton(m_iLastPressedButton, FALSE);
		CheckButton(nID, TRUE);
		m_iLastPressedButton = nID;
	}

protected:
	int			m_iLastPressedButton;
	int			m_buttoncount;
//==> remove IRC [shadow2004]
#if defined(IRC)
//==> RSS-Window [shadow2004]
#ifdef RELWND
	TBBUTTON	TBButtons[13];
	TCHAR		TBStrings[13][200];
#else
	TBBUTTON	TBButtons[12];
	TCHAR		TBStrings[12][200];
#endif
//<== RSS-Window [shadow2004]
#else //IRC
//==> RSS-Window [shadow2004]
#ifdef RELWND
	TBBUTTON	TBButtons[12];
	TCHAR		TBStrings[12][200];
#else
	TBBUTTON	TBButtons[11];
	TCHAR		TBStrings[11][200];
#endif
//<== RSS-Window [shadow2004]
#endif //IRC
//<== remove IRC [shadow2004]
	CStringArray bitmappaths;
	int			m_iToolbarLabelSettings;
	CStringArray aSkinPaths;

	virtual		BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSysColorChange();
	afx_msg void OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTbnQueryDelete(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTbnQueryInsert(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTbnGetButtonInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTbnToolbarChange(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTbnReset(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTbnInitCustomize(NMHDR *pNMHDR, LRESULT *pResult);

	void SetBtnWidth();

	// [FoRcHa]
	void ChangeToolbarBitmap(CString path, bool refresh);
	void ChangeTextLabelStyle(int settings, bool refresh);
	void Refresh();
};
