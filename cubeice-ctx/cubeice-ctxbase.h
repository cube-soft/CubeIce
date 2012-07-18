// -*- coding: shift-jis -*-
/* ------------------------------------------------------------------------- */
/*
 *  cubeice/cubeice-ctx/cubeice-ctxbase.h
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
#ifndef	CUBEICE_CTXBASE_H
#define	CUBEICE_CTXBASE_H

#ifndef CLX_USE_WCHAR
#define CLX_USE_WCHAR
#endif

#include <cubeice/config.h>
#include <vector>
#include <clx/lexical_cast.h>
#include <clx/date_time.h>
#include <clx/tokenizer.h>
#include <cubeice/format.h>
#include <cubeice/wpopen.h>
#include <cubeice/user-setting.h>

#define CUBEICE_ENGINE _T("cubeice-exec.exe")
#define CUBEICE_MAXFILESIZE (1024 * 1024 * 1024)

extern HINSTANCE hDllInstance;

class IMenuInfo {
public:
	typedef CubeICE::char_type char_type;
	typedef CubeICE::string_type string_type;
	typedef std::size_t size_type;
	
	virtual const std::vector<string_type>& GetFileList() const = 0;
	virtual const string_type& GetFilePath( const std::size_t &index ) const = 0;
	virtual size_type GetSize() const = 0;
	virtual string_type GetOption() const = 0;
};

void MenuSelectedCallback(const CubeICE::Context& src, IMenuInfo *info);

namespace CubeICE {
		/* ----------------------------------------------------------------- */
		///
		/// CShlCtxMenuBase
		///
		/// <summary>
		/// �R���e�L�X�g���j���[�A����уt�@�C���Ƀ}�E�X�I�[�o���ꂽ�ۂ�
		/// �\�������c�[���`�b�v���g�����邽�߂̃N���X�ł��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		class CShlCtxMenuBase : public IContextMenu3, IShellExtInit, IQueryInfo, IPersistFile, IMenuInfo {
		protected:
			typedef IMenuInfo::char_type char_type;
			typedef IMenuInfo::string_type string_type;
			typedef IMenuInfo::size_type size_type;
			typedef std::map<UINT, Context> context_map;
			
			struct fileinfo {
			public:
				string_type name;
				size_type size;
				clx::date_time time;
				bool directory;
				
				fileinfo() : name(), size(0), time(), directory(false) {}
			};
			
		public:
			/* ------------------------------------------------------------- */
			/// constructor
			/* ------------------------------------------------------------- */
			CShlCtxMenuBase( ULONG &dllrc ) :
				ctxSetting(), refCount( 1UL ), hInstance( hDllInstance ), dllRefCount( dllrc ),
				fileNum( 0 ), folderNum( 0 ), inserted_() {
				ctxSetting.Load();
				InterlockedIncrement( reinterpret_cast<LONG*>(&dllRefCount) );
				
				TCHAR path[2048] = {};
				GetModuleFileName( hDllInstance, path, sizeof( path ) );
				PathRemoveFileSpec( path );
				cubeiceEnginePath = path;
				cubeiceEnginePath += _T( "\\" ) CUBEICE_ENGINE;
			}
			
			/* ------------------------------------------------------------- */
			/// destructor
			/* ------------------------------------------------------------- */
			virtual ~CShlCtxMenuBase() {
				InterlockedDecrement( reinterpret_cast<LONG*>(&dllRefCount) );
			}
			
			/* ------------------------------------------------------------- */
			///
			/// Initialize
			///
			/// <summary>
			/// IShellExtInit ����p������܂��B�g���V�F���̏��������s���܂��B
			/// </summary>
			///
			/* ------------------------------------------------------------- */
			STDMETHODIMP Initialize( LPCITEMIDLIST pIDFolder, LPDATAOBJECT pDataObj, HKEY hKeyID ) {
				HRESULT			hr;
				FORMATETC		fme;
				STGMEDIUM		stgm;
				HDROP			hDrop;
				UINT			numoffiles;
				
				fme.cfFormat	= CF_HDROP;
				fme.ptd			= NULL;
				fme.dwAspect	= DVASPECT_CONTENT;
				fme.lindex		= -1;
				fme.tymed		= TYMED_HGLOBAL;
				
				hr = pDataObj->GetData( &fme, &stgm );
				if( FAILED( hr ) )
					return hr;
				
				hDrop = static_cast<HDROP>( GlobalLock( stgm.hGlobal ) );
				numoffiles = DragQueryFile( hDrop, static_cast<UINT>( -1 ), NULL, 0 );
				
				for( UINT i = 0 ; i < numoffiles ; ++i ) {
					UINT	bufsize;
					TCHAR	*buffer;
					
					bufsize = DragQueryFile( hDrop, i, NULL, 0 );
					buffer = new TCHAR[bufsize+5];
					DragQueryFile( hDrop, i, buffer, bufsize + 3 );
					
					fileList.push_back(string_type( buffer ) );
					
					delete[] buffer;
				}
				
				GlobalUnlock( hDrop );
				ReleaseStgMedium( &stgm );

				if( pIDFolder ) {
					isDragAndDrop = true;
					TCHAR buffer[MAX_PATH] = {};
					::SHGetPathFromIDList(pIDFolder, buffer);
					this->dropPath = buffer;
				}
				else isDragAndDrop = false;

				return S_OK;
			}
			
			/* ------------------------------------------------------------- */
			///
			/// Load
			///
			/// <summary>
			/// IPersistFile �N���X����p������܂��B�R���e�L�X�g���j���[
			/// �ŉ��炩�̃t�@�C�����I�����ꂽ�ꍇ�ɁA���̃t�@�C���Ɋւ���
			/// ����ǂݍ��ނ��߂Ɏ��s����܂��B
			/// </summary>
			///
			/* ------------------------------------------------------------- */
			STDMETHODIMP Load(LPCOLESTR pszFileName, DWORD dwMode) {
				compFileList.clear();
#ifdef	UNICODE
				const wchar_t	*fname = pszFileName;
#else	// UNICODE
				char	fname[4096];
				WideCharToMultiByte( CP_OEMCP, 0, pszFileName, -1, fname, sizeof( fname ), NULL, NULL );
#endif	// UNICODE
				compFileName = pszFileName;
				HANDLE hFile = CreateFile( compFileName.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
				if( hFile != INVALID_HANDLE_VALUE ) {
					LARGE_INTEGER li = {};
					GetFileSizeEx( hFile, &li );
					CloseHandle( hFile );
					if (li.QuadPart < CUBEICE_MAXFILESIZE) GetArchiveInfo( fname, compFileList );
				}
				return S_OK;
			}
			
			/* ------------------------------------------------------------- */
			///
			/// QueryInterface
			///
			/// <summary>
			/// IUnknown members
			/// </summary>
			///
			/* ------------------------------------------------------------- */
			STDMETHODIMP QueryInterface( REFIID riid, LPVOID FAR *ppv ) {
				*ppv = NULL;
				
				if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IContextMenu)) *ppv = static_cast<LPCONTEXTMENU>(this);
				else if (IsEqualIID(riid, IID_IShellExtInit)) *ppv = static_cast<LPSHELLEXTINIT>( this );
				else if ( IsEqualIID( riid, IID_IQueryInfo ) ) *ppv = static_cast<IQueryInfo*>( this );
				else if ( IsEqualIID( riid, IID_IPersistFile ) ) *ppv = static_cast<LPPERSISTFILE>( this );
				else return E_NOINTERFACE;
				
				AddRef();
				
				return NOERROR;
			}
			
			/* ------------------------------------------------------------- */
			/// AddRef
			/* ------------------------------------------------------------- */
			STDMETHODIMP_(ULONG) AddRef() {
				return InterlockedIncrement( reinterpret_cast<LONG*>(&refCount) );
			}
			
			/* ------------------------------------------------------------- */
			/// Release
			/* ------------------------------------------------------------- */
			STDMETHODIMP_(ULONG) Release() {
				ULONG res = InterlockedDecrement( reinterpret_cast<LONG*>(&refCount) );
				if( res ) return res;
				delete this;
				return 0L;
			}
			
			/* ------------------------------------------------------------- */
			///
			/// QueryContextMenu
			///
			/// <summary>
			/// IContextMenu ����p������܂��B�R���e�L�X�g���j���[�ɕ\������
			/// ���ڂ�}�����܂��B
			///
			/// NOTE: �}������郁�j���[�̍��� ID �� [idCmdFirst, idCmdLast]
			/// �͈͓̔��łȂ���΂����܂���B
			/// </summary>
			///
			/* ------------------------------------------------------------- */
			STDMETHODIMP QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags) {
				if((uFlags & CMF_DEFAULTONLY) != 0) return NO_ERROR;
				
				UINT idCmd = idCmdFirst;
				
				if (ctxSetting.Context().IsExtended()) {
					if (ctxSetting.Context().ExtendedContext().empty()) return NO_ERROR;
				}
				else if (ctxSetting.Context().Builtin() == 0) return NO_ERROR;
				
				// �h���b�O&�h���b�v���j���[�̏ꍇ�́A�ŏ��̃Z�p���[�^�̑}�����X�L�b�v���܂��B
				if(!isDragAndDrop) ::InsertMenu(hMenu, indexMenu++, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
				
				if (isDragAndDrop && ctxSetting.DragDrop().IsExtended()) {
					this->InsertContextContainer(ctxSetting.DragDrop().ExtendedContext(), hMenu, indexMenu, idCmd, idCmdFirst);
				}
				else if (ctxSetting.Context().IsExtended()) {
					this->InsertContextContainer(ctxSetting.Context().ExtendedContext(), hMenu, indexMenu, idCmd, idCmdFirst);
				}
				else {
					ContextContainer v;
					ConvertContext(ctxSetting.Context().Builtin(), v, this->GetInstallDirectory(ctxSetting));
					this->InsertContextContainer(v, hMenu, indexMenu, idCmd, idCmdFirst);
				}
				
				if(idCmd - idCmdFirst > 0) InsertMenu(hMenu, indexMenu++, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
				
				MENUINFO mi = {};
				mi.cbSize = sizeof(mi);
				mi.fMask = MIM_STYLE;
				GetMenuInfo(hMenu, &mi);
				mi.dwStyle = (mi.dwStyle & ~MNS_NOCHECK) | MNS_CHECKORBMP;
				mi.fMask = MIM_STYLE | MIM_APPLYTOSUBMENUS;
				SetMenuInfo(hMenu, &mi);
				
				return MAKE_SCODE(SEVERITY_SUCCESS, FACILITY_NULL, idCmd - idCmdFirst);
			}
			
			/* ------------------------------------------------------------- */
			///
			/// InvokeCommand
			///
			/// <summary>
			/// IContextMenu ����p������܂��BQueryContextMenu �ɂ��ǉ�
			/// ���ꂽ���j���[���ڂ��I�����ꂽ���Ɏ��s����郁���o�֐��ł��B
			/// LOWORD(lpici->lpVerb) �ɑI�����ꂽ���j���[���ڂ̃I�t�Z�b�g��
			/// �i�[����Ă��܂��B
			/// </summary>
			///
			/* ------------------------------------------------------------- */
			STDMETHODIMP InvokeCommand( LPCMINVOKECOMMANDINFO lpcmi) {
				if(HIWORD(lpcmi->lpVerb)) return E_INVALIDARG;
				
				Context& cx = inserted_[LOWORD(lpcmi->lpVerb)];
				MenuSelectedCallback(cx, this);
				
				return S_OK;
			}
			
			/* ------------------------------------------------------------- */
			///
			/// GetCommandString
			///
			/// <summary>
			/// IContextMenu ����p������܂��B�R���e�L�X�g���j���[�ɑ΂���
			/// �R�}���h�����񂠂邢�̓w���v�������Ԃ��܂��B�w���v�������
			/// �G�N�X�v���[���̃X�e�[�^�X�o�[�ɕ\������܂��B
			/// </summary>
			///
			/* ------------------------------------------------------------- */
			STDMETHODIMP GetCommandString(UINT_PTR idCmd, UINT uFlags, UINT FAR *reserved, LPSTR pszName, UINT cchMax) {
				context_map::iterator pos = inserted_.find(idCmd);
				
				switch (uFlags) {
				case GCS_VALIDATEA:
				case GCS_VALIDATEW:
					return (pos != inserted_.end()) ? S_OK : S_FALSE;
				case GCS_VERBA: {
#if defined(UNICODE) || defined(_UNICODE)
					std::basic_string<char> s = CubeICE::UnicodeToSjis(pos->second.Name());
					strncpy_s(pszName, cchMax, s.c_str(), _TRUNCATE);
#else
					strncpy_s(pszName, cchMax, pos->second.Name().c_str(), _TRUNCATE);
#endif
					break;
				}
				case GCS_VERBW: {
#if defined(UNICODE) || defined(_UNICODE)
					wcsncpy_s(reinterpret_cast<LPWSTR>(pszName), cchMax, pos->second.Name().c_str(), _TRUNCATE);
#else
					std::basic_string<wchar_t> s = CubeICE::SjisToUnicode(pos->second.Name());
					wcsncpy_s(reinterpret_cast<LPWSTR>(pszName), cchMax, s.c_str(), _TRUNCATE);
#endif
				}
				case GCS_HELPTEXTA:
				case GCS_HELPTEXTW:
				default:
					break;
				}
				return S_OK;
			}
			
			/* ------------------------------------------------------------- */
			///
			/// GetInfoTip
			///
			/// <summary>
			/// IQueryInfo ����p������܂��B���k�t�@�C���Ƀ}�E�X�I�[�o
			/// ���ꂽ���Ƀc�[���`�b�v�ɕ\����������擾���܂��B
			/// </summary>
			///
			/* ------------------------------------------------------------- */
			STDMETHODIMP GetInfoTip(DWORD dwFlags, WCHAR **ppwszTip) {
				static const size_type maxcolumn = 50; // 1�s�ɏo�͂���ő啶����
				
				string_type tooltip = _T( "���: " );
				TCHAR* ext = PathFindExtension( compFileName.c_str() );
				if( ext ) {
					++ext;
					tooltip += ext;
				}
				tooltip += _T( " �t�@�C��\r\n" );
				
				HANDLE hFile = CreateFile( compFileName.c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
				LARGE_INTEGER li = { 0 };
				SYSTEMTIME st = { 0 };
				if( hFile != INVALID_HANDLE_VALUE ) {
					FILETIME ft, localft;
					
					GetFileSizeEx( hFile, &li );
					GetFileTime( hFile, NULL, NULL, &ft );
					
					FileTimeToLocalFileTime(&ft, &localft);
					FileTimeToSystemTime( &localft, &st );
					CloseHandle( hFile );
				}
				
				tooltip += _T( "�X�V����: " );
				TCHAR tmp[32] = {};
				_stprintf( tmp, _T( "%d/%02d/%02d %d:%02d" ), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute );
				tooltip += tmp;
				
				tooltip += _T("\r\n");
				
				tooltip += _T( "�T�C�Y: " );
				tooltip += cubeice::byte_format( li.QuadPart );
				
				if( dwFlags & QITIPF_USESLOWTIP && ctxSetting.Tooltip() && ctxSetting.TooltipCount() > 0 && compFileList.size() > 0) {
					tooltip += _T( "\r\n" );
					tooltip += _T( "�t�@�C�����X�g" );
					
					for(int i = 0 ; i < static_cast<int>(compFileList.size()) ; ++i ) {
						if (i >= ctxSetting.TooltipCount()) break;
						tooltip += _T( "\r\n" );
						string_type name = compFileList[i].name;
						if (name.size() > maxcolumn) {
							name = name.substr(name.size() - maxcolumn + 3, maxcolumn - 3);
							string_type::size_type pos = name.find_first_of(_T('\\'));
							if (pos != string_type::npos) name = _T("...") + name.substr(pos);
							else name = _T("...") + name;
						}
						tooltip += _T( "  " ) + name;
					}
					
					tooltip += _T("\r\n");
					if (static_cast<int>(compFileList.size()) > ctxSetting.TooltipCount()) tooltip += _T("�ق��A");
					tooltip += _T("�S ") + clx::lexical_cast<string_type>(fileNum) + _T(" �t�@�C�� ");
					tooltip += clx::lexical_cast<string_type>(folderNum) + _T(" �t�H���_");
				}

				*ppwszTip = static_cast<wchar_t*>( CoTaskMemAlloc( ( tooltip.size() + 5 ) * sizeof( wchar_t ) ) );
#ifdef	UNICODE
				lstrcpyW( *ppwszTip, tooltip.c_str() );
#else	// UNICODE
				MultiByteToWideChar( CP_OEMCP, 0, tooltip.c_str(), -1, *ppwszTip, ( tooltip.size() + 5 ) * sizeof( wchar_t ) );
#endif
				return S_OK;
			}
			
		public:
			/* ------------------------------------------------------------- */
			///
			/// GetFileList
			///
			/// <summary>
			/// �I�����ꂽ�t�@�C���̈ꗗ���擾���܂��B
			/// </summary>
			///
			/* ------------------------------------------------------------- */
			const std::vector<string_type>& GetFileList() const { return fileList; }
			
			/* ------------------------------------------------------------- */
			///
			/// GetSize
			///
			/// <summary>
			/// �I�����ꂽ���k�t�@�C���Ɋ܂܂��t�@�C�������擾���܂��B
			/// </summary>
			///
			/* ------------------------------------------------------------- */
			size_type GetSize() const { return fileList.size(); }
			
			/* ------------------------------------------------------------- */
			//  GetFilePath
			/* ------------------------------------------------------------- */
			const string_type& GetFilePath(const std::size_t& index) const { return fileList.at( index ); }
			
			/* ------------------------------------------------------------- */
			///
			/// GetOption
			///
			/// <summary>
			/// NOTE: �g������K�v���o���ꍇ�A�ێ����Ă��郁���o�ϐ�������
			/// cubeice.exe �֑��邽�߂̃I�v�V�����p�R�}���h���C���������
			/// �������邽�߂̃����o�֐��ł��B
			/// </summary>
			///
			/* ------------------------------------------------------------- */
			string_type GetOption() const {
				string_type dest = _T(" ");
				if (this->isDragAndDrop && !this->dropPath.empty()) {
					dest += _T('"');
					dest += _T("/drop:") + this->dropPath;
					dest += _T('"');
				}
				dest += _T(" ");
				
				return dest;
			}
			
		public:
			/* ------------------------------------------------------------- */
			///
			/// <summary>
			/// ���̊g���V�F���N���X�ł͖������A�܂��͓��ʂȏ�����K�v�Ƃ��Ȃ�
			/// �����o�֐��Q�������Œ�`���܂��B
			/// </summary>
			///
			/* ------------------------------------------------------------- */
			STDMETHODIMP Save(LPCOLESTR pszFileName, BOOL fRemember) { return E_NOTIMPL; }
			STDMETHODIMP SaveCompleted(LPCOLESTR pszFileName) { return E_NOTIMPL; }
			STDMETHODIMP GetCurFile(LPOLESTR* ppszFileName) { return E_NOTIMPL; }
			STDMETHODIMP GetClassID(CLSID* pClassID) { return E_NOTIMPL; }
			STDMETHODIMP IsDirty() { return E_NOTIMPL; }
			STDMETHODIMP GetInfoFlags(DWORD*) { return E_NOTIMPL; }
			
			STDMETHODIMP HandleMenuMsg( UINT uMsg, WPARAM wParam, LPARAM lParam ) { return S_OK; }
			STDMETHODIMP HandleMenuMsg2( UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult ) { return S_OK; }
			
		protected:
			/* ------------------------------------------------------------- */
			///
			/// SetMenuIcon
			///
			/// <summary>
			/// �R���e�L�X�g���j���[�ɕ\������A�C�R���̈�������
			/// Windows XP �� Windows Vista �ȍ~�ƂňقȂ��Ă���̂ŁA
			/// OS ���ɃA�C�R����\�����邽�߂̃����o�֐����`���܂��B
			/// </summary>
			///
			/* ------------------------------------------------------------- */
			virtual void SetMenuIcon(UINT cmdid, MENUITEMINFO &miinfo) = 0;
			
			/* ----------------------------------------------------------------- */
			///
			/// InsertedItems
			///
			/// �R���e�L�X�g���j���[�ɕ\���������ڂ̈ꗗ���擾���܂��B
			///
			/* ----------------------------------------------------------------- */
			context_map& InsertedItems() { return inserted_; }
			
		private:
			/* ----------------------------------------------------------------- */
			///
			/// InsertContext
			///
			/// <summary>
			/// �w�肳�ꂽ�R���e�L�X�g���j���[��}�����܂��B
			/// </summary>
			///
			/* ----------------------------------------------------------------- */
			bool InsertContext(const Context& src, HMENU handle, UINT& index, UINT& cmdid, UINT cmdid_first) {
				if (src.Empty()) return false;
				
				UINT cmdid_current = cmdid;
				MENUITEMINFO info = {};
				info.cbSize = sizeof(info);
				info.fMask = MIIM_FTYPE | MIIM_STRING | MIIM_ID;
				info.fType = MFT_STRING;
				info.wID = cmdid;
				info.dwTypeData = const_cast<char_type*>(src.Name().c_str());
				
				if (!src.Children().empty()) {
					UINT tmp = cmdid + 1;
					UINT subindex = 0;
					HMENU hsub = ::CreateMenu();
					if (!this->InsertContextContainer(src.Children(), hsub, subindex, tmp, cmdid_first)) return false;
					cmdid = tmp;
					info.fMask |= MIIM_SUBMENU;
					info.hSubMenu = hsub;
				}
				else ++cmdid;
				
				UINT offset = cmdid_current - cmdid_first;
				inserted_[offset] = src;
				if (!src.IconLocation().empty()) this->SetMenuIcon(offset, info);
				
				::InsertMenuItem(handle, index++, TRUE, &info);
				return true;
			}
			
			/* ----------------------------------------------------------------- */
			///
			/// InsertContextContainer
			///
			/// <summary>
			/// �w�肳�ꂽ�R���e�L�X�g���j���[��}�����܂��B
			/// </summary>
			///
			/* ----------------------------------------------------------------- */
			bool InsertContextContainer(const ContextContainer& src, HMENU handle, UINT& index, UINT& cmdid, UINT cmdid_first) {
				UINT first = cmdid;
				BOOST_FOREACH(const Context& cx, src) {
					this->InsertContext(cx, handle, index, cmdid, cmdid_first);
				}
				return cmdid > first;
			}
			
			/* ----------------------------------------------------------------- */
			///
			/// GetArchiveInfo
			///
			/// <summary>
			/// �����Ɏw�肳�ꂽ���k�t�@�C���Ɋi�[����Ă���t�@�C���E�t�H���_
			/// �̊e������擾���܂��B
			/// </summary>
			///
			/* ----------------------------------------------------------------- */
			void GetArchiveInfo(const string_type& path, std::vector<fileinfo>& dest) {
				string_type cmdline = cubeiceEnginePath;
				cmdline += _T(" l -scsIGNORE ");
				cmdline += _T("\"") + path + _T("\"");
				
				dest.clear();
				
				cube::popen proc;
				if (!proc.open(cmdline.c_str(), _T("r"))) return;
				string_type buffer, src;
				int status = 0;
				while ((status = proc.gets(buffer)) >= 0) {
					if (status == 2) break; // pipe closed
					else if (status == 1 && !buffer.empty()) src = buffer;
					
					clx::escape_separator<TCHAR> sep(_T(" \t"), _T("\""), _T(""));
					clx::basic_tokenizer<clx::escape_separator<TCHAR>, std::basic_string<TCHAR> > v(buffer, sep);
					
					if (buffer.find(_T("files")) != string_type::npos && buffer.find(_T("folders")) != string_type::npos && v.size() == 6) {
						try {
							fileNum = clx::lexical_cast<size_type>(v.at(2));
							folderNum = clx::lexical_cast<size_type>(v.at(4));
						}
						catch (clx::bad_lexical_cast&) {}
					}
					
					if (v.empty() || v.at(0) != _T("<>")) {
						buffer.clear();
						continue;
					}
					
					if (v.size() == 6) {
						// �t�@�C�����X�g�̍X�V
						fileinfo elem;
						elem.name = v.at(5);
						elem.size = v.at(3) != _T("-") ? clx::lexical_cast<std::size_t>(v.at(3)) : 0;
						if (v.at(1) != _T("-")) elem.time.from_string(v.at(1), string_type(_T("%Y-%m-d %H:%M:%S")));
						elem.directory = (v.at(2).find(_T('D')) != string_type::npos);
						dest.push_back( elem );
					}
					buffer.clear();
				}
				return;
			}
			
			/* ----------------------------------------------------------------- */
			///
			/// GetInstallDirectory
			///
			/// <summary>
			/// CubeICE ���C���X�g�[�����ꂽ�f�B���N�g�����擾���܂��B64bit ��
			/// Windows �� 32bit �ł� dll ���g�p����ꍇ�AInstallDirectory()
			/// �����o�֐�����ɂȂ�̂ŁA���̍ۂɂ� dll �����݂��Ă���
			/// �f�B���N�g����Ԃ��܂��B
			/// </summary>
			///
			/* ----------------------------------------------------------------- */
			string_type GetInstallDirectory(const UserSetting& setting) {
				if (!setting.InstallDirectory().empty()) return setting.InstallDirectory();
				
				char_type path[2048] = {};
				::GetModuleFileName(static_cast<HMODULE>(hInstance), path, sizeof(path) / sizeof(path[0]));
				::PathRemoveFileSpec(path);
				return string_type(path);
			}
			
		protected:
			HINSTANCE hInstance;
		private:
			std::vector<string_type> fileList;
			UserSetting ctxSetting;
			ULONG								refCount;
			ULONG								&dllRefCount;
			string_type							compFileName;
			std::vector<fileinfo>				compFileList;
			size_type							fileNum;
			size_type							folderNum;
			string_type							cubeiceEnginePath;
			bool								isDragAndDrop;
			string_type							dropPath;
			context_map inserted_;
		};
} // namespace CubeICE

#endif	// CUBEICE_CTXBASE_H
