// -*- coding: shift-jis -*-
/* ------------------------------------------------------------------------- */
/*
 *  cubeice-setting/resource.h
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
#ifndef CUBEICE_SETTING_RESOURCE_H
#define CUBEICE_SETTING_RESOURCE_H

#ifndef IDC_STATIC
#define IDC_STATIC (-1)
#endif

/* ------------------------------------------------------------------------- */
//  �֘A�t���O���[�v
/* ------------------------------------------------------------------------- */
#define IDC_ASSOCIATION             2100
#define IDC_ASSOC_ZIP               2101
#define IDC_ASSOC_LZH               2102
#define IDC_ASSOC_RAR               2103
#define IDC_ASSOC_TAR               2104
#define IDC_ASSOC_GZ                2105
#define IDC_ASSOC_7Z                2106
#define IDC_ASSOC_ARJ               2107
#define IDC_ASSOC_BZ2               2108
#define IDC_ASSOC_CAB               2109
#define IDC_ASSOC_CHM               2110
#define IDC_ASSOC_CPIO              2111
#define IDC_ASSOC_DEB               2112
#define IDC_ASSOC_DMG               2113
#define IDC_ASSOC_ISO               2114
#define IDC_ASSOC_RPM               2115
#define IDC_ASSOC_TBZ               2116
#define IDC_ASSOC_TGZ               2117
#define IDC_ASSOC_WIM               2118
#define IDC_ASSOC_XAR               2119
#define IDC_ASSOC_XZ                2120
#define IDC_ASSOC_JAR               2121
#define IDC_ASSOC_CHECKALL          2198 // ���ׂđI��
#define IDC_ASSOC_RESETALL          2199 // ���ׂĉ���

/* ------------------------------------------------------------------------- */
//  �R���e�L�X�g���j���[�O���[�v
/* ------------------------------------------------------------------------- */
#define IDC_CONTEXT                 2200
#define IDC_CX_COMPRESSION          2201 // ���k
#define IDC_CX_DECOMPRESSION        2202 // ��
#define IDC_CX_MAIL                 2203 // ���k���ă��[�����M
#define IDC_CX_ZIP                  2204 // zip
#define IDC_CX_ZIP_P                2205 // zip (�p�X���[�h)
#define IDC_CX_7Z                   2206 // 7z
#define IDC_CX_BZIP2                2207 // bzip2
#define IDC_CX_GZIP                 2208 // gzip
#define IDC_CX_EXE                  2220 // exe
#define IDC_CX_DETAIL               2209 // �ڍאݒ�
#define IDC_CX_HERE                 2210 // �����ɉ�
#define IDC_CX_DESKTOP              2211 // �f�X�N�g�b�v�ɉ�
#define IDC_CX_RUNTIME              2212 // �ꏊ���w�肵�ĉ�
#define IDC_CX_MYDOCUMENTS          2219 // �}�C�h�L�������g�ɉ�
#define IDC_CX_ZIP_M                2213 // zip (���[��)
#define IDC_CX_ZIP_P_M              2214 // zip (�p�X���[�h�A���[��)
#define IDC_CX_7Z_M                 2215 // 7z (���[��)
#define IDC_CX_BZIP2_M              2216 // bzip2 (���[��)
#define IDC_CX_GZIP_M               2217 // gzip (���[��)
#define IDC_CX_EXE_M                2221 // exe (���[��)
#define IDC_CX_DETAIL_M             2218 // �ڍאݒ� (���[��)
#define IDC_CX_CUSTOMIZE            2250 // �J�X�^�}�C�Y
#define IDC_CX_RESET                2251 // ���Z�b�g

/* ------------------------------------------------------------------------- */
//  �R���e�L�X�g���j���[�O���[�v
/* ------------------------------------------------------------------------- */
#define IDC_SHORTCUT                2300
#define IDC_SC_COMPRESSION          2301 // COMPRESS_FLAG
#define IDC_SC_DECOMPRESSION        2302 // DECOMPRESS_FLAG
#define IDC_SC_SETTING              2303 // SETTING_FLAG
#define IDC_DETAIL_DEFAULTTYPE      2304 // TODO: �u���k�̎�ށv�͈��k�^�u�Ɉړ�������

