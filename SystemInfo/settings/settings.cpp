#include "../glb/globalVars.h"
#include "../const/itemIDs.h"
#include "settings.h"
#include "../util/utility.h"
#include "../mainWindowProcedure.h"
void registerSettingsDialogClass()
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.lpszClassName = _T("Settings");
	wc.hInstance = ghInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = (WNDPROC)settingsDialogProcedure;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClassEx(&wc);
}

void registerTabContentWrapperWindowClass() {
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.lpszClassName = _T("TabContentWrapper");
	wc.hInstance = ghInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = (WNDPROC)tabWrapperProc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassEx(&wc);
}




std::vector<SettingsTab*> createAndFillTabs(SettingsWindow* sw) {

	const WPARAM headerCount = 3;
	TCITEM tabData;
	TCHAR* tabHeaderTxt[headerCount] = {
			_T("Main"),
			_T("Display"),
			_T("Miscellaneous")
	};

	dialogTabControl = CreateWindowEx(
		0,
		WC_TABCONTROL,
		NULL,
		WS_CHILD | WS_VISIBLE,
		0,
		0,
		440,
		500,
		settingsDialogHwnd,
		(HMENU)IDC_SETTINGS_TABAREA,
		NULL,
		NULL
	);
	std::vector<SettingsTab*> tabs;
	RECT tabHeaderRect;
	GetWindowRect(dialogTabControl, &tabHeaderRect);
	SendMessage(dialogTabControl, TCM_GETITEMRECT, NULL, (LPARAM)& tabHeaderRect);
	INT32 yOffset = tabHeaderRect.bottom - tabHeaderRect.top;
	for (WPARAM tabNum = 0; tabNum < headerCount; tabNum++) {
		tabData.mask = TCIF_TEXT;
		tabData.pszText = tabHeaderTxt[tabNum];
		SendMessage(dialogTabControl, TCM_INSERTITEM, tabNum, (LPARAM)& tabData);
		SettingsTab* st = new SettingsTab();
		HWND contentWrapperHandler = createGenericContainer(
			TAB_CONTENT_WRAPPER_OFFSET + tabNum,
			yOffset,
			sw->getWindowWidth(),
			sw->getWindowHeight(),
			settingsDialogHwnd);
		tabControlWrapperProc = (WNDPROC)SetWindowLongPtr(contentWrapperHandler, GWL_WNDPROC, (LONG_PTR)& tabWrapperProc);

		st->setTabContentWrapperHandle(contentWrapperHandler);
		st->setTabBlocks(createControlsForTab(contentWrapperHandler, TAB_CONTENT_WRAPPER_OFFSET + tabNum, sw));
		tabs.push_back(st);
		tabClickState[TAB_CONTENT_WRAPPER_OFFSET + tabNum] = SW_HIDE;
	}
	return tabs;
}

