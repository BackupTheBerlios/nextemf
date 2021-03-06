//this file is part of eMule
//Copyright (C)2002 Merkur ( devs@emule-project.net / http://www.emule-project.net )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#pragma once
//==> XPMenu [shadow2004]
#ifdef XPMEN
#include "NextEMF/MenuXP.h"
#endif
//<== XPMenu [shadow2004]

class CPartFile;

///////////////////////////////////////////////////////////////////////////////
// CPreviewThread

class CPreviewThread : public CWinThread
{
	DECLARE_DYNCREATE(CPreviewThread)

public:
	virtual	BOOL	InitInstance();
	virtual int		Run();
	void	SetValues(CPartFile* pPartFile,CString player);

protected:
	CPreviewThread();			// protected constructor used by dynamic creation
	virtual ~CPreviewThread();

	CPartFile*  m_pPartfile;
	CString		m_player;

	DECLARE_MESSAGE_MAP()
};


///////////////////////////////////////////////////////////////////////////////
// CPreviewApps

class CPreviewApps
{
public:
	CPreviewApps();

	CString GetDefaultAppsFile() const;
	int ReadAllApps();
	void RemoveAllApps();

//==> XPMenu [shadow2004]
//#ifdef XPMEN
//	int GetAllMenuEntries(CMenuXP* rMenu, const CPartFile* file);
//#else
	int GetAllMenuEntries(CMenu& rMenu, const CPartFile* file);
//#endif
//<== XPMenu [shadow2004]
	void RunApp(CPartFile* file, UINT uMenuID);

	enum ECanPreviewRes{
		NotHandled,
		No,
		Yes
	};
	ECanPreviewRes CanPreview(const CPartFile* file);
	int GetPreviewApp(const CPartFile* file);
	bool Preview(CPartFile* file);

protected:
	struct SPreviewApp
	{
		SPreviewApp& operator=(const SPreviewApp& rCopy)
		{
			strTitle = rCopy.strTitle;
			strCommand = rCopy.strCommand;
			strCommandArgs = rCopy.strCommandArgs;
			astrExtensions.Copy(rCopy.astrExtensions);
			uMinStartOfFile = rCopy.uMinStartOfFile;
			uMinCompletedSize = rCopy.uMinCompletedSize;
			return *this;
		}

		CString strTitle;
		CString strCommand;
		CString strCommandArgs;
		CStringArray astrExtensions;
		UINT uMinStartOfFile;
		UINT uMinCompletedSize;
	};
	CArray<SPreviewApp> m_aApps;
	time_t m_tDefAppsFileLastModified;
	CPartFile* m_pLastCheckedPartFile;
	SPreviewApp* m_pLastPartFileApp;

	void UpdateApps();

};

extern CPreviewApps thePreviewApps;
