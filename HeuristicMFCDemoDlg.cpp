#include "stdafx.h"
#include "HeuristicMFCDemo.h"
#include "HeuristicMFCDemoDlg.h"
#include "afxdialogex.h"

CHeuristicMFCDemoDlg::CHeuristicMFCDemoDlg(CWnd *pParent /*=NULL*/)
	: CDialogEx(CHeuristicMFCDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHeuristicMFCDemoDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_OPEN, m_openList);
	DDX_Control(pDX, IDC_LIST_CLOSE, m_closedList);
	DDX_Control(pDX, IDC_PROGRESS1, m_progressBar);
	DDX_Control(pDX, IDC_STATICSTA, m_statusText);
}

BEGIN_MESSAGE_MAP(CHeuristicMFCDemoDlg, CDialogEx)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDC_BUTTON_EXIT, &CHeuristicMFCDemoDlg::OnBnClickedButtonExit)
ON_BN_CLICKED(IDC_BUTTON_INI, &CHeuristicMFCDemoDlg::OnBnClickedButtonInit)
ON_BN_CLICKED(IDC_BUTTON_SHOW, &CHeuristicMFCDemoDlg::OnBnClickedButtonShow)
ON_MESSAGE(WM_UPDATE_LIST, &CHeuristicMFCDemoDlg::OnUpdateList)
ON_MESSAGE(WM_ALGORITHM_FINISHED, &CHeuristicMFCDemoDlg::OnAlgoFinished)
ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_OPEN, &CHeuristicMFCDemoDlg::OnGetdispinfoList)
ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_CLOSE, &CHeuristicMFCDemoDlg::OnGetdispinfoList)
ON_WM_TIMER()
END_MESSAGE_MAP()

// Message handlers for CHeuristicMFCDemoDlg

BOOL CHeuristicMFCDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);	 // Set large icon
	SetIcon(m_hIcon, FALSE); // Set small icon

	InitializeListControl(m_openList);
	InitializeListControl(m_closedList);

	m_initialState.resize(9);
	m_progressBar.SetRange(0, 100);

	return TRUE; // Return true unless the focus is set to a control
}

void CHeuristicMFCDemoDlg::InitializeListControl(CListCtrl &listCtrl)
{
	CRect rect;
	listCtrl.GetClientRect(rect);
	listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	listCtrl.InsertColumn(0, L"G", LVCFMT_LEFT, 25);
	listCtrl.InsertColumn(1, L"H", LVCFMT_LEFT, 25);
	listCtrl.InsertColumn(2, L"Layout", LVCFMT_LEFT, rect.Width() - 50);
	SetWindowTheme(listCtrl, L"Explorer", NULL);
}

void CHeuristicMFCDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

