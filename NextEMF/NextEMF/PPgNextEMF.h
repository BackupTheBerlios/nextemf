
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

	HTREEITEM m_htiSecurity;
//==>WiZaRd AntiLeechClass [cyrex2001]
#ifdef ANTI_LEECH_CLASS
	HTREEITEM m_htiEnableAntiNickThief;
	HTREEITEM m_htiEnableAntiModThief;
	HTREEITEM m_htiClientBanTime;
#endif //WiZaRd AntiLeechClass
//<==WiZaRd AntiLeechClass [cyrex2001]

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