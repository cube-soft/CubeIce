// -*- coding: shift-jis -*-
/* ------------------------------------------------------------------------- */
/*
 *  cubeice/user-setting.h
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
#ifndef CUBEICE_USER_SETTING_H
#define CUBEICE_USER_SETTING_H

#include <cubeice/config.h>
#include <sstream>
#include <exception>
#include <psdotnet/logger.h>
#include <psdotnet/registry.h>

#include <cubeice/user-setting/utility.h>
#include <cubeice/user-setting/user-setting-base.h>
#include <cubeice/user-setting/user-association-setting.h>
#include <cubeice/user-setting/user-context-setting.h>
#include <cubeice/user-setting/user-shortcut-setting.h>
#include <cubeice/user-setting/user-compression-setting.h>
#include <cubeice/user-setting/user-decompression-setting.h>
#include <cubeice/user-setting/user-filtering-setting.h>
#include <cubeice/user-setting/runtime-setting.h>

/* ------------------------------------------------------------------------- */
//  ���W�X�g���Ɋւ����`
/* ------------------------------------------------------------------------- */
#ifndef CUBEICE_REG_ROOT
#define CUBEICE_REG_ROOT                        _T("Software\\CubeSoft\\CubeICE")
#endif

#ifndef CUBEICE_REG_STRUCTVERSION
#define CUBEICE_REG_STRUCTVERSION               _T("v2")
#endif

/* ------------------------------------------------------------------------- */
//  ��{�I�ȏ��
/* ------------------------------------------------------------------------- */
#define CUBEICE_NAME_INSTALL_DIRECTORY          _T("InstallDirectory")
#define CUBEICE_NAME_VERSION                    _T("Version")

namespace CubeICE {
	/* --------------------------------------------------------------------- */
	///
	/// UserSetting
	///
	/// <summary>
	/// CubeICE �Ɋ֘A����e�탆�[�U�ݒ���擾�E�ۑ�����N���X�ł��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	class UserSetting : public UserSettingBase{
	public:
		typedef CubeICE::UserAssociationSetting association_type;
		typedef CubeICE::UserContextSetting context_type;
		typedef CubeICE::UserDragDropSetting dragdrop_type;
		typedef CubeICE::UserShortcutSetting shortcut_type;
		typedef CubeICE::UserCompressionSetting compression_type;
		typedef CubeICE::UserDecompressionSetting decompression_type;
		typedef CubeICE::UserFilteringSetting filtering_type;
		typedef CubeICE::RuntimeSetting runtime_type;
		
	public:
		/* ----------------------------------------------------------------- */
		/// constructor
		/* ----------------------------------------------------------------- */
		UserSetting() :
			UserSettingBase(),
			path_(), version_(),
			association_(), context_(), dragdrop_(), shortcut_(),
			compression_(), decompression_(),
			filtering_(), runtime_() {
			this->Reset();
			this->LoadBasicData();
		}
		
		/* ----------------------------------------------------------------- */
		/// constructor
		/* ----------------------------------------------------------------- */
		UserSetting(const UserSetting& cp) :
			UserSettingBase(cp),
			path_(cp.path_), version_(cp.version_),
			association_(cp.association_), context_(cp.context_), dragdrop_(cp.dragdrop_), shortcut_(cp.shortcut_),
			compression_(cp.compression_), decompression_(cp.decompression_),
			filtering_(cp.filtering_), runtime_() {}
		
		/* ----------------------------------------------------------------- */
		/// destructor
		/* ----------------------------------------------------------------- */
		virtual ~UserSetting() {}
		
		/* ----------------------------------------------------------------- */
		///
		/// Reset
		///
		/// <summary>
		/// CubeICE �̊e��ݒ荀�ڂ������Œ�`���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		virtual void Reset() {
			UserSettingBase::Reset();
			this->Parameters().push_back(Node(_T("Tooltip"), true));
			this->Parameters().push_back(Node(_T("TooltipCount"), 5));
			this->Parameters().push_back(Node(_T("Explorer"), _T("")));
			this->Parameters().push_back(Node(_T("ErrorReport"), true));
			this->Parameters().push_back(Node(_T("LogLevel"), PsdotNet::LogLevel::Error));
			this->Parameters().push_back(Node(_T("CheckUpdate"), true));
			this->Parameters().push_back(Node(_T("WarnOnDetail"), true));
			
