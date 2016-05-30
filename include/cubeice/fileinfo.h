// -*- coding: shift-jis -*-
/* ------------------------------------------------------------------------- */
/*
 *  cubeice/fileinfo.h
 *
 *  Copyright (c) 2010 CubeSoft, Inc.
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
#ifndef CUBEICE_FILEINFO_H
#define CUBEICE_FILEINFO_H

#include <cubeice/config.h>
#include <psdotnet/date-time.h>
#include <psdotnet/io/filesystem-info.h>

namespace CubeICE {
	/* --------------------------------------------------------------------- */
	///
	/// FileInfo
	///
	/// <summary>
	/// ���k�t�@�C�����̃t�@�C���A�܂��̓f�B���N�g���̏���\�����߂�
	/// �N���X�ł��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	class FileInfo {
	public:
		typedef CubeICE::char_type char_type;
		typedef CubeICE::string_type string_type;
		typedef __int64 size_type;
		typedef PsdotNet::DateTime DateTime;
		
	public:
		/* ----------------------------------------------------------------- */
		/// constructor
		/* ----------------------------------------------------------------- */
		FileInfo() :
			name_(), attr_(0), size_(0), write_() {}
		
		/* ----------------------------------------------------------------- */
		/// destructor
		/* ----------------------------------------------------------------- */
		virtual ~FileInfo() {}
		
		/* ----------------------------------------------------------------- */
		///
		/// Name
		///
		/// <summary>
		/// �t�@�C�������擾�A�܂��͐ݒ肵�܂��B���k�t�@�C�����̃t�@�C���́A
		/// �f�B���N�g���Ɋi�[����Ă��铙�̊֌W�ō\��������Ă���ꍇ��
		/// ����܂��B���̏ꍇ�́A���[�g�f�B���N�g������̑��΃p�X��\���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		const string_type& Name() const { return name_; }
		void Name(const string_type& name) { name_ = name; }
		
		/* ----------------------------------------------------------------- */
		///
		/// Attributes
		///
		/// <summary>
		/// ���݂̃t�@�C���� FileAttributes ���擾�܂��͐ݒ肵�܂��B
		/// NOTE: �����_�� CubeICE ���g�p���Ă��鑮���� Directory �݂̂ł��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void Attributes(const size_type& attr) { attr_ = attr; }
		size_type& Attributes() { return attr_; }
		const size_type& Attributes() const { return attr_; }
		
		/* ----------------------------------------------------------------- */
		///
		/// Attributes
		///
		/// <summary>
		/// ���݂̃t�@�C���̃t�@�C���T�C�Y���擾�A�܂��͐ݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		const size_type& Size() const { return size_; }
		void Size(size_type size) { size_ = size; }
		
		/* ----------------------------------------------------------------- */
		///
		/// LastWriteTime
		///
		/// <summary>
		/// ���݂̃t�@�C���܂��̓f�B���N�g���ɍŌ�ɏ������݂��s��ꂽ
		/// �������擾�A�܂��͐ݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		const DateTime& LastWriteTime() const { return write_; }
		void LastWriteTime(const DateTime& write) { write_ = write; }
		
	private:
		string_type name_;
		size_type attr_;
		size_type size_;
		DateTime write_;
	}; // class FileInfo
} // namespace CubeICE

#endif // CUBEICE_FILEINFO_H
