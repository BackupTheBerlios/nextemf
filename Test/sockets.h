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

//==> Spooky Mode ConChecker [cyrex2001]
#ifdef CONCHECKER //>>>WiZaRd: Spooky Mode ConChecker [eWombat] 
#include ".\NextEMF\ConChecker.h" 
#endif //<<<WiZaRd: Spooky Mode ConChecker [eWombat] 
//<== Spooky Mode ConChecker [cyrex2001]

#define CS_FATALERROR	-5
#define CS_DISCONNECTED	-4
#define CS_SERVERDEAD	-3
#define	CS_ERROR		-2
#define CS_SERVERFULL	-1
#define	CS_NOTCONNECTED	0
#define	CS_CONNECTING	1
#define	CS_CONNECTED	2
#define	CS_WAITFORLOGIN	3
#define CS_WAITFORPROXYLISTENING 4 // deadlake PROXYSUPPORT

//==> Spooky Mode ConChecker [cyrex2001]
#ifdef CONCHECKER //>>>WiZaRd: Spooky Mode ConChecker [eWombat] 
#define MODE_NONE			0
#define MODE_SERVER			1
#define MODE_SPOOKY			2
#endif //<<<WiZaRd: Spooky Mode ConChecker [eWombat] 
//<== Spooky Mode ConChecker [cyrex2001]

#define CS_RETRYCONNECTTIME  30 // seconds

class CServerList;
class CUDPSocket;
class CServerSocket;
class CServer;
class Packet;

class CServerConnect
{
public:
	CServerConnect(CServerList* in_serverlist);
	~CServerConnect();
	void	ConnectionFailed(CServerSocket* sender);
	void	ConnectionEstablished(CServerSocket* sender);
	
	void	ConnectToAnyServer() {ConnectToAnyServer(0,true,true);}
	void	ConnectToAnyServer(uint32 startAt,bool prioSort=false,bool isAuto=true);
	void	ConnectToServer(CServer* toconnect, bool multiconnect=false);
	void	StopConnectionTry();
	static  VOID CALLBACK RetryConnectTimer(HWND hWnd, UINT nMsg, UINT nId, DWORD dwTime);

	void	CheckForTimeout();
	void	DestroySocket(CServerSocket* pSck);	// safe socket closure and destruction
	bool	SendPacket(Packet* packet,bool delpacket = true, CServerSocket* to = 0);
	bool	IsUDPSocketAvailable() const { return udpsocket != NULL; }
	bool	SendUDPPacket(Packet* packet,CServer* host, bool delpacket = false );
	void	KeepConnectionAlive();
	bool	Disconnect();
	bool	IsConnecting()	{return connecting;}
//==> Spooky Mode [cyrex2001]
#ifdef SPOOKY // Fenderman - Spooky Mode [eWombat] 
    bool    IsConnected(bool bSpooky=false)    {if (!bSpooky) return connected;return (bool)(connected || IsSpooky());} 
#else 
	bool	IsConnected()	{return connected;}
#endif // Fenderman - Spooky Mode [eWombat] 
//<== Spooky Mode [cyrex2001]
//==> Spooky Mode ConChecker [cyrex2001]
#ifdef CONCHECKER //>>>WiZaRd: Spooky Mode ConChecker [eWombat] 
    uint32    GetClientID()    {return IsSpooky()?m_pConchecker->GetID():clientid;}
	CServer*	GetGhostServer()	{return m_pGhostServer;} //<<< WiZaRd 4 Spooky Mode 
#else
	uint32	GetClientID()		{return clientid;}
#endif //<<<WiZaRd: Spooky Mode ConChecker [eWombat] 
//<== Spooky Mode ConChecker [cyrex2001]
	CServer*	GetCurrentServer();
	uint32	clientid;
	uint8	pendingConnects;
	uint32	m_curuser;

	bool	IsLowID();
	void	SetClientID(uint32 newid);
	bool	IsLocalServer(uint32 dwIP, uint16 nPort);
	void	TryAnotherConnectionrequest();
	bool	IsSingleConnect()	{return singleconnecting;}
	void	InitLocalIP();
	uint32	GetLocalIP()		{return m_nLocalIP;}

private:
	bool	connecting;
	bool	singleconnecting;
	bool	connected;
	uint8	max_simcons;
	uint32 lastStartAt;
	CServerSocket*	connectedsocket;
	CServerList*	used_list;
	CUDPSocket*		udpsocket;
	CPtrList		m_lstOpenSockets;	// list of currently opened sockets
	UINT			m_idRetryTimer;
	uint32	m_nLocalIP;

	CMap<ULONG ,ULONG&,CServerSocket*,CServerSocket*> connectionattemps;
//==> Spooky Mode [cyrex2001]
#ifdef SPOOKY // Fenderman - Spooky Mode [eWombat] 
public: 
    bool ConnectSpooky(void); 
    void DisconnectSpooky(void) {ResetSpooky();} 

    void    CreateGhostServer(void); 
    bool    IsSpooky() {return m_bSpooky;} 
    uint32    GetGhostServerIP()        {return m_dwGhostServerIP;} 
    uint16    GetGhostServerPort()    {return m_dwGhostServerPort;} 
    //uint32    GetGhostIP()            {return m_pConchecker->GetIP();} 
    //uint16    GetGhostID()            {return m_pConchecker->GetID();}
	int		GetCurrentMode(void);
protected: 
    void    ResetSpooky(bool bInform=true); 
    bool     m_bSpooky; 
    uint32     m_dwGhostServerIP; 
    uint16     m_dwGhostServerPort; 
    CString     m_strGhostServerFullIP; 
    uint32     m_dwGhostID; 
    uint32     m_dwGhostIP; 
    uint32     m_nConErrorCount; 
    CString     m_strGhostFullIP; 
    CServer     *m_pGhostServer;
#endif // Fenderman - Spooky Mode [eWombat] 
//<== Spooky Mode [cyrex2001]
//==> Spooky Mode ConChecker [cyrex2001]
#ifdef CONCHECKER //>>>WiZaRd: Spooky Mode ConChecker [eWombat] 
public: 
    void    SetConChecker(WombatAgent::CConChecker *pChecker)    {m_pConchecker = pChecker;} 
    uint32    GetGhostIP()            {return m_pConchecker->GetIP();} 
    uint16    GetGhostID()            {return m_pConchecker->GetID();} 
protected: 
    WombatAgent::CConChecker *m_pConchecker; 
#endif //<<<WiZaRd: Spooky Mode ConChecker [eWombat] 
//<== Spooky Mode ConChecker [cyrex2001]
};