			compression_.Reset();
			decompression_.Reset();
			association_.Reset();
			context_.Reset();
			dragdrop_.Reset();
			shortcut_.Reset();
			filtering_.Reset();
			runtime_.Reset();
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// Upgrade
		///
		/// <summary>
		/// ���o�[�W�����̃f�[�^�\�����猻�݂̂��̂ɕϊ����܂��B
		///
		/// NOTE: �������̍��ڂɂ��ẮA���o�[�W�����ɂ͓������̂�
		/// �\�����̂� compression/decompression ���ꂼ��ɑ��݂��܂��B
		/// ����ł́Adecomression �Őݒ肳��Ă���l���̗p���Ă��܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		virtual void Upgrade(const Version1& old) {
			this->Tooltip((old.decompression().details() & DETAIL_TOOLTIP) != 0);
			this->TooltipCount(old.decompression().max_filelist());
			this->Explorer(old.decompression().explorer());
			this->ErrorReport((old.decompression().details() & DETAIL_REPORT) != 0);
			this->LogLevel(old.loglevel());
			this->CheckUpdate(old.update());
			
			compression_.Upgrade(old);
			decompression_.Upgrade(old);
			association_.Upgrade(old);
			context_.Upgrade(old);
			dragdrop_.Upgrade(old);
			shortcut_.Upgrade(old);
			filtering_.Upgrade(old);
			runtime_.Upgrade(old);
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// Load
		///
		/// <summary>
		/// ���W�X�g������e�탆�[�U�ݒ��ǂݍ��݂܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void Load() {
			std::basic_stringstream<char_type> ss;
			ss << CUBEICE_REG_ROOT << _T("\\") << CUBEICE_REG_STRUCTVERSION;
			PsdotNet::RegistryKey root = PsdotNet::Registry::CurrentUser().CreateSubKey(ss.str());
			if (!root) return;
			
			Document doc;
			doc.Read(root);
			this->LoadFromDocument(doc);
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// Load
		///
		/// <summary>
		/// XML �t�@�C������e�탆�[�U�ݒ��ǂݍ��݂܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void Load(const string_type& path) {
			Document doc;
			doc.Read(path);
			this->LoadFromDocument(doc);
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// Save
		///
		/// <summary>
		/// ���W�X�g���֊e�탆�[�U�ݒ��ۑ����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void Save() {
			std::basic_stringstream<char_type> ss;
			ss << CUBEICE_REG_ROOT << _T("\\") << CUBEICE_REG_STRUCTVERSION;
			PsdotNet::RegistryKey root = PsdotNet::Registry::CurrentUser().CreateSubKey(ss.str());
			if (!root) return;
			
			Document doc;
			this->SaveToDocument(doc);
			doc.Write(root);
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// Save
		///
		/// <summary>
		/// XML �t�@�C���֊e�탆�[�U�ݒ��ۑ����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void Save(const string_type& path) {
			Document doc;
			this->SaveToDocument(doc);
			doc.Write(path);
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// InstallDirectory
		///
		/// <summary>
		/// CubeICE ���C���X�g�[�����ꂽ�f�B���N�g���̃p�X���擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		const string_type& InstallDirectory() const { return path_; }
		
		/* ----------------------------------------------------------------- */
		///
		/// Version
		///
		/// <summary>
		/// �C���X�g�[������Ă��� CubeICE �̃o�[�W���������擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		const string_type& Version() const { return version_; }
		
		/* ----------------------------------------------------------------- */
		///
		/// Association
		///
		/// <summary>
		/// �t�@�C���̊֘A�t���Ɋւ���ݒ���擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		association_type& Association() { return association_; }
		const association_type& Association() const { return association_; }
		
		/* ----------------------------------------------------------------- */
		///
		/// Context
		///
		/// <summary>
		/// �R���e�L�X�g���j���[�Ɋւ���ݒ���擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		context_type& Context() { return context_; }
		const context_type& Context() const { return context_; }
		
		/* ----------------------------------------------------------------- */
		///
		/// DragDrop
		///
		/// <summary>
		/// �h���b�O&�h���b�v�Ɋւ���ݒ���擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		dragdrop_type& DragDrop() { return dragdrop_; }
		const dragdrop_type& DragDrop() const { return dragdrop_; }
		
