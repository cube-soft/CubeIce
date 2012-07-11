// -*- coding: shift-jis -*-
/* ------------------------------------------------------------------------- */
/*
 *  cubeice/user-setting/user-context-setting.h
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
#ifndef CUBEICE_USER_SETTING_USER_CONTEXT_SETTING_H
#define CUBEICE_USER_SETTING_USER_CONTEXT_SETTING_H

#include <cubeice/config.h>
#include <cubeice/user-setting/user-setting-base.h>
#include <cubeice/context.h>

namespace CubeICE {
	/* --------------------------------------------------------------------- */
	///
	/// UserContextSettingBase
	///
	/// <summary>
	/// CubeICE �̃R���e�L�X�g���j���[�Ɋ֘A����e�탆�[�U�ݒ���擾�E�ۑ�
	/// ���邽�߂̊��N���X�ł��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	class UserContextSettingBase : public UserSettingBase {
	public:
		/* ----------------------------------------------------------------- */
		/// constructor
		/* ----------------------------------------------------------------- */
		UserContextSettingBase() :
			UserSettingBase(), extended_() {
			this->Reset();
		}
		
		/* ----------------------------------------------------------------- */
		/// constructor
		/* ----------------------------------------------------------------- */
		UserContextSettingBase(const UserContextSettingBase& cp) :
			UserSettingBase(cp), extended_(cp.extended_) {}
		
		/* ----------------------------------------------------------------- */
		/// destructor
		/* ----------------------------------------------------------------- */
		virtual ~UserContextSettingBase() {}
		
		/* ----------------------------------------------------------------- */
		///
		/// Reset
		///
		/// <summary>
		/// CubeICE �̃R���e�L�X�g���j���[�Ɋ֘A����ݒ�\�ȍ��ڂ�������
		/// ��`���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		virtual void Reset() {
			UserSettingBase::Reset();
			extended_.clear();
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// IsExtended
		///
		/// ���[�U���Ǝ��ɃJ�X�^�}�C�Y�����R���e�L�X�g���j���[��\�����邩
		/// �ǂ������擾���܂��B
		///
		/* ----------------------------------------------------------------- */
		const bool& IsExtended() const { return this->Find(_T("IsExtended")).Value<ValueKind::Bool>(); }
		
		/* ----------------------------------------------------------------- */
		///
		/// IsExtended
		///
		/// <summary>
		/// ���[�U���Ǝ��ɃJ�X�^�}�C�Y�����R���e�L�X�g���j���[��\�����邩
		/// �ǂ�����ݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void IsExtended(bool enable) { this->Find(_T("IsExtended")).Value(enable); }
		
		/* ----------------------------------------------------------------- */
		///
		/// ExtendedContext
		///
		/// <summary>
		/// ���[�U���Ǝ��ɃJ�X�^�}�C�Y�����R���e�L�X�g���j���[�̈ꗗ��
		/// �擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		ContextContainer& ExtendedContext() { return extended_; }
		const ContextContainer& ExtendedContext() const { return extended_; }
		
		/* ----------------------------------------------------------------- */
		///
		/// SortExtendedContext
		///
		/// <summary>
		/// ���[�U�g���R���e�L�X�g���j���[���e�v�f�Ɏw�肳�ꂽ�C���f�b�N�X
		/// �ɏ]���ă\�[�g���܂��B���A�C���f�b�N�X�� -1 �̏ꍇ�A�i�[�����
		/// ����R���e�i���̃C���f�b�N�X���g�p���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void SortExtendedContext() { this->SortExtendedContext(extended_); }
		
	protected:
		/* ----------------------------------------------------------------- */
		/// LoadFromParameterExec
		/* ----------------------------------------------------------------- */
		virtual void LoadFromParameterExec(const NodeSet& src) {
			if (src.empty()) return;
			
			extended_.clear();
			optional<const Node&> root = PsdotNet::Parameter::FindOptional(src, _T("Extended"));
			if (root && root->Kind() == ValueKind::NodeSet) {
				BOOST_FOREACH(const Node& node, root->Value<ValueKind::NodeSet>()) {
					if (node.Kind() != ValueKind::NodeSet) continue;
					ContextContainer::value_type element;
					bool status = this->LoadExtendedContext(node, element);
					if (status) extended_.push_back(element);
				}
			}
			this->SortExtendedContext();
			
			UserSettingBase::LoadFromParameterExec(src);
		}
		
		/* ----------------------------------------------------------------- */
		/// SaveToParameterExec
		/* ----------------------------------------------------------------- */
		virtual void SaveToParameterExec(NodeSet& dest) {
			UserSettingBase::SaveToParameterExec(dest);
			
			this->SortExtendedContext();
			NodeSet v;
			BOOST_FOREACH(Context& element, extended_) {
				NodeSet child;
				bool status = this->SaveExtendedContext(element, child);
				if (status) v.push_back(Node(this->KeyName(element.Index()), child));
			}
			dest.push_back(Node(_T("Extended"), v));
		}
		
	private:
		/* ----------------------------------------------------------------- */
		/// LoadExtendedContext
		/* ----------------------------------------------------------------- */
		bool LoadExtendedContext(const Node& src, ContextContainer::value_type& dest) {
			if (src.Kind() != ValueKind::NodeSet) return false;
			const NodeSet& v = src.Value<ValueKind::NodeSet>();
			
			// Name
			optional<const Node&> opt = PsdotNet::Parameter::FindOptional(v, _T("Name"));
			if (!opt || opt->Kind() != ValueKind::String) return false;
			else dest.Name(opt->Value<ValueKind::String>());
			
			// Index
			opt = PsdotNet::Parameter::FindOptional(v, _T("Index"));
			if (opt && opt->Kind() == ValueKind::Number) {
				dest.Index(opt->Value<ValueKind::Number>());
			}
			
			// TargetPath
			opt = PsdotNet::Parameter::FindOptional(v, _T("TargetPath"));
			if (opt && opt->Kind() == ValueKind::String) {
				dest.TargetPath(opt->Value<ValueKind::String>());
			}
			
			// Arguments
			opt = PsdotNet::Parameter::FindOptional(v, _T("Arguments"));
			if (opt && opt->Kind() == ValueKind::String) {
				dest.Arguments(opt->Value<ValueKind::String>());
			}
			
			// Icon
			opt = PsdotNet::Parameter::FindOptional(v, _T("IconLocation"));
			if (opt && opt->Kind() == ValueKind::String) {
				dest.IconLocation(opt->Value<ValueKind::String>());
			}
			
			BOOST_FOREACH(const Node& node, v) {
				if (node.Kind() != ValueKind::NodeSet) continue;
				Context child;
				bool status = this->LoadExtendedContext(node, child);
				if (status) dest.Children().push_back(child);
			}
			
			return !dest.Empty();
		}
		
		/* ----------------------------------------------------------------- */
		/// SaveExtendedContext
		/* ----------------------------------------------------------------- */
		bool SaveExtendedContext(const Context& src, NodeSet& dest) {
			dest.push_back(Node(_T("Name"), src.Name()));
			dest.push_back(Node(_T("Index"), src.Index()));
			dest.push_back(Node(_T("TargetPath"), src.TargetPath()));
			dest.push_back(Node(_T("Arguments"), src.Arguments()));
			dest.push_back(Node(_T("IconLocation"), src.IconLocation()));
			
			BOOST_FOREACH(const Context& element, src.Children()) {
				NodeSet child;
				bool status = this->SaveExtendedContext(element, child);
				if (status) dest.push_back(Node(this->KeyName(element.Index()), child));
			}
			
			return !src.Empty();
		}
		
		/* ----------------------------------------------------------------- */
		/// SortExtendedContext
		/* ----------------------------------------------------------------- */
		void SortExtendedContext(ContextContainer& v) {
			for (int i = 0; i < static_cast<int>(v.size()); ++i) {
				if (v[i].Index() == -1) v[i].Index(i);
			}
			
			std::sort(v.begin(), v.end());
			
			for (int i = 0; i < static_cast<int>(v.size()); ++i) {
				if (!v[i].Children().empty()) this->SortExtendedContext(v[i].Children());
				v[i].Index(i);
			}
		}
		
		/* ----------------------------------------------------------------- */
		/// KeyName
		/* ----------------------------------------------------------------- */
		string_type KeyName(int index) {
			std::basic_stringstream<char_type> ss;
			ss << _T("cx") << index;
			return ss.str();
		}
		
	private:
		ContextContainer extended_;
	}; // class UserContextSettingBase
	
	/* --------------------------------------------------------------------- */
	///
	/// UserContextSetting
	///
	/// <summary>
	/// CubeICE �̃R���e�L�X�g���j���[�Ɋ֘A����e�탆�[�U�ݒ���擾�E�ۑ�
	/// ���邽�߂̃N���X�ł��BUserContextSetting �N���X�́A�G�N�X�v���[������
	/// �t�@�C����t�H���_���E�N���b�N�����ۂɕ\�������R���e�L�X�g���j���[
	/// �̍��ڂ�ێ����܂��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	class UserContextSetting : public UserContextSettingBase {
	public:
		/* ----------------------------------------------------------------- */
		/// constructor
		/* ----------------------------------------------------------------- */
		UserContextSetting() :
			UserContextSettingBase() {
			this->Reset();
		}
		
		/* ----------------------------------------------------------------- */
		/// constructor
		/* ----------------------------------------------------------------- */
		UserContextSetting(const UserContextSetting& cp) :
			UserContextSettingBase(cp) {}
		
		/* ----------------------------------------------------------------- */
		/// destructor
		/* ----------------------------------------------------------------- */
		virtual ~UserContextSetting() {}
		
		/* ----------------------------------------------------------------- */
		///
		/// Reset
		///
		/// <summary>
		/// CubeICE �̃R���e�L�X�g���j���[�Ɋ֘A����ݒ�\�ȍ��ڂ�
		/// �����Œ�`���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		virtual void Reset() {
			UserContextSettingBase::Reset();
			this->Parameters().push_back(Node(_T("IsExtended"), false));
			this->Parameters().push_back(Node(_T("Builtin"), 0x7ff3));
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// Upgrade
		///
		/// <summary>
		/// ���o�[�W�����̃f�[�^�\�����猻�݂̂��̂ɕϊ����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		virtual void Upgrade(const Version1& old) {
			this->IsExtended(old.context_customize());
			this->Builtin(old.context_flags());
			this->ExtendedContext().clear();
			this->UpgradeExtendedContext(old.context_submenu(), this->ExtendedContext(), old.install_path());
			this->SortExtendedContext();
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// Builtin
		///
		/// <summary>
		/// �V�X�e���ɗ\�ߑg�ݍ��܂�Ă���R���e�L�X�g���j���[�̑g�ݍ��킹
		/// ���擾���܂��B���̒l�́AIsExtended() ���L���̏ꍇ��
		/// ��������܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		int& Builtin() { return this->Find(_T("Builtin")).Value<ValueKind::Number>(); }
		const int& Builtin() const { return this->Find(_T("Builtin")).Value<ValueKind::Number>(); }
		
		/* ----------------------------------------------------------------- */
		///
		/// Builtin
		///
		/// <summary>
		/// �V�X�e���ɗ\�ߑg�ݍ��܂�Ă���R���e�L�X�g���j���[�̑g�ݍ��킹
		/// ��ݒ肵�܂��B���̒l�́AIsExtended() ���L���̏ꍇ��
		/// ��������܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void Builtin(int flag) { this->Find(_T("Builtin")).Value(flag); }
		
	private:
		/* ----------------------------------------------------------------- */
		///
		/// UpgradeExtendedContext
		///
		/// <summary>
		/// ���o�[�W�����̃R���e�L�X�g���j���[�p�f�[�^����ϊ����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		bool UpgradeExtendedContext(const std::vector<Version1::SUBMENU>& src, ContextContainer& dest, const string_type& install) {
			string_type program = install + _T("\\") + CUBEICE_NAME;
			BOOST_FOREACH(const Version1::SUBMENU& item, src) {
				// ���o�[�W�����̃o�O�ŏd�����ēo�^����Ă���ꍇ������̂ŁA���̑΍�B
				bool exists = false;
				BOOST_FOREACH(const Context& x, dest) {
					if (x.Name() != item.str) continue;
					exists = true;
					break;
				}
				if (exists) continue;
				
				Context cx(item.str);
				cx.Index(dest.size());
				if (!this->SetValueFromVersion1(cx, item.id, program)) continue;
				
				if (!item.children.empty()) {
					this->UpgradeExtendedContext(item.children, cx.Children(), install);
				}
				if (!cx.Empty()) dest.push_back(cx);
			}
			return !dest.empty();
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// SetContextFromVersion1
		///
		/// <summary>
		/// ���o�[�W�����̃R���e�L�X�g���j���[�p�f�[�^����K�v�ȏ���
		/// Context �N���X�ɕۑ����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		bool SetValueFromVersion1(Context& dest, int id, const string_type& path) {
			for (int i = 0; MenuItem[i].stringA; ++i) {
				if (id == MenuItem[i].dispSetting) {
					dest.IconLocation(IconIDToLocation(MenuItem[i].iconID, path));
					return true;
				}
			}
			
			for (int i = 0; SubMenuCompress[i].stringA; ++i) {
				if (id == SubMenuCompress[i].dispSetting) {
					dest.TargetPath(path);
					dest.IconLocation(IconIDToLocation(SubMenuCompress[i].iconID, path));
					if (SubMenuCompress[i].arg) dest.Arguments(SubMenuCompress[i].arg);
					return true;
				}
			}
			
			for (int i = 0; SubMenuCompAndMail[i].stringA; ++i) {
				if (id ==  SubMenuCompAndMail[i].dispSetting) {
					dest.TargetPath(path);
					dest.IconLocation(IconIDToLocation(SubMenuCompAndMail[i].iconID, path));
					if (SubMenuCompAndMail[i].arg) dest.Arguments(SubMenuCompAndMail[i].arg);
					return true;
				}
			}
			
			for (int i = 0; SubMenuDecompress[i].stringA; ++i) {
				if (id == SubMenuDecompress[i].dispSetting) {
					dest.TargetPath(path);
					dest.IconLocation(IconIDToLocation(SubMenuDecompress[i].iconID, path));
					if (SubMenuDecompress[i].arg) dest.Arguments(SubMenuDecompress[i].arg);
					return true;
				}
			}
			
			return false;
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// IconIDToLocation
		///
		/// <summary>
		/// ���o�[�W�����Ŏg�p����Ă����A�C�R�� ID �ɑΉ�����
		/// �A�C�R���t�@�C���ւ̃p�X���擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		string_type IconIDToLocation(int id, const string_type& path) {
			switch (id) {
			case 101: // IDB_LOGO
				return path + _T(",0");
			case 102: // IDB_COMPRESSION
				return path + _T(",1");
			case 103: // IDB_DECOMPRESSION
				return path + _T(",2");
			default:
				break;
			}
			return string_type();
		}
	}; // class UserContextSetting
	
	/* --------------------------------------------------------------------- */
	///
	/// UserDragDropSetting
	///
	/// <summary>
	/// CubeICE �̃R���e�L�X�g���j���[�Ɋ֘A����e�탆�[�U�ݒ���擾�E�ۑ�
	/// ���邽�߂̃N���X�ł��BUserDragDropSetting �N���X�́A
	/// �G�N�X�v���[�����Ńt�@�C����t�H���_���}�E�X�̉E�N���b�N�{�^���ɂ��
	/// �h���b�O&�h���b�v������s�����ۂɕ\�������R���e�L�X�g���j���[
	/// �̍��ڂ�ێ����܂��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	class UserDragDropSetting : public UserContextSettingBase {
	public:
		/* ----------------------------------------------------------------- */
		/// constructor
		/* ----------------------------------------------------------------- */
		UserDragDropSetting() :
			UserContextSettingBase() {
			this->Reset();
		}
		
		/* ----------------------------------------------------------------- */
		/// constructor
		/* ----------------------------------------------------------------- */
		UserDragDropSetting(const UserDragDropSetting& cp) :
			UserContextSettingBase(cp) {}
		
		/* ----------------------------------------------------------------- */
		/// destructor
		/* ----------------------------------------------------------------- */
		virtual ~UserDragDropSetting() {}
		
		/* ----------------------------------------------------------------- */
		///
		/// Reset
		///
		/// <summary>
		/// CubeICE �̃h���b�O&�h���b�v���j���[�Ɋ֘A����ݒ�\�ȍ��ڂ�
		/// �����Œ�`���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		virtual void Reset() {
			UserContextSettingBase::Reset();
			this->Parameters().clear();
			this->Parameters().push_back(Node(_T("IsExtended"), false));
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// Upgrade
		///
		/// <summary>
		/// ���o�[�W�����̃f�[�^�\�����猻�݂̂��̂ɕϊ����܂��B
		/// NOTE: ���o�[�W�����ɂ̓h���b�O&�h���b�v�̍��ڂ͑��݂��Ȃ�����
		/// ���߁A�����l�ɐݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		virtual void Upgrade(const Version1& /* old */) {
			this->IsExtended(false);
			this->ExtendedContext().clear();
		}
	}; // class UserDragDropSetting
} // namespace CubeICE

#endif // CUBEICE_USER_SETTING_USER_CONTEXT_SETTING_H
