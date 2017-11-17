#include "wndproc.h"
#include "initialize_ui.h"
#include "constant.h"
#include "set_autorun.h"
#include <stdio.h>
#pragma warning (disable : 4996)

void trace (const char *fmt, ...)
{
	char buffer[MAX_PATH];
	va_list ap;

	va_start(ap, fmt);
	vsprintf(buffer, fmt, ap);
	va_end(ap);
	strcat(buffer, "\n");
	OutputDebugString(buffer);
}

static int _kc_checklist[] = { VK_UP, VK_DOWN, VK_LEFT, VK_RIGHT, VK_NUMPAD0, 0};
static int _kc_actions[256] = {0};
static BYTE _kc_preks[256] = {0};
static BOOL _left_dragging = FALSE;

BOOL isNumLocked()
{
	return LOBYTE(_kc_preks[VK_NUMLOCK]) ? TRUE : FALSE;
}

BOOL parseKeyValue(int id, const char* iniFile)
{
	static int _idlist[] = { 
		ID_HOT_LEFT_CLICK, ID_HOT_MIDDLE_PRESS, 
		ID_HOT_RIGHT_CLICK, ID_HOT_LEFT_DN_SWITCH, 
		ID_HOT_LEFT_CLICK_X10, 0 
	};
	static const char *_namelist[] = { 
		"LEFT_CLICK", "MIDDLE_PRESS",
		"RIGHT_CLICK", "LEFT_DOWN_SWITCH", 
		"LEFT_CLICK_X10", 0 
	};

	int i;		
	int code = 0;
	const char *name = NULL;
	char buffer[MAX_PATH + 1] = {0};
	
	i = 0;
	while( code = _idlist[i] ) 
	{
		if (id == code) {
			name = _namelist[i];
			break;
		}
		i++;
	}
	
	if (name != NULL) {
		int ret;
		int val;
		const char *p = NULL;
		ret = GetPrivateProfileString("keys", name, "", buffer, MAX_PATH, iniFile);
		p = strstr(buffer, "0x");
		if (p != NULL) {
			ret = sscanf(p+2, "%x", &val);
			if (ret == 1 && val > 0) {
				for (i = 0; i < 256; ++i) {
					if (_kc_actions[i] == id)
						_kc_actions[i] = 0;
				}
				_kc_actions[val] = id;
				trace("> %s ==> %d", name, id);
				return TRUE;
			}
		}
	}
	return FALSE;
}

void loadKeySettings()
{
	char** lppPart={NULL};
	char iniFile[MAX_PATH + 1];
	char *p;
	if (GetModuleFileName(NULL, iniFile, MAX_PATH) )
	{
		GetFullPathName(iniFile,MAX_PATH,iniFile,lppPart);
		p = strrchr(iniFile, '.');
		if (p != NULL) {
			strcpy(p+1, "ini");
//			WritePrivateProfileString("KEYS", "LEFT_CLICK", "haha", iniFile);
			
			parseKeyValue(ID_HOT_LEFT_CLICK, iniFile);
			parseKeyValue(ID_HOT_MIDDLE_PRESS, iniFile);
			parseKeyValue(ID_HOT_RIGHT_CLICK, iniFile);
			parseKeyValue(ID_HOT_LEFT_DN_SWITCH, iniFile);
			parseKeyValue(ID_HOT_LEFT_CLICK_X10, iniFile);
		}
	}
}

void updateCheckListByKeyMap()
{
	int i;
	int j;
	int c1 = sizeof(_kc_checklist);
	int c2 = sizeof(*_kc_checklist);
	int cnt = sizeof(_kc_checklist) / sizeof(_kc_checklist[0]);
	j = 0;
	memset(_kc_checklist, 0, sizeof(_kc_checklist) );
	for (i = 0; i < 256; ++i) {
		if (_kc_actions[i] != 0)
		{
			_kc_checklist[j] = i;
			j++;
			if (j >= cnt-1)
				break;
		}
	}
	trace("");
}

