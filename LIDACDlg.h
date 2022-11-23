#pragma once

#include <windows.h>
#include "LIDAC_add.h"

class WinException
{
public:
	WinException(char* msg)
		: _err(GetLastError()), _msg(msg) {}
	DWORD GetError() const { return _err; }
	char* GetMessage() const { return _msg; }
private:
	DWORD _err;
	char* _msg;
};

// out of memory handler that throws WinException
int NewHandler(size_t size);

class Controller
{
public:
	Controller(HWND hwnd);
	~Controller() {}
	void Command(HWND hwnd, int controlID, int command);
	void Generate(LRESULT selection);
private:
	Edit        m_eSpin;
	Edit        m_eSignal;
	RadioButton m_bSeize;
	RadioButton m_bRelease;
	Button      m_bGenerate;
	Button      m_bClose;
	Spinner     m_cSpinner;
	ComboBox    m_cDropdown;
};
