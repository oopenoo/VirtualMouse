#pragma once

//////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <commctrl.h>
#include <string.h>
#include "constant.h"

#define IDT_LOOP  100
//////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );			//�����ڻص��¼�

void regVirtualMouseHotKey( HWND hwnd );						//ע���ȼ�

void dealWithHotKey( HWND, WPARAM );							//�����ȼ���Ϣ

void dealWithBtnMsg( HWND, WPARAM, HWND *hBtn );	//����ť��Ϣ

void destroyRegedHotKey( HWND );					//����ע����ȼ�

void drawTipText( HDC );							//��ʾ��ʾ��Ϣ

void onKeyDown(BYTE code);

void onKeyUp(BYTE code);