void intKeyAction()
{
	int code;
	int i;
	_kc_actions[VK_LEFT] = ID_HOT_LEFT_CLICK;
	_kc_actions[VK_DOWN] = ID_HOT_MIDDLE_PRESS;
	_kc_actions[VK_RIGHT] = ID_HOT_RIGHT_CLICK;
	_kc_actions[VK_NUMPAD0] = ID_HOT_LEFT_DN_SWITCH;
	_kc_actions[VK_UP] = ID_HOT_LEFT_CLICK_X10;
	
	loadKeySettings();
	updateCheckListByKeyMap();

	i = 0;
	while( code =_kc_checklist[i] ) 
	{
		int act = _kc_actions[code];
		RegisterHotKey( NULL, act,	MOD_NOREPEAT, code );
		i++;
	}
}

int getKeyAction(int code)
{
	return _kc_actions[code];
}

void updateKeyboardMonitor()
{
	int code;
	int i;
	BYTE ks[256] = {0};
	i = 0;
	while( code=_kc_checklist[i] ) 
	{
		ks[code] = GetAsyncKeyState(code) & 0x8000 ? TRUE : FALSE;
		if ( _kc_preks[code] != ks[code] ) {
			if (LOBYTE(ks[code]) )
			{
				onKeyDown(code);
			} else {
				onKeyUp(code);
			}
		}
		i++;
	}
	memcpy(_kc_preks, ks, 256);
	/*
	0---键当前未处于按下状态，而且自上次调用GetAsyncKeyState后改键也未被按过；
	1---键当前未处于按下状态，但在此之前（自上次调用GetAsyncKeyState后）键曾经被按过；
	-32768（即16进制数&H8000） --- 键当前处于按下状态，但在此之前（自上次调用GetAsyncKeyState后）键未被按过；
	-32767（即16进制数&H8001） --- 键当前处于按下状态，而且在此之前（自上次调用GetAsyncKeyState后）键也曾经被按过。
	*/
}

void onKeyDown(BYTE code)
{
	int act = getKeyAction(code);
	switch( act )
	{
	case ID_HOT_LEFT_CLICK:
		mouse_event( MOUSEEVENTF_LEFTDOWN,   0, 0, 0, 0 );
		break;
	case ID_HOT_MIDDLE_PRESS:
		mouse_event( MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, 0 );
		break;
	case ID_HOT_RIGHT_CLICK:
		mouse_event( MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0 );
		break;
	}
//	trace("ON_KEY_DN:%d", code);
}

void onKeyUp(BYTE code)
{
	int i = 0;
	int act = getKeyAction(code);
	switch( act )
	{
	case ID_HOT_LEFT_CLICK:
		mouse_event( MOUSEEVENTF_LEFTUP,   0, 0, 0, 0 );
		_left_dragging = FALSE;
		break;
	case ID_HOT_MIDDLE_PRESS:
		mouse_event( MOUSEEVENTF_MIDDLEUP, 0, 0, 0, 0 );
		break;
	case ID_HOT_RIGHT_CLICK:
		mouse_event( MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0 );
		break;
	case ID_HOT_LEFT_DN_SWITCH:
		_left_dragging ^= 1;
		if (_left_dragging) {
			mouse_event( MOUSEEVENTF_LEFTDOWN,   0, 0, 0, 0 );
			//trace("> LEFT DOWN");
		} else {
			mouse_event( MOUSEEVENTF_LEFTUP,   0, 0, 0, 0 );
			//trace("> LEFT UP");
		}
		break;
	case ID_HOT_LEFT_CLICK_X10:
		for (i = 0; i < 12; ++i)
		{
			mouse_event( MOUSEEVENTF_LEFTDOWN,   0, 0, 0, 0 );
			mouse_event( MOUSEEVENTF_LEFTUP,   0, 0, 0, 0 );
			Sleep(40);
		}
		_left_dragging = FALSE;
		break;
	}
//	trace("ON_KEY_UP:%d", code);
}

