#pragma once

#define	WEBSVC_GEN_URLS		0x0001
#define	WEBSVC_FILE_URLS	0x0002

class CWebServices
{
public:
	CWebServices();

	CString GetDefaultServicesFile() const;
	int ReadAllServices();
	void RemoveAllServices();

	int GetFileMenuEntries(CMenu& rMenu) { return GetAllMenuEntries(rMenu, WEBSVC_FILE_URLS); }
	int GetGeneralMenuEntries(CMenu& rMenu) { return GetAllMenuEntries(rMenu, WEBSVC_GEN_URLS); }
	int GetAllMenuEntries(CMenu& rMenu, DWORD dwFlags = WEBSVC_GEN_URLS | WEBSVC_FILE_URLS);
	bool RunURL(const CAbstractFile* file, UINT uMenuID);
	void Edit();

protected:
	struct SEd2kLinkService
	{
		UINT uMenuID;
		CString strMenuLabel;
		CString strUrl;
		BOOL bFileMacros;
	};
	CArray<SEd2kLinkService, SEd2kLinkService> m_aServices;
	time_t m_tDefServicesFileLastModified;
};

extern CWebServices theWebServices;
