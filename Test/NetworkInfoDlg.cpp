#include "stdafx.h"
#include "emule.h"
#include "NetworkInfoDlg.h"
#include "RichEditCtrlX.h"
#include "OtherFunctions.h"
#include "Sockets.h"
#include "Preferences.h"
#include "ServerList.h"
#include "Server.h"
#include "kademlia/kademlia/kademlia.h"
#include "kademlia/kademlia/prefs.h"
#include "kademlia/kademlia/indexed.h"
#include "WebServer.h"
#include "clientlist.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define	PREF_INI_SECTION	_T("NetworkInfoDlg")


IMPLEMENT_DYNAMIC(CNetworkInfoDlg, CDialog)

BEGIN_MESSAGE_MAP(CNetworkInfoDlg, CResizableDialog)
END_MESSAGE_MAP()

CNetworkInfoDlg::CNetworkInfoDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(CNetworkInfoDlg::IDD, pParent)
{
}

CNetworkInfoDlg::~CNetworkInfoDlg()
{
}

void CNetworkInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NETWORK_INFO, m_info);
}

BOOL CNetworkInfoDlg::OnInitDialog()
{
#ifdef _UNICODE
	ReplaceRichEditCtrl(GetDlgItem(IDC_NETWORK_INFO), this, GetDlgItem(IDC_NETWORK_INFO_LABEL)->GetFont());
#endif
	CResizableDialog::OnInitDialog();
	InitWindowStyles(this);

	AddAnchor(IDC_NETWORK_INFO, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDOK, BOTTOM_RIGHT);
	EnableSaveRestore(PREF_INI_SECTION);

	SetWindowText(GetResString(IDS_NETWORK_INFO));
	SetDlgItemText(IDC_NETWORK_INFO_LABEL, GetResString(IDS_NETWORK_INFO));

	m_info.SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELONG(3, 3));
	m_info.SetAutoURLDetect();
	m_info.SetEventMask(m_info.GetEventMask() | ENM_LINK);

	CHARFORMAT cfDef = {0};
	CHARFORMAT cfBold = {0};

	PARAFORMAT pf = {0};
	pf.cbSize = sizeof pf;
	if (m_info.GetParaFormat(pf)){
		pf.dwMask |= PFM_TABSTOPS;
		pf.cTabCount = 4;
		pf.rgxTabs[0] = 900;
		pf.rgxTabs[1] = 1000;
		pf.rgxTabs[2] = 1100;
		pf.rgxTabs[3] = 1200;
		m_info.SetParaFormat(pf);
	}

	cfDef.cbSize = sizeof cfDef;
	if (m_info.GetSelectionCharFormat(cfDef)){
		cfBold = cfDef;
		cfBold.dwMask |= CFM_BOLD;
		cfBold.dwEffects |= CFE_BOLD;
	}

	CreateNetworkInfo(m_info, cfDef, cfBold, true);

	return TRUE;
}

