// ReleaseWnd.cpp : implementation file
//

#include "stdafx.h"
#include "..\emule.h"
#include "ReleaseWnd.h"
#include "..\HttpDownloadDlg.h"
#include "..\otherfunctions.h"
#include "..\ini2.h"
#include "..\preferences.h"
#include "..\emuledlg.h"
#include "..\log.h"

// CReleaseWnd dialog

IMPLEMENT_DYNAMIC(CReleaseWnd, CDialog)
CReleaseWnd::CReleaseWnd(CWnd* pParent /*=NULL*/)
: CResizableDialog(CReleaseWnd::IDD, pParent)
{
	//list = NULL;
}

CReleaseWnd::~CReleaseWnd()
{
	rel_list.RemoveAll();
}

BOOL CReleaseWnd::OnInitDialog(){
	CResizableDialog::OnInitDialog();
	InitWindowStyles(this);
	AddAnchor(IDC_REL_TREE, TOP_LEFT, CSize(20, 100));
	AddAnchor(m_browser.m_hWnd, CSize(20, 0), BOTTOM_RIGHT);

	if (thePrefs.GetReleaseAutoConnect())
		m_browser.Navigate(thePrefs.GetReleaseURL(), NULL, NULL, NULL, NULL);
////	m_browser.Navigate("http://www.geeklog.net/backend/geeklog.rdf", NULL, NULL, NULL, NULL);

	AddAnchor(IDC_RELEASE_BOX, TOP_LEFT, TOP_RIGHT);  // ADDED STORMIT //
	AddAnchor(IDC_RELURL, TOP_LEFT, TOP_RIGHT);  // ADDED STORMIT //
	AddAnchor(IDC_NEWWIN, TOP_RIGHT);  // ADDED STORMIT //
	AddAnchor(IDC_BN_ADDSITE, TOP_RIGHT);  // ADDED STORMIT //	
	AddAnchor(IDC_BN_GETRELEASES, TOP_RIGHT);  // ADDED STORMIT //	
	AddAnchor(IDC_BN_DELETESITE, TOP_RIGHT);  // ADDED STORMIT //	
	AddAnchor(IDC_BN_THISSITE, TOP_RIGHT);  // ADDED STORMIT //	

	SetAdvanced(false);
	Localize();
	LoadSiteList();
	return true;
}

void CReleaseWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER, m_browser);
}


BEGIN_MESSAGE_MAP(CReleaseWnd, CResizableDialog)
	ON_BN_CLICKED(IDC_BN_GETRELEASES, OnBnClickedBnGetReleases)
	ON_BN_CLICKED(IDC_BN_THISSITE, OnBnClickedBnThisSite)
	ON_BN_CLICKED(IDC_BN_DELETESITE, OnBnClickedBnDeleteSite)
	ON_BN_CLICKED(IDC_BN_ADDSITE, OnBnClickedBnAddSite)
	ON_BN_CLICKED(IDC_BN_ADVANCED, OnBnClickedBnAdvanced)
	ON_NOTIFY(NM_DBLCLK, IDC_REL_TREE, OnNMDblclkRelTree)
END_MESSAGE_MAP()

BOOL CReleaseWnd::PreTranslateMessage(MSG* pMsg) 
{
	return CResizableDialog::PreTranslateMessage(pMsg);
}

