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
#include "stdafx.h"
#include <io.h>
#include <share.h>
#include "emule.h"
#include "Log.h"
#include "OtherFunctions.h"
#include "Preferences.h"
#include "emuledlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


void LogV(UINT uFlags, LPCTSTR pszFmt, va_list argp)
{
	AddLogTextV(uFlags, DLP_DEFAULT, pszFmt, argp);
}

///////////////////////////////////////////////////////////////////////////////
// 

void Log(LPCTSTR pszFmt, ...)
{
	va_list argp;
	va_start(argp, pszFmt);
	LogV(LOG_DEFAULT, pszFmt, argp);
	va_end(argp);
}

void LogError(LPCTSTR pszFmt, ...)
{
	va_list argp;
	va_start(argp, pszFmt);	
	LogV(LOG_ERROR, pszFmt, argp);
	va_end(argp);
}

void LogWarning(LPCTSTR pszFmt, ...)
{
	va_list argp;
	va_start(argp, pszFmt);	
	LogV(LOG_WARNING, pszFmt, argp);
	va_end(argp);
}

///////////////////////////////////////////////////////////////////////////////
// 

void Log(UINT uFlags, LPCTSTR pszFmt, ...)
{
	va_list argp;
	va_start(argp, pszFmt);
	LogV(uFlags, pszFmt, argp);
	va_end(argp);
}

void LogError(UINT uFlags, LPCTSTR pszFmt, ...)
{
	va_list argp;
	va_start(argp, pszFmt);	
	LogV(uFlags | LOG_ERROR, pszFmt, argp);
	va_end(argp);
}

void LogWarning(UINT uFlags, LPCTSTR pszFmt, ...)
{
	va_list argp;
	va_start(argp, pszFmt);	
	LogV(uFlags | LOG_WARNING, pszFmt, argp);
	va_end(argp);
}

///////////////////////////////////////////////////////////////////////////////
// 

void DebugLog(LPCTSTR pszFmt, ...)
{
	va_list argp;
	va_start(argp, pszFmt);	
	LogV(LOG_DEBUG, pszFmt, argp);
	va_end(argp);
}

void DebugLogError(LPCTSTR pszFmt, ...)
{
	va_list argp;
	va_start(argp, pszFmt);	
	LogV(LOG_DEBUG | LOG_ERROR, pszFmt, argp);
	va_end(argp);
}

void DebugLogWarning(LPCTSTR pszFmt, ...)
{
	va_list argp;
	va_start(argp, pszFmt);	
	LogV(LOG_DEBUG | LOG_WARNING, pszFmt, argp);
	va_end(argp);
}

///////////////////////////////////////////////////////////////////////////////
// 

void DebugLog(UINT uFlags, LPCTSTR pszFmt, ...)
{
	va_list argp;
	va_start(argp, pszFmt);	
	LogV(uFlags | LOG_DEBUG, pszFmt, argp);
	va_end(argp);
}

void DebugLogError(UINT uFlags, LPCTSTR pszFmt, ...)
{
	va_list argp;
	va_start(argp, pszFmt);	
	LogV(uFlags | LOG_DEBUG | LOG_ERROR, pszFmt, argp);
	va_end(argp);
}

void DebugLogWarning(UINT uFlags, LPCTSTR pszFmt, ...)
{
	va_list argp;
	va_start(argp, pszFmt);	
	LogV(uFlags | LOG_DEBUG | LOG_WARNING, pszFmt, argp);
	va_end(argp);
}


///////////////////////////////////////////////////////////////////////////////
// 

void AddLogLine(bool bAddToStatusBar, LPCTSTR pszLine, ...)
{
	ASSERT(pszLine != NULL);

	va_list argptr;
	va_start(argptr, pszLine);
	AddLogTextV(LOG_DEFAULT | (bAddToStatusBar ? LOG_STATUSBAR : 0), DLP_DEFAULT, pszLine, argptr);
	va_end(argptr);
}

void AddDebugLogLine(bool bAddToStatusBar, LPCTSTR pszLine, ...)
{
	ASSERT(pszLine != NULL);

	va_list argptr;
	va_start(argptr, pszLine);
	AddLogTextV(LOG_DEBUG | (bAddToStatusBar ? LOG_STATUSBAR : 0), DLP_DEFAULT, pszLine, argptr);
	va_end(argptr);	
}

void AddDebugLogLine(EDebugLogPriority Priority, bool bAddToStatusBar, LPCTSTR pszLine, ...)
{
	// loglevel needs to be merged with LOG_WARNING and LOG_ERROR later
	// (which only means 3 instead of 5 levels which you can select in the preferences)
	// makes no sense to implement two different priority indicators
	//
	// untill there is some time todo this, It will convert DLP_VERYHIGH to ERRORs
	// and DLP_HIGH to LOG_WARNING in order to be able using the Loglevel and color indicator
	ASSERT(pszLine != NULL);

	va_list argptr;
	va_start(argptr, pszLine);
	uint32 nFlag = 0;
	if (Priority == DLP_VERYHIGH)
		nFlag = LOG_ERROR;
	else if (Priority == DLP_HIGH)
		nFlag = LOG_WARNING;

	AddLogTextV(LOG_DEBUG | nFlag | (bAddToStatusBar ? LOG_STATUSBAR : 0), Priority, pszLine, argptr);
	va_end(argptr);	
}

