//==>Reask sourcen after ip chnage or Quickstart or Sivka-Ban [cyrex2001]
#if defined (QUICKSTART) || defined (SIVKA_BAN) || defined (RSAIC_MAELLA) //Reask sourcen after ip chnage or Quickstart or Sivka-Ban

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
	int m_bEnableSivkaBan;
	int m_iSivkaAskTime;
	int m_iSivkaAskCounter;
	int m_bSivkaAskLog;
	int m_bAntiFakeRank;
//==>AntiNickThief [shadow2004]
	int m_bEnableAntiNickThief;
//<==AntiNickThief [shadow2004]
//==>defeat 0-filled partsenders [shadow2004]
#ifdef NULLFILLED
	int m_bEnableZeroFilledTest;
#endif
//<==defeat 0-filled partsenders [shadow2004]
//==>Anti-Leecher [cyrex2001]
#ifdef ANTI_LEECHER
	int m_bEnableALF;
	int m_bEnableAntiCreditHack;
#endif //Anti-Leecher
//<==Anti-Leecher [cyrex2001]
//==> Bold Download-Status [shadow2004]
#ifdef BOLDDL
    int m_bEnableDownloadInBold;
#endif //BOLDDL
//<== Bold Download-Status [shadow2004]
//==>Drop maunal [cyrex2001]
#ifdef DROP
    int iMaxRemoveQRS;
    int m_iDropTimer;
#endif //Drop maunal
//<==Drop maunal [cyrex2001]
//==>timestamp in chatwindow [shadow2004]
	int m_bEnableTimestamp;
//<==timestamp in chatwindow [shadow2004]
//==>Lowid retry by SlugFiller [cyrex2001]
#ifdef LOWID
	int	m_iLowIdRetry;
#endif //Lowid retry
//<==Lowid retry [cyrex2001]
//==> Spooky Mode [cyrex2001]
#ifdef SPOOKY // Spooky Mode [eWombat]
	int m_bEnableSpookyMode;
	int m_bReconSpooky;
	int m_bSpookyFailed;
	int m_iSpookyFailedCount;
#endif // Spooky Mode [eWombat] 
//<== Spooky Mode [cyrex2001]
//==> Spooky Mode ConChecker [cyrex2001]
#ifdef CONCHECKER //>>>WiZaRd: Spooky Mode ConChecker [eWombat] 
	int m_bCheckCon;
    int m_bICMP;
    int m_uiPingTimeOut;
	int m_uiPingTTL;
#endif //<<<WiZaRd: Spooky Mode ConChecker [eWombat] 
//<== Spooky Mode ConChecker [cyrex2001]

	HTREEITEM m_htiSecurity;
	HTREEITEM m_htiIsreaskSourceAfterIPChange;
	HTREEITEM m_htiQuickStart;
	HTREEITEM m_htiQuickStartMaxTime;
	HTREEITEM m_htiQuickStartMaxConnPerFive;
	HTREEITEM m_htiQuickStartMaxConn;
	HTREEITEM m_htiQuickStartAfterIPChange;
	HTREEITEM m_htiSivkaBanGroup;
	HTREEITEM m_htiEnableSivkaBan;
	HTREEITEM m_htiSivkaAskTime;
	HTREEITEM m_htiSivkaAskCounter;
	HTREEITEM m_htiSivkaAskLog;
	HTREEITEM m_htiAntiFakeRank;
//==>Anti-Leecher [cyrex2001]
#ifdef ANTI_LEECHER
	HTREEITEM m_htiEnableALF;
	HTREEITEM m_htiEnableAntiCreditHack;
#endif //Anti-Leecher
//<==Anti-Leecher [cyrex2001]
//==> Bold Download-Status [shadow2004]
#ifdef BOLDDL
    HTREEITEM m_htiEnableDownloadInBold;
#endif //BOLDDL
//<== Bold Download-Status [shadow2004]
//==>AntiNickThief [shadow2004]
	HTREEITEM m_htiEnableAntiNickThief;
//<==AntiNickThief [shadow2004]
//==>defeat 0-filled partsenders [shadow2004]
#ifdef NULLFILLED
	HTREEITEM m_htiEnableZeroFilledTest;
#endif
//<==defeat 0-filled partsenders [shadow2004]
//==>Drop maunal [cyrex2001]
#ifdef DROP
    HTREEITEM m_htiDropSources;
	HTREEITEM m_htiHqrBox;
    HTREEITEM m_htiDropTimer;
#endif //Drop maunal
//<==Drop maunal [cyrex2001]
//==>timestamp in chatwindow [shadow2004]
	HTREEITEM m_htiEnableTimestamp;
//<==timestamp in chatwindow [shadow2004]
//==>Lowid retry by SlugFiller [cyrex2001]
#ifdef LOWID
	HTREEITEM	m_htiLowIdRetry;
#endif //Lowid retry
//<==Lowid retry [cyrex2001]
//==> Spooky Mode [cyrex2001]
#ifdef SPOOKY // Spooky Mode [eWombat]
	HTREEITEM	m_htiSpooky;
	HTREEITEM	m_htiEnableSpookyMode;
	HTREEITEM	m_htiReconSpooky;
	HTREEITEM	m_htiSpookyFailed;
	HTREEITEM	m_htiSpookyFailedCount;
#endif // Spooky Mode [eWombat] 
//<== Spooky Mode [cyrex2001]
//==> Spooky Mode ConChecker [cyrex2001]
#ifdef CONCHECKER //>>>WiZaRd: Spooky Mode ConChecker [eWombat] 
	HTREEITEM	m_htiCheckConGroup;
	HTREEITEM	m_htiCheckCon;
	HTREEITEM	m_htiICMP;
	HTREEITEM	m_htiPingTimeOut;
	HTREEITEM	m_htiPingTTL;
#endif //<<<WiZaRd: Spooky Mode ConChecker [eWombat] 
//<== Spooky Mode ConChecker [cyrex2001]

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
