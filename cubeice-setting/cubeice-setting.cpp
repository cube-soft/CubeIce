#include <tchar.h>
#include <windows.h>
#include <commctrl.h>
#include <string>
#include "cubeice-setting.h"
#include "dialog.h"
#include "style.h"

cubeice::user_setting Setting;

int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPTSTR pCmdLine, int showCmd) {
	std::basic_string<TCHAR> args(pCmdLine);
	if (args == _T("install")) {
		/*
		 * �����ݒ�
		 *   1. �ȉ��̃t�@�C���Ɋ֘A�t��:
		 *      *.zip, *.lzh, *.rar, *.tar, *.gz, *.7z, *.bz2, *.cab, *.tbz, *.tgz, *.xz
		 *   2. �ȉ��̃R���e�L�X�g���j���[���쐬
		 *      ���k/��
		 *   3. �ȉ��̃V���[�g�J�b�g���쐬:
		 *      ���k/��
		 */
		Setting.decompression().flags() |= (
			ZIP_FLAG | LZH_FLAG | RAR_FLAG | TAR_FLAG | GZ_FLAG | SEVENZIP_FLAG |
			BZ2_FLAG | CAB_FLAG | TBZ_FLAG | TGZ_FLAG | XZ_FLAG
		);
		
		Setting.context_flags() |= (COMPRESS_FLAG | DECOMPRESS_FLAG);
		Setting.shortcut_flags() |= (COMPRESS_FLAG | DECOMPRESS_FLAG);
	}
	
	cubeice::create_propsheet(NULL, args == _T("install"));
	
	MSG  msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return 0;
}
