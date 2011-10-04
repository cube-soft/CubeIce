/* ------------------------------------------------------------------------- */
/*
 *  test-compressor.h
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
#ifndef CUBEICE_TEST_COMPRESSOR_H
#define CUBEICE_TEST_COMPRESSOR_H

#include <windows.h>
#include <tchar.h>
#include <string>
#include <vector>
#include "compressor.h"
#include <boost/test/minimal.hpp>

/* ------------------------------------------------------------------------- */
/*
 *  test_compressor_extension
 *
 *  �t�@�C����ނƓ��̓t�@�C���Q����g���q�C����� TAR �������K�v���ǂ���
 *  �𔻒肷��֐��̃e�X�g�D
 */
/* ------------------------------------------------------------------------- */
void test_compressor_extension() {
	std::vector<std::basic_string<TCHAR> > v;
	std::basic_string<TCHAR> filetype;
	bool optar;
	
	// TestCase1: TAR �֘A�ȊO�̃t�@�C���Q
	filetype = _T("7z");
	optar = false;
	v.clear();
	v.push_back(_T("foo.txt"));
	v.push_back(_T("bar\\bas.txt"));
	v.push_back(_T("�e�X�g�t�@�C��.dat"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".7z"));
	BOOST_CHECK(optar == false);
	
	// TestCase2: �P��t�@�C���� gzip �Ɉ��k
	filetype = _T("gzip");
	optar = false;
	v.clear();
	v.push_back(_T("�e�X�g�t�@�C��.txt"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".gz"));
	BOOST_CHECK(optar == false);
	
	// TestCase3: �P��t�@�C���� tgz �Ɉ��k
	filetype = _T("tgz");
	optar = false;
	v.clear();
	v.push_back(_T("�e�X�g�t�@�C��.txt"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".tgz"));
	BOOST_CHECK(optar == true);
	
	// TestCase4: �P��t�@�C���� bzip2 �Ɉ��k (*.xxx.bz2 �ƌ����g���q�ɂȂ�)
	filetype = _T("bzip2");
	optar = false;
	v.clear();
	v.push_back(_T("�e�X�g�t�@�C��.txt"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".txt.bz2"));
	BOOST_CHECK(optar == false);

	// TestCase5: �P��t�@�C���� tbz �Ɉ��k
	filetype = _T("tbz");
	optar = false;
	v.clear();
	v.push_back(_T("�e�X�g�t�@�C��.txt"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".tbz"));
	BOOST_CHECK(optar == true);

	
	// TestCase6: �����t�@�C�� (�f�B���N�g���܂܂�) �� gzip �Ɉ��k
	filetype = _T("gzip");
	optar = false;
	v.clear();
	v.push_back(_T("foo.txt"));
	v.push_back(_T("bar.txt"));
	v.push_back(_T("�e�X�g�t�@�C��.dat"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".tar.gz"));
	BOOST_CHECK(optar == true);
	
	// TestCase7: �����t�@�C�� (�f�B���N�g���܂܂�) �� tgz �Ɉ��k
	filetype = _T("tgz");
	optar = false;
	v.clear();
	v.push_back(_T("foo.txt"));
	v.push_back(_T("bar.txt"));
	v.push_back(_T("�e�X�g�t�@�C��.dat"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".tgz"));
	BOOST_CHECK(optar == true);
	
	// TestCase8: �����t�@�C�� (�f�B���N�g���܂܂�) �� bzip2 �Ɉ��k
	filetype = _T("bzip2");
	optar = false;
	v.clear();
	v.push_back(_T("foo.txt"));
	v.push_back(_T("bar.txt"));
	v.push_back(_T("�e�X�g�t�@�C��.dat"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".tar.bz2"));
	BOOST_CHECK(optar == true);
	
	// TestCase9: �����t�@�C�� (�f�B���N�g���܂܂�) �� tbz �Ɉ��k
	filetype = _T("tbz");
	optar = false;
	v.clear();
	v.push_back(_T("foo.txt"));
	v.push_back(_T("bar.txt"));
	v.push_back(_T("�e�X�g�t�@�C��.dat"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".tbz"));
	BOOST_CHECK(optar == true);
	
	// TestCase10: �����t�@�C�� (�t�@�C�� + �f�B���N�g��) �� gzip �Ɉ��k
	filetype = _T("gzip");
	optar = false;
	v.clear();
	v.push_back(_T("foo.txt"));
	v.push_back(_T("bar"));
	v.push_back(_T("bar\\bas.txt"));
	v.push_back(_T("�e�X�g�t�@�C��.dat"));
	v.push_back(_T("�e�X�g�f�B���N�g��"));
	v.push_back(_T("�e�X�g�f�B���N�g��\\�t�@�C��1.mp3"));
	v.push_back(_T("�e�X�g�f�B���N�g��\\�t�@�C��2.wmv"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".tar.gz"));
	BOOST_CHECK(optar == true);
	
	// TestCase11: �����t�@�C�� (�t�@�C�� + �f�B���N�g��) �� tgz �Ɉ��k
	filetype = _T("tgz");
	optar = false;
	v.clear();
	v.push_back(_T("foo.txt"));
	v.push_back(_T("bar"));
	v.push_back(_T("bar\\bas.txt"));
	v.push_back(_T("�e�X�g�t�@�C��.dat"));
	v.push_back(_T("�e�X�g�f�B���N�g��"));
	v.push_back(_T("�e�X�g�f�B���N�g��\\�t�@�C��1.mp3"));
	v.push_back(_T("�e�X�g�f�B���N�g��\\�t�@�C��2.wmv"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".tgz"));
	BOOST_CHECK(optar == true);
	
	// TestCase12: �����t�@�C�� (�t�@�C�� + �f�B���N�g��) �� bzip2 �Ɉ��k
	filetype = _T("bzip2");
	optar = false;
	v.clear();
	v.push_back(_T("foo.txt"));
	v.push_back(_T("bar"));
	v.push_back(_T("bar\\bas.txt"));
	v.push_back(_T("�e�X�g�t�@�C��.dat"));
	v.push_back(_T("�e�X�g�f�B���N�g��"));
	v.push_back(_T("�e�X�g�f�B���N�g��\\�t�@�C��1.mp3"));
	v.push_back(_T("�e�X�g�f�B���N�g��\\�t�@�C��2.wmv"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".tar.bz2"));
	BOOST_CHECK(optar == true);
	
	// TestCase13: �����t�@�C�� (�t�@�C�� + �f�B���N�g��) �� tbz �Ɉ��k
	filetype = _T("tbz");
	optar = false;
	v.clear();
	v.push_back(_T("foo.txt"));
	v.push_back(_T("bar"));
	v.push_back(_T("bar\\bas.txt"));
	v.push_back(_T("�e�X�g�t�@�C��.dat"));
	v.push_back(_T("�e�X�g�f�B���N�g��"));
	v.push_back(_T("�e�X�g�f�B���N�g��\\�t�@�C��1.mp3"));
	v.push_back(_T("�e�X�g�f�B���N�g��\\�t�@�C��2.wmv"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".tbz"));
	BOOST_CHECK(optar == true);
	
	// TestCase14: �P��� tar �t�@�C���� gzip �Ɉ��k
	filetype = _T("gzip");
	optar = false;
	v.clear();
	v.push_back(_T("�e�X�g�t�@�C��.tar"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".tar.gz"));
	BOOST_CHECK(optar == false);
	
	// TestCase15: �P��� tar �t�@�C���� tgz �Ɉ��k
	filetype = _T("tgz");
	optar = false;
	v.clear();
	v.push_back(_T("�e�X�g�t�@�C��.tar"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".tgz"));
	BOOST_CHECK(optar == false);
	
	// TestCase16: �P��� tar �t�@�C���� bzip2 �Ɉ��k
	filetype = _T("bzip2");
	optar = false;
	v.clear();
	v.push_back(_T("�e�X�g�t�@�C��.tar"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".tar.bz2"));
	BOOST_CHECK(optar == false);
	
	// TestCase17: �P��� tar �t�@�C���� tbz �Ɉ��k
	filetype = _T("tbz");
	optar = false;
	v.clear();
	v.push_back(_T("�e�X�g�t�@�C��.tar"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".tbz"));
	BOOST_CHECK(optar == false);
	
	// TestCase18: ������ tar �t�@�C���� gzip �Ɉ��k
	filetype = _T("gzip");
	optar = false;
	v.clear();
	v.push_back(_T("foo.tar"));
	v.push_back(_T("bar.tar"));
	v.push_back(_T("�e�X�g�t�@�C��1.tar"));
	v.push_back(_T("�e�X�g�t�@�C��2.tar"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".tar.gz"));
	BOOST_CHECK(optar == true);
	
	// TestCase19: ������ tar �t�@�C���� tgz �Ɉ��k
	filetype = _T("tgz");
	optar = false;
	v.clear();
	v.push_back(_T("foo.tar"));
	v.push_back(_T("bar.tar"));
	v.push_back(_T("�e�X�g�t�@�C��1.tar"));
	v.push_back(_T("�e�X�g�t�@�C��2.tar"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".tgz"));
	BOOST_CHECK(optar == true);
	
	// TestCase20: ������ tar �t�@�C���� bzip2 �Ɉ��k
	filetype = _T("bzip2");
	optar = false;
	v.clear();
	v.push_back(_T("foo.tar"));
	v.push_back(_T("bar.tar"));
	v.push_back(_T("�e�X�g�t�@�C��1.tar"));
	v.push_back(_T("�e�X�g�t�@�C��2.tar"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".tar.bz2"));
	BOOST_CHECK(optar == true);
	
	// TestCase21: ������ tar �t�@�C���� tbz �Ɉ��k
	filetype = _T("tbz");
	optar = false;
	v.clear();
	v.push_back(_T("foo.tar"));
	v.push_back(_T("bar.tar"));
	v.push_back(_T("�e�X�g�t�@�C��1.tar"));
	v.push_back(_T("�e�X�g�t�@�C��2.tar"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".tbz"));
	BOOST_CHECK(optar == true);
	
	// TestCase22: tar �t�@�C�����܂ޕ����̃t�@�C���� gzip �Ɉ��k
	filetype = _T("gzip");
	optar = false;
	v.clear();
	v.push_back(_T("hoge.tar"));
	v.push_back(_T("fuga.tar"));
	v.push_back(_T("foo.txt"));
	v.push_back(_T("bar"));
	v.push_back(_T("bar\\bas.txt"));
	v.push_back(_T("�e�X�g�t�@�C��.dat"));
	v.push_back(_T("�e�X�g�f�B���N�g��"));
	v.push_back(_T("�e�X�g�f�B���N�g��\\�t�@�C��1.mp3"));
	v.push_back(_T("�e�X�g�f�B���N�g��\\�t�@�C��2.wmv"));
	v.push_back(_T("�e�X�g�f�B���N�g��\\�e�X�g�A�[�J�C�u1.tar"));
	v.push_back(_T("�e�X�g�f�B���N�g��\\�e�X�g�A�[�J�C�u2.tar"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".tar.gz"));
	BOOST_CHECK(optar == true);
	
	// TestCase23: tar �t�@�C�����܂ޕ����̃t�@�C���� tgz �Ɉ��k
	filetype = _T("tgz");
	optar = false;
	v.clear();
	v.push_back(_T("hoge.tar"));
	v.push_back(_T("fuga.tar"));
	v.push_back(_T("foo.txt"));
	v.push_back(_T("bar"));
	v.push_back(_T("bar\\bas.txt"));
	v.push_back(_T("�e�X�g�t�@�C��.dat"));
	v.push_back(_T("�e�X�g�f�B���N�g��"));
	v.push_back(_T("�e�X�g�f�B���N�g��\\�t�@�C��1.mp3"));
	v.push_back(_T("�e�X�g�f�B���N�g��\\�t�@�C��2.wmv"));
	v.push_back(_T("�e�X�g�f�B���N�g��\\�e�X�g�A�[�J�C�u1.tar"));
	v.push_back(_T("�e�X�g�f�B���N�g��\\�e�X�g�A�[�J�C�u2.tar"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".tgz"));
	BOOST_CHECK(optar == true);
	
	// TestCase24: tar �t�@�C�����܂ޕ����̃t�@�C���� bzip2 �Ɉ��k
	filetype = _T("bzip2");
	optar = false;
	v.clear();
	v.push_back(_T("hoge.tar"));
	v.push_back(_T("fuga.tar"));
	v.push_back(_T("foo.txt"));
	v.push_back(_T("bar"));
	v.push_back(_T("bar\\bas.txt"));
	v.push_back(_T("�e�X�g�t�@�C��.dat"));
	v.push_back(_T("�e�X�g�f�B���N�g��"));
	v.push_back(_T("�e�X�g�f�B���N�g��\\�t�@�C��1.mp3"));
	v.push_back(_T("�e�X�g�f�B���N�g��\\�t�@�C��2.wmv"));
	v.push_back(_T("�e�X�g�f�B���N�g��\\�e�X�g�A�[�J�C�u1.tar"));
	v.push_back(_T("�e�X�g�f�B���N�g��\\�e�X�g�A�[�J�C�u2.tar"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".tar.bz2"));
	BOOST_CHECK(optar == true);
	
	// TestCase25: tar �t�@�C�����܂ޕ����̃t�@�C���� tbz �Ɉ��k
	filetype = _T("tbz");
	optar = false;
	v.clear();
	v.push_back(_T("hoge.tar"));
	v.push_back(_T("fuga.tar"));
	v.push_back(_T("foo.txt"));
	v.push_back(_T("bar"));
	v.push_back(_T("bar\\bas.txt"));
	v.push_back(_T("�e�X�g�t�@�C��.dat"));
	v.push_back(_T("�e�X�g�f�B���N�g��"));
	v.push_back(_T("�e�X�g�f�B���N�g��\\�t�@�C��1.mp3"));
	v.push_back(_T("�e�X�g�f�B���N�g��\\�t�@�C��2.wmv"));
	v.push_back(_T("�e�X�g�f�B���N�g��\\�e�X�g�A�[�J�C�u1.tar"));
	v.push_back(_T("�e�X�g�f�B���N�g��\\�e�X�g�A�[�J�C�u2.tar"));
	BOOST_CHECK(cubeice::compressor::extension(v.begin(), v.end(), filetype, optar) == _T(".tbz"));
	BOOST_CHECK(optar == true);
}

#endif // CUBEICE_TEST_COMPRESSOR_H