void CReleaseWnd::UpdateReleaseList(CString strFileName, CString SiteName)
{
	CStdioFile f;
	CString strLine, strTitle, strDesc, strLink, strCategory, strSiteTitle;
	HTREEITEM siteitem, categoryitem, titleitem;
	release_data rel;
	bool m_bHasCategories = false;
	CTreeCtrl* rel_tree = (CTreeCtrl*)GetDlgItem(IDC_REL_TREE);
	int initem=0,alreadyshowed=0,inchannel=0;

	if (!f.Open(strFileName, CFile::modeRead | CFile::typeText))
		return;	
	while(f.ReadString(strLine)) {
		int i;
		strLine.Replace(_T("&lt;"),_T("<"));
		strLine.Replace(_T("&gt;"),_T(">"));
		strLine.Replace(_T("<![CDATA["),_T(""));
		strLine.Replace(_T("]]>"),_T(""));
		strLine.Replace(_T("&#38;"),_T("&"));
		strLine.Replace(_T("&38;"),_T("&"));
		strLine.Replace(_T("&038;"),_T("&"));
		strLine.Replace(_T("&#39;"),_T("'"));
		strLine.Replace(_T("&#33;"),_T("!"));	
		strLine.Replace(_T("&39;"),_T("'"));
		strLine.Replace(_T("&039;"),_T("'"));
		strLine.Replace(_T("&amp;"),_T("&"));
		strLine.Replace(_T("&quot;"),_T("\""));
		strLine.Replace(_T("\r"),_T(""));
		strLine.Replace(_T("\n"),_T(""));

		if (strLine.Find(_T("<channel>")) > -1)	// Looking for the SiteName to be Showed in order than the rss link
		{
			alreadyshowed=0;
			inchannel=1;
		}
		// enkeyDEV(Treviño) START
		if (strLine.Find(_T("<channel rdf:about=")) > -1)
		{
			alreadyshowed=0;
			inchannel=1;
		}
		// enkeyDEV(Treviño) END
		if (inchannel)
		{
			if ((i=strLine.Find(_T("<title>"))) > -1)
			{
				strSiteTitle = strLine.Mid(i+7);
				if ((i=strSiteTitle.Find(_T("</title"))) > -1)
					strSiteTitle = strSiteTitle.Left(i);
				
				HTREEITEM hCurrent = rel_tree->GetNextItem(TVI_ROOT, TVGN_CHILD);
				if(hCurrent)
				{
					while (hCurrent)
					{
					TVITEM item;
					TCHAR szText[1024];
					item.hItem = hCurrent;
					item.mask = TVIF_TEXT | TVIF_HANDLE;
					item.pszText = szText;
					item.cchTextMax = 1024;

					if(rel_tree->GetItemText(hCurrent) == strSiteTitle.Trim())
					{
						siteitem = hCurrent;
						break;
					}

					hCurrent = rel_tree->GetNextItem(hCurrent, TVGN_NEXT);

					if(!hCurrent) 
						siteitem = rel_tree->InsertItem(strSiteTitle.Trim());
					}
				}
				else siteitem = rel_tree->InsertItem(strSiteTitle.Trim());
			}
			if (strLine.Find(_T("</title>")) > -1)
				inchannel=0;

		}

		// enkeyDEV(Treviño) | END
		if (strLine.Find(_T("<item>")) > -1)
		{
			alreadyshowed=0;
			initem=1;
		}
		// enkeyDEV(Treviño) | START
		if (strLine.Find(_T("<item rdf:about=")) > -1)
		{
			alreadyshowed=0;
			initem=1;
		}
		// enkeyDEV(Treviño) | END
		if (initem)
		{
			if ((i=strLine.Find(_T("<title>"))) > -1)
			{
				strTitle = strLine.Mid(i+7);
				if ((i=strTitle.Find(_T("</title"))) > -1)
					strTitle = strTitle.Left(i);
			}

			// enkeyDEV(Treviño) - Description Reader | START
			if ((i=strLine.Find(_T("<description>"))) > -1)	
			{
				strDesc = strLine.Mid(i+13);
				if ((i=strDesc.Find(_T("</description"))) > -1)
					strDesc = strDesc.Left(i);
			}			
			// enkeyDEV(Treviño) - Description Reader | END

			// enkeyDEV(Treviño) - Category Reader | START
			if ((i=strLine.Find(_T("<category>"))) > -1)	
			{
				m_bHasCategories = true;
				strCategory = strLine.Mid(i+10);
				if ((i=strCategory.Find(_T("</category"))) > -1)
					strCategory = strCategory.Left(i);

				HTREEITEM hCurrent = rel_tree->GetNextItem(siteitem, TVGN_CHILD);
				if(hCurrent)
				{
					while (hCurrent)
					{
					TVITEM item;
					TCHAR szText[1024];
					item.hItem = hCurrent;
					item.mask = TVIF_TEXT | TVIF_HANDLE;
					item.pszText = szText;
					item.cchTextMax = 1024;

					if(rel_tree->GetItemText(hCurrent) == strCategory.Trim())
					{
						categoryitem = hCurrent;
						break;
					}

					hCurrent = rel_tree->GetNextItem(hCurrent, TVGN_NEXT);

					if(!hCurrent) 
						categoryitem = rel_tree->InsertItem(strCategory.Trim(), siteitem);
					}
				}
				else categoryitem = rel_tree->InsertItem(strCategory.Trim(), siteitem);
			}
			// enkeyDEV(Treviño) - Category Reader | END

			if ((i=strLine.Find(_T("<link>"))) > -1)
			{
				strLink = strLine.Mid(i+6);
				if ((i=strLink.Find(_T("</link"))) > -1)
					strLink = strLink.Left(i);
			}
			if (strLine.Find(_T("</item>")) > -1)
				initem=0;
		}
		if (!alreadyshowed && !initem && (strTitle.GetLength() > 0) && (strLink.GetLength() > 0))
		{
			alreadyshowed=1;

			if (strTitle.Find(_T("resented by")) == -1)
			{
				rel.site = SiteName.Trim();
				rel.title = strTitle.Trim();
				// enkeyDEV(Treviño) | START	
				if (strDesc.GetLength() > 0)
					rel.description = strDesc.Trim();
				if (strCategory.GetLength() > 0)
					rel.category = strCategory.Trim();
				if (strSiteTitle.GetLength() > 0)						
					rel.site = strSiteTitle.Trim();	// SiteName Showed in order than the rss link
				// enkeyDEV(Treviño) | END
				rel.link = strLink.Trim();
				titleitem = rel_tree->InsertItem(rel.title, m_bHasCategories ? categoryitem : siteitem);
				rel_list.SetAt(titleitem, rel);
			}
		}
	}
	f.Close();

	if(m_bHasCategories) 
		rel_tree->SortChildren(siteitem);

	rel_tree->Expand(siteitem, TVE_EXPAND);
}

