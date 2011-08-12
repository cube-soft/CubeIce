// -*- coding: shift-jis -*-
/* ------------------------------------------------------------------------- */
/*
 *  psdotnet/forms/file-dialog.h
 *
 *  Copyright (c) 2011, clown.
 *
 *  Distributed under the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 */
/* ------------------------------------------------------------------------- */
#ifndef PSDOTNET_FORMS_FILE_DIALOG_H
#define PSDOTNET_FORMS_FILE_DIALOG_H

#include <psdotnet/config.h>
#include <algorithm>
#include <string>
#include <vector>
#include <windows.h>
#include <tchar.h>
#include <boost/foreach.hpp>
#include <boost/utility.hpp>
#include <psdotnet/forms/dialog-result.h>

namespace PsdotNet {
	namespace Forms {
		/* ----------------------------------------------------------------- */
		/*
		 *  FileDialog
		 *
		 *  OPENFILENAME �\���̂��g�p���ĕ\������_�C�A���O�̊��N���X�D
		 *  ���̃N���X�𒼐ڎg�p���鎖�͂ł��Ȃ��D����� FileDialog ��
		 *  �h���N���X�ł��� OpenFileDialog �N���X�C�܂��� SaveFileDialog
		 *  �N���X���g�p���鎖�D
		 */
		/* ----------------------------------------------------------------- */
		class FileDialog {
		public:
			typedef OPENFILENAME data_type;
			typedef DWORD size_type;
			typedef TCHAR char_type;
			typedef std::basic_string<char_type> string_type;
			typedef std::pair<string_type, string_type> string_pair;
			typedef std::vector<string_type> string_array;
			typedef std::vector<string_pair> string_pair_array;
			
