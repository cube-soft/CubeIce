// -*- coding: shift-jis -*-
/* ------------------------------------------------------------------------- */
/*
 *  cubeice-setting/cubeice-setting.cpp
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
#include "cubeice-setting.h"
#include <psdotnet/environment.h>
#include "setting-dialog.h"

int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPTSTR pCmdLine, int showCmd) {
	CubeICE::UserSetting setting;
	setting.Load();
	
#ifndef PSDOTNET_INVALIDATE_LOG_MACRO
	CubeICE::string_type path = setting.InstallDirectory() + _T("\\cubeice-setting.log");
	PsdotNet::FileAppender writer(path, PsdotNet::FileAppender::CloseOnWrite | PsdotNet::FileAppender::WriteDateTime | PsdotNet::FileAppender::WriteLevel);
	PsdotNet::Logger::Configure(writer, PsdotNet::Utility::ToLogLevel(setting.LogLevel()));
#endif
	
	LOG_INFO(_T("CubeICE version %s (x%d)"), setting.Version().c_str(), (PsdotNet::Environment::Is64BitProcess() ? 64 : 86));
	LOG_INFO(PsdotNet::Environment::OSVersion().VersionString().c_str());
	LOG_INFO(_T("InstallDirectory: %s"), setting.InstallDirectory().c_str());
	
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
	}
	
	CubeICE::SettingDialog dialog(setting);
	dialog.ShowDialog();
	
	return 0;
}