int CReleaseWnd::ConnectToSite(CString URL)
{
	return 0;
}

void CReleaseWnd::OnBnClickedBnGetReleases()
{
	CString strURL;
	CString strTempFilename;
	CComboBox* list = (CComboBox*)GetDlgItem(IDC_RELURL);

	strTempFilename.Format(_T("%sreleases.rdf"), thePrefs.GetConfigDir());
	if (list->GetCount() == 0)
	{
		AddRelease(GetResString(IDS_URL)/* + _T("/rss.php")*/);
		list->SetCurSel(0);
	}
	GetDlgItem(IDC_RELURL)->GetWindowText(strURL);
	int doonce=1;
	for (int i = 0 ; i < list->GetCount() ; i ++)
	{
		list->GetLBText(i, strURL);
		if (strURL.GetLength()<1)
		{
			strURL = GetResString(IDS_URL)/* + _T("/rss.php")*/;
			GetDlgItem(IDC_RELURL)->SetWindowText(strURL);
		}
		if (strURL.Find(_T("://")) == -1)
		{
			AddLogLine(true, GetResString(IDS_INVALIDURL) );
			return;
		}
		CHttpDownloadDlg dlgDownload;
		dlgDownload.m_sURLToDownload = strURL;
		dlgDownload.m_sFileToDownloadInto = strTempFilename;
		if (dlgDownload.DoModal() != IDOK)
		{
			AddLogLine(true, GetResString(IDS_ERR_FAILEDDOWNLOADMET), strURL);
		}
		else
		{
			if (doonce==1) {
				CTreeCtrl* rel_tree = (CTreeCtrl*)GetDlgItem(IDC_REL_TREE);
				rel_tree->DeleteAllItems();
				rel_list.RemoveAll();
				doonce=0;
			}
			CString SiteName;
			SiteName = strURL.Mid(strURL.Find(_T("://"))+3);
			UpdateReleaseList(strTempFilename, SiteName);
			remove(CT2CA(strTempFilename));
		}
	}
}