void onHotKey(int code)
{
	
	int act = getKeyAction(code);
}
//////////////////////////////////////////////////////////////////////////


//主窗口回调函数
LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	static HWND hBtn[5];
	//LOGFONT		lf;
	HDC			hdc;
	PAINTSTRUCT ps;
	char		btnLab[10];
	HFONT		hFont;

	switch( message )
	{
	case WM_CREATE:						//创建创建时的相关初始化
		regVirtualMouseHotKey( hwnd );

		InitWndUI( hwnd, ((LPCREATESTRUCT)lParam)->hInstance, hBtn);
		if( checkAutorun() )
			SendMessage( hBtn[3], BM_SETCHECK, 1, 0 );

		intKeyAction();
		SetTimer(hwnd, IDT_LOOP, 2, (TIMERPROC)NULL);
		return 0;

	case WM_COMMAND:					//处理按钮消息
		dealWithBtnMsg( hwnd, wParam, hBtn );	return 0;

	case WM_HOTKEY:						//处理热键消息
		//dealWithHotKey( hwnd, wParam );
		onHotKey(wParam);
		return 0;

	case WM_PAINT:						//处理重绘消息
		hdc = BeginPaint (hwnd, &ps) ;
		hFont = getTextFont(0);
		//hFont = CreateFontIndirect (&lf);
		SelectObject (hdc, hFont ) ;
		//SetBkColor( hdc, GetSysColor(COLOR_BACKGROUND) ); // RGB(236, 233, 216)
		SetBkColor( hdc, GetSysColor(COLOR_BTNFACE) );
		drawTipText( hdc );
		//DeleteObject( hFont );
		EndPaint (hwnd, &ps) ;
		return 0;

	case WM_NOTIFY:						//处理超链接消息
	  switch (((LPNMHDR)lParam)->code)
	  {
		  case NM_CLICK:
		  case NM_RETURN:
			ShellExecute( NULL, "open", "http://www.cnblogs.com/mr-wid/", NULL, NULL, SW_SHOW );
			break;
	  }
	  return 0;

	case WM_TIMER: 
		if (wParam == IDT_LOOP) 
		{ 
			updateKeyboardMonitor();	
		}
		return 0;

	case WM_DESTROY:					//退出程序

		GetWindowText( hBtn[0], btnLab, 10 );
		if( strcmp( btnLab, "暂停模拟" ) == 0 )			//当热键没有被注销时
		{
			destroyRegedHotKey( hwnd );					//注销模拟热键
			UnregisterHotKey( hwnd, ID_HOT_WND_HIDE );	//注销主界面呼出热键
			UnregisterHotKey( hwnd, ID_HOT_PAUSE );		//注销暂停\继续热键
		}
		KillTimer(hwnd, IDT_LOOP);
		PostQuitMessage( 0 );		return 0;

	}

	return DefWindowProc( hwnd, message, wParam, lParam );
}

//////////////////////////////////////////////////////////////////////////

