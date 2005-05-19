#pragma once
#include "MuleListCtrl.h"

class CServerList; 
class CServer;

class CServerListCtrl : public CMuleListCtrl 
{
	DECLARE_DYNAMIC(CServerListCtrl)
public:
	CServerListCtrl();
	virtual ~CServerListCtrl();

	bool	Init(CServerList* in_list);
	bool	AddServer(CServer* toadd,bool bAddToList = true);
	void	RemoveServer(CServer* todel);
	bool	AddServerMetToList(const CString& strFile);
	void	RefreshServer(const CServer* server);
	void	RemoveAllDeadServers();
	void	Hide() {ShowWindow(SW_HIDE);}
	void	Visable() {ShowWindow(SW_SHOW);}
	void	Localize();
	void	ShowServerCount();
	bool	StaticServerFileAppend(CServer *server);
	bool	StaticServerFileRemove(CServer *server);

protected:
	CServerList*	server_list;

	void SetAllIcons();
	static int CALLBACK SortProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSysColorChange();
	afx_msg	void OnColumnClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnNMLdblclk (NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnGetInfoTip(NMHDR *pNMHDR, LRESULT *pResult);
};