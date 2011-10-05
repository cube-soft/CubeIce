/* ------------------------------------------------------------------------- */
/*
 *  test-io.h
 *
 *  Copyright (c) 2010 - 2011 CubeSoft Inc.
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
#ifndef CUBEICE_TEST_IO_H
#define CUBEICE_TEST_IO_H

#include <windows.h>
#include <tchar.h>
#include <string>
#include "io.h"
#include <boost/test/minimal.hpp>

/* ------------------------------------------------------------------------- */
/*
 *  test_directory
 *
 *  �f�B���N�g���쐬�E�폜�̃e�X�g�D
 */
/* ------------------------------------------------------------------------- */
inline void test_directory() {
	TCHAR buffer[MAX_PATH] = {};
	BOOST_CHECK(GetCurrentDirectory(sizeof(buffer) / sizeof(TCHAR), buffer) > 0);
	
	std::basic_string<TCHAR> root = buffer;
	std::basic_string<TCHAR> dir;
	
	// TestCase1: �ʏ�P�[�X
	// �J�����g�p�X���ɋ�̃f�B���N�g�����쐬���C������폜����D
	dir = root + _T("\\cubeice-test-createdir");
	BOOST_CHECK(cubeice::createdir(dir));
	BOOST_CHECK(PathIsDirectory(dir.c_str()));
	BOOST_CHECK(cubeice::removedir(dir));
	
	// TestCase2: �ċA�I�Ƀf�B���N�g�����쐬���C�폜����D
	dir = root + _T("\\cubeice-test-createdir");
	std::basic_string<TCHAR> subdir = dir + _T("\\subdir");
	BOOST_CHECK(cubeice::createdir(subdir));
	BOOST_CHECK(PathIsDirectory(dir.c_str()));
	BOOST_CHECK(PathIsDirectory(subdir.c_str()));
	BOOST_CHECK(cubeice::removedir(dir));
	
}

/* ------------------------------------------------------------------------- */
/*
 *  test_tmpfile
 *
 *  �ꎞ�p�X�擾�̃e�X�g�D
 */
/* ------------------------------------------------------------------------- */
inline void test_tmpfile() {
	TCHAR buffer[MAX_PATH] = {};
	BOOST_CHECK(GetCurrentDirectory(sizeof(buffer) / sizeof(TCHAR), buffer) > 0);

	std::basic_string<TCHAR> root = buffer;
	std::basic_string<TCHAR> prefix = _T("cubeice");
	std::basic_string<TCHAR> path;

	// TestCase1: �e�f�B���N�g�����w�肷��D�ꎞ�t�@�C���͎��ۂɂ͍쐬����Ȃ��D
	path = cubeice::tmpfile(root, prefix, false);
	BOOST_CHECK(!path.empty());
	BOOST_CHECK(!::PathFileExists(path.c_str())); // tmpfile() �֐��͎��ۂɃt�@�C���͍쐬���Ȃ��D

	// TestCase2: �e�f�B���N�g�����w�肷��D�ꎞ�t�@�C�������ۂɍ쐬�����D
	path = cubeice::tmpfile(root, prefix, true);
	BOOST_CHECK(!path.empty());
	BOOST_CHECK(::PathFileExists(path.c_str()));
	BOOST_CHECK(::DeleteFile(path.c_str()) != FALSE);
	BOOST_CHECK(!::PathFileExists(path.c_str()));

	// TestCase3: �e�f�B���N�g�����w�肵�Ȃ��D�ꎞ�t�@�C���͎��ۂɂ͍쐬����Ȃ��D
	path = cubeice::tmpfile(prefix, false);
	BOOST_CHECK(!path.empty());
	BOOST_CHECK(!::PathFileExists(path.c_str()));

	// TestCase4: �e�f�B���N�g�����w�肵�Ȃ��D�ꎞ�t�@�C�������ۂɍ쐬�����D
	path = cubeice::tmpfile(prefix, true);
	BOOST_CHECK(!path.empty());
	BOOST_CHECK(::PathFileExists(path.c_str()));
	BOOST_CHECK(::DeleteFile(path.c_str()) != FALSE);
	BOOST_CHECK(!::PathFileExists(path.c_str()));
}

/* ------------------------------------------------------------------------- */
/*
 *  test_tmpdir
 *
 *  �ꎞ�f�B���N�g���擾�̃e�X�g�D
 */
/* ------------------------------------------------------------------------- */
inline void test_tmpdir() {
	TCHAR buffer[MAX_PATH] = {};
	BOOST_CHECK(GetCurrentDirectory(sizeof(buffer) / sizeof(TCHAR), buffer) > 0);

	std::basic_string<TCHAR> root = buffer;
	std::basic_string<TCHAR> prefix = _T("cubeice");
	std::basic_string<TCHAR> path;

	// TestCase1: �e�f�B���N�g�����w�肷��D
	path = cubeice::tmpdir(root, prefix);
	BOOST_CHECK(!path.empty());
	BOOST_CHECK(::PathIsDirectory(path.c_str()));
	BOOST_CHECK(cubeice::removedir(path));
	BOOST_CHECK(!::PathIsDirectory(path.c_str()));

	// TestCase2: �e�f�B���N�g�����w�肵�Ȃ��D
	path = cubeice::tmpdir(prefix);
	BOOST_CHECK(!path.empty());
	BOOST_CHECK(::PathIsDirectory(path.c_str()));
	BOOST_CHECK(cubeice::removedir(path));
	BOOST_CHECK(!::PathIsDirectory(path.c_str()));
}

#endif // CUBEICE_TEST_IO_H
