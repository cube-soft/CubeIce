// -*- coding: shift-jis -*-
/* ------------------------------------------------------------------------- */
/*
 *  cubeice/user-setting/runtime-compression-setting.h
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
#ifndef CUBEICE_USER_SETTING_RUNTIME_COMPRESSION_SETTING_H
#define CUBEICE_USER_SETTING_RUNTIME_COMPRESSION_SETTING_H

#include <cubeice/config.h>
#include <cubeice/user-setting/user-setting-base.h>

namespace CubeICE {
	/* --------------------------------------------------------------------- */
	/// RuntimeCompressionSetting
	/* --------------------------------------------------------------------- */
	class RuntimeCompressionSetting : public UserSettingBase {
	public:
		/* ----------------------------------------------------------------- */
		/// constructor
		/* ----------------------------------------------------------------- */
		RuntimeCompressionSetting() :
			UserSettingBase(), path_(), password_() {}
		
		/* ----------------------------------------------------------------- */
		/// destructor
		/* ----------------------------------------------------------------- */
		virtual ~RuntimeCompressionSetting() {}
		
		/* ----------------------------------------------------------------- */
		///
		/// Reset
		///
		/// <summary>
		/// CubeICE �̃����^�C�����̊e��ݒ�����������܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		virtual void Reset() {
			UserSettingBase::Reset();
			this->Parameters().push_back(Node(_T("Type"), _T("zip")));
			this->Parameters().push_back(Node(_T("Level"), CompressionLevel::Normal));
			this->Parameters().push_back(Node(_T("Method"), _T("Deflate")));
			this->Parameters().push_back(Node(_T("EnablePassword"), false));
			this->Parameters().push_back(Node(_T("ShowPassword"), false));
			this->Parameters().push_back(Node(_T("EncodingMethod"), _T("ZipCrypto")));
			this->Parameters().push_back(Node(_T("Overwrite"), OverwriteKind::Confirm));
			this->Parameters().push_back(Node(_T("ThreadSize"), 1));
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// Type
		///
		/// <summary>
		/// ���k�`�����擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		const string_type& Type() const { return this->Find(_T("Type")).Value<ValueKind::String>(); }
		
		/* ----------------------------------------------------------------- */
		///
		/// Type
		///
		/// <summary>
		/// ���k�`����ݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void Type(const string_type& str) { this->Find(_T("Type")).Value(str); }
		
		/* ----------------------------------------------------------------- */
		///
		/// Level
		///
		/// <summary>
		/// ���k���x�����擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		const int& Level() const { return this->Find(_T("Level")).Value<ValueKind::Number>(); }
		
		/* ----------------------------------------------------------------- */
		///
		/// Level
		///
		/// <summary>
		/// ���k���x����ݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void Level(int n) { this->Find(_T("Level")).Value(n); }
		
		/* ----------------------------------------------------------------- */
		///
		/// Method
		///
		/// <summary>
		/// ���k���@���擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		const string_type& Method() const { return this->Find(_T("Method")).Value<ValueKind::String>(); }
		
		/* ----------------------------------------------------------------- */
		///
		/// Method
		///
		/// <summary>
		/// ���k���@��ݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void Method(const string_type& str) { this->Find(_T("Method")).Value(str); }
		
		/* ----------------------------------------------------------------- */
		///
		/// EnablePassword
		///
		/// <summary>
		/// �p�X���[�h��ݒ肷�邩�ǂ������擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		const bool& EnablePassword() const { return this->Find(_T("EnablePassword")).Value<ValueKind::Bool>(); }
		
		/* ----------------------------------------------------------------- */
		///
		/// EnablePassword
		///
		/// <summary>
		/// �p�X���[�h��ݒ肷�邩�ǂ�����ݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void EnablePassword(bool enable) { this->Find(_T("EnablePassword")).Value(enable); }
		
		/* ----------------------------------------------------------------- */
		///
		/// ShowPassword
		///
		/// <summary>
		/// �p�X���[�h����͂���ۂɁA���͂����p�X���[�h���̂܂܂�\������
		/// ���ǂ������擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		const bool& ShowPassword() const { return this->Find(_T("ShowPassword")).Value<ValueKind::Bool>(); }
		
		/* ----------------------------------------------------------------- */
		///
		/// ShowPassword
		///
		/// <summary>
		/// �p�X���[�h����͂���ۂɁA���͂����p�X���[�h���̂܂܂�\������
		/// ���ǂ�����ݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void ShowPassword(bool enable) { this->Find(_T("ShowPassword")).Value(enable); }
		
		/* ----------------------------------------------------------------- */
		///
		/// EncodingMethod
		///
		/// <summary>
		/// �p�X���[�h��ݒ肷��ۂ̈Í����������擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		const string_type& EncodingMethod() const { return this->Find(_T("EncodingMethod")).Value<ValueKind::String>(); }
		
		/* ----------------------------------------------------------------- */
		///
		/// EncodingMethod
		///
		/// <summary>
		/// �p�X���[�h��ݒ肷��ۂ̈Í���������ݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void EncodingMethod(const string_type& str) { this->Find(_T("EncodingMethod")).Value(str); }
		
		/* ----------------------------------------------------------------- */
		///
		/// Overwrite
		///
		/// <summary>
		/// �����̈��k�t�@�C�������݂��Ă���ꍇ�̈������擾���܂��B
		/// �ݒ�\�Ȓl�͎��̒ʂ�ł��B
		/// 
		/// 0 : �㏑��
		/// 1 : �����̈��k�t�@�C���ɒǉ�
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		const int& Overwrite() const { return this->Find(_T("Overwrite")).Value<ValueKind::Number>(); }
		
		/* ----------------------------------------------------------------- */
		///
		/// Overwrite
		///
		/// <summary>
		/// �����̈��k�t�@�C�������݂��Ă���ꍇ�̈�����ݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void Overwrite(int n) { this->Find(_T("Overwrite")).Value(n); }
		
		/* ----------------------------------------------------------------- */
		///
		/// ThreadSize
		///
		/// <summary>
		/// CPU �X���b�h�����擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		const int& ThreadSize() const { return this->Find(_T("ThreadSize")).Value<ValueKind::Number>(); }
		
		/* ----------------------------------------------------------------- */
		///
		/// ThreadSize
		///
		/// <summary>
		/// CPU �X���b�h����ݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void ThreadSize(int n) { this->Find(_T("ThreadSize")).Value(n); }
		
		/* ----------------------------------------------------------------- */
		///
		/// OutputPath
		///
		/// <summary>
		/// �o�͐�p�X���擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		const string_type& OutputPath() const { return path_; }
		
		/* ----------------------------------------------------------------- */
		///
		/// OutputPath
		///
		/// <summary>
		/// �o�͐�p�X��ݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void OutputPath(const string_type& path) { path_ = path; }
		
		/* ----------------------------------------------------------------- */
		///
		/// Password
		///
		/// <summary>
		/// �p�X���[�h���擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		const string_type& Password() const { return password_; }
		
		/* ----------------------------------------------------------------- */
		///
		/// Password
		///
		/// <summary>
		/// �p�X���[�h��ݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void Password(const string_type& str) { password_ = str; }
		
	private:
		string_type path_;
		string_type password_;
	};
} // namespace CubeICE

#endif // CUBEICE_USER_SETTING_RUNTIME_COMPRESSION_SETTING_H
