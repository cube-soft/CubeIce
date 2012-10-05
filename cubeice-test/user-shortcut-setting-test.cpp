// -*- coding: shift-jis -*-
/* ------------------------------------------------------------------------- */
/*
 *  user-shortcut-setting-test.cpp
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
#include <boost/test/unit_test.hpp>
#include <cubeice/user-setting.h>
#include "cubeice-test.h"

BOOST_AUTO_TEST_SUITE(UserShortcutSettingTest)

/* ------------------------------------------------------------------------- */
///
/// TestDefaultValue
///
/// <summary>
/// �e��ݒ�̏����l�̃e�X�g�B
/// </summary>
///
/* ------------------------------------------------------------------------- */
BOOST_AUTO_TEST_CASE(TestDefaultValue) {
	try {
		CubeICE::UserSetting root;
		CubeICE::UserShortcutSetting& setting = root.Shortcut();
		BOOST_CHECK(setting.Parameters().empty());
		BOOST_CHECK(setting.Items().empty());
	}
	catch (std::exception& err) {
		BOOST_FAIL(err.what());
	}
}

/* ------------------------------------------------------------------------- */
///
/// TestAddAndDelete
///
/// <summary>
/// �V���[�g�J�b�g�̒ǉ��A����э폜�����̃e�X�g�B
/// </summary>
///
/* ------------------------------------------------------------------------- */
BOOST_AUTO_TEST_CASE(TestAddAndDelete) {
	try {
		UserSettingSaver saver;
		
		CubeICE::UserSetting setting;
		setting.Load();
		
		CubeICE::string_type name = _T("CubeICE �V���[�g�J�b�g�e�X�g");
		CubeICE::string_type directory = PsdotNet::Environment::GetFolderPath(PsdotNet::Environment::SpecialFolder::DesktopDirectory);
		CubeICE::string_type path = directory + _T("\\") + name + _T(".lnk");
		CubeICE::string_type target = setting.InstallDirectory() + _T("\\") + CUBEICE_NAME;
		CubeICE::string_type icon = setting.InstallDirectory() + _T("\\") + CUBEICE_NAME + _T(",1");
		CubeICE::Shortcut newitem(name, directory, target, _T("/c:zip"), icon);
		
		setting.Shortcut().Add(newitem);
		setting.Save();
		BOOST_CHECK(PsdotNet::IO::File::Exists(path) == true);
		
		setting.Shortcut().Delete(name);
		setting.Save();
		BOOST_CHECK(PsdotNet::IO::File::Exists(path) == false);
		
		// �폜�����V���[�g�J�b�g�ēx�o�^����e�X�g
		setting.Shortcut().Add(newitem);
		setting.Save();
		BOOST_CHECK(PsdotNet::IO::File::Exists(path) == true);
		
		setting.Shortcut().Delete(name);
		setting.Save();
		BOOST_CHECK(PsdotNet::IO::File::Exists(path) == false);
	}
	catch (const std::exception& err) {
		BOOST_FAIL(err.what());
	}
}

BOOST_AUTO_TEST_SUITE_END()
