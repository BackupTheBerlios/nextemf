#pragma once

//==> removed 3D-Bar-display [shadow2004]
#ifdef BAR3D
#include "3dpreviewcontrol.h"
#endif
//<== removed 3D-Bar-display [shadow2004]
class CPPgDisplay : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgDisplay)

public:
	CPPgDisplay();
	virtual ~CPPgDisplay();

// Dialog Data
	enum { IDD = IDD_PPG_DISPLAY };

	void Localize(void);

protected:
	enum ESelectFont
	{
		sfServer,
		sfLog
	} m_eSelectFont;
	void LoadSettings(void);

//==> removed 3D-Bar-display [shadow2004]
#ifdef BAR3D
	void DrawPreview();		//Cax2 - aqua bar
	C3DPreviewControl	m_3DPreview;
#endif
//<== removed 3D-Bar-display [shadow2004]
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	static UINT CALLBACK ChooseFontHook(HWND hdlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
//==> removed 3D-Bar-display [shadow2004]
#ifdef BAR3D
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
#endif
//<== removed 3D-Bar-display [shadow2004]
	afx_msg void OnSettingsChange()					{ SetModified(); }
	afx_msg void OnBnClickedSelectHypertextFont();
	afx_msg void OnBtnClickedResetHist();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
//==> removed 3D-Bar-display [shadow2004]
#ifdef BAR3D
	afx_msg void On3DDepth(NMHDR *pNMHDR, LRESULT *pResult);
#endif
//<== removed 3D-Bar-display [shadow2004]

};
