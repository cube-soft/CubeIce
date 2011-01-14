/* ------------------------------------------------------------------------- */
/*
 *  dialog.cpp
 *
 *  Copyright (c) 2010 CubeSoft.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    - No names of its contributors may be used to endorse or promote
 *      products derived from this software without specific prior written
 *      permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  Last-modified: Wed 17 Nov 2010 17:39:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef NO_WIN32_LEAN_AND_MEAN
#define NO_WIN32_LEAN_AND_MEAN
#include <shlobj.h>
#endif // NO_WIN32_LEAN_AND_MEAN

#include <cstdlib>
#include <tchar.h>
#include <windows.h>
#include <commctrl.h>
#include "dialog.h"
#include "clx/split.h"

namespace cubeice {
	/* ----------------------------------------------------------------- */
	//  browse_proc
	/* ----------------------------------------------------------------- */
	static int CALLBACK browse_proc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
		if(uMsg == BFFM_INITIALIZED){
			SendMessage(hwnd, BFFM_SETSELECTION, (WPARAM)TRUE, lpData);
		}
		return 0;
	}
	
	/* ----------------------------------------------------------------- */
	/*
	 *  browsefolder
	 *
	 *  see also:
	 *  http://msdn.microsoft.com/en-us/library/bb762115(VS.85).aspx
	 *  http://msdn.microsoft.com/en-us/library/bb773205(VS.85).aspx
	 */
	/* ----------------------------------------------------------------- */
	static std::basic_string<TCHAR> browsefolder(const TCHAR* description) {
		typedef TCHAR char_type;
		char_type path[CUBE_MAX_PATH] = {};
		GetCurrentDirectory(CUBE_MAX_PATH, path);
		
		BROWSEINFO info = {};
		info.pszDisplayName = path;
		info.lpszTitle = description;
		info.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
		info.lpfn = &browse_proc;
		info.lParam = (LPARAM)path;
		LPITEMIDLIST dest = SHBrowseForFolder(&info);
		
		SHGetPathFromIDList(dest, path);
		CoTaskMemFree(dest);
		
		return path;
	}
	
	namespace detail {
		/* ----------------------------------------------------------------- */
		/*
		 *  change_flag
		 *
		 *  �`�F�b�N�{�b�N�X�̕ω����������Ƃ��� flags �̒l���X�V����
		 *  ���߂̕⏕�֐��D
		 */
		/* ----------------------------------------------------------------- */
		static void change_flag(std::size_t& dest, HWND handle, int id, std::size_t kind) {
			if (IsDlgButtonChecked(handle, id) == BST_CHECKED) dest |= kind;
			else dest &= ~kind;
		}
		
		/* ---------------------------------------------------------------- */
		/*
		 *  archive_initdialog
		 *
		 *  �u���k�v�^�u����сu�𓀁v�^�u�̏����ݒ�
		 */
		/* ---------------------------------------------------------------- */
		static void archive_initdialog(HWND hWnd, user_setting::archive_type& setting) {
			// �u�o�͐�t�H���_�v�O���[�v
			if (setting.output_condition() == OUTPUT_SPECIFIC) {
				CheckDlgButton(hWnd, IDC_SPECIFIC_RADIO, BM_SETCHECK);
				EnableWindow(GetDlgItem(hWnd, IDC_DEST_TEXTBOX), TRUE);
				EnableWindow(GetDlgItem(hWnd, IDC_DEST_BUTTON), TRUE);
			}
			else if (setting.output_condition() == OUTPUT_SOURCE) {
				CheckDlgButton(hWnd, IDC_SOURCE_RADIO, BM_SETCHECK);
				EnableWindow(GetDlgItem(hWnd, IDC_DEST_TEXTBOX), FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_DEST_BUTTON), FALSE);
			}
			else {
				CheckDlgButton(hWnd, IDC_RUNTIME_RADIO, BM_SETCHECK);
				EnableWindow(GetDlgItem(hWnd, IDC_DEST_TEXTBOX), FALSE);
				EnableWindow(GetDlgItem(hWnd, IDC_DEST_BUTTON), FALSE);
			}
			
			if (!setting.output_path().empty()) {
				SetWindowText(GetDlgItem(hWnd, IDC_DEST_TEXTBOX), setting.output_path().c_str());
			}
			
			// �u�ڍׁv�O���[�v
			const flag_map& detail = detail_map();
			for (flag_map::const_iterator pos = detail.begin(); pos != detail.end(); ++pos) {
				if (setting.details() & pos->second) CheckDlgButton(hWnd, pos->first, BM_SETCHECK);
			}
			
			if ((setting.details() & DETAIL_OVERWRITE) == 0) {
				EnableWindow(GetDlgItem(hWnd, IDC_NEWER_CHECKBOX), FALSE);
			}
			
			if ((setting.details() & DETAIL_OPEN) == 0) {
				EnableWindow(GetDlgItem(hWnd, IDC_SKIP_DESKTOP_CHECKBOX), FALSE);
			}
		}

		/* ---------------------------------------------------------------- */
		/*
		 *  general_initdialog
		 * 
		 *  �u��ʁv�^�u�̏����ݒ�
		 */
		/* ---------------------------------------------------------------- */
		static void general_initdialog(HWND hWnd) {
			// �u�֘A�t���v�O���[�v
			const flag_map& decomp = decompress_map();
			for (flag_map::const_iterator pos = decomp.begin(); pos != decomp.end(); ++pos) {
				if (Setting.decompression().flags() & pos->second) {
					CheckDlgButton(hWnd, pos->first, BM_SETCHECK);
				}
			}
			
			// �R���e�L�X�g���j���[�̈��k�̃T�u����
			const flag_map& comp = compress_map();
			for (flag_map::const_iterator pos = comp.begin(); pos != comp.end(); ++pos) {
				if (Setting.compression().flags() & pos->second) {
					CheckDlgButton(hWnd, pos->first, BM_SETCHECK);
				}
			}
			
			// �u�R���e�L�X�g���j���[�v�O���[�v
			const flag_map& context = context_map();
			for (flag_map::const_iterator pos = context.begin(); pos != context.end(); ++pos) {
				if (Setting.context_flags() & pos->second) {
					CheckDlgButton(hWnd, pos->first, BM_SETCHECK);
				}
				else if (pos->second == COMPRESS_FLAG) {
					// ���k�̃T�u���ڂ𖳌���
					for (flag_map::const_iterator subpos = comp.begin(); subpos != comp.end(); ++subpos) {
						EnableWindow(GetDlgItem(hWnd, subpos->first), FALSE);
					}
				}
			}

			// �u�V���[�g�J�b�g�v�O���[�v
			const flag_map& shortcut = shortcut_map();
			for (flag_map::const_iterator pos = shortcut.begin(); pos != shortcut.end(); ++pos) {
				if (Setting.shortcut_flags() & pos->second) {
					CheckDlgButton(hWnd, pos->first, BM_SETCHECK);
				}
			}
			
			// �V���[�g�J�b�g�̈��k�̎��
			HWND combo = GetDlgItem(hWnd, IDC_SC_TYPE_COMBOBOX);
			const param_map& params = compress_parameters();
			for (param_map::const_iterator pos = params.begin(); pos != params.end(); ++pos) {
				SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)pos->second.first.c_str());
			}
			SendMessage(combo, CB_SETCURSEL, Setting.shortcut_compress_index(), 0L);
		}
		
		/* ---------------------------------------------------------------- */
		//  filter_initdialog
		/* ---------------------------------------------------------------- */
		static void filter_initdialog(HWND hWnd) {
			if (!Setting.filters().empty()) {
				std::basic_string<TCHAR> s;
				clx::join(Setting.filters(), s, _T("\r\n"));
				HWND handle = GetDlgItem(hWnd, IDC_FILTER_TEXTBOX);
				SetWindowText(handle, s.c_str());
			}
		}
		
		/* ----------------------------------------------------------------- */
		//  filter_gettext
		/* ----------------------------------------------------------------- */
		static void filter_gettext(HWND hWnd) {
			const int maxlen = 64 * 1024;
			int len = GetWindowTextLength(GetDlgItem(hWnd, IDC_FILTER_TEXTBOX));
			if (len < maxlen) {  
				cubeice::user_setting::char_type buffer[maxlen] = {};
				std::size_t status = GetDlgItemText(hWnd, IDC_FILTER_TEXTBOX, (LPSTR)buffer,  sizeof(buffer));
				if (status > 0 || GetLastError() == 0) {
					cubeice::user_setting::string_type s(buffer);
					Setting.filters().clear();
					clx::split_if(s, Setting.filters(), clx::is_any_of("\r\n"));
				}
			}
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  common_dialogproc
		 *
		 *  �e��{�^�����������Ƃ��̋����ȂǁC�S�Ẵ^�u�y�[�W�ŋ��ʂ�
		 *  ������L�q����D
		 */
		/* ----------------------------------------------------------------- */
		static BOOL CALLBACK common_dialogproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
			switch (msg) {
			case WM_DESTROY:
			case WM_CLOSE:
				PostQuitMessage(0);
				break;
			case WM_NOTIFY:
			{
				NMHDR* nmhdr = (NMHDR *)lp;
				if (nmhdr->code == PSN_APPLY) {
					filter_gettext(hWnd);
					Setting.save();
				}
				break;
			}
			default:
				break;
			}
			return FALSE;
		}
		
		/* ----------------------------------------------------------------- */
		//  general_dialogproc
		/* ----------------------------------------------------------------- */
		static BOOL CALLBACK general_dialogproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
			switch (msg) {
			case WM_INITDIALOG:
				general_initdialog(hWnd);
				return TRUE;
			case WM_COMMAND:
			{
				std::size_t parameter = LOWORD(wp);
				
				// �u�֘A�t���v�O���[�v
				const flag_map& decomp = decompress_map();
				flag_map::const_iterator pos = decomp.find(parameter);
				if (pos != decomp.end()) {
					change_flag(Setting.decompression().flags(), hWnd, pos->first, pos->second);
					return TRUE;
				}
				
				// �R���e�L�X�g���j���[�̈��k�̃T�u����
#ifdef nouse
				const flag_map& comp = compress_map();
				pos = comp.find(parameter);
				if (pos != comp.end()) {
					change_flag(Setting.compression().flags(), hWnd, pos->first, pos->second);
					return TRUE;
				}
#endif
				
				// �u�R���e�L�X�g���j���[�v�O���[�v
				const flag_map& context = context_map();
				pos = context.find(parameter);
				if (pos != context.end()) {
					change_flag(Setting.context_flags(), hWnd, pos->first, pos->second);
					
					// ���k�̃T�u���ڂ̗L��/������ύX����D
#ifdef nouse
					if (pos->first == IDC_CT_COMPRESS_CHECKBOX) {
						BOOL enabled = IsDlgButtonChecked(hWnd, IDC_CT_COMPRESS_CHECKBOX);
						for (flag_map::const_iterator subpos = comp.begin(); subpos != comp.end(); ++subpos) {
							EnableWindow(GetDlgItem(hWnd, subpos->first), enabled);
						}
					}
#endif
					return TRUE;
				}
				
				// �u�R���e�L�X�g���j���[�v�O���[�v
				const flag_map& shortcut = shortcut_map();
				pos = shortcut.find(parameter);
				if (pos != shortcut.end()) {
					change_flag(Setting.shortcut_flags(), hWnd, pos->first, pos->second);
					return TRUE;
				}
				
				if (parameter == IDC_SC_TYPE_COMBOBOX) {
					Setting.shortcut_compress_index() = SendMessage(GetDlgItem(hWnd, IDC_SC_TYPE_COMBOBOX), CB_GETCURSEL, 0, 0);
					return TRUE;
				}
				
				break;
			}
			default:
				break;
			}
			return common_dialogproc(hWnd, msg, wp, lp);
		}
		
		/* ---------------------------------------------------------------- */
		/*
		 *  archive_dialogproc
		 *
		 *  compress_dialogproc(), decompress_dialogproc() ���ʂ̏����D
		 */
		/* ---------------------------------------------------------------- */
		static BOOL archive_dialogproc(user_setting::archive_type& setting, HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
			std::size_t parameter = LOWORD(wp);
			
			// �u�o�͐�t�H���_�v�O���[�v
			const flag_map& output = output_map();
			flag_map::const_iterator pos = output.find(parameter);
			if (pos != output.end()) {
				setting.output_condition() = pos->second;
				BOOL enabled = (setting.output_condition() == OUTPUT_SPECIFIC) ? TRUE : FALSE;
				EnableWindow(GetDlgItem(hWnd, IDC_DEST_TEXTBOX), enabled);
				EnableWindow(GetDlgItem(hWnd, IDC_DEST_BUTTON), enabled);
				return TRUE;
			}
			
			// �u�ڍׁv�O���[�v
			const flag_map& detail = detail_map();
			pos = detail.find(parameter);
			if (pos != detail.end()) {
				change_flag(setting.details(), hWnd, pos->first, pos->second);
				if (pos->first == IDC_OVERWRITE_CHECKBOX) {
					BOOL enabled = (setting.details() & DETAIL_OVERWRITE) ? TRUE : FALSE;
					EnableWindow(GetDlgItem(hWnd, IDC_NEWER_CHECKBOX), enabled);
				}
				
				if (pos->first == IDC_POSTOPEN_CHECKBOX) {
					BOOL enabled = (setting.details() & DETAIL_OPEN) ? TRUE : FALSE;
					EnableWindow(GetDlgItem(hWnd, IDC_SKIP_DESKTOP_CHECKBOX), enabled);
				}
				
				return TRUE;
			}
			
			if (parameter == IDC_DEST_BUTTON) {
				std::basic_string<TCHAR> dest = browsefolder(_T("�ۑ���f�B���N�g����I�����ĉ������B"));
				if (!dest.empty()) {
					setting.output_path() = dest;
					SetWindowText(GetDlgItem(hWnd, IDC_DEST_TEXTBOX), dest.c_str());
				}
			}
			return common_dialogproc(hWnd, msg, wp, lp);
		}
		
		/* ----------------------------------------------------------------- */
		//  compress_dialogproc
		/* ----------------------------------------------------------------- */
		static BOOL CALLBACK compress_dialogproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
			switch (msg) {
			case WM_INITDIALOG:
				archive_initdialog(hWnd, Setting.compression());
				break;
			case WM_COMMAND:
				return archive_dialogproc(Setting.compression(), hWnd, msg, wp, lp);
			default:
				break;
			}
			return common_dialogproc(hWnd, msg, wp, lp);
		}
		
		/* ----------------------------------------------------------------- */
		//  decompress_dialogproc
		/* ----------------------------------------------------------------- */
		static BOOL CALLBACK decompress_dialogproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
			switch (msg) {
			case WM_INITDIALOG:
				archive_initdialog(hWnd, Setting.decompression());
				break;
			case WM_COMMAND:
				return archive_dialogproc(Setting.decompression(), hWnd, msg, wp, lp);
			default:
				break;
			}
			return common_dialogproc(hWnd, msg, wp, lp);
		}
		
		/* ----------------------------------------------------------------- */
		//  filter_dialogproc
		/* ----------------------------------------------------------------- */
		static BOOL CALLBACK filter_dialogproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
			switch (msg) {
			case WM_INITDIALOG:
				filter_initdialog(hWnd);
				break;
			default:
				break;
			}
			return common_dialogproc(hWnd, msg, wp, lp);
		}
		
		/* ----------------------------------------------------------------- */
		//  version_dialogproc
		/* ----------------------------------------------------------------- */
		static BOOL CALLBACK version_dialogproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
			switch (msg) {
			case WM_INITDIALOG:
			{
				// �A�C�R��
				HANDLE logo = LoadImage(GetModuleHandle(NULL), _T("IDI_APP"), IMAGE_ICON, 48, 48, LR_DEFAULTCOLOR | LR_SHARED | LR_VGACOLOR);
				HWND pic = GetDlgItem(hWnd, IDC_LOGO_PICTUREBOX);
				SendMessage(pic, STM_SETIMAGE, IMAGE_ICON, LPARAM(logo));
				
				// �o�[�W����
				HKEY subkey;
				LONG result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, CUBEICE_REG_ROOT, 0, KEY_READ, &subkey);
				if (!result) {
					TCHAR buffer[1024] = {};
					DWORD type = 0;
					DWORD size = 1024;
					if (RegQueryValueEx(subkey, CUBEICE_REG_VERSION, NULL, &type, (LPBYTE)buffer, &size) == ERROR_SUCCESS) {
						std::basic_string<TCHAR> message = "Version: ";
						message += buffer;
						if (sizeof(INT_PTR) == 4) message += " (x86)";
						else message += " (x64)";
						SetWindowText(GetDlgItem(hWnd, IDC_VERSION_LABEL), message.c_str());
					}
				}
				
				if (Setting.update()) CheckDlgButton(hWnd, IDC_UPDATE_CHECKBOX, BM_SETCHECK);
				
				break;
			}
			case WM_COMMAND:
				switch (LOWORD(wp)) {
				case IDC_UPDATE_CHECKBOX:
					Setting.update() = (IsDlgButtonChecked(hWnd, IDC_UPDATE_CHECKBOX) == BST_CHECKED);
					return TRUE;
				}
				break;
			default:
				break;
			}
			return common_dialogproc(hWnd, msg, wp, lp);
		}
	}
	
	/* --------------------------------------------------------------------- */
	//  create_propsheet
	/* --------------------------------------------------------------------- */
	int create_propsheet(HWND parent) {
		static const std::size_t pagenum = 5;
		HPROPSHEETPAGE ref[pagenum] = {};
		
		PROPSHEETPAGE page = {};
		page.dwSize = sizeof (PROPSHEETPAGE);
		page.dwFlags = PSP_DEFAULT | PSP_USEICONID;
		page.hInstance = GetModuleHandle(NULL);
		
		// �u��ʁv�y�[�W
		page.pszTemplate = _T("IDD_GENERAL");
		page.pfnDlgProc = (DLGPROC)detail::general_dialogproc;
		ref[0] = CreatePropertySheetPage(&page);
		
		// �u���k�v�y�[�W
		page.pszTemplate = _T("IDD_COMPRESS");
		page.pfnDlgProc = (DLGPROC)detail::compress_dialogproc;
		ref[1] = CreatePropertySheetPage(&page);
		
		// �u�𓀁v�y�[�W
		page.pszTemplate = _T("IDD_DECOMPRESS");
		page.pfnDlgProc = (DLGPROC)detail::decompress_dialogproc;
		ref[2] = CreatePropertySheetPage(&page);
		
		// �u�t�B���^�����O�v�y�[�W
		page.pszTemplate = _T("IDD_FILTER");
		page.pfnDlgProc = (DLGPROC)detail::filter_dialogproc;
		ref[3] = CreatePropertySheetPage(&page);

		// �u�o�[�W�������v�y�[�W
		page.pszTemplate = _T("IDD_VERSION");
		page.pfnDlgProc = (DLGPROC)detail::version_dialogproc;;
		ref[4] = CreatePropertySheetPage(&page);
		
		PROPSHEETHEADER psh = {};
		psh.dwSize = sizeof (PROPSHEETHEADER);
		psh.dwFlags = PSH_DEFAULT | PSH_USEHICON  | PSH_NOAPPLYNOW;
		psh.hwndParent = parent;
		psh.hIcon = LoadIcon(page.hInstance, _T("IDI_APP"));
		psh.pszCaption = _T("CubeICE �ݒ�");
		psh.nPages = pagenum;
		psh.phpage = ref;
		
		return PropertySheet(&psh);
	}
}