void AddLogTextV(UINT uFlags, EDebugLogPriority dlpPriority, LPCTSTR pszLine, va_list argptr)
{
	ASSERT(pszLine != NULL);

	if ((uFlags & LOG_DEBUG) && !(thePrefs.GetVerbose() && dlpPriority >= thePrefs.GetVerboseLogPriority()))
		return;	

	TCHAR szLogLine[1000];
	if (_vsntprintf(szLogLine, ARRSIZE(szLogLine), pszLine, argptr) == -1)
		szLogLine[ARRSIZE(szLogLine) - 1] = _T('\0');
	
	if (theApp.emuledlg)
		theApp.emuledlg->AddLogText(uFlags, szLogLine);
#ifdef _DEBUG
	else{
		TRACE(_T("App Log: %s\n"), szLogLine);
	}
#endif
}


///////////////////////////////////////////////////////////////////////////////
// CLogFile

CLogFile::CLogFile()
{
	(void)m_strFilePath;
	m_uMaxFileSize = (UINT)-1;
	m_uBytesWritten = 0;
	m_tStarted = 0;
	m_fp = NULL;
	m_bInOpenCall = false;
}

CLogFile::~CLogFile()
{
	Close();
}

const CString& CLogFile::GetFilePath() const
{
	return m_strFilePath;
}

bool CLogFile::SetFilePath(LPCTSTR pszFilePath)
{
	if (IsOpen())
		return false;
	m_strFilePath = pszFilePath;
	return true;
}

void CLogFile::SetMaxFileSize(UINT uMaxFileSize)
{
	if (uMaxFileSize == 0)
		uMaxFileSize = (UINT)-1;
	else if (uMaxFileSize < 0x10000)
		uMaxFileSize = 0x10000;
	m_uMaxFileSize = uMaxFileSize;
}

bool CLogFile::IsOpen() const
{
	return m_fp != NULL;
}

bool CLogFile::Create(LPCTSTR pszFilePath, UINT uMaxFileSize)
{
	Close();
	m_strFilePath = pszFilePath;
	m_uMaxFileSize = uMaxFileSize;
	return Open();
}

bool CLogFile::Open()
{
	if (m_fp != NULL)
		return true;

	m_fp = _tfsopen(m_strFilePath, _T("a+b"), _SH_DENYWR);
	if (m_fp != NULL)
	{
		m_tStarted = time(NULL);
		m_uBytesWritten = _filelength(fileno(m_fp));
#ifdef _UNICODE
		if (m_uBytesWritten == 0)
		{
			// write Unicode byte-order mark 0xFEFF
			fputwc(0xFEFF, m_fp);
		}
		else if (m_uBytesWritten >= sizeof(WORD))
		{
			// check for Unicode byte-order mark 0xFEFF
			WORD wBOM;
			if (fread(&wBOM, sizeof(wBOM), 1, m_fp) == 1)
			{
				if (wBOM == 0xFEFF)
				{
					// log file already in Unicode format
					fseek(m_fp, 0, SEEK_END); // actually not needed because file is opened in 'Append' mode..
				}
				else
				{
					ASSERT( !m_bInOpenCall );
					if (!m_bInOpenCall) // just for safety
					{
						m_bInOpenCall = true;
						StartNewLogFile();
						m_bInOpenCall = false;
					}
				}
			}
		}
#endif
	}
	return m_fp != NULL;
}

bool CLogFile::Close()
{
	if (m_fp == NULL)
		return true;
	bool bResult = (fclose(m_fp) == 0);
	m_fp = NULL;
	m_tStarted = 0;
	m_uBytesWritten = 0;
	return bResult;
}

bool CLogFile::Log(LPCTSTR pszMsg, int iLen)
{
	if (m_fp == NULL)
		return false;

	// don't use 'fputs' + '_filelength' -- gives poor performance
	size_t uToWrite = ((iLen == -1) ? _tcslen(pszMsg) : (size_t)iLen)*sizeof(TCHAR);
	size_t uWritten = fwrite(pszMsg, 1, uToWrite, m_fp);
	bool bResult = !ferror(m_fp);
	m_uBytesWritten += uWritten;

	if (m_uBytesWritten >= m_uMaxFileSize)
		StartNewLogFile();
	else
		fflush(m_fp);

	return bResult;
}

void CLogFile::StartNewLogFile()
{
	time_t tStarted = m_tStarted;
	Close();

	TCHAR szDateLogStarted[40];
	_tcsftime(szDateLogStarted, ARRSIZE(szDateLogStarted), _T("%Y.%m.%d %H.%M.%S"), localtime(&tStarted));

	TCHAR szDrv[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szNam[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];
	_tsplitpath(m_strFilePath, szDrv, szDir, szNam, szExt);

	CString strLogBakNam;
	strLogBakNam = szNam;
	strLogBakNam += _T(" - ");
	strLogBakNam += szDateLogStarted;

	TCHAR szLogBakFilePath[MAX_PATH];
	_tmakepath(szLogBakFilePath, szDrv, szDir, strLogBakNam, szExt);

	if (_trename(m_strFilePath, szLogBakFilePath) != 0)
		_tremove(m_strFilePath);

	Open();
}
