// -*- coding: shift-jis -*-
/* ------------------------------------------------------------------------- */
/*
 *  cubeice/archiveinfo.h
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
#ifndef CUBEICE_ARCHIVELIST_H
#define CUBEICE_ARCHIVELIST_H

#include <cubeice/config.h>
#include <cubeice/fileinfo.h>
#include <vector>

namespace CubeICE {
	/* --------------------------------------------------------------------- */
	///
	/// ArchiveInfo
	///
	/// <summary>
	/// ���k�t�@�C���̊e������擾���邽�߂̃N���X�ł��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	class ArchiveInfo {
	public:
		typedef CubeICE::char_type char_type;
		typedef CubeICE::string_type string_type;
		typedef std::vector<CubeICE::FileInfo> FileList;
		
	public:
		/* ----------------------------------------------------------------- */
		/// constructor
		/* ----------------------------------------------------------------- */
		ArchiveInfo() :
			files_() {}
		
		/* ----------------------------------------------------------------- */
		/// destructor
		/* ----------------------------------------------------------------- */
		virtual ~ArchiveInfo() {}
		
		/* ----------------------------------------------------------------- */
		///
		/// Reset
		///
		/// <summary>
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void Reset() {
			files_.clear();
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// Reset
		///
		/// <summary>
		/// ���ݕێ����Ă���t�@�C���A����уf�B���N�g���̈ꗗ��j�����A
		/// �����Ŏw�肳�ꂽ���k�t�@�C�����Ɋ܂܂��t�@�C���A�����
		/// �f�B���N�g���ꗗ�̏��ŏ㏑�����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void Reset(const string_type& path) {
			this->Reset();
			this->ParseByExe(path);
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// Files
		///
		/// <summary>
		/// ���k�t�@�C�����Ɋ܂܂��t�@�C���A����уf�B���N�g���̈ꗗ��
		/// �擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		const FileList& Files() const { return files_; }
		
	private:
		/* ----------------------------------------------------------------- */
		/* ----------------------------------------------------------------- */
		void Parse(const string_type& path) {
			
		}
		
	private:
		FileList files_;
	}; // class ArchiveList
} // namespace CubeICE

#endif // CUBEICE_ARCHIVELIST_H
