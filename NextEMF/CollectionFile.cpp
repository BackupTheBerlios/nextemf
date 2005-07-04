//this file is part of eMule
//Copyright (C)2002 Merkur ( merkur-@users.sourceforge.net / http://www.emule-project.net )
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
#include "StdAfx.h"
#include "collectionfile.h"
#include "OtherFunctions.h"
#include "Packets.h"
#include "Ed2kLink.h"
#include "resource.h"
#include "Log.h"
#include "Kademlia/Kademlia/Entry.h"
#include "Kademlia/Kademlia/Tag.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CCollectionFile, CAbstractFile)

CCollectionFile::CCollectionFile(void)
{
}

CCollectionFile::CCollectionFile(CFileDataIO* in_data)
{
	UINT tagcount = in_data->ReadUInt32();

	for (UINT i = 0; i < tagcount; i++)
	{
		CTag* toadd = new CTag(in_data, true);
		if (toadd)
			taglist.Add(toadd);
	}
	
	CTag* pTagHash = GetTag(FT_FILEHASH);
	if(pTagHash)
		SetFileHash(pTagHash->GetHash());
	else
		ASSERT(0);
	
	// here we have two choices
	//	- if the server/client sent us a filetype, we could use it (though it could be wrong)
	//	- we always trust our filetype list and determine the filetype by the extension of the file
	//
	// if we received a filetype from server, we use it.
	// if we did not receive a filetype, we determine it by examining the file's extension.
	//
	// but, in no case, we will use the receive file type when adding this search result to the download queue, to avoid
	// that we are using 'wrong' file types in part files. (this has to be handled when creating the part files)
	const CString& rstrFileType = GetStrTagValue(FT_FILETYPE);
	SetFileName(GetStrTagValue(FT_FILENAME), false, rstrFileType.IsEmpty());
	SetFileSize(GetIntTagValue(FT_FILESIZE));
	if (!rstrFileType.IsEmpty())
	{
		if (_tcscmp(rstrFileType, _T(ED2KFTSTR_PROGRAM))==0)
		{
			CString strDetailFileType = GetFileTypeByName(GetFileName());
			if (!strDetailFileType.IsEmpty())
				SetFileType(strDetailFileType);
			else
				SetFileType(rstrFileType);
		}
		else
			SetFileType(rstrFileType);
	}

	if(!GetFileSize() || !GetFileName().Compare(_T("")))
		ASSERT(0);
}

CCollectionFile::CCollectionFile(CAbstractFile* pAbstractFile) : CAbstractFile(pAbstractFile)
{
	if(!GetTag(FT_FILEHASH))
		taglist.Add(new CTag(FT_FILEHASH, pAbstractFile->GetFileHash()));

	if(!GetTag(FT_FILESIZE))
		taglist.Add(new CTag(FT_FILESIZE, pAbstractFile->GetFileSize()));

	if(!GetTag(FT_FILENAME))
		taglist.Add(new CTag(FT_FILENAME, pAbstractFile->GetFileName()));

	if(pAbstractFile->GetFileComment().Compare(_T("")))
		if(!GetTag(FT_FILECOMMENT))
			taglist.Add(new CTag(FT_FILECOMMENT, pAbstractFile->GetFileComment()));

	if(pAbstractFile->GetFileRating())
		if(!GetTag(FT_FILERATING))
			taglist.Add(new CTag(FT_FILERATING, pAbstractFile->GetFileRating()));

	UpdateFileRatingCommentAvail();
}

bool CCollectionFile::InitFromLink(CString sLink)
{
	CED2KLink* pLink = NULL;
	CED2KFileLink* pFileLink = NULL;
	try 
	{
		pLink = CED2KLink::CreateLinkFromUrl(sLink);
		if(!pLink)
			throw GetResString(IDS_ERR_NOTAFILELINK);
		pFileLink = pLink->GetFileLink();
		if (!pFileLink)
			throw GetResString(IDS_ERR_NOTAFILELINK);
	} 
	catch (CString error) 
	{
		CString strBuffer;
		strBuffer.Format(GetResString(IDS_ERR_INVALIDLINK),error);
		LogError(LOG_STATUSBAR, GetResString(IDS_ERR_LINKERROR), strBuffer);
		return false;
	}

	taglist.Add(new CTag(FT_FILEHASH, pFileLink->GetHashKey()));
	md4cpy(m_abyFileHash, pFileLink->GetHashKey());

	taglist.Add(new CTag(FT_FILESIZE, pFileLink->GetSize()));
	SetFileSize(pFileLink->GetSize());

	taglist.Add(new CTag(FT_FILENAME, pFileLink->GetName()));
	SetFileName(pFileLink->GetName(), false, false);

	delete pLink;
	return true;
}

CCollectionFile::~CCollectionFile(void)
{
}

void CCollectionFile::WriteCollectionInfo(CFileDataIO *out_data)
{
	out_data->WriteUInt32(taglist.GetSize());

	for (int i = 0; i < taglist.GetSize(); i++)
	{
		CTag tempTag(*taglist.GetAt(i));
		tempTag.WriteNewEd2kTag(out_data, utf8strRaw);
	}
}

void CCollectionFile::UpdateFileRatingCommentAvail()
{
//	bool bOldHasComment = m_bHasComment;
//	uint32 bOldUserRatings = m_bUserRating;

	m_bHasComment = false;
	uint32 uRatings = 0;
	uint32 uUserRatings = 0;

	for(POSITION pos = m_kadNotes.GetHeadPosition(); pos != NULL; )
	{
		Kademlia::CEntry* entry = m_kadNotes.GetNext(pos);
		if (!m_bHasComment && !entry->GetStrTagValue(TAG_DESCRIPTION).IsEmpty())
			m_bHasComment = true;
		uint16 rating = entry->GetIntTagValue(TAG_FILERATING);
		if(rating!=0)
		{
			uRatings++;
			uUserRatings += rating;
		}
	}

	if(uRatings)
		m_bUserRating = uUserRatings / uRatings;

//	if (bOldHasComment != m_bHasComment || bOldUserRatings != m_bUserRating)
//		theApp.emuledlg->searchwnd->UpdateSearch(this);
}