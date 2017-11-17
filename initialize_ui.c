#include "initialize_ui.h"
#include "constant.h"
#pragma warning (disable : 4996)

#define FONTS_NUM 3
static HFONT _wnd_font[FONTS_NUM] = { NULL };

//////////////////////////////////////////////////////////////////////////

HFONT getTextFont(int idx)
{
	HFONT hFont = _wnd_font[idx];
	if (idx >= FONTS_NUM)
		return NULL;
	if (hFont == NULL)
	{
		LOGFONT		lf;
		memset( &lf, 0, sizeof(LOGFONT) );
		strcpy( lf.lfFaceName, "΢���ź�" );
		lf.lfWidth			= 0;
		lf.lfHeight			= 18;
		lf.lfEscapement		= 0;
		lf.lfOrientation	= 0;
		lf.lfWeight			= FW_NORMAL;
		lf.lfItalic			= 0;
		lf.lfUnderline		= 0;
		lf.lfStrikeOut		= 0;
		lf.lfCharSet		= GB2312_CHARSET;
		switch (idx) {
		case 1:
			lf.lfWidth  = 0;
			lf.lfHeight = 20;
			break;
		case 2:
			strcpy( lf.lfFaceName, "Tahoma" );
			lf.lfWidth			= 5;
			lf.lfHeight			= 12;
			lf.lfUnderline		= 1;
			break;
		}
		hFont = CreateFontIndirect (&lf);
		_wnd_font[idx] = hFont;
	}
	return hFont;
}


void freeTextFont()
{
	int i;
	for (i = 0; i < FONTS_NUM; ++i)
	{
		HFONT hFont = _wnd_font[i];
		if (hFont != NULL) {
			DeleteObject(hFont);
			_wnd_font[i] = NULL;
		}
	}
}
//////////////////////////////////////////////////////////////////////////

//��ʼ��������
void InitWndUI( HWND hwnd, HINSTANCE hInstance, HWND *hwndBtn, HFONT *listFont)
{
	HFONT hFont = getTextFont(1);
	HFONT hFont2 = getTextFont(0);
	HFONT hFont3 = getTextFont(2);
	//////////////////////////////////////////////////////////////////////////
	//������ť
	hwndBtn[0] = CreateWindow(
		TEXT("button"), TEXT("��ͣģ��"),
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		290, 20, 80, 40,
		hwnd, (HMENU)ID_BTN_PAUSE, hInstance, NULL
	);
	SendMessage( hwndBtn[0], WM_SETFONT, (WPARAM)hFont, 0 );

	hwndBtn[1] = CreateWindow(
		TEXT("button"), TEXT("���ش���"),
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		290, 100, 80, 40,
		hwnd, (HMENU)ID_BTN_HIDE, hInstance, NULL
	);
	SendMessage( hwndBtn[1], WM_SETFONT, (WPARAM)hFont, 0 );

	hwndBtn[2] = CreateWindow(
		TEXT("button"), TEXT("�˳�ģ��"),
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		290, 180, 80, 40,
		hwnd, (HMENU)ID_BTN_EXIT, hInstance, NULL
	);
	SendMessage( hwndBtn[2], WM_SETFONT, (WPARAM)hFont, 0 );

	//////////////////////////////////////////////////////////////////////////
	//��ѡ��
	//hwndBtn[3] = CreateWindow(
	//	TEXT("button"), TEXT("�����Զ�����"),
	//	WS_CHILD|WS_VISIBLE|BS_CHECKBOX,
	//	270, 240, 120, 20,
	//	hwnd, (HMENU)ID_ID_AUTORUN, hInstance, NULL
	//);

	hwndBtn[3] = CreateWindowEx(WS_EX_TRANSPARENT,
		TEXT("button"), TEXT("�����Զ�����"),
		WS_CHILD|WS_VISIBLE|BS_CHECKBOX,
		270, 240, 120, 20,
		hwnd, (HMENU)ID_ID_AUTORUN, hInstance, NULL
		);

//	lf.lfWidth = 7;		lf.lfHeight = 16;
	SendMessage( hwndBtn[3], WM_SETFONT, (WPARAM)hFont2, 0 );

	//////////////////////////////////////////////////////////////////////////
	//������
	//lf.lfWidth			= 6;
	//lf.lfHeight			= 12;
	//lf.lfUnderline		= 1;
	hwndBtn[4] = CreateWindow(
		TEXT("syslink"), TEXT(""),
		WS_CHILD|WS_VISIBLE|SS_NOTIFY,
		10, 246, 180, 20,
		hwnd, (HMENU)ID_ID_HYPERLINK, hInstance, 0
	);
	SetWindowText( hwndBtn[4], TEXT("<a href=\"\">http://www.cnblogs.com/mr-wid</a>") );
	SendMessage( hwndBtn[4], WM_SETFONT, (WPARAM)hFont3, 0 );
}
