#ifndef CUBE_ARCHIVER_H
#define CUBE_ARCHIVER_H

#include <string>
#include <vector>
#include <tchar.h>
#include <clx/split.h>

#define CUBE_ICE_ENGINE _T("7z.exe")

namespace cube {
	class archiver {
	public:
		archiver() {}

		/* ----------------------------------------------------------------- */
		//  compress
		/* ----------------------------------------------------------------- */
		template <class InputIterator>
		void compress(InputIterator first, InputIterator last) {
			const TCHAR filter[] = _T("All files(*.*)\0*.*\0\0");
			std::basic_string<TCHAR> dest = cube::dialog::savefile(filter);
			std::basic_string<TCHAR> cmdline = CUBE_ICE_ENGINE;
			cmdline += _T(" a -tzip -bd -scsWIN -y \"") + dest + _T("\"");
			while (++first != last) cmdline += _T(" \"") + *first + _T("\"");
			this->execute(cmdline);
		}

		/* ----------------------------------------------------------------- */
		//  decompress
		/* ----------------------------------------------------------------- */
		template <class InputIterator>
		void decompress(InputIterator first, InputIterator last) {
			while (++first != last) {
				std::basic_string<TCHAR>::size_type pos = first->find_last_of('\\');
				std::basic_string<TCHAR> filename = (pos == std::basic_string<TCHAR>::npos) ? *first : first->substr(pos);
				std::basic_string<TCHAR> dest = cube::dialog::browsefolder(_T("�𓀂���t�H���_���w�肵�ĉ������B"));
				dest += _T("\\") + filename.substr(0, filename.find_last_of(_T('.')));
				std::basic_string<TCHAR> cmdline = CUBE_ICE_ENGINE;
				cmdline += _T(" x -bd -scsWIN -y -o\"") + dest + _T("\"");
				cmdline += _T(" \"") + *first + _T("\"");
				this->execute(cmdline);
			}
		}
		
	private:
		/* ----------------------------------------------------------------- */
		/*
		 *  execute
		 *
		 *  MEMO: CreateProcess �ɓn���R�}���h���C��������͕ύX�����
		 *  �\�������邽�߁C��������z�� (vector) �ɃR�s�[���ēn���Ă���D
		 */
		/* ----------------------------------------------------------------- */
		void execute(const std::basic_string<TCHAR>& cmdline) {
			PROCESS_INFORMATION pi = {};
			
#if 0
			// MEMO: �����������������s���������Cpopen ���g�p����� DOS �����J���Ă��܂��D
			// TODO: popen �����̊֐��� CreateProcess �� CreatePipe ��p���č쐬����D
			// see also: http://support.microsoft.com/kb/190351/ja
			cube::dialog::progressbar progress(app_);

			FILE* fd = _tpopen(cmdline.c_str(), _T("r"));
			TCHAR buffer[2048] = {};
			while (_fgetts(buffer, 2048, fd)) {
				progress.text(buffer);
				progress += 10; // TODO: (1 / ���t�@�C����) �����i�߂�D
				Sleep(50);
			}
#else
			// MEMO: ���݂́C��������t�@�C���ɓf���o���Č��ʂ�񋓂��Ă���D
			// TODO: �쐬�����v���Z�X�̕W���o�͂�W�����͂Ƀ��_�C���N�g���C
			// ���̓��e����s���\������D
			SECURITY_ATTRIBUTES sa;
			sa.nLength= sizeof(SECURITY_ATTRIBUTES);
			sa.lpSecurityDescriptor = NULL;
			sa.bInheritHandle = TRUE;
			HANDLE handle = CreateFile( _T("tmp.txt"), GENERIC_WRITE, FILE_SHARE_WRITE, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
			
			STARTUPINFO si = {};
			//GetStartupInfo(&si);
			si.dwFlags = STARTF_USESTDHANDLES;
			si.hStdOutput = handle;
			
			// \0 �ŏI��� const �łȂ� char �z��
			std::vector<TCHAR> tmp(cmdline.begin(), cmdline.end());
			tmp.push_back(0);
			
			//MessageBox(NULL, cmdline.c_str(), _T("cmdline"), MB_OK);
	
			CreateProcess(
				NULL, // ���s�\���W���[��
				&tmp.at(0), // �R�}���h���C��
				NULL,
				NULL,
				TRUE,
				CREATE_NO_WINDOW | DETACHED_PROCESS, // �쐬�̃t���O
				NULL,
				NULL, // �J�����g�f�B���N�g��
				&si, &pi
			);
			
			WaitForSingleObject(pi.hProcess, INFINITE);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			CloseHandle(handle);
			
			// 1�s���\������T���v���D
			handle = CreateFile( _T("tmp.txt"), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
			std::basic_string<TCHAR> src;
			TCHAR buffer[2048] = {};
			DWORD n = 0;
			while (ReadFile(handle, buffer, 2048, &n, NULL) && n > 0) src += buffer;
			std::vector<std::basic_string<TCHAR> > v;
			clx::split_if(src, v, clx::is_any_of(_T("\r\n")));
			
			// progressbar: �v���O���X�o�[��\�����邽�߂̃N���X�D
			// ++���Z�q��1�i�ށDtext() �Ɉ������w�肷��ƁC�w�肳�ꂽ���e���\�������D
			cube::dialog::progressbar progress;
			for (std::vector<std::basic_string<TCHAR> >::iterator pos = v.begin(); pos != v.end(); ++pos) {
				progress.text(*pos);
				progress += 5; // TODO: (1 / ���t�@�C����) �����i�߂�D
				Sleep(10);
			}
			CloseHandle(handle);
			DeleteFile( _T("tmp.txt"));
#endif
		}

	private:
		/* ----------------------------------------------------------------- */
		//  non-copyable
		/* ----------------------------------------------------------------- */
		archiver(const archiver& cp);
		archiver& operator=(const archiver& cp);
	};
}

#endif // CUBE_ARCHIVER_H