/* ------------------------------------------------------------------------- */
/*
 *  cubeice-ctxdata.h
 *
 *  Copyright (c) 2010 CubeSoft Inc.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    - No names of its contributors may be used to endorse or promote
 *      products derived from this software without specific prior written
 *      permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  Last-modified: Wed 12 Jan 2011 14:28:04 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef		SHL_CTX_MENU_DATA_H
#define		SHL_CTX_MENU_DATA_H

#define		MAKE_AWSTRING( text )		text, L##text

#include <windows.h>
#include <vector>
#include <string>
#include "resource.h"

// ���O�o�͂𖳌��ɂ���D
#ifndef PSDOTNET_INVALIDATE_LOG_MACRO
#define PSDOTNET_INVALIDATE_LOG_MACRO
#endif
#include <cubeice/user-setting.h>

const int	ICON_SIZE = 16;
const int	ICON_Y_SIZE = ICON_SIZE;
const int	ICON_X_SIZE = ICON_SIZE;

class IMenuInfo {
public:
	typedef std::basic_string<TCHAR>		tstring;

	virtual const std::vector<tstring> &GetFileList() = 0;
	virtual const tstring &GetFilePath( const std::size_t &index ) = 0;
	virtual std::size_t GetSize() = 0;
	virtual tstring GetOption() = 0;
};

void MenuSelectedCallback( const LPCTSTR arg, IMenuInfo *info );

struct SUB_MENU_ITEM {
	cubeice::user_setting::size_type	dispSetting;		// Display setting
	WORD								iconID;				// Display menu item icon
	LPCSTR								stringA;			// Display menu item string [A]
	LPCWSTR								stringW;			// Display menu item string [W]
	const SUB_MENU_ITEM					*submenu;			// Submenu
	LPCSTR								explanationA;		// Explanation string [A]
	LPCWSTR								explanationW;		// Explanation string [W]
	LPCTSTR								arg;				// Callback argument
};

const WORD		ICON_NOT_USED = -1;

const SUB_MENU_ITEM		SubMenuCompress[] = {
	{ COMP_ZIP_FLAG,		ICON_NOT_USED,	MAKE_AWSTRING( "zip" ),					NULL,				MAKE_AWSTRING( "zip ���k" ),				TEXT( "/c:zip" ) },
	{ COMP_ZIP_PASS_FLAG,	ICON_NOT_USED,	MAKE_AWSTRING( "zip (�p�X���[�h)" ),	NULL,				MAKE_AWSTRING( "zip ���k (�p�X���[�h)" ),		TEXT( "/c:zip /p" ) },
	{ COMP_SEVENZIP_FLAG,	ICON_NOT_USED,	MAKE_AWSTRING( "7z" ),					NULL,				MAKE_AWSTRING( "7-zip ���k" ),			TEXT( "/c:7z" ) },
	{ COMP_BZIP2_FLAG,		ICON_NOT_USED,	MAKE_AWSTRING( "bzip2" ),				NULL,				MAKE_AWSTRING( "bzip2 ���k" ),			TEXT( "/c:bzip2" ) },
	{ COMP_GZIP_FLAG,		ICON_NOT_USED,	MAKE_AWSTRING( "gzip" ),				NULL,				MAKE_AWSTRING( "gzip ���k" ),			TEXT( "/c:gzip" ) },
	{ COMP_EXE_FLAG,		ICON_NOT_USED,	MAKE_AWSTRING( "exe" ),					NULL,				MAKE_AWSTRING( "���ȉ𓀌`���ň��k" ),		TEXT( "/c:exe" ) },
	{ COMP_DETAIL_FLAG,		ICON_NOT_USED,	MAKE_AWSTRING( "�ڍאݒ�" ),			NULL,				MAKE_AWSTRING( "�ڍׂ�ݒ肵�Ĉ��k" ),			TEXT( "/c:detail" ) },
	{ 0, ICON_NOT_USED, NULL, NULL, NULL, NULL, NULL, NULL }		// end marker
};
const SUB_MENU_ITEM		SubMenuCompAndMail[] = {
	{ MAIL_ZIP_FLAG,		ICON_NOT_USED,	MAKE_AWSTRING( "zip" ),					NULL,				MAKE_AWSTRING( "zip ���k���ă��[�����M" ),				TEXT( "/c:zip /m" ) },
	{ MAIL_ZIP_PASS_FLAG,	ICON_NOT_USED,	MAKE_AWSTRING( "zip (�p�X���[�h)" ),	NULL,				MAKE_AWSTRING( "zip ���k���ă��[�����M (�p�X���[�h)" ),		TEXT( "/c:zip /p /m" ) },
	{ MAIL_SEVENZIP_FLAG,	ICON_NOT_USED,	MAKE_AWSTRING( "7z" ),					NULL,				MAKE_AWSTRING( "7-zip ���k���ă��[�����M" ),			TEXT( "/c:7z /m" ) },
	{ MAIL_BZIP2_FLAG,		ICON_NOT_USED,	MAKE_AWSTRING( "bzip2" ),				NULL,				MAKE_AWSTRING( "bzip2 ���k���ă��[�����M" ),			TEXT( "/c:bzip2 /m" ) },
	{ MAIL_GZIP_FLAG,		ICON_NOT_USED,	MAKE_AWSTRING( "gzip" ),				NULL,				MAKE_AWSTRING( "gzip ���k���ă��[�����M" ),			TEXT( "/c:gzip /m" ) },
	{ MAIL_EXE_FLAG,		ICON_NOT_USED,	MAKE_AWSTRING( "exe" ),					NULL,				MAKE_AWSTRING( "���ȉ𓀌`���ň��k���Ă��ă��[�����M" ),		TEXT( "/c:exe /m" ) },
	{ MAIL_DETAIL_FLAG,		ICON_NOT_USED,	MAKE_AWSTRING( "�ڍאݒ�" ),			NULL,				MAKE_AWSTRING( "���k���ă��[�����M (�ڍאݒ�)" ),			TEXT( "/c:detail /m" ) },
	{ 0, ICON_NOT_USED, NULL, NULL, NULL, NULL, NULL, NULL }		// end marker
};
const SUB_MENU_ITEM		SubMenuDecompress[] = {
	{ DECOMP_HERE_FLAG,		ICON_NOT_USED,	MAKE_AWSTRING( "�����ɉ�" ),			NULL,				MAKE_AWSTRING( "���̏ꏊ�ɉ�" ),		TEXT( "/x:source" ) },
	{ DECOMP_DESKTOP_FLAG,	ICON_NOT_USED,	MAKE_AWSTRING( "�f�X�N�g�b�v�ɉ�" ),	NULL,				MAKE_AWSTRING( "�f�X�N�g�b�v�ɉ�" ),	TEXT( "/x:desktop" ) },
	{ DECOMP_MYDOCUMENTS_FLAG,ICON_NOT_USED,	MAKE_AWSTRING( "�}�C�h�L�������g�ɉ�" ),	NULL,				MAKE_AWSTRING( "�}�C�h�L�������g�ɉ�" ),	TEXT( "/x:mydocuments" ) },
	{ DECOMP_RUNTIME_FLAG,	ICON_NOT_USED,	MAKE_AWSTRING( "�ꏊ���w�肵�ĉ�" ),	NULL,				MAKE_AWSTRING( "�ꏊ���w�肵�ĉ�" ),	TEXT( "/x:runtime" ) },
	{ 0, ICON_NOT_USED, NULL, NULL, NULL, NULL, NULL, NULL }		// end marker
};

const SUB_MENU_ITEM		MenuItem[] = {
	{ COMPRESS_FLAG,	IDB_COMPRESS,		MAKE_AWSTRING( "���k" ),				SubMenuCompress,	MAKE_AWSTRING( "���k" ),			NULL },
	{ DECOMPRESS_FLAG,	IDB_DECOMPRESS,		MAKE_AWSTRING( "��" ),				SubMenuDecompress,	MAKE_AWSTRING( "��" ),			NULL },
	{ MAIL_FLAG,		IDB_COMPRESS,		MAKE_AWSTRING( "���k���ă��[�����M" ),	SubMenuCompAndMail,	MAKE_AWSTRING( "���k���ă��[�����M" ),	NULL },
//	{ SETTING_FLAG,		IDB_LOGO,		MAKE_AWSTRING( "�ݒ�" ),				NULL,				MAKE_AWSTRING( "�ݒ���s��" ),			NULL },
	{ 0, ICON_NOT_USED, NULL, NULL, NULL, NULL, NULL, NULL }		// end marker
};

#endif	// SHL_CTX_MENU_DATA_H