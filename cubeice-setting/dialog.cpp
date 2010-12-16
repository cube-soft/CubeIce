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
#include <cstdlib>
#include <tchar.h>
#include <windows.h>
#include <commctrl.h>
#include "dialog.h"

namespace cubeice {
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
		 *  detail_initdialog
		 *
		 *  �u���k�v�^�u����сu�𓀁v�^�u�̏����ݒ�
		 */
		/* ---------------------------------------------------------------- */
		static void detail_initdialog(HWND hWnd, user_setting_property& detail) {
			// �u�o�͐�t�H���_�v�O���[�v
			if (detail.output_condition() & OUTPUT_SPECIFIC) {
				CheckDlgButton(hWnd, IDC_SPECIFIC_CHECKBOX, BM_SETCHECK);
			}
			if (detail.output_condition() & OUTPUT_SOURCE) {
				CheckDlgButton(hWnd, IDC_SOURCE_CHECKBOX, BM_SETCHECK);
			}
			if (detail.output_condition() & OUTPUT_RUNTIME) {
				CheckDlgButton(hWnd, IDC_RUNTIME_CHECKBOX, BM_SETCHECK);
			}

			// �u�ڍׁv�O���[�v
			if (detail.overwrite() & OVERWRITE_NOTIFY) {
				CheckDlgButton(hWnd, IDC_OVERWRITE_CHECKBOX, BM_SETCHECK);
			}
			if (detail.overwrite() & OVERWRITE_NEWER) {
				CheckDlgButton(hWnd, IDC_NEWER_CHECKBOX, BM_SETCHECK);
			}
			if (detail.conv_charset()) {
				CheckDlgButton(hWnd, IDC_CHARCODE_CHECKBOX, BM_SETCHECK);
			}
			if (detail.filter()) {
				CheckDlgButton(hWnd, IDC_FILTER_CHECKBOX, BM_SETCHECK);
			}
			if (detail.postopen()) {
				CheckDlgButton(hWnd, IDC_POSTOPEN_CHECKBOX, BM_SETCHECK);
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
			
			// �u�R���e�L�X�g���j���[�v�O���[�v
			const flag_map& context = context_map();
			for (flag_map::const_iterator pos = context.begin(); pos != context.end(); ++pos) {
				if (Setting.context_flags() & pos->second) {
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
				break;
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
				
				// �u�R���e�L�X�g���j���[�v�O���[�v
				const flag_map& context = context_map();
				pos = context.find(parameter);
				if (pos != context.end()) {
					change_flag(Setting.context_flags(), hWnd, pos->first, pos->second);
					return TRUE;
				}
				
				// �R���e�L�X�g���j���[�̈��k�̃T�u����
				const flag_map& comp = compress_map();
				pos = comp.find(parameter);
				if (pos != comp.end()) {
					change_flag(Setting.compression().flags(), hWnd, pos->first, pos->second);
					return TRUE;
				}

				return common_dialogproc(hWnd, msg, wp, lp);
			}
			default:
				return common_dialogproc(hWnd, msg, wp, lp);
			}
			return FALSE;
		}
				
		/* ----------------------------------------------------------------- */
		//  compress_dialogproc
		/* ----------------------------------------------------------------- */
		static BOOL CALLBACK archive_dialogproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
			switch (msg) {
			case WM_INITDIALOG:
				detail_initdialog(hWnd, Setting.compression());
				break;
			case WM_COMMAND:
				switch(LOWORD(wp)) {
				case IDC_SPECIFIC_CHECKBOX:
					change_flag(Setting.compression().output_condition(), hWnd, IDC_SPECIFIC_CHECKBOX, OUTPUT_SPECIFIC);
					return TRUE;
				case IDC_SOURCE_CHECKBOX:
					change_flag(Setting.compression().output_condition(), hWnd, IDC_SOURCE_CHECKBOX, OUTPUT_SOURCE);
					return TRUE;
				case IDC_RUNTIME_CHECKBOX:
					change_flag(Setting.compression().output_condition(), hWnd, IDC_RUNTIME_CHECKBOX, OUTPUT_RUNTIME);
					return TRUE;
				case IDC_FILTER_CHECKBOX:
					Setting.decompression().filter() = (IsDlgButtonChecked(hWnd, IDC_FILTER_CHECKBOX) == BST_CHECKED);
					return TRUE;
				case IDC_OVERWRITE_CHECKBOX:
					change_flag(Setting.compression().overwrite(), hWnd, IDC_OVERWRITE_CHECKBOX, OVERWRITE_NOTIFY);
					return TRUE;
				case IDC_NEWER_CHECKBOX:
					change_flag(Setting.compression().overwrite(), hWnd, IDC_NEWER_CHECKBOX, OVERWRITE_NEWER);
					return TRUE;
				case IDC_POSTOPEN_CHECKBOX:
					Setting.decompression().postopen() = (IsDlgButtonChecked(hWnd, IDC_POSTOPEN_CHECKBOX) == BST_CHECKED);
					return TRUE;
				default:
					return common_dialogproc(hWnd, msg, wp, lp);
				}
			default:
				return common_dialogproc(hWnd, msg, wp, lp);
			}
			return FALSE;
		}
		
