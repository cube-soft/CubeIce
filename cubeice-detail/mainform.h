// -*- coding: shift-jis -*-
/* ------------------------------------------------------------------------- */
/*
 *  cubeice-detail/mainform.h
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
 */
/* ------------------------------------------------------------------------- */
#ifndef CUBEICE_DETAIL_MAINFORM_H
#define CUBEICE_DETAIL_MAINFORM_H

#include <cubeice/config.h>
#include <cubeice/user-setting.h>
#include <psdotnet/forms.h>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
#include <list>
#include "edit-dialog.h"
#include "version-dialog.h"
#include "listview-helper.h"
#include "resource.h"

namespace CubeICE {
	/* --------------------------------------------------------------------- */
	///
	/// MainForm
	///
	/// <summary>
	/// CubeICE �̏ڍאݒ��ʂ�\���N���X�ł��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	class MainForm : public PsdotNet::Forms::Form {
	private:
		typedef PsdotNet::Forms::Form super;
		typedef CubeICE::UserSettingBase::ValueKind ValueKind;
		typedef CubeICE::UserSettingBase::NodeSet NodeSet;
		typedef CubeICE::UserSettingBase::Node Node;
		
	public:
		/* ----------------------------------------------------------------- */
		/// constructor
		/* ----------------------------------------------------------------- */
		explicit MainForm(UserSetting& setting) :
			super(_T("IDD_MAINFORM")), setting_(setting), listview_(), associate_(false) {
			this->Icon(PsdotNet::Drawing::Icon(PsdotNet::Drawing::Icon::Type::Resource, _T("IDI_APP")));
		}
		
		/* ----------------------------------------------------------------- */
		/// destructor
		/* ----------------------------------------------------------------- */
		virtual ~MainForm() {}
		
	protected:
		/* ----------------------------------------------------------------- */
		///
		/// OnCreateControl
		///
		/// <summary>
		/// ��ʂ̏����ݒ���s���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		virtual void OnCreateControl() {
			listview_.Handle(*this, IDC_MAINFORM_LISTVIEW);
			listview_.View(PsdotNet::Forms::ListViewStyle::Details);
			listview_.GridLines(true);
			listview_.FullRowSelect(true);
			listview_.Columns().Add(PsdotNet::Forms::ColumnHeader(_T("���O"), 250));
			listview_.Columns().Add(PsdotNet::Forms::ColumnHeader(_T("���"), 80));
			listview_.Columns().Add(PsdotNet::Forms::ColumnHeader(_T("���"), 120));
			listview_.Columns().Add(PsdotNet::Forms::ColumnHeader(_T("�l"), 250));
			this->InsertListView();
			
			super::OnCreateControl();
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// OnResize
		///
		/// <summary>
		/// ���T�C�Y�C�x���g�������������̏����ł��B�e��R���|�[�l���g��
		/// �V���ȃT�C�Y�ɏ]���čĔz�u���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		virtual void OnResize(PsdotNet::EventArgs& e) {
			int margin = 6;
			
			RECT whole = {};
			::GetClientRect(this->Handle(), &whole);
			
			handle_type happly = ::GetDlgItem(this->Handle(), IDC_MAINFORM_APPLY);
			RECT rapply = {};
			::GetClientRect(happly, &rapply);
			int x = whole.right - rapply.right - margin;
			int y = whole.bottom - rapply.bottom - margin;
			::SetWindowPos(happly, NULL, x, y, rapply.right, rapply.bottom, SWP_NOZORDER);
			
			handle_type hcancel = ::GetDlgItem(this->Handle(), IDCANCEL);
			RECT rcancel = {};
			::GetClientRect(hcancel, &rcancel);
			x -= (rcancel.right + margin);
			if (rcancel.bottom > rapply.bottom) y = whole.bottom - rcancel.bottom - margin;
			::SetWindowPos(hcancel, NULL, x, y, rcancel.right, rcancel.bottom, SWP_NOZORDER);
			
			handle_type hok = ::GetDlgItem(this->Handle(), IDOK);
			RECT rok = {};
			::GetClientRect(hok, &rok);
			x -= (rok.right + margin);
			if (rok.bottom > rcancel.bottom) y = whole.bottom - rok.bottom - margin;
			::SetWindowPos(hok, NULL, x, y, rok.right, rok.bottom, SWP_NOZORDER);
			