//注册热键
void regVirtualMouseHotKey( HWND hwnd )
{
	BOOL ret = FALSE;
	//RegisterHotKey( hwnd, ID_HOT_UP,	MOD_CONTROL, VK_NUMPAD8 );			//Ctrl + 8, 上
	//RegisterHotKey( hwnd, ID_HOT_DOWN,	MOD_CONTROL, VK_NUMPAD2 );			//Ctrl + 2, 下
	//RegisterHotKey( hwnd, ID_HOT_LEFT,	MOD_CONTROL, VK_NUMPAD4 );			//Ctrl + 4, 左
	//RegisterHotKey( hwnd, ID_HOT_RIGHT, MOD_CONTROL, VK_NUMPAD6 );			//Ctrl + 6, 右

	//RegisterHotKey( hwnd, ID_HOT_SL_UP,		MOD_CONTROL, VK_UP );			//Ctrl + UP,	微调, 上
	//RegisterHotKey( hwnd, ID_HOT_SL_DOWN,	MOD_CONTROL, VK_DOWN );			//Ctrl + DOWN,	微调, 下
	//RegisterHotKey( hwnd, ID_HOT_SL_LEFT,	MOD_CONTROL, VK_LEFT );			//Ctrl + LEFT,	微调, 左
	//RegisterHotKey( hwnd, ID_HOT_SL_RIGHT,	MOD_CONTROL, VK_RIGHT );		//Ctrl + RIGHT,	微调, 右

	RegisterHotKey( hwnd, ID_HOT_TOPLEFT,	MOD_CONTROL, VK_NUMPAD7 );		//注册 Ctrl + 7, 左上
	RegisterHotKey( hwnd, ID_HOT_TOPRIGHT,	MOD_CONTROL, VK_NUMPAD9 );		//注册 Ctrl + 9, 右上
	RegisterHotKey( hwnd, ID_HOT_BOTLEFT,	MOD_CONTROL, VK_NUMPAD1 );		//注册 Ctrl + 1, 左下
	RegisterHotKey( hwnd, ID_HOT_BOTRIGHT,	MOD_CONTROL, VK_NUMPAD3 );		//注册 Ctrl + 3, 右下

	RegisterHotKey( hwnd, ID_HOT_WND_HIDE,		MOD_CONTROL, VK_F12 );			//注册 Ctrl + 12 , 显示窗口
	RegisterHotKey( hwnd, ID_HOT_PAUSE,			MOD_CONTROL, VK_F10 );			//注册 Ctrl + 10 , 暂停\继续热键
}

//////////////////////////////////////////////////////////////////////////

//撤销注册的热键
void destroyRegedHotKey( HWND hwnd )
{
	int hotID = ID_HOT_UP;
	for( hotID; hotID <= ID_HOT_LEFT_CLICK_X10; hotID++ )
		UnregisterHotKey( hwnd, hotID );
}

//////////////////////////////////////////////////////////////////////////

//处理热键消息
void dealWithHotKey( HWND hwnd, WPARAM wParam )
{
	POINT ptCorPos;
	GetCursorPos( &ptCorPos );

	switch( wParam )
	{
	case ID_HOT_UP:			ptCorPos.y -= 40;	break;			//移动, 上	
	case ID_HOT_DOWN:		ptCorPos.y += 40;	break;			//移动, 下
	case ID_HOT_LEFT:		ptCorPos.x -= 40;	break;			//移动, 左
	case ID_HOT_RIGHT:		ptCorPos.x += 40;	break;			//移动, 右
	case ID_HOT_SL_UP:		ptCorPos.y -= 10;	break;			//微调, 上
	case ID_HOT_SL_DOWN:	ptCorPos.y += 10;	break;			//微调, 下
	case ID_HOT_SL_LEFT:	ptCorPos.x -= 10;	break;			//微调, 左
	case ID_HOT_SL_RIGHT:	ptCorPos.x += 10;	break;			//微调, 右
		
	case ID_HOT_TOPLEFT:		//移动, 左上
		ptCorPos.x -= 40;	ptCorPos.y -= 40;	break;

	case ID_HOT_TOPRIGHT:		//移动, 右上
		ptCorPos.x += 40;	ptCorPos.y -= 40;	break;

	case ID_HOT_BOTLEFT:		//移动, 左下
		ptCorPos.x -= 40;	ptCorPos.y += 40;	break;

	case ID_HOT_BOTRIGHT:		//移动, 右下
		ptCorPos.x += 40;	ptCorPos.y += 40;	break;

	case ID_HOT_LEFT_CLICK:		//左击
		//if ( isNumLocked() ) {
		//	mouse_event( MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0 );
		//	sprintf(buffer, "ON_HOT_KEY:%d\n", wParam);
		//	OutputDebugString(buffer);
		//}
		//mouse_event( MOUSEEVENTF_LEFTUP,   0, 0, 0, 0 );
		trace("ID_HOT_LEFT_CLICK");
		break;

	case ID_HOT_RIGHT_CLICK:	//右击
		//mouse_event( MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0 );
		//mouse_event( MOUSEEVENTF_RIGHTUP,   0, 0, 0, 0 );
		trace("ID_HOT_RIGHT_CLICK");
		break;
	case ID_HOT_MIDDLE_PRESS:	//中键击键
		//mouse_event( MOUSEEVENTF_MIDDLEDOWN, 0, 0, 0, 0 );
		//mouse_event( MOUSEEVENTF_MIDDLEUP,   0, 0, 0, 0 );
		trace("ID_HOT_MIDDLE_PRESS");
		break;

	case ID_HOT_MIDDLE_UP:		//滚轮向上
		mouse_event( MOUSEEVENTF_WHEEL, 0, 0, (DWORD)50, 0 );	break;

	case ID_HOT_MIDDLE_DOWN:	//滚轮向下
		mouse_event( MOUSEEVENTF_WHEEL, 0, 0, (DWORD)-50, 0 );	break;


	case ID_HOT_WND_HIDE:		//呼出主界面
		ShowWindow( hwnd, SW_SHOWNORMAL );
		SetForegroundWindow(hwnd);	break;

	case ID_HOT_PAUSE:			//暂停\继续热键
		SendMessage( hwnd, WM_COMMAND, ID_BTN_PAUSE | BN_CLICKED, 0 );	break;
	}
	SetCursorPos( ptCorPos.x, ptCorPos.y );
}

