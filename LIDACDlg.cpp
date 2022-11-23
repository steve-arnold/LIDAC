#include "stdafx.h"

using namespace std;

HINSTANCE hInst = 0;
static char appAboutName[] = "About LIDAC";

int NewHandler(size_t size)
{
	UNREFERENCED_PARAMETER(size);
	throw WinException("Out of memory");
}

// Forward declarations of functions included in this code module:
INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	hInst = hInstance;
	_set_new_handler(&NewHandler);

	HWND hDialog = 0;
	// need this for spinner control under windows XP
	INITCOMMONCONTROLSEX cc;
	cc.dwSize = sizeof(INITCOMMONCONTROLSEX);
	cc.dwICC = ICC_BAR_CLASSES | ICC_UPDOWN_CLASS | ICC_USEREX_CLASSES;
	InitCommonControlsEx(&cc);

	hDialog = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_LIDAC_DIALOG), 0, static_cast<DLGPROC>(DialogProc));
	if (!hDialog)
	{
		char buf[100];
		wsprintf(buf, "Error x%x", GetLastError());
		MessageBox(0, buf, "CreateDialog", MB_ICONEXCLAMATION | MB_OK);
		return 1;
	}
	// Attach icon to main dialog
	const HANDLE hbicon = LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDR_MAINFRAME),
		IMAGE_ICON, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), 0);
	// load big icon
	if (hbicon)
		SendMessage(hDialog, WM_SETICON, ICON_BIG, LPARAM(hbicon));

	// load small icon
	const HANDLE hsicon = LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDR_MAINFRAME),
		IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
	if (hsicon)
		SendMessage(hDialog, WM_SETICON, ICON_SMALL, LPARAM(hsicon));

	MSG  msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		// only handle non-dialog messages here
		if (!IsDialogMessage(hDialog, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}

Controller::Controller(HWND hwnd)
	: m_eSpin(hwnd, IDC_EDIT1),
	m_eSignal(hwnd, IDC_EDIT2),
	m_bSeize(hwnd, IDC_SEIZE),
	m_bRelease(hwnd, IDC_RELEASE),
	m_bGenerate(hwnd, IDC_GENERATE),
	m_bClose(hwnd, IDOK),
	m_cSpinner(hwnd, IDC_SPIN),
	m_cDropdown(hwnd, IDC_COMBO)
{
	// initializations...
	m_bSeize.Select();
	m_cSpinner.setLimits(10, 1); // set spinner box limits
	m_cDropdown.addStrings();

	HFONT hfFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	SendMessage(m_eSignal.Hwnd(), WM_SETFONT, reinterpret_cast<WPARAM>(hfFont), MAKELPARAM(FALSE, 0));
}

void Controller::Command(HWND hwnd, int controlID, int command)
{
	switch (controlID)
	{
	case IDC_GENERATE:
		LRESULT offset;
		if (command == BN_CLICKED)
		{
			offset = m_cDropdown.getSelection();
			if (offset != CB_ERR)
			{
				Generate(offset);
			}
		}
		break;
	case IDOK:
		PostMessage(hwnd, WM_CLOSE, 0, 0);
		break;
	}
}

void Controller::Generate(LRESULT select)
{
	BOOL release = m_bRelease.IsSelected();
	// get selected licence tag
	string tagText = licenseTags[select];

	// create output stream
	stringstream sstr;

	// configure the stream for correct display of HEX
	sstr.setf(ios_base::hex, ios::basefield);
	sstr.setf(ios_base::uppercase);
	sstr.fill('0');
	sstr.width(2);

	// Print the first static lines
	sstr << signalTemplate[9].c_str() << "\r\n" << signalTemplate[0].c_str() << "\r\n" << signalTemplate[1].c_str() << "\r\n";

	// seize or release
	string seizeRelease = (release) ? "02;" : "01;";
	sstr << signalTemplate[2].c_str() << seizeRelease.c_str() << "\r\n";

	// how many licences?
	sstr << signalTemplate[3].c_str();
	sstr.width(2);
	sstr << hex << m_cSpinner.getSelection() << ";\r\n";

	// length of tag
	size_t length = tagText.size();
	sstr << signalTemplate[4].c_str();
	sstr.width(2);
	sstr << hex << length << ";\r\n";
	size_t lines = (length + 9) / 10;
	for (size_t l = 0; l < lines; l++)
	{
		size_t remainder = (length)-(l * 10);
		if (remainder > 10)
		{
			// full line of 10 values
			sstr << signalTemplate[5].c_str();
			for (int i = 0; i < 10; i++)
			{
				// byte number
				size_t point = ((l * 10 + i));
				sstr << dec << (10 + point) << "-";
				sstr.width(2);
				sstr << hex << (int)tagText[point];
				if (i < 9)
					sstr << "&";
			}
			sstr << ";\r\n";
		}
		else
		{
			// final line, full or partial
			sstr << signalTemplate[5].c_str();
			for (size_t i = 0; i < remainder; i++)
			{
				size_t point = ((l * 10 + i));
				sstr << dec << (10 + point) << "-";
				sstr.width(2);
				sstr << hex << (int)tagText[point];
				if (i < (remainder - 1))
					sstr << "&";
			}
			sstr << ";\r\n";
		}
	}
	sstr << signalTemplate[5].c_str();
	for (int i = 0; i < 6; i++)
	{
		int dummy[] = { 0x0, 0x0, 0x8, 0x1, 0x99, 0x1 };
		sstr << dec << (10 + length + i) << "-";
		sstr.width(2);
		sstr << hex << (int)dummy[i];
		if (i < 5)
			sstr << "&";
	}
	sstr << ";\r\n";
	// Finish off
	sstr << signalTemplate[6].c_str() << "\r\n" << signalTemplate[7].c_str() << "\r\n" << signalTemplate[8].c_str() << "\r\n";

	// Load up the edit control String
	m_eSignal.SetString(const_cast<char*>(sstr.str().c_str()));
}

INT_PTR CALLBACK DialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	static Controller* control = 0;
	switch (message)
	{
	case WM_INITDIALOG:
		try
		{
			control = new Controller(hwnd);
			HMENU hSysMenu;
			hSysMenu = GetSystemMenu(hwnd, FALSE);
			// add a system menu separator
			AppendMenu(hSysMenu, MF_SEPARATOR, NULL, NULL);
			// add a system menu item
			AppendMenu(hSysMenu, MF_STRING, IDD_ABOUTBOX, appAboutName);
		}
		catch (WinException e)
		{
			MessageBox(0, e.GetMessage(), "Exception", MB_ICONEXCLAMATION | MB_OK);
		}
		catch (...)
		{
			MessageBox(0, "Unknown", "Exception", MB_ICONEXCLAMATION | MB_OK);
			return -1;
		}
		return TRUE;
	case WM_COMMAND:
		if (control)
			control->Command(hwnd, LOWORD(wParam), HIWORD(wParam));
		return (INT_PTR)TRUE;
	case WM_DESTROY:
		PostQuitMessage(0);
		return (INT_PTR)TRUE;
	case WM_CLOSE:
		if (control)
			delete control;
		DestroyWindow(hwnd);
		return (INT_PTR)TRUE;
	case WM_SYSCOMMAND:
	{
		switch (wParam)
		{
		case IDD_ABOUTBOX:
		{
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
			break;
		}
		}
		break;
	}
	}
	return (INT_PTR)FALSE;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
