//////////////////////////////////////////////////////////////////////////
//				  AntiLeechClass v1 B3t4 by WiZaRd						//
//----------------------------------------------------------------------//
//																		//
//	this class is currently under development, please do NOT change		//
//	anything here until it has final state!								//
//	Please give credit to the author(s) if you use it and it'd be nice	//
//	if you'd drop a line to The_Wizard_of_DOS@web.de if you have any	//
//	suggestions/improvements.											//
//////////////////////////////////////////////////////////////////////////
//
//AntiLeechClass currently contains:
//- AntiNickThief v2.2
//- AntiModThief v2.1
//for details see below!
//
//
//>>>>>>>>>>>>>>>>>>>>>>> AntiNickThief start <<<<<<<<<<<<<<<<<<<<<<<<<<//
//The whole feature is not about detecting clients with the same name but 
//clients using a nickthief.
//Though it is annoying, this is not harmful in any way to us!
//BUT clients using a nickthief will for 99% also use other bad "features" 
//like a very low sessionupload, fakeranks or even no ratio/0 UL
//
//That's why I want to detect (and ban) them! And the nickthief is very 
//easy to detect!
//
//The AntiNickThief sends a random string to each user, if the user will 
//adapt that string to his name and send it back, we can (safely) ban him
//
//
//Revision History:
//v2.2: added a small fix to always get the mirroring clients
//		even shortly after the 1-update-per-day
//
//v2.1: use one string for all users saving lots of RAM
//		also only update once per day saving a few CPU cycles
//		and also fixed a bug causing garbage and eating up CPU (mea culpa)
//		I know that there will be a short period when we will not detect 
//		nickthieves (i.e. because of the 1-update-per-day) but sooner or later
//		they will run into it ;)
//
//v2:	send random string of random length
//		this should take care of leechers simply cutting the anti-tag 
//		if it has exactly 6 chars
//
//v1:	send random string to each user
//>>>>>>>>>>>>>>>>>>>>>>> AntiNickThief end <<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//
//
//>>>>>>>>>>>>>>>>>>>>>>> AntiModThief start <<<<<<<<<<<<<<<<<<<<<<<<<<<//
//The whole feature is not about detecting clients with the same modstring but 
//clients using a modthief.
//Though it is annoying, this is not harmful in any way to us!
//BUT clients using a modthief will for 99% also use other bad "features" 
//like a very low sessionupload, fakeranks or even no ratio/0 UL
//
//That's why I want to detect (and ban) them! And the modthief is very 
//easy to detect! Though, it will only detect stupid modthieves, the number
//of bans is impressive... :(
//
//The AntiModThief checks if a user is mirroring our string but not in correct case
//(which is supposed to be an easy way to avoid banning, but it isn't...) or using
//the modsting with a different eMule version
//
//
//Revision History:
//v2.1:	Also check for correct length (this is to ban clients appending s.thing to
//		the modstring before sending it (might be Community-Tags?)
//
//v2:	Also check for uncorrect case
//
//v1:	Check for uncorrect eMule version
//>>>>>>>>>>>>>>>>>>>>>>>> AntiModThief end <<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//

#include "stdafx.h"
#include "AntiLeech.h"
#include "updownclient.h"
#include "Preferences.h" //for GetUserNick()
#include "Opcodes.h"//cyrex2001 =>MIN2MS
#include "emule.h"//cyrex2001 =>theApp.m_strCurVersionLong and theApp.m_strModVersion 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

CAntiLeech theAntiLeechClass;

