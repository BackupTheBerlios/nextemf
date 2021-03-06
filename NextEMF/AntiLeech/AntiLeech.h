//PLEASE see notes in AntiLeech.cpp!

#pragma once
class CUpDownClient;

class CAntiLeech
{
//>>> Global functions
public:
	static	void	Init();
	static	void	UnInit();
//<<< Global functions
//>>> AntiNickThief
public:
	//this creates a new string or returns the current one
	static	CString	GetAntiNickThiefNick();

	//this returns if we have a nickthief
	static	bool	FindOurTagIn(const CString& tocomp);

private:
	//this will be automatically called
	static	void	CreateAntiNickThiefTag();

	//the string
	static	CString m_sAntiNickThiefTag;

	//the update timer
	static	uint32	m_uiAntiNickThiefCreateTimer;

	//a second string
	static	CString	m_sAntiNickThiefUpdate;
//<<< AntiNickThief

//==>Modfaker by MORPH [cyrex2001]
#ifndef ANTI_MODFAKER
//>>> AntiModThief
public:
	//this returns if we have a modthief
	static	bool	CheckForModThief(const CUpDownClient* client);

private:
	static	CString m_sMyVersion;
//<<< AntiModThief
#endif //Modfaker by MORPH
//<==Modfaker by MORPH [cyrex2001]
};

extern CAntiLeech theAntiLeechClass;