void CReleaseWnd::OnBnClickedBnThisSite()
{
	CString strURL;
	CString strTempFilename;

	strTempFilename.Format(_T("%sreleases.rdf"), thePrefs.GetConfigDir());
	GetDlgItem(IDC_RELURL)->GetWindowText(strURL);
	if (strURL.Find(_T("://")) == -1)
	{
		AddLogLine(true, GetResString(IDS_INVALIDURL) );
		return;
	}
	CHttpDownloadDlg dlgDownload;
	dlgDownload.m_sURLToDownload = strURL;
	dlgDownload.m_sFileToDownloadInto = strTempFilename;
	if (dlgDownload.DoModal() != IDOK)
	{
		AddLogLine(true, GetResString(IDS_ERR_FAILEDDOWNLOADMET), strURL);
	}
	else
	{
		CTreeCtrl* rel_tree = (CTreeCtrl*)GetDlgItem(IDC_REL_TREE);
		rel_tree->DeleteAllItems();
		CString SiteName;
		SiteName = strURL.Mid(strURL.Find(_T("://"))+3);
		UpdateReleaseList(strTempFilename, SiteName);
		remove(CT2CA(strTempFilename));
	}
}

void CReleaseWnd::OnBnClickedBnDeleteSite()
{
	CComboBox* list = (CComboBox*)GetDlgItem(IDC_RELURL);
	list->DeleteString(list->GetCurSel());
	list->SetCurSel(0);
	SaveSiteList();
}

void CReleaseWnd::OnBnClickedBnAddSite()
{
	CString strURL;
	CComboBox* list = (CComboBox*)GetDlgItem(IDC_RELURL);
	GetDlgItem(IDC_RELURL)->GetWindowText(strURL);
	if (strURL.Find(_T("://")) > -1)
	{
		list->AddString(strURL);
		SaveSiteList();
	}

}

void CReleaseWnd::LoadSiteList(void)
{
	CString strNumber ;
	CString strFriend ;
	CString strURL ;
	CComboBox* list = (CComboBox*)GetDlgItem(IDC_RELURL);
	TCHAR buffer[490];
	::GetModuleFileName(0, buffer, 490);
	LPTSTR pszFileName = _tcsrchr(buffer, '\\') + 1;
	*pszFileName = '\0';
	CIni friends_ini((thePrefs.GetConfigDir() + CString(_T("releases.ini"))), _T("Releases")) ;
	int iNumberOfFriends = friends_ini.GetInt(_T("NumberOfURLs"), 0, _T("Releases")) ;
	if (iNumberOfFriends != 0)
	{
		for (int i = 0 ; i < iNumberOfFriends ; i ++)
		{
			strNumber.Format(_T("%d"), i) ;
			strFriend = friends_ini.GetString(((CString)_T("Release_") + strNumber), _T(""), _T("Releases")) ;
			if (strFriend.Trim() != _T("")) AddRelease(strFriend);
		}
		list->GetLBText(0, strURL);
	}
	if (strURL == _T(""))
	{

		AddRelease(GetResString(IDS_URL)/* + _T("/rss.php")*/);
////		AddRelease(_T("http://www.geeklog.net/backend/geeklog.rdf"));

	}
	list->SetCurSel(0);
}

