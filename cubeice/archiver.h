/* ------------------------------------------------------------------------- */
/*
 *  archiver.h
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
#include <clx/hexdump.h>
#include <clx/strip.h>
#include <clx/split.h>
#include <clx/replace.h>
#include <clx/date_time.h>
#include <clx/timer.h>
#include <clx/lexical_cast.h>
#include <babel/babel.h>
#include "wpopen.h"
#include "pathmatch.h"
#include "user-setting.h"
#include "dialog.h"
#include "sendmail.h"

#define CUBEICE_ENGINE _T("cubeice-exec.exe")
#define MAIL_SUBJECT ""
#define MAIL_BODY ""

namespace cubeice {
	/* --------------------------------------------------------------------- */
	//  archiver
	/* --------------------------------------------------------------------- */
	class archiver {
	public:
		typedef TCHAR char_type;
		typedef std::basic_string<TCHAR> string_type;
		typedef __int64 size_type;
		typedef cubeice::user_setting setting_type;
		
		/* ----------------------------------------------------------------- */
		//  constructor
		/* ----------------------------------------------------------------- */
		explicit archiver(const setting_type& setting) :
			setting_(setting), files_(), size_(0) {}
		
		/* ----------------------------------------------------------------- */
		//  compress
		/* ----------------------------------------------------------------- */
		template <class InputIterator>
		void compress(InputIterator first, InputIterator last) {
			static const string_type keyword = _T("Compressing");
			static const string_type error = _T("ERROR:");
			
			bool pass = false;
			bool mail = false;
			
			// �I�v�V������ǂݔ�΂��D
			if (first->compare(0, 3, _T("/c:")) != 0) return;
			string_type filetype(first->substr(3));
			for (; first != last && first->at(0) == _T('/'); ++first) {
				if (first->compare(0, 2, _T("/p")) == 0) pass = true;
				if (first->compare(0, 2, _T("/m")) == 0) mail = true;
			}
			
			// NOTE: �h���b�O&�h���b�v�̍ۂɃt�@�C�����w�肳��Ȃ�����
			// (�V���[�g�J�b�g���_�u���N���b�N�����j�Ƒz�肷��D
			// TODO: ����ȊO�̏ꍇ���z�肵�����b�Z�[�W��\������K�v�����邩�ǂ����D
			if (first == last) {
				MessageBox(NULL, _T("���k�������t�@�C����t�H���_���uCubeICE ���k�v�V���[�g�J�b�g�Ƀh���b�O&�h���b�v���ĉ������B"), _T("CubeICE ���k"), MB_OK | MB_ICONINFORMATION);
				return;
			}
			
			string_type ext;
			string_type dest;
			std::vector<string_type> options;
			if (filetype == _T("detail")) {
				cubeice::runtime_setting runtime;
				ext = this->compress_extension(runtime.type(), first, last);
				dest = first->substr(0, first->find_last_of(_T('.'))) + ext;
				runtime.path() = dest;
				if (cubeice::dialog::runtime_setting(runtime) == IDCANCEL) return;
				
				// �����^�C�����̐ݒ�𔽉f����
				runtime.save();
				dest = runtime.path();
				filetype = runtime.type();
				ext = this->compress_extension(filetype, first, last);
				if (filetype == _T("tgz")) filetype = _T("gzip");
				else if (filetype == _T("tbz")) filetype = _T("bzip2");
				options.push_back(_T("-mx=") + clx::lexical_cast<string_type>(runtime.level()));
				if (runtime.type() == _T("zip")) options.push_back(_T("mm=") + runtime.method());
				else if (runtime.type() == _T("7z")) options.push_back(_T("m0=") + runtime.method());
				options.push_back(_T("-mmt=") + clx::lexical_cast<string_type>(runtime.thread_size()));
				if (runtime.enable_password()) {
					pass = true;
					cubeice::password() = runtime.password();
					if (runtime.type() == _T("zip")) options.push_back(_T("-mem=") + runtime.encoding());
				}
			}
			else {
				// �ۑ���p�X�̌���
				ext = this->compress_extension(filetype, first, last);
				dest = this->compress_path(setting_.compression(), *first, ext);
				if (dest.empty()) return;
				
				// �p�X���[�h�̐ݒ�D
				if (pass && cubeice::dialog::password(COMPRESS_FLAG) == IDCANCEL) return;
			}
			
			// �ꎞ�t�@�C���̃p�X������
			string_type tmp = tmpfile(_T("cubeice"));
			if (tmp.empty()) return;
			
			cubeice::dialog::progressbar progress;
			progress.show();
			progress.marquee(true);
			
			// �v���O���X�o�[�̐ݒ�
			this->compress_filelist(first, last, progress);
			if (this->size_ == 0) progress.marquee(true);
			else progress.marquee(false);
			
			// �R�}���h���C���̍쐬
			std::basic_string<TCHAR> cmdline = CUBEICE_ENGINE;
			cmdline += _T(" a");
			if (filetype == _T("exe")) cmdline += _T(" -sfx7z.sfx");
			else if (ext == _T(".tgz") || ext == _T(".tbz") || ext.find(_T(".tar")) != string_type::npos) cmdline += _T(" -ttar");
			else cmdline += _T(" -t") + filetype;
			if (pass) cmdline += _T(" -p\"") + cubeice::password() + _T("\"");
			//cmdline += _T(" -bd -scsWIN -y \"") + tmp + _T("\"");
			cmdline += _T(" -scsWIN -y \"") + tmp + _T("\"");
			for (InputIterator pos = first; pos != last; ++pos) cmdline += _T(" \"") + *pos + _T("\"");
			for(std::vector<string_type>::const_iterator pos = options.begin(); pos != options.end(); ++pos) cmdline += _T(' ') + *pos;
			cube::popen proc;
			if (!proc.open(cmdline.c_str(), _T("r"))) return;
			progress.text(dest);
			
			// ���C������
			int status = 0;
			string_type line;
			string_type report;
			double calcpos = 0.0; // �v�Z��̃v���O���X�o�[�̈ʒu
			unsigned int index = 0;
			progress.position(progress.minimum());
			progress.subposition(progress.minimum());
			progress.title(_T("0% - ") + this->filename(dest) + _T(" �����k���Ă��܂� - CubeICE"));
			while ((status = proc.gets(line)) >= 0) {
				progress.refresh();
				if (progress.is_cancel()) {
					proc.close();
					break;
				}
				
				if (status == 2) break; // pipe closed
				else if (status == 0 || line.empty()) {
					// TODO: �v���O���X�o�[�̉��P
					//size_type fsize = this->filesize(tmp);
					//double tmppos = (progress.maximum() - progress.minimum()) / (this->size_ / (double)fsize);
					//progress.position(tmppos);
					//if (progress.position() < progress.maximum() * 0.95) ++progress;
					Sleep(1);
					continue;
				}
				assert(status == 1);
				
				string_type::size_type pos = line.find(error);
				if (pos != string_type::npos) {
					if (progress.position() < progress.maximum()  * 0.95) ++progress;
					report += clx::strip_copy(line.substr(pos));
					if (pos != 0) report += _T(" (") + clx::strip_copy(line.substr(0, pos)) + _T(")");
					report += _T("\r\n");
					continue;
				}
				
				pos = line.find(_T(":%")); // :%n% �ƌ����`�Ői�������\�������
				if (pos != string_type::npos) {
					string_type percent = line.substr(pos);
					clx::strip_if(percent, clx::is_any_of(_T(":% ")));
					line.erase(pos);
					calcpos = clx::lexical_cast<double>(percent) * 100.0;
					progress.position(calcpos);
					string_type title = percent + _T("% - ") + this->filename(dest) + _T(" �����k���Ă��܂� - CubeICE");
					progress.title(title);
				}
				
				pos = line.find(keyword);
				if (pos == string_type::npos || line.size() <= keyword.size()) continue;
				string_type filename = clx::strip_copy(line.substr(pos + keyword.size()));
				progress.text(filename);
				
				// �v���O���X�o�[�̍X�V
				//fileinfo elem = this->compress_getinfo(first, last, filename);
				//calcpos += (progress.maximum() - progress.minimum()) / (this->size_ / (double)elem.size);
				//if (this->size_ > 0) progress.position(calcpos);
				
				if (index < files_.size() - 1) ++index;
			}
			
			if (status < 0) report += error + _T(" Broken pipe.\r\n");
			if ((setting_.compression().details() & DETAIL_REPORT) && !report.empty()) {
				cubeice::dialog::report(report);
			}
			
			if (status == 2) {
				// *.tar �̏���
				if ((filetype == _T("gzip") || filetype == _T("bzip2")) &&
				    (ext.find(_T(".tar")) != string_type::npos || ext == _T(".tgz") || ext == _T(".tbz"))) {
					string_type prev = tmp;
					tmp = tmpfile(_T("cubeice"));
					this->compress_tar(prev, tmp, filetype, pass, progress);
					if (PathFileExists(prev.c_str())) DeleteFile(prev.c_str());
				}
				
				if (mail) {
#ifdef	UNICODE
					babel::init_babel();
					cube::utility::sendmail::SendMail( MAIL_SUBJECT, MAIL_BODY, babel::unicode_to_sjis( tmp ).c_str(), babel::unicode_to_sjis( dest.substr( dest.find_last_of( _T( '\\' ) ) + 1 ) ).c_str() );
#else	// UNICODE
					cube::utility::sendmail::SendMail( MAIL_SUBJECT, MAIL_BODY, tmp.c_str(), dest.substr( dest.find_last_of( _T( '\\' ) ) + 1 ).c_str() );
#endif	// UNICODE
					if ((setting_.compression().details() & DETAIL_MAIL_REMOVE)) {
						if (PathFileExists(tmp.c_str())) DeleteFile(tmp.c_str());
						return;
					}
				}
				
				MoveFileEx(tmp.c_str(), dest.c_str(), (MOVEFILE_COPY_ALLOWED));
				
				// �t�B���^�����O
				if ((setting_.compression().details() & DETAIL_FILTER) && !setting_.filters().empty()) {
					this->compress_filter(dest, setting_.filters());
				}
				
				// �t�H���_���J��
				if (setting_.compression().details() & DETAIL_OPEN) {
					string_type root = dest.substr(0, dest.find_last_of(_T('\\')));
					if ((setting_.compression().details() & DETAIL_SKIP_DESKTOP) == 0 || !this->is_desktop(root)) {
						ShellExecute(NULL, _T("open"), _T("explorer.exe"), root.c_str(), NULL, SW_SHOWNORMAL);
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
			static const string_type error = _T("ERROR:");
			
			// ���W�X�g���̐ݒ�𖳎�����R�}���h���ǂ����D
			string_type force;
			if (first->compare(0, 3, _T("/x:")) == 0) force = first->substr(3);
			
			// �I�v�V������ǂݔ�΂��D
			while (first != last && first->at(0) == _T('/')) ++first;
			
			// NOTE: �h���b�O&�h���b�v�̍ۂɃt�@�C�����w�肳��Ȃ�����
			// (�V���[�g�J�b�g���_�u���N���b�N�����j�Ƒz�肷��D
			// TODO: ����ȊO�̏ꍇ���z�肵�����b�Z�[�W��\������K�v�����邩�ǂ����D
			if (first == last) {
				MessageBox(NULL, _T("�𓀂��������k�t�@�C�����uCubeICE �𓀁v�V���[�g�J�b�g�Ƀh���b�O&�h���b�v���ĉ������B"), _T("CubeICE ��"), MB_OK | MB_ICONINFORMATION);
				return;
			}
			
			for (; first != last; ++first) {
				string_type report; // �G���[���|�[�g
				
				string_type src = *first;
				string_type srcname = src;
				string_type filetype;
				if (!this->decompress_filetype(src, filetype)) {
					string_type message = src + _T(" �͖��Ή��̃t�@�C���`���̂��߉𓀂ł��܂���B");
					MessageBox(NULL, message.c_str(), _T("CubeICE �𓀃G���["), MB_OK | MB_ICONERROR);
					return;
				}
				
				// �ۑ���p�X�̎擾
				string_type root = this->decompress_path(setting_.decompression(), src, force);
				if (root.empty()) break;
				
				cubeice::dialog::progressbar progress;
				progress.show();
				progress.marquee(true);
				
				// �p�X���[�h�̐ݒ�
				bool pass = this->decompress_password(src, string_type(), progress);
				if (pass) {
					do {
						if (cubeice::dialog::password(DECOMPRESS_FLAG) == IDCANCEL) return;
					} while (this->decompress_password(src, cubeice::password(), progress));
				}
				
				// �ꎞ�t�H���_�̍쐬
				string_type tmp = tmpdir(_T("cubeice"));
				if (tmp.empty()) break;
				
				// �v���O���X�o�[�̐i�s�x�̐ݒ�
				string_type folder = this->decompress_filelist(src, progress);
				if (this->size_ == 0) progress.marquee(true);
				else progress.marquee(false);
				
				// �t�H���_�̍쐬
				if ((setting_.decompression().details() & DETAIL_CREATE_FOLDER)) {
					if ((setting_.decompression().details() & DETAIL_SINGLE_FOLDER) == 0 || folder.empty()) {
						root = this->decompress_dirname(setting_.decompression(), root, src);
					}
				}
				
				// *.tar �n�̏���
				// TODO: ���݁C�g���q���{���̎�ނƈقȂ�t�@�C�����Ώۂɂ��Ă��邪�C
				// �����̋U���i�H�j�g���q�̃t�@�C���� *.tar ���ǂ������ǂ����f���邩�D
				if ((filetype == _T("gzip") || filetype == _T("bzip2")) && this->is_tar(src)) {
					src = this->decompress_tar(src, tmp, progress, report);
					this->decompress_filelist(src, progress);
					if (src.empty() || !PathFileExists(src.c_str())) break;
					if (this->size_ == 0) progress.marquee(true);
					else progress.marquee(false);
				}
				
				// �R�}���h���C���̐���
				string_type cmdline = CUBEICE_ENGINE;
				cmdline += _T(" x -bd -scsWIN -y");
				if (pass) cmdline += _T(" -p") + cubeice::password();
				cmdline += _T(" -o\"") + tmp + _T("\"");
				cmdline += _T(" \"") + src + _T("\"");
				cube::popen proc;
				if (!proc.open(cmdline.c_str(), _T("r"))) return;
				
				unsigned int index = 0;
				
				// ���C������
				int status = 0;
				int to_all = 0; // �͂�/������/���l�[��
				string_type line;
				double calcpos = 0.0; // �v�Z��̃v���O���X�o�[�̈ʒu
				progress.position(progress.minimum());
				progress.subposition(progress.minimum());
				progress.title(_T("0% - ") + this->filename(srcname) + _T(" ���𓀂��Ă��܂� - CubeICE"));
				while ((status = proc.gets(line)) >= 0) {
					if (progress.subposition() > progress.maximum() - 1.0) progress.subposition(progress.minimum());
					progress.refresh();
					if (progress.is_cancel()) {
						proc.close();
						break;
					}
					
					progress.text(root + _T("\r\n") + files_[index].name);
					if (status == 2) break; // pipe closed
					else if (status == 0 || line.empty()) {
						// �v���O���X�o�[�̍X�V
						size_type fsize = filesize(tmp + _T("\\") + files_[index].name);
						double tmppos = calcpos + (progress.maximum() - progress.minimum()) / (this->size_ / (double)fsize);
						progress.position(tmppos);
						double subpos = (progress.maximum() - progress.minimum()) / (files_[index].size / (double)fsize);
						progress.subposition(subpos);
						
						// �^�C�g���̍X�V
						size_type percent = static_cast<size_type>(tmppos / 100.0);
						string_type title = clx::lexical_cast<string_type>(percent) + _T("% - ") + this->filename(srcname) + _T(" ���𓀂��Ă��܂� - CubeICE");
						progress.title(title);
						
						Sleep(1);
						continue;
					}
					assert(status == 1);
					
					string_type::size_type pos = line.find(error);
					if (pos != string_type::npos) {
						if (progress.position() < progress.maximum()) ++progress;
						report += clx::strip_copy(line.substr(pos));
						if (pos != 0) report += _T(" (") + clx::strip_copy(line.substr(0, pos)) + _T(")");
						report += _T("\r\n");
						continue;
					}
					
					pos = line.find(keyword);
					if (pos == string_type::npos || line.size() <= keyword.size()) {
						if (progress.position() < progress.maximum() * 0.95) ++progress;
						continue;
					}
					string_type filename = clx::strip_copy(line.substr(pos + keyword.size()));
					
					// �v���O���X�o�[�̍X�V
					if (this->size_ > 0) {
						calcpos += (progress.maximum() - progress.minimum()) / (this->size_ / (double)this->files_[index].size);
						if (calcpos > progress.maximum()) calcpos = progress.maximum();
						progress.position(calcpos);
						progress.subposition(progress.maximum());
						
						// �^�C�g���̍X�V
						size_type percent = (calcpos > 1.0) ? static_cast<size_type>(calcpos / 100.0) : 0;
						string_type title = clx::lexical_cast<string_type>(percent) + _T("% - ") + this->filename(srcname) + _T(" ���𓀂��Ă��܂� - CubeICE");
						progress.title(title);
					}
					
					// �㏑���̊m�F
					int result = this->is_overwrite(root + _T('\\') + filename, tmp, filename, setting_.decompression(), to_all);
					if ((result & ID_TO_ALL)) {
						result &= ~ID_TO_ALL;
						to_all = result;
					}
					if (result == IDCANCEL) break;
					else if (result == IDNO) continue;
					
					// �t�B���^�����O
					string_type message = root + _T("\r\n");
					if ((setting_.decompression().details() & DETAIL_FILTER) && this->is_filter(filename, setting_.filters())) {
						// report += _T("Filtering: ") + filename + _T("\r\n");
					}
					else if (!this->move(tmp + _T('\\') + filename, root + _T('\\') + filename, result == IDRENAME)) {
						report += error + _T(" Can not move file.");
						report += _T(" (") + keyword + _T(' ') + filename + _T(")\r\n");
					}
					
					if (index < files_.size() - 1) ++index;
				}
				
				if (status < 0) report += error + _T(" Broken pipe.");
				if ((setting_.decompression().details() & DETAIL_REPORT) && !report.empty()) {
					cubeice::dialog::report(report);
				}
				
				// �t�H���_���J��
				if (setting_.decompression().details() & DETAIL_OPEN) {
					if ((setting_.decompression().details() & DETAIL_CREATE_FOLDER) &&
						(setting_.decompression().details() & DETAIL_SINGLE_FOLDER) &&
						!folder.empty()) {
						root += _T("\\") + folder;
					}
					
					bool skip_flag = (setting_.decompression().details() & DETAIL_SKIP_DESKTOP) != 0;
					if ((!skip_flag || !this->is_desktop(root)) && PathFileExists(root.c_str())) {
						ShellExecute(NULL, _T("open"), _T("explorer.exe"), root.c_str(), NULL, SW_SHOWNORMAL);
					}
				}
				
				this->remove(tmp.c_str());
			}
		}
		
	private:
		struct fileinfo {
		public:
			string_type    name;
			size_type      size;
			clx::date_time time;
			bool           directory;
			
			fileinfo() : name(), size(0), time(), directory(false) {}
		};
		
		const setting_type& setting_;
		std::vector<fileinfo> files_;
		size_type size_; // �g�[�^���T�C�Y
		
	private: // compress_xxx
		/* ----------------------------------------------------------------- */
		/*
		 *  compress_extension
		 */
		/* ----------------------------------------------------------------- */
		template <class InputIterator>
		string_type compress_extension(const string_type& filetype, InputIterator first, InputIterator last) {
			string_type ext = _T(".");
			if (filetype == _T("gzip")) ext += _T("gz");
			else if (filetype == _T("bzip2")) ext += _T("bz2");
			else ext += filetype;

			if (filetype == _T("gzip") || filetype == _T("bzip2")) {
				int n = 0;
				for (InputIterator pos = first; pos != last; ++pos) ++n;
				if (n > 1 || first->find_last_of(_T('.')) == string_type::npos) {
					ext = _T(".tar") + ext;
				}
			}
			
			return ext;
		}

		/* ----------------------------------------------------------------- */
		//  compress_path
		/* ----------------------------------------------------------------- */
		string_type compress_path(const setting_type::archive_type& setting, const string_type& src, const string_type& ext) {
			const TCHAR filter[] = _T("All files(*.*)\0*.*\0\0");
			string_type path = src.substr(0, src.find_last_of(_T('.')));
			string_type::size_type pos = path.find_last_of(_T('.'));
			if (pos != string_type::npos && path.substr(pos) == _T(".tar")) path = path.substr(0, pos);
			path += ext;
			
			string_type dest =  this->rootdir(setting, src);
			if (dest.empty()) dest = cubeice::dialog::savefile(filter, path.c_str());
			else {
				string_type::size_type first = path.find_last_of(_T('\\'));
				if (first == string_type::npos) first = 0;
				else ++first;
				string_type filename = path.substr(first);
				dest += _T('\\') + filename;
				if (PathFileExists(dest.c_str())) dest = cubeice::dialog::savefile(filter, path.c_str());
			}
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  compress_filelist
		/* ----------------------------------------------------------------- */
		template <class InputIterator>
		void compress_filelist(InputIterator first, InputIterator last, cubeice::dialog::progressbar& progress) {
			this->size_ = 0;
			this->files_.clear();
			
			for (; first != last; ++first) {
				progress.refresh();
				if (PathIsDirectory(first->c_str())) compress_filelist_folder(*first);
				else {
					fileinfo elem = this->createinfo(*first);
					this->files_.push_back(elem);
					this->size_ += elem.size;
				}
			}
		}
		
		/* ----------------------------------------------------------------- */
		//  compress_filelist_folder
		/* ----------------------------------------------------------------- */
		void compress_filelist_folder(const string_type& root) {
			string_type path = root + _T("\\*.*");
			WIN32_FIND_DATA wfd = {};
			HANDLE handle = FindFirstFile(path.c_str(), &wfd);
			if (handle == INVALID_HANDLE_VALUE) return;
			
			do {
				if (_tcscmp(wfd.cFileName, _T(".")) != 0 && _tcscmp(wfd.cFileName, _T("..")) != 0) {
					string_type s = root + _T('\\') + wfd.cFileName;
					if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) this->compress_filelist_folder(s);
					else {
						fileinfo elem = this->createinfo(s);
						this->files_.push_back(elem);
						this->size_ += elem.size;
					}
				}
			} while (FindNextFile(handle, &wfd));
			FindClose(handle);
		}
		
		/* ----------------------------------------------------------------- */
		//  compress_tar
		/* ----------------------------------------------------------------- */
		void compress_tar(const string_type& src, const string_type& dest, const string_type& filetype, bool pass, cubeice::dialog::progressbar& progress) {
			static const string_type keyword = _T("Compressing");
			
			string_type tar;
			string_type ext = dest.substr(dest.find_last_of(_T('.')));
			tar = dest.substr(0, dest.find_last_of(_T('.')));
			if (ext != _T(".gz") && ext != _T(".bz2")) tar += _T(".tar");
			if (!MoveFileEx(src.c_str(), tar.c_str(), MOVEFILE_COPY_ALLOWED)) return;
			
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
				progress.refresh();
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
		
		/* ----------------------------------------------------------------- */
		//  compress_getinfo
		/* ----------------------------------------------------------------- */
		template <class InputIterator>
		fileinfo compress_getinfo(InputIterator first, InputIterator last, const string_type& filename) {
			while (first != last) {
				string_type path = first->substr(0, first->find_last_of(_T('\\')));
				path += _T('\\') + filename;
				//std::map<string_type, fileinfo>::const_iterator pos = this->filelist_.find(path);
				//if (pos != this->filelist_.end()) return pos->second;
				if (PathFileExists(path.c_str())) return this->createinfo(path);
				++first;
			}
			return fileinfo();
		}
		
	private: // decompress_xxx
		/* ----------------------------------------------------------------- */
		//  decompress_path
		/* ----------------------------------------------------------------- */
		string_type decompress_path(const setting_type::archive_type& setting, const string_type& src, string_type force) {
			// �ۑ���p�X�̌���
			string_type root = this->rootdir(setting, src, force);
			if (root.empty()) {
				string_type init = src.substr(0, src.find_last_of(_T('\\')));
				root = cubeice::dialog::browsefolder(init.c_str(), _T("�𓀂���t�H���_���w�肵�ĉ������B"));
			}
			return root;
		}
		
		/* ----------------------------------------------------------------- */
		//  decompress_dirname
		/* ----------------------------------------------------------------- */
		string_type decompress_dirname(const setting_type::archive_type& setting, const string_type& root, const string_type& src) {
			string_type dest = root;
			
			// �t�H���_�̍쐬
			string_type::size_type pos = src.find_last_of(_T('\\'));
			string_type filename = (pos == string_type::npos) ? src : src.substr(++pos);
			dest += _T('\\') + filename.substr(0, filename.find_last_of(_T('.')));
			pos = dest.find_last_of(_T('.'));
			if (pos != string_type::npos && dest.substr(pos) == _T(".tar")) dest.erase(pos);
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  decompress_filetype
		/* ----------------------------------------------------------------- */
		bool decompress_filetype(const string_type& path, string_type& filetype) {
			if (PathIsDirectory(path.c_str())) return false;
			
			string_type cmdline = CUBEICE_ENGINE;
			cmdline += _T(" l ");
			cmdline += _T("\"") + path + _T("\"");
			
			cube::popen proc;
			if (!proc.open(cmdline.c_str(), _T("r"))) return false;
			string_type buffer;
			int status = 0;
			while ((status = proc.gets(buffer)) >= 0) {
				if (status == 2) break; // pipe closed
				else if (status == 0 || buffer.empty()) continue;
				
				clx::escape_separator<TCHAR> sep(_T(" \t"), _T("\""), _T(""));
				clx::basic_tokenizer<clx::escape_separator<TCHAR>, std::basic_string<TCHAR> > v(buffer, sep);
				
				buffer.clear();
				if (v.size() >= 3 && v.at(0) == _T("Type") && v.at(1) == _T("=")) {
					// �ΏۊO�̃t�@�C���^�C�v���L�q����D
					if (v.at(2) == _T("Compound")) continue;
					
					filetype = v.at(2);
					proc.close();
					return true;
				}
			}
			
			// NOTE: 7z �̏ꍇ�t�@�C���^�C�v���擾�ł��Ȃ���������D
			// �g���q�� *.7z �̏ꍇ�̓X���[����D
			string_type::size_type pos = path.find_last_of(_T('.'));
			if (pos != string_type::npos && path.substr(pos) == _T(".7z")) {
				filetype = _T("7z");
				return true;
			}
			
			return false;
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  decompress_filelist
		 *
		 *  �Ԃ�l�� root �ƂȂ�t�H���_���P��t�H���_�̏ꍇ�C����
		 *  �t�H���_���D
		 */
		/* ----------------------------------------------------------------- */
		string_type decompress_filelist(const string_type& path, cubeice::dialog::progressbar& progress) {
			string_type cmdline = CUBEICE_ENGINE;
			cmdline += _T(" l ");
			cmdline += _T("\"") + path + _T("\"");
			
			this->size_ = 0;
			string_type dest;
			
			cube::popen proc;
			if (!proc.open(cmdline.c_str(), _T("r"))) return dest;
			string_type buffer, src;
			int status = 0;
			bool single = true;
			while ((status = proc.gets(buffer)) >= 0) {
				progress.refresh();
				if (status == 2) break; // pipe closed
				else if (status == 1 && !buffer.empty()) src = buffer;
				
				clx::escape_separator<TCHAR> sep(_T(" \t"), _T("\""), _T(""));
				clx::basic_tokenizer<clx::escape_separator<TCHAR>, std::basic_string<TCHAR> > v(buffer, sep);
				
				if (v.empty() || v.at(0) != _T("<>")) {
					buffer.clear();
					continue;
				}
				
				if (v.size() == 6) {
					// �t�@�C�����X�g�̍X�V
					fileinfo elem;
					elem.name = v.at(5);
					elem.size = v.at(3) != _T("-") ? clx::lexical_cast<std::size_t>(v.at(3)) : 0;
					if (v.at(1) != _T("-")) elem.time.from_string(v.at(1), string_type(_T("%Y-%m-d %H:%M:%S")));
					elem.directory = (v.at(2).find(_T('D')) != string_type::npos);
					//filelist_[v.at(5)] = elem;
					files_.push_back(elem);
					this->size_ += elem.size;
					
					// �P��t�H���_���ǂ����̃`�F�b�N
					if (single) {
						string_type::size_type pos = v.at(5).find_first_of(_T('\\'));
						string_type folder = (pos != string_type::npos) ? v.at(5).substr(0, pos) : v.at(5);
						if (pos != string_type::npos || elem.directory) {
							if (dest.empty()) dest = folder;
							else if (dest != folder) single = false;
						}
						else single = false;
					}
				}
				buffer.clear();
			}
			if (!single) dest.erase();
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  decompress_tar
		/* ----------------------------------------------------------------- */
		string_type decompress_tar(const string_type& src, const string_type& root,
			cubeice::dialog::progressbar& progress, string_type& report) {
			static const string_type keyword = _T("Extracting");
			static const string_type error = _T("ERROR:");
			
			string_type cmdline = CUBEICE_ENGINE;
			cmdline += _T(" x -bd -scsWIN -y");
			cmdline += _T(" -o\"") + root + _T("\"");
			cmdline += _T(" \"") + src + _T("\"");
			cube::popen proc;
			if (!proc.open(cmdline.c_str(), _T("r"))) return src;
			
			int status = 0;
			string_type filename;
			string_type line;
			while ((status = proc.gets(line)) >= 0) {
				progress.refresh();
				if (status == 2) break; // pipe closed
				else if (status == 0 || line.empty()) {
					Sleep(10);
					continue;
				}
				assert(status == 1);
				
				string_type::size_type pos = line.find(error);
				if (pos != string_type::npos) {
					if (progress.position() < progress.maximum()) ++progress;
					report += clx::strip_copy(line.substr(pos));
					if (pos != 0) report += _T(" (") + clx::strip_copy(line.substr(0, pos)) + _T(")");
					report += _T("\r\n");
					continue;
				}
				
				pos = line.find(keyword);
				if (pos == string_type::npos || line.size() <= keyword.size()) continue;
				filename = clx::strip_copy(line.substr(pos + keyword.size()));
			}
			
			return root + _T('\\') + filename;
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  decompress_password
		 *
		 *  �A�[�J�C�u�t�@�C���Ƀp�X���[�h���ݒ肳��Ă��邩�ǂ�����
		 *  ���肷��D
		 */
		/* ----------------------------------------------------------------- */
		bool decompress_password(const string_type& path, const string_type& password, cubeice::dialog::progressbar& progress) {
			static const string_type keyword(_T("Enter password"));
			static const string_type pass(_T("Testing"));
			static const string_type error(_T("Wrong password?"));
			static const int limit = 10;
			
			string_type cmdline = CUBEICE_ENGINE;
			cmdline += _T(" t");
			if (!password.empty()) cmdline += _T(" -p\"") + password + _T("\"");
			cmdline += _T(" \"") + path + _T("\"");
			cube::popen proc;
			if (!proc.open(cmdline.c_str(), _T("r"))) return false;
			
			int n = 0;
			int status = 0;
			string_type line;
			while ((status = proc.gets(line)) >= 0) {
				progress.refresh();
				if (status == 2) break; // closed pipe
				else if (status == 0 || line.empty()) {
					Sleep(10);
					continue;
				}
				assert(status == 1);
				
				if (line.find(keyword) != string_type::npos) return true;
				else if (line.find(error) != string_type::npos) return true; // try again
				else if (line.find(pass) != string_type::npos) {
					if (++n >= limit) break;
				}
			}
			
			return false;
		}
		
	private: // others
		/* ----------------------------------------------------------------- */
		//  non-copyable
		/* ----------------------------------------------------------------- */
		archiver(const archiver& cp);
		archiver& operator=(const archiver& cp);
		
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
				string_type folder;
				ZeroMemory(buffer, CUBE_MAX_PATH);
				DWORD status = GetFullPathName(src.c_str(), CUBE_MAX_PATH, buffer, NULL);
				if (status == 0) folder = src;
				else folder = buffer;
				return folder.substr(0, folder.find_last_of(_T('\\')));
			}
			else if (setting.output_condition() == OUTPUT_SPECIFIC) {
				if (!setting.output_path().empty()) return setting.output_path();
				else return desktop;
			}
			return string_type();
		}
		
		/* ----------------------------------------------------------------- */
		//  createdir
		/* ----------------------------------------------------------------- */
		bool createdir(const string_type& path) {
			if (PathFileExists(path.c_str())) {
				if (PathIsDirectory(path.c_str())) return true;
				else DeleteFile(path.c_str());
			}
			
			string_type::size_type pos = path.find_last_of(_T('\\'));
			if (pos != string_type::npos) {
				if (!createdir(path.substr(0, pos))) return false;
			}
			return CreateDirectory(path.c_str(), NULL) == TRUE;
		}
		
		/* ----------------------------------------------------------------- */
		//  createinfo
		/* ----------------------------------------------------------------- */
		fileinfo createinfo(const string_type& path) {
			fileinfo dest;
			
			dest.name = path;
			dest.directory = (PathIsDirectory(path.c_str()) == TRUE);
			HANDLE h = CreateFile(path.c_str(), 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (h != INVALID_HANDLE_VALUE) {
				FILETIME t = {};
				if (GetFileTime(h, NULL, NULL, &t)) {
					SYSTEMTIME sys = {};
					FileTimeToSystemTime(&t , &sys);
					dest.time.assign(static_cast<int>(sys.wYear), static_cast<int>(sys.wMonth), static_cast<int>(sys.wDay),
						static_cast<int>(sys.wHour), static_cast<int>(sys.wMinute), static_cast<int>(sys.wSecond));
				}
				dest.size = GetFileSize(h, NULL);
			}
			CloseHandle(h);
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  move
		 *
		 *  �t�@�C�����ړ�������D�������C�f�B���N�g���̏ꍇ�͈ړ�������
		 *  ����ɓ����̃f�B���N�g�����ړ���ɍ쐬����D
		 */
		/* ----------------------------------------------------------------- */
		bool move(const string_type& src, const string_type& dest, bool rename) {
			bool status = false;
			if (PathIsDirectory(src.c_str())) {
				status = createdir(dest);
			} else {
				string_type branch(dest.substr(0, dest.find_last_of(_T('\\'))));
				status = createdir(branch);
				// TODO: ���l�[��������ǉ�
				if (rename && PathFileExists(dest.c_str())) {
					TCHAR drivename[8], dirname[4096], basename[1024], extname[32];
					_wsplitpath_s(dest.c_str(), drivename, sizeof(drivename), dirname, sizeof(dirname), basename, sizeof(basename), extname, sizeof(extname));
					TCHAR renamed[8192];
					// renamed ��.....(N).�g���q�ɕύX���� N�͐����B�ꉞ N< MAX_RENAME_DIGIT�Ƃ��Ă���
					for (int i = 2; i <= 2147483647; i++) {
						swprintf_s(renamed, sizeof(renamed), _T("%s%s%s(%d)%s"), drivename, dirname, basename, i, extname);
						if (!PathFileExists(renamed)) break;
					}
					status &= (MoveFileEx(src.c_str(), renamed, (MOVEFILE_COPY_ALLOWED)) == TRUE);
				} else {
					status &= (MoveFileEx(src.c_str(), dest.c_str(), (MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING)) == TRUE);
				}
			}
			return status;
		}
		
		/* ----------------------------------------------------------------- */
		//  remove
		/* ----------------------------------------------------------------- */
		bool remove(LPCTSTR lpPathName, bool bReadOnly = TRUE) {
			if (lpPathName == NULL) return false;
			
			TCHAR szPathName[_MAX_PATH] = {};
			_tcscpy(szPathName, lpPathName);
			_tcscat(szPathName, _T("\\*.*"));
			
			WIN32_FIND_DATA wfd;
			HANDLE handle = FindFirstFile(szPathName, &wfd);
			if(handle == INVALID_HANDLE_VALUE) return false;
			
			do {
				if (_tcscmp(wfd.cFileName, _T(".")) != 0 && _tcscmp(wfd.cFileName, _T("..")) != 0) {
					TCHAR szFileName[_MAX_PATH] = {};
					_tcscpy(szFileName, lpPathName);
					_tcscat(szFileName, _T("\\") );
					_tcscat(szFileName, wfd.cFileName);
					if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
						this->remove(szFileName, bReadOnly);
					}
					else {
						if (bReadOnly == TRUE) {
							DWORD dwFileAttributes;
							dwFileAttributes = GetFileAttributes(szFileName);
							dwFileAttributes &= ~FILE_ATTRIBUTE_READONLY;
							SetFileAttributes(szFileName, dwFileAttributes);
						}
						
						DeleteFile(szFileName);
					}
				}
			} while (FindNextFile(handle, &wfd));
			FindClose(handle);
			
			if (bReadOnly == TRUE) {
				DWORD dwFileAttributes;
				dwFileAttributes = GetFileAttributes(lpPathName);
				dwFileAttributes &= ~FILE_ATTRIBUTE_READONLY;
				SetFileAttributes(lpPathName, dwFileAttributes);
			}
			BOOL bResult = RemoveDirectory(lpPathName);

			return (bResult == TRUE);
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  punct
		 *
		 *  3�����ɃJ���}��}������D
		 */
		/* ----------------------------------------------------------------- */
		string_type punct(size_type num) {
			static const int digit = 1000;
			
			string_type dest;
			while (num / digit > 0) {
				string_type comma = dest.empty() ? _T("") : _T(",");
				char_type buffer[8] = {};
				_stprintf_s(buffer, _T("%03d"), num % 1000);
				dest = buffer + comma + dest;
				num /= 1000;
			}
			
			string_type comma = dest.empty() ? _T("") : _T(",");
			char_type buffer[8] = {};
			_stprintf_s(buffer, _T("%d"), num % 1000);
			dest = buffer + comma + dest;
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  is_filter
		/* ----------------------------------------------------------------- */
		bool is_filter(const string_type& src, const std::set<string_type>& filters) {
			for (std::set<string_type>::const_iterator pos = filters.begin(); pos != filters.end(); ++pos) {
				if (cubeice::pathmatch(src, *pos)) return true;
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
		int is_overwrite(const string_type& target, const string_type& compared, const setting_type::archive_type& setting, int force) {
			if ((setting.details() & DETAIL_OVERWRITE) && PathFileExists(target.c_str()) && !PathIsDirectory(target.c_str())) {
				if ((setting.details() & DETAIL_IGNORE_NEWER) && !is_older(target, compared)) return IDYES;
				else if (force > 0) return force;
				else {
					string_type message = target + _T(" �͊��ɑ��݂��܂��B�㏑�����܂����H");
					return MessageBox(NULL, message.c_str(), _T("�㏑���̊m�F"), MB_YESNO | MB_ICONWARNING);
				}
			}
			return IDOK;
		}
		
		/* ----------------------------------------------------------------- */
		/*
		 *  is_overwrite
		 *
		 *  �t�@�C���̏㏑�����s�����ǂ������m�F����D
		 *  �t�@�C���̏㏑���_�C�A���O�Ƀt�@�C�������o�͂���o�[�W�����D
		 */
		/* ----------------------------------------------------------------- */
		int is_overwrite(const string_type& target, const string_type& root, const string_type& filename, const setting_type::archive_type& setting, int force) {
			string_type compared = root + _T("\\") + filename;
			if ((setting.details() & DETAIL_OVERWRITE) && PathFileExists(target.c_str()) && !PathIsDirectory(target.c_str())) {
				if ((setting.details() & DETAIL_IGNORE_NEWER) && !is_older(target, compared)) return IDYES;
				else if (force > 0) return force;
				else {
					string_type message = _T("���̏ꏊ�ɂ͓������O�̃t�@�C�������ɑ��݂��܂��B�㏑�����܂����H\r\n");
					
					// ���݂̃t�@�C��
					fileinfo elem = this->createinfo(target);
					message += _T("\r\n");
					message += _T("���݂̃t�@�C��\r\n");
					message += target + _T("\r\n");
					message += _T("�T�C�Y: ") + this->punct(elem.size) + _T(" �o�C�g\r\n");
					message += _T("�X�V����: ") + elem.time.to_string(_T("%Y-%m-%d %H:%M:%S\r\n"));
					
					// �V�����t�@�C��
					//elem = this->filelist_[filename];
					elem = this->createinfo(compared);
					message += _T("\r\n");
					message += _T("�V�����t�@�C��\r\n");
					message += filename + _T("\r\n");
					message += _T("�T�C�Y: ") + this->punct(elem.size) + _T(" �o�C�g\r\n");
					message += _T("�X�V����: ") + elem.time.to_string(_T("%Y-%m-%d %H:%M:%S"));
					
					return cubeice::dialog::overwrite(message);
				}
			}
			return IDOK;
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
		
		/* ----------------------------------------------------------------- */
		//  filesize
		/* ----------------------------------------------------------------- */
		size_type filesize(const string_type& path) {
			HANDLE handle = CreateFile(path.c_str(), 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			DWORD dwSizeHigh = 0;
			DWORD dwSizeLow = GetFileSize(handle, &dwSizeHigh);
			CloseHandle(handle);
			return (static_cast<size_type>(dwSizeHigh) << sizeof(DWORD) * CHAR_BIT) | dwSizeLow;
		}

		/* ----------------------------------------------------------------- */
		//  filename
		/* ----------------------------------------------------------------- */
		string_type filename(const string_type& path) {
			string_type::size_type pos = path.find_last_of(_T('\\'));
			return (pos == string_type::npos) ? path : path.substr(pos + 1);
		}
	};
}

#endif // CUBE_ARCHIVER_H