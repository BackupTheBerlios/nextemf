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
#include <sys/stat.h>
#include <share.h>
#include <io.h>
#include <atlrx.h>
#include "emule.h"
#include "OtherFunctions.h"
#include "DownloadQueue.h"
#include "Preferences.h"
#include "PartFile.h"
#include "SharedFileList.h"
#include "UpDownClient.h"
#include "Opcodes.h"
#include <shlobj.h>
#include "emuledlg.h"
#include "MenuCmds.h"
#include "ZipFile.h"
#include <atlbase.h>
#include "StringConversion.h"
#include "shahashset.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Base chars for encode an decode functions
static byte base16Chars[17] = "0123456789ABCDEF";
static byte base32Chars[33] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
#define BASE16_LOOKUP_MAX 23
static byte base16Lookup[BASE16_LOOKUP_MAX][2] = {
    { '0', 0x0 },
    { '1', 0x1 },
    { '2', 0x2 },
    { '3', 0x3 },
    { '4', 0x4 },
    { '5', 0x5 },
    { '6', 0x6 },
    { '7', 0x7 },
    { '8', 0x8 },
    { '9', 0x9 },
	{ ':', 0x9 },
    { ';', 0x9 },
    { '<', 0x9 },
    { '=', 0x9 },
    { '>', 0x9 },
    { '?', 0x9 },
    { '@', 0x9 },
    { 'A', 0xA },
    { 'B', 0xB },
    { 'C', 0xC },
    { 'D', 0xD },
    { 'E', 0xE },
    { 'F', 0xF }
};

CString CastItoXBytes(uint16 count, bool isK, bool isPerSec, uint32 decimal){
	return CastItoXBytes((double)count, isK, isPerSec, decimal);
}

CString CastItoXBytes(uint32 count, bool isK, bool isPerSec, uint32 decimal){
	return CastItoXBytes((double)count, isK, isPerSec, decimal);
}

CString CastItoXBytes(uint64 count, bool isK, bool isPerSec, uint32 decimal){
	return CastItoXBytes((double)count, isK, isPerSec, decimal);
}

CString CastItoXBytes(float count, bool isK, bool isPerSec, uint32 decimal){
	return CastItoXBytes((double)count, isK, isPerSec, decimal);
}

CString CastItoXBytes(double count, bool isK, bool isPerSec, uint32 decimal){
	if( count <= 0.0 )
	{
		if(isPerSec)
			return _T("0 ") + GetResString(IDS_BYTESPERSEC);
		else
			return _T("0 ") + GetResString(IDS_BYTES);
	}
	else if( isK )
	{
		if( count >  1.7E+300 )
			count =  1.7E+300;
		else
			count *= 1024.0;
	}
	CString buffer;
	if( isPerSec )
	{
		if (count < 1024.0)
			buffer.Format(_T("%.0f %s"), count, GetResString(IDS_BYTESPERSEC));
		else if (count < 1024000.0)
			buffer.Format(_T("%.*f %s"), decimal, count/1024.0, GetResString(IDS_KBYTESPERSEC));
		else if (count < 1048576000.0)
			buffer.Format(_T("%.*f %s"), decimal, count/1048576.0, GetResString(IDS_MBYTESPERSEC));
		else if (count < 1073741824000.0)
			buffer.Format(_T("%.*f %s"), decimal, count/1073741824.0, GetResString(IDS_GBYTESPERSEC));
		else 
			buffer.Format(_T("%.*f %s"), decimal, count/1099511627776.0, GetResString(IDS_TBYTESPERSEC));
	}
	else
	{
		if (count < 1024.0)
			buffer.Format(_T("%.0f %s"), count, GetResString(IDS_BYTES));
		else if (count < 1024000.0)
			buffer.Format(_T("%.*f %s"), decimal, count/1024.0, GetResString(IDS_KBYTES));
		else if (count < 1048576000.0)
			buffer.Format(_T("%.*f %s"), decimal, count/1048576.0, GetResString(IDS_MBYTES));
		else if (count < 1073741824000.0)
			buffer.Format(_T("%.*f %s"), decimal, count/1073741824.0, GetResString(IDS_GBYTES));
		else 
			buffer.Format(_T("%.*f %s"), decimal, count/1099511627776.0, GetResString(IDS_TBYTES));
	}
	return buffer;
}

CString CastItoIShort(uint16 count, bool isK, uint32 decimal){
	return CastItoIShort((double)count, isK, decimal);
}

CString CastItoIShort(uint32 count, bool isK, uint32 decimal){
	return CastItoIShort((double)count, isK, decimal);
}

CString CastItoIShort(uint64 count, bool isK, uint32 decimal){
	return CastItoIShort((double)count, isK, decimal);
}

CString CastItoIShort(float count, bool isK, uint32 decimal){
	return CastItoIShort((double)count, isK, decimal);
}

CString CastItoIShort(double count, bool isK, uint32 decimal){
	if( count <= 0.0 )
	{
		return _T("0");
	}
	else if( isK )
	{
		if( count >  1.7E+300 )
			count =  1.7E+300;
		else
			count *= 1000.0;
	}
	CString output;
	if (count < 1000.0)
		output.Format(_T("%.0f"), count);
	else if (count < 1000000.0)
		output.Format(_T("%.*f%s"), decimal, count/1000.0, GetResString(IDS_KILO));
	else if (count < 1000000000.0)
		output.Format(_T("%.*f%s"), decimal, count/1000000.0, GetResString(IDS_MEGA));
	else if (count < 1000000000000.0)
		output.Format(_T("%.*f%s"), decimal, count/1000000000.0, GetResString(IDS_GIGA));
	else if (count < 1000000000000000.0)
		output.Format(_T("%.*f%s"), decimal, count/1000000000000.0, GetResString(IDS_TERRA));
	return output;
}

CString CastSecondsToHM(time_t tSeconds)
{
	if (tSeconds == -1)	// invalid or unknown time value
		return _T("?");

	CString buffer;
	UINT count = tSeconds;
	if (count < 60)
		buffer.Format(_T("%i %s"), count, GetResString(IDS_SECS));
	else if (count < 3600)
		buffer.Format(_T("%i:%s %s"), count/60, LeadingZero(count - (count/60)*60), GetResString(IDS_MINS));
	else if (count < 86400)
		buffer.Format(_T("%i:%s %s"), count/3600, LeadingZero((count - (count/3600)*3600)/60), GetResString(IDS_HOURS));
	else
		buffer.Format(_T("%i %s %i %s"), count/86400, GetResString(IDS_DAYS), (count - (count/86400)*86400)/3600, GetResString(IDS_HOURS));
	return buffer;
}

CString CastSecondsToLngHM(LONGLONG llSeconds)
{
	if (llSeconds == -1) // invalid or unknown time value
		return _T("?");

	CString buffer;
	ULONGLONG count = llSeconds;
	if (count < 60)
		buffer.Format(_T("%I64d %s"), count, GetResString(IDS_LONGSECS));
	else if (count < 3600)
		buffer.Format(_T("%I64d:%s %s"), count/60, LeadingZero((UINT)(count - (count/60)*60)), GetResString(IDS_LONGMINS));
	else if (count < 86400)
		buffer.Format(_T("%I64d:%s %s"), count/3600, LeadingZero((UINT)((count - (count/3600)*3600)/60)), GetResString(IDS_LONGHRS));
	else {
		ULONGLONG cntDays = count/86400;
		ULONGLONG cntHrs = (count - (count/86400)*86400)/3600;
		if (cntHrs)
			buffer.Format(_T("%I64d %s %I64d:%s %s"), cntDays, GetResString(IDS_DAYS2), cntHrs, LeadingZero((uint32)(count - (cntDays*86400) - (cntHrs*3600))/60), GetResString(IDS_LONGHRS));
		else
			buffer.Format(_T("%I64d %s %u %s"), cntDays, GetResString(IDS_DAYS2), (uint32)(count - (cntDays*86400) - (cntHrs*3600))/60, GetResString(IDS_LONGMINS));
	}
	return buffer;
} 

CString LeadingZero(uint32 units)
{
	CString temp;
	if (units < 10)
		temp.Format(_T("0%i"), units);
	else
		temp.Format(_T("%i"), units);
	return temp;
}

//<<--9/21/02
void ShellOpenFile(CString name){ 
    ShellExecute(NULL, _T("open"), name, NULL, NULL, SW_SHOW); 
} 
void ShellOpenFile(CString name, LPCTSTR pszVerb){ 
    ShellExecute(NULL, pszVerb, name, NULL, NULL, SW_SHOW); 
} 

namespace {
	bool IsHexDigit(int c) {
		switch (c) {
		case '0': return true;
		case '1': return true;
		case '2': return true;
		case '3': return true;
		case '4': return true;
		case '5': return true;
		case '6': return true;
		case '7': return true;
		case '8': return true;
		case '9': return true;
		case 'A': return true;
		case 'B': return true;
		case 'C': return true;
		case 'D': return true;
		case 'E': return true;
		case 'F': return true;
		case 'a': return true;
		case 'b': return true;
		case 'c': return true;
		case 'd': return true;
		case 'e': return true;
		case 'f': return true;
		default: return false;
		}
	}
}

CString URLDecode(const CString& inStr)
{
	// decode escape sequences
	CString res;
	for (int x = 0; x < inStr.GetLength(); x++)
	{
		if (inStr.GetAt(x) == _T('%') && x + 2 < inStr.GetLength() && IsHexDigit(inStr.GetAt(x+1)) && IsHexDigit(inStr.GetAt(x+2)))
		{
			TCHAR hexstr[3];
			_tcsncpy(hexstr, inStr.Mid(x+1, 2), 2);
			hexstr[2] = _T('\0');
			x += 2;

			// Convert the hex to ASCII
			res.AppendChar((TCHAR)_tcstoul(hexstr, NULL, 16));
		}
		else
		{
			res.AppendChar(inStr.GetAt(x));
		}
	}
	return res;
}

CString URLEncode(const CString& sInT)
{
	CStringA sIn(sInT);
    LPCSTR pInBuf = sIn;

    CString sOut;
    LPTSTR pOutBuf = sOut.GetBuffer(sIn.GetLength() * 3);
    if(pOutBuf)
    {
		// do encoding
		while (*pInBuf)
		{
			if (_istalnum((_TUCHAR)*pInBuf))
				*pOutBuf++ = (BYTE)*pInBuf;
			else if (_istspace((_TUCHAR)*pInBuf))
				*pOutBuf++ = _T('+');
			else
			{
				*pOutBuf++ = _T('%');
				*pOutBuf++ = toHex((BYTE)*pInBuf >> 4);
				*pOutBuf++ = toHex((BYTE)*pInBuf % 16);
			}
			pInBuf++;
		}
		*pOutBuf = _T('\0');
		sOut.ReleaseBuffer();
    }
    return sOut;
}

CString MakeStringEscaped(CString in)
{
	in.Replace(_T("&"), _T("&&"));
	return in;
}

CString RemoveAmbersand(const CString& rstr)
{
	CString str(rstr);
	str.Remove(_T('&'));
	return str;
}

bool HaveEd2kRegAccess()
{
	CRegKey regkey;
	DWORD dwRegResult = regkey.Create(HKEY_CLASSES_ROOT, _T("ed2k\\shell\\open\\command"));
	regkey.Close();
	return (dwRegResult == ERROR_SUCCESS);
}