void CReleaseWnd::SaveSiteList(void)
{
	CString strNumber ;
	CString strFriend ;
	CString strURL ;
	CComboBox* list = (CComboBox*)GetDlgItem(IDC_RELURL);
	TCHAR buffer[490];
	::GetModuleFileName(0, buffer, 490);
	LPTSTR pszFileName = _tcsrchr(buffer, '\\') + 1;
	*pszFileName = '\0';
	CIni friends_ini((thePrefs.GetConfigDir() + CString(_T("releases.ini"))), _T("Releases")) ;
	friends_ini.WriteInt(_T("NumberOfURLs"), list->GetCount(), _T("Releases"));
	for (int i = 0 ; i < list->GetCount() ; i ++) {
		strNumber.Format(_T("%d"), i);
		list->GetLBText(i, strURL);
		if (strURL.Trim() != _T("")) friends_ini.WriteString(((CString)_T("Release_") +strNumber), strURL.Trim(), _T("Releases"));
	}
	list->SetCurSel(0);
}

void CReleaseWnd::AddRelease(CString release)
{
	CString strList;
	CComboBox* list = (CComboBox*)GetDlgItem(IDC_RELURL);
	for (int i = 0 ; i < list->GetCount() ; i ++)
	{
		list->GetLBText(i, strList);
		if (strList == release.Trim())
			return;
	}
	list->AddString(release.Trim());
}

void CReleaseWnd::Localize() 
{
	SetDlgItemText(IDC_BN_THISSITE,GetResString(IDS_BN_THISSITE));
	SetDlgItemText(IDC_BN_GETRELEASES,GetResString(IDS_BN_GETRELEASES));
	SetDlgItemText(IDC_BN_DELETESITE,GetResString(IDS_BN_DELETESITE));
	SetDlgItemText(IDC_NEWWIN,GetResString(IDS_NEWWIN));
	SetDlgItemText(IDC_BN_ADDSITE,GetResString(IDS_BN_ADDSITE));
}

void CReleaseWnd::SetAdvanced(bool clicked)
{
	if(clicked) thePrefs.m_bAdvancedRel = !thePrefs.m_bAdvancedRel;
	GetDlgItem(IDC_BN_ADVANCED)->SetWindowText(thePrefs.m_bAdvancedRel ? _T("<< Simple") : _T("Advanced >>"));
	GetDlgItem(IDC_RELURL)->ShowWindow(thePrefs.m_bAdvancedRel ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_BN_THISSITE)->ShowWindow(thePrefs.m_bAdvancedRel ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_BN_DELETESITE)->ShowWindow(thePrefs.m_bAdvancedRel ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_BN_ADDSITE)->ShowWindow(thePrefs.m_bAdvancedRel ? SW_SHOW : SW_HIDE);
	GetDlgItem(IDC_NEWWIN)->ShowWindow(thePrefs.m_bAdvancedRel ? SW_SHOW : SW_HIDE);
}

void CReleaseWnd::OnNMDblclkRelTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	CTreeCtrl* tree = (CTreeCtrl*)GetDlgItem(IDC_REL_TREE);
	const CRBMap <HTREEITEM, release_data>::CPair* pair;
	pair = rel_list.Lookup(tree->GetSelectedItem());
	if (pair)
	{
		CString relLink = pair->m_value.link;
		if ((relLink.Left(7) == _T("http://")) || (relLink.Left(6) == _T("ftp://")))
		{
			if (IsDlgButtonChecked(IDC_NEWWIN))
			{
				ShellExecute(NULL, _T("open"), _T("iexplore.exe"), relLink, NULL, SW_SHOW); 
			}
			else
				m_browser.Navigate(relLink, NULL, NULL, NULL, NULL);
		}
		else if (relLink.Left(10) == _T("addlink://"))
		{
			AddRelease(relLink.Mid(10));
		}
		else
			AddLogLine(true, GetResString(IDS_REL_WARNING5));
	}
	*pResult = 0;
}
