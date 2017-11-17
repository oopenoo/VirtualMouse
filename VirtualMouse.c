#include <windows.h>

#include "wndproc.h"
#include <commctrl.h>
#include "resource.h"

#pragma comment ( lib, "comctl32.lib")

//////////////////////////////////////////////////////////////////////////

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow )
{
	static char szAppName[] = "VirtualMouse";

	HWND		hwnd;
	MSG			msg;
	WNDCLASS	wndclass;

	wndclass.style			= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc	= WndProc;
	wndclass.hInstance		= hInstance;
	wndclass.hCursor		= LoadCursor( NULL, IDC_ARROW );
	wndclass.hIcon			= LoadIcon( hInstance, MAKEINTRESOURCE(IDI_ICON_LEFT) );
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hbrBackground	= CreateSolidBrush( GetSysColor(COLOR_BTNFACE) ); // RGB(236, 233, 216)
	wndclass.lpszClassName	= szAppName;
	wndclass.lpszMenuName	= NULL;

	if( !RegisterClass( &wndclass ) )
	{
		MessageBox( NULL, TEXT("������ע��ʧ��!"), TEXT("����"), MB_OK | MB_ICONERROR );
		return 0;
	}
	
	InitCommonControls();

	hwnd = CreateWindow(
		szAppName, TEXT("�������"),
		WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT,
		400, 300,
		NULL, NULL, hInstance, NULL
	);

	ShowWindow( hwnd, iCmdShow );
	UpdateWindow( hwnd );

	while( GetMessage( &msg, NULL, 0, 0 ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	return msg.wParam;
}
