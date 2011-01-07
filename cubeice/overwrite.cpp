/* ------------------------------------------------------------------------- */
/*
 *  overwrite.cpp
 *
 *  Copyright (c) 2010 CubeSoft Inc.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see < http://www.gnu.org/licenses/ >.
 *
 *  Last-modified: Tue 28 Dec 2010 20:42:00 JST
 */
/* ------------------------------------------------------------------------- */
#include "dialog.h"

namespace cubeice {
	namespace dialog {
		/* ----------------------------------------------------------------- */
		//  overwrite_wndproc
		/* ----------------------------------------------------------------- */
		static BOOL CALLBACK overwrite_wndproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
			switch (msg) {
			case WM_INITDIALOG:
			{
				// アイコン
				HICON app = LoadIcon(GetModuleHandle(NULL), _T("IDI_APP"));
				SendMessage(hWnd, WM_SETICON, 0, LPARAM(app));
				HICON info = LoadIcon(NULL, IDI_WARNING);
				HWND pic = GetDlgItem(hWnd, IDC_ICON_PICTUREBOX);
				SendMessage(pic, STM_SETIMAGE, IMAGE_ICON, LPARAM(info));
				
				// テキスト
				SetWindowText(GetDlgItem(hWnd, IDC_INFO_LABEL), (const TCHAR*)lp);
				
				// 画面中央に表示
				RECT rect = {};
				GetWindowRect(hWnd, (LPRECT)&rect);
				int x = (GetSystemMetrics(SM_CXSCREEN) - (rect.right - rect.left)) / 2;
				int y = (GetSystemMetrics(SM_CYSCREEN) - (rect.bottom - rect.top)) / 2;
				SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
				return TRUE;
			}
			case WM_COMMAND:
				switch (LOWORD(wp)) {
				case IDYES:
				case IDNO:
				case IDYESTOALL:
				case IDNOTOALL:
				case IDCANCEL:
				case IDRENAMETOALL:
					EndDialog(hWnd, LOWORD(wp));
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
			
			return FALSE;
		}
		
		/* ----------------------------------------------------------------- */
		//  overwrite
		/* ----------------------------------------------------------------- */
		int overwrite(const std::basic_string<TCHAR>& message) {
			return DialogBoxParam(GetModuleHandle(NULL), _T("IDD_OVERWRITE"), NULL, overwrite_wndproc, (LPARAM)message.c_str());
		}
	}
}
