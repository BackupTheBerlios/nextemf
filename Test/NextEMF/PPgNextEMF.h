//==>Reask sourcen after ip chnage or Quickstart or Sivka-Ban [cyrex2001]
#if defined (RSAIC_SIVKA) || defined (QUICKSTART) || defined (SIVKA_BAN) //Reask sourcen after ip chnage or Quickstart or Sivka-Ban

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
	int m_bIsreaskSourceAfterIPChange;
	int m_bQuickStart;
	int m_iQuickStartMaxTime;
	int m_iQuickStartMaxConnPerFive;
	int m_iQuickStartMaxConn;
	int m_bQuickStartAfterIPChange;
	int m_iSivkaAskTime;
	int m_iSivkaAskCounter;
	int m_bSivkaAskLog;
	int m_bAntiFakeRank;
//==>Anti-Leecher [cyrex2001]
#ifdef ANTI_LEECHER
	int m_bEnableAntiLeecher;
	int m_bEnableAntiBadComunity;
	int m_bEnableAntiGplBreaker;
	int m_bEnableAntiCreditHack;
#endif //Anti-Leecher
//<==Anti-Leecher [cyrex2001]

	HTREEITEM m_htiSecurity;
	HTREEITEM m_htiIsreaskSourceAfterIPChange;
	HTREEITEM m_htiQuickStart;
	HTREEITEM m_htiQuickStartMaxTime;
	HTREEITEM m_htiQuickStartMaxConnPerFive;
	HTREEITEM m_htiQuickStartMaxConn;
	HTREEITEM m_htiQuickStartAfterIPChange;
	HTREEITEM m_htiSivkaAskTime;
	HTREEITEM m_htiSivkaAskCounter;
	HTREEITEM m_htiSivkaAskLog;
	HTREEITEM m_htiAntiFakeRank;
//==>Anti-Leecher [cyrex2001]
#ifdef ANTI_LEECHER
	HTREEITEM m_htiEnableAntiLeecher;
	HTREEITEM m_htiEnableAntiBadComunity;
	HTREEITEM m_htiEnableAntiGplBreaker;
	HTREEITEM m_htiEnableAntiCreditHack;
#endif //Anti-Leecher
//<==Anti-Leecher [cyrex2001]

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
#endif //Reask sourcen after ip chnage or Quickstart or Sivka-Ban
//<==Reask sourcen after ip chnage or Quickstart or Sivka-Ban [cyrex2001]