//>>> AntiNickThief
#define MIN_LENGTH	4						//min chars to use
#define MAX_LENGTH	8						//max chars to use
#if (MIN_LENGTH > MAX_LENGTH)
#error MAX_LENGTH MUST BE GREATER THAN OR EQUAL TO MIN_LENGTH
#endif
#define MAX_ADD		(MAX_LENGTH-MIN_LENGTH) //max chars to add
#define MAX_VALID	(24*60*60*1000)			//1 day expiration
#define MAX_RECHECK MIN2MS(30)				//~1 reask time
CString	CAntiLeech::m_sAntiNickThiefTag; 
uint32	CAntiLeech::m_uiAntiNickThiefCreateTimer = NULL;
CString	CAntiLeech::m_sAntiNickThiefUpdate = NULL;
//<<< AntiNickThief
//==>Modfaker by MORPH [cyrex2001]
#ifndef ANTI_MODFAKER
CString	CAntiLeech::m_sMyVersion; //>>> AntiModThief
#endif //Modfaker by MORPH
//<==Modfaker by MORPH [cyrex2001]
//>>> Global functions
void	CAntiLeech::Init()
{
	CreateAntiNickThiefTag();
}

void	CAntiLeech::UnInit()
{
	//currently not needed at all but we might need it in the future!
}
//<<< Global functions
//>>> AntiNickThief
void CAntiLeech::CreateAntiNickThiefTag()
{
	// don't run this untill it's neccessary - Stulle
	if(::GetTickCount() < m_uiAntiNickThiefCreateTimer)
		return;

	CString nick = _T("");	
	uint8 maxchar = MIN_LENGTH+rand()%MAX_ADD; //BuGFiX: d'oh - stupid me!!
	for(uint8 i = 0; i < maxchar; ++i)
	{
		if(rand()%2)
			nick.AppendFormat(_T("%c"), _T('A')+rand()%25);
		else
			nick.AppendFormat(_T("%c"), _T('a')+rand()%25);			
	}
	m_sAntiNickThiefTag.Format(_T("[%s]"), nick);
	m_uiAntiNickThiefCreateTimer = ::GetTickCount()+MAX_VALID;
}

CString	CAntiLeech::GetAntiNickThiefNick()
{
	CString ret;
	if(m_sAntiNickThiefTag.IsEmpty()) // no tag� - Stulle
		CreateAntiNickThiefTag(); // create a tag! - Stulle
	if(::GetTickCount() > m_uiAntiNickThiefCreateTimer)
		CreateAntiNickThiefTag();
	ret.Format(_T("%s %s"), thePrefs.GetUserNick(), m_sAntiNickThiefTag);
	return ret;
}

bool CAntiLeech::FindOurTagIn(const CString& tocomp)
{
	if(m_sAntiNickThiefTag.IsEmpty()) // no tag� - Stulle
		CreateAntiNickThiefTag(); // create a tag! - Stulle
	//is he mirroring our current tag?
	if(tocomp.Find(GetAntiNickThiefNick()) != -1)
		return true;

	//if we are below the timelimit, then also check for the old string!
	if(m_uiAntiNickThiefCreateTimer > ::GetTickCount() //should always be true...
		&& !m_sAntiNickThiefUpdate.IsEmpty() //just to be sure... :)
		//and we find the old string...		
		&& tocomp.Find(m_sAntiNickThiefUpdate) != -1)
		return true;

	//else he is a nice guy ;)
	return false;
}
//<<< AntiNickThief

//==>Modfaker by MORPH [cyrex2001]
#ifndef ANTI_MODFAKER
//>>> AntiModThief
bool CAntiLeech::CheckForModThief(const CUpDownClient* client)
{	
	ASSERT(client);

	CString tocomp = client->GetClientModVer();
	m_sMyVersion.Format(_T("eMule v%s [%s]"), theApp.m_strCurVersionLong, theApp.m_strModVersion);
	if(tocomp.IsEmpty())
		return false;
	CString OurMod = theApp.m_strModVersion; //cache it
	return (StrStrI(tocomp, OurMod) //uses our string
		&& (tocomp.GetLength() != OurMod.GetLength() //but not the correct length
		|| !StrStr(tocomp, OurMod) //but not in correct case		
		|| (client->GetClientSoftVer() != m_sMyVersion) //or wrong version
		));
}
//<<< AntiModThief
#endif //Modfaker by MORPH
//<==Modfaker by MORPH [cyrex2001]
