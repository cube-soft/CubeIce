#ifndef CUBE_ARCHIVER_H
#define CUBE_ARCHIVER_H

#include <string>
#include <vector>
#include <tchar.h>
#include <shlwapi.h>
#include <clx/strip.h>
#include <clx/split.h>
#include "wpopen.h"
#include "user-setting.h"

#define CUBEICE_ENGINE _T("7z.exe")

namespace cubeice {
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
			// �ۑ���p�X�̌���
			string_type dest =  this->rootdir(setting_.decompression(), *first);
			if (dest.empty()) {
				const TCHAR filter[] = _T("All files(*.*)\0*.*\0\0");
				dest = cubeice::dialog::savefile(filter);
			}
			else dest += _T('\\') + this->compress_name(*first, _T(".zip")); // .zip �͎b��
			if (dest.empty()) return;
			
			// �㏑���̊m�F
			if ((setting_.compression().details() & DETAIL_OVERWRITE) &&
				(setting_.compression().details() & OUTPUT_RUNTIME) == 0 &&
				PathFileExists(dest.c_str())) {
				string_type message = dest + _T("\r\n���͊��ɑ��݂��܂��B�㏑�����܂����H");
				if (MessageBox(NULL, message.c_str(), _T("CubeICE"), MB_YESNO) == IDNO) return;
			}
			
			// �R�}���h���C���̍쐬
			std::basic_string<TCHAR> cmdline = CUBEICE_ENGINE;
			cmdline += _T(" a -tzip -bd -scsWIN -y \"") + dest + _T("\"");
			while (++first != last) cmdline += _T(" \"") + *first + _T("\"");
			
			cubeice::dialog::progressbar progress;
			progress.marquee(true); // �b��
			
			cube::popen proc;
			if (!proc.open(cmdline.c_str(), _T("r"))) return;
			
			// 1�t�@�C���ӂ�̃v���O���X�o�[�̐i�s��
			//size_type step = (n > 0) ? (progress.maximum() - progress.minimum()) / n : 1;
			double step = 0.0;
			
			int status = 0;
			string_type buffer;
			while ((status = proc.gets(buffer)) >= 0) {
				if (status == 2) break; // pipe closed
				else if (status == 1 && !buffer.empty()) {
					progress.text(buffer);
					if (step > 0) progress += step;
				}
				buffer.clear();
				
				progress.refresh();
				if (progress.is_cancel()) {
					proc.close();
					break;
				}
				Sleep(10);
			}
			
