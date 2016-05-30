// -*- coding: shift-jis -*-
/* ------------------------------------------------------------------------- */
/*
 *  cubeice/user-setting/user-decompression-setting.h
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
#ifndef CUBEICE_USER_SETTING_USER_DECOMPRESSION_SETTING_H
#define CUBEICE_USER_SETTING_USER_DECOMPRESSION_SETTING_H

#include <cubeice/config.h>
#include <cubeice/user-setting/user-setting-base.h>
#include <cubeice/user-setting/user-setting-value.h>

namespace CubeICE {
	/* --------------------------------------------------------------------- */
	///
	/// UserDecompressionSetting
	///
	/// <summary>
	/// CubeICE ���k�Ɋ֘A����e�탆�[�U�ݒ���擾�E�ۑ�����N���X�ł��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	class UserDecompressionSetting : public UserSettingBase {
	public:
		/* ----------------------------------------------------------------- */
		/// constructor
		/* ----------------------------------------------------------------- */
		UserDecompressionSetting() :
			UserSettingBase() {
			this->Reset();
		}
		
		/* ----------------------------------------------------------------- */
		/// constructor
		/* ----------------------------------------------------------------- */
		UserDecompressionSetting(const UserDecompressionSetting& cp) :
			UserSettingBase(cp) {}
		
		/* ----------------------------------------------------------------- */
		/// destructor
		/* ----------------------------------------------------------------- */
		virtual ~UserDecompressionSetting() {}
		
		/* ----------------------------------------------------------------- */
		///
		/// Reset
		///
		/// <summary>
		/// CubeICE �̉𓀂Ɋ֘A����ݒ�\�ȍ��ڂ������Œ�`���܂��B
		/// �ݒ荀�ڂ��ƂɁA���O�A����я����l���`���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		virtual void Reset() {
			UserSettingBase::Reset();
			this->Parameters().push_back(Node(_T("OutputCondition"), OutputKind::Specific));
			this->Parameters().push_back(Node(_T("OutputPath"), _T("")));
			this->Parameters().push_back(Node(_T("SourceEncoding"), EncodingKind::Auto));
			this->Parameters().push_back(Node(_T("Overwrite"), OverwriteKind::Confirm));
			this->Parameters().push_back(Node(_T("CreateFolder"), CreateFolderKind::SkipSingleFolder));
			this->Parameters().push_back(Node(_T("Filtering"), true));
			this->Parameters().push_back(Node(_T("Open"), OpenKind::SkipDesktop));
			this->Parameters().push_back(Node(_T("DeleteOnExtract"), false));
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
			this->OutputCondition(old.decompression().output_condition());
			this->OutputPath(old.decompression().output_path());
			
			int details = old.decompression().details();
			if ((details & DETAIL_OVERWRITE) == 0) this->Overwrite(OverwriteKind::Force);
			else if ((details & DETAIL_IGNORE_NEWER) != 0) this->Overwrite(OverwriteKind::ConfirmOlder);
			else this->Overwrite(OverwriteKind::Confirm);
			
			if ((details & DETAIL_CREATE_FOLDER) == 0) this->CreateFolder(CreateFolderKind::Skip);
			else if ((details & DETAIL_SINGLE_FILE) != 0) this->CreateFolder(CreateFolderKind::SkipSingleFile);
			else if ((details & DETAIL_SINGLE_FOLDER) != 0) this->CreateFolder(CreateFolderKind::SkipSingleFolder);
			else this->CreateFolder(CreateFolderKind::Create);
			
			if ((details & DETAIL_OPEN) == 0) this->Open(OpenKind::Skip);
			else if ((details & DETAIL_SKIP_DESKTOP) != 0) this->Open(OpenKind::SkipDesktop);
			else this->Open(OpenKind::Open);
			
			this->Filtering((details & DETAIL_FILTER) != 0);
			this->DeleteOnExtract((details & DETAIL_REMOVE_SRC) != 0);
		}
		
		/* ----------------------------------------------------------------- */
		/*!
		 *  OutputCondition
		 *
		 *  ���k�t�@�C���̕ۑ���̌�����@���擾���܂��B
		 *  ������@�̏ڍׂɂ��ẮACubeICE::OutputKind ���Q�Ɖ������B
		 */
		/* ----------------------------------------------------------------- */
		const int& OutputCondition() const {
			return this->Find(_T("OutputCondition")).Value<ValueKind::Number>();
		}
		
		/* ----------------------------------------------------------------- */
		/*!
		 *  OutputCondition
		 *
		 *  ���k�t�@�C���̕ۑ���̌�����@��ݒ肵�܂��B
		 *  ������@�̏ڍׂɂ��ẮACubeICE::OutputKind ���Q�Ɖ������B
		 */
		/* ----------------------------------------------------------------- */
		void OutputCondition(int condition) {
			this->Find(_T("OutputCondition")).Value<ValueKind::Number>() = condition;
		}
		
		/* ----------------------------------------------------------------- */
		/*!
		 *  OutputPath
		 *
		 *  ���k�t�@�C���̕ۑ���p�X���擾���܂��B
		 *  OutputPath() �́AOutputCondition() �֐��� OutputKind::Specific
		 *  �ɐݒ肳��Ă��鎞�̂ݎg�p����܂��B�擾�ł���l���󕶎��̏ꍇ�A
		 *  �f�X�N�g�b�v��ۑ���Ƃ��܂��B
		 */
		/* ----------------------------------------------------------------- */
		const string_type& OutputPath() const {
			return this->Find(_T("OutputPath")).Value<ValueKind::String>();
		}
		
		/* ----------------------------------------------------------------- */
		/*!
		 *  OutputPath
		 *
		 *  ���k�t�@�C���̕ۑ���p�X��ݒ肵�܂��B
		 *  OutputPath() �́AOutputCondition() �֐��� OutputKind::Specific
		 *  �ɐݒ肳��Ă��鎞�̂ݎg�p����܂��B�擾�ł���l���󕶎��̏ꍇ�A
		 *  �f�X�N�g�b�v��ۑ���Ƃ��܂��B
		 */
		/* ----------------------------------------------------------------- */
		void OutputPath(const string_type& path) {
			this->Find(_T("OutputPath")).Value<ValueKind::String>() = path;
		}
		
		/* ----------------------------------------------------------------- */
		/*!
		 *  SourceEncoding
		 *
		 *  �����G���R�[�f�B���O��ϊ�����ۂ́A���͌��̕����G���R�[�f�B���O
		 *  �̎w����擾���܂��B�ڍׂɂ��ẮACubeICE::EncodingKind ��
		 *  �Q�Ɖ������B
		 */
		/* ----------------------------------------------------------------- */
		const int& SourceEncoding() const {
			return this->Find(_T("SourceEncoding")).Value<ValueKind::Number>();
		}
		
		/* ----------------------------------------------------------------- */
		/*!
		 *  SourceEncoding
		 *
		 *  �����G���R�[�f�B���O��ϊ�����ۂ́A���͌��̕����G���R�[�f�B���O
		 *  �̎w���ݒ肵�܂��B�ڍׂɂ��ẮACubeICE::EncodingKind ��
		 *  �Q�Ɖ������B
		 */
		/* ----------------------------------------------------------------- */
		void SourceEncoding(int kind) {
			this->Find(_T("SourceEncoding")).Value<ValueKind::Number>() = kind;
		}
		
		/* ----------------------------------------------------------------- */
		/*!
		 *  Overwrite
		 *
		 *  �����̈��k�t�@�C�������݂���ꍇ�ɁA�㏑���m�F�p�̃_�C�A���O��
		 *  �\�����邩�ǂ������擾���܂��B
		 *  �ڍׂɂ��ẮACubeICE::OverwriteKind ���Q�Ɖ������B
		 */
		/* ----------------------------------------------------------------- */
		const int& Overwrite() const {
			return this->Find(_T("Overwrite")).Value<ValueKind::Number>();
		}
		
		/* ----------------------------------------------------------------- */
		/*!
		 *  Overwrite
		 *
		 *  �����̈��k�t�@�C�������݂���ꍇ�ɁA�㏑���m�F�p�̃_�C�A���O��
		 *  �\�����邩�ǂ�����ݒ肵�܂��B
		 *  �ڍׂɂ��ẮACubeICE::OverwriteKind ���Q�Ɖ������B
		 */
		/* ----------------------------------------------------------------- */
		void Overwrite(int kind) {
			this->Find(_T("Overwrite")).Value<ValueKind::Number>() = kind;
		}
		
		/* ----------------------------------------------------------------- */
		/*!
		 *  CreateFolder
		 *
		 *  �𓀂���ۂɃt�H���_���쐬���邩�ǂ������擾���܂��B
		 *  �ڍׂɂ��ẮACubeICE::CreateFolderKind ���Q�Ɖ������B
		 */
		/* ----------------------------------------------------------------- */
		const int& CreateFolder() const {
			return this->Find(_T("CreateFolder")).Value<ValueKind::Number>();
		}
		
		/* ----------------------------------------------------------------- */
		/*!
		 *  CreateFolder
		 *
		 *  �𓀂���ۂɃt�H���_���쐬���邩�ǂ�����ݒ肵�܂��B
		 *  �ڍׂɂ��ẮACubeICE::CreateFolderKind ���Q�Ɖ������B
		 */
		/* ----------------------------------------------------------------- */
		void CreateFolder(int kind) {
			this->Find(_T("CreateFolder")).Value<ValueKind::Number>() = kind;
		}
		
		/* ----------------------------------------------------------------- */
		/*!
		 *  Filtering
		 *
		 *  ���k����ۂɁA����̃t�@�C����t�H���_���t�B���^�����O���邩
		 *  �ǂ������擾���܂��B
		 */
		/* ----------------------------------------------------------------- */
		const bool& Filtering() const {
			return this->Find(_T("Filtering")).Value<ValueKind::Bool>();
		}
		
		/* ----------------------------------------------------------------- */
		/*!
		 *  Filtering
		 *
		 *  ���k����ۂɁA����̃t�@�C����t�H���_���t�B���^�����O���邩
		 *  �ǂ�����ݒ肵�܂��B
		 */
		/* ----------------------------------------------------------------- */
		void Filtering(bool enable) {
			this->Find(_T("Filtering")).Value<ValueKind::Bool>() = enable;
		}
		
		/* ----------------------------------------------------------------- */
		/*!
		 *  Open
		 *
		 *  ���k�����I����A���k�t�@�C����ۑ������t�H���_���J�����ǂ�����
		 *  �擾���܂��B�ڍׂ́ACubeICE::OpenKind ���Q�Ɖ������B
		 */
		/* ----------------------------------------------------------------- */
		const int& Open() const {
			return this->Find(_T("Open")).Value<ValueKind::Number>();
		}
		
		/* ----------------------------------------------------------------- */
		/*!
		 *  Open
		 *
		 *  ���k�����I����A���k�t�@�C����ۑ������t�H���_���J�����ǂ�����
		 *  �ݒ肵�܂��B�ڍׂ́ACubeICE::OpenKind ���Q�Ɖ������B
		 */
		/* ----------------------------------------------------------------- */
		void Open(int kind) {
			this->Find(_T("Open")).Value<ValueKind::Number>() = kind;
		}
		
		/* ----------------------------------------------------------------- */
		/*!
		 *  DeleteOnExtract
		 *
		 *  �𓀌�A���̓t�@�C�����폜���邩�ǂ������擾���܂��B
		 */
		/* ----------------------------------------------------------------- */
		const bool& DeleteOnExtract() const {
			return this->Find(_T("DeleteOnExtract")).Value<ValueKind::Bool>();
		}
		
		/* ----------------------------------------------------------------- */
		/*!
		 *  DeleteOnExtract
		 *
		 *  �𓀌�A���̓t�@�C�����폜���邩�ǂ�����ݒ肵�܂��B
		 */
		/* ----------------------------------------------------------------- */
		void DeleteOnExtract(bool enable) {
			this->Find(_T("DeleteOnExtract")).Value<ValueKind::Bool>() = enable;
		}
	}; // class UserDecompressionSetting
} // namespace CubeICE

#endif // CUBEICE_USER_SETTING_USER_DECOMPRESSION_SETTING_H
