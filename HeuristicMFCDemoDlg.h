#pragma once
#include "afxcmn.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <vector>
#include <string>
#include <queue>
#include <stack>
#include <map>
#include <algorithm>
#include <random>

#define MY_TIMER_ID 0x1a
#define WM_UPDATE_LIST (WM_USER + 0xa0)
#define WM_ALGORITHM_FINISHED (WM_USER + 0xa1)

class Node
{
public:
	Node() : h(0), g(0), x(0), y(0), HashNum(0)
	{
		memset(state, 0, sizeof(state));
	}

	bool operator<(const Node& b) const;
	void SetState(const int s1[3][3]);

	int state[3][3];
	int h, g, x, y;
	long long HashNum;
};

class CHeuristicMFCDemoDlg : public CDialogEx
{

public:
	CHeuristicMFCDemoDlg(CWnd *pParent = NULL);

	enum
	{
		IDD = IDD_HeuristicMFCDemo_DIALOG
	};

protected:
	virtual void DoDataExchange(CDataExchange *pDX);

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnUpdateList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAlgoFinished(WPARAM wParam, LPARAM lParam);
	void InitializeListControl(CListCtrl& listCtrl);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnBnClickedButtonInit();
	afx_msg void OnBnClickedButtonShow();
	afx_msg void OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	bool RunAStar(void);

private:
	CListCtrl m_openList;
	CListCtrl m_closedList;
	CProgressCtrl m_progressBar;
	CStatic m_statusText;

	std::vector<Node> m_openListNodes;
	std::vector<Node> m_closedListNodes;
	std::vector<int> m_initialState;
	std::map<long long, int> m_pathMap;
	int m_stateCount;
	std::map<int, long long> m_pathStates;
	std::map<int, int> m_previousStates;
	int MOVE_X[4] = { 1, -1, 0, 0 }, MOVE_Y[4] = { 0, 0, -1, 1 };
	bool m_hasSolution;
	std::stack<long long> m_solutionPath;
	std::priority_queue<Node> m_priorityQueue;
};
