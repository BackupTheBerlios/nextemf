/*
NextEMF - Mod -> Declaring PreProzessing Commands
[shadow2004]

include where following PreProzesses needed

//==> Präprozessoren [shadow2004]
#include ".\NextEMF\PraeProDec.h"
//<== Präprozessoren [shadow2004]
*/
#pragma once

//==> Präprozessoren [shadow2004]
#define BOLDDL						//	Aktiver Download in Fett
#define FIX_CLOSED					//	ChatFix: Close- and Submit-Button
#define USERHASH					//	show Userhash in Info
#define CACHEUP						//	Cachesize up to 4MB
#define DLWND1						//	Some Cosmetics in Downloadwindow
#define QUICKSTART					//	Quickstart
#define BOLDPROZ					//	Show Percentage in Bold
#define ASKING						//	Asking-Bug-Fix
#define CSP							//	Chunk Selection Patch
#define SourceXT					//	Show full sources when searching over Server
#define CLEANUP						//	Maella's Extended CleanUp
#define GRAPHIX						//  Graphs In 'statistics' Window Fixed
#define WEBFIX1						//	Some Fixes in Webserver by CML
#define FIX01						//	some optimizations by WiZaRd (StatisticDlg.cpp)
#define FIX02						//	Some Bug will crash while adding a friend by Libbnor & WiZaRd
#define FIX03						//	Empty Username may crash eMule - now banned by WiZaRd
#define FIX04						//	Save Preferences only if needed by WiZaRd
#define FIX05						//	Fixed some IDC by WiZaRd [shadow2004]
#define FIX06						//	Bugfix to prevent a crash via Kad by WiZaRd [shadow2004]
#define COMP_SRC					//	A better way to show Complete Sources in Shared-Dialog by WiZaRd [shadow2004]
#define SEC_IDENT					//	A better way to show Secure Identification in Statistic by WiZaRd [shadow2004]
#define KADRECON					//	Reconnect on Kad by Pawcio [shadow2004]
#define CRASHRPT					//	Crashreport by Maella
#define RELAX						//	Relax on Start-Up by WiZaRd [shadow2004]
//<== Präprozessoren [shadow2004]
//==> Crashreport [shadow2004]
#ifdef CRASHRPT
#define CRASH_MOD_VERSION _T("NextEMF v0.2 - CVS 13.01.05")
#define CRASH_DEBUG_FILE _T("http://people.freenet.de/schmalspur/eMule/crashrpt.rar")
#define CRASHRPT_POST_MAIL _T("shadow2004@buerotiger.de")
#endif
//<== Crashreport [shadow2004]