		/* ----------------------------------------------------------------- */
		///
		/// Shortcut
		///
		/// <summary>
		/// �f�X�N�g�b�v�ɍ쐬����V���[�g�J�b�g�Ɋւ���ݒ���擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		shortcut_type& Shortcut() { return shortcut_; }
		const shortcut_type& Shortcut() const { return shortcut_; }
		
		/* ----------------------------------------------------------------- */
		///
		/// Compression
		///
		/// <summary>
		/// ���k�Ɋւ���ݒ���擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		compression_type& Compression() { return compression_; }
		const compression_type& Compression() const { return compression_; }
		
		/* ----------------------------------------------------------------- */
		///
		/// Decompression
		///
		/// <summary>
		/// �𓀂Ɋւ���ݒ���擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		decompression_type& Decompression() { return decompression_; }
		const decompression_type& Decompression() const { return decompression_; }
		
		/* ----------------------------------------------------------------- */
		///
		/// Filtering
		///
		/// <summary>
		/// �t�B���^�����O�Ɋւ���ݒ���擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		filtering_type& Filtering() { return filtering_; }
		const filtering_type& Filtering() const { return filtering_; }
		
		/* ----------------------------------------------------------------- */
		///
		/// Runtime
		///
		/// �����^�C�����Ɋւ���ݒ���擾���܂��B
		///
		/* ----------------------------------------------------------------- */
		runtime_type& Runtime() { return runtime_; }
		const runtime_type& Runtime() const { return runtime_; }
		
		/* ----------------------------------------------------------------- */
		///
		/// Tooltip
		///
		/// <summary>
		/// �G�N�X�v���[���ň��k�t�@�C���Ƀ}�E�X�J�[�\�������킹������
		/// �\�������c�[���`�b�v�ŁA�t�@�C���ꗗ��\�����邩�ǂ�����
		/// �擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		const bool& Tooltip() const {
			return this->Find(_T("Tooltip")).Value<ValueKind::Bool>();
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// Tooltip
		///
		/// <summary>
		/// �G�N�X�v���[���ň��k�t�@�C���Ƀ}�E�X�J�[�\�������킹������
		/// �\�������c�[���`�b�v�ŁA�t�@�C���ꗗ��\�����邩�ǂ�����
		/// �ݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void Tooltip(bool enable) {
			this->Find(_T("Tooltip")).Value<ValueKind::Bool>() = enable;
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// TooltipCount
		///
		/// <summary>
		/// �G�N�X�v���[���ň��k�t�@�C���Ƀ}�E�X�J�[�\�������킹������
		/// �\�������c�[���`�b�v�ŁA�t�@�C���ꗗ��\������ۂ̍ő�\��
		/// �������擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		const int& TooltipCount() const {
			return this->Find(_T("TooltipCount")).Value<ValueKind::Number>();
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// TooltipCount
		///
		/// <summary>
		/// �G�N�X�v���[���ň��k�t�@�C���Ƀ}�E�X�J�[�\�������킹������
		/// �\�������c�[���`�b�v�ŁA�t�@�C���ꗗ��\������ۂ̍ő�\��
		/// ������ݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void TooltipCount(int n) {
			this->Find(_T("TooltipCount")).Value<ValueKind::Number>() = n;
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// Explorer
		///
		/// <summary>
		/// ���k�E�𓀏I����ɏo�͐�t�H���_���J���ۂɎg�p����v���O������
		/// �p�X���擾���܂��B�擾�����l���󕶎��̏ꍇ�A�G�N�X�v���[����
		/// �g�p����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		const string_type& Explorer() const {
			return this->Find(_T("Explorer")).Value<ValueKind::String>();
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// Explorer
		///
		/// <summary>
		/// ���k�E�𓀏I����ɏo�͐�t�H���_���J���ۂɎg�p����v���O������
		/// �p�X��ݒ肵�܂��B�擾�����l���󕶎��̏ꍇ�A�G�N�X�v���[����
		/// �g�p����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void Explorer(const string_type& path) {
			this->Find(_T("Explorer")).Value<ValueKind::String>() = path;
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// ErrorReport
		///
		/// <summary>
		/// CubeICE ���s���ɉ��炩�̃G���[�������������A�G���[���|�[�g�p
		/// �_�C�A���O��\�����邩�ǂ������擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		const bool& ErrorReport() const { return this->Find(_T("ErrorReport")).Value<ValueKind::Bool>(); }
		
