// -*- coding: shift-jis -*-
/* ------------------------------------------------------------------------- */
/*
 *  cubeice/user-setting/utility.h
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
#ifndef CUBEICE_USER_SETTING_UTILITY_H
#define CUBEICE_USER_SETTING_UTILITY_H

#include <cubeice/config.h>
#include <cubeice/context.h>
#include <map>

namespace CubeICE {
	/* --------------------------------------------------------------------- */
	///
	/// CompressionLevel
	///
	/// <summary>
	/// ���k���x�����`���܂��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	namespace CompressionLevel {
		enum {
			None                = 0, // �����k
			Fastest             = 1, // �ő�
			Fast                = 3, // ����
			Normal              = 5, // �W��
			Maximum             = 7, // �ō�
			Ultra               = 9  // �����k
		};
	}
	
	/* --------------------------------------------------------------------- */
	///
	/// OutputKind
	///
	/// <summary>
	/// �o�͐�̌�����@���`���܂��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	namespace OutputKind {
		enum {
			Specific            = 0x00, // �\�ߌ��߂�ꂽ�t�H���_���o�͐�Ƃ��܂��B
			SameAsSource        = 0x01, // ���͌��Ɠ����t�H���_���o�͐�Ƃ��܂��B
			Runtime             = 0x02  // ���s���Ɏw�肵�܂��B
		};
	}
	
	/* --------------------------------------------------------------------- */
	///
	/// OverwriteKind
	///
	/// <summary>
	/// �����̃t�@�C�������݂����ꍇ�̏㏑�����@���`���܂��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	namespace OverwriteKind {
		enum {
			Force               = 0x00, // �����I�ɏ㏑�����܂��B
			Confirm             = 0x01, // �㏑���̊m�F���s���܂��B
			ConfirmOlder        = 0x02, // �X�V�������V�����ꍇ�͋����I�ɏ㏑�����܂��B
			Add                 = 0x03  // �ǉ����܂��B
		};
	}
	
	/* --------------------------------------------------------------------- */
	///
	/// CreateFolderKind
	///
	/// <summary>
	/// �𓀎��Ƀt�H���_���쐬���邩�ǂ������`���܂��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	namespace CreateFolderKind {
		enum {
			Skip                = 0x00, // �t�H���_���쐬���܂���B
			Create              = 0x01, // �t�H���_���쐬���܂��B
			SkipSingleFolder    = 0x02, // �P��t�H���_�̏ꍇ�A�t�H���_���쐬���܂���B
			SkipSingleFile      = 0x03  // �P��t�@�C���A�܂��͒P��t�H���_�̏ꍇ�A�t�H���_���쐬���܂���
		};
	}
	
	/* --------------------------------------------------------------------- */
	///
	/// OpenKind
	///
	/// <summary>
	/// ���k�E�𓀏����̏I����A�o�͐�t�H���_���J�����ǂ������`���܂��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	namespace OpenKind {
		enum {
			Skip                = 0x00, // �J�����ɏI�����܂��B
			Open                = 0x01, // �t�H���_���J���܂��B
			SkipDesktop         = 0x02  // �o�͐悪�f�X�N�g�b�v�̏ꍇ�A�J�����ɏI�����܂��B
		};
	}
	
	/* --------------------------------------------------------------------- */
	///
	/// EncodingKind
	///
	/// <summary>
	/// �����G���R�[�f�B���O��ϊ�����ۂ́A���̕����G���R�[�f�B���O�̎w��
	/// ���@���`���܂��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	namespace EncodingKind {
		enum {
			Auto                = 0x00,
			Shift_JIS           = 0x01,
			EUC_JP              = 0x02,
			UTF8                = 0x03,
			Unicode             = 0x04
		};
	}
	
	/* --------------------------------------------------------------------- */
	///
	/// ContextKind
	///
	/// <summary>
	/// �R���e�L�X�g���j���[�ɕ\�����鍀�ڂ��`���܂��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	namespace ContextKind {
		enum {
			Compression                 = 0x00000001, // ���k
			Decompression               = 0x00000002, // ��
			Setting                     = 0x00000004, // �ݒ�
			Mail                        = 0x00000008, // ���k���ă��[�����M
			DecompressionHere           = 0x00000010, // �����ɉ�
			DecompressionDesktop        = 0x00000020, // �f�X�N�g�b�v�ɉ�
			DecompressionRuntime        = 0x00000040, // �𓀐���w��
			DecompressionMyDocuments    = 0x00000080, // �}�C�h�L�������g�ɉ�
			CompressionZip              = 0x00000100, // zip
			CompressionZipPassword      = 0x00000200, // zip (�p�X���[�h)
			Compression7z               = 0x00000400, // 7z
			CompressionBzip2            = 0x00000800, // bzip2
			CompressionGzip             = 0x00001000, // gzip
			CompressionDetail           = 0x00002000, // �ڍאݒ�
			CompressionSelfExecutable   = 0x00004000, // exe
			MailZip                     = 0x00010000, // zip
			MailZipPassword             = 0x00020000, // zip (�p�X���[�h) 
			Mail7z                      = 0x00040000, // 7z
			MailBzip2                   = 0x00080000, // bzip2
			MailGzip                    = 0x00100000, // gzip
			MailDetail                  = 0x00200000, // �ڍאݒ�
			MailSelfExecutable          = 0x00400000, // exe
		};
	}
	
	/* --------------------------------------------------------------------- */
	///
	/// GetCompressionContexts
	///
	/// <summary>
	/// ���k�Ɋւ���R���e�L�X�g���j���[�ɕ\�����鍀�ڂ��擾���܂��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	inline void GetCompressionContexts(std::map<int, Context>& dest, const string_type& install) {
		string_type program = install + _T("\\") + CUBEICE_NAME;
		dest[ContextKind::CompressionZip] = Context(_T("zip"), program, _T("/c:zip"), _T(""));
		dest[ContextKind::CompressionZipPassword] = Context(_T("zip (�p�X���[�h)"), program, _T("/c:zip /p"), _T(""));
		dest[ContextKind::Compression7z] = Context(_T("7z"), program, _T("/c:7z"), _T(""));
		dest[ContextKind::CompressionBzip2] = Context(_T("bzip2"), program, _T("/c:bzip2"), _T(""));
		dest[ContextKind::CompressionGzip] = Context(_T("gzip"), program, _T("/c:gzip"), _T(""));
		dest[ContextKind::CompressionSelfExecutable] = Context(_T("exe"), program, _T("/c:exe"), _T(""));
		dest[ContextKind::CompressionDetail] = Context(_T("�ڍאݒ�"), program, _T("/c:detail"), _T(""));
	}
	
	/* --------------------------------------------------------------------- */
	///
	/// GetDecompressionContexts
	///
	/// <summary>
	/// �𓀂Ɋւ���R���e�L�X�g���j���[�ɕ\�����鍀�ڂ��擾���܂��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	inline void GetDecompressionContexts(std::map<int, Context>& dest, const string_type& install) {
		string_type program = install + _T("\\") + CUBEICE_NAME;
		dest[ContextKind::DecompressionHere] = Context(_T("�����ɉ�"), program, _T("/x /o:source"), _T(""));
		dest[ContextKind::DecompressionDesktop] = Context(_T("�f�X�N�g�b�v�ɉ�"), program, _T("/x /o:desktop"), _T(""));
		dest[ContextKind::DecompressionRuntime] = Context(_T("�ꏊ���w�肵�ĉ�"), program, _T("/x /o:runtime"), _T(""));
		dest[ContextKind::DecompressionMyDocuments] = Context(_T("�}�C�h�L�������g�ɉ�"), program, _T("/x /o:mydocuments"), _T(""));
	}
	
	/* --------------------------------------------------------------------- */
	///
	/// GetMailContexts
	///
	/// <summary>
	/// ���k���ă��[�����M�Ɋւ���R���e�L�X�g���j���[�ɕ\�����鍀�ڂ�
	/// �擾���܂��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	inline void GetMailContexts(std::map<int, Context>& dest, const string_type& install) {
		string_type program = install + _T("\\") + CUBEICE_NAME;
		dest[ContextKind::MailZip] = Context(_T("zip"), program, _T("/c:zip /m"), _T(""));
		dest[ContextKind::MailZipPassword] = Context(_T("zip (�p�X���[�h)"), program, _T("/c:zip /p /m"), _T(""));
		dest[ContextKind::Mail7z] = Context(_T("7z"), program, _T("/c:7z /m"), _T(""));
		dest[ContextKind::MailBzip2] = Context(_T("bzip2"), program, _T("/c:bzip2 /m"), _T(""));
		dest[ContextKind::MailGzip] = Context(_T("gzip"), program, _T("/c:gzip /m"), _T(""));
		dest[ContextKind::MailSelfExecutable] = Context(_T("exe"), program, _T("/c:exe /m"), _T(""));
		dest[ContextKind::MailDetail] = Context(_T("�ڍאݒ�"), program, _T("/c:detail /m"), _T(""));
	}
	
	/* --------------------------------------------------------------------- */
	///
	/// GetContexts
	///
	/// <summary>
	/// �r���g�C���^�̃R���e�L�X�g���j���[��W�J���܂��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	inline void ConvertContext(int src, ContextContainer& dest, const string_type& install) {
		string_type program = install + _T("\\") + CUBEICE_NAME;
		
		if ((src & ContextKind::Compression) != 0) {
			Context cx(_T("���k"), program + _T(",1"));
			std::map<int, Context> v;
			GetCompressionContexts(v, install);
			for (std::map<int, Context>::iterator pos = v.begin(); pos != v.end(); ++pos) {
				if ((src & pos->first) != 0) cx.Children().push_back(pos->second);
			}
			if (!cx.Children().empty()) dest.push_back(cx);
		}
		
		if ((src & ContextKind::Decompression) != 0) {
			Context cx(_T("��"), program + _T(",2"));
			std::map<int, Context> v;
			GetDecompressionContexts(v, install);
			for (std::map<int, Context>::iterator pos = v.begin(); pos != v.end(); ++pos) {
				if ((src & pos->first) != 0) cx.Children().push_back(pos->second);
			}
			if (!cx.Children().empty()) dest.push_back(cx);
		}
		
		if ((src & ContextKind::Mail) != 0) {
			Context cx(_T("���k���ă��[�����M"), program + _T(",1"));
			std::map<int, Context> v;
			GetMailContexts(v, install);
			for (std::map<int, Context>::iterator pos = v.begin(); pos != v.end(); ++pos) {
				if ((src & pos->first) != 0) cx.Children().push_back(pos->second);
			}
			if (!cx.Children().empty()) dest.push_back(cx);
		}
		
		if ((src & ContextKind::Setting) != 0) {
			dest.push_back(Context(_T("�ݒ�"), install + _T("\\") + CUBEICE_SETTING_NAME, _T(""), program + _T(",0")));
		}
	}
} // namespace CubeICE

#endif // CUBEICE_USER_SETTING_UTILITY_H