std::vector<SettingsBlock*> createControlsForTab(
	HWND controlTabWrapperHandle,
	INT32 id, SettingsWindow* sw) {
	std::vector<SettingsBlock*> settingsBlocks;
	SettingsBlock* sb = new SettingsBlock();
	std::vector<SettingsControl*> controls;
	TCHAR* checkBoxNames[3] = {
		L"Show CPU usage",
		L"Screencap client area only",
		L"Remember last window position"
	};
	if (id == TAB_CONTENT_WRAPPER_OFFSET) {
		for (int t = 0, yOffset = 15; t < 3; t++, yOffset += 25) {
			SettingsControl* sc = new SettingsControl();
			HWND hwnd = CreateWindowEx(0,
				_T("Button"),
				checkBoxNames[t],
				WS_CHILD |
				WS_VISIBLE |
				BS_AUTOCHECKBOX,
				15, yOffset,
				200, 25,
				controlTabWrapperHandle,
				(HMENU)id + 10 + t,
				NULL,
				NULL);
			sc->setControlHandle(hwnd);
			controls.push_back(sc);
		}
	}
	else if (id == TAB_CONTENT_WRAPPER_OFFSET + 1) {

		HWND htmlExportSettingsStaticHeader = CreateWindowEx(
			0,
			_T("Static"),
			_T("HTML export theme"),
			WS_VISIBLE |
			WS_CHILD,
			0, 0,
			100, 20,
			controlTabWrapperHandle,
			NULL,
			NULL,
			NULL);
		SettingsControl* htmlExportSettingsHeaderThemeGroupBoxSC = new SettingsControl();
		HWND htmlExportSettingsHeaderThemeGroupBox = CreateWindowEx(
			0,
			_T("Button"),
			_T("Header"),
			WS_VISIBLE |
			WS_CHILD | BS_GROUPBOX,
			10, 10 + 20,
			150, 150,
			controlTabWrapperHandle,
			NULL,
			NULL,
			NULL);
		htmlExportSettingsHeaderThemeGroupBoxSC->setControlHandle(htmlExportSettingsHeaderThemeGroupBox);
		controls.push_back(htmlExportSettingsHeaderThemeGroupBoxSC);

		
		SettingsControl* htmlHeaderBlockBgColor = new SettingsControl();
		HWND htmlHeaderBlockBgColorHwnd =
			ControlBuilder::initBuilder()
			->withClassName(L"Static")
			->withWindowName(
				convertColorReftoHexColorString(
					glbUserSettings->getHtmlExportHeaderBgColorRGB()))
			->withStyles(WS_VISIBLE | WS_CHILD | SS_SUNKEN | SS_CENTERIMAGE | SS_CENTER | SS_NOTIFY)
			->withCoords(15, 40)
			->withDimensions(50, 30)
			->withParent(controlTabWrapperHandle)
			->withControlId(TAB_�ONTENT_HTML_EXPORT_HEADER_BG_COLOR)
			->build();

		htmlHeaderBlockBgColor->setControlHandle(htmlHeaderBlockBgColorHwnd);
		controls.push_back(htmlHeaderBlockBgColor);

		SettingsControl * htmlHeaderBlockFgColor = new SettingsControl();
		HWND htmlHeaderBlockFgColorHwnd =
			ControlBuilder::initBuilder()
			->withClassName(L"Static")
			->withWindowName(L"#ffff00")
			->withStyles(WS_VISIBLE | WS_CHILD | SS_SUNKEN | SS_CENTERIMAGE | SS_CENTER | SS_NOTIFY)
			->withCoords(15 + 50, 40)
			->withDimensions(50, 30)
			->withParent(controlTabWrapperHandle)
			->withControlId(TAB_�ONTENT_HTML_EXPORT_HEADER_FG_COLOR)
			->build();

		htmlHeaderBlockBgColor->setControlHandle(htmlHeaderBlockFgColorHwnd);
		controls.push_back(htmlHeaderBlockBgColor);

		/*	HWND htmlExportSettingsInfoThemeGroupBox = CreateWindowEx(
				0,
				_T("Static"),
				_T("Header"),
				WS_VISIBLE |
				WS_CHILD | SS_SUNKEN,
				10, 30,
				100, 20,
				controlTabWrapperHandle,
				NULL,
				NULL,
				NULL);*/




		SettingsControl * htmlHeaderBlock = new SettingsControl();
		HWND htmlHeaderBlockStaticHwnd =
			ControlBuilder::initBuilder()
			->withClassName(L"Static")
			->withWindowName(L" Operating System")
			->withStyles(WS_VISIBLE | WS_CHILD | SS_SUNKEN)
			->withCoords(15, 150)
			->withDimensions(200, 20)
			->withParent(controlTabWrapperHandle)
			->withControlId(TAB_�ONTENT_HTML_EXPORT_HEADER)
			->build();

		htmlHeaderBlock->setControlHandle(htmlHeaderBlockStaticHwnd);
		controls.push_back(htmlHeaderBlock);

		SettingsControl * htmlInfoBlock = new SettingsControl();
		HWND htmlInfoBlockStaticHwnd =
			ControlBuilder::initBuilder()
			->withClassName(L"Static")
			->withWindowName((TCHAR*)sw->getExportInfoPreviewOsString().c_str())
			->withStyles(WS_VISIBLE | WS_CHILD | SS_SUNKEN)
			->withCoords(15, 150 + 18)
			->withDimensions(500, 20)
			->withParent(controlTabWrapperHandle)
			->withControlId(TAB_�ONTENT_HTML_EXPORT_INFO)
			->build();

		htmlHeaderBlock->setControlHandle(htmlInfoBlockStaticHwnd);
		controls.push_back(htmlInfoBlock);
	}
	sb->setControls(controls);
	settingsBlocks.push_back(sb);
	return settingsBlocks;
}