/* ------------------------------------------------------------------------- */
//  �o�͐�O���[�v
/* ------------------------------------------------------------------------- */
#define IDC_OUTPUT                  3000
#define IDC_OUTPUT_SPECIFIC         3001 // �w�肵���t�H���_
#define IDC_OUTPUT_SOURCE           3002 // ���̃t�@�C���Ɠ����t�H���_
#define IDC_OUTPUT_RUNTIME          3003 // ���s���Ɏw�肷��
#define IDC_OUTPUT_PATH             3004
#define IDC_OUTPUT_PATH_BUTTON      3005

/* ------------------------------------------------------------------------- */
//  �ڍ׃O���[�v
/* ------------------------------------------------------------------------- */
#define IDC_DETAIL                  3100
#define IDC_DETAIL_CONFIRM          3101 // �㏑���̊m�F���s��
#define IDC_DETAIL_CONFIRMOLDER     3102 // �X�V�������V�����ꍇ�͊m�F���Ȃ�
#define IDC_DETAIL_CREATEFOLDER     3103 // �t�H���_��������������
#define IDC_DETAIL_SKIPSINGLEFOLDER 3109 // �P��t�H���_�̏ꍇ�͐������Ȃ�
#define IDC_DETAIL_SKIPSINGLEFILE   3118 // �P��t�@�C���̏ꍇ�͐������Ȃ�
#define IDC_DETAIL_ENCODE           3104 // �t�@�C�����̕����R�[�h��ϊ�����
#define IDC_DETAIL_FILTERING        3105 // �t�@�C���̃t�B���^�������O���s��
#define IDC_DETAIL_OPEN             3106 // �ۑ���t�H���_���J��
#define IDC_DETAIL_SKIPDESKTOP      3107 // �f�X�N�g�b�v�̏ꍇ�͊J���Ȃ�
#define IDC_DETAIL_DELETEONMAIL     3110 // ���[���Y�t�̍ۂɃt�@�C�����c���Ȃ�
#define IDC_DETAIL_TOOLTIP          3111 // �c�[���`�b�v�Ƀt�@�C���ꗗ��\������
#define IDC_DETAIL_TOOLTIPCOUNT_T   3112
#define IDC_DETAIL_TOOLTIPCOUNT     3113 // �c�[���`�b�v�̕\������ő匏��
#define IDC_DETAIL_DELETEONEXTRACT  3115 // ���t�@�C�����폜����

/* ------------------------------------------------------------------------- */
//  �u�t�B���^�����O�v�^�u
/* ------------------------------------------------------------------------- */
#define IDC_FILTERING               4001

/* ------------------------------------------------------------------------- */
//  �u�o�[�W�������v�^�u
/* ------------------------------------------------------------------------- */
#define IDC_VERSION_LOGO            5000
#define IDC_VERSION_NAME            5001
#define IDC_VERSION_VERSION         5002
#define IDC_VERSION_COMPANY         5003
#define IDC_VERSION_URL             5004
#define IDC_VERSION_UPDATE          5005
#define IDC_VERSION_ERRORREPORT     5006
#define IDC_VERSION_DEBUG           5007

/* ------------------------------------------------------------------------- */
//  �R���e�L�X�g���j���[�̃J�X�^�}�C�Y�p�_�C�A���O
/* ------------------------------------------------------------------------- */
#define IDC_CURRENT_LABEL           6001
#define IDC_ADD_LABEL               6002
#define IDC_CURRENT_TREEVIEW        6003
#define IDC_ADD_TREEVIEW            6004
#define IDC_ADD_BUTTON              6005
#define IDC_DELETE_BUTTON           6006
#define IDC_UP_BUTTON               6007
#define IDC_DOWN_BUTTON             6008
#define IDC_FOLDER_BUTTON           6009
#define IDC_RENAME_BUTTON           6010
#define IDC_RESET_BUTTON            6011

#define IDM_UP_MENU                 6107
#define IDM_DOWN_MENU               6108
#define IDM_FOLDER_MENU             6109
#define IDM_RENAME_MENU             6110
#define IDM_RESET_MENU              6111
#define IDM_DELETE_MENU             6106
#define IDM_ADD_MENU                6105

#endif // CUBEICE_SETTING_RESOURCE_H
