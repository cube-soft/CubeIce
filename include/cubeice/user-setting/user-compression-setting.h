// -*- coding: shift-jis -*-
/* ------------------------------------------------------------------------- */
/*
 *  cubeice/user-setting/user-compression-setting.h
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
#ifndef CUBEICE_USER_SETTING_USER_COMPRESSION_SETTING_H
#define CUBEICE_USER_SETTING_USER_COMPRESSION_SETTING_H

#include <cubeice/config.h>
#include <cubeice/user-setting/user-setting-base.h>
#include <cubeice/user-setting/user-setting-value.h>

namespace CubeICE {
	/* --------------------------------------------------------------------- */
	///
	/// UserCompressionSetting
	///
	/// <summary>
	/// CubeICE ���k�Ɋ֘A����e�탆�[�U�ݒ���擾�E�ۑ�����N���X�ł��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	class UserCompressionSetting : public UserSettingBase {
	public:
		/* ----------------------------------------------------------------- */
		/// constructor
		/* ----------------------------------------------------------------- */
		UserCompressionSetting() :
			UserSettingBase() {
			this->Reset();
		}
		
		/* ----------------------------------------------------------------- */
		/// constructor
		/* ----------------------------------------------------------------- */
		UserCompressionSetting(const UserCompressionSetting& cp) :
			UserSettingBase(cp) {}
		
		/* ----------------------------------------------------------------- */
		/// destructor
		/* ----------------------------------------------------------------- */
		virtual ~UserCompressionSetting() {}
		
		/* ----------------------------------------------------------------- */
		///
		/// Reset
		///
		/// <summary>
		/// CubeICE �̈��k�Ɋ֘A����ݒ�\�ȍ��ڂ������Œ�`���܂��B
		/// �ݒ荀�ڂ��ƂɁA���O�A����я����l���`���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		virtual void Reset() {
			UserSettingBase::Reset();
			this->Parameters().push_back(Node(_T("OutputCondition"), OutputKind::Specific));
			this->Parameters().push_back(Node(_T("OutputPath"), _T("")));
			this->Parameters().push_back(Node(_T("DefaultType"), _T("zip")));
			this->Parameters().push_back(Node(_T("DefaultEnablePassword"), false));
			this->Parameters().push_back(Node(_T("Overwrite"), OverwriteKind::Confirm));
			this->Parameters().push_back(Node(_T("Filtering"), false));
			this->Parameters().push_back(Node(_T("Open"), OpenKind::Skip));
			this->Parameters().push_back(Node(_T("DeleteOnMail"), false));
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
			this->OutputCondition(old.compression().output_condition());
			this->OutputPath(old.compression().output_path());
			
			int details = old.compression().details();
			if ((details & DETAIL_OVERWRITE) != 0) this->Overwrite(OverwriteKind::Confirm);
			else this->Overwrite(OverwriteKind::Force);
			
			if ((details & DETAIL_OPEN) == 0) this->Open(OpenKind::Skip);
			else if ((details & DETAIL_SKIP_DESKTOP) != 0) this->Open(OpenKind::SkipDesktop);
			else this->Open(OpenKind::Open);
			
			this->Filtering((details & DETAIL_FILTER) != 0);
			this->DeleteOnMail((details & DETAIL_MAIL_REMOVE) != 0);
			
			switch (old.shortcut_compress_index()) {
			case 0:
			case 1:
				this->DefaultType(_T("zip"));
				break;
			case 2:
				this->DefaultType(_T("7z"));
				break;
			case 3:
				this->DefaultType(_T("gz"));
				break;
			case 4:
				this->DefaultType(_T("bz2"));
				break;
			
			}
			this->DefaultEnablePassword(old.shortcut_compress_index() == 1);
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// OutputCondition
		///
		/// <summary>
		/// ���k�t�@�C���̕ۑ���̌�����@���擾���܂��B
		/// ������@�̏ڍׂɂ��ẮACubeICE::OutputKind ���Q�Ɖ������B
		/// </summary>
		///
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
		 *  DefaultType
		 *
		 *  �u�����Ɉ��k�v�ȂǁA�����I�Ɉ��k�`�����w�肳��Ȃ������ꍇ��
		 *  �g�p����鈳�k�`�����擾���܂��B
		 */
		/* ----------------------------------------------------------------- */
		const string_type& DefaultType() const {
			return this->Find(_T("DefaultType")).Value<ValueKind::String>();
		}
		
		/* ----------------------------------------------------------------- */
		/*!
		 *  DefaultType
		 *
		 *  �u�����Ɉ��k�v�ȂǁA�����I�Ɉ��k�`�����w�肳��Ȃ������ꍇ��
		 *  �g�p����鈳�k�`����ݒ肵�܂��B
		 */
		/* ----------------------------------------------------------------- */
		void DefaultType(const string_type& type) {
			this->Find(_T("DefaultType")).Value<ValueKind::String>() = type;
		}
		
		/* ----------------------------------------------------------------- */
		/*!
		 *  DefaultEnablePassword
		 *
		 *  �u�����Ɉ��k�v�ȂǁA�����I�Ɉ��k�`�����w�肳��Ȃ������ꍇ��
		 *  �p�X���[�h��ݒ肷�邩�ǂ������擾���܂��BDefaultType() �Őݒ�
		 *  ����Ă��鈳�k�`�����p�X���[�h�ɂ��Í����ɑΉ����Ă��Ȃ��ꍇ�A
		 *  ���̐ݒ�͖�������܂��B
		 */
		/* ----------------------------------------------------------------- */
		const bool& DefaultEnablePassword() const {
			return this->Find(_T("DefaultEnablePassword")).Value<ValueKind::Bool>();
		}
		
		/* ----------------------------------------------------------------- */
		/*!
		 *  DefaultEnablePassword
		 *
		 *  �u�����Ɉ��k�v�ȂǁA�����I�Ɉ��k�`�����w�肳��Ȃ������ꍇ��
		 *  �p�X���[�h��ݒ肷�邩�ǂ�����ݒ肵�܂��BDefaultType() �Őݒ�
		 *  ����Ă��鈳�k�`�����p�X���[�h�ɂ��Í����ɑΉ����Ă��Ȃ��ꍇ�A
		 *  ���̐ݒ�͖�������܂��B
		 */
		/* ----------------------------------------------------------------- */
		void DefaultEnablePassword(bool enable) {
			this->Find(_T("DefaultEnablePassword")).Value<ValueKind::Bool>() = enable;
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
		 *  DeleteOnMail
		 *
		 *  �u���k���ă��[�����M�v��I�����A�����I����Ɉ��k�t�@�C�����폜
		 *  ���邩�ǂ������擾���܂��B
		 */
		/* ----------------------------------------------------------------- */
		const bool& DeleteOnMail() const {
			return this->Find(_T("DeleteOnMail")).Value<ValueKind::Bool>();
		}
		
		/* ----------------------------------------------------------------- */
		/*!
		 *  DeleteOnMail
		 *
		 *  �u���k���ă��[�����M�v��I�����A�����I����Ɉ��k�t�@�C�����폜
		 *  ���邩�ǂ�����ݒ肵�܂��B
		 */
		/* ----------------------------------------------------------------- */
		void DeleteOnMail(bool enable) {
			this->Find(_T("DeleteOnMail")).Value<ValueKind::Bool>() = enable;
		}
	};
} // namespace CubeICE

#endif // CUBEICE_USER_SETTING_USER_COMPRESSION_SETTING_H
