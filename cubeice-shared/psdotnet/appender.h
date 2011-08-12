// -*- coding: shift-jis -*-
/* ------------------------------------------------------------------------- */
/*
 *  psdotnet/appender.h
 *
 *  Copyright (c) 2011, clown.
 *
 *  Distributed under the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 */
/* ------------------------------------------------------------------------- */
#ifndef PSDOTNET_APPENDER_H
#define PSDOTNET_APPENDER_H

#include <psdotnet/config.h>
#include <windows.h>
#include <tchar.h>
#include <boost/lexical_cast.hpp>
#include <psdotnet/logger.h>

#pragma warning(push)
#pragma warning(disable:4996)

namespace PsdotNet {
	/* --------------------------------------------------------------------- */
	//  FileAppender
	/* --------------------------------------------------------------------- */
	class FileAppender : public IAppender {
	public:
		/* ----------------------------------------------------------------- */
		//  options
		/* ----------------------------------------------------------------- */
		enum {
			AppendMode                  = 0x0001,   // �����ɒǉ����[�h�Ńt�@�C�����J��
			FlushOnWrite                = 0x0002,   // �������ޓx�Ƀt���b�V������
			CloseOnWrite                = 0x0004,   // �������ޓx�Ƀt�@�C�������
			WriteDateTime               = 0x0010,   // �������o�͂���
			WriteLevel                  = 0x0020,   // ���O���x�����o�͂���
			WriteFileAndLine            = 0x0040,   // �t�@�C�����C�s�ԍ����o�͂���
			WriteAll                    = 0x0070    // �����C���O���x���C�t�@�C�����C�s�ԍ����o�͂���
		};
		
		/* ----------------------------------------------------------------- */
		//  constructor
		/* ----------------------------------------------------------------- */
		explicit FileAppender(const string_type& path, const size_type& options = AppendMode | WriteAll) :
			IAppender(), handle_(NULL), path_(path), options_(options) {
			if ((options_ & AppendMode) == 0) {
				handle_ = this->Open(path_);
				this->Clear();
				this->Close();
			}
		}
		
		/* ----------------------------------------------------------------- */
		//  destructor
		/* ----------------------------------------------------------------- */
		virtual ~FileAppender() {}
		
		/* ----------------------------------------------------------------- */
		//  Set
		/* ----------------------------------------------------------------- */
		virtual void Set(const LogInfo& info) {
			info_ = info;
		}
		
		/* ----------------------------------------------------------------- */
		//  operator()
		/* ----------------------------------------------------------------- */
		virtual void operator()(const char_type* format, ...) {
			string_type message;
			
			if ((options_ & WriteDateTime) != 0) {
				message += this->GetDateTimeStr(info_);
				message += _T(" ");
			}
			
			if ((options_ & WriteLevel) != 0) {
				message += this->GetLevelStr(info_);
				message += _T(" ");
			}
			
			if ((options_ & WriteFileAndLine) != 0) {
				message += this->GetFileStr(info_);
				message += _T(":");
				message += this->GetLineStr(info_);
				message += _T(" ");
			}
			
			static const int init_size = 1024;
			char_type buf[init_size];
			va_list ap;
			va_start(ap, format);
			int n = _vsntprintf(buf, init_size, format, ap);
			va_end(ap);
			
			if (n >= 0 && n < init_size) message += buf;
			else if (n >= init_size) {
				char_type* retry = new char_type[n + 1];
				va_start(ap, format);
				n = _vsntprintf(retry, n + 1, format, ap);
				va_end(ap);
				if (n >= 0) message += buf;
				delete [] retry;
			}
			
			message += _T("\r\n");
			
			if (!handle_) handle_ = this->OpenWithAppendMode(path_);
			this->Write(message);
			if ((options_ & CloseOnWrite) != 0) this->Close();
			else if ((options_ & FlushOnWrite) != 0) this->Flush();
		}
		
	private:
		/* --------------------------------------------------------------------- */
		//  Open
		/* --------------------------------------------------------------------- */
		HANDLE Open(const string_type& path) {
			return CreateFile(
				path.c_str(),                                               // �t�@�C����
				GENERIC_WRITE,                                              // �A�N�Z�X���[�h
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,     // ���L���[�h
				NULL,                                                       // �Z�L�����e�B�L�q�q
				OPEN_ALWAYS,                                                // �쐬���@
				FILE_ATTRIBUTE_NORMAL,                                      // �t�@�C������
				NULL
			);
		}
		
		/* --------------------------------------------------------------------- */
		//  OpenWithAppendMode
		/* --------------------------------------------------------------------- */
		HANDLE OpenWithAppendMode(const string_type& path) {
			HANDLE dest = this->Open(path);
			if (dest) SetFilePointer(dest, 0, NULL, FILE_END);
			return dest;
		}
		
		/* --------------------------------------------------------------------- */
		//  Close
		/* --------------------------------------------------------------------- */
		void Close() {
			if (handle_) CloseHandle(handle_);
			handle_ = NULL;
		}
		
		/* --------------------------------------------------------------------- */
		//  Write
		/* --------------------------------------------------------------------- */
		void Write(const string_type& message) {
			if (handle_) {
				DWORD written = 0;
				WriteFile(handle_, message.c_str(), message.size() * sizeof(char_type), &written, NULL);
			}
		}
		
		/* --------------------------------------------------------------------- */
		//  Flush
		/* --------------------------------------------------------------------- */
		void Flush() {
			if (handle_) FlushFileBuffers(handle_);
		}
		
		/* --------------------------------------------------------------------- */
		//  Clear
		/* --------------------------------------------------------------------- */
		void Clear() {
			if (handle_) {
				SetFilePointer(handle_, 0, NULL, FILE_BEGIN);
				SetEndOfFile(handle_);
			}
		}
		
		/* --------------------------------------------------------------------- */
		//  GetLevelStr
		/* --------------------------------------------------------------------- */
		string_type GetLevelStr(const LogInfo& info) {
			switch (info.Level().ToEnum()) {
			case LogLevel::Trace: return string_type(_T("[TRACE]"));
			case LogLevel::Debug: return string_type(_T("[DEBUG]"));
			case LogLevel::Info:  return string_type(_T("[INFO ]"));
			case LogLevel::Warn:  return string_type(_T("[WARN ]"));
			case LogLevel::Error: return string_type(_T("[ERROR]"));
			case LogLevel::Fatal: return string_type(_T("[FATAL]"));
			default: break;
			}
			return string_type(_T("[?????]"));
		}
		
		/* --------------------------------------------------------------------- */
		//  GetDateTimeStr
		/* --------------------------------------------------------------------- */
		string_type GetDateTimeStr(const LogInfo& info) {
			return info_.Date() + _T(" ") + info_.Time();
		}
		
		/* --------------------------------------------------------------------- */
		//  GetFileStr
		/* --------------------------------------------------------------------- */
		string_type GetFileStr(const LogInfo& info) {
			string_type::size_type pos = info.File().find_last_of(_T('\\'));
			return (pos != string_type::npos) ? info.File().substr(pos + 1) : info.File();
		}
		
		/* --------------------------------------------------------------------- */
		//  GetLineStr
		/* --------------------------------------------------------------------- */
		string_type GetLineStr(const LogInfo& info) {
			return boost::lexical_cast<string_type>(info.Line());
		}
		
	private:
		HANDLE handle_;
		string_type path_;
		size_type options_;
		LogInfo info_;
	};
} // namespace PsdotNet

#pragma warning(pop)

#endif // PSDOTNET_APPENDER_H