bool Ask4RegFix(bool checkOnly, bool dontAsk)
{
	// Barry - Make backup first
	if (!checkOnly)
		BackupReg();

	// check registry if ed2k links is assigned to emule
	CRegKey regkey;
	if (regkey.Create(HKEY_CLASSES_ROOT, _T("ed2k\\shell\\open\\command")) == ERROR_SUCCESS)
	{
		TCHAR rbuffer[500];
		ULONG maxsize = ARRSIZE(rbuffer);
		regkey.QueryStringValue(NULL, rbuffer, &maxsize);

		TCHAR modbuffer[490];
		::GetModuleFileName(NULL, modbuffer, ARRSIZE(modbuffer));
		CString strCanonFileName = modbuffer;
		strCanonFileName.Replace(_T("%"), _T("%%"));

		TCHAR regbuffer[520];
		_sntprintf(regbuffer, ARRSIZE(regbuffer), _T("\"%s\" \"%%1\""), strCanonFileName);
		if (_tcscmp(rbuffer, regbuffer) != 0)
		{
			if (checkOnly)
				return true;
			if (dontAsk || (AfxMessageBox(GetResString(IDS_ASSIGNED2K), MB_ICONQUESTION|MB_YESNO) == IDYES))
			{
				regkey.SetStringValue(NULL, regbuffer);	
				
				regkey.Create(HKEY_CLASSES_ROOT, _T("ed2k\\DefaultIcon"));
				regkey.SetStringValue(NULL, modbuffer);

				regkey.Create(HKEY_CLASSES_ROOT, _T("ed2k"));
				regkey.SetStringValue(NULL, _T("URL: ed2k Protocol"));
				regkey.SetStringValue(_T("URL Protocol"), _T(""));

				regkey.Open(HKEY_CLASSES_ROOT, _T("ed2k"));
				regkey.RecurseDeleteKey(_T("ddexec"));
				regkey.RecurseDeleteKey(_T("ddeexec"));
			}
		}
		regkey.Close();
	}
	return false;
}

void BackupReg(void)
{
	// Look for pre-existing old ed2k links
	CRegKey regkey;
	if (regkey.Create(HKEY_CLASSES_ROOT, _T("ed2k\\shell\\open\\command")) == ERROR_SUCCESS)
	{
		TCHAR rbuffer[500];
		ULONG maxsize = ARRSIZE(rbuffer);
		// Is it ok to write new values
		if ((regkey.QueryStringValue(_T("OldDefault"), rbuffer, &maxsize) != ERROR_SUCCESS) || (maxsize == 0))
		{
			maxsize = ARRSIZE(rbuffer);
			if ( regkey.QueryStringValue(NULL, rbuffer, &maxsize) == ERROR_SUCCESS )
				regkey.SetStringValue(_T("OldDefault"), rbuffer);

			regkey.Create(HKEY_CLASSES_ROOT, _T("ed2k\\DefaultIcon"));
			maxsize = ARRSIZE(rbuffer);
			if (regkey.QueryStringValue(NULL, rbuffer, &maxsize) == ERROR_SUCCESS)
				regkey.SetStringValue(_T("OldIcon"), rbuffer);
		}
		regkey.Close();
	}
}

// Barry - Restore previous values
void RevertReg(void)
{
	// restore previous ed2k links before being assigned to emule
	CRegKey regkey;
	if (regkey.Create(HKEY_CLASSES_ROOT, _T("ed2k\\shell\\open\\command")) == ERROR_SUCCESS)
	{
		TCHAR rbuffer[500];
		ULONG maxsize = ARRSIZE(rbuffer);
		if (regkey.QueryStringValue(_T("OldDefault"), rbuffer, &maxsize) == ERROR_SUCCESS)
		{
			regkey.SetStringValue(NULL, rbuffer);
			regkey.DeleteValue(_T("OldDefault"));
			regkey.Create(HKEY_CLASSES_ROOT, _T("ed2k\\DefaultIcon"));
			maxsize = ARRSIZE(rbuffer);
			if (regkey.QueryStringValue(_T("OldIcon"), rbuffer, &maxsize) == ERROR_SUCCESS)
			{
				regkey.SetStringValue(NULL, rbuffer);
				regkey.DeleteValue(_T("OldIcon"));
			}
		}
		regkey.Close();
	}
}

int GetMaxWindowsTCPConnections() {
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if(!GetVersionEx((OSVERSIONINFO*)&osvi)) {
		//if OSVERSIONINFOEX doesn't work, try OSVERSIONINFO
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if(!GetVersionEx((OSVERSIONINFO*)&osvi))
			return -1;  //shouldn't ever happen
	}

	if(osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) // Windows NT product family
		return -1;  //no limits

	if(osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) { // Windows 95 product family

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0) { //old school 95
			HKEY hKey;
			DWORD dwValue;
			DWORD dwLength = sizeof(dwValue);
			LONG lResult;

			RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("System\\CurrentControlSet\\Services\\VxD\\MSTCP"),
				0, KEY_QUERY_VALUE, &hKey);
			lResult = RegQueryValueEx(hKey, TEXT("MaxConnections"), NULL, NULL,
				(LPBYTE)&dwValue, &dwLength);
			RegCloseKey(hKey);

			if(lResult != ERROR_SUCCESS || lResult < 1)
				return 100;  //the default for 95 is 100

			return dwValue;

		} else { //98 or ME
			HKEY hKey;
			TCHAR szValue[32];
			DWORD dwLength = sizeof(szValue);
			LONG lResult;

			RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("System\\CurrentControlSet\\Services\\VxD\\MSTCP"),
				0, KEY_QUERY_VALUE, &hKey);
			lResult = RegQueryValueEx(hKey, TEXT("MaxConnections"), NULL, NULL,
				(LPBYTE)szValue, &dwLength);
			RegCloseKey(hKey);

			LONG lMaxConnections;
			if(lResult != ERROR_SUCCESS || (lMaxConnections = _tstoi(szValue)) < 1)
				return 100;  //the default for 98/ME is 100

			return lMaxConnections;
		}         
	}

	return -1;  //give the user the benefit of the doubt, most use NT+ anyway
}

WORD DetectWinVersion()
{
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if(!GetVersionEx((OSVERSIONINFO*)&osvi))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if(!GetVersionEx((OSVERSIONINFO*)&osvi)) 
		return FALSE;
	}

	switch(osvi.dwPlatformId)
	{
		case VER_PLATFORM_WIN32_NT:
			if(osvi.dwMajorVersion <= 4)
				return _WINVER_NT4_;
			if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
				return _WINVER_2K_;
			if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
				return _WINVER_XP_;
			return _WINVER_XP_; // never return Win95 if we get the info about a NT system
      
		case VER_PLATFORM_WIN32_WINDOWS:
			if(osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
				return _WINVER_95_; 
			if(osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
				return _WINVER_98_; 
			if(osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
				return _WINVER_ME_; 
			break;
		
		default:
			break;
	}
	
	return _WINVER_95_;		// there should'nt be anything lower than this
}

int IsRunningXPSP2(){
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if(!GetVersionEx((OSVERSIONINFO*)&osvi))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if(!GetVersionEx((OSVERSIONINFO*)&osvi)) 
			return -1;
	}

	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT && osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1){
		if (osvi.wServicePackMajor >= 2)
			return 1;
	}
	return 0;
}

uint64 GetFreeDiskSpaceX(LPCTSTR pDirectory)
{
	extern bool g_bUnicoWS;
	static BOOL _bInitialized = FALSE;
	static BOOL (WINAPI *_pfnGetDiskFreeSpaceEx)(LPCTSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER) = NULL;
	static BOOL (WINAPI *_pfnGetDiskFreeSpaceExA)(LPCSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER) = NULL;

	if (!_bInitialized)
	{
		_bInitialized = TRUE;
		if (g_bUnicoWS)
		{
			(FARPROC&)_pfnGetDiskFreeSpaceExA = GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetDiskFreeSpaceExA");
		}
		else
		{
			// Why does it not work to load "GetDiskFreeSpaceExW" explicitly from UnicoWS.dll ?
			//extern HMODULE g_hUnicoWS;
			//(FARPROC&)_pGetDiskFreeSpaceEx = GetProcAddress(g_hUnicoWS != NULL ? g_hUnicoWS : GetModuleHandle(_T("kernel32.dll")), _TWINAPI("GetDiskFreeSpaceEx"));
			(FARPROC&)_pfnGetDiskFreeSpaceEx = GetProcAddress(GetModuleHandle(_T("kernel32.dll")), _TWINAPI("GetDiskFreeSpaceEx"));
		}
	}

	if (_pfnGetDiskFreeSpaceEx)
	{
		ULARGE_INTEGER nFreeDiskSpace;
		ULARGE_INTEGER dummy;
		(*_pfnGetDiskFreeSpaceEx)(pDirectory, &nFreeDiskSpace, &dummy, &dummy);
		return nFreeDiskSpace.QuadPart;
	}
	else if (_pfnGetDiskFreeSpaceExA)
	{
		USES_CONVERSION;
		ULARGE_INTEGER nFreeDiskSpace;
		ULARGE_INTEGER dummy;
		(*_pfnGetDiskFreeSpaceExA)(T2CA(pDirectory), &nFreeDiskSpace, &dummy, &dummy);
		return nFreeDiskSpace.QuadPart;
	}
	else 
	{
		TCHAR cDrive[16];
		TCHAR *p = _tcschr(pDirectory, _T('\\'));
		if (p)
		{
			memcpy(cDrive, pDirectory, (p - pDirectory)*sizeof(TCHAR));
			cDrive[p - pDirectory] = _T('\0');
		}
		else
			_tcscpy(cDrive, pDirectory);
		DWORD dwSectPerClust, dwBytesPerSect, dwFreeClusters, dwDummy;
		GetDiskFreeSpace(cDrive, &dwSectPerClust, &dwBytesPerSect, &dwFreeClusters, &dwDummy);
		return (dwFreeClusters * dwSectPerClust * dwBytesPerSect);
	}
}

CString GetRateString(uint16 rate)
{ 
	switch (rate){ 
	case 0: 
		return GetResString(IDS_CMT_NOTRATED); 
	case 1: 
		return GetResString(IDS_CMT_FAKE); 
	case 2: 
		return GetResString(IDS_CMT_POOR); 
	case 3: 
		return GetResString(IDS_CMT_GOOD); 
	case 4: 
		return GetResString(IDS_CMT_FAIR); 
	case 5: 
		return GetResString(IDS_CMT_EXCELLENT); 
	} 
	return GetResString(IDS_CMT_NOTRATED); 
} 

// Returns a BASE32 encoded byte array
//
// [In]
//   buffer: Pointer to byte array
//   bufLen: Lenght of buffer array
//
// [Return]
//   CString object with BASE32 encoded byte array
CString EncodeBase32(const unsigned char* buffer, unsigned int bufLen)
{
	CString Base32Buff;
    
	unsigned int i, index;
    unsigned char word;

    for(i = 0, index = 0; i < bufLen;) {

		// Is the current word going to span a byte boundary?
        if (index > 3) {
            word = (buffer[i] & (0xFF >> index));
            index = (index + 5) % 8;
            word <<= index;
            if (i < bufLen - 1)
                word |= buffer[i + 1] >> (8 - index);

            i++;
        } else {
            word = (buffer[i] >> (8 - (index + 5))) & 0x1F;
            index = (index + 5) % 8;
            if (index == 0)
               i++;
        }

		Base32Buff += (char) base32Chars[word];
    }

    return Base32Buff;
}

// Returns a BASE16 encoded byte array
//
// [In]
//   buffer: Pointer to byte array
//   bufLen: Lenght of buffer array
//
// [Return]
//   CString object with BASE16 encoded byte array
CString EncodeBase16(const unsigned char* buffer, unsigned int bufLen)
{
	CString Base16Buff;

	for(unsigned int i = 0; i < bufLen; i++) {
		Base16Buff += base16Chars[buffer[i] >> 4];
		Base16Buff += base16Chars[buffer[i] & 0xf];
	}

    return Base16Buff;
}

