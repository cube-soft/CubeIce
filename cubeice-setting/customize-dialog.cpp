/* ------------------------------------------------------------------------- */
/*
 *  customize-dialog.cpp
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
#include <Windows.h>
#include <CommCtrl.h>
#include <vector>
#include <map>
#include "../cubeice-ctx/cubeice-ctxdata.h"

#define		SUBMENU_ROOT_ID		static_cast<LPARAM>(-1)
#define		SUBMENU_DIR_ID		static_cast<LPARAM>(0)

namespace cubeice {
	namespace dialog {
		/* ----------------------------------------------------------------- */
		//  InitMenuList
		/* ----------------------------------------------------------------- */
		void InitMenuList(HWND hTreeView, HTREEITEM hParent, const SUB_MENU_ITEM *smi)
		{
			TVINSERTSTRUCT	tvi;

			tvi.hParent			= hParent;
			tvi.hInsertAfter	= TVI_LAST;
			tvi.item.mask		= TVIF_TEXT | TVIF_PARAM;

			for(int i = 0 ; smi[i].stringA ; ++i) {
				HTREEITEM	hTreeItem;

#ifdef	UNICODE
				tvi.item.pszText	= const_cast<LPWSTR>(smi[i].stringW);
#else
				tvi.item.pszText	= const_cast<LPSTR>(smi[i].stringA);
#endif
				tvi.item.lParam		= smi[i].dispSetting;

				hTreeItem = TreeView_InsertItem(hTreeView, &tvi);
				if(smi[i].submenu)
					InitMenuList(hTreeView, hTreeItem, smi[i].submenu);
			}

		}

		/* ----------------------------------------------------------------- */
		//  SerializeMenuItemData
		/* ----------------------------------------------------------------- */
		void SerializeMenuItemData(std::map<cubeice::user_setting::size_type, const TCHAR*> &table, const SUB_MENU_ITEM *smi)
		{
			for(int i = 0 ; smi[i].stringA ; ++i) {
#ifdef	UNICODE
				table[smi[i].dispSetting] = smi[i].stringW;
#else
				table[smi[i].dispSetting] = smi[i].stringA;
#endif
				if(smi[i].submenu)
					SerializeMenuItemData(table, smi[i].submenu);
			}
		}

		/* ----------------------------------------------------------------- */
		//  SetMenuItem
		/* ----------------------------------------------------------------- */
		void SetMenuItem(HWND hMenuTreeView, HTREEITEM hParentItem, const std::vector<cubeice::user_setting::SUBMENU> &submenu, const std::map<cubeice::user_setting::size_type, const TCHAR*> &table)
		{
			TVINSERTSTRUCT	tvi;
			tvi.hParent			= hParentItem;
			tvi.hInsertAfter	= TVI_LAST;
			tvi.item.mask		= TVIF_TEXT | TVIF_PARAM;

			BOOST_FOREACH(const cubeice::user_setting::SUBMENU &s, submenu) {
				if(s.id == SUBMENU_DIR_ID || table.find(s.id) != table.end()) {
					HTREEITEM					hItem;

					// NOTE: �K��̍��ڂ����[�U�����l�[���\�ɕύX�������߁C
					// �ǂ̂悤�ȏꍇ�ł� SUBMENU �ɕۑ�����Ă��镶������g�p����D
					// tvi.item.pszText	= (s.id == SUBMENU_DIR_ID) ? const_cast<LPTSTR>(s.str.c_str()) : const_cast<LPTSTR>(table.find(s.id)->second);
					tvi.item.pszText = const_cast<LPTSTR>(s.str.c_str());
					tvi.item.lParam		= s.id;
					hItem = TreeView_InsertItem(hMenuTreeView, &tvi);

					SetMenuItem(hMenuTreeView, hItem, s.children, table);
				}
			}
		}

		/* ----------------------------------------------------------------- */
		//  InitTreeViewItem
		/* ----------------------------------------------------------------- */
		void InitTreeViewItem(HWND hMenuTreeView, HWND hItemListTreeView, const std::vector<cubeice::user_setting::SUBMENU> &submenu)
		{
			HTREEITEM	hRootItem;

			InitMenuList(hItemListTreeView, TVI_ROOT, MenuItem);

			TVINSERTSTRUCT	tvi;
			tvi.hParent			= TVI_ROOT;
			tvi.hInsertAfter	= TVI_LAST;
			tvi.item.mask		= TVIF_TEXT | TVIF_PARAM;
			tvi.item.pszText	= TEXT("�V�����t�H���_��ǉ�");
			tvi.item.lParam		= SUBMENU_DIR_ID;
			TreeView_InsertItem(hItemListTreeView, &tvi);

			tvi.item.mask		= TVIF_TEXT | TVIF_PARAM;
			tvi.item.pszText	= TEXT("���[�g");
			tvi.item.lParam		= SUBMENU_ROOT_ID;
			hRootItem = TreeView_InsertItem(hMenuTreeView, &tvi);

			std::map<cubeice::user_setting::size_type, const TCHAR*>	table;
			SerializeMenuItemData(table, MenuItem);
			SetMenuItem(hMenuTreeView, hRootItem, submenu, table);

			TreeView_Select(hMenuTreeView, hRootItem, TVGN_CARET);
			TreeView_Expand(hMenuTreeView, hRootItem, TVE_EXPAND);
		}
		
		/* ----------------------------------------------------------------- */
		//  GetAlternativeString
		/* ----------------------------------------------------------------- */
		TCHAR* GetAlternativeString(int id) {
			for (int i = 0; SubMenuCompress[i].stringA; ++i) {
#ifdef UNICODE
				if (id == SubMenuCompress[i].dispSetting) return const_cast<TCHAR*>(SubMenuCompress[i].explanationW);
#else
				if (id == SubMenuCompress[i].dispSetting) return const_cast<TCHAR*>(SubMenuCompress[i].explanationA);
#endif
			}
			
			for (int i = 0; SubMenuCompAndMail[i].stringA; ++i) {
#ifdef UNICODE
				if (id == SubMenuCompAndMail[i].dispSetting) return const_cast<TCHAR*>(SubMenuCompAndMail[i].explanationW);
#else
				if (id == SubMenuCompAndMail[i].dispSetting) return const_cast<TCHAR*>(SubMenuCompAndMail[i].explanationA);
#endif
			}

			for (int i = 0; SubMenuDecompress[i].stringA; ++i) {
#ifdef UNICODE
				if (id == SubMenuDecompress[i].dispSetting) return const_cast<TCHAR*>(SubMenuDecompress[i].explanationW);
#else
				if (id == SubMenuDecompress[i].dispSetting) return const_cast<TCHAR*>(SubMenuDecompress[i].explanationA);
#endif
			}

			return NULL;
		}
		
		/* ----------------------------------------------------------------- */
		//  CopyTreeViewItem
		/* ----------------------------------------------------------------- */
		HTREEITEM CopyTreeViewItem(HWND &hTo, HTREEITEM hToParentItem, HWND &hFrom, HTREEITEM hFromParentItem, const HTREEITEM &InsertAfter = TVI_LAST)
		{
			static	TCHAR	str[2048];
			TVINSERTSTRUCT	tvi;
			HTREEITEM	hInsertItem, hChildItem;

			tvi.hParent			= hToParentItem;
			tvi.hInsertAfter	= InsertAfter;
			tvi.item.mask		= TVIF_HANDLE | TVIF_TEXT | TVIF_PARAM;
			tvi.item.hItem		= hFromParentItem;
			tvi.item.pszText	= str;
			tvi.item.cchTextMax	= sizeof(str) / sizeof(str[0]);

			TreeView_GetItem(hFrom, &tvi.item);
			tvi.item.mask		= TVIF_TEXT | TVIF_PARAM;

			// ���[�g�����Ƀ��[�t�v�f��ǉ�����ꍇ�́C�\�����镶�����ς���D
			if (hToParentItem == TreeView_GetRoot(hTo)) {
				TCHAR* alt = GetAlternativeString(static_cast<int>(tvi.item.lParam));
				if (alt) tvi.item.pszText = alt;
			}
			
			hInsertItem = TreeView_InsertItem(hTo, &tvi);
			hChildItem = TreeView_GetChild(hFrom, hFromParentItem);
			while(hChildItem) {
				CopyTreeViewItem(hTo, hInsertItem, hFrom, hChildItem);
				hChildItem = TreeView_GetNextSibling(hFrom, hChildItem);
			}
			return hInsertItem;
		}

		/* ----------------------------------------------------------------- */
		//  GetSubmenuStruct
		/* ----------------------------------------------------------------- */
		void GetSubmenuStruct(std::vector<cubeice::user_setting::SUBMENU> &submenu, HWND hMenuTreeView, HTREEITEM hParentItem)
		{
			HTREEITEM	hChildItem;

			submenu.clear();
			hChildItem = TreeView_GetChild(hMenuTreeView, hParentItem);
			while(hChildItem) {
				static	TCHAR	str[2048];
				TVITEM	tvi;
				tvi.mask		= TVIF_HANDLE | TVIF_TEXT | TVIF_PARAM;
				tvi.hItem		= hChildItem;
				tvi.pszText		= str;
				tvi.cchTextMax	= sizeof(str) / sizeof(str[0]);

				TreeView_GetItem(hMenuTreeView, &tvi);
				submenu.push_back(cubeice::user_setting::SUBMENU(static_cast<int>(tvi.lParam), str));

				GetSubmenuStruct(submenu.back().children, hMenuTreeView, hChildItem);
				hChildItem = TreeView_GetNextSibling(hMenuTreeView, hChildItem);
			}
		}

		/* ----------------------------------------------------------------- */
		//  CheckValidation
		/* ----------------------------------------------------------------- */
		bool CheckValidation(const std::vector<cubeice::user_setting::SUBMENU> &v)
		{
			if(!v.size())
				return false;
			BOOST_FOREACH(const cubeice::user_setting::SUBMENU &s, v) {
				bool	f = false;
				if(s.id == SUBMENU_DIR_ID) {
					f = true;
				} else {
					for(int i = 0 ; MenuItem[i].stringA ; ++i) {
						if(MenuItem[i].dispSetting == s.id) {
							f = true;
							break;
						}
					}
				}
				if(f && !CheckValidation(s.children))
					return false;
			}
			return true;
		}
		
		/* ----------------------------------------------------------------- */
		//  NOTE: �G���^�[�L�[���t�b�N���邽��
		/* ----------------------------------------------------------------- */
		static HHOOK hKeybordHook;
		static HWND hTreeMenu;
		static HWND hTreeOrg;
		
		/* ----------------------------------------------------------------- */
		/*
		 *  KeyboardProc
		 *
		 *  �G���^�[�L�[�������ꂽ�ۂɁCTreeView �̍��ڂ��ҏW��Ԃ�����
		 *  �ꍇ�͕ҏW���m�肵�ďI���D
		 */
		/* ----------------------------------------------------------------- */
		static LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
			if (code == HC_ACTION) {
				if (wParam == 0x0D) {
					if (TreeView_GetEditControl(hTreeMenu)) {
						TreeView_EndEditLabelNow(hTreeMenu, FALSE);
						return TRUE;
					}
				}
			}
			return CallNextHookEx(hKeybordHook, code, wParam, lParam);
		}

		/* ----------------------------------------------------------------- */
		//  customize_wndproc
		/* ----------------------------------------------------------------- */
		static INT_PTR CALLBACK customize_wndproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
			static cubeice::user_setting *setting;
			
			switch (msg) {
			case WM_INITDIALOG:
			{
				hTreeMenu = GetDlgItem(hWnd, IDC_CURRENT_TREEVIEW);
				hTreeOrg = GetDlgItem(hWnd, IDC_ADD_TREEVIEW);
				hKeybordHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, 0, GetCurrentThreadId());
				
				setting = reinterpret_cast<cubeice::user_setting*>(lp);
				InitTreeViewItem(hTreeMenu, hTreeOrg, setting->context_submenu());
				
				// �A�C�R��
				HICON app = LoadIcon(GetModuleHandle(NULL), _T("IDI_APP"));
				SendMessage(hWnd, WM_SETICON, 0, LPARAM(app));
				
				// ��ʒ����ɕ\��
				RECT rect = {};
				GetWindowRect(hWnd, (LPRECT)&rect);
				int x = (GetSystemMetrics(SM_CXSCREEN) - (rect.right - rect.left)) / 2;
				int y = (GetSystemMetrics(SM_CYSCREEN) - (rect.bottom - rect.top)) / 2;
				SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
				
				return FALSE;
			}
			case WM_NOTIFY:
				if(reinterpret_cast<LPNMHDR>(lp)->code == TVN_BEGINLABELEDIT && reinterpret_cast<LPNMHDR>(lp)->hwndFrom == hTreeMenu) {
					if(reinterpret_cast<LPNMTVDISPINFO>(lp)->item.lParam == SUBMENU_ROOT_ID) {
						SetWindowLongPtr(hWnd, DWLP_MSGRESULT, TRUE);
					}
					else SetWindowLongPtr(hWnd, DWLP_MSGRESULT, FALSE);
					return TRUE;
				} else if(reinterpret_cast<LPNMHDR>(lp)->code == TVN_ENDLABELEDIT && reinterpret_cast<LPNMHDR>(lp)->hwndFrom == hTreeMenu) {
					TreeView_SetItem(hTreeMenu, &reinterpret_cast<TV_DISPINFO*>(lp)->item);
					return TRUE;
				}
				break;
			case WM_COMMAND:
				if (HIWORD(wp) != BN_CLICKED) break;
				
				switch (LOWORD(wp)) {
				case IDOK:
				{
					std::vector<cubeice::user_setting::SUBMENU> v;
					GetSubmenuStruct(v, hTreeMenu, TreeView_GetChild(hTreeMenu, TVI_ROOT));
					if(!CheckValidation(v)) {
						MessageBox(hWnd, _T("��̃t�H���_�����݂��܂��B�t�H���_�̒��ɂ͕K�����k�A�܂��͉𓀍��ڂ�I�����ĉ������B"), _T("�G���["), MB_OK | MB_ICONERROR);
						break;
					}
					setting->context_submenu() = v;
					setting->context_customize() = true;
				}
				case IDCANCEL:
					if (hKeybordHook) UnhookWindowsHookEx(hKeybordHook);
					hKeybordHook = NULL;
					hTreeMenu = NULL;
					hTreeOrg = NULL;
					EndDialog(hWnd, LOWORD(wp));
					break;
				case IDC_ADD_BUTTON: // �ǉ�
				{
					HTREEITEM	hInsertTo;
					HTREEITEM	hInsertFrom;
					HTREEITEM	hInsertedItem;
					TV_ITEM		tvitem;

					hInsertTo = TreeView_GetSelection(hTreeMenu);
					hInsertFrom = TreeView_GetSelection(hTreeOrg);

					if(!hInsertTo || !hInsertFrom)
						break;

					bool		f = false;
					tvitem.mask		= TVIF_HANDLE | TVIF_PARAM;
					tvitem.hItem	= hInsertTo;
					TreeView_GetItem(hTreeMenu, &tvitem);
					for(int i = 0 ; MenuItem[i].stringA ; ++i) {
						if(tvitem.lParam == MenuItem[i].dispSetting) {
							f = true;
							break;
						}
					}
					if(tvitem.lParam != SUBMENU_ROOT_ID && tvitem.lParam != SUBMENU_DIR_ID && !f)
						return 0;

					hInsertedItem = CopyTreeViewItem(hTreeMenu, hInsertTo, hTreeOrg, hInsertFrom);
					TreeView_Expand(hTreeMenu, hInsertTo, TVE_EXPAND);
					tvitem.mask		= TVIF_HANDLE | TVIF_PARAM;
					tvitem.hItem	= hInsertedItem;
					TreeView_GetItem(hTreeMenu, &tvitem);
					if(tvitem.lParam == static_cast<LPARAM>(0)) {
						TreeView_Select(hTreeMenu, tvitem.hItem, TVGN_CARET);
						TreeView_EditLabel(hTreeMenu, tvitem.hItem);
					}
					break;
				}
				case IDC_DELETE_BUTTON: // �폜
				{
					HTREEITEM	hItem;
					hItem = TreeView_GetSelection(hTreeMenu);
					if(hItem && TreeView_GetParent(hTreeMenu, hItem))
						TreeView_DeleteItem(hTreeMenu, hItem);
					break;
				}
				case IDC_UP_BUTTON: // ���
				{
					HTREEITEM hTreeItem = TreeView_GetSelection(hTreeMenu);
					if(!hTreeItem) break;

					HTREEITEM hPrevItem = TreeView_GetPrevSibling(hTreeMenu, hTreeItem);
					if(!hPrevItem) break;

					hPrevItem = TreeView_GetPrevSibling(hTreeMenu, hPrevItem);
					if(!hPrevItem) hPrevItem = TVI_FIRST;

					HTREEITEM hNewItem = CopyTreeViewItem(hTreeMenu, TreeView_GetParent(hTreeMenu, hTreeItem), hTreeMenu, hTreeItem, hPrevItem);
					TreeView_DeleteItem(hTreeMenu, hTreeItem);
					TreeView_SelectItem(hTreeMenu, hNewItem);
					break;
				}
				case IDC_DOWN_BUTTON: // ����
				{
					HTREEITEM hTreeItem = TreeView_GetSelection(hTreeMenu);
					if(!hTreeItem) break;

					HTREEITEM hNextItem = TreeView_GetNextSibling(hTreeMenu, hTreeItem);
					if(!hNextItem) break;

					HTREEITEM hNewItem = CopyTreeViewItem(hTreeMenu, TreeView_GetParent(hTreeMenu, hTreeItem), hTreeMenu, hTreeItem, hNextItem);
					TreeView_DeleteItem(hTreeMenu, hTreeItem);
					TreeView_SelectItem(hTreeMenu, hNewItem);
					break;
				}
				case IDC_RENAME_BUTTON: // ���l�[��
				{
					HTREEITEM hTreeItem = TreeView_GetSelection(hTreeMenu);
					TreeView_EditLabel(hTreeMenu, hTreeItem);
					break;
				}
				case IDC_RESET_BUTTON: // ���Z�b�g
					TreeView_DeleteAllItems(hTreeMenu);
					TreeView_DeleteAllItems(hTreeOrg);
					InitTreeViewItem(hTreeMenu, hTreeOrg, setting->context_submenu());
					break;
				default:
					return FALSE;
				}
				return TRUE;
			default:
				break;
			}
			
			return FALSE;
		}

		/* ----------------------------------------------------------------- */
		//  customize
		/* ----------------------------------------------------------------- */
		int customize(HWND owner, cubeice::user_setting& setting) {
			return DialogBoxParam(GetModuleHandle(NULL), _T("IDD_CONTEXT_MENU"), owner, customize_wndproc, (LPARAM)&setting);
		}
	}
}