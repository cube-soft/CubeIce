/* ------------------------------------------------------------------------- */
/*
 *  user-setting.h
 *
 *  Copyright (c) 2010 CubeSoft.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    - No names of its contributors may be used to endorse or promote
 *      products derived from this software without specific prior written
 *      permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  Last-modified: Mon 13 Dec 2010 12:31:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CUBEICE_USER_SETTING_H
#define CUBEICE_USER_SETTING_H

#include <cstdlib>
#include <string>
#include <tchar.h>
#include <winreg.h>

/* ------------------------------------------------------------------------- */
//  �o�͐�t�H���_�Ɋ֘A����t���O
/* ------------------------------------------------------------------------- */
#define OUTPUT_RUNTIME      0x00    // ���s���Ɏw�肷��
#define OUTPUT_SPECIFIC     0x01    // �w�肵���t�H���_
#define OUTPUT_SOURCE       0x02    // ���̃t�@�C���Ɠ����t�H���_

/* ------------------------------------------------------------------------- */
//  �㏑���Ɋ֘A����t���O
/* ------------------------------------------------------------------------- */
#define OVERWRITE_FORCE     0x00    // �����I�ɏ㏑��
#define OVERWRITE_NOTIFY    0x01    // �㏑���̊m�F
#define OVERWRITE_NEWER     0x02    // �X�V���������̃t�@�C�����V�����ꍇ�㏑��

/* ------------------------------------------------------------------------- */
//  ���W�X�g���Ɋւ�����
/* ------------------------------------------------------------------------- */
#define CUBEICE_REG_ROOT            "Software\\CubeSoft\\CubeICE"
#define CUBEICE_REG_COMPRESS        "Compress"
#define CUBEICE_REG_DECOMPRESS      "Decompress"
#define CUBEICE_REG_FLAGS           "Flags"
#define CUBEICE_REG_OUTPUT_FLAG     "OutputFlag"
#define CUBEICE_REG_OUTPUT_PATH     "OutputPath"
#define CUBEICE_REG_CREATE_FOLDER   "CreateFolder"
#define CUBEICE_REG_CONV_CHARSET    "ConvCharset"
#define CUBEICE_REG_FILTER          "Filter"
#define CUBEICE_REG_POSTOPEN        "PostOpen"
#define CUBEICE_REG_CONTEXT         "ContextFlags"

namespace cubeice {
	/* --------------------------------------------------------------------- */
	//  user_setting_property
	/* --------------------------------------------------------------------- */
	class user_setting_property {
	public:
		typedef std::size_t size_type;
		typedef TCHAR char_type;
		typedef std::basic_string<char_type> string_type;
		
		/* ----------------------------------------------------------------- */
		//  constructor
		/* ----------------------------------------------------------------- */
		user_setting_property() :
			root_(), flags_(0), output_condition_(0), output_path_(),
			create_folder_(false), conv_charset_(false), filter_(false), postopen_(false) {}
		
		explicit user_setting_property(const string_type& root) :
			root_(), flags_(0), output_condition_(0), output_path_(),
			create_folder_(false), conv_charset_(false), filter_(false), postopen_(false) {
			this->load(root);	
		}
		
		/* ----------------------------------------------------------------- */
		//  load
		/* ----------------------------------------------------------------- */
		void load(const string_type& root) {
			HKEY hkResult;		// �L�[�̃n���h��
			DWORD dwDisposition;	// �������ʂ��󂯎��
			LONG lResult;		// �֐��̖߂�l���i�[����
			lResult = RegOpenKeyEx(HKEY_CURRENT_CONFIG, root_.c_str(), 0, KEY_ALL_ACCESS, NULL, &hkResult);
			if (!lResult) {
				DWORD dwType;		// �l�̎�ނ��󂯎��
				DWORD dwSize;		// �f�[�^�̃T�C�Y���󂯎��
			 
				// flags
				RegQueryValueEx(hkResult, CUBEICE_REG_FLAGS, NULL, &dwType, NULL, &dwSize);
				RegQueryValueEx(hkResult, CUBEICE_REG_FLAGS, NULL, &dwType, (LPBYTE)&flags_, &dwSize);
				// output_flag
				RegQueryValueEx(hkResult, CUBEICE_REG_OUTPUT_FLAG, NULL, &dwType, NULL, &dwSize);
				RegQueryValueEx(hkResult, CUBEICE_REG_OUTPUT_FLAG, NULL, &dwType, (LPBYTE)&output_condition_, &dwSize);
				// output_path
				RegQueryValueEx(hkResult, CUBEICE_REG_OUTPUT_PATH, NULL, &dwType, NULL, &dwSize);
				RegQueryValueEx(hkResult, CUBEICE_REG_OUTPUT_PATH, NULL, &dwType, (LPBYTE)&output_path_.c_str, &dwSize);
				// create_folder
				DWORD dw_create_folder;
				RegQueryValueEx(hkResult, CUBEICE_REG_CREATE_FOLDER, NULL, &dwType, NULL, &dwSize);
				RegQueryValueEx(hkResult, CUBEICE_REG_CREATE_FOLDER, NULL, &dwType, (LPBYTE)&dw_create_folder, &dwSize);
				create_folder_ = (dw_create_folder != 0) ? TRUE : FALSE;
				// conv_charset
				DWORD dw_conv_charset;
				RegQueryValueEx(hkResult, CUBEICE_REG_CONV_CHARSET, NULL, &dwType, NULL, &dwSize);
				RegQueryValueEx(hkResult, CUBEICE_REG_CONV_CHARSET, NULL, &dwType, (LPBYTE)&dw_conv_charset, &dwSize);
				conv_charset_ = (dw_conv_charset != 0) ? TRUE : FALSE;
				// filter
				DWORD dw_filter;
				RegQueryValueEx(hkResult, CUBEICE_REG_FILTER, NULL, &dwType, NULL, &dwSize);
				RegQueryValueEx(hkResult, CUBEICE_REG_FILTER, NULL, &dwType, (LPBYTE)&dw_filter, &dwSize);
				filter_ = (dw_filter != 0) ? TRUE : FALSE;
				// postopen
				DWORD dw_postopen;
				RegQueryValueEx(hkResult, CUBEICE_REG_POSTOPEN, NULL, &dwType, NULL, &dwSize);
				RegQueryValueEx(hkResult, CUBEICE_REG_POSTOPEN, NULL, &dwType, (LPBYTE)&dw_postopen, &dwSize);
				postopen_ = (dw_postopen != 0) ? TRUE : FALSE;
			}
		}
		