// Decodes a BASE16 string into a byte array
//
// [In]
//   base16Buffer: String containing BASE16
//   base16BufLen: Lenght BASE16 coded string's length
//
// [Out]
//   buffer: byte array containing decoded string
bool DecodeBase16(const TCHAR *base16Buffer, unsigned int base16BufLen, byte *buffer, unsigned int bufflen)
{
	unsigned int uDecodeLengthBase16 = DecodeLengthBase16(base16BufLen);
	if (uDecodeLengthBase16 > bufflen)
		return false;
//==> Optimizer [shadow2004]
#ifdef OPTIM
    memzero(buffer, uDecodeLengthBase16);
#else
    memset(buffer, 0, uDecodeLengthBase16);
#endif
//<== Optimizer [shadow2004]
  
    for(unsigned int i = 0; i < base16BufLen; i++) {
		int lookup = toupper(base16Buffer[i]) - '0';

        // Check to make sure that the given word falls inside a valid range
		byte word = 0;
        
		if ( lookup < 0 || lookup >= BASE16_LOOKUP_MAX)
           word = 0xFF;
        else
           word = base16Lookup[lookup][1];

		if(i % 2 == 0) {
			buffer[i/2] = word << 4;
		} else {
			buffer[(i-1)/2] |= word;
		}
	}
	return true;
}

// Calculates length to decode from BASE16
//
// [In]
//   base16Length: Actual length of BASE16 string
//
// [Return]
//   New length of byte array decoded
unsigned int DecodeLengthBase16(unsigned int base16Length)
{
	return base16Length / 2U;
}

uint32 DecodeBase32(LPCTSTR pszInput, uchar* paucOutput, uint32 nBufferLen)
{
	if (pszInput == NULL)
		return false;
	uint32 nDecodeLen = (_tcslen(pszInput)*5)/8;
	if ((_tcslen(pszInput)*5) % 8 > 0)
		nDecodeLen++;
	uint32 nInputLen = _tcslen( pszInput );
	if (paucOutput == NULL || nBufferLen == 0)
		return nDecodeLen;
	if (nDecodeLen > nBufferLen || paucOutput == NULL) 
		return 0;

	DWORD nBits	= 0;
	int nCount	= 0;

	for ( int nChars = nInputLen ; nChars-- ; pszInput++ )
	{
		if ( *pszInput >= 'A' && *pszInput <= 'Z' )
			nBits |= ( *pszInput - 'A' );
		else if ( *pszInput >= 'a' && *pszInput <= 'z' )
			nBits |= ( *pszInput - 'a' );
		else if ( *pszInput >= '2' && *pszInput <= '7' )
			nBits |= ( *pszInput - '2' + 26 );
		else
			return 0;
		
		nCount += 5;

		if ( nCount >= 8 )
		{
			*paucOutput++ = (BYTE)( nBits >> ( nCount - 8 ) );
			nCount -= 8;
		}

		nBits <<= 5;
	}

	return nDecodeLen;
}

uint32 DecodeBase32(LPCTSTR pszInput, CAICHHash& Hash){
	return DecodeBase32(pszInput, Hash.GetRawHash(), Hash.GetHashSize());
}

typedef struct
{
	LPCTSTR	pszInitialDir;
	LPCTSTR	pszDlgTitle;
} BROWSEINIT, *LPBROWSEINIT;

extern "C" int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM /*lParam*/, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED)
	{
		// Set initial directory
		if ( ((LPBROWSEINIT)lpData)->pszInitialDir != NULL )
			SendMessage(hWnd, BFFM_SETSELECTION, TRUE, (LPARAM)((LPBROWSEINIT)lpData)->pszInitialDir);

		// Set dialog's window title
		if ( ((LPBROWSEINIT)lpData)->pszDlgTitle != NULL )
			SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)((LPBROWSEINIT)lpData)->pszDlgTitle);
	}

	return 0;
}

bool SelectDir(HWND hWnd, LPTSTR pszPath, LPCTSTR pszTitle, LPCTSTR pszDlgTitle)
{
	bool bResult = false;
	CoInitialize(0);
	LPMALLOC pShlMalloc;
	if (SHGetMalloc(&pShlMalloc) == NOERROR)
	{
		BROWSEINFO BrsInfo = {0};
		BrsInfo.hwndOwner = hWnd;
		BrsInfo.lpszTitle = (pszTitle != NULL) ? pszTitle : pszDlgTitle;
		BrsInfo.ulFlags = BIF_VALIDATE | BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS | BIF_SHAREABLE | BIF_DONTGOBELOWDOMAIN;

		BROWSEINIT BrsInit = {0};
		if (pszPath != NULL || pszTitle != NULL || pszDlgTitle != NULL){
			// Need the 'BrowseCallbackProc' to set those strings
			BrsInfo.lpfn = BrowseCallbackProc;
			BrsInfo.lParam = (LPARAM)&BrsInit;
			BrsInit.pszDlgTitle = (pszDlgTitle != NULL) ? pszDlgTitle : NULL/*pszTitle*/;
			BrsInit.pszInitialDir = pszPath;
		}

		LPITEMIDLIST pidlBrowse;
		if ((pidlBrowse = SHBrowseForFolder(&BrsInfo)) != NULL){
			if (SHGetPathFromIDList(pidlBrowse, pszPath))
				bResult = true;
			pShlMalloc->Free(pidlBrowse);
		}
		pShlMalloc->Release();
	}
	CoUninitialize();
	return bResult;
}

void MakeFoldername(LPTSTR pszPath)
{
	CString strPath(pszPath);
	if (!strPath.IsEmpty()) // don't canonicalize an empty path, we would get a "\"
	{
		PathCanonicalize(pszPath, strPath);
		PathRemoveBackslash(pszPath);
	}
}

CString StringLimit(CString in,uint16 length){
	if (in.GetLength()<=length || length<10) return in;

	return (in.Left(length-8) + _T("...") + in.Right(8));
}

BOOL DialogBrowseFile(CString& rstrPath, LPCTSTR pszFilters, LPCTSTR pszDefaultFileName, DWORD dwFlags,bool openfilestyle) {
    CFileDialog myFileDialog(openfilestyle,NULL,pszDefaultFileName,
							 dwFlags | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, pszFilters, NULL, 
							 0/*automatically use explorer style open dialog on systems which support it*/);
    if (myFileDialog.DoModal() != IDOK)
		return FALSE;
	rstrPath = myFileDialog.GetPathName();
	return TRUE;
}

void md4str(const uchar* hash, TCHAR* pszHash)
{
    static const TCHAR _acHexDigits[] = _T("0123456789ABCDEF");
    for (int i = 0; i < 16; i++){
		*pszHash++ = _acHexDigits[hash[i] >> 4];
		*pszHash++ = _acHexDigits[hash[i] & 0xf];
	}
	*pszHash = _T('\0');
}

CString md4str(const uchar* hash)
{
	TCHAR szHash[MAX_HASHSTR_SIZE];
	md4str(hash, szHash);
	return CString(szHash);
}

void md4strA(const uchar* hash, CHAR* pszHash)
{
    static const CHAR _acHexDigits[] = "0123456789ABCDEF";
    for (int i = 0; i < 16; i++){
		*pszHash++ = _acHexDigits[hash[i] >> 4];
		*pszHash++ = _acHexDigits[hash[i] & 0xf];
	}
	*pszHash = '\0';
}

CStringA md4strA(const uchar* hash)
{
	CHAR szHash[MAX_HASHSTR_SIZE];
	md4strA(hash, szHash);
	return CStringA(szHash);
}

bool strmd4(const char* pszHash, uchar* hash)
{
//==> Optimizer [shadow2004]
#ifdef OPTIM
	memzero(hash, 16);
#else
	memset(hash, 0, 16);
#endif
//<== Optimizer [shadow2004]
	for (int i = 0; i < 16; i++)
	{
		char byte[3];
		byte[0] = pszHash[i*2+0];
		byte[1] = pszHash[i*2+1];
		byte[2] = '\0';

		UINT b;
		if (sscanf(byte, "%x", &b) != 1)
			return false;
		hash[i] = b;
	}
	return true;
}

bool strmd4(const CString& rstr, uchar* hash)
{
//==> Optimizer [shadow2004]
#ifdef OPTIM
	memzero(hash, 16);
#else
	memset(hash, 0, 16);
#endif
//<== Optimizer [shadow2004]
	if (rstr.GetLength() != 16*2)
		return false;
	for (int i = 0; i < 16; i++)
	{
		char byte[3];
		byte[0] = (char)rstr[i*2+0];
		byte[1] = (char)rstr[i*2+1];
		byte[2] = '\0';

		UINT b;
		if (sscanf(byte, "%x", &b) != 1)
			return false;
		hash[i] = b;
	}
	return true;
}

void StripTrailingCollon(CString& rstr)
{
	if (!rstr.IsEmpty())
	{
		if (rstr[rstr.GetLength() - 1] == _T(':'))
			rstr = rstr.Left(rstr.GetLength() - 1);
	}
}

CString CleanupFilename(CString filename)
{
	filename = URLDecode(filename);
	filename.MakeLower();

	//remove substrings, defined in the preferences (.ini)
	CString strlink = thePrefs.GetFilenameCleanups().MakeLower();
	int curPos = 0;
	CString resToken = strlink.Tokenize(_T("|"), curPos);
	while (!resToken.IsEmpty())
	{
		filename.Replace(resToken, _T(""));
		resToken = strlink.Tokenize(_T("|"), curPos);
	}

	// Replace "." with spaces - except the last one (extension-dot)
	int extpos = filename.ReverseFind(_T('.'));
	if (extpos > 0)
	{
		for (int i = 0; i < extpos; i++)
		{
			if (filename.GetAt(i) != _T('.'))
				continue;
			if (i > 0 && i < filename.GetLength()-1 && _istdigit(filename.GetAt(i-1)) && _istdigit(filename.GetAt(i+1)))
				continue;
			filename.SetAt(i, _T(' '));
		}
	}

	// replace space-holders with spaces
	filename.Replace(_T('_'), _T(' '));
	filename.Replace(_T('+'), _T(' '));
	filename.Replace(_T('='), _T(' '));

	// remove invalid filename characters
	filename.Replace(_T("\\"), _T(""));
	filename.Replace(_T("\""), _T(""));
	filename.Replace(_T("/"), _T(""));
	filename.Replace(_T(":"), _T(""));
	filename.Replace(_T("*"), _T(""));
	filename.Replace(_T("?"), _T(""));
	filename.Replace(_T("<"), _T(""));
	filename.Replace(_T(">"), _T(""));
	filename.Replace(_T("|"), _T(""));

	// remove [AD]
	CString tempStr;
	int pos1 = -1;
	for (;;)
	{
		pos1 = filename.Find(_T('['), pos1+1);
		if (pos1 == -1)
			break;
		int pos2 = filename.Find(_T(']'), pos1);
		if (pos1 > -1 && pos2 > pos1)
		{
			if (pos2 - pos1 > 1)
			{
				tempStr = filename.Mid(pos1+1, pos2-pos1-1);
				int numcount = 0;
				for (int i = 0; i < tempStr.GetLength(); i++)
				{
					if (_istdigit(tempStr.GetAt(i)))
						numcount++;
				}
				if (numcount > tempStr.GetLength()/2)
					continue;
			}
			filename = filename.Left(pos1) + filename.Right(filename.GetLength()-pos2-1);
			pos1--;
		}
		else
			break;
	}

	// Make leading Caps 
	if (filename.GetLength() > 1)
	{
		tempStr = filename.GetAt(0);
		tempStr.MakeUpper();
		filename.SetAt(0, tempStr.GetAt(0));

		int topos = filename.ReverseFind(_T('.')) - 1;
		if (topos < 0)
			topos = filename.GetLength() - 1;

		for (int ix = 0; ix < topos; ix++)
		{
			if (!IsCharAlpha(filename.GetAt(ix)))
			{
				if (	(ix < filename.GetLength()-2 && _istdigit(filename.GetAt(ix+2))) ||
						filename.GetAt(ix)==_T('\'')
					)
					continue;

				tempStr = filename.GetAt(ix+1);
				tempStr.MakeUpper();
				filename.SetAt(ix+1, tempStr.GetAt(0));
			}
		}
	}

	// additional formatting
	filename.Replace(_T("()"), _T(""));
	filename.Replace(_T("  "), _T(" "));
	filename.Replace(_T(" ."), _T("."));
	filename.Replace(_T("( "), _T("("));
	filename.Replace(_T(" )"), _T(")"));
	filename.Replace(_T("()"), _T(""));
	filename.Replace(_T("{ "), _T("{"));
	filename.Replace(_T(" }"), _T("}"));
	filename.Replace(_T("{}"), _T(""));


	filename.Trim();
	return filename;
}

