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
#include "stdafx.h"
#ifdef _DEBUG
#include "DebugHelpers.h"
#endif
#include "emule.h"
#include "UpDownClient.h"
#include "FriendList.h"
#include "Clientlist.h"
#include "OtherFunctions.h"
#include "PartFile.h"
#include "ListenSocket.h"
#include "Friend.h"
#include <zlib/zlib.h>
#include "Packets.h"
#include "Opcodes.h"
#include "SafeFile.h"
#include "Preferences.h"
#include "Server.h"
#include "ClientCredits.h"
#include "IPFilter.h"
#include "UploadQueue.h"
#include "Version.h"
#include "Sockets.h"
#include "DownloadQueue.h"
#include "SearchList.h"
#include "SharedFileList.h"
#include "Kademlia/Kademlia/Kademlia.h"
#include "Kademlia/Net/KademliaUDPListener.h"
#ifndef _CONSOLE
#include "emuledlg.h"
#include "ServerWnd.h"
#include "TransferWnd.h"
#include "ChatWnd.h"
#include "CxImage/xImage.h"
#include "PreviewDlg.h"
#include "StatisticsDlg.h"
#endif
//==>Anti-Leecher [cyrex2001]
#ifdef ANTI_LEECHER
LPCTSTR CUpDownClient::TestLeecher()
	{
	CString FullEmuleName = DbgGetFullClientSoftVer();
	if (old_m_strClientSoftware != m_strClientSoftware)
		{
////////////////////////// LSD-Mod based leecher ////////////////////////////////////////////////////////////////
		if((StrStrI(m_strModVersion,_T("LSD.7c")) && StrStrI(m_strClientSoftware,_T("eMule v0.30")))||
			(StrStrI(m_strModVersion,_T("LSD.7c")) && StrStrI(m_strClientSoftware,_T("eMule v0.42")))||
			(StrStrI(m_strModVersion,_T("LSD.7c")) && StrStrI(m_strClientSoftware,_T("eMule v0.43")))||
			(StrStrI(m_strModVersion,_T("LSD.7c")) && StrStrI(m_strClientSoftware,_T("eDonkey v0.60")))||
			(StrStrI(m_strModVersion,_T("LSD.13")) && !StrStrI(m_strClientSoftware,_T("eMule v0.30")))||
			(StrStrI(m_strModVersion,_T("LSD.12")) && !StrStrI(m_strClientSoftware,_T("eMule v0.30")))||
			(StrStrI(m_strModVersion,_T("LSD.14f")) && StrStrI(m_strClientSoftware,_T("eMule v0.40f")))||
////////////////////////// sivka-Mod based leecher //////////////////////////////////////////////////////////////
			(StrStrI(m_strModVersion,_T("sivka v10e9f")) && StrStrI(m_strClientSoftware,_T("eMule v0.40f")))||
			(StrStrI(m_strModVersion,_T("sivka v12")) && StrStrI(m_strClientSoftware,_T("eMule v0.43")))||
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			(StrStrI(m_strModVersion,_T("L!ONetwork eMule v4.2")) && StrStrI(m_strClientSoftware,_T("eMule v0.43")))||
			(StrStrI(m_strModVersion,_T("Plus")) && StrStrI(m_strClientSoftware,_T("eDonkey")))||
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			StrStrI(FullEmuleName,_T("eMule v2.1f"))|| //pcsl999 Anti-leecher
			StrStrI(m_strClientSoftware,_T("Old eMule v0.60"))|| //pcsl999 Anti-leecher
			StrStrI(m_strClientSoftware,_T("eMule v5.0b"))|| //pcsl999 Anti-leecher
			StrStrI(m_strClientSoftware,_T("eDonkey v0.60"))|| //pcsl999 Anti-leecher
			StrStrI(m_strClientSoftware,_T("eMule v0.69"))|| //pcsl999 Anti-leecher
			StrStrI(m_strClientSoftware,_T("eMule v0.60"))|| //pcsl999 Anti-leecher
			StrStrI(m_strClientSoftware,_T("eMule v0.31"))|| //pcsl999 Anti-leecher
			StrStrI(m_strClientSoftware,_T("eMule v0.32"))|| //pcsl999 Anti-leecher
			StrStrI(m_strClientSoftware,_T("eMule v0.33"))|| //pcsl999 Anti-leecher
			StrStrI(m_strClientSoftware,_T("eMule v0.35"))|| //pcsl999 Anti-leecher
			StrStrI(m_strClientSoftware,_T("eMule v0.36"))|| //pcsl999 Anti-leecher
			StrStrI(m_strClientSoftware,_T("eMule v0.37"))|| //pcsl999 Anti-leecher
			StrStrI(m_strClientSoftware,_T("eMule v0.38"))|| //pcsl999 Anti-leecher
			StrStrI(m_strClientSoftware,_T("eMule v0.39"))|| //pcsl999 Anti-leecher
			(StrStrI(m_pszUsername,_T("[verycd]")) && GetClientModVer().IsEmpty())|| //pcsl999 Anti-leecher
			(StrStrI(m_pszUsername,_T("[verycd:")) && GetClientModVer().IsEmpty())|| //pcsl999 Anti-leecher
			(StrStrI(m_pszUsername,_T("[CHN]")) && GetClientModVer().IsEmpty())|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("ACAT"))|| //pcsl999 Anti-leecher
			StrStrI(FullEmuleName,_T("eMule v0.28 []")) ||  //pcsl999
			StrStrI(FullEmuleName,_T("eMule v0.29 []")) ||  //pcsl999
			StrStrI(FullEmuleName,_T("eMule v0.30 []")) ||  //pcsl999
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//                                        ModVersion
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			StrStrI(m_strModVersion,_T("          "))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("$.M@ri0")) || // OXY
			StrStrI(m_strModVersion,_T("$motty"))||
			StrStrI(m_strModVersion,_T("-=NOS=-")) || // OXY
			StrStrI(m_strModVersion,_T("-|eVortex|A2|-"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("-|eV|A4-GAMMA|-"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("/ikabot/Xman"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("00de"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("2b82 Sivka MOD"))|| // Hawkstar
			StrStrI(m_strModVersion,_T("4dow"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("5ive"))||
			StrStrI(m_strModVersion,_T("[BB]"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("[booh]"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("[e]muli")) || // OXY
			StrStrI(m_strModVersion,_T("^Plus v1.5 C^"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("A Tribut to Dimmu Borgir")) || // OXY
			StrStrI(m_strModVersion,_T("Agent Smith")) || // OXY
			StrStrI(m_strModVersion,_T("Alibaba"))|| // Hawkstar
			StrStrI(m_strModVersion,_T("Andromeda")) || // OXY
			StrStrI(m_strModVersion,_T("Another Edition")) || // OXY
			StrStrI(m_strModVersion,_T("Arcadian"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("B@d-D3vi7"))||//cyrex2001 =>04/10/01
			StrStrI(m_strModVersion,_T("badboyz"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Bad-Devil"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Bad Donkey"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Balckviperv2a"))|| //efMod
			StrStrI(m_strModVersion,_T("Bastard"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Bauner community"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("BB.v6.1"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("beta place release"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Bio-Project"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Bionic")) || // OXY
			StrStrI(m_strModVersion,_T("BL4CKF0X"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Black&White"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Black-Cat"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("BlackAngel"))||
			StrStrI(m_strModVersion,_T("Blackangel"))||
			StrStrI(m_strModVersion,_T("Blackmule"))||
			StrStrI(m_strModVersion,_T("BlackStorm")) || // OXY
			StrStrI(m_strModVersion,_T("blackviper"))||
			StrStrI(m_strModVersion,_T("BlackWombat")) || // OXY
			StrStrI(m_strModVersion,_T("Blistering Slayer")) || // OXY
			StrStrI(m_strModVersion,_T("BM.5"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("BM06"))|| // Hawkstar seen on 01.05.2004
			StrStrI(m_strModVersion,_T("Boost")) || // OXY
			StrStrI(m_strModVersion,_T("booster"))||
			StrStrI(m_strModVersion,_T("BOSS"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Brain  Mule"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Brainkiller")) || // OXY
			StrStrI(m_strModVersion,_T("Brasil Mod")) || // OXY
			StrStrI(m_strModVersion,_T("Burton")) || // OXY
			StrStrI(m_strModVersion,_T("Butthead")) || // OXY
			StrStrI(m_strModVersion,_T("BuzzFuzz")) || // OXY
			StrStrI(m_strModVersion,_T("Byte Reaper")) || // OXY
			StrStrI(m_strModVersion,_T("Chiefcooker")) || // OXY
			StrStrI(m_strModVersion,_T("CoCoon")) || // OXY
			StrStrI(m_strModVersion,_T("Codename Revolution")) || // OXY
			StrStrI(m_strModVersion,_T("Community"))|| //efMod
			StrStrI(m_strModVersion,_T("Coololli")) || // OXY
			StrStrI(m_strModVersion,_T("Cow")) || // OXY
			StrStrI(m_strModVersion,_T("crack"))||
			StrStrI(m_strModVersion,_T("Cy4n1d3")) || // OXY
			StrStrI(m_strModVersion,_T("Cyrex2001 v4.0 -"))||
			StrStrI(m_strModVersion,_T("d-unit"))||
			StrStrI(m_strModVersion,_T("d3vil"))|| // Hawkstar, seen on 09.04.2004
			StrStrI(m_strModVersion,_T("D4rkm4nX"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Da Rul0r"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Dampfer")) || // OXY
			StrStrI(m_strModVersion,_T("DaReDeVil")) || // OXY
			StrStrI(m_strModVersion,_T("DarkMule")) || // OXY
			StrStrI(m_strModVersion,_T("darkmule"))||
			StrStrI(m_strModVersion,_T("Darknessfall")) || // OXY
			StrStrI(m_strModVersion,_T("Defcon"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("DerSauger")) || // OXY
			StrStrI(m_strModVersion,_T("Diablo")) || // OXY
			StrStrI(m_strModVersion,_T("DM-")) || // OXY
			StrStrI(m_strModVersion,_T("DMX"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Dodge This")) || // OXY
			StrStrI(m_strModVersion,_T("dodgethis")) || // OXY
			StrStrI(m_strModVersion,_T("Dr Frankenstein"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Dreamcatcher")) || // OXY
			StrStrI(m_strModVersion,_T("DSL-Light"))|| // Hawkstar - Attention 1k Up Client
			StrStrI(m_strModVersion,_T("ed2kFiles"))|| // Hawkstar
			StrStrI(m_strModVersion,_T("eD@sevil")) || // OXY
			StrStrI(m_strModVersion,_T("eDevil")) || // OXY
			StrStrI(m_strModVersion,_T("eErdhörnchen")) || // OXY
			StrStrI(m_strModVersion,_T("eFish")) || // OXY
			StrStrI(m_strModVersion,_T("Egoist")) || // OXY
			StrStrI(m_strModVersion,_T("EGOmule")) || // OXY
			StrStrI(m_strModVersion,_T("egomule"))||
			StrStrI(m_strModVersion,_T("ElbenDonner"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Element"))||
			StrStrI(m_strModVersion,_T("element")) || // OXY
			StrStrI(m_strModVersion,_T("EIfen"))||
			StrStrI(m_strModVersion,_T("elfen")) ||
			StrStrI(m_strModVersion,_T("eMule Administrator Edition"))||//cyrex2001=>04/08/19
			StrStrI(m_strModVersion,_T("eMule Heaven"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Emulereactor"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("emule_reloaded"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Energy Faker")) || // OXY
			StrStrI(m_strModVersion,_T("ENTER MODSTRING"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("eraser"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("eSladevil Mod")) || // OXY
			StrStrI(m_strModVersion,_T("eTake")) || // OXY
			StrStrI(m_strModVersion,_T("ethunder"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Evian"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("eViper")) || // OXY
			StrStrI(m_strModVersion,_T("eVort"))||
			StrStrI(m_strModVersion,_T("eXeMl"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("EMLEX"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Emulereactor Community Mod"))||//cyrex2001
			StrStrI(m_strModVersion,_T("Experiment")) ||
			StrStrI(m_strModVersion,_T("Extasy"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("eZebra")) || // OXY
			StrStrI(m_strModVersion,_T("F!r@ Mule"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("father"))||
			StrStrI(m_strModVersion,_T("FCB POWAAA"))|| // Hawkstar 14.05.2004
			StrStrI(m_strModVersion,_T("Fincan"))||//cyrex2001
			StrStrI(m_strModVersion,_T("Flash")) || // OXY
			StrStrI(m_strModVersion,_T("Forza Juve")) || // OXY
			StrStrI(m_strModVersion,_T("Free Iraq"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Freeza")) || // OXY
			StrStrI(m_strModVersion,_T("FriendMod")) || // Hawkstar
			StrStrI(m_strModVersion,_T("Friendshare")) || // OXY
			StrStrI(m_strModVersion,_T("Frisee")) || // OXY
			StrStrI(m_strModVersion,_T("Funeral")) || // OXY
			StrStrI(m_strModVersion,_T("Fusspilz"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Futurmule")) || // OXY
			StrStrI(m_strModVersion,_T("G0lK4d"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("geezer"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Give'N'Get"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Go_30e_1d"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("ghostt"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Goldise-LSD"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("GolK"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("G0lK4d"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("GOLKAD"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("gt mod")) || // OXY
			StrStrI(m_strModVersion,_T("hacked by Fireblade")) || // Hawkstar
			StrStrI(m_strModVersion,_T("hardpaw"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Heartbreaker"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Heaven")) || // OXY
			StrStrI(m_strModVersion,_T("Herki")) || // OXY
			StrStrI(m_strModVersion,_T("Highjet")) || // OXY
			StrStrI(m_strModVersion,_T("html")) ||
			StrStrI(m_strModVersion,_T("http://00de"))||
			StrStrI(m_strModVersion,_T("Hurikan")) || // OXY
			StrStrI(m_strModVersion,_T("IcE-MoD"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Icemule")) || // OXY
			StrStrI(m_strModVersion,_T("IMP Delta"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("ImperatoR")) || // OXY
			StrStrI(m_strModVersion,_T("imperator"))||
			StrStrI(m_strModVersion,_T("Kalahari")) || // OXY
			StrStrI(m_strModVersion,_T("Kalitsch")) || // OXY
			StrStrI(m_strModVersion,_T("Ketamine"))||
			StrStrI(m_strModVersion,_T("Khannibal"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Killians"))||
			StrStrI(m_strModVersion,_T("killians")) || // OXY
			StrStrI(m_strModVersion,_T("Knussel")) || // OXY
			StrStrI(m_strModVersion,_T("Kocureq"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Korsan")) || // OXY
			StrStrI(m_strModVersion,_T("LegoLas"))||
			StrStrI(m_strModVersion,_T("Legolas")) || // OXY
			StrStrI(m_strModVersion,_T("LHE")) || //pcsl999
			StrStrI(m_strModVersion,_T("Light")) || //pcsl999
			StrStrI(m_strModVersion,_T("LoRe")) || // OXY
			StrStrI(m_strModVersion,_T("Losing Grip")) || // OXY
			StrStrI(m_strModVersion,_T("lovelace.10e x"))|| // Hawkstar
			StrStrI(m_strModVersion,_T("LSD-"))|| //pcsl999
			StrStrI(m_strModVersion,_T("LSD-13h"))|| //=> Fake/Leecher Version (Original LSD.13h)//by ZX
			StrStrI(m_strModVersion,_T("LSD.136"))||  //pcsl999
			StrStrI(m_strModVersion,_T("LSD.16g"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Malawi")) || // OXY
			StrStrI(m_strModVersion,_T("Master Mod")) || // OXY
			StrStrI(m_strModVersion,_T("MCD")) || // OXY
			StrStrI(m_strModVersion,_T("McDv")) || // OXY
			StrStrI(m_strModVersion,_T("Mega Mod")) || // OXY
			StrStrI(m_strModVersion,_T("Merrek")) || // OXY
			StrStrI(m_strModVersion,_T("MFCK V1b-Heartbreaker"))||//cyrex2001
			StrStrI(m_strModVersion,_T("MFCK vnov"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Miciolino")) || // OXY
			StrStrI(m_strModVersion,_T("Miroslav"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Mison"))||
			StrStrI(m_strModVersion,_T("Mister BU")) || // OXY
			StrStrI(m_strModVersion,_T("moddet")) || // OXY
			StrStrI(m_strModVersion,_T("moep")) || // OXY
			StrStrI(m_strModVersion,_T("mortimer"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("motty")) || // OXY
			StrStrI(m_strModVersion,_T("Muli_Checka's")) || // OXY
			StrStrI(m_strModVersion,_T("NEO_Tor"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Netstorm")) || // OXY
			StrStrI(m_strModVersion,_T("NoOneElseMe"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("o-kad"))|| // OXY
			StrStrI(m_strModVersion,_T("Odinz")) || // OXY
			StrStrI(m_strModVersion,_T("Omega Mod")) || // OXY
			StrStrI(m_strModVersion,_T("OS_v"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Osama BinLaden World Tour 2004"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("P&N")) || // OXY
			StrStrI(m_strModVersion,_T("Pharao")) || // OXY
			StrStrI(m_strModVersion,_T("Pille"))|| // Hawkstar seen on 01.05.2004
			StrStrI(m_strModVersion,_T("Power Mule")) || // OXY
			StrStrI(m_strModVersion,_T("P.P.R.C. Burner"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Proj3ct"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Pruna")) || // OXY
			StrStrI(m_strModVersion,_T("Psycho"))||
			StrStrI(m_strModVersion,_T("Pulp Fiction")) || // OXY
			StrStrI(m_strModVersion,_T("pwNd muLe"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("PyRiX"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Raffgier")) || // OXY
			StrStrI(m_strModVersion,_T("Rammstein")) || // OXY
			StrStrI(m_strModVersion,_T("Rappi"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Raptor")) || // OXY
			StrStrI(m_strModVersion,_T("Rasmoe")) || // OXY
			StrStrI(m_strModVersion,_T("Raziboom"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Relikt²")) || // OXY
			StrStrI(m_strModVersion,_T("Rennschwein")) || // OXY
			StrStrI(m_strModVersion,_T("Retestrak"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Rocket"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Rul0r"))||
			StrStrI(m_strModVersion,_T("RVB")) || // OXY
			StrStrI(m_strModVersion,_T("ryaKigaM")) || // OXY
			StrStrI(m_strModVersion,_T("ryKigaM")) || // OXY
			StrStrI(m_strModVersion,_T("S.I.B"))|| // Hawkstar
			StrStrI(m_strModVersion,_T("Saftysewombat")) || // OXY
			StrStrI(m_strModVersion,_T("Sauger")) || // OXY
			StrStrI(m_strModVersion,_T("SC-EMULE"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("SC-v2.5"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Schlumpmule")) || // OXY
			StrStrI(m_strModVersion,_T("Senfei"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Serengeti")) || // OXY
			StrStrI(m_strModVersion,_T("www.serverclient.net")) || // Hawkstar - Attention 1k Up Client
			StrStrI(m_strModVersion,_T("SEsL"))|| //Hawkstar: Ban Sternhase Mods
			StrStrI(m_strModVersion,_T("SF-TG"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Shadowvamp")) || // OXY
			StrStrI(m_strModVersion,_T("Sharedifferent"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Sherlock")) || // OXY
			StrStrI(m_strModVersion,_T("siv8b1a-eklmn2sr2"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("sivka vx9c2"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Sonic")) || // OXY
			StrStrI(m_strModVersion,_T("Soul Food")) || // OXY
			StrStrI(m_strModVersion,_T("Speed-Unit")) || // OXY
			StrStrI(m_strModVersion,_T("SpeedLoad")) || // OXY
			StrStrI(m_strModVersion,_T("SpeedMule")) || // OXY
			StrStrI(m_strModVersion,_T("Sphinx")) || // OXY
			StrStrI(m_strModVersion,_T("Spider²")) || // OXY
			StrStrI(m_strModVersion,_T("Splatter_"))||
			StrStrI(m_strModVersion,_T("spydev"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Stoneheng")) || // OXY
			StrStrI(m_strModVersion,_T("StrangeThing"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Stufftown")) || // OXY
			StrStrI(m_strModVersion,_T("STV v1f"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Supaleecher")) || // OXY
			StrStrI(m_strModVersion,_T("SweetMule"))||
			StrStrI(m_strModVersion,_T("sweetmule"))|| // Hawkstar
			StrStrI(m_strModVersion,_T("t1m? t0 sh4r?")) || // OXY
			StrStrI(m_strModVersion,_T("The Centre")) || // OXY
			StrStrI(m_strModVersion,_T("The G@m3r´s Edition")) || // OXY
			StrStrI(m_strModVersion,_T("Thubb & Stinger")) ||  //pcsl999
			StrStrI(m_strModVersion,_T("Thubb&Stinger")) || // OXY
			StrStrI(m_strModVersion,_T("Thunder")) || // OXY
			StrStrI(m_strModVersion,_T("Torni ArGo")) || // OXY
			StrStrI(m_strModVersion,_T("Tornisemule")) || // OXY
			StrStrI(m_strModVersion,_T("toXMule")) || // OXY
			StrStrI(m_strModVersion,_T("Tycoon")) || // OXY
			StrStrI(m_strModVersion,_T("Typhon")) || // OXY
			StrStrI(m_strModVersion,_T("UnDrGRoUnd")) || // OXY
			StrStrI(m_strModVersion,_T("Unknow Poison")) || // OXY
			StrStrI(m_strModVersion,_T("UpTempo")) || // OXY
			StrStrI(m_strModVersion,_T("Up-2-No-Limits"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("URL nicht verf"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("VIPERKAD"))|| // Hawkstar
			StrStrI(m_strModVersion,_T("ViVA JuVE"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Watson")) || // OXY
			StrStrI(m_strModVersion,_T("what now"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Windozeur"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Wolly"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("www"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("www.serverclient.net")) || // Hawkstar - Attention 1k Up Client
			StrStrI(m_strModVersion,_T("X-Crash")) || // OXY
			StrStrI(m_strModVersion,_T("X-MorPh v.03"))||//cyrex2001
			StrStrI(m_strModVersion,_T("X-Mule Plus Edition")) || // OXY
			StrStrI(m_strModVersion,_T("X-treme")) || // pcsl999
			StrStrI(m_strModVersion,_T("xmichal"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("xMule"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("XYJ"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Yaotzin")) || // OXY
			StrStrI(m_strModVersion,_T("Zone Reloaded"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Zulu")) || // Hawkstar
			StrStrI(m_strModVersion,_T("ZZ_Tor"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("$motty"))||
			StrStrI(m_strModVersion,_T("|eV|")) || // OXY
			StrStrI(m_strModVersion,_T("|X|")) || // OXY
			(StrStrI(m_strModVersion,_T("NextEMF")) && !StrStrI(m_strClientSoftware,_T("eMule v0.44d")))||//cyrex2001 04/12/01

			(StrStrI(m_strModVersion,_T("EastShare")) && StrStrI(m_strClientSoftware,_T("0.29")))||
			StrStrI(m_strModVersion,_T("EastShare")) && (StrStrI(m_strClientSoftware,_T("0.30")) && !StrStrI(m_strModVersion,_T("EastShare v1")))||
			(StrStrI(m_strModVersion,_T("LSD.7c")) && !StrStrI(m_strClientSoftware,_T("27")))||
			(StrStrI(m_strModVersion,_T("Morph")) && StrStrI(m_strModVersion,_T("Max")))||
			(!GetClientModVer().IsEmpty() && StrStrI(m_strClientSoftware,_T("edonkey")))||
			((GetVersion()>589) && (GetSourceExchangeVersion()>0) && (GetClientSoft()==51)) //LSD, edonkey user with eMule property
			)
			{
			old_m_strClientSoftware = m_strClientSoftware;
			return _T("Bad MODSTRING detected");
			}
			if (StrStrI(m_strModVersion,_T("NextEMF")) && (m_strModVersion.GetLength()>13))
			{
				old_m_strClientSoftware = m_strClientSoftware;
				return _T("MODFAKER with lenght>12 detected");
			}
		}
	else if (old_m_pszUsername != m_pszUsername)
		{
		if (
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//                                        Username
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			(StrStrI(m_pszUsername,_T("LSD16g")) && GetClientModVer().IsEmpty()) || //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T(" Leecher"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("!Kill.All.The.White.Men!"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("$GAM3R$"))||
			StrStrI(m_pszUsername,_T("$motty"))||
			StrStrI(m_pszUsername,_T("$WAREZ$"))||
			StrStrI(m_pszUsername,_T("00de.de"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("[+Element"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("[Cy4n1d3]"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("[eM-C]"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("[FNF]"))|| //Bowlfish Anti-leecher
			StrStrI(m_pszUsername,_T("[FNF["))|| //Bowlfish Anti-leecher
			StrStrI(m_pszUsername,_T("[FZE]"))|| //=> Leecher MOD (Community)//by ZX
			StrStrI(m_pszUsername,_T("[FZR]"))|| //=> Leecher MOD (Community)//by ZX
			StrStrI(m_pszUsername,_T("[RAMMSTEIN"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("[TB]"))|| //Bowlfish Anti-leecher
			StrStrI(m_pszUsername,_T("[toXic]"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("[USS]"))|| //by ZX
			StrStrI(m_pszUsername,_T("[ÖÐ¹úÂ¿]"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("00de.de"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("alzamon"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("BuzzFuzz"))||
			StrStrI(m_pszUsername,_T("celinesexy"))||
			StrStrI(m_pszUsername,_T("chaos59.com"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("Chief"))||
			StrStrI(m_pszUsername,_T("Chosen One"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("CommunityDark"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("darkmule"))||
			StrStrI(m_pszUsername,_T("darkzone-world.com"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("Dodge"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("dodgethis"))||
			StrStrI(m_pszUsername,_T("Emulereactor"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("emule's Cow"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("emule-element.tk"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("emule-speed"))||
			StrStrI(m_pszUsername,_T("eMule0.30e Element"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("eMule@#$"))||
			StrStrI(m_pszUsername,_T("energyfaker"))||
			StrStrI(m_pszUsername,_T("etocs.de"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("eVortex"))||
			StrStrI(m_pszUsername,_T("G@m3r"))||
			StrStrI(m_pszUsername,_T("G@m3rs Edit"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("G@m3r´s Edit"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("Gate-eMule"))||
			StrStrI(m_pszUsername,_T("happyfree.ch.vu"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("haspepapa-welt.de"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("Ketamine"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("Killians"))||
			StrStrI(m_pszUsername,_T("Kocureq"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("Leecha"))||
			StrStrI(m_pszUsername,_T("leecha"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("Leeching"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("Lossius"))|| //Hawkstar 14.05.2004
			StrStrI(m_pszUsername,_T("megaemule.6x.to"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("miciolino.de"))||//by ZX
			StrStrI(m_pszUsername,_T("MISON"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("Odin"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("phArAo"))||
			StrStrI(m_pszUsername,_T("pubsman"))||
			StrStrI(m_pszUsername,_T("R@ppi"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("RAMMSTEIN"))||//
			StrStrI(m_pszUsername,_T("Reverse"))||
			StrStrI(m_pszUsername,_T("Rocket.T35.com"))||
			StrStrI(m_pszUsername,_T("Shadowmule.de"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("Sharedifferent"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("Speed-Unit"))||
			StrStrI(m_pszUsername,_T("Stern"))|| //Hawkstar 02.06.2004 // Ban Sternhase for GPL-Break
			StrStrI(m_pszUsername,_T("Turbomod"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("world-of-eselshare.com"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("|eVorte|X|"))||
			StrStrI(m_pszUsername,_T("§Ú·R§¯Å]"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("haspepapa-welt.de"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("alzamon"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("megaemule.6x.to"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("happyfree.ch.vu"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("world-of-eselshare.com"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("etocs.de"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("CommunityDark"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("Leeching"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("Choose One"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("emule's Cow"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T(" Leecher"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("Kocureq"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("Dodge"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("Shadowmule.de"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("[+Element"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("[Cy4n1d3]"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("Sharedifferent"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("!Kill.All.The.White.Men!"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("§Ú·R§¯Å]"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("eMule0.30e Element"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("Chosen One"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("00de.de"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("Turbomod"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("darkzone-world.com"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("[eM-C]"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("$GAM3R$"))||
			StrStrI(m_pszUsername,_T("G@m3r"))||
			StrStrI(m_pszUsername,_T("$WAREZ$"))||
			StrStrI(m_pszUsername,_T("RAMMSTEIN"))||
			StrStrI(m_pszUsername,_T("Leecha"))||
			StrStrI(m_pszUsername,_T("energyfaker"))||
			StrStrI(m_pszUsername,_T("darkmule"))||
			StrStrI(m_pszUsername,_T("phArAo"))||
			StrStrI(m_pszUsername,_T("dodgethis"))||
			StrStrI(m_pszUsername,_T("Reverse"))||
			StrStrI(m_pszUsername,_T("eVortex"))||
			StrStrI(m_pszUsername,_T("|eVorte|X|"))||
			StrStrI(m_pszUsername,_T("Chief"))||
			StrStrI(m_pszUsername,_T("$motty"))||
			StrStrI(m_pszUsername,_T("emule-speed"))||
			StrStrI(m_pszUsername,_T("celinesexy"))||
			StrStrI(m_pszUsername,_T("Gate-eMule"))||
			StrStrI(m_pszUsername,_T("BuzzFuzz"))||
			StrStrI(m_pszUsername,_T("Speed-Unit"))||
			StrStrI(m_pszUsername,_T("Killians"))||
			StrStrI(m_pszUsername,_T("pubsman"))||
			StrStrI(m_pszUsername,_T("eMule@#$"))||
			(StrStrI(m_pszUsername,_T("emule")) && StrStrI(m_pszUsername,_T("booster")))
			)
			{
			old_m_pszUsername = m_pszUsername;
			return _T("Bad USERNAME detected");
			}
		}else if (IsLeecher())
			return _T("Allready Known");
		return NULL;
	}

LPCTSTR CUpDownClient::BadComunity()
	{
	if (old_m_pszUsername1 != m_pszUsername)
		{
		if (
			StrStrI(m_pszUsername,_T("[Cy4n1d3]"))||
			StrStrI(m_pszUsername,_T("[D4rkMul3]"))||
			StrStrI(m_pszUsername,_T("[LH 2.2]"))||
			StrStrI(m_pszUsername,_T("[Li0Network.Net]"))||
			StrStrI(m_pszUsername,_T("[LUL]"))||
			StrStrI(m_pszUsername,_T("[LioNetwork]"))||
			StrStrI(m_pszUsername,_T("[LionHeart Network]"))||
			StrStrI(m_pszUsername,_T("[Li0Network.Net]"))||
			StrStrI(m_pszUsername,_T("[Neo]"))||
			StrStrI(m_pszUsername,_T("[PrimeEvil]"))||
			StrStrI(m_pszUsername,_T("[TWpower]"))||
			StrStrI(m_pszUsername,_T("[ThiefDEF]"))||
			StrStrI(m_pszUsername,_T("[V]"))||
			StrStrI(m_pszUsername,_T("[WarezFaw]"))||
			StrStrI(m_pszUsername,_T("[VeryCDhex2002]"))||
			StrStrI(m_pszUsername,_T("[ih.2y.net]"))||
			StrStrI(m_pszUsername,_T("[lh.2y.net]"))||
			StrStrI(m_pszUsername,_T("[lionheart"))||
			StrStrI(m_pszUsername,_T("[verycd]"))||
			StrStrI(m_pszUsername,_T("[VeryCDhex2002]"))||
			StrStrI(m_pszUsername,_T("[VeryCDÂÛÌ³ID]"))||
			StrStrI(m_pszUsername,_T("[¨¯¸¦]"))||
			StrStrI(m_pszUsername,_T("[´ëÇÑ¹Î±¹]"))||
			StrStrI(m_pszUsername,_T("[Æj¸]]"))||
			StrStrI(m_pszUsername,_T("[ÖÐ¹úÂ¿]"))||
			StrStrI(m_pszUsername,_T("[´òµ¹³ÂË®±â]"))||
			StrStrI(m_pszUsername,_T("[ÅéÄã¸öÁü¶£ßËÑ½]"))||
			StrStrI(m_pszUsername,_T("[acat]")))
			{
			old_m_pszUsername1 = m_pszUsername;
			return _T("Bad Comunity detected");
			}
		}else if (IsBadComunity())
			return _T("Allready Known");
		return NULL;
	}

LPCTSTR CUpDownClient::GplBreaker()
	{
	CString FullEmuleName = DbgGetFullClientSoftVer();
	if ((old_m_pszUsername2 != m_pszUsername)||(old_m_strClientSoftware1 != m_strClientSoftware))
		{
		if (
			StrStrI(FullEmuleName,_T("eMule v0.42e [V]")) ||
			StrStrI(m_strModVersion,_T("LSD.16g"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("LioNetwork"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Li0Network"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("LH 1."))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("LH 2."))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("LHEMULE")) ||
			StrStrI(m_strModVersion,_T("Mison"))||
			StrStrI(m_pszUsername,_T("EMULE-CLIENT"))|| //pcsl999 Anti-leecher
			(StrStrI(m_pszUsername,_T("emule.com.cn")) && GetClientModVer().IsEmpty())|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("HARDMULE"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("POWERMULE"))|| //pcsl999 Anti-leecher
			StrStrI(m_pszUsername,_T("BIONIC"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("HARDMULE"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("BIONIC"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("ANONYMER ALKOHOLIKER"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("I'M WITH STUPID"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("00de"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("¦n¤ÍÁp·ù"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("°­Å[³¡¶¤"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("°­Å[Áp·ù"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("§¯Å]"))||
			StrStrI(m_strModVersion,_T("Power Mule"))||
			StrStrI(m_strModVersion,_T("Fincan"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Cy4n1d3"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("Kalitsch"))||
			StrStrI(m_strModVersion,_T("LSD.13b.XYJ.o3f"))|| //pcsl999 Anti-leecher
			StrStrI(m_strModVersion,_T("POWERMULE")))
			{
			old_m_pszUsername2 = m_pszUsername;
			old_m_strClientSoftware1 = m_strClientSoftware;
			return _T("GPL-Breaker detected");
			}
		}else if (IsGplBreaker())
			return _T("Allready Known");
		return NULL;
	}
#endif //Anti-Leecher
//<==Anti-Leecher [cyrex2001]