void CHeuristicMFCDemoDlg::OnPaint()
{
	CPaintDC dc(this);
	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_BACKGROUND);
	CDC MemDC;
	CRect rect;
	MemDC.CreateCompatibleDC(&dc);
	GetClientRect(&rect);

	BITMAP bm;
	bitmap.GetBitmap(&bm);
	CBitmap *pOldBitmap = MemDC.SelectObject(&bitmap);

	dc.StretchBlt(0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

	MemDC.SelectObject(pOldBitmap);

	CDialogEx::OnPaint();

	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CHeuristicMFCDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CHeuristicMFCDemoDlg::OnBnClickedButtonExit()
{
	CDialog::OnCancel();
}

void CHeuristicMFCDemoDlg::OnBnClickedButtonInit()
{
	m_openList.DeleteAllItems();
	m_closedList.DeleteAllItems();

	// Disable the button while initializing
	GetDlgItem(IDC_BUTTON_INI)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SHOW)->EnableWindow(FALSE);
	m_statusText.SetWindowText(L"Initializing...");

	// Start the initialization in a background thread
	AfxBeginThread([](LPVOID pParam) -> UINT
		{
			CHeuristicMFCDemoDlg* pDlg = static_cast<CHeuristicMFCDemoDlg*>(pParam);
			std::random_device rd;
			std::mt19937 gen(rd());
			pDlg->m_initialState = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };

			do
			{
				for (int i = 0; i < 20; i++)
				{
					std::swap(pDlg->m_initialState[std::uniform_int_distribution<>(0, 8)(gen)],
						pDlg->m_initialState[std::uniform_int_distribution<>(0, 8)(gen)]);
				}

				pDlg->m_stateCount = 1;
				pDlg->m_pathStates.clear();
				pDlg->m_previousStates.clear();
				while (!pDlg->m_priorityQueue.empty())
					pDlg->m_priorityQueue.pop();
				pDlg->m_pathMap.clear();

				int puzzleLayout[3][3], nonZeroElems[9], top = 0;
				long long cnt = 0;
				for (int i = 0; i < 9; ++i)
				{
					puzzleLayout[i / 3][i % 3] = pDlg->m_initialState[i];
					if (pDlg->m_initialState[i] != 0)
					{
						nonZeroElems[top++] = pDlg->m_initialState[i];
						cnt += std::count_if(nonZeroElems, nonZeroElems + top, [pDlg, i](int val)
							{ return val > pDlg->m_initialState[i]; });
					}
				}
				pDlg->m_hasSolution = (cnt % 2 == 0);

				Node cur;
				cur.SetState(puzzleLayout);
				pDlg->m_priorityQueue.push(cur);
				pDlg->m_pathStates[pDlg->m_stateCount] = cur.HashNum;
				pDlg->m_pathMap[cur.HashNum] = pDlg->m_stateCount;
				pDlg->m_previousStates[pDlg->m_stateCount] = -1;
				pDlg->m_stateCount++;

			} while (!pDlg->m_hasSolution);

			// Send a message to update the UI
			pDlg->PostMessage(WM_UPDATE_LIST, 0, 0);

			return 0;
		}, this);
}

LRESULT CHeuristicMFCDemoDlg::OnUpdateList(WPARAM wParam, LPARAM lParam)
{
	for (int i = 0; i < 9; ++i)
	{
		GetDlgItem(IDC_BUTTON1 + i)->SetWindowText(std::to_wstring(m_initialState[i]).c_str());
		GetDlgItem(IDC_BUTTON1 + i)->ShowWindow(m_initialState[i] == 0 ? SW_HIDE : SW_SHOW);
	}

	m_progressBar.SetPos(0);
	m_statusText.SetWindowText(L"Initialized!");
	GetDlgItem(IDC_BUTTON_INI)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SHOW)->EnableWindow(TRUE);
	UpdateWindow();

	return 0;
}

bool CHeuristicMFCDemoDlg::RunAStar(void)
{
	m_openListNodes.clear();
	m_closedListNodes.clear();

	while (!m_priorityQueue.empty())
	{
		Node cur = m_priorityQueue.top();
		m_priorityQueue.pop();
		m_closedListNodes.push_back(cur);

		for (int i = 0; i < 4; i++)
		{
			int x = cur.x + MOVE_X[i], y = cur.y + MOVE_Y[i];
			if (x >= 0 && x < 3 && y >= 0 && y < 3)
			{
				std::swap(cur.state[cur.x][cur.y], cur.state[x][y]);
				Node next = cur;
				next.SetState(cur.state);
				next.x = x;
				next.y = y;
				next.g++;

				if (m_pathMap.find(next.HashNum) == m_pathMap.end())
				{
					m_pathMap[next.HashNum] = m_stateCount;
					m_pathStates[m_stateCount] = next.HashNum;
					m_previousStates[m_stateCount] = m_pathMap[cur.HashNum];
					m_stateCount++;
					m_priorityQueue.push(next);
					m_openListNodes.push_back(next);
				}

				if (next.h == 0)
					return true;
				std::swap(cur.state[cur.x][cur.y], cur.state[x][y]);
			}
		}
	}
	return false;
}