struct SED2KFileType
{
	LPCTSTR pszExt;
	EED2KFileType iFileType;
} _aED2KFileTypes[] = 
{
    { _T(".669"),   ED2KFT_AUDIO },
    { _T(".aac"),   ED2KFT_AUDIO },
    { _T(".aif"),   ED2KFT_AUDIO },
    { _T(".aiff"),  ED2KFT_AUDIO },
    { _T(".amf"),   ED2KFT_AUDIO },
    { _T(".ams"),   ED2KFT_AUDIO },
    { _T(".ape"),   ED2KFT_AUDIO },
    { _T(".au"),    ED2KFT_AUDIO },
    { _T(".dbm"),   ED2KFT_AUDIO },
    { _T(".dmf"),   ED2KFT_AUDIO },
    { _T(".dsm"),   ED2KFT_AUDIO },
    { _T(".far"),   ED2KFT_AUDIO },
    { _T(".flac"),  ED2KFT_AUDIO },
    { _T(".it"),    ED2KFT_AUDIO },
    { _T(".mdl"),   ED2KFT_AUDIO },
    { _T(".med"),   ED2KFT_AUDIO },
    { _T(".mid"),   ED2KFT_AUDIO },
    { _T(".midi"),  ED2KFT_AUDIO },
    { _T(".mka"),   ED2KFT_AUDIO },
    { _T(".mod"),   ED2KFT_AUDIO },
    { _T(".mol"),   ED2KFT_AUDIO },
    { _T(".mp1"),   ED2KFT_AUDIO },
    { _T(".mp2"),   ED2KFT_AUDIO },
    { _T(".mp3"),   ED2KFT_AUDIO },
    { _T(".mp4"),   ED2KFT_AUDIO },
    { _T(".mpa"),   ED2KFT_AUDIO },
    { _T(".mpc"),   ED2KFT_AUDIO },
    { _T(".mpp"),   ED2KFT_AUDIO },
    { _T(".mtm"),   ED2KFT_AUDIO },
    { _T(".nst"),   ED2KFT_AUDIO },
    { _T(".ogg"),   ED2KFT_AUDIO },
    { _T(".okt"),   ED2KFT_AUDIO },
    { _T(".psm"),   ED2KFT_AUDIO },
    { _T(".ptm"),   ED2KFT_AUDIO },
    { _T(".ra"),    ED2KFT_AUDIO },
    { _T(".rmi"),   ED2KFT_AUDIO },
    { _T(".s3m"),   ED2KFT_AUDIO },
    { _T(".stm"),   ED2KFT_AUDIO },
    { _T(".ult"),   ED2KFT_AUDIO },
    { _T(".umx"),   ED2KFT_AUDIO },
    { _T(".wav"),   ED2KFT_AUDIO },
    { _T(".wma"),   ED2KFT_AUDIO },
    { _T(".wow"),   ED2KFT_AUDIO },
    { _T(".xm"),    ED2KFT_AUDIO },

    { _T(".asf"),   ED2KFT_VIDEO },
    { _T(".avi"),   ED2KFT_VIDEO },
    { _T(".divx"),  ED2KFT_VIDEO },
    { _T(".m1v"),   ED2KFT_VIDEO },
    { _T(".m2v"),   ED2KFT_VIDEO },
    { _T(".mkv"),   ED2KFT_VIDEO },
    { _T(".mov"),   ED2KFT_VIDEO },
    { _T(".mp1v"),  ED2KFT_VIDEO },
    { _T(".mp2v"),  ED2KFT_VIDEO },
    { _T(".mpe"),   ED2KFT_VIDEO },
    { _T(".mpeg"),  ED2KFT_VIDEO },
    { _T(".mpg"),   ED2KFT_VIDEO },
    { _T(".mps"),   ED2KFT_VIDEO },
    { _T(".mpv"),   ED2KFT_VIDEO },
    { _T(".mpv1"),  ED2KFT_VIDEO },
    { _T(".mpv2"),  ED2KFT_VIDEO },
    { _T(".ogm"),   ED2KFT_VIDEO },
    { _T(".qt"),    ED2KFT_VIDEO },
    { _T(".ram"),   ED2KFT_VIDEO },
    { _T(".rm"),    ED2KFT_VIDEO },
    { _T(".rv"),    ED2KFT_VIDEO },
    { _T(".rv9"),   ED2KFT_VIDEO },
    { _T(".ts"),    ED2KFT_VIDEO },
    { _T(".vivo"),  ED2KFT_VIDEO },
    { _T(".vob"),   ED2KFT_VIDEO },
    { _T(".wmv"),   ED2KFT_VIDEO },
    { _T(".xvid"),  ED2KFT_VIDEO },

    { _T(".bmp"),   ED2KFT_IMAGE },
    { _T(".dcx"),   ED2KFT_IMAGE },
    { _T(".emf"),   ED2KFT_IMAGE },
    { _T(".gif"),   ED2KFT_IMAGE },
    { _T(".ico"),   ED2KFT_IMAGE },
    { _T(".jpeg"),  ED2KFT_IMAGE },
    { _T(".jpg"),   ED2KFT_IMAGE },
    { _T(".pct"),   ED2KFT_IMAGE },
    { _T(".pcx"),   ED2KFT_IMAGE },
    { _T(".pic"),   ED2KFT_IMAGE },
    { _T(".pict"),  ED2KFT_IMAGE },
    { _T(".png"),   ED2KFT_IMAGE },
    { _T(".psd"),   ED2KFT_IMAGE },
    { _T(".psp"),   ED2KFT_IMAGE },
    { _T(".tga"),   ED2KFT_IMAGE },
    { _T(".tif"),   ED2KFT_IMAGE },
    { _T(".tiff"),  ED2KFT_IMAGE },
    { _T(".wmf"),   ED2KFT_IMAGE },
    { _T(".xif"),   ED2KFT_IMAGE },

    { _T(".7z"),	ED2KFT_ARCHIVE },
    { _T(".ace"),   ED2KFT_ARCHIVE },
	{ _T(".alz"),   ED2KFT_ARCHIVE },
    { _T(".arj"),   ED2KFT_ARCHIVE },
    { _T(".bz2"),   ED2KFT_ARCHIVE },
    { _T(".cab"),   ED2KFT_ARCHIVE },
    { _T(".gz"),    ED2KFT_ARCHIVE },
    { _T(".hqx"),   ED2KFT_ARCHIVE },
    { _T(".lha"),   ED2KFT_ARCHIVE },
    { _T(".lzh"),   ED2KFT_ARCHIVE },
    { _T(".msi"),   ED2KFT_ARCHIVE },
    { _T(".rar"),   ED2KFT_ARCHIVE },
    { _T(".sea"),   ED2KFT_ARCHIVE },
    { _T(".sit"),   ED2KFT_ARCHIVE },
    { _T(".tar"),   ED2KFT_ARCHIVE },
    { _T(".tgz"),   ED2KFT_ARCHIVE },
    { _T(".uc2"),   ED2KFT_ARCHIVE },
    { _T(".z"),		ED2KFT_ARCHIVE },
	{ _T(".zip"),   ED2KFT_ARCHIVE },

    { _T(".bat"),   ED2KFT_PROGRAM },
    { _T(".cmd"),   ED2KFT_PROGRAM },
    { _T(".com"),   ED2KFT_PROGRAM },
    { _T(".exe"),   ED2KFT_PROGRAM },

    { _T(".bin"),   ED2KFT_CDIMAGE },
    { _T(".bwa"),   ED2KFT_CDIMAGE },
    { _T(".bwi"),   ED2KFT_CDIMAGE },
    { _T(".bws"),   ED2KFT_CDIMAGE },
    { _T(".bwt"),   ED2KFT_CDIMAGE },
    { _T(".ccd"),   ED2KFT_CDIMAGE },
    { _T(".cue"),   ED2KFT_CDIMAGE },
    { _T(".dmg"),   ED2KFT_CDIMAGE },
    { _T(".dmz"),   ED2KFT_CDIMAGE },
    { _T(".img"),   ED2KFT_CDIMAGE },
    { _T(".iso"),   ED2KFT_CDIMAGE },
    { _T(".mdf"),   ED2KFT_CDIMAGE },
    { _T(".mds"),   ED2KFT_CDIMAGE },
    { _T(".nrg"),   ED2KFT_CDIMAGE },
    { _T(".sub"),   ED2KFT_CDIMAGE },
    { _T(".toast"), ED2KFT_CDIMAGE },

    { _T(".chm"),   ED2KFT_DOCUMENT },
    { _T(".css"),   ED2KFT_DOCUMENT },
    { _T(".diz"),   ED2KFT_DOCUMENT },
    { _T(".doc"),   ED2KFT_DOCUMENT },
    { _T(".dot"),   ED2KFT_DOCUMENT },
    { _T(".hlp"),   ED2KFT_DOCUMENT },
    { _T(".htm"),   ED2KFT_DOCUMENT },
    { _T(".html"),  ED2KFT_DOCUMENT },
    { _T(".nfo"),   ED2KFT_DOCUMENT },
    { _T(".pdf"),   ED2KFT_DOCUMENT },
    { _T(".pps"),   ED2KFT_DOCUMENT },
    { _T(".ppt"),   ED2KFT_DOCUMENT },
    { _T(".ps"),    ED2KFT_DOCUMENT },
    { _T(".rtf"),   ED2KFT_DOCUMENT },
    { _T(".wri"),   ED2KFT_DOCUMENT },
    { _T(".txt"),   ED2KFT_DOCUMENT },
    { _T(".xls"),   ED2KFT_DOCUMENT },
	{ _T(".xml"),   ED2KFT_DOCUMENT }
};

int __cdecl CompareE2DKFileType(const void* p1, const void* p2)
{
	return _tcscmp( ((const SED2KFileType*)p1)->pszExt, ((const SED2KFileType*)p2)->pszExt );
}

EED2KFileType GetED2KFileTypeID(LPCTSTR pszFileName)
{
	LPCTSTR pszExt = _tcsrchr(pszFileName, _T('.'));
	if (pszExt == NULL)
		return ED2KFT_ANY;
	CString strExt(pszExt);
	strExt.MakeLower();

	SED2KFileType ft;
	ft.pszExt = strExt;
	ft.iFileType = ED2KFT_ANY;
	const SED2KFileType* pFound = (SED2KFileType*)bsearch(&ft, _aED2KFileTypes, ARRSIZE(_aED2KFileTypes), sizeof _aED2KFileTypes[0], CompareE2DKFileType);
	if (pFound != NULL)
		return pFound->iFileType;
	return ED2KFT_ANY;
}

// Retuns the ed2k file type term which is to be used in server searches
LPCSTR GetED2KFileTypeSearchTerm(EED2KFileType iFileID)
{
	if (iFileID == ED2KFT_AUDIO)		return ED2KFTSTR_AUDIO;
	if (iFileID == ED2KFT_VIDEO)		return ED2KFTSTR_VIDEO;
	if (iFileID == ED2KFT_IMAGE)		return ED2KFTSTR_IMAGE;
	if (iFileID == ED2KFT_DOCUMENT)		return ED2KFTSTR_DOCUMENT;
	if (iFileID == ED2KFT_PROGRAM)		return ED2KFTSTR_PROGRAM;
	// NOTE: Archives and CD-Images are published with file type "Pro"
	if (iFileID == ED2KFT_ARCHIVE)		return ED2KFTSTR_PROGRAM;
	if (iFileID == ED2KFT_CDIMAGE)		return ED2KFTSTR_PROGRAM;
	return NULL;
}

