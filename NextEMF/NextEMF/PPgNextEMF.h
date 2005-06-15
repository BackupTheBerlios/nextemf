
#pragma once
#include "preferences.h"
#include "wizard.h"
#include "TreeOptionsCtrlEx.h"

class CPPgNextEMF : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgNextEMF)

public:
	CPPgNextEMF();
	virtual ~CPPgNextEMF();

// Dialog Data
	enum { IDD = IDD_PPG_NEXTEMF };

protected:
//==>WiZaRd AntiLeechClass [cyrex2001]
#ifdef ANTI_LEECH_CLASS
	bool m_bEnableAntiNickThief;
	bool m_bEnableAntiModThief;
	int	 m_iClientBanTime;
#endif //WiZaRd AntiLeechClass
//<==WiZaRd AntiLeechClass [cyrex2001]

//==>SNAFU [shadow2004]
#ifdef SNAFU
	bool m_bSnafu;
#endif
//<==SNAFU [shadow2004]

//==> Emulate others by WiZaRd & Spike [shadow2004]
#ifdef EMULATE
	bool EmuMLDonkey;
	bool EmueDonkey;
	bool EmueDonkeyHybrid;
	bool EmuShareaza;
#endif
//<== Emulate others by WiZaRd & Spike [shadow2004]

//==> Chunk Selection Patch by Xman [shadow2004]
#ifdef CSP
	int m_iEnableCSP;
#endif
//<== Chunk Selection Patch by Xman [shadow2004]

//==> Linear Prio [shadow2004]
#ifdef LINPRIO
	bool m_bAutoSetResumeOrder;
#endif
//<== Linear Prio [shadow2004]

	HTREEITEM m_htiSecurity;
//==>WiZaRd AntiLeechClass [cyrex2001]
#ifdef ANTI_LEECH_CLASS
	HTREEITEM m_htiEnableAntiNickThief;
	HTREEITEM m_htiEnableAntiModThief;
	HTREEITEM m_htiClientBanTime;
#endif //WiZaRd AntiLeechClass
//<==WiZaRd AntiLeechClass [cyrex2001]

//==>SNAFU [shadow2004]
#ifdef SNAFU
	HTREEITEM m_htiEnableSnafu;
#endif
//<==SNAFU [shadow2004]

//==> Emulate others by WiZaRd & Spike [shadow2004]
#ifdef EMULATE
	HTREEITEM m_htiEmulator;
	HTREEITEM m_htiEnableMLDonkey;
	HTREEITEM m_htiEnableeDonkey;
	HTREEITEM m_htiEnableeDonkeyHybrid;
	HTREEITEM m_htiEnableShareaza;
#endif
//<== Emulate others by WiZaRd & Spike [shadow2004]

//==> Chunk Selection Patch by Xman [shadow2004]
#ifdef CSP
	HTREEITEM m_htiEnableCSP;
	HTREEITEM m_htiEnableCSPNormal;
	HTREEITEM m_htiEnableCSPXman;
#endif
//<== Chunk Selection Patch by Xman [shadow2004]

//==> Linear Prio [shadow2004]
#ifdef LINPRIO
	HTREEITEM m_htiDownload;
	HTREEITEM m_htiAutoSetResumeOrder;
#endif
//<== Linear Prio [shadow2004]

	CTreeOptionsCtrlEx m_ctrlTreeOptions;
	bool m_bInitializedTreeOpts;

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnTreeOptsCtrlNotify(WPARAM wParam, LPARAM lParam);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	void Localize(void);	
	void LoadSettings(void);
	virtual BOOL OnApply();
	virtual BOOL OnInitDialog();
	virtual BOOL OnKillActive();
	afx_msg void OnSettingsChange()			{ SetModified(); }
	afx_msg void OnNMCustomdrawUpSlotsSlider(NMHDR *pNMHDR, LRESULT *pResult);
};
