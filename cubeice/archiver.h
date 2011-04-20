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
#include <boost/thread.hpp>
#include <boost/bind.hpp>
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
#define CUBEICE_MAXCOLUMN 50
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
			setting_(setting), input_files_(), files_(), size_(0), progress_() {}
		
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
			bool update;
			if (filetype == _T("detail")) {
				cubeice::runtime_setting runtime;
				ext = this->compress_extension(runtime.type(), first, last);
				dest = first->substr(0, first->find_last_of(_T('.'))) + ext;
				runtime.path() = dest;
				if (cubeice::dialog::runtime_setting(progress_.handle(), runtime) == IDCANCEL) return;
				
				// �����^�C�����̐ݒ�𔽉f����
				runtime.save();
				dest = runtime.path();
				filetype = runtime.type();
				update = runtime.update();
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
				if (pass && cubeice::dialog::password(progress_.handle(), COMPRESS_FLAG) == IDCANCEL) return;
			}
			
			// �ꎞ�t�@�C���̃p�X������
			string_type tmp = update ? dest : tmpfile(_T("cubeice"));
			if (tmp.empty()) return;
			
			progress_.style(cubeice::dialog::progressbar::simple);
			progress_.show();
			progress_.title(_T("0% - ") + this->filename(dest) + _T(" �����k���Ă��܂� - CubeICE"));
			progress_.marquee(true);
			
			// �v���O���X�o�[�̐ݒ�
			this->compress_prepare_filelist(first, last);
			boost::thread	thr( boost::bind( &cubeice::archiver::compress_filelist, this ) );
			HANDLE th = thr.native_handle();
			SetPriorityClass(th, ABOVE_NORMAL_PRIORITY_CLASS);
			if (progress_.is_marquee()) progress_.marquee(false);
			
			// �R�}���h���C���̍쐬
			std::basic_string<TCHAR> cmdline = CUBEICE_ENGINE;
			if( update )
				cmdline += _T(" u");
			else
				cmdline += _T(" a");
			if (filetype == _T("exe")) cmdline += _T(" -sfx7z.sfx");
			else if (ext == _T(".tgz") || ext == _T(".tbz") || ext.find(_T(".tar")) != string_type::npos) cmdline += _T(" -ttar");
			else cmdline += _T(" -t") + filetype;
			if (pass) cmdline += _T(" -p\"") + cubeice::password() + _T("\"");
			cmdline += _T(" -scsWIN -y \"") + tmp + _T("\"");
			for (InputIterator pos = first; pos != last; ++pos) cmdline += _T(" \"") + *pos + _T("\"");
			for(std::vector<string_type>::const_iterator pos = options.begin(); pos != options.end(); ++pos) cmdline += _T(' ') + *pos;
			cube::popen proc;
			if (!proc.open(cmdline.c_str(), _T("r"))) return;
			
			// NOTE: marquee �X�^�C�����畜�A����ہC�o�[�� 1% �܂Ői�܂Ȃ��ƍĕ`�悳��Ȃ��D
			// ���̂��߁C��u���� 1% �ɐi�߂ċ����I�ɍĕ`����s���Ă���D
			progress_.position(100);
			progress_.refresh();
			progress_.position(progress_.minimum());
			progress_.refresh();
			progress_.text(dest);
			
			// ���C������
			int status = 0;
			string_type line;
			string_type report;
			unsigned int index = 0;
			int percent = 0;
			progress_.start();
			while ((status = proc.gets(line)) >= 0) {
				if (!this->refresh(proc)) break;
				if (status == 2) break; // pipe closed
				else if (status == 0 || line.empty()) {
					Sleep(10);
					continue;
				}
				assert(status == 1);
				
				string_type::size_type pos = line.find(error);
				if (pos != string_type::npos) {
					report += clx::strip_copy(line.substr(pos));
					if (pos != 0) report += _T(" (") + clx::strip_copy(line.substr(0, pos)) + _T(")");
					report += _T("\r\n");
					continue;
				}
				
				pos = line.find(_T(":%")); // :%n% �ƌ����`�Ői�������\�������
				if (pos != string_type::npos) {
					string_type s = line.substr(pos);
					clx::strip_if(s, clx::is_any_of(_T(":% ")));
					line.erase(pos);
					int prev = percent;
					percent = clx::lexical_cast<int>(s) * 100;
					progress_.position(percent);
					if (percent > prev) {
						string_type title = s + _T("% - ") + this->filename(dest) + _T(" �����k���Ă��܂� - CubeICE");
						progress_.title(title);
					}
				}
				
				pos = line.find(keyword);
				if (pos == string_type::npos || line.size() <= keyword.size()) continue;
				string_type filename = clx::strip_copy(line.substr(pos + keyword.size()));
				if (filename.size() > CUBEICE_MAXCOLUMN) {
					string_type::size_type startpos = filename.size() - CUBEICE_MAXCOLUMN;
					filename = _T("...") + filename.substr(startpos);
				}
				progress_.text(filename);
				
				if (index < files_.size() - 1) ++index;
				progress_.numcount();
			}
			
			if (status < 0) report += error + _T(" Broken pipe.\r\n");
			if ((setting_.compression().details() & DETAIL_REPORT) && !report.empty()) {
				cubeice::dialog::report(progress_.handle(), report);
			}
			
			if (status == 2) {
				// *.tar �̏���
				if ((filetype == _T("gzip") || filetype == _T("bzip2")) &&
				    (ext.find(_T(".tar")) != string_type::npos || ext == _T(".tgz") || ext == _T(".tbz"))) {
					string_type prev = tmp;
					tmp = tmpfile(_T("cubeice"));
					tmp = tmp.substr(0, tmp.find_last_of(_T('\\'))+1);
					tmp += PathFindFileName(dest.c_str());
					this->compress_tar(prev, tmp, filetype, pass);
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
				
				if( !update )
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
			
			if (!update && PathFileExists(tmp.c_str())) DeleteFile(tmp.c_str());
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
			static const string_type password(_T("Enter password"));
			static const string_type password_error(_T("Wrong password?"));
			
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
				string_type original = src;
				string_type title_message = this->filename(original) + _T(" ���𓀂��Ă��܂� - CubeICE");
				string_type filetype;
				if (!this->decompress_filetype(src, filetype)) {
					string_type message = src + _T(" �͖��Ή��̃t�@�C���`���̂��߉𓀂ł��܂���B");
					MessageBox(NULL, message.c_str(), _T("CubeICE �𓀃G���["), MB_OK | MB_ICONERROR);
					return;
				}
				
				// �ۑ���p�X�̎擾
				string_type root = this->decompress_path(setting_.decompression(), src, force);
				if (root.empty()) break;
				
				progress_.show();
				progress_.title(_T("0% - ") + title_message);
				progress_.marquee(true);
				
				// �ꎞ�t�H���_�̍쐬
				string_type tmp = tmpdir(_T("cubeice"));
				if (tmp.empty()) break;
				
				// �v���O���X�o�[�̐i�s�x�̐ݒ�
				string_type folder = this->decompress_filelist(src);
				if (progress_.is_cancel()) break;
				if (this->size_ == 0) progress_.marquee(true);
				else if (progress_.is_marquee()) progress_.marquee(false);
				
				// *.tar �n�̏���
				// TODO: ���݁C�g���q���{���̎�ނƈقȂ�t�@�C�����Ώۂɂ��Ă��邪�C
				// �����̋U���i�H�j�g���q�̃t�@�C���� *.tar ���ǂ������ǂ����f���邩�D
				if ((filetype == _T("gzip") || filetype == _T("bzip2")) && this->is_tar(src)) {
					src = this->decompress_tar(src, tmp, report);
					progress_.position(progress_.minimum());
					progress_.subposition(progress_.minimum());
					progress_.title(_T("0% - ") + title_message);
					folder = this->decompress_filelist(src);
					if (src.empty() || !PathFileExists(src.c_str())) break;
					if (this->size_ == 0) progress_.marquee(true);
					else if (progress_.is_marquee()) progress_.marquee(false);
				}
				
				// �t�H���_�̍쐬
				if ((setting_.decompression().details() & DETAIL_CREATE_FOLDER)) {
					if ((setting_.decompression().details() & DETAIL_SINGLE_FOLDER) == 0 || folder.empty()) {
						root = this->decompress_dirname(setting_.decompression(), root, src);
					}
				}
				
				// �R�}���h���C���̐���
				string_type cmdline = decompress_cmdline(src, tmp, false);
				cube::popen proc;
				if (!proc.open(cmdline.c_str(), _T("r"))) return;
				
				// ���C������
				unsigned int index = 0;
				int status = 0;
				int to_all = 0; // �͂�/������/���l�[��
				string_type line;
				double calcpos = 0.0; // �v�Z��̃v���O���X�o�[�̈ʒu
				int percent = 0;
				
				// NOTE: marquee �X�^�C�����畜�A����ہC�o�[�� 1% �܂Ői�܂Ȃ��ƍĕ`�悳��Ȃ��D
				// ���̂��߁C��u���� 1% �ɐi�߂ċ����I�ɍĕ`����s���Ă���D
				if (this->size_ > 0) {
					progress_.position(100);
					progress_.subposition(100);
					progress_.refresh();
					progress_.position(progress_.minimum());
					progress_.subposition(progress_.minimum());
					progress_.refresh();
				}
				
				progress_.start();
				while ((status = proc.gets(line)) >= 0) {
					if (!progress_.is_marquee() && progress_.subposition() > progress_.maximum() - 1.0) {
						progress_.subposition(progress_.minimum());
					}
					if (!this->refresh(proc)) break;
					
					string_type message = (root.size() > CUBEICE_MAXCOLUMN) ? _T("...") + root.substr(root.size() - CUBEICE_MAXCOLUMN) : root;
					message += _T("\r\n");
					message += (files_[index].name.size() > CUBEICE_MAXCOLUMN) ?
						_T("...") + files_[index].name.substr(files_[index].name.size() - CUBEICE_MAXCOLUMN) :
						files_[index].name;
					progress_.text(message);
					
					if (status == 2) break; // pipe closed
					else if (status == 0 || line.empty()) {
						if (!this->refresh(proc)) break;
						
						// �v���O���X�o�[�̍X�V
						if (this->size_ > 0) {
							size_type fsize = filesize(tmp + _T("\\") + files_[index].name);
							if (!this->refresh(proc)) break;
							
							double tmppos = (this->size_ > 0 && fsize > 0) ?
								calcpos + (progress_.maximum() - progress_.minimum()) / (this->size_ / (double)fsize) :
								calcpos;
							if (tmppos > progress_.maximum()) tmppos = progress_.maximum();
							progress_.position(tmppos);
							if (!this->refresh(proc)) break;
							
							double subpos = (files_[index].size > 0 && fsize > 0) ?
								(progress_.maximum() - progress_.minimum()) / (files_[index].size / (double)fsize) :
								0.0;
							if (subpos > progress_.maximum()) subpos = progress_.maximum();
							progress_.subposition(subpos);
							if (!this->refresh(proc)) break;
							
							int prev = percent;
							percent = (tmppos > 1.0) ? static_cast<int>(tmppos / 100.0) : 0;
							if (percent > prev) {
								string_type title = clx::lexical_cast<string_type>(percent) + _T("% - ") + title_message;
								progress_.title(title);
							}
							if (!this->refresh(proc)) break;
						}
						Sleep(10);
						continue;
					}
					assert(status == 1);
					
					// �p�X���[�h����
					if (line.find(password) != string_type::npos || line.find(password_error) != string_type::npos) {
						proc.close();
						
						string_type remove_file = root + _T("\\") + files_[std::max(static_cast<int>(index - 1), 0)].name;
						if (PathFileExists(remove_file.c_str()) == TRUE && this->filesize(remove_file) == 0) {
							DeleteFile(remove_file.c_str());
						}
						if (cubeice::dialog::password(progress_.handle(), DECOMPRESS_FLAG) == IDCANCEL) break;
						cmdline = decompress_cmdline(src, tmp, true);
						if (!proc.open(cmdline.c_str(), _T("r"))) break;
						index = 0;
						status = 0;
						to_all = 0;
						calcpos = 0.0;
						if (this->size_ > 0) {
							progress_.position(progress_.minimum());
							progress_.subposition(progress_.minimum());
							progress_.title(_T("0% - ") + title_message);
						}
						continue;
					}
					
					string_type::size_type pos = line.find(error);
					if (pos != string_type::npos) {
						report += clx::strip_copy(line.substr(pos));
						if (pos != 0) report += _T(" (") + clx::strip_copy(line.substr(0, pos)) + _T(")");
						report += _T("\r\n");
						continue;
					}
					
					pos = line.find(keyword);
					if (pos == string_type::npos || line.size() <= keyword.size()) {
						continue;
					}
					string_type filename = clx::strip_copy(line.substr(pos + keyword.size()));
					
					// �v���O���X�o�[�̍X�V
					if (this->size_ > 0) {
						if (this->size_ > 0 && this->files_[index].size > 0) {
							calcpos += (progress_.maximum() - progress_.minimum()) / (this->size_ / (double)this->files_[index].size);
						}
						if (calcpos > progress_.maximum()) calcpos = progress_.maximum();
						progress_.position(calcpos);
						progress_.subposition(progress_.maximum());
						
						// �^�C�g���̍X�V
						int prev = percent;
						percent = (calcpos > 1.0) ? static_cast<int>(calcpos / 100.0) : 0;
						if (percent > prev) {
							string_type title = clx::lexical_cast<string_type>(percent) + _T("% - ") + title_message;
							progress_.title(title);
						}
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
					if ((setting_.decompression().details() & DETAIL_FILTER) && this->is_filter(filename, setting_.filters())) {
						// report += _T("Filtering: ") + filename + _T("\r\n");
					}
					else if (!this->move(tmp + _T('\\') + filename, root + _T('\\') + filename, result == IDRENAME)) {
						// TODO:
						// - move �����s����O�� MAX_PATH �̃`�F�b�N
						// - false �������ꍇ�� GetLastError() �ɑΉ����郁�b�Z�[�W���o�́i���s�𔲂��j
						report += error + _T(" Can not move file.");
						report += _T(" (") + keyword + _T(' ') + filename + _T(")\r\n");
					}
					
					if (index < files_.size() - 1) ++index;
					
					// �i�s�󋵂̍X�V
					progress_.numcount();
				}
				
				if (status < 0) report += error + _T(" Broken pipe.");
				if ((setting_.decompression().details() & DETAIL_REPORT) && !report.empty()) {
					cubeice::dialog::report(progress_.handle(), report);
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
						ShellExecute(NULL, _T("open"), _T("explorer.exe"), ( _T("\"") + root + _T("\"") ).c_str(), NULL, SW_SHOWNORMAL);
					}
				}
				
				this->remove(tmp.c_str());
				
				if ((setting_.decompression().details() & DETAIL_REMOVE_SRC) && report.empty() && !progress_.is_cancel()) {
					DeleteFile(original.c_str());
				}
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
		std::vector<string_type> input_files_;
		std::vector<fileinfo> files_;
		bool done_get_filelist_;
		size_type size_; // �g�[�^���T�C�Y
		cubeice::dialog::progressbar progress_;
		
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
				if (n > 1 || PathIsDirectory(first->c_str())) {
					ext = _T(".tar") + ext;
				}
				else if (filetype == _T("bzip2")) ext = first->substr(first->find_last_of('.')) + ext;
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
		//  compress_prepare_filelist
		/* ----------------------------------------------------------------- */
		template <class InputIterator>
		void compress_prepare_filelist(InputIterator first, InputIterator last) {
			for (; first != last; ++first) {
				input_files_.push_back(*first);
			}
		}
		
		/* ----------------------------------------------------------------- */
		//  compress_filelist
		/* ----------------------------------------------------------------- */
		void compress_filelist() {
			this->size_ = 0;
			this->files_.clear();
			
			for (std::vector<string_type>::const_iterator first = input_files_.begin(); first != input_files_.end(); ++first) {
				progress_.refresh();
				if (progress_.is_cancel()) return;
				while (progress_.is_suspend() && !progress_.is_cancel()) {
					progress_.refresh();
					Sleep(10);
				}
				
				bool directory = (PathIsDirectory(first->c_str()) != FALSE);
				
				// NOTE: ����C���t�@�C���������K�v�Ƃ��Ă��Ȃ��̂ō������̂��߂Ƀt�@�C�����̎擾���Ȃ��D
#ifdef CUBEICE_ORIGINAL
				fileinfo elem = this->createinfo(*first);
#else
				fileinfo elem;
				elem.name = *first;
				elem.size = 0;
				elem.directory = directory;
#endif
				this->files_.push_back(elem);
				this->size_ += elem.size;
				progress_.denomcount();
				if (directory) compress_filelist_folder(*first);
			}
			progress_.denomcount(0);
		}
		
		/* ----------------------------------------------------------------- */
		//  compress_filelist_folder
		/* ----------------------------------------------------------------- */
		void compress_filelist_folder(const string_type& root) {
			string_type path = root + _T("\\*.*");
			WIN32_FIND_DATA wfd = {};
			HANDLE handle = FindFirstFile(path.c_str(), &wfd);
			if (handle == INVALID_HANDLE_VALUE) return;
			
			int filenum = 0;
			do {
				progress_.refresh();
				if (progress_.is_cancel()) return;
				while (progress_.is_suspend() && !progress_.is_cancel()) {
					progress_.refresh();
					Sleep(10);
				}
				
				string_type filename(wfd.cFileName);
				if (filename != _T(".") && filename != _T("..")) {
					string_type s = root + _T('\\') + filename;
					bool directory = ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
					
					// NOTE: ����C���t�@�C���������K�v�Ƃ��Ă��Ȃ��̂ō������̂��߂Ƀt�@�C�����̎擾���Ȃ��D
#ifdef CUBEICE_ORIGINAL
					fileinfo elem = this->createinfo(s);
#else
					fileinfo elem;
					elem.name = s;
					elem.size = 0;
					elem.directory = directory;
#endif
					this->files_.push_back(elem);
					++filenum;
					progress_.denomcount();
					this->size_ += elem.size;
					if (directory) this->compress_filelist_folder(s);
				}
			} while (FindNextFile(handle, &wfd));
			FindClose(handle);
			//progress_.denomcount(filenum);
		}
		
		/* ----------------------------------------------------------------- */
		//  compress_tar
		/* ----------------------------------------------------------------- */
		void compress_tar(const string_type& src, const string_type& dest, const string_type& filetype, bool pass) {
			static const string_type keyword = _T("Compressing");
			
			string_type tar;
			string_type ext = dest.substr(dest.find_last_of(_T('.')));
			tar = dest.substr(0, dest.find_last_of(_T('.')));
			if (ext != _T(".gz") && ext != _T(".bz2")) tar += _T(".tar");
			if (!MoveFileEx(src.c_str(), tar.c_str(), MOVEFILE_COPY_ALLOWED)) return;
			
			std::basic_string<TCHAR> cmdline = CUBEICE_ENGINE;
			cmdline += _T(" a -t") + filetype;
			if (pass) cmdline += _T(" -p") + cubeice::password();
			cmdline += _T(" -scsWIN -y \"") + dest + _T("\"");
			cmdline += _T(" \"") + tar + _T("\"");
			cube::popen proc;
			if (!proc.open(cmdline.c_str(), _T("r"))) return;
			
			progress_.title(_T("0% - ") + this->filename(dest) + _T(" �����k���Ă��܂� - CubeICE"));
			progress_.position(progress_.minimum());
			
			int status = 0;
			string_type line;
			int percent = 0;
			while ((status = proc.gets(line)) >= 0) {
				if (!this->refresh(proc)) break;
				if (status == 2) break; // pipe closed
				else if (status == 0 || line.empty()) {
					Sleep(10);
					continue;
				}
				assert(status == 1);
				
				string_type::size_type pos = line.find(_T(":%")); // :%n% �ƌ����`�Ői�������\�������
				if (pos != string_type::npos) {
					string_type s = line.substr(pos);
					clx::strip_if(s, clx::is_any_of(_T(":% ")));
					line.erase(pos);
					int prev = percent;
					percent = clx::lexical_cast<int>(s) * 100;
					progress_.position(percent);
					if (percent > prev) {
						string_type title = s + _T("% - ") + this->filename(dest) + _T(" �����k���Ă��܂� - CubeICE");
						progress_.title(title);
					}
				}
				
				pos = line.find(keyword);
				if (pos == string_type::npos || line.size() <= keyword.size()) continue;
				string_type filename = clx::strip_copy(line.substr(pos + keyword.size()));
				if (filename.size() > CUBEICE_MAXCOLUMN) {
					string_type::size_type startpos = filename.size() - CUBEICE_MAXCOLUMN;
					filename = _T("...") + filename.substr(startpos);
				}
				progress_.text(filename);
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
		//  decompress_cmdline
		/* ----------------------------------------------------------------- */
		string_type decompress_cmdline(const string_type& src, const string_type& tmp, bool is_password) {
			string_type cmdline = CUBEICE_ENGINE;
			cmdline += _T(" x -bd -scsWIN -y");
			if (is_password) cmdline += _T(" -p\"") + cubeice::password() + _T("\"");
			cmdline += _T(" -o\"") + tmp + _T("\"");
			cmdline += _T(" \"") + src + _T("\"");
			return cmdline;
		}
		
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
		string_type decompress_filelist(const string_type& path) {
			string_type cmdline = CUBEICE_ENGINE;
			cmdline += _T(" l ");
			cmdline += _T("\"") + path + _T("\"");
			
			this->size_ = 0;
			this->files_.clear();
			string_type dest;
			
			cube::popen proc;
			if (!proc.open(cmdline.c_str(), _T("r"))) return dest;
			string_type buffer, src;
			int status = 0;
			bool single = true;
			while ((status = proc.gets(buffer)) >= 0) {
				progress_.refresh();
				if (progress_.is_cancel()) {
					proc.close();
					break;
				}
				if (progress_.is_suspend()) {
					proc.suspend();
					while (progress_.is_suspend() && !progress_.is_cancel()) {
						progress_.refresh();
						Sleep(5);
					}
					proc.resume();
					continue;
				}
				
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
					files_.push_back(elem);
					progress_.denomcount();
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
			progress_.denomcount(0);
			
			return dest;
		}
		
		/* ----------------------------------------------------------------- */
		//  decompress_tar
		/* ----------------------------------------------------------------- */
		string_type decompress_tar(const string_type& src, const string_type& root, string_type& report) {
			static const string_type keyword = _T("Extracting");
			static const string_type error = _T("ERROR:");
			
			string_type cmdline = CUBEICE_ENGINE;
			cmdline += _T(" x -bd -scsWIN -y");
			cmdline += _T(" -o\"") + root + _T("\"");
			cmdline += _T(" \"") + src + _T("\"");
			cube::popen proc;
			if (!proc.open(cmdline.c_str(), _T("r"))) return src;
			
			string_type message = progress_.text();
			if (!this->files_.empty()) {
				progress_.text(this->filename(this->files_[0].name) + _T(" �𒊏o���Ă��܂�..."));
			}
			
			// NOTE: marquee �X�^�C�����畜�A����ہC�o�[�� 1% �܂Ői�܂Ȃ��ƍĕ`�悳��Ȃ��D
			// ���̂��߁C��u���� 1% �ɐi�߂ċ����I�ɍĕ`����s���Ă���D
			string_type title_message = this->filename(src) + _T(" ���𓀂��Ă��܂� - CubeICE");
			if (this->size_ > 0) {
				progress_.position(100);
				progress_.subposition(100);
				progress_.refresh();
				progress_.position(progress_.minimum());
				progress_.subposition(progress_.minimum());
				progress_.refresh();
			}
			progress_.title(_T("0% - ") + title_message);
			
			int status = 0;
			int percent = 0;
			string_type filename;
			string_type line;
			while ((status = proc.gets(line)) >= 0) {
				if (!this->refresh(proc)) break;
				if (status == 2) break; // pipe closed
				else if (status == 0 || line.empty()) {
					if (!this->refresh(proc)) break;
					
					// �v���O���X�o�[�̍X�V
					if (this->size_ > 0 && !this->files_.empty()) {
						size_type fsize = filesize(root + _T("\\") + this->files_[0].name);
						if (!this->refresh(proc)) break;
						
						double tmppos = (this->size_ > 0 && fsize > 0) ? (progress_.maximum() - progress_.minimum()) / (this->size_ / (double)fsize) : 0;
						if (tmppos > progress_.maximum()) tmppos = progress_.maximum();
						progress_.position(tmppos);
						progress_.subposition(tmppos);
						if (!this->refresh(proc)) break;
						
						int prev = percent;
						percent = (tmppos > 1.0) ? static_cast<int>(tmppos / 100.0) : 0;
						if (percent > prev) {
							string_type title = clx::lexical_cast<string_type>(percent) + _T("% - ") + title_message;
							progress_.title(title);
						}
						if (!this->refresh(proc)) break;
					}
					Sleep(10);
					continue;
				}
				assert(status == 1);
				
				string_type::size_type pos = line.find(error);
				if (pos != string_type::npos) {
					report += clx::strip_copy(line.substr(pos));
					if (pos != 0) report += _T(" (") + clx::strip_copy(line.substr(0, pos)) + _T(")");
					report += _T("\r\n");
					continue;
				}
				
				pos = line.find(keyword);
				if (pos == string_type::npos || line.size() <= keyword.size()) continue;
				filename = clx::strip_copy(line.substr(pos + keyword.size()));
			}
			
			progress_.text(message);
			return root + _T('\\') + filename;
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
		 *   - mydocuments: �}�C�h�L�������g
		 */
		/* ----------------------------------------------------------------- */
		string_type rootdir(const setting_type::archive_type& setting, const string_type& src, const string_type& force = _T("")) {
			// �f�X�N�g�b�v�̃p�X���擾���Ă����D
			LPITEMIDLIST item;
			SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &item);
			char_type buffer[CUBE_MAX_PATH] = {};
			SHGetPathFromIDList(item, buffer);
			string_type desktop = buffer;
			// �}�C�h�L�������g�̃p�X���擾���Ă���
			SHGetSpecialFolderLocation(NULL, CSIDL_MYDOCUMENTS, &item);
			SHGetPathFromIDList(item, buffer);
			string_type mydocuments = buffer;
			
			if (force == _T("runtime")) return string_type();
			if (force == _T("desktop")) return desktop;
			if (force == _T("mydocuments")) return mydocuments;
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
				// dest ����f�B���N�g�������𒊏o Path.GetDirectoryName(dest);
				// TODO: ���̕��������������ꍇ������̂Œ����D
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
					
					return cubeice::dialog::overwrite(progress_.handle(), message);
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
			if (ext == _T(".tgz") || ext == _T(".tbz")) return true;

			if (pos==0) return false;
			pos = path.find_last_of(_T('.'), pos-1);
			if (pos == string_type::npos) return false;
			ext = path.substr(pos);
			if (ext == _T(".tar.gz") || ext == _T(".tar.bz2")) return true;
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
			return ( dwSizeLow == -1 ) ? 0 : ( (static_cast<size_type>(dwSizeHigh) << sizeof(DWORD) * CHAR_BIT) | dwSizeLow );
		}

		/* ----------------------------------------------------------------- */
		//  filename
		/* ----------------------------------------------------------------- */
		string_type filename(const string_type& path) {
			string_type::size_type pos = path.find_last_of(_T('\\'));
			return (pos == string_type::npos) ? path : path.substr(pos + 1);
		}

		/* ----------------------------------------------------------------- */
		//  refresh
		/* ----------------------------------------------------------------- */
		bool refresh(cube::popen& proc) {
			progress_.refresh();
			if (progress_.is_cancel()) {
				proc.close();
				return false;
			}
			
			if (progress_.is_suspend()) {
				proc.suspend();
				while (progress_.is_suspend() && !progress_.is_cancel()) {
					progress_.refresh();
					Sleep(10);
				}
				proc.resume();
			}
			return true;
		}
	};
}

#endif // CUBE_ARCHIVER_H