// HeuristicMFCDemo.h: PROJECT_NAME application's main header file
//

#pragma once

#ifndef __AFXWIN_H__
#error "Include 'stdafx.h' before this file to generate PCH file"
#endif

#include "resource.h"	// Main symbols


// CHeuristicMFCDemoApp:
// For implementation details, see HeuristicMFCDemo.cpp
//

class CHeuristicMFCDemoApp : public CWinApp
{
public:
	CHeuristicMFCDemoApp();

// Override
public:
	virtual BOOL InitInstance();

// Implementations

	DECLARE_MESSAGE_MAP()
};

extern CHeuristicMFCDemoApp theApp;