#if !defined(AFX_CREDITSTHREAD_H__6AF70D43_0818_11D4_8541_A7287FA48D3D__INCLUDED_)
#define AFX_CREDITSTHREAD_H__6AF70D43_0818_11D4_8541_A7287FA48D3D__INCLUDED_

#pragma once
#include "GDIThread.h"

class CCreditsThread : public CGDIThread
{
public:
	DECLARE_DYNAMIC(CCreditsThread)
	CCreditsThread(CWnd* pWnd, HDC hDC, CRect rectScreen);

// Attributes
public:
	CRect		m_rectScreen;
	CRgn		m_rgnScreen;

	int			m_nScrollPos;

	// background bitmap
	CDC			m_dcBk;
	CBitmap		m_bmpBk;
	CBitmap*	m_pbmpOldBk;

	// credits bitmap
	CDC			m_dcCredits;
	CBitmap		m_bmpCredits;
	CBitmap*	m_pbmpOldCredits;

	// screen bitmap
	CDC			m_dcScreen;
	CBitmap		m_bmpScreen;
	CBitmap*	m_pbmpOldScreen;

	// mask bitmap
	CDC			m_dcMask;
	CBitmap		m_bmpMask;
	CBitmap*	m_pbmpOldMask;

	int			m_nCreditsBmpWidth;
	int			m_nCreditsBmpHeight;

	CArray<CString, CString>	m_arCredits;
	CArray<COLORREF, COLORREF>	m_arColors;
	CArray<CFont*, CFont*>		m_arFonts;
	CArray<int, int>			m_arFontHeights;

// Operations
public:
	int CalcCreditsHeight();
	void InitText();
	void InitColors();
	void InitFonts();
	void CreateCredits();
	virtual BOOL InitInstance();
	virtual void SingleStep();
	void PaintBk(CDC* pDC);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreditsThread)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCreditsThread();

	// Generated message map functions
	//{{AFX_MSG(CCreditsThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_CREDITSTHREAD_H__6AF70D43_0818_11D4_8541_A7287FA48D3D__INCLUDED_)
