//this file is part of eMule
//Copyright (C)2002-2004 Merkur ( devs@emule-project.net / http://www.emule-project.net )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


/* 
 SHA haset basically exists of 1 Tree for all Parts (9.28MB) + n  Trees
 for all blocks (180KB) while n is the number of Parts.
 This means it is NOT a complete hashtree, since the 9.28MB is a given level, in order
 to be able to create a hashset format similar to the MD4 one.

 If the number of elements for the next level are odd (for example 21 blocks to spread into 2 hashs)
 the majority of elements will go into the left branch if the parent node was a left branch
 and into the right branch if the parent node was a right branch. The first node is always
 taken as a left branch.

Example tree:
	FileSize: 19506000 Bytes = 18,6 MB

								X (18,6)                                   MasterHash
							 /     \
						 X (18,55)   \
					/		\	       \
                   X(9,28)  x(9,28)   X (0,05MB)						   PartHashs
			   /      \    /       \        \
		X(4,75)   X(4,57) X(4,57)  X(4,75)   \

						[...............]
X(180KB)   X(180KB)  [...] X(140KB) | X(180KB) X(180KB [...]			   BlockHashs
									v
						 Border between first and second Part (9.28MB)

HashsIdentifier:
When sending hashs, they are send with a 16bit identifier which specifies its postion in the
tree (so StartPosition + HashDataSize would lead to the same hash)
The identifier basically describes the way from the top of the tree to the hash. a set bit (1)
means follow the left branch, a 0 means follow the right. The highest bit which is set is seen as the start-
postion (since the first node is always seend as left).

Example

								x                   0000000000000001
							 /     \		
						 x		    \				0000000000000011
					  /		\	       \
                    x       _X_          x 	        0000000000000110


*/

#pragma once

#define HASHSIZE		20
#define KNOWN2_MET_FILENAME		_T("known2.met")

enum EAICHStatus {
	AICH_ERROR = 0,
	AICH_EMPTY,
	AICH_UNTRUSTED,
	AICH_TRUSTED,
	AICH_VERIFIED,
	AICH_HASHSETCOMPLETE
};

class CFileDataIO;
class CKnownFile;
class CSafeMemFile;
class CPartFile;
class CUpDownClient;
/////////////////////////////////////////////////////////////////////////////////////////
///CAICHHash
class CAICHHash 
{
public:
	~CAICHHash()									{;}
	CAICHHash()										{ ZeroMemory(m_abyBuffer, HASHSIZE); }
	CAICHHash(CFileDataIO* file)					{ Read(file); }
	CAICHHash(uchar* data)							{ Read(data); }
	CAICHHash(const CAICHHash& k1)					{ *this = k1; }
	CAICHHash&	operator=(const CAICHHash& k1)		{ memcpy(m_abyBuffer, k1.m_abyBuffer, HASHSIZE); return *this; }
	friend bool operator==(const CAICHHash& k1,const CAICHHash& k2)	{ return memcmp(k1.m_abyBuffer, k2.m_abyBuffer, HASHSIZE) == 0;}
	friend bool operator!=(const CAICHHash& k1,const CAICHHash& k2)	{ return !(k1 == k2); }
	void		Read(CFileDataIO* file);
	void		Write(CFileDataIO* file) const;
	void		Read(uchar* data)					{ memcpy(m_abyBuffer, data, HASHSIZE); }
	CString		GetString() const;
	uchar*		GetRawHash()						{ return m_abyBuffer; }

	static int	GetHashSize()						{ return HASHSIZE;}
private:
	uchar m_abyBuffer[HASHSIZE];
};

/////////////////////////////////////////////////////////////////////////////////////////
///CAICHHashAlgo
class CAICHHashAlgo 
{
public:
	virtual void	Reset() = 0;
	virtual void	Add(LPCVOID pData, DWORD nLength) = 0;
	virtual void	Finish(CAICHHash& Hash) = 0;
	virtual void	GetHash(CAICHHash& Hash) = 0;
};