		/* ----------------------------------------------------------------- */
		///
		/// ErrorReport
		///
		/// <summary>
		/// CubeICE ���s���ɉ��炩�̃G���[�������������A�G���[���|�[�g�p
		/// �_�C�A���O��\�����邩�ǂ�����ݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void ErrorReport(bool enable) { this->Find(_T("ErrorReport")).Value(enable); }
		
		/* ----------------------------------------------------------------- */
		///
		/// LogLevel
		///
		/// <summary>
		/// ���O�̏o�̓��x�����擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		const int& LogLevel() const {
			return this->Find(_T("LogLevel")).Value<ValueKind::Number>();
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// LogLevel
		///
		/// <summary>
		/// ���O�̏o�̓��x����ݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void LogLevel(int level) { this->Find(_T("LogLevel")).Value(level); }
		
		/* ----------------------------------------------------------------- */
		///
		/// LogLevel
		///
		/// <summary>
		/// ���O�̏o�̓��x����ݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void LogLevel(const PsdotNet::LogLevel& level) { this->LogLevel(level.ToEnum()); }
		
		/* ----------------------------------------------------------------- */
		///
		/// CheckUpdate
		///
		/// <summary>
		/// �A�b�v�f�[�g�`�F�b�N�v���O�������X�^�[�g�A�b�v�ɓo�^���邩
		/// �ǂ�����\���l���擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		const bool& CheckUpdate() const { return this->Find(_T("CheckUpdate")).Value<ValueKind::Bool>(); }
		
		/* ----------------------------------------------------------------- */
		///
		/// CheckUpdate
		///
		/// <summary>
		/// �A�b�v�f�[�g�`�F�b�N�v���O�������X�^�[�g�A�b�v�ɓo�^���邩
		/// �ǂ�����\���l��ݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void CheckUpdate(bool enable) { this->Find(_T("CheckUpdate")).Value(enable); }
		
		/* ----------------------------------------------------------------- */
		///
		/// WarnOnDetail
		///
		/// <summary>
		/// CubeICE �ڍאݒ���N�������ۂɌx����\�����邩�ǂ�����\���l��
		/// �擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		const bool& WarnOnDetail() const { return this->Find(_T("WarnOnDetail")).Value<ValueKind::Bool>(); }
		
		/* ----------------------------------------------------------------- */
		///
		/// WarnOnDetail
		///
		/// <summary>
		/// CubeICE �ڍאݒ���N�������ۂɌx����\�����邩�ǂ�����\���l��
		/// �ݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void WarnOnDetail(bool enable) { this->Find(_T("WarnOnDetail")).Value(enable); }
		
	private:
		/* ----------------------------------------------------------------- */
		///
		/// LoadBasicData
		///
		/// <summary>
		/// �C���X�g�[���f�B���N�g���A�o�[�W�������AHKEY_LOCAL_MACHINE �ȉ�
		/// �ɕۑ�����Ă��� CubeICE �̊�{�I�ȃf�[�^��ǂݍ��݂܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void LoadBasicData() {
			try {
				using PsdotNet::Registry;
				using PsdotNet::RegistryKey;
				using PsdotNet::RegistryValueKind;
				
				RegistryKey subkey = Registry::LocalMachine().OpenSubKey(CUBEICE_REG_ROOT, false);
				if (!subkey) return;
				
				path_ = subkey.GetValue<string_type>(CUBEICE_NAME_INSTALL_DIRECTORY);
				version_ = subkey.GetValue<string_type>(CUBEICE_NAME_VERSION);
			}
			catch (const std::exception& /* err */) {}
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// LoadFromDocument
		///
		/// <summary>
		/// CubeICE �̊e��ݒ��ǂݍ��݂܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void LoadFromDocument(const Document& doc) {
			this->LoadFromParameter(doc.Root());
			filtering_.LoadFromParameter(doc.Root());
			
			// ���k�֘A�̐ݒ�
			optional<const Node&> node_compression = PsdotNet::Parameter::FindOptional(doc.Root(), _T("Compression"));
			if (node_compression) compression_.LoadFromParameter(node_compression->Value<ValueKind::NodeSet>());
			
			// �𓀊֘A�̐ݒ�
			optional<const Node&> node_decompression = PsdotNet::Parameter::FindOptional(doc.Root(), _T("Decompression"));
			if (node_decompression) decompression_.LoadFromParameter(node_decompression->Value<ValueKind::NodeSet>());
			