// Returns a file type which is used eMule internally only, examining the extention of the given filename
CString GetFileTypeByName(LPCTSTR pszFileName)
{
	EED2KFileType iFileType = GetED2KFileTypeID(pszFileName);
	switch (iFileType) {
		case ED2KFT_AUDIO:		return _T(ED2KFTSTR_AUDIO);
		case ED2KFT_VIDEO:		return _T(ED2KFTSTR_VIDEO);
		case ED2KFT_IMAGE:		return _T(ED2KFTSTR_IMAGE);
		case ED2KFT_DOCUMENT:	return _T(ED2KFTSTR_DOCUMENT);
		case ED2KFT_PROGRAM:	return _T(ED2KFTSTR_PROGRAM);
		case ED2KFT_ARCHIVE:	return _T(ED2KFTSTR_ARCHIVE);
		case ED2KFT_CDIMAGE:	return _T(ED2KFTSTR_CDIMAGE);
		default:				return _T("");
	}
}

// Returns a file type which is used eMule internally only (GUI)
CString GetFileTypeDisplayStrFromED2KFileType(LPCTSTR pszED2KFileType)
{
	ASSERT( pszED2KFileType != NULL );
	if (pszED2KFileType != NULL)
	{
		if (_tcscmp(pszED2KFileType, _T(ED2KFTSTR_AUDIO)) == 0)			return GetResString(IDS_SEARCH_AUDIO);
		else if (_tcscmp(pszED2KFileType, _T(ED2KFTSTR_VIDEO)) == 0)    return GetResString(IDS_SEARCH_VIDEO);
		else if (_tcscmp(pszED2KFileType, _T(ED2KFTSTR_IMAGE)) == 0)    return GetResString(IDS_SEARCH_PICS);
		else if (_tcscmp(pszED2KFileType, _T(ED2KFTSTR_DOCUMENT)) == 0)	return GetResString(IDS_SEARCH_DOC);
		else if (_tcscmp(pszED2KFileType, _T(ED2KFTSTR_PROGRAM)) == 0)  return GetResString(IDS_SEARCH_PRG);
		else if (_tcscmp(pszED2KFileType, _T(ED2KFTSTR_ARCHIVE)) == 0)	return GetResString(IDS_SEARCH_ARC);
		else if (_tcscmp(pszED2KFileType, _T(ED2KFTSTR_CDIMAGE)) == 0)  return GetResString(IDS_SEARCH_CDIMG);
	}
	return _T("");
}

class CED2KFileTypes{
public:
	CED2KFileTypes(){
		qsort(_aED2KFileTypes, ARRSIZE(_aED2KFileTypes), sizeof _aED2KFileTypes[0], CompareE2DKFileType);
#ifdef _DEBUG
		// check for duplicate entries
		LPCTSTR pszLast = _aED2KFileTypes[0].pszExt;
		for (int i = 1; i < ARRSIZE(_aED2KFileTypes); i++){
			ASSERT( _tcscmp(pszLast, _aED2KFileTypes[i].pszExt) != 0 );
			pszLast = _aED2KFileTypes[i].pszExt;
		}
#endif
	}
};
CED2KFileTypes theED2KFileTypes; // get the list sorted *before* any code is accessing it

TCHAR *stristr(const TCHAR *str1, const TCHAR *str2)
{
	const TCHAR *cp = str1;
	const TCHAR *s1;
	const TCHAR *s2;

	if (!*str2)
		return (TCHAR *)str1;

	while (*cp)
	{
		s1 = cp;
		s2 = str2;

		while (*s1 && *s2 && _totlower(*s1) == _totlower(*s2))
			s1++, s2++;

		if (!*s2)
			return (TCHAR *)cp;

		cp++;
	}

	return NULL;
}

int GetSystemErrorString(DWORD dwError, CString &rstrError)
{
	// FormatMessage language flags:
	//
	// - MFC uses: MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT)
	//				SUBLANG_SYS_DEFAULT = 0x02 (system default)
	//
	// - SDK uses: MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)
	//				SUBLANG_DEFAULT		= 0x01 (user default)
	//
	//
	// Found in "winnt.h"
	// ------------------
	//  Language IDs.
	//
	//  The following two combinations of primary language ID and
	//  sublanguage ID have special semantics:
	//
	//    Primary Language ID   Sublanguage ID      Result
	//    -------------------   ---------------     ------------------------
	//    LANG_NEUTRAL          SUBLANG_NEUTRAL     Language neutral
	//    LANG_NEUTRAL          SUBLANG_DEFAULT     User default language
	//    LANG_NEUTRAL          SUBLANG_SYS_DEFAULT System default language
	//
	// *** SDK notes also:
	// If you pass in zero, 'FormatMessage' looks for a message for LANGIDs in 
	// the following order:
	//
	//	1) Language neutral 
	//	2) Thread LANGID, based on the thread's locale value 
	//  3) User default LANGID, based on the user's default locale value 
	//	4) System default LANGID, based on the system default locale value 
	//	5) US English 
	LPTSTR pszSysMsg = NULL;
	DWORD dwLength = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
								   NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
								   (LPTSTR)&pszSysMsg, 0, NULL);
	if (pszSysMsg != NULL && dwLength != 0)
	{
		if (dwLength >= 2 && pszSysMsg[dwLength - 2] == _T('\r'))
			pszSysMsg[dwLength - 2] = _T('\0');
		rstrError = pszSysMsg;
		rstrError.Replace(_T("\r\n"), _T(" ")); // some messages contain CRLF within the message!?
	}
	else {
		rstrError.Empty();
	}

	if (pszSysMsg)
		LocalFree(pszSysMsg);

	return rstrError.GetLength();
}

int GetModuleErrorString(DWORD dwError, CString &rstrError, LPCTSTR pszModule)
{
	LPTSTR pszSysMsg = NULL;
	DWORD dwLength = FormatMessage(FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
								   GetModuleHandle(pszModule), dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
								   (LPTSTR)&pszSysMsg, 0, NULL);
	if (pszSysMsg != NULL && dwLength != 0)
	{
		if (dwLength >= 2 && pszSysMsg[dwLength - 2] == _T('\r'))
			pszSysMsg[dwLength - 2] = _T('\0');
		rstrError = pszSysMsg;
		rstrError.Replace(_T("\r\n"), _T(" ")); // some messages contain CRLF within the message!?
	}
	else {
		rstrError.Empty();
	}

	if (pszSysMsg)
		LocalFree(pszSysMsg);

	return rstrError.GetLength();
}

int GetErrorMessage(DWORD dwError, CString &rstrErrorMsg, DWORD dwFlags)
{
	int iMsgLen = GetSystemErrorString(dwError, rstrErrorMsg);
	if (iMsgLen == 0)
	{
		if ((long)dwError >= 0)
			rstrErrorMsg.Format(_T("Error %u"), dwError);
		else
			rstrErrorMsg.Format(_T("Error 0x%08x"), dwError);
	}
	else if (dwFlags & 1)
	{
		CString strFullErrorMsg;
		if ((long)dwError >= 0)
			strFullErrorMsg.Format(_T("Error %u: %s"), dwError, rstrErrorMsg);
		else
			strFullErrorMsg.Format(_T("Error 0x%08x: %s"), dwError, rstrErrorMsg);
		rstrErrorMsg = strFullErrorMsg;
	}

	return rstrErrorMsg.GetLength();
}

CString GetErrorMessage(DWORD dwError, DWORD dwFlags)
{
	CString strError;
	GetErrorMessage(dwError, strError, dwFlags);
	return strError;
}

int GetAppImageListColorFlag()
{
	HDC hdcScreen = ::GetDC(HWND_DESKTOP);
	int iColorBits = GetDeviceCaps(hdcScreen, BITSPIXEL) * GetDeviceCaps(hdcScreen, PLANES);
	::ReleaseDC(HWND_DESKTOP, hdcScreen);
	int iIlcFlag;
	if (iColorBits >= 32)
		iIlcFlag = ILC_COLOR32;
	else if (iColorBits >= 24)
		iIlcFlag = ILC_COLOR24;
	else if (iColorBits >= 16)
		iIlcFlag = ILC_COLOR16;
	else if (iColorBits >= 8)
		iIlcFlag = ILC_COLOR8;
	else if (iColorBits >= 4)
		iIlcFlag = ILC_COLOR4;
	else
		iIlcFlag = ILC_COLOR;
	return iIlcFlag;
}

CString DbgGetHexDump(const uint8* data, UINT size)
{
	CString buffer; 
	buffer.Format(_T("Size=%u"), size);
	buffer += _T(", Data=[");
	UINT i = 0;
	for(; i < size && i < 50; i++){
		if (i > 0)
			buffer += _T(" ");
		TCHAR temp[3];
		_stprintf(temp, _T("%02x"), data[i]);
		buffer += temp;
	}
	buffer += (i == size) ? _T("]") : _T("..]");
	return buffer;
}

void DbgSetThreadName(LPCSTR szThreadName, ...) 
{
#ifdef DEBUG

#ifndef MS_VC_EXCEPTION
#define MS_VC_EXCEPTION 0x406d1388 

typedef struct tagTHREADNAME_INFO 
{
	DWORD dwType;		// must be 0x1000 
	LPCSTR szName;		// pointer to name (in same addr space) 
	DWORD dwThreadID;	// thread ID (-1 caller thread) 
	DWORD dwFlags;		// reserved for future use, must be zero 
} THREADNAME_INFO; 
#endif

	__try
	{
		va_list args;
		va_start(args, szThreadName);
		int lenBuf = 0;
		char *buffer = NULL;
		int lenResult;
		do // the VS debugger truncates the string to 31 characters anyway!
		{
			lenBuf += 128;
			if (buffer != NULL)
				delete [] buffer;
			buffer = new char[lenBuf];
			lenResult = _vsnprintf(buffer, lenBuf, szThreadName, args);
		} while (lenResult == -1);
		va_end(args);
		THREADNAME_INFO info; 
		info.dwType = 0x1000; 
		info.szName = buffer; 
		info.dwThreadID = (DWORD)-1; 
		info.dwFlags = 0; 
		__try 
		{ 
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(DWORD), (DWORD *)&info); 
		} __except (EXCEPTION_CONTINUE_EXECUTION) { } 
		delete [] buffer;
	}
	__except (EXCEPTION_CONTINUE_EXECUTION) {}
#endif
}

CString RemoveFileExtension(const CString& rstrFilePath)
{
	int iDot = rstrFilePath.ReverseFind(_T('.'));
	if (iDot == -1)
		return rstrFilePath;
	return rstrFilePath.Mid(0, iDot);
}

int CompareDirectories(const CString& rstrDir1, const CString& rstrDir2)
{
	// use case insensitive compare as a starter
	if (rstrDir1.CompareNoCase(rstrDir2)==0)
		return 0;

	// if one of the paths ends with a '\' the paths may still be equal from the file system's POV
	CString strDir1(rstrDir1);
	CString strDir2(rstrDir2);
	PathRemoveBackslash(strDir1.GetBuffer());	// remove any available backslash
	strDir1.ReleaseBuffer();
	PathRemoveBackslash(strDir2.GetBuffer());	// remove any available backslash
	strDir2.ReleaseBuffer();
	return strDir1.CompareNoCase(strDir2);		// compare again
}

