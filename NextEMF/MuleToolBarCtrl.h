#pragma once

#define IDC_TOOLBAR			16127
#define IDC_TOOLBARBUTTON	16129

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