		public:
			/* ------------------------------------------------------------- */
			//  constructor
			/* ------------------------------------------------------------- */
			FileDialog() : data_(), paths_(), initdir_(), title_(), filters_() {
				data_.Flags |= OFN_EXPLORER;
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  ShowDialog
			 *
			 *  �_�C�A���O�{�b�N�X��\������DOK �{�^���������ꂽ�ꍇ�ɂ�
			 *  DialogResult::OK, ����ȊO�̃{�^���������ꂽ�ꍇ�ɂ�
			 *  DialogResult::Cancel ���Ԃ�D
			 */
			/* ------------------------------------------------------------- */
			virtual DialogResult ShowDialog(HWND owner) = 0;
			
			/* ------------------------------------------------------------- */
			/*
			 *  FileName
			 *
			 *  �_�C�A���O�{�b�N�X�őI�����ꂽ�t�@�C�������擾����D
			 *  FileName() �����o�֐��́C���ۂɂ́C�t�@�C�������܂ރp�X��
			 *  �Ԃ���� (System.Windows.Forms.FileDialog.FileName)�D
			 */
			/* ------------------------------------------------------------- */
			//const string_type& FileName() const { return paths_.at(0); }
			string_type FileName() const { return !paths_.empty() ? paths_.at(0) : string_type(); }
			
			/* ------------------------------------------------------------- */
			/*
			 *  FileNames
			 *
			 *  �_�C�A���O�{�b�N�X�őI�����ꂽ���ׂẴt�@�C���̖��O��
			 *  �擾����D
			 */
			/* ------------------------------------------------------------- */
			const string_array& FileNames() const { return paths_; }
			
			/* ------------------------------------------------------------- */
			/*
			 *  DefaultExt
			 *
			 *  �t�@�C���̎�ނł��ׂẴt�@�C�� (*.*) ���w�肳��C����
			 *  �g���q�Ȃ��̃t�@�C���������͂��ꂽ�ꍇ�Ɏ����I�ɕt������
			 *  �g���q���擾����D
			 */
			/* ------------------------------------------------------------- */
			const string_type& DefaultExt() const { return ext_; }
			
			/* ------------------------------------------------------------- */
			/*
			 *  DefaultExt
			 *
			 *  �t�@�C���̎�ނł��ׂẴt�@�C�� (*.*) ���w�肳��C����
			 *  �g���q�Ȃ��̃t�@�C���������͂��ꂽ�ꍇ�Ɏ����I�ɕt������
			 *  �g���q��ݒ肷��D�ݒ肷��g���q�́C". (�h�b�g)" �Ȃ���
			 *  �w�肷��D
			 */
			/* ------------------------------------------------------------- */
			void DefaultExt(const string_type& ext) {
				ext_ = ext;
				data_.lpstrDefExt = ext_.c_str();
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  AddExtension
			 *
			 *  ���[�U���g���q�Ȃ��̃t�@�C��������͂����ۂɁC�����I��
			 *  �g���q��t�����邩�ǂ����̐^�U�l���擾����D
			 */
			/* ------------------------------------------------------------- */
			bool AddExtension() const { return data_.lpstrDefExt != NULL; }
			
			/* ------------------------------------------------------------- */
			/*
			 *  AddExtension
			 *
			 *  ���[�U���g���q�Ȃ��̃t�@�C��������͂����ۂɁC�����I��
			 *  �g���q��t�����邩�ǂ����̐^�U�l��ݒ肷��D
			 */
			/* ------------------------------------------------------------- */
			void AddExtension(bool enable) {
				if (enable) data_.lpstrDefExt = ext_.c_str();
				else data_.lpstrDefExt = NULL;
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  InitialDirectory
			 *
			 *  �_�C�A���O�{�b�N�X�̏����f�B���N�g�����擾����D
			 */
			/* ------------------------------------------------------------- */
			const string_type& InitialDirectory() const { return initdir_; }
			
			/* ------------------------------------------------------------- */
			/*
			 *  InitialDirectory
			 *
			 *  �_�C�A���O�{�b�N�X�̏����f�B���N�g����ݒ肷��D������
			 *  NativeData().lpstrInitialDir �̒l���X�V�����D
			 */
			/* ------------------------------------------------------------- */
			void InitialDirectory(const string_type& path) {
				initdir_ = path;
				data_.lpstrInitialDir = initdir_.c_str();
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  Title
			 *
			 *  �_�C�A���O�{�b�N�X�̃^�C�g���o�[�ɕ\������镶������擾
			 *  ����D
			 */
			/* ------------------------------------------------------------- */
			const string_type Title() const { return title_; }
			
			/* ------------------------------------------------------------- */
			/*
			 *  Title
			 *
			 *  �_�C�A���O�{�b�N�X�̃^�C�g���o�[�ɕ\������镶�����ݒ�
			 *  ����D������ NativeData().lpstrTitle �̒l���X�V�����D
			 */
			/* ------------------------------------------------------------- */
			void Title(const string_type& str) {
				title_ = str;
				data_.lpstrTitle = title_.c_str();
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  Filters
			 *
			 *  �t�@�C����ރR���{�{�b�N�X�ɕ\��������t�B���^�[�̈ꗗ��
			 *  �擾�܂��͐ݒ肷��D�e�v�f�� (�����C�g���q) �ƌ���������
			 *  �y�A�ɂȂ��Ă���D
			 *
			 *  NOTE: System.Windows.Forms.FileDialog �ł́C
			 *  "description1|ext1|description2|ext2|..."
			 *  �̂悤�� "|" ���Z�p���[�^�Ƃ���������Ŏ擾�E�ݒ肷��
			 *  �悤�ɂȂ��Ă��邪�CPsdotNet �ł́Cstd::pair<string, string>
			 *  �̔z�� (vector) �ň����悤�ɂ���D
			 */
			/* ------------------------------------------------------------- */
			const string_pair_array& Filters() const { return filters_; }
			string_pair_array& Filters() { return filters_; }
			
			/* ------------------------------------------------------------- */
			/*
			 *  AddFilter
			 *
			 *  �t�@�C����ރR���{�{�b�N�X�ɕ\��������t�B���^�[��ǉ�����D
			 *  filters().push_back(description, extension) �Ɠ������D
			 */
			/* ------------------------------------------------------------- */
			void AddFilter(const string_type& description, const string_type& extension) {
				filters_.push_back(string_pair(description, extension));
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  FilterIndex
			 *
			 *  �t�@�C����ރR���{�{�b�N�X�ŁC�\��������t�B���^�[��
			 *  �C���f�b�N�X���擾����D
			 */
			/* ------------------------------------------------------------- */
			const size_type& FilterIndex() const { return data_.nFilterIndex; }
			
			/* ------------------------------------------------------------- */
			/*
			 *  FilterIndex
			 *
			 *  �t�@�C����ރR���{�{�b�N�X�ŁC�\��������t�B���^�[��
			 *  �C���f�b�N�X��ݒ肷��D
			 */
			/* ------------------------------------------------------------- */
			void FilterIndex(const size_type& n) { data_.nFilterIndex = n; }
			
			/* ------------------------------------------------------------- */
			/*
			 *  CheckFileExists
			 *
			 *  ���[�U�[�����݂��Ȃ��t�@�C����I�����悤�Ƃ����Ƃ��C
			 *  �x���_�C�A���O��\�����đI��s�\�ɂ��邩�ǂ����̐^�U�l��
			 *  �擾����D
			 *
			 *  NOTE: System.Windows.Forms.FileDialog �� CheckFileExists()
			 *  �̑��ɁCCheckPathExists() �ƌ����v���p�e�B����`����Ă��邪�C
			 *  OFN_FILEMUSTEXIST �� OFN_PATHMUSTEXIST �t���O���܂ނ���
			 *  CheckPathExists �v���p�e�B�ɑ������郁���o�֐��͗p�ӂ��Ȃ�
			 *  ���Ƃ���D
			 */
			/* ------------------------------------------------------------- */
			bool CheckFileExists() const { return (data_.Flags & OFN_FILEMUSTEXIST) != 0; }
			
			/* ------------------------------------------------------------- */
			/*
			 *  CheckFileExists
			 *
			 *  ���[�U�[�����݂��Ȃ��t�@�C����I�����悤�Ƃ����Ƃ��C
			 *  �x���_�C�A���O��\�����đI��s�\�ɂ��邩�ǂ����̐^�U�l��
			 *  �ݒ肷��D
			 */
			/* ------------------------------------------------------------- */
			void CheckFileExists(bool enable) {
				if (enable) data_.Flags |= OFN_FILEMUSTEXIST;
				else data_.Flags &= ~OFN_FILEMUSTEXIST;
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  DereferenceLinks
			 *
			 *  �_�C�A���O�{�b�N�X���C�V���[�g�J�b�g�ŎQ�Ƃ��ꂽ�t�@�C����
			 *  �ꏊ��Ԃ����ǂ����C�܂��̓V���[�g�J�b�g�̏ꏊ (.lnk) ��
			 *  �Ԃ����ǂ������w�肷��l���擾����D
			 */
			/* ------------------------------------------------------------- */
			bool DereferenceLinks() const { return (data_.Flags & OFN_NODEREFERENCELINKS) == 0; }
			
			/* ------------------------------------------------------------- */
			/*
			 *  DereferenceLinks
			 *
			 *  �_�C�A���O�{�b�N�X���C�V���[�g�J�b�g�ŎQ�Ƃ��ꂽ�t�@�C����
			 *  �ꏊ��Ԃ����ǂ����C�܂��̓V���[�g�J�b�g�̏ꏊ (.lnk) ��
			 *  �Ԃ����ǂ������w�肷��l��ݒ肷��D
			 */
			/* ------------------------------------------------------------- */
			void DereferenceLinks(bool enable) {
				if (enable) data_.Flags &= ~OFN_NODEREFERENCELINKS;
				else data_.Flags |= OFN_NODEREFERENCELINKS;
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  RestoreDirectory
			 *
			 *  �_�C�A���O�{�b�N�X�����O�ɁC���݂̃f�B���N�g����
			 *  �������邩�ǂ����������l���擾����D
			 */
			/* ------------------------------------------------------------- */
			bool RestoreDirectory() const { return (data_.Flags & OFN_NOCHANGEDIR) != 0; }
			
			/* ------------------------------------------------------------- */
			/*
			 *  RestoreDirectory
			 *
			 *  �_�C�A���O�{�b�N�X�����O�ɁC���݂̃f�B���N�g����
			 *  �������邩�ǂ����������l���擾����D
			 */
			/* ------------------------------------------------------------- */
			void RestoreDirectory(bool enable) {
				if (enable) data_.Flags |= OFN_NOCHANGEDIR;
				else data_.Flags &= ~OFN_NOCHANGEDIR;
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  ValidateNames
			 *
			 *  �_�C�A���O�{�b�N�X���L���� Win32 �t�@�C�����������󂯓����
			 *  ���ǂ����������l���擾����D
			 */
			/* ------------------------------------------------------------- */
			bool ValidateNames() const { return (data_.Flags & OFN_NOVALIDATE) == 0; }
			
			/* ------------------------------------------------------------- */
			/*
			 *  ValidateNames
			 *
			 *  �_�C�A���O�{�b�N�X���L���� Win32 �t�@�C�����������󂯓����
			 *  ���ǂ����������l���擾����D
			 */
			/* ------------------------------------------------------------- */
			void ValidateNames(bool enable) {
				if (enable) data_.Flags &= ~OFN_NOVALIDATE;
				else data_.Flags |= OFN_NOVALIDATE;
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  ShowHelp
			 *
			 *  �_�C�A���O�{�b�N�X�� [�w���v] �{�^����\�����邩�ǂ�����
			 *  �����l���擾����D
			 */
			/* ------------------------------------------------------------- */
			bool ShowHelp() const { return (data_.Flags & OFN_SHOWHELP) != 0; }
			
			/* ------------------------------------------------------------- */
			/*
			 *  ShowHelp
			 *
			 *  �_�C�A���O�{�b�N�X�� [�w���v] �{�^����\�����邩�ǂ�����
			 *  �����l��ݒ肷��D
			 *  NOTE: OFN_SHOWHELP �t���O��L���ɂ����ꍇ�C�e�E�B���h�E��
			 *  �w�肳��Ă��Ȃ���΂Ȃ�Ȃ��D
			 */
			/* ------------------------------------------------------------- */
			void ShowHelp(bool enable) {
				if (enable) data_.Flags |= OFN_SHOWHELP;
				else data_.Flags &= ~OFN_SHOWHELP;
			}
			
			/* ------------------------------------------------------------- */
			//  NativeData
			/* ------------------------------------------------------------- */
			const data_type& NativeData() const { return data_; }
			
			/* ------------------------------------------------------------- */
			//  NativeData
			/* ------------------------------------------------------------- */
			data_type& NativeData() { return data_; }
			
		protected:
			/* ------------------------------------------------------------- */
			//  destructor
			/* ------------------------------------------------------------- */
			~FileDialog() {}
			
			/* ------------------------------------------------------------- */
			/*
			 *  Split
			 *
			 *  �Z�p���[�^���k�������Ƃ��ĘA������Ă���z�� src �𕪊�����
			 *  ������̔z�� dest �Ɋi�[����D
			 */
			/* ------------------------------------------------------------- */
			template <class Container>
			bool Split(const Container& src, string_array& dest) {
				dest.clear();
				typename Container::const_iterator first = src.begin();
				while (first != src.end() && *first != 0) {
					typename Container::const_iterator last = std::find(first, src.end(), 0);
					string_type item;
					for (; first != last; ++first) item += *first;
					if (!item.empty()) dest.push_back(item);
					if (first != src.end()) ++first;
				}
				return !dest.empty();
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  Split
			 *
			 *  �Z�p���[�^���k�������Ƃ��ĘA������Ă���z�� src �𕪊�����
			 *  ������̔z�� dest �Ɋi�[����D
			 */
			/* ------------------------------------------------------------- */
			template <class T, std::size_t N>
			bool Split(const T (&src)[N], string_array& dest) {
				dest.clear();
				const T* first = src;
				while (first != src + N && *first != 0) {
					const T* last = std::find(first, src + N, 0);
					string_type item;
					for (; first != last; ++first) item += *first;
					if (!item.empty()) dest.push_back(item);
					if (first != src + N) ++first;
				}
				return !dest.empty();
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  Join
			 *
			 *  ������̔z�� src ���Z�p���[�^���k�������Ƃ��ĘA������
			 *  dest �Ɋi�[����Dsrc ����̏ꍇ�� false ��Ԃ��D
			 */
			/* ------------------------------------------------------------- */
			template <class Container>
			bool Join(const string_array& src, Container& dest) {
				dest.clear();
				BOOST_FOREACH(string_type item, src) {
					dest.insert(dest.end(), item.begin(), item.end());
					dest.push_back(0);
				}
				if (dest.empty()) return false;
				
				dest.push_back(0);
				return true;
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  Join
			 *
			 *  ������y�A�̔z��𕶎���̔z��ɓW�J���� join �����o�֐�
			 *  �����s����D
			 */
			/* ------------------------------------------------------------- */
			template <class Container>
			bool Join(const string_pair_array& src, Container& dest) {
				string_array tmp;
				BOOST_FOREACH(string_pair item, src) {
					tmp.push_back(item.first);
					tmp.push_back(item.second);
				}
				return this->Join(tmp, dest);
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  ClearInvalidData
			 *
			 *  show() �����o�֐����I������Ɩ����ɂȂ郁���o�ϐ��̒l��
			 *  �N���A����D
			 */
			/* ------------------------------------------------------------- */
			void ClearInvalidData() {
				data_.lpstrFilter = NULL;
				data_.lpstrFile = NULL;
				data_.nMaxFile = 0;
				data_.lpstrFileTitle = NULL;
				data_.nMaxFileTitle = 0;
			}
			
		protected:
			data_type data_;
			string_array paths_;
			string_type ext_;
			string_type initdir_;
			string_type title_;
			string_pair_array filters_;
		};
		
		/* ----------------------------------------------------------------- */
		/*
		 *  OpenFileDialog
		 *
		 *  �u�t�@�C�����J���v�_�C�A���O�{�b�N�X��\������N���X�D
		 *  .NetFramework ��System.Windows.Forms.OpenFileDialog ���Q�l��
		 *  �e�����o�֐����`���Ă���D
		 *  http://msdn.microsoft.com/ja-jp/library/y1kh29w3.aspx
		 */
		/* ----------------------------------------------------------------- */
		class OpenFileDialog : public FileDialog, boost::noncopyable {
		public:
			/* ------------------------------------------------------------- */
			//  constructor
			/* ------------------------------------------------------------- */
			OpenFileDialog() : FileDialog() {
				this->CheckFileExists(true);
			}
			
			/* ------------------------------------------------------------- */
			//  destructor
			/* ------------------------------------------------------------- */
			~OpenFileDialog() {}
			
			/* ------------------------------------------------------------- */
			//  ShowDialog
			/* ------------------------------------------------------------- */
			virtual DialogResult ShowDialog(HWND owner = NULL) {
				char_type path[PSDOTNET_MAX_PATH] = {};
				char_type filename[PSDOTNET_MAX_PATH] = {};
				
				std::vector<char_type> filter;
				char_type* pf = this->Join(filters_, filter) ? reinterpret_cast<char_type*>(&filter[0]) : NULL;
				
				data_.lStructSize     = sizeof(data_);
				data_.hwndOwner       = owner;
				data_.lpstrFilter     = pf;
				data_.lpstrFile       = path;
				data_.nMaxFile        = sizeof(path) / sizeof(char_type);
				data_.lpstrFileTitle  = filename;
				data_.nMaxFileTitle   = sizeof(filename) / sizeof(char_type);
				
				BOOL result = ::GetOpenFileName(&data_);
				
				if (result) {
					paths_.clear();
					if (this->Multiselect()) {
						string_array tmp;
						if (this->Split(path, tmp)) {
							paths_.clear();
							if (tmp.size() > 1) {
								string_type folder = tmp.at(0);
								for (size_type i = 1; i < tmp.size(); ++i) paths_.push_back(folder + _T('\\') + tmp.at(i));
							}
							else if (tmp.size() == 1) paths_.push_back(tmp.at(0));
						}
					}
					else paths_.push_back(path);
					
					// NOTE: System.Windows.Forms.FileDialog �� filename_ ��
					// ������v���p�e�B�������Ă��Ȃ����߁C���̒l�͎̂Ă�D
					// filename_ = filename;
					
					this->ClearInvalidData();
				}
				
				if (result != FALSE) return DialogResult::OK;
				return DialogResult::Cancel;
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  Multiselect
			 *
			 *  �_�C�A���O�{�b�N�X�ŕ����̃t�@�C����I���ł��邩�ǂ�����
			 *  �����l���擾����D
			 */
			/* ------------------------------------------------------------- */
			bool Multiselect() const { return (this->data_.Flags & OFN_ALLOWMULTISELECT) != 0; }
			
			/* ------------------------------------------------------------- */
			/*
			 *  Multiselect
			 *
			 *  �_�C�A���O�{�b�N�X�ŕ����̃t�@�C����I���ł��邩�ǂ�����
			 *  �����l��ݒ肷��D
			 */
			/* ------------------------------------------------------------- */
			void Multiselect(bool enable) {
				if (enable) this->data_.Flags |= OFN_ALLOWMULTISELECT;
				else this->data_.Flags &= ~OFN_ALLOWMULTISELECT;
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  ShowReadOnly
			 *
			 *  �_�C�A���O�{�b�N�X�ɓǂݎ���p�`�F�b�N�{�b�N�X���\������
			 *  �Ă��邩�ǂ����������l���擾����D
			 */
			/* ------------------------------------------------------------- */
			bool ShowReadOnly() const { return (this->data_.Flags & OFN_HIDEREADONLY) == 0; }
			
			/* ------------------------------------------------------------- */
			/*
			 *  ShowReadOnly
			 *
			 *  �_�C�A���O�{�b�N�X�ɓǂݎ���p�`�F�b�N�{�b�N�X���\������
			 *  �Ă��邩�ǂ����������l��ݒ肷��D
			 */
			/* ------------------------------------------------------------- */
			void ShowReadOnly(bool enable) {
				if (enable) this->data_.Flags &= ~OFN_HIDEREADONLY;
				else this->data_.Flags |= OFN_HIDEREADONLY;
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  ReadOnlyChecked
			 *
			 *  �ǂݎ���p�`�F�b�N�{�b�N�X���I�����I�t���������l���擾
			 *  ����D
			 */
			/* ------------------------------------------------------------- */
			bool ReadOnlyChecked() const { return (this->data_.Flags & OFN_READONLY) != 0; }
			
			/* ------------------------------------------------------------- */
			/*
			 *  ReadOnlyChecked
			 *
			 *  �ǂݎ���p�`�F�b�N�{�b�N�X���I�����I�t���������l��ݒ�
			 *  ����D
			 */
			/* ------------------------------------------------------------- */
			void ReadOnlyChecked(bool enable) {
				if (enable) this->data_.Flags |= OFN_READONLY;
				else this->data_.Flags &= ~OFN_READONLY;
			}
		};
		
		/* ----------------------------------------------------------------- */
		/*
		 *  SaveFileDialog
		 *
		 *  �u�t�@�C����ۑ��v�_�C�A���O�{�b�N�X��\������N���X�D
		 *  .NetFramework ��System.Windows.Forms.SaveFileDialog ���Q�l��
		 *  �e�����o�֐����`���Ă���D
		 *  http://msdn.microsoft.com/ja-jp/library/9tah40xd.aspx
		 */
		/* ----------------------------------------------------------------- */
		class SaveFileDialog : public FileDialog, boost::noncopyable {
		public:
			/* ------------------------------------------------------------- */
			//  constructor
			/* ------------------------------------------------------------- */
			SaveFileDialog() : FileDialog() {
				this->CheckFileExists(false);
				this->CreatePrompt(false);
				this->OverwritePrompt(true);
			}
			
			/* ------------------------------------------------------------- */
			//  destructor
			/* ------------------------------------------------------------- */
			~SaveFileDialog() {}
			
			/* ------------------------------------------------------------- */
			//  ShowDialog
			/* ------------------------------------------------------------- */
			virtual DialogResult ShowDialog(HWND owner = NULL) {
				char_type path[PSDOTNET_MAX_PATH] = {};
				char_type filename[PSDOTNET_MAX_PATH] = {};
				
				std::vector<char_type> filter;
				if (filters_.empty()) this->AddFilter(_T("���ׂẴt�@�C�� (*.*)"), _T("*.*"));
				char_type* pf = this->Join(filters_, filter) ? reinterpret_cast<char_type*>(&filter[0]) : NULL;
				
				data_.lStructSize     = sizeof(data_);
				data_.hwndOwner       = owner;
				data_.lpstrFilter     = pf;
				data_.lpstrFile       = path;
				data_.nMaxFile        = sizeof(path) / sizeof(char_type);
				data_.lpstrFileTitle  = filename;
				data_.nMaxFileTitle   = sizeof(filename) / sizeof(char_type);
				
				BOOL result = ::GetSaveFileName(&data_);
				
				if (result) {
					paths_.clear();
					paths_.push_back(path);
					
					// NOTE: System.Windows.Forms.FileDialog �� filename_ ��
					// ������v���p�e�B�������Ă��Ȃ����߁C���̒l�͎̂Ă�D
					// filename_ = filename;
					
					this->ClearInvalidData();
				}
				
				if (result != FALSE) return DialogResult::OK;
				return DialogResult::Cancel;
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  OverwritePrompt
			 *
			 *  ���ɑ��݂���t�@�C����I�����悤�Ƃ����Ƃ��C�㏑����
			 *  �m�F�_�C�A���O��\�����邩�ǂ����̐^�U�l���擾����D
			 */
			/* ------------------------------------------------------------- */
			bool OverwritePrompt() const { return (data_.Flags & OFN_OVERWRITEPROMPT) != 0; }
			
			/* ------------------------------------------------------------- */
			/*
			 *  OverwritePrompt
			 *
			 *  ���ɑ��݂���t�@�C����I�����悤�Ƃ����Ƃ��C�㏑����
			 *  �m�F�_�C�A���O��\�����邩�ǂ����̐^�U�l��ݒ肷��D
			 */
			/* ------------------------------------------------------------- */
			void OverwritePrompt(bool enable) {
				if (enable) data_.Flags |= OFN_OVERWRITEPROMPT;
				else data_.Flags &= ~OFN_OVERWRITEPROMPT;
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  CreatePrompt
			 *
			 *  ���ݑ��݂��Ȃ��t�@�C�����w�肵�悤�Ƃ����Ƃ��C�쐬�̊m�F
			 *  �_�C�A���O��\�����邩�ǂ����̐^�U�l���擾����D
			 */
			/* ------------------------------------------------------------- */
			bool CreatePrompt() const { return (data_.Flags & OFN_CREATEPROMPT) != 0; }
			
			/* ------------------------------------------------------------- */
			/*
			 *  CreatePrompt
			 *
			 *  ���ݑ��݂��Ȃ��t�@�C�����w�肵�悤�Ƃ����Ƃ��C�쐬�̊m�F
			 *  �_�C�A���O��\�����邩�ǂ����̐^�U�l��ݒ肷��D
			 */
			/* ------------------------------------------------------------- */
			void CreatePrompt(bool enable) {
				if (enable) data_.Flags |= OFN_CREATEPROMPT;
				else data_.Flags &= ~OFN_CREATEPROMPT;
			}
		};
	}
}

#endif // PSDOTNET_FORMS_FILE_DIALOG_H