		/* ----------------------------------------------------------------- */
		//  save
		/* ----------------------------------------------------------------- */
		void save() {
			HKEY hkResult;		// �L�[�̃n���h��
			DWORD dwDisposition;	// �������ʂ��󂯎��
			LONG lResult;		// �֐��̖߂�l���i�[����
			lResult = RegCreateKeyEx(HKEY_CURRENT_CONFIG, root_.c_str(), 0, "", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkResult, &dwDisposition);
			if (!lResult) {
				RegSaveKeyEx(hkResult, CUBEICE_REG_FLAGS, 0, REG_DWORD, (CONST BYTE*)&flags_, sizeof(flags));
				// OUTPUT_FLAG == output_condition ?? 
				RegSaveKeyEx(hkResult, CUBEICE_REG_OUTPUT_FLAG, 0, REG_DWORD, (CONST BYTE*)&output_condition_, sizeof(output_condition_));
				RegSaveKeyEx(hkResult, CUBEICE_REG_OUTPUT_PATH, 0, REG_SZ, output_path_.c_str, output_path_.length() + 1);
				DWORD dw_create_folder = (create_folder_) ? 1 : 0;
				RegSaveKeyEx(hkResult, CUBEICE_REG_CREATE_FOLDER,0,REG_DWORD, (CONST BYTE*)&dw_create_folder, sizeof(dw_create_folder));
				DWORD dw_conv_charset  = (conv_charset_) ? 1 : 0;
				RegSaveKeyEx(hkResult, CUBEICE_REG_CONV_CHARSET, 0, REG_DWORD, (CONST BYTE*)&dw_conv_charset, sizeof(dw_conv_charset));
				DWORD dw_filter = (filter) ? 1 : 0;
				RegSaveKeyEx(hkResult, CUBEICE_REG_FILTER, 0, REG_DWORD, (CONST BYTE*)&dw_filter, sizeof(dw_filter));
				DWORD dw_postopen = (postopen_) ? 1 : 0;
				RegSaveKeyEx(hkResult, CUBEICE_REG_POSTOPEN, 0, REG_DWORD, (CONST BYTE*)&dw_postopen, sizeof(dw_postopen));
			}
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  flags
		 *
		 *  ���k�ɑΉ�����t�@�C���Q�D���k�̓R���e�L�X�g���j���[��
		 *  �u���k�v�̃T�u���ڂɕ\����������̂ɂȂ�C
		 *  �e�t���O (FLAG_ZIP, ..., FLAG_XZ) �̘a�W���ƂȂ�D�������C
		 *  7-zip �����k�ɑΉ����Ă�����̂̂݁D
		 */
		/* ----------------------------------------------------------------- */
		size_type& flags() { return flags_; }
		const size_type& flags() const { return flags_; }
		
		/* ----------------------------------------------------------------- */
		/*
		 *  output_condition
		 *
		 *  �o�͐�t�H���_�̐ݒ�DOUTPUT_SPECIFIC, OUTPUT_SOURCE, 
		 *  OUTPUT_RUNTIME �̂ǂꂩ�D
		 */
		/* ----------------------------------------------------------------- */
		size_type& output_condition() { return output_condition_; }
		const size_type& output_condition() const { return output_condition_; }
		
		/* ----------------------------------------------------------------- */
		/*
		 *  output_path
		 *
		 *  output_condition �� OUTPUT_SPECIFIC �̏ꍇ�Cpath_compress
		 *  �̒l���L���ƂȂ�D
		 */
		/* ----------------------------------------------------------------- */
		string_type& output_path() { return output_path_; }
		const string_type& output_path() const { return output_path_; }
		
		/* ----------------------------------------------------------------- */
		/*
		 *  create_folder
		 *
		 *  �t�H���_�������������邩�ǂ����D
		 */
		/* ----------------------------------------------------------------- */
		bool& create_folder() { return create_folder_; }
		const bool& create_folder() const { return create_folder_; }
		
		/* ----------------------------------------------------------------- */
		/*
		 *  conv_charset
		 *
		 *  �����R�[�h��ϊ����邩�ǂ����D
		 */
		/* ----------------------------------------------------------------- */
		bool& conv_charset() { return conv_charset_; }
		const bool& conv_charset() const { return conv_charset_; }
		
		/* ----------------------------------------------------------------- */
		/*
		 *  filter
		 *
		 *  �t�B���^�����O���s�����ǂ����D
		 */
		/* ----------------------------------------------------------------- */
		bool& filter() { return filter_; }
		const bool& filter() const { return filter_; }
		
		/* ----------------------------------------------------------------- */
		/*
		 *  postopen
		 *
		 *  ���s��C�ۑ���t�H���_���J�����ǂ����D
		 */
		/* ----------------------------------------------------------------- */
		bool& postopen() { return postopen_; }
		const bool& postopen() const { return postopen_; }
		
	private:
		string_type root_;
		size_type flags_;
		size_type output_condition_;
		string_type output_path_;
		bool create_folder_;
		bool conv_charset_;
		bool filter_;
		bool postopen_;
	};
	
	/* --------------------------------------------------------------------- */
	//  user_setting
	/* --------------------------------------------------------------------- */
	class user_setting {
	public:
		typedef user_setting_property property_type;
		typedef property_type::size_type size_type;
		typedef property_type::char_type char_type;
		typedef property_type::string_type string_type;
		
		/* ----------------------------------------------------------------- */
		//  constructor
		/* ----------------------------------------------------------------- */
		user_setting() :
			comp_(), decomp_(), flags_(0) {}
		
		/* ----------------------------------------------------------------- */
		//  load
		/* ----------------------------------------------------------------- */
		void load() {
			comp_.load();
			decomp_.load();
			// load context
			HKEY hkResult;		// �L�[�̃n���h��
			DWORD dwDisposition;	// �������ʂ��󂯎��
			LONG lResult;		// �֐��̖߂�l���i�[����
			lResult = RegOpenKeyEx(HKEY_CURRENT_CONFIG, root_.c_str(), 0, KEY_ALL_ACCESS, NULL, &hkResult);
			if (!lResult) {
				DWORD dwType;		// �l�̎�ނ��󂯎��
				DWORD dwSize;		// �f�[�^�̃T�C�Y���󂯎��
				// flags
				RegQueryValueEx(hkResult, CUBEICE_REG_CONTEXT, NULL, &dwType, NULL, &dwSize);
				RegQueryValueEx(hkResult, CUBEICE_REG_CONTEXT, NULL, &dwType, (LPBYTE)&flags_, &dwSize);
			}
		}
		
		/* ----------------------------------------------------------------- */
		//  save
		/* ----------------------------------------------------------------- */
		void save() {
			comp_.save();
			decomp_.save();
			// save context
			HKEY hkResult;		// �L�[�̃n���h��
			DWORD dwDisposition;	// �������ʂ��󂯎��
			LONG lResult;		// �֐��̖߂�l���i�[����
			lResult = RegCreateKeyEx(HKEY_CURRENT_CONFIG, root_.c_str(), 0, "", REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkResult, &dwDisposition);
			if (!lResult) {
				RegSaveKeyEx(hkResult, CUBEICE_REG_CONTEXT, 0, REG_DWORD, (CONST BYTE*)&flags_, sizeof(flags));
			}
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  compression
		 *
		 *  ���k�Ɋւ���ݒ�D
		 */
		/* ----------------------------------------------------------------- */
		property_type& compression() { return comp_; }
		const property_type& compression() const { return comp_; }
		
		/* ----------------------------------------------------------------- */
		/*
		 *  decompression
		 *
		 *  �𓀂Ɋւ���ݒ�D
		 */
		/* ----------------------------------------------------------------- */
		property_type& decompression() { return decomp_; }
		const property_type& decompression() const { return decomp_; }
		
		/* ----------------------------------------------------------------- */
		/*
		 *  contexts
		 *
		 *  �R���e�L�X�g���j���[�ɕ\����������́D
		 */
		/* ----------------------------------------------------------------- */
		size_type& context_flags() { return flags_; }
		const size_type& context_flags() const { return flags_; }
		
	private:
		property_type comp_;
		property_type decomp_;
		size_type flags_;
	};
}

#endif // CUBEICE_USER_SETTING_H