HWND createGenericContainer(INT32 id, INT32 yOffset, INT32 w, INT32 h, HWND parent) {
	return ControlManager::appCreateControl(
		_T("TabContentWrapper"),
		0,
		yOffset,
		w,
		h,
		WS_CHILD,
		parent,
		id,
		NULL,
		NULL,
		ghInstance,
		NULL
	);
}

HWND createTabContentWrapper(INT32 id, INT32 w, INT32 h, INT32 yOffset, HWND parent) {
	return createGenericContainer(id, w, h, yOffset, parent);
}

LRESULT CALLBACK settingsDialogProcedure(HWND dialogWindow, UINT message,
	WPARAM wParam, LPARAM lParam) {
	settingsDialogHwnd = dialogWindow;
	switch (message) {
	case WM_NOTIFY: {
		switch (((LPNMHDR)lParam)->code) {
		case TCN_SELCHANGE: {
			handleTabSelectionChange(TabCtrl_GetCurSel(dialogTabControl));
			break;
		}
		}
		break;
	}
	case WM_CREATE: {
		std::vector<SettingsTab*> tabs = createAndFillTabs(sw);
		sw->setTabs(tabs);
		tabClickState[TAB_CONTENT_WRAPPER_OFFSET] = SW_SHOW;
		sw->showTabs();
		EnumChildWindows(dialogWindow, (WNDENUMPROC)__SetFont, (LPARAM)GetStockObject(DEFAULT_GUI_FONT)); //setting the fonr
		break;
	}

	case WM_CLOSE: {
		delete sw;
		break;
	}
	case WM_KEYDOWN: {
		break;
	}

	default: {
		break;
	}
	}
	return (DefWindowProcW(dialogWindow, message, wParam, lParam));
}