void CreateNetworkInfo(CRichEditCtrlX& rCtrl, CHARFORMAT& rcfDef, CHARFORMAT& rcfBold, bool bFullInfo)
{
	CString buffer;

	///////////////////////////////////////////////////////////////////////////
	// ED2K
	///////////////////////////////////////////////////////////////////////////
	rCtrl.SetSelectionCharFormat(rcfBold);
	rCtrl << _T("eD2K ") << GetResString(IDS_NETWORK) << _T("\r\n");
	rCtrl.SetSelectionCharFormat(rcfDef);

	rCtrl << GetResString(IDS_STATUS) << _T(":\t");
	if (theApp.serverconnect->IsConnected())
		rCtrl << GetResString(IDS_CONNECTED);
	else if(theApp.serverconnect->IsConnecting())
		rCtrl << GetResString(IDS_CONNECTING);
	else 
		rCtrl << GetResString(IDS_DISCONNECTED);
	rCtrl << _T("\r\n");

	if (theApp.serverconnect->IsConnected()){
		rCtrl << GetResString(IDS_IP) << _T(":") << GetResString(IDS_PORT) << _T(":") ;
		if (theApp.serverconnect->IsLowID())
			buffer = GetResString(IDS_UNKNOWN);
		else
			buffer.Format(_T("%s:%u"), ipstr(theApp.serverconnect->GetClientID()), thePrefs.GetPort());
		rCtrl << _T("\t") << buffer << _T("\r\n");

		rCtrl << GetResString(IDS_ID) << _T(":\t");
		if (theApp.serverconnect->IsConnected()){
			buffer.Format(_T("%u"),theApp.serverconnect->GetClientID());
			rCtrl << buffer;
		}
		rCtrl << _T("\r\n");

		rCtrl << _T("\t");
		if (theApp.serverconnect->IsLowID())
			rCtrl << GetResString(IDS_IDLOW);
		else
			rCtrl << GetResString(IDS_IDHIGH);
		rCtrl << _T("\r\n");

		CServer* cur_server = theApp.serverconnect->GetCurrentServer();
		CServer* srv = cur_server ? theApp.serverlist->GetServerByAddress(cur_server->GetAddress(), cur_server->GetPort()) : NULL;
		if (srv){
			rCtrl << _T("\r\n");
			rCtrl.SetSelectionCharFormat(rcfBold);
			rCtrl << _T("eD2K ") << GetResString(IDS_SERVER) << _T("\r\n");
			rCtrl.SetSelectionCharFormat(rcfDef);

			rCtrl << GetResString(IDS_SW_NAME) << _T(":\t") << srv->GetListName() << _T("\r\n");
			rCtrl << GetResString(IDS_DESCRIPTION) << _T(":\t") << srv->GetDescription() << _T("\r\n");
			rCtrl << GetResString(IDS_IP) << _T(":") << GetResString(IDS_PORT) << _T(":\t") << srv->GetAddress() << _T(":") << srv->GetPort() << _T("\r\n");
			rCtrl << GetResString(IDS_VERSION) << _T(":\t") << srv->GetVersion() << _T("\r\n");
			rCtrl << GetResString(IDS_UUSERS) << _T(":\t") << GetFormatedUInt(srv->GetUsers()) << _T("\r\n");
			rCtrl << GetResString(IDS_PW_FILES) << _T(":\t") << GetFormatedUInt(srv->GetFiles()) << _T("\r\n");

			if (bFullInfo)
			{
				rCtrl << GetResString(IDS_IDLOW) << _T(":\t") << GetFormatedUInt(srv->GetLowIDUsers()) << _T("\r\n");
				rCtrl << GetResString(IDS_PING) << _T(":\t") << srv->GetPing() << _T(" ms\r\n");

				rCtrl << _T("\r\n");
				rCtrl.SetSelectionCharFormat(rcfBold);
				rCtrl << _T("eD2K ") << GetResString(IDS_SERVER) << _T(" ") << GetResString(IDS_FEATURES) << _T("\r\n");
				rCtrl.SetSelectionCharFormat(rcfDef);

				rCtrl << GetResString(IDS_SERVER_LIMITS) << _T(": ") << GetFormatedUInt(srv->GetSoftFiles()) << _T("/") << GetFormatedUInt(srv->GetHardFiles()) << _T("\r\n");

				if (thePrefs.IsExtControlsEnabled())
				{
					rCtrl << GetResString(IDS_SRV_TCPCOMPR) << _T(": ");
					if (srv->GetTCPFlags() & SRV_TCPFLG_COMPRESSION)
						rCtrl << GetResString(IDS_YES);
					else
						rCtrl << GetResString(IDS_NO);
					rCtrl << _T("\r\n");

					rCtrl << _T("Short tags") << _T(": ");
					if (srv->GetTCPFlags() & SRV_TCPFLG_NEWTAGS)
						rCtrl << GetResString(IDS_YES);
					else
						rCtrl << GetResString(IDS_NO);
					rCtrl << _T("\r\n");

					rCtrl << _T("Unicode") << _T(": ");
					if (srv->GetTCPFlags() & SRV_TCPFLG_UNICODE)
						rCtrl << GetResString(IDS_YES);
					else
						rCtrl << GetResString(IDS_NO);
					rCtrl << _T("\r\n");

					rCtrl << GetResString(IDS_SRV_UDPSR) << _T(": ");
					if (srv->GetUDPFlags() & SRV_UDPFLG_EXT_GETSOURCES)
						rCtrl << GetResString(IDS_YES);
					else
						rCtrl << GetResString(IDS_NO);
					rCtrl << _T("\r\n");

					rCtrl << GetResString(IDS_SRV_UDPFR) << _T(": ");
					if (srv->GetUDPFlags() & SRV_UDPFLG_EXT_GETFILES)
						rCtrl << GetResString(IDS_YES);
					else
						rCtrl << GetResString(IDS_NO);
					rCtrl << _T("\r\n");
				}
			}
		}
	}
	rCtrl << _T("\r\n");

	///////////////////////////////////////////////////////////////////////////
	// Kademlia
	///////////////////////////////////////////////////////////////////////////
	rCtrl.SetSelectionCharFormat(rcfBold);
	rCtrl << GetResString(IDS_KADEMLIA) << _T(" ") << GetResString(IDS_NETWORK) << _T("\r\n");
	rCtrl.SetSelectionCharFormat(rcfDef);
	
	rCtrl << GetResString(IDS_STATUS) << _T(":\t");
	if(Kademlia::CKademlia::isConnected()){
		ASSERT(Kademlia::CKademlia::getPrefs() != NULL);
		ASSERT(Kademlia::CKademlia::getIndexed() != NULL);
		if(Kademlia::CKademlia::isFirewalled())
			rCtrl << GetResString(IDS_FIREWALLED);
		else
			rCtrl << GetResString(IDS_KADOPEN);
		rCtrl << _T("\r\n");

		CString IP;
		IP = ipstr(ntohl(Kademlia::CKademlia::getPrefs()->getIPAddress()));
		buffer.Format(_T("%s:%i"), IP, thePrefs.GetUDPPort());
		rCtrl << GetResString(IDS_IP) << _T(":") << GetResString(IDS_PORT) << _T("\t") << buffer << _T("\r\n");

		buffer.Format(_T("%u"),Kademlia::CKademlia::getPrefs()->getIPAddress());
		rCtrl << GetResString(IDS_ID) << _T(":\t") << buffer << _T("\r\n");

		rCtrl << GetResString(IDS_BUDDY) << _T(":\t");
		switch ( theApp.clientlist->GetBuddyStatus() )
		{
			case 0:
				rCtrl << GetResString(IDS_BUDDYNONE);
				break;
			case 1:
				rCtrl << GetResString(IDS_CONNECTING);
				break;
			case 2:
				rCtrl << GetResString(IDS_CONNECTED);
				break;
		}
		rCtrl << _T("\r\n");

		if (bFullInfo)
		{
			ASSERT(Kademlia::CKademlia::getIndexed() != NULL);
			rCtrl <<  GetResString(IDS_INDEXED) << _T(":\r\n");
			buffer.Format(GetResString(IDS_KADINFO_SRC) , Kademlia::CKademlia::getIndexed()->m_totalIndexSource);
			rCtrl << buffer;
			buffer.Format(GetResString(IDS_KADINFO_KEYW), Kademlia::CKademlia::getIndexed()->m_totalIndexKeyword);
			rCtrl << buffer;
		}
	}
	else if (Kademlia::CKademlia::isRunning())
		rCtrl << GetResString(IDS_CONNECTING) << _T("\r\n");
	else
		rCtrl << GetResString(IDS_DISCONNECTED) << _T("\r\n");
	rCtrl << _T("\r\n");

	///////////////////////////////////////////////////////////////////////////
	// Web Interface
	///////////////////////////////////////////////////////////////////////////
	rCtrl.SetSelectionCharFormat(rcfBold);
	rCtrl << GetResString(IDS_WEBSRV) << _T("\r\n");
	rCtrl.SetSelectionCharFormat(rcfDef);
	rCtrl << GetResString(IDS_STATUS) << _T(":\t");
	rCtrl << (theApp.webserver->IsRunning() ? GetResString(IDS_ENABLED) : GetResString(IDS_DISABLED)) << _T("\r\n");
	if (thePrefs.GetWSIsEnabled()){
		CString count;
		count.Format(_T("%i %s"),theApp.webserver->GetSessionCount(),GetResString(IDS_ACTSESSIONS));
		rCtrl << _T("\t") << count << _T("\r\n");
		uint32 nLocalIP = theApp.serverconnect->GetLocalIP();
		rCtrl << _T("URL:\t") << _T("http://") << ipstr(nLocalIP) << _T(":") << thePrefs.GetWSPort() << _T("/\r\n");
	}
//==>Userhash in status window  [cyrex2001]
#ifdef USERHASH
	rCtrl << _T("\r\n");
	buffer.Format(_T("%s"),(LPCTSTR)(md4str((uchar*)thePrefs.GetUserHash())));
	rCtrl << GetResString(IDS_CD_UHASH) << _T("\t") << buffer.Left (16) << _T("-");
	rCtrl << _T("\r\n\t") << buffer.Mid (16,255);
#endif //Userhash in status window
//<==Userhash in status window [cyrex2001]
}