			y -= margin * 2;
			::SetWindowPos(listview_.Handle(), NULL, 0, 0, whole.right, y, SWP_NOZORDER);
		}
		
		/* ----------------------------------------------------------------- */
		/// OnCommand
		/* ----------------------------------------------------------------- */
		virtual void OnCommand(PsdotNet::Forms::Message& m) {
			switch (LOWORD(m.WParam())) {
			case IDOK:
				this->SaveSetting();
				this->Close();
				m.Result(TRUE);
				break;
			case IDCANCEL:
				this->Close();
				m.Result(TRUE);
				break;
			case IDC_MAINFORM_APPLY:
				this->SaveSetting();
				this->EnableApplyButton(false);
				m.Result(TRUE);
				break;
			case IDM_FILE_EXPORT:
				m.Result(this->Export());
				break;
			case IDM_HELP_ABOUT:
				m.Result(this->About());
				break;
			default:
				m.Result(this->OnModifyCommand(m));
				if (m.Result()) this->EnableApplyButton(true);
				else super::OnCommand(m);
				break;
			}
		}
		
		/* ----------------------------------------------------------------- */
		/// OnNotify
		/* ----------------------------------------------------------------- */
		virtual void OnNotify(PsdotNet::Forms::Message& m) {
			switch (LOWORD(m.WParam())) {
			case IDC_MAINFORM_LISTVIEW:
				if (((LPNMHDR)m.LParam())->code == NM_DBLCLK) {
					m.Result(this->ModifyListViewItem(listview_.SelectedIndex()));
					if (m.Result()) this->EnableApplyButton(true);
				}
				else m.Result(FALSE);
				break;
			default:
				super::OnNotify(m);
				break;
			}
		}
		
		/* ----------------------------------------------------------------- */
		/// OnContextMenu
		/* ----------------------------------------------------------------- */
		virtual void OnContextMenu(PsdotNet::Forms::Message& m) {
			m.Result(FALSE);
			handle_type handle = reinterpret_cast<handle_type>(m.WParam());
			int index = listview_.SelectedIndex();
			if (handle != listview_.Handle() || index < 0) return;
			HMENU popup = ::GetSubMenu(::LoadMenu(::GetModuleHandle(NULL), _T("IDR_POPUPMENU")), 0);
			if (!popup) return;
			
			// �V���[�g�J�b�g�A�R���e�L�X�g���j���[�̏ꍇ�́u���Z�b�g�v�A
			// ����ȊO�̏ꍇ�́u�폜�v���g�p�ł��Ȃ��B
			MENUITEMINFO info = {};
			info.cbSize = sizeof(info);
			info.fMask = MIIM_STATE;
			info.fState = MFS_DISABLED;
			string_type kind = listview_.Items(index).SubItems(2).Text();
			UINT position = (IsContextKind(kind) || IsShortcutKind(kind)) ? IDM_POPUP_RESET : IDM_POPUP_DELETE;
			::SetMenuItemInfo(popup, position, FALSE, &info);
			
			::TrackPopupMenu(popup, TPM_LEFTALIGN | TPM_TOPALIGN, static_cast<int>(LOWORD(m.LParam())), static_cast<int>(HIWORD(m.LParam())), 0, this->Handle(), NULL);
			m.Result(TRUE);
		}
		
	private:
		/* ----------------------------------------------------------------- */
		///
		/// OnModifyCommand
		///
		/// <summary>
		/// �e�탆�[�U�ݒ���C���i�ǉ��A�폜���܂ށj����\���̂���
		/// �R�}���h�́AOnCommand() �ł͂Ȃ����̃����o�֐����ɒ�`���܂��B
		/// OnCommand() �͂��̃����o�֐��̕Ԃ�l�����āATRUE �ł����
		/// �K�p�{�^����L���ɂ��܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		result_type OnModifyCommand(PsdotNet::Forms::Message& m) {
			switch (LOWORD(m.WParam())) {
				case IDM_FILE_IMPORT:           return this->Import();
				case IDM_EDIT_CREATECONTEXT:    return this->CreateContext();
				case IDM_EDIT_CREATESHORTCUT:   return this->CreateShortcut();
				case IDM_EDIT_RESETALL:         return this->ResetAll();
				case IDM_POPUP_MODIFY:          return this->ModifyListViewItem(listview_.SelectedIndex());
				case IDM_POPUP_RESET:           return this->ResetListViewItem(listview_.SelectedIndex());
				case IDM_POPUP_DELETE:          return this->DeleteListViewItem(listview_.SelectedIndex());
				default: break;
			}
			return FALSE;
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// About
		///
		/// <summary>
		/// �o�[�W�����_�C�A���O��\�����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		result_type About() {
			VersionDialog dialog(setting_);
			dialog.Icon(this->Icon());
			dialog.ShowDialog(*this);
			return TRUE;
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// ResetAll
		///
		/// <summary>
		/// ���ׂĂ̍��ڂ������l�ɐݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		result_type ResetAll() {
			string_type warning = _T("���[�U�ɂ���Đݒ肳�ꂽ�R���e�L�X�g���j���[��V���[�g�J�b�g�͑S�č폜����܂��B���Z�b�g���܂����H");
			string_type title = _T("���Z�b�g�̊m�F");
			if (PsdotNet::Forms::MsgBox::Show(*this, warning, title, PsdotNet::Forms::MsgBoxButtons::OKCancel, PsdotNet::Forms::MsgBoxIcon::Warning) == PsdotNet::Forms::DialogResult::OK) {
				setting_.Reset();
				this->ResetListView();
				return TRUE;
			}
			return FALSE;
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// Import
		///
		/// <summary>
		/// �O�� XML �t�@�C�����烆�[�U�ݒ��ǂݍ��݂܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		result_type Import() {
			PsdotNet::Forms::OpenFileDialog dialog;
			dialog.AddFilter(_T("XML �t�@�C�� (*.xml)"), _T("*.xml"));
			dialog.AddFilter(_T("���ׂẴt�@�C�� (*.*)"), _T("*.*"));
			if (dialog.ShowDialog(*this) == PsdotNet::Forms::DialogResult::OK) {
				setting_.Load(dialog.FileName());
				listview_.Items().Clear();
				this->InsertListView();
				return TRUE;
			}
			return FALSE;
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// Export
		///
		/// <summary>
		/// ���݂̐ݒ���O�� XML �t�@�C���ɕۑ����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		result_type Export() {
			PsdotNet::Forms::SaveFileDialog dialog;
			dialog.AddFilter(_T("XML �t�@�C�� (*.xml)"), _T("*.xml"));
			dialog.AddFilter(_T("���ׂẴt�@�C�� (*.*)"), _T("*.*"));
			if (dialog.ShowDialog(*this) == PsdotNet::Forms::DialogResult::OK) {
				setting_.Save(dialog.FileName());
				return TRUE;
			}
			return FALSE;
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// CreateShortcut
		///
		/// <summary>
		/// �V�����V���[�g�J�b�g���ڂ��쐬���� ListView �ɒǉ����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		result_type CreateShortcut() {
			Shortcut newitem;
			newitem.Name(_T("�V�����V���[�g�J�b�g"));
			newitem.Directory(PsdotNet::Environment::GetFolderPath(PsdotNet::Environment::SpecialFolder::DesktopDirectory));
			newitem.TargetPath(setting_.InstallDirectory() + _T("\\") + CUBEICE_NAME);
			newitem.IconLocation(setting_.InstallDirectory() + _T("\\") + CUBEICE_NAME + _T(",0"));
			
			EditShortcutDialog dialog(newitem);
			dialog.Icon(this->Icon());
			if (dialog.ShowDialog(*this)  != PsdotNet::Forms::DialogResult::OK) return FALSE;
			
			newitem.Name(dialog.Name());
			newitem.Directory(dialog.Directory());
			newitem.TargetPath(dialog.TargetPath());
			newitem.Arguments(dialog.Arguments());
			newitem.IconLocation(dialog.IconLocation());
			
			if (setting_.Shortcut().GetOptional(newitem.Name())) {
				setting_.Shortcut().Delete(newitem.Name());
				setting_.Shortcut().Add(newitem);
			}
			else {
				setting_.Shortcut().Add(newitem);
				this->ResetListView();
			}
			
			return TRUE;
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// CreateContext
		///
		/// <summary>
		/// �V�����R���e�L�X�g���j���[���ڂ��쐬���� ListView �ɒǉ����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		result_type CreateContext() {
			Context newitem;
			newitem.Name(_T("�V�����R���e�L�X�g���j���["));
			newitem.TargetPath(setting_.InstallDirectory() + _T("\\") + CUBEICE_NAME);
			newitem.Index(0);
			
			EditContextDialog dialog(newitem);
			dialog.Icon(this->Icon());
			if (dialog.ShowDialog(*this)  != PsdotNet::Forms::DialogResult::OK) return FALSE;
			
			UserContextSettingBase* context = NULL;
			if (dialog.Kind() == 0) context = &setting_.Context();
			else context = &setting_.DragDrop();
			
			// ���͂��ꂽ���O�̃R���e�L�X�g���j���[�����ɑ��݂���̂ŁA�C���ƌ��Ȃ�
			std::list<string_type> token = this->GetToken(dialog.Name(), _T("\\"));
			std::list<string_type> tmp(token);
			optional<Context&> target = this->FindContext(tmp, context->ExtendedContext());
			if (target) {
				target->Name(token.back());
				target->TargetPath(dialog.TargetPath());
				target->Arguments(dialog.Arguments());
				target->Index(dialog.Index());
				context->SortExtendedContext();
				return TRUE;
			}
			
			// �V�K�ǉ�
			newitem.Name(token.back());
			newitem.TargetPath(dialog.TargetPath());
			newitem.Arguments(dialog.Arguments());
			newitem.Index(dialog.Index());
			result_type status = this->InsertContext(newitem, context->ExtendedContext(), token);
			if (status) {
				context->SortExtendedContext();
				this->ResetListView();
				return TRUE;
			}
			else return FALSE;
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// InsertContext
		///
		/// <summary>
		/// �V�����R���e�L�X�g���j���[���ڂ��R���e�i�ɒǉ����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		result_type InsertContext(const Context& src, ContextContainer& dest, std::list<string_type> path) {
			if (path.size() == 1) {
				dest.push_back(src);
				return TRUE;
			}
			else {
				string_type name = path.front();
				path.pop_front();
				optional<Context&> context = this->FindContext(name, dest);
				if (context) return this->InsertContext(src, context->Children(), path);
				else {
					Context branch(name);
					result_type status = this->InsertContext(src, branch.Children(), path);
					if (status) dest.push_back(branch);
					return status;
				}
			}
			return FALSE;
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// DeleteContext
		///
		/// <summary>
		/// ���[�U���I�������R���e�L�X�g���j���[���폜���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		result_type DeleteContext(ContextContainer& target, std::list<string_type> path) {
			if (path.size() == 1) {
				for (ContextContainer::iterator pos = target.begin(); pos != target.end(); ++pos) {
					if (pos->Name() == path.front()) {
						target.erase(pos);
						return TRUE;
					}
				}
			}
			else {
				string_type name = path.front();
				path.pop_front();
				optional<Context&> context = this->FindContext(name, target);
				if (context) return this->DeleteContext(context->Children(), path);
			}
			return FALSE;
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// ResetListViewItem
		///
		/// <summary>
		/// ���[�U���I���������ڂ������l�ɐݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		result_type ResetListViewItem(int index) {
			string_type cond = listview_.Items(index).SubItems(1).Text();
			string_type kind = listview_.Items(index).SubItems(2).Text();
			if (cond == CUBEICE_DETAIL_STR_DEFAULT || IsContextKind(kind) || IsShortcutKind(kind)) return FALSE;
			
			string_type name = listview_.Items(index).SubItems(0).Text();
			if (name == _T("Filtering")) return this->ResetListViewFilteringItem(index);
			
			optional<Node&> node = this->GetOptional(setting_, name);
			optional<Node&> builtin = this->GetOptional(builtin_, name);
			if (!node || !builtin || node->Kind() != builtin->Kind()) return FALSE;
			
			switch (node->Kind().ToEnum()) {
			case ValueKind::String: {
				string_type value = builtin->Value<ValueKind::String>();
				node->Value(value);
				listview_.Items(index).SubItems(3).Text(value);
				break;
			}
			case ValueKind::Number: {
				int value = builtin->Value<ValueKind::Number>();
				node->Value(value);
				listview_.Items(index).SubItems(3).Text(boost::lexical_cast<string_type>(value));
				break;
			}
			case ValueKind::Bool: {
				bool value = builtin->Value<ValueKind::Bool>();
				node->Value(value);
				listview_.Items(index).SubItems(3).Text(value ? CUBEICE_DETAIL_STR_TRUE : CUBEICE_DETAIL_STR_FALSE);
				break;
			}
			default:
				return FALSE;
			}
			
			listview_.Items(index).SubItems(1).Text(CUBEICE_DETAIL_STR_DEFAULT);
			return TRUE;
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// ResetListViewFilteringItem
		///
		/// <summary>
		/// �t�B���^�����O�������l�ɐݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		result_type ResetListViewFilteringItem(int index) {
			setting_.Filtering().Clear();
			BOOST_FOREACH(const string_type& s, builtin_.Filtering().Data()) setting_.Filtering().Add(s);
			listview_.Items(index).SubItems(1).Text(CUBEICE_DETAIL_STR_DEFAULT);
			listview_.Items(index).SubItems(3).Text(boost::algorithm::join(setting_.Filtering().Data(), _T("|")));
			return TRUE;
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// DeleteListViewItem
		///
		/// <summary>
		/// ���[�U���I���������ڂ��폜���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		result_type DeleteListViewItem(int index) {
			string_type kind = listview_.Items(index).SubItems(2).Text();
			if (IsContextKind(kind)) return this->DeleteListViewContextItem(index);
			else if (IsShortcutKind(kind)) return this->DeleteListViewShortcutItem(index);
			return FALSE;
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// DeleteListViewContextItem
		///
		/// <summary>
		/// ���[�U���I�������R���e�L�X�g���j���[���폜���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		result_type DeleteListViewContextItem(int index) {
			std::list<string_type> token = this->GetToken(listview_.Items(index).SubItems(0).Text(), _T("\\"));
			string_type head = token.front();
			token.pop_front();
			
			UserContextSettingBase* context = NULL;
			if (head == _T("Context")) context = &setting_.Context();
			else context = &setting_.DragDrop();
			
			head = token.front();
			token.pop_front();
			if (head != _T("Extended")) return FALSE;
			
			if (this->DeleteContext(context->ExtendedContext(), token)) {
				this->ResetListView();
				return TRUE;
			}
			return FALSE;
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// DeleteListViewShortcutItem
		///
		/// <summary>
		/// ���[�U���I�������V���[�g�J�b�g���폜���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		result_type DeleteListViewShortcutItem(int index) {
			std::list<string_type> token = this->GetToken(listview_.Items(index).SubItems(0).Text(), _T("\\"));
			if (token.size() != 2 || token.front() != _T("Shortcut")) return FALSE;
			setting_.Shortcut().Delete(token.back());
			listview_.Items().RemoveAt(index);
			return TRUE;
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// ModifyListViewItem
		///
		/// <summary>
		/// ���[�U���I���������ڂ�ҏW���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		result_type ModifyListViewItem(int index) {
			string_type kind = listview_.Items(index).SubItems(2).Text();
			if (IsContextKind(kind)) return this->ModifyListViewContextItem(index);
			else if (IsShortcutKind(kind)) return this->ModifyListViewShortcutItem(index);
			else if (GetValueKind(kind) == ValueKind::Bool) return this->ModifyListViewBoolItem(index);
			else return this->ModifyListViewCommonItem(index);
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// ModifyListViewCommonItem
		///
		/// <summary>
		/// ���[�U���I���������ڂ�ҏW���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		result_type ModifyListViewCommonItem(int index) {
			string_type name  = listview_.Items(index).SubItems(0).Text();
			string_type kind  = listview_.Items(index).SubItems(2).Text();
			string_type value = listview_.Items(index).SubItems(3).Text();
			
			EditDialog dialog(name, kind, value);
			dialog.Icon(this->Icon());
			if (dialog.ShowDialog(*this) == PsdotNet::Forms::DialogResult::OK && dialog.Value() != value) {
				listview_.Items(index).SubItems(1).Text(CUBEICE_DETAIL_STR_CUSTOM);
				listview_.Items(index).SubItems(3).Text(dialog.Value());
				return this->ModifyUserSetting(name, listview_.Items(index).SubItems(3).Text());
			}
			return FALSE;
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// ModifyListViewBoolItem
		///
		/// <summary>
		/// ���[�U���I�������^�U�l�̍��ڂ�ҏW���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		result_type ModifyListViewBoolItem(int index) {
			string_type name  = listview_.Items(index).SubItems(0).Text();
			string_type value = listview_.Items(index).SubItems(3).Text();
			
			listview_.Items(index).SubItems(1).Text(CUBEICE_DETAIL_STR_CUSTOM);
			listview_.Items(index).SubItems(3).Text(GetInverse(value));
			
			return this->ModifyUserSetting(name, listview_.Items(index).SubItems(3).Text());
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// ModifyListViewContextItem
		///
		/// <summary>
		/// ���[�U���I�������R���e�L�X�g���j���[��ҏW���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		result_type ModifyListViewContextItem(int index) {
			std::list<string_type> token = this->GetToken(listview_.Items(index).SubItems(0).Text(), _T("\\"));;
			string_type head = token.front();
			token.pop_front();
			int kind = (head == _T("Context")) ? 0 : 1;
			
			head = token.front();
			token.pop_front();
			if (head != _T("Extended")) return FALSE;
			
			UserContextSettingBase* root = NULL;
			if (kind == 0) root = &setting_.Context();
			else root = &setting_.DragDrop();
			optional<Context&> context = this->FindContext(token, root->ExtendedContext());
			if (!context) return FALSE;
			
			EditContextDialog dialog(kind, *context);
			dialog.Icon(this->Icon());
			if (dialog.ShowDialog(*this) != PsdotNet::Forms::DialogResult::OK) return FALSE;
			
			context->Index(dialog.Index());
			context->TargetPath(dialog.TargetPath());
			context->Arguments(dialog.Arguments());
			context->IconLocation(dialog.IconLocation());
			
			string_type name = this->GetTailToken(dialog.Name(), _T("\\"));
			if (context->Name() != dialog.Name()) {
				context->Name(name);
				token = this->GetToken(listview_.Items(index).SubItems(0).Text(), _T("\\"));
				token.back() = name;
				name = boost::algorithm::join(token, _T("\\"));
				listview_.Items(index).SubItems(0).Text(name);
				if (!context->Children().empty()) this->ModifyListView(context->Children(), ++index, name);
			}
			
			return TRUE;
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// ModifyListViewShortcutItem
		///
		/// <summary>
		/// ���[�U���I�������V���[�g�J�b�g��ҏW���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		result_type ModifyListViewShortcutItem(int index) {
			std::list<string_type> token = this->GetToken(listview_.Items(index).SubItems(0).Text(), _T("\\"));
			string_type head = token.front();
			token.pop_front();
			if (head != _T("Shortcut")) return FALSE;
			
			head = token.front();
			token.pop_front();
			if (!token.empty()) return FALSE;
			
			optional<Shortcut&> shortcut = setting_.Shortcut().Get(head);
			if (!shortcut) return FALSE;
			
			EditShortcutDialog dialog(*shortcut);
			dialog.Icon(this->Icon());
			if (dialog.ShowDialog(*this)  != PsdotNet::Forms::DialogResult::OK) return FALSE;
			
			string_type name = this->GetTailToken(dialog.Name(), _T("\\"));
			if (shortcut->Name() != dialog.Name()) {
				token = this->GetToken(listview_.Items(index).SubItems(0).Text(), _T("\\"));
				token.back() = name;
				listview_.Items(index).SubItems(0).Text(boost::algorithm::join(token, _T("\\")));
				
				Shortcut newitem;
				newitem.Name(name);
				newitem.Directory(dialog.Directory());
				newitem.TargetPath(dialog.TargetPath());
				newitem.Arguments(dialog.Arguments());
				newitem.IconLocation(dialog.IconLocation());
				setting_.Shortcut().Delete(shortcut->Name());
				setting_.Shortcut().Add(newitem);
			}
			else {
				shortcut->Directory(dialog.Directory());
				shortcut->TargetPath(dialog.TargetPath());
				shortcut->Arguments(dialog.Arguments());
				shortcut->IconLocation(dialog.IconLocation());
			}
			
			return TRUE;
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// ModifyUserSetting
		///
		/// <summary>
		/// ���C����ʂŕҏW���ꂽ���e�� UserSetting �N���X�ɔ��f���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		result_type ModifyUserSetting(const string_type& name, const string_type& value) {
			std::list<string_type> token = this->GetToken(name, _T("\\"));
			string_type head = token.front();
			token.pop_front();
			
			if (token.empty()) {
				if (head == _T("Filtering")) return this->ModifyUserSetting(value, setting_.Filtering());
				else return this->ModifyUserSetting(head, value, setting_.Parameters());
			}
			else if (head == _T("Association")) {
				result_type status = this->ModifyUserSetting(token.back(), value, setting_.Association().Parameters());
				if (status != FALSE) associate_ = true;
				return status;
			}
			else if (head == _T("Compression")) return this->ModifyUserSetting(token.back(), value, setting_.Compression().Parameters());
			else if (head == _T("Decompression")) return this->ModifyUserSetting(token.back(), value, setting_.Decompression().Parameters());
			else if (head == _T("Runtime")) return this->ModifyUserSetting(token.back(), value, setting_.Runtime().Compression().Parameters());
			else if (head == _T("Context") && token.size() == 1) return this->ModifyUserSetting(token.back(), value, setting_.Context().Parameters());
			else if (head == _T("DragDrop") && token.size() == 1) return this->ModifyUserSetting(token.back(), value, setting_.DragDrop().Parameters());
			
			return FALSE;
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// ModifyUserSetting
		///
		/// <summary>
		/// ���� name �Ŏw�肳�ꂽ���ڂ� value �ɐݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		result_type ModifyUserSetting(const string_type& name, const string_type& value, NodeSet& dest) {
			optional<Node&> opt = PsdotNet::Parameter::Find(dest, name);
			if (!opt) return FALSE;
			
			switch (opt->Kind().ToEnum()) {
			case ValueKind::String:
				opt->Value(value);
				break;
			case ValueKind::Number:
				try {
					opt->Value(boost::lexical_cast<int>(value));
				}
				catch (boost::bad_lexical_cast& /* err */) {
					return FALSE;
				}
				break;
			case ValueKind::Bool:
				opt->Value(GetBool(value));
				break;
			default:
				return FALSE;
			}
			return TRUE;
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// ModifyUserSetting
		///
		/// <summary>
		/// �t�B���^�����O�ݒ���C�����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		result_type ModifyUserSetting(const string_type& value, UserFilteringSetting& dest) {
			std::list<string_type> token = this->GetToken(value, _T("|"));
			dest.Clear();
			BOOST_FOREACH(const string_type& s, token) dest.Add(s);
			return TRUE;
		}
		
	private:
		/* ----------------------------------------------------------------- */
		///
		/// ResetListView
		///
		/// <summary>
		/// ListView �ɕ\������Ă��鍀�ڂ����Z�b�g���܂��B�X�N���[�������
		/// �����ꍇ�́A���Z�b�g��Ɍ��̈ʒu�܂ŃX�N���[�����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void ResetListView() {
			int index = listview_.TopIndex();
			PsdotNet::Drawing::Rectangle rc = listview_.GetItemRect(index);
			listview_.Items().Clear();
			this->InsertListView();
			ListView_Scroll(listview_.Handle(), 0, rc.Height() * index);
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// InsertListView
		///
		/// <summary>
		/// �e�탆�[�U�ݒ�� ListView �ɑ}�����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void InsertListView() {
			this->InsertListView(setting_.Parameters(), builtin_.Parameters(), _T(""));
			this->InsertListView(setting_.Filtering());
			this->InsertListView(setting_.Compression().Parameters(), builtin_.Compression().Parameters(), _T("Compression\\"));
			this->InsertListView(setting_.Decompression().Parameters(), builtin_.Decompression().Parameters(), _T("Decompression\\"));
			this->InsertListView(setting_.Runtime().Compression().Parameters(), builtin_.Runtime().Compression().Parameters(), _T("Runtime\\Compression\\"));
			this->InsertListView(setting_.Association().Parameters(), builtin_.Association().Parameters(), _T("Association\\"));
			this->InsertListView(setting_.Context().Parameters(), builtin_.Context().Parameters(), _T("Context\\"));
			
			this->InsertListView(setting_.Context().ExtendedContext(), _T("Context\\Extended\\"));
			this->InsertListView(setting_.DragDrop().Parameters(), builtin_.DragDrop().Parameters(), _T("DragDrop\\"));
			this->InsertListView(setting_.DragDrop().ExtendedContext(), _T("DragDrop\\Extended\\"));
			this->InsertListView(setting_.Shortcut(), _T("Shortcut\\"));
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// InsertListView
		///
		/// <summary>
		/// �e�탆�[�U�ݒ�� ListView �ɑ}�����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void InsertListView(const NodeSet& parameters, const NodeSet& builtin, const string_type& prefix) {
			BOOST_FOREACH(const Node& element, parameters) {
				if (element.Kind() == ValueKind::NodeSet) continue;
				PsdotNet::Forms::ListViewItem item(prefix + element.Name());
				AddCondition(element, builtin, item);
				AddValue(element, item);
				listview_.Items().Add(item);
			}
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// InsertListView
		///
		/// <summary>
		/// �t�B���^�����O�Ɋւ��郆�[�U�ݒ�� ListView �ɐݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void InsertListView(const UserFilteringSetting& setting) {
			string_type value = boost::algorithm::join(setting.Data(), _T("|"));
			string_type compared = boost::algorithm::join(builtin_.Filtering().Data(), _T("|"));
			PsdotNet::Forms::ListViewItem item(_T("Filtering"));
			item.SubItems().Add(value == compared ? CUBEICE_DETAIL_STR_DEFAULT : CUBEICE_DETAIL_STR_CUSTOM);
			item.SubItems().Add(CUBEICE_DETAIL_STR_STRING);
			item.SubItems().Add(value);
			listview_.Items().Add(item);
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// InsertListView
		///
		/// <summary>
		/// �R���e�L�X�g���j���[�Ɋւ��郆�[�U�ݒ�� ListView �ɑ}�����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void InsertListView(const ContextContainer& src, const string_type& prefix) {
			BOOST_FOREACH(const Context& element, src) {
				PsdotNet::Forms::ListViewItem item(prefix + element.Name());
				item.SubItems().Add(CUBEICE_DETAIL_STR_CUSTOM);
				item.SubItems().Add(CUBEICE_DETAIL_STR_CONTEXT);
				item.SubItems().Add(CUBEICE_DETAIL_STR_STRUCT);
				listview_.Items().Add(item);
				
				if (!element.Children().empty()) {
					this->InsertListView(element.Children(), prefix + element.Name() + _T("\\"));
				}
			}
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// InsertListView
		///
		/// <summary>
		/// �V���[�g�J�b�g�Ɋւ��郆�[�U�ݒ�� ListView �ɑ}�����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void InsertListView(const UserShortcutSetting& setting, const string_type& prefix) {
			BOOST_FOREACH(const Shortcut& element, setting.Items()) {
				PsdotNet::Forms::ListViewItem item(prefix + element.Name());
				item.SubItems().Add(CUBEICE_DETAIL_STR_CUSTOM);
				item.SubItems().Add(CUBEICE_DETAIL_STR_SHORTCUT);
				item.SubItems().Add(CUBEICE_DETAIL_STR_STRUCT);
				listview_.Items().Add(item);
			}
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// ModifyListView
		///
		/// <summary>
		/// �w�肵���ʒu�ȉ��ɑ��݂��郊�X�g�r���[�̊e���ڂ̖��O��V����
		/// �C�����e�ɂ��܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void ModifyListView(const ContextContainer& src, int& pos, const string_type& prefix) {
			BOOST_FOREACH(const Context& cx, src) {
				string_type name = prefix + _T("\\") + cx.Name();
				listview_.Items(pos++).SubItems(0).Text(name);
				if (!cx.Children().empty()) this->ModifyListView(cx.Children(), pos, name);
			}
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// SaveSetting
		///
		/// <summary>
		/// ���݂̐ݒ��ۑ����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void SaveSetting() {
			setting_.Save();
			
			if (associate_) { // �֘A�t���͊Ǘ��Ҍ������K�v�Ȃ̂ŁA�ʃv���O�����Ŏ��s
				::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
				string_type associate = setting_.InstallDirectory() + _T("\\") + CUBEICE_ASSOCIATE_NAME;
				SHELLEXECUTEINFO info = {};
				info.cbSize = sizeof(SHELLEXECUTEINFO);
				info.lpFile = associate.c_str();
				::ShellExecuteEx(&info);
				::CoUninitialize();
				associate_ = false;
			}
		}
		
		/* ----------------------------------------------------------------- */
		/// EnableApplyButton
		/* ----------------------------------------------------------------- */
		void EnableApplyButton(bool enable) {
			::EnableWindow(::GetDlgItem(this->Handle(), IDC_MAINFORM_APPLY), enable ? TRUE : FALSE);
		}
		
		/* ----------------------------------------------------------------- */
		/// FindContext
		/* ----------------------------------------------------------------- */
		optional<Context&> FindContext(const string_type& name, ContextContainer& target) {
			BOOST_FOREACH(Context& element, target) {
				if (name == element.Name()) return optional<Context&>(element);
			}
			return optional<Context&>();
		}
		
		/* ----------------------------------------------------------------- */
		/// FindContext
		/* ----------------------------------------------------------------- */
		optional<Context&> FindContext(std::list<string_type>& token, ContextContainer& target) {
			string_type head = token.front();
			token.pop_front();
			BOOST_FOREACH(Context& element, target) {
				if (head == element.Name()) {
					if (token.empty()) return optional<Context&>(element);
					else if (!element.Children().empty()) return this->FindContext(token, element.Children());
				}
			}
			return optional<Context&>();
		}
		
		/* ----------------------------------------------------------------- */
		/// GetToken
		/* ----------------------------------------------------------------- */
		std::list<string_type> GetToken(const string_type& src, const string_type& delimiter) {
			std::list<string_type> dest;
			boost::algorithm::split(dest, src, boost::is_any_of(delimiter));
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		/// GetTailToken
		/* ----------------------------------------------------------------- */
		string_type GetTailToken(const string_type& src, const string_type& delimiter) {
			return this->GetToken(src, delimiter).back();
		}
		
		/* ----------------------------------------------------------------- */
		/// GetOptional
		/* ----------------------------------------------------------------- */
		optional<Node&> GetOptional(UserSetting& target, const string_type& name) {
			std::list<string_type> token = this->GetToken(name, _T("\\"));
			string_type head = token.front();
			token.pop_front();
			
			if (head == _T("Compression")) return target.Compression().FindOptional(token.back());
			else if (head == _T("Decompression")) return target.Decompression().FindOptional(token.back());
			else if (head == _T("Association")) return target.Association().FindOptional(token.back());
			else if (head == _T("Runtime")) return target.Runtime().Compression().FindOptional(token.back());
			else if (head == _T("Context")) return target.Context().FindOptional(token.back());
			else if (head == _T("DragDrop")) return target.DragDrop().FindOptional(token.back());
			else return target.FindOptional(head);
		}
		
	private:
		UserSetting& setting_;
		UserSetting builtin_;
		PsdotNet::Forms::ListView listview_;
		bool associate_;
	}; // class MainForm
} // namespace CubeICE

#endif // CUBEICE_DETAIL_DETAIL_DIALOG_H
