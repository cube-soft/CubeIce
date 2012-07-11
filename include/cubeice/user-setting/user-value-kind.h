/* ------------------------------------------------------------------------- */
/*
 *  cubeice/user-setting/config.h
 *
 *  Copyright (c) 2010 CubeSoft Inc.
 *
 *  Distributed under the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 */
/* ------------------------------------------------------------------------- */
#ifndef CUBEICE_USER_SETTING_CONFIG_H
#define CUBEICE_USER_SETTING_CONFIG_H

namespace CubeICE {
	/* --------------------------------------------------------------------- */
	/*!
	 *  OutputKind
	 *
	 *  �o�͐�̌�����@���`���܂��B
	 */
	/* --------------------------------------------------------------------- */
	namespace OutputKind {
		enum {
			Runtime             = 0x00, // ���s���Ɏw�肵�܂��B
			Specific            = 0x01, // �\�ߌ��߂�ꂽ�t�H���_���o�͐�Ƃ��܂��B
			SameAsSource        = 0x02  // ���͌��Ɠ����t�H���_���o�͐�Ƃ��܂��B
		};
	}
	
	/* --------------------------------------------------------------------- */
	/*!
	 *  OverwriteKind
	 *
	 *  �����̃t�@�C�������݂����ꍇ�̏㏑�����@���`���܂��B
	 */
	/* --------------------------------------------------------------------- */
	namespace OverwriteKind {
		enum {
			Force               = 0x00, // �����I�ɏ㏑�����܂��B
			Confirm             = 0x01, // �㏑���̊m�F���s���܂��B
			ConfirmOlder        = 0x02  // �X�V�������V�����ꍇ�͋����I�ɏ㏑�����܂��B
		};
	}
	
	/* --------------------------------------------------------------------- */
	/*!
	 *  CreateFolderKind
	 *
	 *  �𓀎��Ƀt�H���_���쐬���邩�ǂ������`���܂��B
	 */
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
	/*!
	 *  OpenKind
	 *
	 *  ���k�E�𓀏����̏I����A�o�͐�t�H���_���J�����ǂ������`���܂��B
	 */
	/* --------------------------------------------------------------------- */
	namespace OpenKind {
		enum {
			Skip                = 0x00, // �J�����ɏI�����܂��B
			Open                = 0x01, // �t�H���_���J���܂��B
			SkipDesktop         = 0x02  // �o�͐悪�f�X�N�g�b�v�̏ꍇ�A�J�����ɏI�����܂��B
		};
	}
	
	/* --------------------------------------------------------------------- */
	/*!
	 *  EncodingKind
	 *
	 *  �����G���R�[�f�B���O��ϊ�����ۂ́A���̕����G���R�[�f�B���O�̎w��
	 *  ���@���`���܂��B
	 */
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
	/*!
	 *  ContextKind
	 *
	 *  �R���e�L�X�g���j���[�ɕ\�����鍀�ڂ��`���܂��B
	 */
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
			CompressionSevenZip         = 0x00000400, // 7z
			CompressionBzip2            = 0x00000800, // bzip2
			CompressionGzip             = 0x00001000, // gzip
			CompressionDetail           = 0x00002000, // �ڍאݒ�
			CompressionSelfExecutable   = 0x00004000, // exe
			MailZip                     = 0x00010000, // zip
			MailZipPassword             = 0x00020000, // zip (�p�X���[�h) 
			MailSevenZip                = 0x00040000, // 7z
			MailBzip2                   = 0x00080000, // bzip2
			MailGzip                    = 0x00100000, // gzip
			MailDetail                  = 0x00200000, // �ڍאݒ�
			MailSelfExecutable          = 0x00400000  // exe
		};
	}
	
	/* --------------------------------------------------------------------- */
	/*!
	 *  IconKind
	 *
	 *  �g�p�ł���A�C�R���̎�ނ��`���܂��B
	 */
	/* --------------------------------------------------------------------- */
	namespace IconKind {
		enum {
			None                = 0x00,              // �A�C�R����\�����Ȃ�
			Logo                = IDB_LOGO,          // CubeICE �̃��S�p�A�C�R��
			Compression         = IDB_COMPRESSION,   // ���k�p�A�C�R��
			Decompression       = IDB_DECOMPRESSION, // �𓀗p�A�C�R��
			File                = IDB_FILE,          // CubeICE �̈��k�t�@�C���p�A�C�R��
			Folder              = IDB_FOLDER         // �t�H���_�A�C�R��
		};
	}
	
} // namespace CubeICE

#endif // CUBEICE_USER_SETTING_USER_VALUE_KIND_H
