﻿
// Gauss_Signal.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CGaussSignalApp:
// Сведения о реализации этого класса: Gauss_Signal.cpp
//

class CGaussSignalApp : public CWinApp
{
public:
	CGaussSignalApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CGaussSignalApp theApp;
