#pragma once

//==> Maella [FAF] -Allow Bandwidth Settings in <1KB Incremements-
#ifdef FAF
#include "NumEdit.h"
#endif
//<== Maella [FAF] -Allow Bandwidth Settings in <1KB Incremements-class CPPgConnection : public CPropertyPage

class CPPgConnection : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPgConnection)

public:
	CPPgConnection();
	virtual ~CPPgConnection();

// Dialog Data
	enum { IDD = IDD_PPG_CONNECTION };

	void Localize(void);
	void LoadSettings(void);

protected:
	bool guardian;
//==> SlotSpeed [shadow2004]
#ifdef SLOT
	CSliderCtrl m_ctlMaxUp;
#endif
//<== SlotSpeed [shadow2004]

//==> Maella [FAF] -Allow Bandwidth Settings in <1KB Incremements-
#ifdef FAF
	CNumEdit m_maxUpload;
	CNumEdit m_maxUploadCapacity; // for Graph, Quick speed selector
	CNumEdit m_maxDownload;
	CNumEdit m_maxDownloadCapacity; // for Graph, Quick speed selector
#else
	CSliderCtrl m_ctlMaxDown;
	CSliderCtrl m_ctlMaxUp;
#endif
//<== Maella [FAF] -Allow Bandwidth Settings in <1KB Incremements-

//==> SlotSpeed [shadow2004]
#ifdef SLOT
	void CalculateMaxUpSlotSpeed();
#endif
//<== SlotSpeed [shadow2004]

	void ShowLimitValues();
//==> Maella [FAF] -Allow Bandwidth Settings in <1KB Incremements-
#ifndef FAF
	void SetRateSliderTicks(CSliderCtrl& rRate);
#endif
//<== Maella [FAF] -Allow Bandwidth Settings in <1KB Incremements-

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSettingsChange()					{ SetModified(); }
	afx_msg void OnEnChangeUDPDisable();
//==> Maella [FAF] -Allow Bandwidth Settings in <1KB Incremements-
#ifndef FAF
	afx_msg void OnLimiterChange();
#endif
//<== Maella [FAF] -Allow Bandwidth Settings in <1KB Incremements-
	afx_msg void OnBnClickedWizard();
	afx_msg void OnBnClickedNetworkKademlia();
	afx_msg void OnHelp();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnBnClickedOpenports();
	afx_msg void OnStartPortTest();
	afx_msg void OnEnChangeTCP();
	afx_msg void OnEnChangeUDP();
	afx_msg void OnEnChangePorts(uint8 istcpport);
//==> SlotSpeed [shadow2004]
#ifdef SLOT
	afx_msg void OnEnKillfocusMaxup();
#endif
//<== SlotSpeed [shadow2004]
};
