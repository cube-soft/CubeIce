/* ------------------------------------------------------------------------- */
/*
 *  archiver.h
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
 *  Last-modified: Tue 21 Dec 2010 11:59:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CUBE_ARCHIVER_H
#define CUBE_ARCHIVER_H

// ���x�ׂ̍����^�C�}�[���g�p����
#ifndef CLX_USE_HIGH_PRECISION_TIMER
#define CLX_USE_HIGH_PRECISION_TIMER
#endif

#include <string>
#include <vector>
#include <tchar.h>
#include <shlwapi.h>
#include <clx/strip.h>
#include <clx/split.h>
#include <clx/timer.h>
#include "wpopen.h"
#include "user-setting.h"
#include "dialog.h"

#define CUBEICE_ENGINE _T("7z.exe")

namespace cubeice {
	/* --------------------------------------------------------------------- */
	//  archiver
	/* --------------------------------------------------------------------- */
	class archiver {
	public:
		typedef TCHAR char_type;
		typedef std::basic_string<TCHAR> string_type;
		typedef std::size_t size_type;
		typedef cubeice::user_setting setting_type;
		
		/* ----------------------------------------------------------------- */
		//  constructor
		/* ----------------------------------------------------------------- */
		explicit archiver(const setting_type& setting) :
			setting_(setting) {}
		
		/* ----------------------------------------------------------------- */
		//  compress
		/* ----------------------------------------------------------------- */
		template <class InputIterator>
		void compress(InputIterator first, InputIterator last) {
			static const string_type keyword = _T("Compressing");
			
			bool pass = false;
			
			// �I�v�V������ǂݔ�΂��D
			if (first->compare(0, 3, _T("/c:")) != 0) return;
			string_type filetype(first->substr(3));
			for (;first != last && first->at(0) == _T('/'); ++first) {
				if (first->compare(0, 2, _T("/p")) == 0) pass = true;
			}
			
			// �ۑ���p�X�̌���
			string_type dest = this->compress_path(setting_.compression(), *first, filetype);
			if (dest.empty()) return;
			
			// �p�X���[�h�̐ݒ�D
			if (pass && password_dialog() == IDCANCEL) return;
			
			// �ꎞ�t�@�C���̃p�X������
			string_type tmp = tmpfile(_T("cubeice"));
			if (tmp.empty()) return;
			
			// �㏑���̊m�F
			if (setting_.compression().output_condition() != OUTPUT_RUNTIME) {
				int result = this->is_overwrite(dest, tmp, setting_.compression(), MB_YESNO | MB_ICONWARNING);
				if (result != IDOK && result != IDYES) return;
			}
			
			// �v���O���X�o�[�̐ݒ�
			cubeice::dialog::progressbar progress;
			size_type n = this->compress_size(first, last, 5.0);
			char msgbuf[32] = {};
			double step = (n > 0) ? (progress.maximum() - progress.minimum()) / static_cast<double>(n) : 0.0;
			if (n == 0) progress.marquee(true);
			
			// �R�}���h���C���̍쐬
			std::basic_string<TCHAR> cmdline = CUBEICE_ENGINE;
			cmdline += _T(" a");
			if (filetype == _T("exe")) cmdline += _T(" -sfx7z.sfx");
			else if (filetype == _T("gzip") || filetype == _T("bzip2")) cmdline += _T(" -ttar");
			else cmdline += _T(" -t") + filetype;
			if (pass) cmdline += _T(" -p") + cubeice::password();
			cmdline += _T(" -bd -scsWIN -y \"") + tmp + _T("\"");
			for (; first != last; ++first) cmdline += _T(" \"") + *first + _T("\"");
			cube::popen proc;
			if (!proc.open(cmdline.c_str(), _T("r"))) return;
			
			// ���C������
			int status = 0;
			string_type line;
			while ((status = proc.gets(line)) >= 0) {
				progress.refresh();
				if (progress.is_cancel()) {
					proc.close();
					break;
				}
				
				if (status == 2) break; // pipe closed
				else if (status == 0 || line.empty()) {
					Sleep(10);
					continue;
				}
				assert(status == 1);
				
				string_type::size_type pos = line.find(keyword);
				if (pos == string_type::npos || line.size() <= keyword.size()) continue;
				
				string_type filename = clx::strip_copy(line.substr(pos + keyword.size()));
				string_type message = dest + _T("\n") + filename;
				progress.text(message);
				if (n > 0) progress += step;
			}
			
			if (status == 2) {
				if (filetype == _T("gzip") || filetype == _T("bzip2")) this->compress_tar(tmp, dest, filetype, pass);
				else MoveFile(tmp.c_str(), dest.c_str());
				
				// �t�B���^�����O
				if ((setting_.compression().details() & DETAIL_FILTER) && !setting_.filters().empty()) {
					this->compress_filter(dest, setting_.filters());
				}
				
				// �t�H���_���J��
				if (setting_.compression().details() & DETAIL_OPEN) {
					string_type root = dest.substr(0, dest.find_last_of(_T('\\')));
					if ((setting_.compression().details() & DETAIL_SKIP_DESKTOP) == 0 || !this->is_desktop(root)) {
						ShellExecute(NULL, _T("open"), root.c_str(), NULL, NULL, SW_SHOWNORMAL);
					}
				}
			}
			
			if (PathFileExists(tmp.c_str())) DeleteFile(tmp.c_str());
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  decompress
		 *
		 *  NOTE: �����̈��k�t�@�C�����w�肳�ꂽ�ꍇ�́C1�t�@�C������
		 *  �𓀏��������s����D
		 */
		/* ----------------------------------------------------------------- */
		template <class InputIterator>
		void decompress(InputIterator first, InputIterator last) {
			static const string_type keyword = _T("Extracting");
			
			// ���W�X�g���̐ݒ�𖳎�����R�}���h���ǂ����D
			string_type force;
			if (first->compare(0, 3, _T("/x:")) == 0) force = first->substr(3);
			
			// �I�v�V������ǂݔ�΂��D
			while (first != last && first->at(0) == _T('/')) ++first;
			
			for (; first != last; ++first) {
				string_type src = *first;
				
				// �ۑ���p�X�̎擾
				string_type root = this->decompress_path(setting_.decompression(), src, force);
				if (root.empty()) break;
				
				// �p�X���[�h�̐ݒ�
				bool pass = this->is_password(src);
				if (pass && password_dialog() == IDCANCEL) break;
				
				// �t�H���_�̍쐬
				if (!PathFileExists(root.c_str()) && CreateDirectory(root.c_str(), NULL) == 0) break;
				
				// �ꎞ�t�H���_�̍쐬
				string_type tmp = tmpdir(_T("cubeice"));
				if (tmp.empty()) break;
				
				// �v���O���X�o�[�̏�����
				cubeice::dialog::progressbar progress;
				progress.text(root);
				
				// *.tar �n�̏���
				if (this->is_tar(src)) src = this->decompress_tar(src, tmp, pass);
				
				// �v���O���X�o�[�̐i�s�x�̐ݒ�
				size_type n = this->decompress_size(src, 5.0);
				double step = (n > 0) ? (progress.maximum() - progress.minimum()) / static_cast<double>(n) : 0.0;
				if (n == 0) progress.marquee(true);
				
				// �R�}���h���C���̐���
				string_type cmdline = CUBEICE_ENGINE;
				cmdline += _T(" x -bd -scsWIN -y");
				if (pass) cmdline += _T(" -p") + cubeice::password();
				cmdline += _T(" -o\"") + tmp + _T("\"");
				cmdline += _T(" \"") + src + _T("\"");
				cube::popen proc;
				if (!proc.open(cmdline.c_str(), _T("r"))) return;
				
				// ���C������
				int status = 0;
				bool yes_to_all = false;
				string_type line;
				while ((status = proc.gets(line)) >= 0) {
					progress.refresh();
					if (progress.is_cancel()) {
						proc.close();
						break;
					}
					
					if (status == 2) break; // pipe closed
					else if (status == 0 || line.empty()) {
						Sleep(10);
						continue;
					}
					assert(status == 1);
					
					string_type::size_type pos = line.find(keyword);
					if (pos == string_type::npos || line.size() <= keyword.size()) continue;
					
					string_type filename = clx::strip_copy(line.substr(pos + keyword.size()));
					
					// �㏑���̊m�F
					if (!yes_to_all) {
						int result = this->is_overwrite(root + _T('\\') + filename, tmp + _T('\\') + filename,
							setting_.decompression(), MB_YESNOCANCEL | MB_ICONWARNING);
						if (result == IDCANCEL) break;
						else if (result == IDNO) continue;
						else if (result == IDYES) yes_to_all = true; // TODO: �u�S�Ă͂��v�̃{�^����\��������D
					}
					
					string_type message = root + _T("\n");
					if ((setting_.decompression().details() & DETAIL_FILTER) && this->is_filter(filename, setting_.filters())) {
						message += _T("Filtering: ");
					}
					else this->move(tmp + _T('\\') + filename, root + _T('\\') + filename);
					// TODO: ������ root + _T('\\') + filename �̕��� filename �𕶎��R�[�h�ϊ�����D
					message += filename;
					progress.text(message);
					if (n > 0) progress += step;
				}
				
				// �t�H���_���J��
				if (setting_.decompression().details() & DETAIL_OPEN) {
					if ((setting_.decompression().details() & DETAIL_SKIP_DESKTOP) == 0 || !this->is_desktop(root)) {
						ShellExecute(NULL, _T("open"), root.c_str(), NULL, NULL, SW_SHOWNORMAL);
					}
				}
			}
		}
		
	private: // compress_xxx
		/* ----------------------------------------------------------------- */
		//  compress_path
		/* ----------------------------------------------------------------- */
		string_type compress_path(const setting_type::archive_type& setting, const string_type& src, const string_type& filetype) {
			string_type dest =  this->rootdir(setting, src);
			if (dest.empty()) {
				const TCHAR filter[] = _T("All files(*.*)\0*.*\0\0");
				string_type init = src.substr(0, src.find_last_of(_T('.'))) + this->extension(filetype);
				dest = cubeice::dialog::savefile(filter, init.c_str());
			}
			else {
				string_type::size_type first = src.find_last_of(_T('\\'));
				if (first == string_type::npos) first = 0;
				else ++first;
				string_type filename = src.substr(first);
				dest += _T('\\') + filename.substr(0, filename.find_last_of(_T('.'))) + this->extension(filetype);
			}
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  compress_size
		/* ----------------------------------------------------------------- */
		template <class InputIterator>
		size_type compress_size(InputIterator first, InputIterator last, double timeout) {
			size_type n = 0;
			clx::timer t;
			for (; first != last; ++first) {
				if (PathIsDirectory(first->c_str())) n += compress_size_folder(*first);
				else ++n;
				if (t.elapsed() > timeout) return 0;
			}
			return n;
		}
		
		/* ----------------------------------------------------------------- */
		//  decompress_tar
		/* ----------------------------------------------------------------- */
		void compress_tar(const string_type& src, const string_type& dest, const string_type& filetype, bool pass) {
			static const string_type keyword = _T("Compressing");
			
			string_type tar;
			string_type ext = dest.substr(dest.find_last_of(_T('.')));
			tar = dest.substr(0, dest.find_last_of(_T('.')));
			if (ext != _T(".gz") && ext != _T(".bz2")) tar += _T(".tar");
			if (!MoveFile(src.c_str(), tar.c_str())) return;
			
			std::basic_string<TCHAR> cmdline = CUBEICE_ENGINE;
			cmdline += _T(" a -t") + filetype;
			if (pass) cmdline += _T(" -p") + cubeice::password();
			cmdline += _T(" -bd -scsWIN -y \"") + dest + _T("\"");
			cmdline += _T(" \"") + tar + _T("\"");
			cube::popen proc;
			if (!proc.open(cmdline.c_str(), _T("r"))) return;
			
			int status = 0;
			string_type line;
			while ((status = proc.gets(line)) >= 0) {
				if (status == 2) break; // pipe closed
				else if (status == 0 || line.empty()) {
					Sleep(10);
					continue;
				}
				assert(status == 1);
				
				string_type::size_type pos = line.find(keyword);
				if (pos == string_type::npos || line.size() <= keyword.size()) continue;
			}
			
			DeleteFile(tar.c_str());
		}
		
		/* ----------------------------------------------------------------- */
		//  compress_size_folder
		/* ----------------------------------------------------------------- */
		size_type compress_size_folder(const string_type& root) {
			string_type path = root + _T("\\*.*");
			WIN32_FIND_DATA wfd = {};
			HANDLE handle = FindFirstFile(path.c_str(), &wfd);
			if (handle == INVALID_HANDLE_VALUE) return 1;
			
			size_type n = 0;
			do {
				if (_tcscmp(wfd.cFileName, _T(".")) != 0 && _tcscmp(wfd.cFileName, _T("..")) != 0) {
					if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
						string_type subdir = root + _T('\\') + wfd.cFileName;
						n += this->compress_size_folder(subdir);
					}
					else ++n;
				}
			} while (FindNextFile(handle, &wfd));
			FindClose(handle);
			
			return n;
		}
		
		/* ----------------------------------------------------------------- */
		//  compress_filter
		/* ----------------------------------------------------------------- */
		void compress_filter(const string_type& path, const std::set<string_type>& filters) {
			std::basic_string<TCHAR> cmdline = CUBEICE_ENGINE;
			cmdline += _T(" d -r -bd -scsWIN -y \"") + path + _T("\"");
			for (std::set<string_type>::const_iterator pos = filters.begin(); pos != filters.end(); ++pos) {
				cmdline += _T(" \"") + *pos + _T("\"");
			}
			
			cube::popen proc;
			if (!proc.open(cmdline.c_str(), _T("r"))) return;
			int status = 0;
			string_type line;
			while ((status = proc.gets(line)) >= 0) {
				if (status == 2) break; // closed pipe.
				else if (status == 0 || line.empty()) {
					Sleep(10);
					continue;
				}
				assert(status == 1);
			}
		}
		
	private: // decompress_xxx
		/* ----------------------------------------------------------------- */
		//  decompress_path
		/* ----------------------------------------------------------------- */
		string_type decompress_path(const setting_type::archive_type& setting, const string_type& src, string_type force) {
			// �ۑ���p�X�̌���
			string_type root = this->rootdir(setting, src, force);
			if (root.empty()) root = cubeice::dialog::browsefolder(_T("�𓀂���t�H���_���w�肵�ĉ������B"));
			if (root.empty()) return root;
			
			// �t�H���_�̍쐬
			if (setting.details() & DETAIL_CREATE_FOLDER) {
				string_type::size_type pos = src.find_last_of('\\');
				string_type filename = (pos == string_type::npos) ? src : src.substr(++pos);
				root += _T('\\') + filename.substr(0, filename.find_last_of(_T('.')));
			}
			return root;
		}
		
		/* ----------------------------------------------------------------- */
		//  decompress_size
		/* ----------------------------------------------------------------- */
		size_type decompress_size(const string_type& path, double timeout) {
			string_type cmdline = CUBEICE_ENGINE;
			cmdline += _T(" l ");
			cmdline += path;
			
			clx::timer t;
			cube::popen proc;
			if (!proc.open(cmdline.c_str(), _T("r"))) return 0;
			string_type buffer, src;
			int status = 0;
			while ((status = proc.gets(buffer)) >= 0) {
				if (status == 2) break; // pipe closed
				else if (status == 1 && !buffer.empty()) src = buffer;
				buffer.clear();
				if (t.elapsed() > timeout) return 0;
			}
			
			std::vector<string_type> v;
			clx::split(src, v);
			if (v.size() < 5) return 0;
			
			try {
				return clx::lexical_cast<size_type>(v.at(2)) + clx::lexical_cast<size_type>(v.at(4));
			}
			catch (std::exception&) {}
			
			return 0;
		}
		
		/* ----------------------------------------------------------------- */
		//  decompress_tar
		/* ----------------------------------------------------------------- */
		string_type decompress_tar(const string_type& src, const string_type& root, bool pass) {
			static const string_type keyword = _T("Extracting");
			
			string_type cmdline = CUBEICE_ENGINE;
			cmdline += _T(" x -bd -scsWIN -y");
			if (pass) cmdline += _T(" -p") + cubeice::password();
			cmdline += _T(" -o\"") + root + _T("\"");
			cmdline += _T(" \"") + src + _T("\"");
			cube::popen proc;
			if (!proc.open(cmdline.c_str(), _T("r"))) return src;
			
			int status = 0;
			string_type filename;
			string_type line;
			while ((status = proc.gets(line)) >= 0) {
				if (status == 2) break; // pipe closed
				else if (status == 0 || line.empty()) {
					Sleep(10);
					continue;
				}
				assert(status == 1);
				
				string_type::size_type pos = line.find(keyword);
				if (pos == string_type::npos || line.size() <= keyword.size()) continue;
				filename = clx::strip_copy(line.substr(pos + keyword.size()));
			}
			
			return root + _T('\\') + filename;
		}
		
	private: // others
		const setting_type& setting_;
		
		/* ----------------------------------------------------------------- */
		//  non-copyable
		/* ----------------------------------------------------------------- */
		archiver(const archiver& cp);
		archiver& operator=(const archiver& cp);
		
		/* ----------------------------------------------------------------- */
		/*
		 *  extension
		 *
		 *  NOTE: gzip, bz2 �̏ꍇ�� *.tar �ŕԂ��D
		 */
		/* ----------------------------------------------------------------- */
		string_type extension(const string_type& filetype) {
			string_type ext = _T(".");
			if (filetype == _T("gzip")) ext += _T("tar.gz");
			else if (filetype == _T("bzip2")) ext += _T("tar.bz2");
			else ext += filetype;
			return ext;
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  tmpfile
		 *
		 *  �����_���Ȉꎞ�t�@�C���𐶐����ăp�X��Ԃ��D
		 */
		/* ----------------------------------------------------------------- */
		string_type tmpfile(const string_type& prefix) {
			char_type dir[1024] = {};
			if (GetTempPath(1024, dir) == 0) return string_type();
			char_type path[2048] = {};
			if (GetTempFileName(dir, prefix.c_str(), 0, path) == 0) return string_type();
			
			// �ꎞ�t�@�C������������Ă���̂ō폜����D
			if (PathFileExists(path)) DeleteFile(path);
			return string_type(path);
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  tmpdir
		 *
		 *  ���C���_���Ȉꎞ�f�B���N�g���𐶐����ăp�X��Ԃ��D
		 */
		/* ----------------------------------------------------------------- */
		string_type tmpdir(const string_type& prefix) {
			string_type path = tmpfile(prefix);
			if (path.empty()) return path;
			if (CreateDirectory(path.c_str(), NULL) == 0) return string_type();
			return path;
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  rootdir
		 *
		 *  ���[�U�ݒ�̒l�����ɏo�͐�f�B���N�g�����擾����D�����
		 *  �f�B���N�g�����w�肪�`�F�b�N����Ă���C���o�͐悪
		 *  �󔒂̏ꍇ�̓f�X�N�g�b�v�̃p�X��Ԃ��D
		 *
		 *  force �Ɏw�肳���\���̂�����͈̂ȉ��̒ʂ�D
		 *   - runtime: ���s���Ɏw��
		 *   - desktop: �f�X�N�g�b�v
		 *   - source: �\�[�X�t�@�C���Ɠ����ꏊ
		 */
		/* ----------------------------------------------------------------- */
		string_type rootdir(const setting_type::archive_type& setting, const string_type& src, const string_type& force = _T("")) {
			// �f�X�N�g�b�v�̃p�X���擾���Ă����D
			LPITEMIDLIST item;
			SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &item);
			char_type buffer[CUBE_MAX_PATH] = {};
			SHGetPathFromIDList(item, buffer);
			string_type desktop = buffer;
			
			if (force == _T("runtime")) return string_type();
			if (force == _T("desktop")) return desktop;
			else if (force == _T("source") || setting.output_condition() == OUTPUT_SOURCE) {
				// TODO: ���΃p�X�̏ꍇ�ǂ����邩�D
				return src.substr(0, src.find_last_of(_T('\\')));
			}
			else if (setting.output_condition() == OUTPUT_SPECIFIC) {
				if (!setting.output_path().empty()) return setting.output_path();
				else return desktop;
			}
			return string_type();
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  move
		 *
		 *  �t�@�C�����ړ�������D�������C�f�B���N�g���̏ꍇ�͈ړ�������
		 *  ����ɓ����̃f�B���N�g�����ړ���ɍ쐬����D
		 */
		/* ----------------------------------------------------------------- */
		void move(const string_type& src, const string_type& dest) {
			if (PathIsDirectory(src.c_str())) CreateDirectory(dest.c_str(), NULL);
			else {
				string_type branch(dest.substr(0, dest.find_last_of(_T('\\'))));
				if (!PathFileExists(branch.c_str())) CreateDirectory(branch.c_str(), NULL);
				MoveFile(src.c_str(), dest.c_str());
			}
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  is_filter
		 *
		 *  �f�B���N�g���P�ʂŋ�؂�C���ꂼ��ɑ΂��ăt�B���^�����O������
		 *  �ɊY�����邩�ǂ������`�F�b�N����D
		 *  TODO: ���C���h�J�[�h�ւ̑Ή����l����D
		 */
		/* ----------------------------------------------------------------- */
		bool is_filter(const string_type& src, const std::set<string_type>& filters) {
			std::vector<string_type> v;
			clx::split_if(src, v, clx::is_any_of(_T("\\")));
			for (std::vector<string_type>::const_iterator pos = v.begin(); pos != v.end(); ++pos) {
				if (filters.find(*pos) != filters.end()) return true;
			}
			return false;
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  is_older
		 *
		 *  �t�@�C���̍X�V�������r����D
		 +/
		/* ----------------------------------------------------------------- */
		bool is_older(const string_type& target, const string_type& compared) {
			HANDLE h = CreateFile(target.c_str(), 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (h == INVALID_HANDLE_VALUE) return false;
			FILETIME t1 = {};
			BOOL status = GetFileTime(h, NULL, NULL, &t1);
			CloseHandle(h);
			if (status == FALSE) return false;
			
			h = CreateFile(compared.c_str(), 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (h == INVALID_HANDLE_VALUE) return false;
			FILETIME t2 = {};
			status = GetFileTime(h, NULL, NULL, &t2);
			if (status == FALSE) return false;
			
			if (CompareFileTime(&t1, &t2) < 0) return true;
			return false;
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  is_overwrite
		 *
		 *  �t�@�C���̏㏑�����s�����ǂ������m�F����D�쐬��ɊY���t�@�C��
		 *  �����݂��Ȃ��ꍇ�� IDOK ��Ԃ��D
		 */
		/* ----------------------------------------------------------------- */
		int is_overwrite(const string_type& target, const string_type& compared, const setting_type::archive_type& setting, UINT type) {
			if ((setting.details() & DETAIL_OVERWRITE) && PathFileExists(target.c_str())) {
				if ((setting.details() & DETAIL_IGNORE_NEWER) && !is_older(target, compared)) return IDYES;
				else {
					string_type message = target + _T(" �͊��ɑ��݂��܂��B�㏑�����܂����H");
					return MessageBox(NULL, message.c_str(), _T("�㏑���̊m�F"), type);
				}
			}
			return IDOK;
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  is_password
		 *
		 *  �A�[�J�C�u�t�@�C���Ƀp�X���[�h���ݒ肳��Ă��邩�ǂ�����
		 *  ���肷��D
		 */
		/* ----------------------------------------------------------------- */
		bool is_password(const string_type& path) {
			static const string_type keyword(_T("Enter password"));
			static const string_type pass(_T("Testing"));
			
			string_type cmdline = CUBEICE_ENGINE;
			cmdline += _T(" t \"") + path + _T("\"");
			cube::popen proc;
			if (!proc.open(cmdline.c_str(), _T("r"))) return false;
			
			int n = 0;
			int status = 0;
			string_type line;
			while ((status = proc.gets(line)) >= 0) {
				if (status == 2) break; // closed pipe
				else if (status == 0 || line.empty()) {
					Sleep(10);
					continue;
				}
				assert(status == 1);
				if (line.find(keyword) != string_type::npos) return true;
				else if (line.find(pass) != string_type::npos) {
					if (++n >= 10) break;
				}
			}
			
			return false;
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  is_tar
		 *
		 *  *.tar �n�̈��k�t�@�C�����ǂ����𔻒肷��D����`�F�b�N���Ă���
		 *  �g���q�́C*.tar.gz, *.tar.bz2, *.tgz, *.tbz �� 4��ށD
		 */
		/* ----------------------------------------------------------------- */
		bool is_tar(const string_type& path) {
			string_type::size_type pos = path.find_last_of(_T('.'));
			if (pos == string_type::npos) return false;
			string_type ext = path.substr(pos);
			if (ext == _T(".gz") || ext == _T(".bz2") || ext == _T(".tgz") || ext == _T(".tbz")) return true;
			return false;
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  is_desktop
		 *
		 *  �w�肳�ꂽ�p�X���f�X�N�g�b�v���ǂ����𔻒肷��D
		 */
		/* ----------------------------------------------------------------- */
		bool is_desktop(const string_type& path) {
			IMalloc* p = NULL;
			SHGetMalloc(&p);
			
			LPITEMIDLIST item = NULL;
			if(SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOPDIRECTORY, &item))) {
				char_type buffer[CUBE_MAX_PATH] = {};
				SHGetPathFromIDList(item, buffer);
				p->Free(item);
				p->Release();
				return path == string_type(buffer);
			}
			
			// not fouond
			p->Release();
			return false;
		}
	};
}

#endif // CUBE_ARCHIVER_H