			// �R���e�L�X�g���j���[�֘A�̐ݒ�
			optional<const Node&> node_context = PsdotNet::Parameter::FindOptional(doc.Root(), _T("Context"));
			if (node_context) context_.LoadFromParameter(node_context->Value<ValueKind::NodeSet>());
			
			// �h���b�O&�h���b�v�֘A�̐ݒ�
			optional<const Node&> node_dragdrop = PsdotNet::Parameter::FindOptional(doc.Root(), _T("DragDrop"));
			if (node_dragdrop) dragdrop_.LoadFromParameter(node_dragdrop->Value<ValueKind::NodeSet>());
			
			// �֘A�t���֘A�̐ݒ�
			optional<const Node&> node_association = PsdotNet::Parameter::FindOptional(doc.Root(), _T("Association"));
			if (node_association) association_.LoadFromParameter(node_association->Value<ValueKind::NodeSet>());
			
			// �V���[�g�J�b�g�֘A�̐ݒ�
			optional<const Node&> node_shortcut = PsdotNet::Parameter::FindOptional(doc.Root(), _T("Shortcut"));
			if (node_shortcut) shortcut_.LoadFromParameter(node_shortcut->Value<ValueKind::NodeSet>());
			
			// �����^�C�����̐ݒ�
			optional<const Node&> node_runtime = PsdotNet::Parameter::FindOptional(doc.Root(), _T("Runtime"));
			if (node_runtime) runtime_.LoadFromParameter(node_runtime->Value<ValueKind::NodeSet>());
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// SaveToDocument
		///
		/// <summary>
		/// CubeICE �̊e��ݒ��ۑ����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void SaveToDocument(Document& doc) {
			this->SaveToParameter(doc.Root());
			filtering_.SaveToParameter(doc.Root());
			
			// ���k�֘A�̐ݒ�
			NodeSet ns_compression;
			compression_.SaveToParameter(ns_compression);
			doc.Root().push_back(Node(_T("Compression"), ns_compression));
			
			// �𓀊֘A�̐ݒ�
			NodeSet ns_decompression;
			decompression_.SaveToParameter(ns_decompression);
			doc.Root().push_back(Node(_T("Decompression"), ns_decompression));
			
			// �R���e�L�X�g���j���[�֘A�̐ݒ�
			NodeSet ns_context;
			context_.SaveToParameter(ns_context);
			doc.Root().push_back(Node(_T("Context"), ns_context));
			
			// �h���b�O&�h���b�v�֘A�̐ݒ�
			NodeSet ns_dragdrop;
			dragdrop_.SaveToParameter(ns_dragdrop);
			doc.Root().push_back(Node(_T("DragDrop"), ns_dragdrop));
			
			// �֘A�t���֘A�̐ݒ�
			NodeSet ns_association;
			association_.SaveToParameter(ns_association);
			doc.Root().push_back(Node(_T("Association"), ns_association));
			
			// �V���[�g�J�b�g�֘A�̐ݒ�
			NodeSet ns_shortcut;
			shortcut_.SaveToParameter(ns_shortcut);
			doc.Root().push_back(Node(_T("Shortcut"), ns_shortcut));
			
			// �����^�C�����̐ݒ�
			NodeSet ns_runtime;
			runtime_.SaveToParameter(ns_runtime);
			doc.Root().push_back(Node(_T("Runtime"), ns_runtime));
		}
		
	private:
		string_type             path_;              // �C���X�g�[���f�B���N�g��
		string_type             version_;           // �o�[�W����
		association_type        association_;       // �t�@�C���̊֘A�t��
		context_type            context_;           // �R���e�L�X�g���j���[
		dragdrop_type           dragdrop_;          // �h���b�O&�h���b�v���j���[
		shortcut_type           shortcut_;          // �f�X�N�g�b�v�ɍ쐬����V���[�g�J�b�g
		compression_type        compression_;       // ���k
		decompression_type      decompression_;     // ��
		filtering_type          filtering_;         // �t�B���^�����O
		runtime_type            runtime_;           // �����^�C�����̐ݒ�
	}; // class UserSetting
} // namespace CubeICE

#endif // CUBEICE_USER_SETTING_H