//////////////////////////////////////////////////////////////////////////

//处理按钮消息
void dealWithBtnMsg( HWND hwnd,  WPARAM wParam, HWND *hBtn )
{
	char btnLab[10];

	switch( LOWORD(wParam) )
	{
	case ID_BTN_HIDE:			//处理隐藏按钮消息
		ShowWindow( hwnd, SW_MINIMIZE );		//先最小化
		ShowWindow( hwnd, SW_HIDE );			//再隐藏
		return ;

	case ID_BTN_PAUSE:			//处理暂停\继续按钮消息
		GetWindowText( hBtn[0], btnLab, 10 );
		if( strcmp( btnLab, "暂停模拟" ) == 0 )
		{
			destroyRegedHotKey( hwnd );
			SetWindowText( hBtn[0], TEXT("继续模拟") );
		}
		else
		{
			regVirtualMouseHotKey( hwnd );
			SetWindowText( hBtn[0], TEXT("暂停模拟") );
		}
		return ;

	case ID_BTN_EXIT:			//处理退出模拟消息
		SendMessage( hwnd, WM_DESTROY, 0, 0 );
		return ;

	case ID_ID_AUTORUN:			//处理开机自启动消息
		VirtualMouseAutorun( hBtn[3] );
		return ;
	}
}

//////////////////////////////////////////////////////////////////////////

//绘制提示信息
void drawTipText( HDC hdc )
{
	int i = 1, x = 10, y = 30;

	TCHAR szTip[][128] = {
		TEXT("注意: 以下数字均指小键盘数字。"),
		TEXT("________________________________________"),
		TEXT("虚拟左键: ?"),
		TEXT("虚拟右键: ?"),
		TEXT("虚拟中键: ?"),
		TEXT("虚拟左键按下切换: ?"),
		TEXT("虚拟左键x10: ?"),
		TEXT("")
	};

	TextOut ( hdc, 10, 5, szTip[0], lstrlen (szTip[0]) ) ;
	for( i; i < sizeof( szTip ) / sizeof( szTip[0] ); i++ )
	{
		TextOut ( hdc, x, y, szTip[i], lstrlen (szTip[i]) ) ;
		y += 20;
	}
}