bool IsGoodIP(uint32 nIP, bool forceCheck)
{
	// always filter following IP's
	// -------------------------------------------
	// 0.0.0.0							invalid
	// 127.0.0.0 - 127.255.255.255		Loopback
    // 224.0.0.0 - 239.255.255.255		Multicast
    // 240.0.0.0 - 255.255.255.255		Reserved for Future Use
	// 255.255.255.255					invalid

	if (nIP==0 || (uint8)nIP==127 || (uint8)nIP>=224){
#ifdef _DEBUG
		if (nIP==0x0100007F && thePrefs.GetAllowLocalHostIP())
			return true;
#endif
		return false;
	}

	if (!thePrefs.FilterLANIPs() && !forceCheck/*ZZ:UploadSpeedSense*/)
		return true;

	// filter LAN IP's
	// -------------------------------------------
	//	0.*								"This" Network
	//	10.0.0.0 - 10.255.255.255		Class A
	//	172.16.0.0 - 172.31.255.255		Class B
	//	192.168.0.0 - 192.168.255.255	Class C

	uint8 nFirst = (uint8)nIP;
	uint8 nSecond = (uint8)(nIP >> 8);

	if (nFirst==192 && nSecond==168) // check this 1st, because those LANs IPs are mostly spreaded
		return false;

	if (nFirst==172 && nSecond>=16 && nSecond<=31)
		return false;

	if (nFirst==0 || nFirst==10)
		return false;

	return true; 
}

bool IsGoodIPPort(uint32 nIP, uint16 nPort)
{
	return IsGoodIP(nIP) && nPort!=0;
}

CString GetFormatedUInt(ULONG ulVal)
{
	TCHAR szVal[12];
	_ultot(ulVal, szVal, 10);

	static NUMBERFMT nf;
	if (nf.Grouping == 0) {
		nf.NumDigits = 0;
		nf.LeadingZero = 0;
		nf.Grouping = 3;
		// we are hardcoding the following two format chars by intention because the C-RTL also has the decimal sep hardcoded to '.'
		nf.lpDecimalSep = _T(".");
		nf.lpThousandSep = _T(",");
		nf.NegativeOrder = 0;
	}
	CString strVal;
	const int iBuffSize = ARRSIZE(szVal)*2;
	int iResult = GetNumberFormat(LOCALE_SYSTEM_DEFAULT, 0, szVal, &nf, strVal.GetBuffer(iBuffSize), iBuffSize);
	strVal.ReleaseBuffer();
	if (iResult == 0)
		strVal = szVal;
	return strVal;
}

CString GetFormatedUInt64(ULONGLONG ullVal)
{
	TCHAR szVal[24];
	_ui64tot(ullVal, szVal, 10);

	static NUMBERFMT nf;
	if (nf.Grouping == 0) {
		nf.NumDigits = 0;
		nf.LeadingZero = 0;
		nf.Grouping = 3;
		// we are hardcoding the following two format chars by intention because the C-RTL also has the decimal sep hardcoded to '.'
		nf.lpDecimalSep = _T(".");
		nf.lpThousandSep = _T(",");
		nf.NegativeOrder = 0;
	}
	CString strVal;
	const int iBuffSize = ARRSIZE(szVal)*2;
	int iResult = GetNumberFormat(LOCALE_SYSTEM_DEFAULT, 0, szVal, &nf, strVal.GetBuffer(iBuffSize), iBuffSize);
	strVal.ReleaseBuffer();
	if (iResult == 0)
		strVal = szVal;
	return strVal;
}

void Debug(LPCTSTR pszFmtMsg, ...)
{
	va_list pArgs;
	va_start(pArgs, pszFmtMsg);
	CString strBuff;
#ifdef _DEBUG
	time_t tNow = time(NULL);
	int iTimeLen = _tcsftime(strBuff.GetBuffer(40), 40, _T("%H:%M:%S "), localtime(&tNow));
	strBuff.ReleaseBuffer(iTimeLen);
#endif
	strBuff.AppendFormatV(pszFmtMsg, pArgs);

	// get around a bug in the debug device which is not capable of dumping long strings
	int i = 0;
	while (i < strBuff.GetLength()){
		OutputDebugString(strBuff.Mid(i, 1024));
		i += 1024;
	}
	va_end(pArgs);
}

void DebugHexDump(const uint8* data, UINT lenData)
{
	uint16 lenLine = 16;
	uint32 pos = 0;
	byte c = 0;
	while (pos < lenData)
	{
		CStringA line;
		CStringA single;
		line.Format("%08X ", pos);
		lenLine = min((lenData - pos), 16);
		for (int i=0; i<lenLine; i++)
		{
			single.Format(" %02X", data[pos+i]);
			line += single;
			if (i == 7)
				line += ' ';
		}
		line += CString(' ', 60 - line.GetLength());
		for (int i=0; i<lenLine; i++)
		{
			c = data[pos + i];
			single.Format("%c", (((c > 31) && (c < 127)) ? (_TUCHAR)c : '.'));
			line += single;
		}
		Debug(_T("%hs\n"), line);
		pos += lenLine;
	}
}

void DebugHexDump(CFile& file)
{
	int iSize = (int)(file.GetLength() - file.GetPosition());
	if (iSize > 0)
	{
		uint8* data = NULL;
		try{
			data = new uint8[iSize];
			file.Read(data, iSize);
			DebugHexDump(data, iSize);
		}
		catch(CFileException* e){
			TRACE("*** DebugHexDump(CFile&); CFileException\n");
			e->Delete();
		}
		catch(CMemoryException* e){
			TRACE("*** DebugHexDump(CFile&); CMemoryException\n");
			e->Delete();
		}
		delete[] data;
	}
}

LPCTSTR DbgGetFileNameFromID(const uchar* hash)
{
	CKnownFile* reqfile = theApp.sharedfiles->GetFileByID((uchar*)hash);
	if (reqfile != NULL)
		return reqfile->GetFileName();

	CPartFile* partfile = theApp.downloadqueue->GetFileByID((uchar*)hash);
	if (partfile != NULL)
		return partfile->GetFileName();

	return NULL;
}

CString DbgGetFileInfo(const uchar* hash)
{
	if (hash == NULL)
		return CString();

	CString strInfo(_T("File="));
	LPCTSTR pszName = DbgGetFileNameFromID(hash);
	if (pszName != NULL)
		strInfo += pszName;
	else
		strInfo += md4str(hash);
	return strInfo;
}

CString DbgGetBlockInfo(const Requested_Block_Struct* block)
{
	return DbgGetBlockInfo(block->StartOffset, block->EndOffset);
}

CString DbgGetBlockInfo(uint32 StartOffset, uint32 EndOffset)
{
	CString strInfo;
	strInfo.Format(_T("%u-%u (%u bytes)"), StartOffset, EndOffset, EndOffset - StartOffset + 1);

	strInfo.AppendFormat(_T(", Part %u"), StartOffset/PARTSIZE);
	if (StartOffset/PARTSIZE != EndOffset/PARTSIZE)
		strInfo.AppendFormat(_T("-%u(**)"), EndOffset/PARTSIZE);

	strInfo.AppendFormat(_T(", Block %u"), StartOffset/EMBLOCKSIZE);
	if (StartOffset/EMBLOCKSIZE != EndOffset/EMBLOCKSIZE)
	{
		strInfo.AppendFormat(_T("-%u"), EndOffset/EMBLOCKSIZE);
		if (EndOffset/EMBLOCKSIZE - StartOffset/EMBLOCKSIZE > 1)
			strInfo += _T("(**)");
	}

	return strInfo;
}

CString DbgGetBlockFileInfo(const Requested_Block_Struct* block, const CPartFile* partfile)
{
	CString strInfo(DbgGetBlockInfo(block));
	strInfo += _T("; ");
	strInfo += DbgGetFileInfo(partfile ? partfile->GetFileHash() : NULL);
	return strInfo;
}

int GetHashType(const uchar* hash)
{
	if (hash[5] == 13 && hash[14] == 110)
		return SO_OLDEMULE;
	else if (hash[5] == 14 && hash[14] == 111)
		return SO_EMULE;
 	else if (hash[5] == 'M' && hash[14] == 'L')
		return SO_MLDONKEY;
	else
		return SO_UNKNOWN;
}

LPCTSTR DbgGetHashTypeString(const uchar* hash)
{
	int iHashType = GetHashType(hash);
	if (iHashType == SO_EMULE)
		return _T("eMule");
	if (iHashType == SO_MLDONKEY)
		return _T("MLdonkey");
	if (iHashType == SO_OLDEMULE)
		return _T("Old eMule");
	ASSERT( iHashType == SO_UNKNOWN );
	return _T("Unknown");
}

CString DbgGetClientID(uint32 nClientID)
{
	CString strClientID;
	if (IsLowID(nClientID))
		strClientID.Format(_T("LowID=%u"), nClientID);
	else
		strClientID = ipstr(nClientID);
	return strClientID;
}

#define _STRVAL(o)	{_T(#o), o}

CString DbgGetDonkeyClientTCPOpcode(UINT opcode)
{
	static const struct
	{
		LPCTSTR pszOpcode;
		UINT uOpcode;
	} _aOpcodes[] =
	{
		_STRVAL(OP_HELLO),
		_STRVAL(OP_SENDINGPART),
		_STRVAL(OP_REQUESTPARTS),
		_STRVAL(OP_FILEREQANSNOFIL),
		_STRVAL(OP_END_OF_DOWNLOAD),
		_STRVAL(OP_ASKSHAREDFILES),
		_STRVAL(OP_ASKSHAREDFILESANSWER),
		_STRVAL(OP_HELLOANSWER),
		_STRVAL(OP_CHANGE_CLIENT_ID),
		_STRVAL(OP_MESSAGE),
		_STRVAL(OP_SETREQFILEID),
		_STRVAL(OP_FILESTATUS),
		_STRVAL(OP_HASHSETREQUEST),
		_STRVAL(OP_HASHSETANSWER),
		_STRVAL(OP_STARTUPLOADREQ),
		_STRVAL(OP_ACCEPTUPLOADREQ),
		_STRVAL(OP_CANCELTRANSFER),
		_STRVAL(OP_OUTOFPARTREQS),
		_STRVAL(OP_REQUESTFILENAME),
		_STRVAL(OP_REQFILENAMEANSWER),
		_STRVAL(OP_CHANGE_SLOT),
		_STRVAL(OP_QUEUERANK),
		_STRVAL(OP_ASKSHAREDDIRS),
		_STRVAL(OP_ASKSHAREDFILESDIR),
		_STRVAL(OP_ASKSHAREDDIRSANS),
		_STRVAL(OP_ASKSHAREDFILESDIRANS),
		_STRVAL(OP_ASKSHAREDDENIEDANS)
	};

	for (int i = 0; i < ARRSIZE(_aOpcodes); i++)
	{
		if (_aOpcodes[i].uOpcode == opcode)
			return _aOpcodes[i].pszOpcode;
	}
	CString strOpcode;
	strOpcode.Format(_T("0x%02x"), opcode);
	return strOpcode;
}

CString DbgGetMuleClientTCPOpcode(UINT opcode)
{
	static const struct
	{
		LPCTSTR pszOpcode;
		UINT uOpcode;
	} _aOpcodes[] =
	{
		_STRVAL(OP_EMULEINFO),
		_STRVAL(OP_EMULEINFOANSWER),
		_STRVAL(OP_COMPRESSEDPART),
		_STRVAL(OP_QUEUERANKING),
		_STRVAL(OP_FILEDESC),
		_STRVAL(OP_REQUESTSOURCES),
		_STRVAL(OP_ANSWERSOURCES),
		_STRVAL(OP_PUBLICKEY),
		_STRVAL(OP_SIGNATURE),
		_STRVAL(OP_SECIDENTSTATE),
		_STRVAL(OP_REQUESTPREVIEW),
		_STRVAL(OP_PREVIEWANSWER),
		_STRVAL(OP_MULTIPACKET),
		_STRVAL(OP_MULTIPACKETANSWER),
		_STRVAL(OP_PEERCACHE_QUERY),
		_STRVAL(OP_PEERCACHE_ANSWER),
		_STRVAL(OP_PEERCACHE_ACK),
		_STRVAL(OP_PUBLICIP_ANSWER),
		_STRVAL(OP_PUBLICIP_REQ),
		_STRVAL(OP_PORTTEST),
		_STRVAL(OP_CALLBACK),
		_STRVAL(OP_BUDDYPING),
		_STRVAL(OP_BUDDYPONG),
		_STRVAL(OP_REASKCALLBACKTCP),
		_STRVAL(OP_AICHANSWER),
		_STRVAL(OP_AICHREQUEST),
		_STRVAL(OP_AICHFILEHASHANS),
		_STRVAL(OP_AICHFILEHASHREQ)
	};

	for (int i = 0; i < ARRSIZE(_aOpcodes); i++)
	{
		if (_aOpcodes[i].uOpcode == opcode)
			return _aOpcodes[i].pszOpcode;
	}
	CString strOpcode;
	strOpcode.Format(_T("0x%02x"), opcode);
	return strOpcode;
}