void CHeuristicMFCDemoDlg::OnBnClickedButtonShow()
{
	m_progressBar.SetMarquee(TRUE, 100);
	if (m_hasSolution)
	{
		AfxBeginThread([](LPVOID param) -> UINT
					   {
			CHeuristicMFCDemoDlg* dlg = (CHeuristicMFCDemoDlg*)param;
			if (dlg->RunAStar()) {
				dlg->PostMessage(WM_ALGORITHM_FINISHED, NULL, NULL);
			}
			return 0; }, this);
	}
	else
	{
		m_statusText.SetWindowText(L"No solution found!");
		MessageBox(L"Please initialize the status before running the algorithm", L"Error", MB_OK);
	}
}

void CHeuristicMFCDemoDlg::OnGetdispinfoList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO *>(pNMHDR);
	LV_ITEM *pItem = &(pDispInfo)->item;

	std::vector<Node> *pNodeList = nullptr;
	if (pNMHDR->hwndFrom == m_openList.GetSafeHwnd())
		pNodeList = &m_openListNodes;
	else if (pNMHDR->hwndFrom == m_closedList.GetSafeHwnd())
		pNodeList = &m_closedListNodes;

	if (pNodeList && pItem->iItem >= 0 && pItem->iItem < static_cast<int>(pNodeList->size()))
	{
		const Node &node = (*pNodeList)[pItem->iItem];
		if (pItem->mask & LVIF_TEXT)
		{
			switch (pItem->iSubItem)
			{
			case 0:
				swprintf_s(pItem->pszText, pItem->cchTextMax, L"%d", node.g);
				break;
			case 1:
				swprintf_s(pItem->pszText, pItem->cchTextMax, L"%d", node.h);
				break;
			case 2:
				// Need to pad leading zeros if string length is less than 3x3.
				swprintf_s(pItem->pszText, pItem->cchTextMax, L"%09lld", node.HashNum);
				break;
			}
		}
	}
	*pResult = 0;
}

LRESULT CHeuristicMFCDemoDlg::OnAlgoFinished(WPARAM wParam, LPARAM lParam)
{
	m_progressBar.SetMarquee(FALSE, 100);
	m_progressBar.SetPos(100);
	m_statusText.SetWindowText(L"Found a solution!");

	// Update item amounts of the list
	m_openList.SetItemCount((int)m_openListNodes.size());
	m_closedList.SetItemCount((int)m_closedListNodes.size());

	// Force the list control to redraw
	m_openList.Invalidate();
	m_closedList.Invalidate();

	int id = m_pathMap[12345678LL]; // 012345678
	std::stack<long long> st;
	st.push(m_pathStates[id]);

	while (m_previousStates[id] != -1)
	{
		id = m_previousStates[id];
		st.push(m_pathStates[id]);
	}

	m_solutionPath = std::move(st);
	SetTimer(MY_TIMER_ID, 200, NULL); // Trigger every 200ms

	GetDlgItem(IDC_BUTTON_INI)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SHOW)->EnableWindow(FALSE);
	return 0;
}

void CHeuristicMFCDemoDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == MY_TIMER_ID)
	{
		if (m_solutionPath.empty())
		{
			KillTimer(MY_TIMER_ID);
			m_statusText.SetWindowText(L"Completed!");
			GetDlgItem(IDC_BUTTON_INI)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_SHOW)->EnableWindow(TRUE);
			return;
		}

		long long tmp = m_solutionPath.top();
		m_solutionPath.pop();

		for (int i = 0; i < 9; ++i)
		{
			int value = (int)(tmp % 10);
			GetDlgItem(IDC_BUTTON1 + i)->SetWindowText(std::to_wstring(value).c_str());
			GetDlgItem(IDC_BUTTON1 + i)->ShowWindow(value == 0 ? SW_HIDE : SW_SHOW);
			tmp /= 10;
		}

		UpdateWindow();
	}

	CDialogEx::OnTimer(nIDEvent);
}

bool Node::operator<(const Node &b) const
{
	return (h + g) > (b.h + b.g);
}

void Node::SetState(const int s1[3][3])
{
	HashNum = 0;
	h = 0;
	int k = 0;
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			HashNum = HashNum * 10 + s1[i][j];
			state[i][j] = s1[i][j];
			h += (s1[i][j] != k++);
			if (s1[i][j] == 0)
			{
				x = i;
				y = j;
			}
		}
	}
}
