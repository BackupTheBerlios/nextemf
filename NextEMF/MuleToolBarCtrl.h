#pragma once

#define IDC_TOOLBAR			16127
#define IDC_TOOLBARBUTTON	16129

#define	TBBTN_CONNECT	(IDC_TOOLBARBUTTON + 0)
#define	TBBTN_KAD		(IDC_TOOLBARBUTTON + 1)
#define	TBBTN_SERVER	(IDC_TOOLBARBUTTON + 2)
#define	TBBTN_TRANSFERS	(IDC_TOOLBARBUTTON + 3)
#define	TBBTN_SEARCH	(IDC_TOOLBARBUTTON + 4)
#define	TBBTN_SHARED	(IDC_TOOLBARBUTTON + 5)
#define	TBBTN_MESSAGES	(IDC_TOOLBARBUTTON + 6)
#define	TBBTN_STATS		(IDC_TOOLBARBUTTON + 7)
#define	TBBTN_OPTIONS	(IDC_TOOLBARBUTTON + 8)
#define	TBBTN_TOOLS		(IDC_TOOLBARBUTTON + 9)
#define	TBBTN_HELP		(IDC_TOOLBARBUTTON + 10)

#define	MULE_TOOLBAR_BAND_NR	0

class CMuleToolbarCtrl : public CToolBarCtrl
{
	DECLARE_DYNAMIC(CMuleToolbarCtrl)

public:
	CMuleToolbarCtrl();
	virtual ~CMuleToolbarCtrl();

	void Init();
	void Localize();
	void SaveCurHeight();
	void PressMuleButton(int nID);

protected:
	CSize		m_sizBtnBmp;
	int			m_iPreviousHeight;
	int			m_iLastPressedButton;
	int			m_buttoncount;
	TBBUTTON	TBButtons[12];
	TCHAR		TBStrings[12][200];
	CBitmap		m_bmpBack;

	void ChangeToolbarBitmap();
	void ChangeTextLabelStyle();
	void UpdateIdealSize();
	void SetAllButtonsStrings();
	void SetAllButtonsWidth();
	void AutoSize();
};
