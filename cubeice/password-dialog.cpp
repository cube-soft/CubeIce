/* ------------------------------------------------------------------------- */
/*
 *  password-dialog.cpp
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
	/* --------------------------------------------------------------------- */
	/*
	 *  password
	 *
	 *  NOTE: ����́C�S�Ẵp�X���[�h�_�C�A���O����̃p�X���[�h�L��
	 *  �̈�����L���Ă���D
	 */
	/* --------------------------------------------------------------------- */
	std::basic_string<TCHAR>& password() {
		static std::basic_string<TCHAR> pass;
		return pass;
	}
	
	namespace dialog {
		/* ----------------------------------------------------------------- */
		//  password_wndproc
		/* ----------------------------------------------------------------- */
		static INT_PTR CALLBACK password_wndproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
			switch (msg) {
			case WM_INITDIALOG:
			{
				// �A�C�R��
				HICON app = LoadIcon(GetModuleHandle(NULL), _T("IDI_APP"));
				SendMessage(hWnd, WM_SETICON, 0, LPARAM(app));
				HICON info = LoadIcon(NULL, IDI_INFORMATION);
				HWND pic = GetDlgItem(hWnd, IDC_ICON_PICTUREBOX);
				SendMessage(pic, STM_SETIMAGE, IMAGE_ICON, LPARAM(info));
				
				// �p�X���[�h������ύX
				SendMessage(GetDlgItem(hWnd, IDC_PASSWORD_TEXTBOX), EM_SETPASSWORDCHAR, (WPARAM)_T('*'), 0);
				SendMessage(GetDlgItem(hWnd, IDC_CONFIRM_TEXTBOX), EM_SETPASSWORDCHAR, (WPARAM)_T('*'), 0);
				
				// ��ʒ����ɕ\��
				RECT rect = {};
				GetWindowRect(hWnd, (LPRECT)&rect);
				int x = (GetSystemMetrics(SM_CXSCREEN) - (rect.right - rect.left)) / 2;
				int y = (GetSystemMetrics(SM_CYSCREEN) - (rect.bottom - rect.top)) / 2;
				SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
				
				return TRUE;
			}
			case WM_COMMAND:
				switch (LOWORD(wp)) {
				case IDOK:
				{
					// NOTE: �p�X���[�h�\����Ԃœ��͂����ꍇ�͐��������`�F�b�N���Ȃ�
					TCHAR buffer[256] = {};
					GetDlgItemText(hWnd, IDC_PASSWORD_TEXTBOX, buffer, 256);
					std::basic_string<TCHAR> pass(buffer);
					if (pass.empty()) {
						MessageBox(hWnd, _T("�p�X���[�h����͂��ĉ������B"), _T("�G���["), MB_OK | MB_ICONERROR);
						break;
					}
					else if (!IsDlgButtonChecked(hWnd, IDC_SHOWPASS_CHECKBOX)) {
						if (GetDlgItemText(hWnd, IDC_CONFIRM_TEXTBOX, buffer, 256)) {
							std::basic_string<TCHAR> confirm(buffer);
							if (pass != confirm) {
								MessageBox(hWnd, _T("�p�X���[�h����v���܂���B"), _T("�G���["), MB_OK | MB_ICONERROR);
								break;
							}
						}
					}
					else if (GetDlgItem(hWnd, IDC_CONFIRM_TEXTBOX) != NULL) { // ���k�p�p�X���[�h�_�C�A���O
						for (std::basic_string<TCHAR>::const_iterator pos = pass.begin(); pos != pass.end(); ++pos) {
							if (*pos > 0x007f) {
								if (MessageBox(hWnd, _T("�p�X���[�h�ɔ��p�p���ȊO�̕������g�p����ƁA���ɂ���Ă͉𓀂ł��Ȃ��Ȃ�ꍇ������܂��B"), _T("�p�X���[�h�̊m�F"), MB_OKCANCEL | MB_ICONWARNING) == IDOK) break;
								else return FALSE;
							}
						}
					}
					if (pass.find('\"') != std::basic_string<TCHAR>::npos) {
						MessageBox(hWnd, _T("�p�X���[�h�Ƀ_�u���N�I�[�g���܂߂邱�Ƃ͂ł��܂���B"), _T("�G���["), MB_OK | MB_ICONERROR);
						break;
					}
					cubeice::password() = pass;
					EndDialog(hWnd, IDOK);
					break;
				}
				case IDCANCEL:
					EndDialog(hWnd, IDCANCEL);
					break;
				case IDC_SHOWPASS_CHECKBOX:
				{
					BOOL enabled = IsDlgButtonChecked(hWnd, IDC_SHOWPASS_CHECKBOX);
					EnableWindow(GetDlgItem(hWnd, IDC_CONFIRM_TEXTBOX), !enabled);
					TCHAR secret = enabled ? _T('\0') : _T('*');
					SendMessage(GetDlgItem(hWnd, IDC_PASSWORD_TEXTBOX), EM_SETPASSWORDCHAR, (WPARAM)secret, 0);
					
					// �p�X���[�h�\���̐؂�ւ����x���̂ŋ����I�ɕύX����D
					TCHAR buffer[256] = {};
					GetDlgItemText(hWnd, IDC_PASSWORD_TEXTBOX, buffer, 256);
					SetDlgItemText(hWnd, IDC_PASSWORD_TEXTBOX, buffer);
					break;
				}
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
		//  password
		/* ----------------------------------------------------------------- */
		int password(HWND owner, int which) {
			if (which == 0x01) return DialogBoxParam(GetModuleHandle(NULL), _T("IDD_PASSWORD"), owner, password_wndproc, which);
			else return DialogBoxParam(GetModuleHandle(NULL), _T("IDD_PASSWORD2"), owner, password_wndproc, which);
		}
	}
}