/////////////////////////////////////////////////////////////////////////////////////////
///CAICHHashTree
class CAICHHashTree
{
	friend class CAICHHashTree;
	friend class CAICHHashSet;
public:
	CAICHHashTree(uint32 nDataSize, bool bLeftBranch, uint32 nBaseSize);
	~CAICHHashTree();
	void			SetBlockHash(uint32 nSize, uint32 nStartPos, CAICHHashAlgo* pHashAlg);
	bool			ReCalculateHash(CAICHHashAlgo* hashalg, bool bDontReplace );
	bool			VerifyHashTree(CAICHHashAlgo* hashalg, bool bDeleteBadTrees);
	CAICHHashTree*	FindHash(uint32 nStartPos, uint32 nSize)					{uint8 buffer = 0; return FindHash(nStartPos, nSize, &buffer);}

protected:
	CAICHHashTree*	FindHash(uint32 nStartPos, uint32 nSize, uint8* nLevel);
	bool			CreatePartRecoveryData(uint32 nStartPos, uint32 nSize, CFileDataIO* fileDataOut, uint16 wHashIdent);
	void			WriteHash(CFileDataIO* fileDataOut, uint16 wHashIdent) const;
	bool			WriteLowestLevelHashs(CFileDataIO* fileDataOut, uint16 wHashIdent, bool bNoIdent = false) const;
	bool			LoadLowestLevelHashs(CFileDataIO* fileInput);
	bool			SetHash(CFileDataIO* fileInput, uint16 wHashIdent, sint8 nLevel = (-1), bool bAllowOverwrite = true);
	CAICHHashTree*	m_pLeftTree;
	CAICHHashTree*	m_pRightTree;

public:
	CAICHHash		m_Hash;
	uint32			m_nDataSize;		// size of data which is covered by this hash
	uint32			m_nBaseSize;		// blocksize on which the lowest hash is based on
	bool			m_bIsLeftBranch;	// left or right branch of the tree
	bool			m_bHashValid;		// the hash is valid and not empty
};

/////////////////////////////////////////////////////////////////////////////////////////
///CAICHUntrustedHashs
class CAICHUntrustedHash {
public:
	CAICHUntrustedHash&	operator=(const CAICHUntrustedHash& k1)		{ m_adwIpsSigning.Copy(k1.m_adwIpsSigning); m_Hash = k1.m_Hash ; return *this; }
	bool	AddSigningIP(uint32 dwIP);	

	CAICHHash				m_Hash;
	CArray<uint32, uint32>	m_adwIpsSigning;
};

/////////////////////////////////////////////////////////////////////////////////////////
///CAICHUntrustedHashs
class CAICHRequestedData {
public:
	CAICHRequestedData()	{m_nPart = 0; m_pPartFile = NULL; m_pClient= NULL;}
	CAICHRequestedData&	operator=(const CAICHRequestedData& k1)		{ m_nPart = k1.m_nPart; m_pPartFile = k1.m_pPartFile; m_pClient = k1.m_pClient; return *this; }
	uint16			m_nPart;
	CPartFile*		m_pPartFile;
	CUpDownClient*	m_pClient;
};

/////////////////////////////////////////////////////////////////////////////////////////
///CAICHHashSet
class CAICHHashSet
{
public:
	CAICHHashSet(CKnownFile*	pOwner);
	~CAICHHashSet(void);
	bool			CreatePartRecoveryData(uint32 nPartStartPos, CFileDataIO* fileDataOut, bool bDbgDontLoad = false);
	bool			ReadRecoveryData(uint32 nPartStartPos, CSafeMemFile* fileDataIn);
	bool			ReCalculateHash(bool bDontReplace = false);
	bool			VerifyHashTree(bool bDeleteBadTrees);
	void			UntrustedHashReceived(const CAICHHash& Hash, uint32 dwFromIP);
	bool			IsPartDataAvailable(uint32 nPartStartPos);
	void			SetStatus(EAICHStatus bNewValue)			{m_eStatus = bNewValue;}
	EAICHStatus		GetStatus()	const							{return m_eStatus;}
	void			SetOwner(CKnownFile* val)					{m_pOwner = val;}
	
	void			FreeHashSet();
	void			SetFileSize(uint32 nSize);
	
	CAICHHash&		GetMasterHash()						{return m_pHashTree.m_Hash;} 
	void			SetMasterHash(const CAICHHash& Hash, EAICHStatus eNewStatus);
	bool			HasValidMasterHash()				{return m_pHashTree.m_bHashValid;}

	bool			SaveHashSet();
	bool			LoadHashSet(); // only call directly when debugging

	CAICHHashAlgo*	GetNewHashAlgo();
	static void		ClientAICHRequestFailed(CUpDownClient* pClient);
	static void		RemoveClientAICHRequest(const CUpDownClient* pClient);
	static bool		IsClientRequestPending(const CPartFile* pForFile, uint16 nPart);
	static CAICHRequestedData GetAICHReqDetails(const  CUpDownClient* pClient);
	void			DbgTest();

	CAICHHashTree	m_pHashTree;
	static CList<CAICHRequestedData, CAICHRequestedData&> m_liRequestedData;
private:
	CKnownFile*		m_pOwner;
	EAICHStatus		m_eStatus;
	CArray<CAICHUntrustedHash, CAICHUntrustedHash&> m_aUntrustedHashs;
};