#undef _STRVAL

CString DbgGetClientTCPPacket(UINT protocol, UINT opcode, UINT size)
{
	CString str;
	if (protocol == OP_EDONKEYPROT)
		str.Format(_T("protocol=eDonkey  opcode=%s  size=%u"), DbgGetDonkeyClientTCPOpcode(opcode), size);
	else if (protocol == OP_PACKEDPROT)
		str.Format(_T("protocol=Packed  opcode=%s  size=%u"), DbgGetMuleClientTCPOpcode(opcode), size);
	else if (protocol == OP_EMULEPROT)
		str.Format(_T("protocol=eMule  opcode=%s  size=%u"), DbgGetMuleClientTCPOpcode(opcode), size);
	else
		str.Format(_T("protocol=0x%02x  opcode=0x%02x  size=%u"), protocol, opcode, size);
	return str;
}

CString DbgGetClientTCPOpcode(UINT protocol, UINT opcode)
{
	CString str;
	if (protocol == OP_EDONKEYPROT)
		str.Format(_T("%s"), DbgGetDonkeyClientTCPOpcode(opcode));
	else if (protocol == OP_PACKEDPROT)
		str.Format(_T("%s"), DbgGetMuleClientTCPOpcode(opcode));
	else if (protocol == OP_EMULEPROT)
		str.Format(_T("%s"), DbgGetMuleClientTCPOpcode(opcode));
	else
		str.Format(_T("protocol=0x%02x  opcode=0x%02x"), protocol, opcode);
	return str;
}

void DebugRecv(LPCSTR pszMsg, const CUpDownClient* client, const uchar* packet, uint32 nIP)
{
	// 111.222.333.444 = 15 chars
	if (client){
		if (client != NULL && packet != NULL)
			Debug(_T("%-24hs from %s; %s\n"), pszMsg, client->DbgGetClientInfo(true), DbgGetFileInfo(packet));
		else if (client != NULL && packet == NULL)
			Debug(_T("%-24hs from %s\n"), pszMsg, client->DbgGetClientInfo(true));
		else if (client == NULL && packet != NULL)
			Debug(_T("%-24hs; %s\n"), pszMsg, DbgGetFileInfo(packet));
		else
			Debug(_T("%-24hs\n"), pszMsg);
	}
	else{
		if (nIP != 0 && packet != NULL)
			Debug(_T("%-24hs from %-15s; %s\n"), pszMsg, ipstr(nIP), DbgGetFileInfo(packet));
		else if (nIP != 0 && packet == NULL)
			Debug(_T("%-24hs from %-15s\n"), pszMsg, ipstr(nIP));
		else if (nIP == 0 && packet != NULL)
			Debug(_T("%-24hs; %s\n"), pszMsg, DbgGetFileInfo(packet));
		else
			Debug(_T("%-24hs\n"), pszMsg);
	}
}

void DebugSend(LPCSTR pszMsg, const CUpDownClient* client, const uchar* packet)
{
	if (client != NULL && packet != NULL)
		Debug(_T(">>> %-20hs to   %s; %s\n"), pszMsg, client->DbgGetClientInfo(true), DbgGetFileInfo(packet));
	else if (client != NULL && packet == NULL)
		Debug(_T(">>> %-20hs to   %s\n"), pszMsg, client->DbgGetClientInfo(true));
	else if (client == NULL && packet != NULL)
		Debug(_T(">>> %-20hs; %s\n"), pszMsg, DbgGetFileInfo(packet));
	else
		Debug(_T(">>> %-20hs\n"), pszMsg);
}

void DebugSend(LPCSTR pszOpcode, uint32 ip, uint16 port)
{
	TCHAR szIPPort[22];
	_stprintf(szIPPort, _T("%s:%u"), ipstr(ntohl(ip)), port);
	Debug(_T(">>> %-20hs to   %-21s\n"), pszOpcode, szIPPort);
}

void DebugSendF(LPCSTR pszOpcode, uint32 ip, uint16 port, LPCTSTR pszMsg, ...)
{
	va_list args;
	va_start(args, pszMsg);
	TCHAR szIPPort[22];
	_stprintf(szIPPort, _T("%s:%u"), ipstr(ntohl(ip)), port);
	CString str;
	str.Format(_T(">>> %-20hs to   %-21s; "), pszOpcode, szIPPort);
	str.AppendFormatV(pszMsg, args);
	va_end(args);
	Debug(_T("%s\n"), str);
}

void DebugRecv(LPCSTR pszOpcode, uint32 ip, uint16 port)
{
	TCHAR szIPPort[22];
	_stprintf(szIPPort, _T("%s:%u"), ipstr(ntohl(ip)), port);
	Debug(_T("%-24hs from %-21s\n"), pszOpcode, szIPPort);
}

void DebugHttpHeaders(const CStringAArray& astrHeaders)
{
	for (int i = 0; i < astrHeaders.GetCount(); i++)
	{
		const CStringA& rstrHdr = astrHeaders.GetAt(i);
		Debug(_T("<%hs\n"), rstrHdr);
	}
}

ULONGLONG GetDiskFileSize(LPCTSTR pszFilePath)
{
	static BOOL _bInitialized = FALSE;
	static DWORD (WINAPI *_pfnGetCompressedFileSize)(LPCTSTR, LPDWORD) = NULL;

	if (!_bInitialized){
		_bInitialized = TRUE;
		(FARPROC&)_pfnGetCompressedFileSize = GetProcAddress(GetModuleHandle(_T("kernel32.dll")), _TWINAPI("GetCompressedFileSize"));
	}

	// If the file is not compressed nor sparse, 'GetCompressedFileSize' returns the 'normal' file size.
	if (_pfnGetCompressedFileSize)
	{
		ULONGLONG ullCompFileSize;
		LPDWORD pdwCompFileSize = (LPDWORD)&ullCompFileSize;
		pdwCompFileSize[0] = (*_pfnGetCompressedFileSize)(pszFilePath, &pdwCompFileSize[1]);
		if (pdwCompFileSize[0] != INVALID_FILE_SIZE || GetLastError() == NO_ERROR)
			return ullCompFileSize;
	}

	// If 'GetCompressedFileSize' failed or is not available, use the default function
    WIN32_FIND_DATA fd;
    HANDLE hFind = FindFirstFile(pszFilePath, &fd);
    if (hFind == INVALID_HANDLE_VALUE)
		return 0;
	FindClose(hFind);

	return (ULONGLONG)fd.nFileSizeHigh << 32 | (ULONGLONG)fd.nFileSizeLow;
}

// Listview helper function
void GetPopupMenuPos(CListCtrl& lv, CPoint& point)
{
	// If the context menu was not opened using the right mouse button,
	// but the keyboard (Shift+F10), get a useful position for the context menu.
	if (point.x == -1 && point.y == -1)
	{
		int iIdxItem = lv.GetNextItem(-1, LVNI_SELECTED | LVNI_FOCUSED);
		if (iIdxItem != -1)
		{
			CRect rc;
			if (lv.GetItemRect(iIdxItem, &rc, LVIR_BOUNDS))
			{
				point.x = rc.left + lv.GetColumnWidth(0) / 2;
				point.y = rc.top + rc.Height() / 2;
				lv.ClientToScreen(&point);
			}
		}
		else
		{
			point.x = 16;
			point.y = 32;
			lv.ClientToScreen(&point);
		}
	}
}

void GetPopupMenuPos(CTreeCtrl& tv, CPoint& point)
{
	// If the context menu was not opened using the right mouse button,
	// but the keyboard (Shift+F10), get a useful position for the context menu.
	if (point.x == -1 && point.y == -1)
	{
		HTREEITEM hSel = tv.GetNextItem(TVI_ROOT, TVGN_CARET);
		if (hSel)
		{
			CRect rcItem;
			if (tv.GetItemRect(hSel, &rcItem, TRUE))
			{
				point.x = rcItem.left;
				point.y = rcItem.top;
				tv.ClientToScreen(&point);
			}
		}
		else
		{
			point.x = 16;
			point.y = 32;
			tv.ClientToScreen(&point);
		}
	}
}

time_t safe_mktime(struct tm* ptm)
{
	if (ptm == NULL)
		return -1;
	return mktime(ptm);
}

CString StripInvalidFilenameChars(const CString& strText, bool bKeepSpaces)
{
	LPCTSTR pszSource = strText;
	CString strDest;

	while (*pszSource != _T('\0'))
	{
		if (!(((_TUCHAR)*pszSource >= 0 && (_TUCHAR)*pszSource <= 31) ||
			// lots of invalid chars for filenames in windows :=)
			*pszSource == _T('\"') || *pszSource == _T('*') || *pszSource == _T('<')  || *pszSource == _T('>') ||
			*pszSource == _T('?')  || *pszSource == _T('|') || *pszSource == _T('\\') || *pszSource == _T('/') || 
			*pszSource == _T(':')) )
		{
			if (!bKeepSpaces && *pszSource == _T(' '))
				strDest += _T("%20");
			else
				strDest += *pszSource;
		}
		pszSource++;
	}
	return strDest;
}

CString CreateED2kLink(const CAbstractFile* pFile, bool bEscapeLink)
{
	CString strLink;
	strLink.Format(_T("ed2k://|file|%s|%u|%s|"),
		EncodeUrlUtf8(StripInvalidFilenameChars(pFile->GetFileName(), false)),
		pFile->GetFileSize(),
		EncodeBase16(pFile->GetFileHash(),16));
	if (bEscapeLink)
		strLink += _T("/");
	return strLink;
}

CString CreateHTMLED2kLink(const CAbstractFile* f)
{
	CString strCode = _T("<a href=\"") + CreateED2kLink(f) + _T("\">") + StripInvalidFilenameChars(f->GetFileName(), true) + _T("</a>");
	return strCode;
}

bool operator==(const CCKey& k1,const CCKey& k2)
{
	return !md4cmp(k1.m_key, k2.m_key);
}

bool operator==(const CSKey& k1,const CSKey& k2)
{
	return !md4cmp(k1.m_key, k2.m_key);
}

CString ipstr(uint32 nIP)
{
	// following gives the same string as 'inet_ntoa(*(in_addr*)&nIP)' but is not restricted to ASCII strings
	const BYTE* pucIP = (BYTE*)&nIP;
	CString strIP;
	strIP.ReleaseBuffer(_stprintf(strIP.GetBuffer(3+1+3+1+3+1+3), _T("%u.%u.%u.%u"), pucIP[0], pucIP[1], pucIP[2], pucIP[3]));
	return strIP;
}

CString ipstr(uint32 nIP, uint16 nPort)
{
	// following gives the same string as 'inet_ntoa(*(in_addr*)&nIP)' but is not restricted to ASCII strings
	const BYTE* pucIP = (BYTE*)&nIP;
	CString strIP;
	strIP.ReleaseBuffer(_stprintf(strIP.GetBuffer(3+1+3+1+3+1+3+1+5), _T("%u.%u.%u.%u:%u"), pucIP[0], pucIP[1], pucIP[2], pucIP[3], nPort));
	return strIP;
}