		/* ----------------------------------------------------------------- */
		//  decompress_dialogproc
		/* ----------------------------------------------------------------- */
		static BOOL CALLBACK expand_dialogproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
			switch (msg) {
			case WM_INITDIALOG:
				detail_initdialog(hWnd, Setting.decompression());
			case WM_COMMAND:
				switch(LOWORD(wp)) {
				case IDC_SPECIFIC_CHECKBOX:
					change_flag(Setting.decompression().output_condition(), hWnd, IDC_SPECIFIC_CHECKBOX, OUTPUT_SPECIFIC);
					return TRUE;
				case IDC_SOURCE_CHECKBOX:
					change_flag(Setting.decompression().output_condition(), hWnd, IDC_SOURCE_CHECKBOX, OUTPUT_SOURCE);
					return TRUE;
				case IDC_RUNTIME_CHECKBOX:
					change_flag(Setting.decompression().output_condition(), hWnd, IDC_RUNTIME_CHECKBOX, OUTPUT_RUNTIME);
					return TRUE;
				case IDC_FILTER_CHECKBOX:
					Setting.decompression().filter() = (IsDlgButtonChecked(hWnd, IDC_FILTER_CHECKBOX) == BST_CHECKED);
					return TRUE;
				case IDC_OVERWRITE_CHECKBOX:
					change_flag(Setting.decompression().overwrite(), hWnd, IDC_OVERWRITE_CHECKBOX, OVERWRITE_NOTIFY);
					return TRUE;
				case IDC_NEWER_CHECKBOX:
					change_flag(Setting.decompression().overwrite(), hWnd, IDC_NEWER_CHECKBOX, OVERWRITE_NEWER);
					return TRUE;
				case IDC_MAKE_FOLDER_CHECKBOX:
					Setting.decompression().create_folder() = (IsDlgButtonChecked(hWnd, IDC_MAKE_FOLDER_CHECKBOX) == BST_CHECKED);
					return TRUE;
				case IDC_CHARCODE_CHECKBOX:
					Setting.decompression().conv_charset() = (IsDlgButtonChecked(hWnd, IDC_CHARCODE_CHECKBOX) == BST_CHECKED);
					return TRUE;
				case IDC_POSTOPEN_CHECKBOX:
					Setting.decompression().postopen() = (IsDlgButtonChecked(hWnd, IDC_POSTOPEN_CHECKBOX) == BST_CHECKED);
					return TRUE;
				default:
					return common_dialogproc(hWnd, msg, wp, lp);
				}
			default:
				return common_dialogproc(hWnd, msg, wp, lp);
			}
			return FALSE;
		}
		
		/* ----------------------------------------------------------------- */
		//  filter_dialogproc
		/* ----------------------------------------------------------------- */
		static BOOL CALLBACK filter_dialogproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
			switch (msg) {
			case WM_COMMAND:
				break;
			default:
				return common_dialogproc(hWnd, msg, wp, lp);
			}
			return FALSE;
		}

		/* ----------------------------------------------------------------- */
		//  version_dialogproc
		/* ----------------------------------------------------------------- */
		static BOOL CALLBACK version_dialogproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
			switch (msg) {
			case WM_INITDIALOG:
			{
				break;
			}
			case WM_COMMAND:
				break;
			default:
				return common_dialogproc(hWnd, msg, wp, lp);
			}
			return FALSE;
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
		page.pszTemplate = _T("GENERAL_TABPAGE");
		page.pfnDlgProc = (DLGPROC)detail::general_dialogproc;
		ref[0] = CreatePropertySheetPage(&page);
		
		// �u���k�v�y�[�W
		page.pszTemplate = _T("ARCHIVE_TABPAGE");
		page.pfnDlgProc = (DLGPROC)detail::archive_dialogproc;
		ref[1] = CreatePropertySheetPage(&page);
		
		// �u�𓀁v�y�[�W
		page.pszTemplate = _T("EXPAND_TABPAGE");
		page.pfnDlgProc = (DLGPROC)detail::expand_dialogproc;
		ref[2] = CreatePropertySheetPage(&page);
		
		// �u�t�B���^�����O�v�y�[�W
		page.pszTemplate = _T("FILTER_TABPAGE");
		page.pfnDlgProc = (DLGPROC)detail::filter_dialogproc;
		ref[3] = CreatePropertySheetPage(&page);

		// �u�o�[�W�������v�y�[�W
		page.pszTemplate = _T("VERSION_TABPAGE");
		page.pfnDlgProc = (DLGPROC)detail::version_dialogproc;;
		ref[4] = CreatePropertySheetPage(&page);
		
		PROPSHEETHEADER psh = {};
		psh.dwSize = sizeof (PROPSHEETHEADER);
		psh.dwFlags = PSH_DEFAULT | PSH_USEHICON;
		psh.hwndParent = parent;
		psh.hIcon = LoadIcon(page.hInstance, _T("CUBEICE_ICON"));
		psh.pszCaption = _T("CubeICE �ݒ�");
		psh.nPages = pagenum;
		psh.phpage = ref;
		
		return PropertySheet(&psh);
	}
}