LRESULT CALLBACK tabWrapperProc(HWND hwnd, UINT message,
	WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE: {
		break;
	}
	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case TAB_�ONTENT_HTML_EXPORT_HEADER_BG_COLOR: {
			
			glbUserSettings->setHtmlExportHeaderBgColorRGB(initializeColorDlgBox(hwnd));
			ShowWindow(GetDlgItem(hwnd, TAB_�ONTENT_HTML_EXPORT_HEADER_BG_COLOR), SW_HIDE);
			ShowWindow(GetDlgItem(hwnd, TAB_�ONTENT_HTML_EXPORT_HEADER_BG_COLOR), SW_SHOW);
			SetWindowText(GetDlgItem(hwnd, TAB_�ONTENT_HTML_EXPORT_HEADER_BG_COLOR),
				convertColorReftoHexColorString(glbUserSettings->getHtmlExportHeaderBgColorRGB()));
			break;
		}
		case TAB_�ONTENT_HTML_EXPORT_HEADER_FG_COLOR: {
			glbUserSettings->setHtmlExportHeaderFgColorRGB(initializeColorDlgBox(hwnd));
			ShowWindow(GetDlgItem(hwnd, TAB_�ONTENT_HTML_EXPORT_HEADER_FG_COLOR), SW_HIDE);
			ShowWindow(GetDlgItem(hwnd, TAB_�ONTENT_HTML_EXPORT_HEADER_FG_COLOR), SW_SHOW);
			break;
		}
		}
		//UpdateWindow(hwnd);
		break;
	}
	case WM_CTLCOLORSTATIC: {
		HDC hdcStatic = (HDC)wParam;
		if (GetDlgCtrlID((HWND)lParam) == TAB_�ONTENT_HTML_EXPORT_HEADER) {
			SetTextColor(hdcStatic, RGB(255, 255, 255));
			SetBkColor(hdcStatic, purpleColorRef);
			return (long)CreateSolidBrush(purpleColorRef);
		}
		else if (GetDlgCtrlID((HWND)lParam) == TAB_�ONTENT_HTML_EXPORT_INFO) {
			SetTextColor(hdcStatic, RGB(0, 0, 0));
			SetBkColor(hdcStatic, RGB(255, 255, 255));
			return (long)CreateSolidBrush(RGB(255, 255, 255));
		}
		else if (GetDlgCtrlID((HWND)lParam) == TAB_�ONTENT_HTML_EXPORT_HEADER_BG_COLOR) {
			SetTextColor(hdcStatic, RGB(0, 0, 0));
			SetBkColor(hdcStatic, glbUserSettings->getHtmlExportHeaderBgColorRGB());
			return (long)CreateSolidBrush(glbUserSettings->getHtmlExportHeaderBgColorRGB());
		}
		else if (GetDlgCtrlID((HWND)lParam) == TAB_�ONTENT_HTML_EXPORT_HEADER_FG_COLOR) {
			SetTextColor(hdcStatic, RGB(0, 0, 0));
			SetBkColor(hdcStatic, glbUserSettings->getHtmlExportHeaderFgColorRGB());
			return (long)CreateSolidBrush(glbUserSettings->getHtmlExportHeaderFgColorRGB());
		}

		break;
	}
	}
	return (DefWindowProcW(hwnd, message, wParam, lParam));
}


void handleTabSelectionChange(INT32 selectedTabIndex) {
	try {
		if (selectedTabIndex < 0) {
			throw _T("Something went terribly wrong");
		}
		else {
			for (int c = 0; c < 3; c++) {
				tabClickState[TAB_CONTENT_WRAPPER_OFFSET + c] = FALSE;
			}
			tabClickState[TAB_CONTENT_WRAPPER_OFFSET + selectedTabIndex] = TRUE;
			sw->showTabs();
		}
	}
	catch (TCHAR * e) {
		MessageBox(NULL, e, _T("Runtime error"), MB_OK | MB_ICONERROR);
	}
}

bool CALLBACK __SetFont(HWND child, LPARAM font)
{
	SendMessage(child, WM_SETFONT, (LPARAM)GetStockObject(DEFAULT_GUI_FONT), true);
	SendMessage(GetDlgItem(GetParent(child), TAB_�ONTENT_HTML_EXPORT_HEADER),
		WM_SETFONT,
		(WPARAM)helveticaFont, MAKELPARAM(true, 0));
	SendMessage(GetDlgItem(GetParent(child), TAB_�ONTENT_HTML_EXPORT_INFO),
		WM_SETFONT,
		(WPARAM)helveticaFont, MAKELPARAM(true, 0));
	return true;
}
std::unordered_map<WPARAM, INT32> tabClickState;

COLORREF initializeColorDlgBox(HWND hwnd) {

	CHOOSECOLOR cc;
	COLORREF chosenColor;
	static COLORREF acrCustClr[16]; // array of custom colors 
						 // owner window
	HBRUSH hbrush;                  // brush handle
	static DWORD rgbCurrent;        // initial color selection

	ZeroMemory(&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hwnd;
	cc.lpCustColors = (LPDWORD)acrCustClr;
	cc.rgbResult = rgbCurrent;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;

	if (ChooseColor(&cc) == TRUE)
	{
		hbrush = CreateSolidBrush(cc.rgbResult);
		rgbCurrent = cc.rgbResult;
	}
	chosenColor = cc.rgbResult;
	return chosenColor;
}