CString ipstr(LPCTSTR pszAddress, uint16 nPort)
{
	CString strIPPort;
	strIPPort.Format(_T("%s:%u"), pszAddress, nPort);
	return strIPPort;
}

CStringA ipstrA(uint32 nIP)
{
	// following gives the same string as 'inet_ntoa(*(in_addr*)&nIP)' but is not restricted to ASCII strings
	const BYTE* pucIP = (BYTE*)&nIP;
	CStringA strIP;
	strIP.ReleaseBuffer(sprintf(strIP.GetBuffer(3+1+3+1+3+1+3), "%u.%u.%u.%u", pucIP[0], pucIP[1], pucIP[2], pucIP[3]));
	return strIP;
}

bool IsDaylightSavingTimeActive(LONG& rlDaylightBias)
{
	TIME_ZONE_INFORMATION tzi;
	if (GetTimeZoneInformation(&tzi) != TIME_ZONE_ID_DAYLIGHT)
		return false;
	rlDaylightBias = tzi.DaylightBias;
	return true;
}

bool IsNTFSVolume(LPCTSTR pszVolume)
{
	DWORD dwMaximumComponentLength = 0;
	DWORD dwFileSystemFlags = 0;
	TCHAR szFileSystemNameBuffer[128];
	if (!GetVolumeInformation(pszVolume, NULL, 0, NULL, &dwMaximumComponentLength, &dwFileSystemFlags, szFileSystemNameBuffer, 128))
		return false;
	return (_tcscmp(szFileSystemNameBuffer, _T("NTFS")) == 0);
}

bool IsFileOnNTFSVolume(LPCTSTR pszFilePath)
{
	CString strRootPath(pszFilePath);
	BOOL bResult = PathStripToRoot(strRootPath.GetBuffer());
	strRootPath.ReleaseBuffer();
	if (!bResult)
		return false;
	PathAddBackslash(strRootPath.GetBuffer());
	strRootPath.ReleaseBuffer();
	return IsNTFSVolume(strRootPath);
}

bool IsAutoDaylightTimeSetActive()
{
	CRegKey key;
	if (key.Open(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\TimeZoneInformation"), KEY_READ) == ERROR_SUCCESS)
	{
		DWORD dwDisableAutoDaylightTimeSet = 0;
		if (key.QueryDWORDValue(_T("DisableAutoDaylightTimeSet"), dwDisableAutoDaylightTimeSet) == ERROR_SUCCESS)
		{
			if (dwDisableAutoDaylightTimeSet)
				return false;
		}
	}
	return true; // default to 'Automatically adjust clock for daylight saving changes'
}

bool AdjustNTFSDaylightFileTime(uint32& ruFileDate, LPCTSTR pszFilePath)
{
	if (!thePrefs.GetAdjustNTFSDaylightFileTime())
		return false;
	if (ruFileDate == 0 || ruFileDate == -1)
		return false;

	// See also KB 129574
	LONG lDaylightBias = 0;
	if (IsDaylightSavingTimeActive(lDaylightBias))
	{
		if (IsAutoDaylightTimeSetActive())
		{
			if (IsFileOnNTFSVolume(pszFilePath))
			{
				ruFileDate += lDaylightBias*60;
				return true;
			}
		}
		else
		{
			// If 'Automatically adjust clock for daylight saving changes' is disabled and
			// if the file's date is within DST period, we get again a wrong file date.
			//
			// If 'Automatically adjust clock for daylight saving changes' is disabled, 
			// Windows always reports 'Active DST'(!!) with a bias of '0' although there is no 
			// DST specified. This means also, that there is no chance to determine if a date is
			// within any DST.
			
			// Following code might be correct, but because we don't have a DST and because we don't have a bias,
			// the code won't do anything useful.
			/*struct tm* ptm = localtime((time_t*)&ruFileDate);
			bool bFileDateInDST = (ptm && ptm->tm_isdst == 1);
			if (bFileDateInDST)
			{
				ruFileDate += lDaylightBias*60;
				return true;
			}*/
		}
	}

	return false;
}


bool ExpandEnvironmentStrings(CString& rstrStrings)
{
	DWORD dwSize = ExpandEnvironmentStrings(rstrStrings, NULL, 0);
	if (dwSize == 0)
		return false;

	CString strExpanded;
	DWORD dwCount = ExpandEnvironmentStrings(rstrStrings, strExpanded.GetBuffer(dwSize-1), dwSize);
	if (dwCount == 0 || dwCount != dwSize){
		ASSERT(0);
		return false;
	}
	strExpanded.ReleaseBuffer(dwCount-1);
	rstrStrings = strExpanded;
	return true;
}

uint16 GetRandomUInt16()
{
#if RAND_MAX == 0x7fff
	// get 2 random numbers
	UINT uRand0 = rand();
	UINT uRand1 = rand();

	// NOTE: if that assert fires, you have most likely called that function *without* calling 'srand' first.
	// NOTE: each spawned thread HAS to call 'srand' for itself to get real random numbers.
	ASSERT( !(uRand0 == 41 && uRand1 == 18467) );

	return uRand0 | ((uRand1 >= RAND_MAX/2) ? 0x8000 : 0x0000);
#else
#error "Implement it!"
#endif
}

uint32 GetRandomUInt32()
{
#if RAND_MAX == 0x7fff
	//return ((uint32)GetRandomUInt16() << 16) | (uint32)GetRandomUInt16();
	// this would give the receiver the following information:
	//	random number N
	//	random number N+1 is below or greater/equal than 0x8000
	//	random number N+2
	//	random number N+3 is below or greater/equal than 0x8000

	uint32 uRand0 = GetRandomUInt16();
	srand(GetTickCount());
	uint32 uRand1 = GetRandomUInt16();
	return (uRand0 << 16) | uRand1;
#else
#error "Implement it!"
#endif
}

HWND ReplaceRichEditCtrl(CWnd* pwndRE, CWnd* pwndParent, CFont* pFont)
{
	HWND hwndNewRE = NULL;

	ASSERT( pwndRE );
	if (pwndRE)
	{
		CHAR szClassName[MAX_PATH];
		if (GetClassNameA(*pwndRE, szClassName, ARRSIZE(szClassName)) && __ascii_stricmp(szClassName, "RichEdit20W")==0)
			return NULL;

		CRect rcWnd;
		pwndRE->GetWindowRect(&rcWnd);

		DWORD dwStyle = pwndRE->GetStyle();
		dwStyle |= WS_VSCROLL | WS_HSCROLL;
		DWORD dwExStyle = pwndRE->GetExStyle();

		CString strText;
		pwndRE->GetWindowText(strText);

		UINT uCtrlID = GetWindowLong(*pwndRE, GWL_ID);

		pwndRE->DestroyWindow();
		pwndRE = NULL;

		pwndParent->ScreenToClient(&rcWnd);
		HWND hwndNewRE = CreateWindowEx(dwExStyle, RICHEDIT_CLASS, strText, dwStyle, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), pwndParent->m_hWnd, (HMENU)uCtrlID, NULL, NULL);
		if (hwndNewRE && pFont && pFont->m_hObject)
			::SendMessage(hwndNewRE, WM_SETFONT, (WPARAM)pFont->m_hObject, 0);
	}
	return hwndNewRE;
}

void TriggerPortTest(uint16 tcp, uint16 udp) {
	CString m_sTestURL;

	// do not alter the connection test, this is a manual test only. If you want to change the behaviour, use your server!
	m_sTestURL.Format(PORTTESTURL, tcp, udp , thePrefs.GetLanguageID());

	ShellOpenFile(m_sTestURL);
}

int CompareLocaleStringNoCase(LPCTSTR psz1, LPCTSTR psz2)
{
	// SDK says: The 'CompareString' function is optimized to run at the highest speed when 'dwCmpFlags' is set to 0 
	// or NORM_IGNORECASE, and 'cchCount1' and 'cchCount2' have the value -1.
	int iResult = CompareString(GetThreadLocale(), NORM_IGNORECASE, psz1, -1, psz2, -1);
	if (iResult == 0)
		return 0;
	return iResult - 2;
}

int CompareLocaleString(LPCTSTR psz1, LPCTSTR psz2)
{
	// SDK says: The 'CompareString' function is optimized to run at the highest speed when 'dwCmpFlags' is set to 0 
	// or NORM_IGNORECASE, and 'cchCount1' and 'cchCount2' have the value -1.
	int iResult = CompareString(GetThreadLocale(), 0, psz1, -1, psz2, -1);
	if (iResult == 0)
		return 0;
	return iResult - 2;
}

int __cdecl CompareCStringPtrLocaleStringNoCase(const void* p1, const void* p2)
{
	const CString* pstr1 = (const CString*)p1;
	const CString* pstr2 = (const CString*)p2;
	return CompareLocaleStringNoCase(*pstr1, *pstr2);
}

int __cdecl CompareCStringPtrLocaleString(const void* p1, const void* p2)
{
	const CString* pstr1 = (const CString*)p1;
	const CString* pstr2 = (const CString*)p2;
	return CompareLocaleString(*pstr1, *pstr2);
}

void Sort(CStringArray& astr, int (__cdecl *pfnCompare)(const void*, const void*))
{
	qsort(astr.GetData(), astr.GetCount(), sizeof(CString*), pfnCompare);
}

void AddAutoStart()
{
#ifndef _DEBUG
	RemAutoStart();
	CString strKeyName;
	strKeyName = _T("eMuleAutoStart");
    TCHAR sExeFilePath[490];
	DWORD length;
	length = ::GetModuleFileName(NULL, sExeFilePath, 490);
	if(!length)
		return;
	CString sFullExeCommand;
	sFullExeCommand.Format(_T("%s -AutoStart"), sExeFilePath);
	CRegKey mKey;
	mKey.Create(HKEY_CURRENT_USER,
		_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),
		REG_NONE,REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,	NULL,NULL);
	mKey.SetStringValue(strKeyName, sFullExeCommand);
	mKey.Close();
#endif
}

void RemAutoStart()
{
	CString strKeyName;
	strKeyName = _T("eMuleAutoStart");
	CRegKey mKey;
	mKey.Create(HKEY_CURRENT_USER,
		_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),
		REG_NONE,REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,	NULL,NULL);
	mKey.DeleteValue(strKeyName);
	mKey.Close();
}

int FontPointSizeToLogUnits(int nPointSize)
{
	HDC hDC = ::GetDC(NULL);
	if (hDC)
	{
		// convert nPointSize to logical units based on pDC
		POINT pt;
		pt.y = ::GetDeviceCaps(hDC, LOGPIXELSY) * nPointSize;
		pt.y /= 720;    // 72 points/inch, 10 decipoints/point
		pt.x = 0;
		::DPtoLP(hDC, &pt, 1);
		POINT ptOrg = { 0, 0 };
		::DPtoLP(hDC, &ptOrg, 1);
		nPointSize = -abs(pt.y - ptOrg.y);
		::ReleaseDC(NULL, hDC);
	}
	return nPointSize;
}

bool IsUnicodeFile(LPCTSTR pszFilePath)
{
	bool bResult = false;
	FILE* fp = _tfsopen(pszFilePath, _T("rb"), _SH_DENYWR);
	if (fp != NULL)
	{
		WORD wBOM = 0;
		bResult = (fread(&wBOM, sizeof(wBOM), 1, fp) == 1 && wBOM == 0xFEFF);
		fclose(fp);
	}
	return bResult;
}

bool RegularExpressionMatch(CString regexpr, CString teststring) {
	
	CAtlRegExp<> reFN;

	REParseError status = reFN.Parse( regexpr );
	if (REPARSE_ERROR_OK != status)
	{
		// Unexpected error.
		return false;
	}

	CAtlREMatchContext<> mcUrl;
	if (!reFN.Match(
		teststring,
		&mcUrl))
	{
		// Unexpected error.
		return false;
	} else {
		return true;
	}
}