			// �t�H���_���J��
			if (setting_.compression().details() & DETAIL_OPEN) {
				string_type root = dest.substr(0, dest.find_last_of(_T('\\')));
				ShellExecute(NULL, _T("open"), root.c_str(), NULL, NULL, SW_SHOWNORMAL);
			}
		}
		
		/* ----------------------------------------------------------------- */
		//  decompress
		/* ----------------------------------------------------------------- */
		template <class InputIterator>
		void decompress(InputIterator first, InputIterator last) {
			static const string_type keyword = _T("Extracting");
			
			while (++first != last) {
				// �ۑ���p�X�̌���
				string_type root = this->rootdir(setting_.decompression(), *first);
				if (root.empty()) root = cubeice::dialog::browsefolder(_T("�𓀂���t�H���_���w�肵�ĉ������B"));
				if (root.empty()) break;
				
				// �t�H���_�̍쐬
				if (setting_.decompression().details() & DETAIL_CREATE_FOLDER) {
					string_type::size_type pos = first->find_last_of('\\');
					string_type filename = (pos == string_type::npos) ? *first : first->substr(pos);
					root += _T("\\") + filename.substr(0, filename.find_last_of(_T('.')));
				}
				
				// TODO: �㏑���̊m�F
				
				// �R�}���h���C���̐���
				string_type cmdline = CUBEICE_ENGINE;
				cmdline += _T(" x -bd -scsWIN -y -o\"") + root + _T("\"");
				cmdline += _T(" \"") + *first + _T("\"");
				
				cubeice::dialog::progressbar progress;
				progress.text(root);
				size_type n = this->size(*first);
				double step = (n > 0) ? (progress.maximum() - progress.minimum()) / static_cast<double>(n) : 0.0;
				if (n == 0) progress.marquee(true);
				
				cube::popen proc;
				if (!proc.open(cmdline.c_str(), _T("r"))) return;
				int status = 0;
				string_type buffer;
				while ((status = proc.gets(buffer)) >= 0) {
					if (status == 2) break; // pipe closed
					else if (status == 1 && !buffer.empty()) {
						string_type::size_type pos = buffer.find(keyword);
						if (pos != string_type::npos && buffer.size() > keyword.size()) {
							string_type message = root + _T("\n") + clx::strip_copy(buffer.substr(pos + keyword.size()));
							progress.text(message);
							if (n > 0) progress += step;
						}
					}
					buffer.clear();
					
					progress.refresh();
					if (progress.is_cancel()) {
						proc.close();
						break;
					}
					Sleep(10);
				}
				
				// �t�H���_���J��
				if (setting_.decompression().details() & DETAIL_OPEN) {
					ShellExecute(NULL, _T("open"), root.c_str(), NULL, NULL, SW_SHOWNORMAL);
				}
			}
		}
		
	private:
		const setting_type& setting_;
		
		/* ----------------------------------------------------------------- */
		//  non-copyable
		/* ----------------------------------------------------------------- */
		archiver(const archiver& cp);
		archiver& operator=(const archiver& cp);
		
		/* ----------------------------------------------------------------- */
		/*
		 *  rootdir
		 *
		 *  ���[�U�ݒ�̒l�����ɏo�͐�f�B���N�g�����擾����D�����
		 *  �f�B���N�g�����w�肪�`�F�b�N����Ă���C���o�͐悪
		 *  �󔒂̏ꍇ�̓f�X�N�g�b�v�̃p�X��Ԃ��D
		 */
		/* ----------------------------------------------------------------- */
		string_type rootdir(const setting_type::archive_type& setting, const string_type& src) {
			string_type dest;
			if (setting.output_condition() == OUTPUT_SPECIFIC) {
				if (!setting.output_path().empty()) return setting.output_path();
				else { // �f�X�N�g�b�v�̃p�X���擾
					LPITEMIDLIST item;
					SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &item);
					
					char_type buffer[CUBE_MAX_PATH] = {};
					SHGetPathFromIDList(item, buffer);
					return string_type(buffer);
				}
			}
			else if (setting.output_condition() == OUTPUT_SOURCE) {
				// TODO: ���΃p�X�̏ꍇ�ǂ����邩�D
				return src.substr(0, src.find_last_of(_T('\\')));
			}
			
			return string_type();
		}
		
		/* ----------------------------------------------------------------- */
		//  compress_name
		/* ----------------------------------------------------------------- */
		string_type compress_name(const string_type& src, const string_type& ext) {
			string_type::size_type first = src.find_last_of(_T('\\'));
			if (first == string_type::npos) first = 0;
			else ++first;
			return src.substr(first, src.find_last_of(_T('.'))) + ext;
		}

		/* ----------------------------------------------------------------- */
		//  size
		/* ----------------------------------------------------------------- */
		size_type size(const string_type& path) {
			string_type cmdline = CUBEICE_ENGINE;
			cmdline += _T(" l ");
			cmdline += path;
			
			cube::popen proc;
			if (!proc.open(cmdline.c_str(), _T("r"))) return 0;
			string_type buffer, src;
			int status = 0;
			while ((status = proc.gets(buffer)) >= 0) {
				if (status == 2) break; // pipe closed
				else if (status == 1 && !buffer.empty()) src = buffer;
				buffer.clear();
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
	};
}

#endif // CUBE_ARCHIVER_H