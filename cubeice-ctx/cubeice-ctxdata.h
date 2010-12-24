#ifndef		SHL_CTX_MENU_DATA_H
#define		SHL_CTX_MENU_DATA_H

#define		MAKE_AWSTRING( text )		text, L##text

#include <windows.h>
#include <vector>
#include <string>
#include "user-setting.h"
#include "resource.h"

class IMenuInfo {
public:
	typedef std::basic_string<TCHAR>		tstring;

	virtual const std::vector<tstring> &GetFileList() = 0;
	virtual const tstring &GetFilePath( const std::size_t &index ) = 0;
	virtual std::size_t GetSize() = 0;
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
	{ 0,				ICON_NOT_USED,	MAKE_AWSTRING( "zip" ),					NULL,				MAKE_AWSTRING( "zip���k" ),				TEXT( "/c:zip" ) },
	{ 0,				ICON_NOT_USED,	MAKE_AWSTRING( "zip(pass)" ),			NULL,				MAKE_AWSTRING( "zip���k(�p�X)" ),		TEXT( "/c:zip /p" ) },
	{ 0,				ICON_NOT_USED,	MAKE_AWSTRING( "7-zip" ),				NULL,				MAKE_AWSTRING( "7-zip���k" ),			TEXT( "/c:7z" ) },
//	{ 0,				ICON_NOT_USED,	MAKE_AWSTRING( "bzip2" ),				NULL,				MAKE_AWSTRING( "bzip2���k" ),			TEXT( "/c:bzip2" ) },
//	{ 0,				ICON_NOT_USED,	MAKE_AWSTRING( "gzip" ),				NULL,				MAKE_AWSTRING( "gzip���k" ),			TEXT( "/c:gzip" ) },
	{ 0,				ICON_NOT_USED,	MAKE_AWSTRING( "exe" ),					NULL,				MAKE_AWSTRING( "���ȉ𓀌`��" ),		TEXT( "/c:exe" ) },
	{ 0, ICON_NOT_USED, NULL, NULL, NULL, NULL, NULL, NULL }		// end marker
};
const SUB_MENU_ITEM		SubMenuDecompress[] = {
	{ 0,				ICON_NOT_USED,	MAKE_AWSTRING( "�����ɓW�J" ),			NULL,				MAKE_AWSTRING( "���̏ꏊ�ɓW�J" ),		TEXT( "/x:here" ) },
	{ 0,				ICON_NOT_USED,	MAKE_AWSTRING( "�f�X�N�g�b�v�ɓW�J" ),	NULL,				MAKE_AWSTRING( "�f�X�N�g�b�v�ɓW�J" ),	TEXT( "/x:desktop" ) },
	{ 0,				ICON_NOT_USED,	MAKE_AWSTRING( "�ꏊ���w�肵�ēW�J" ),	NULL,				MAKE_AWSTRING( "�ꏊ���w�肵�ēW�J" ),	TEXT( "/x:specified" ) },
	{ 0, ICON_NOT_USED, NULL, NULL, NULL, NULL, NULL, NULL }		// end marker
};

const SUB_MENU_ITEM		MenuItem[] = {
	{ COMPRESS_FLAG,	IDB_LOGO,		MAKE_AWSTRING( "CubeICE�ň��k" ),		SubMenuCompress,	MAKE_AWSTRING( "���k���s��" ),			NULL },
	{ DECOMPRESS_FLAG,	IDB_LOGO,		MAKE_AWSTRING( "CubeICE�œW�J" ),		SubMenuDecompress,	MAKE_AWSTRING( "�W�J���s��" ),			NULL },
	{ SETTING_FLAG,		IDB_LOGO,		MAKE_AWSTRING( "CubeICE�̐ݒ�" ),		NULL,				MAKE_AWSTRING( "�ݒ���s��" ),			TEXT( "" ) },
	{ 0, ICON_NOT_USED, NULL, NULL, NULL, NULL, NULL, NULL }		// end marker
};

#endif	// SHL_CTX_MENU_DATA_H