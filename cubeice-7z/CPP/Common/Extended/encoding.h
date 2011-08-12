#ifndef CUBEICE_ENCODING_H
#define CUBEICE_ENCODING_H

#include <cassert>
#include <string>
#include <map>
#include <vector>
#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>
#include <babel/babel.h>

#pragma comment(lib, "shlwapi.lib")

// Resouces
#define IDC_ICON_PICTUREBOX         1000
#define IDC_ENCODING_LISTBOX        1001
#define IDC_PREVIEW_TEXTBOX         1002

namespace cubeice {
	/* --------------------------------------------------------------------- */
	//  EncodingDialog
	/* --------------------------------------------------------------------- */
	class EncodingDialog {
	public:
		typedef TCHAR char_type;
		typedef std::basic_string<TCHAR> string_type;
		typedef std::size_t size_type;
		
		enum {
			ASCII           = babel::base_encoding::ansi,
			JIS             = babel::base_encoding::jis,
			ShiftJIS        = babel::base_encoding::sjis,
			EUCJP           = babel::base_encoding::euc,
			UTF8            = babel::base_encoding::utf8,
			UTF16           = babel::base_encoding::utf16,
			Unknown         = -1,
			Ignore          = -2
		};
	
	public:
		/* ----------------------------------------------------------------- */
		//  constructor
		/* ----------------------------------------------------------------- */
		EncodingDialog(const string_type& resource_name, const char* src, int encoding = Unknown) :
			handle_(NULL), resource_(resource_name), src_(src), encoding_(encoding), kinds_() {
			babel::init_babel();
			if (encoding_ == Unknown || encoding_ == Ignore) {
				int tmp = this->Guess(src_);
				if (tmp != Unknown) encoding_ = tmp;
			}
		}
		
		/* ----------------------------------------------------------------- */
		//  denstructor
		/* ----------------------------------------------------------------- */
		virtual ~EncodingDialog() {}
		
		/* ----------------------------------------------------------------- */
		//  ShowDialog
		/* ----------------------------------------------------------------- */
		int ShowDialog(HWND owner = NULL) {
			return this->RunDialog(owner) ? IDOK : IDCANCEL;
		}
		
		/* ----------------------------------------------------------------- */
		//  Encoding
		/* ----------------------------------------------------------------- */
		int& Encoding() { return encoding_; }
		const int& Encoding() const { return encoding_; }
		
		/* ----------------------------------------------------------------- */
		//  Handle
		/* ----------------------------------------------------------------- */
		HWND& Handle() { return handle_; }
		const HWND& Handle() const { return handle_; }
		
		/* ----------------------------------------------------------------- */
		//  Convert
		/* ----------------------------------------------------------------- */
		std::basic_string<wchar_t> Convert() {
			return this->Convert(src_, encoding_);
		}
		
		/* ----------------------------------------------------------------- */
		//  Test
		/* ----------------------------------------------------------------- */
		bool Test() {
			std::basic_string<wchar_t> converted = this->Convert();
			if (PathFileExists(converted.c_str()) || PathIsDirectory(converted.c_str())) return true;
			wchar_t dir[1024] = {};
			if (::GetTempPath(1024, dir) == 0) return false;
			std::basic_string<wchar_t> path = dir + converted;
			HANDLE test = CreateFile(path.c_str(),
				GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (!test) return false;
			::CloseHandle(test);
			::DeleteFile(path.c_str());
			return true;
		}
		
	protected:
		/* ----------------------------------------------------------------- */
		//  RunDialog
		/* ----------------------------------------------------------------- */
		virtual bool RunDialog(HWND owner) {
			int result = ::DialogBoxParam(::GetModuleHandle(NULL), resource_.c_str(), owner, StaticWndProc, reinterpret_cast<LPARAM>(this));
			return result == IDOK;
		}

		/* ----------------------------------------------------------------- */
		//  WndProc
		/* ----------------------------------------------------------------- */
		virtual bool WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
			return this->DefaultWndProc(uMsg, wParam, lParam);
		}
		
		/* ----------------------------------------------------------------- */
		//  OnCreate
		/* ----------------------------------------------------------------- */
		virtual bool OnCreate() {
			// �x�����b�Z�[�W�p�̃A�C�R����\��
			HWND handle = ::GetDlgItem(this->Handle(), IDC_ICON_PICTUREBOX);
			assert(handle != NULL);
			HICON warn = ::LoadIcon(NULL, IDI_WARNING);
			::SendMessage(handle, STM_SETIMAGE, IMAGE_ICON, reinterpret_cast<LPARAM>(warn));
			
			// ���X�g�{�b�N�X�̏�����
			handle = ::GetDlgItem(this->Handle(), IDC_ENCODING_LISTBOX);
			assert(handle != NULL);
			kinds_.clear();
			::SendMessage(handle, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(_T("Shift-JIS")));
			kinds_.push_back(ShiftJIS);
			::SendMessage(handle, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(_T("EUC-JP")));
			kinds_.push_back(EUCJP);
			::SendMessage(handle, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(_T("JIS")));
			kinds_.push_back(JIS);
			::SendMessage(handle, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(_T("Unicode (UTF-8)")));
			kinds_.push_back(UTF8);
			::SendMessage(handle, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(_T("Unicode (UTF-16)")));
			kinds_.push_back(UTF16);
			::SendMessage(handle, LB_SETCURSEL, this->EncodingToIndex(encoding_), 0);
			
			// �e�L�X�g�{�b�N�X�̏�����
			handle = ::GetDlgItem(this->Handle(), IDC_PREVIEW_TEXTBOX);
			assert(handle != NULL);
			std::basic_string<wchar_t> converted = this->Convert();
			::SendMessage(handle, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(converted.c_str()));
			
			return true;
		}
		
		/* ----------------------------------------------------------------- */
		//  OnCommand
		/* ----------------------------------------------------------------- */
		virtual bool OnCommand(WPARAM wParam, LPARAM lParam) {
			switch (LOWORD(wParam)) {
			case IDC_ENCODING_LISTBOX:
				if (HIWORD(wParam) == LBN_SELCHANGE) {
					return this->EncodingListBox_SelectedIndexChanged(wParam, lParam);
				}
				break;
			case IDC_PREVIEW_TEXTBOX:
			default:
				return this->DefaultCommandProc(wParam, lParam);
			}
			return false;
		}
		
		/* ----------------------------------------------------------------- */
		//  DefaultCommandProc
		/* ----------------------------------------------------------------- */
		bool DefaultCommandProc(WPARAM wParam, LPARAM lParam) {
			switch (LOWORD(wParam)) {
			case IDOK:
			{
				HWND handle = GetDlgItem(this->Handle(), IDC_ENCODING_LISTBOX);
				assert(handle != NULL);
				int index = ::SendMessage(handle, LB_GETCURSEL, 0, 0);
				encoding_ = this->IndexToEncoding(index);
				EndDialog(this->Handle(), IDOK);
				break;
			}
			case IDCANCEL:
				EndDialog(this->Handle(), IDCANCEL);
				return true;
			default:
				break;
			}
			return false;
		}
		
		/* ----------------------------------------------------------------- */
		//  DefaultWndProc
		/* ----------------------------------------------------------------- */
		bool DefaultWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam) {
			switch (uMsg) {
				case WM_INITDIALOG:     return this->OnCreate();
				case WM_COMMAND:        return this->OnCommand(wParam, lParam);
				default: break;
			}
			return false;
		}
		
	private: // event handlers
		/* ----------------------------------------------------------------- */
		//  DefaultWndProc
		/* ----------------------------------------------------------------- */
		bool EncodingListBox_SelectedIndexChanged(WPARAM wParam, LPARAM lParam) {
			// �ύX���ꂽ�G���R�[�f�B���O�̎擾
			HWND handle = GetDlgItem(this->Handle(), IDC_ENCODING_LISTBOX);
			assert(handle != NULL);
			int index = ::SendMessage(handle, LB_GETCURSEL, 0, 0);
			int encoding = this->IndexToEncoding(index);
			
			// �ύX���ꂽ�G���R�[�f�B���O�Ŏ��ۂɕϊ����Ă݂�
			handle = GetDlgItem(this->Handle(), IDC_PREVIEW_TEXTBOX);
			assert(handle != NULL);
			std::basic_string<wchar_t> converted = this->Convert(src_, encoding);
			::SendMessage(handle, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(converted.c_str()));
			
			return true;
		}
		
	private:
		/* ----------------------------------------------------------------- */
		//  IndexToEncoding
		/* ----------------------------------------------------------------- */
		int IndexToEncoding(int index) {
			if (index < 0 || static_cast<size_type>(index) >= kinds_.size()) return Unknown;
			return kinds_.at(index);
		}
		
		/* ----------------------------------------------------------------- */
		//  EncodingToIndex
		/* ----------------------------------------------------------------- */
		int EncodingToIndex(int encoding) {
			for (size_type i = 0; i < kinds_.size(); ++i) {
				if (encoding == kinds_.at(i)) return i;
			}
			return -1;
		}
		
		/* ----------------------------------------------------------------- */
		//  Guess
		/* ----------------------------------------------------------------- */
		int Guess(const std::basic_string<char>& src) {
			babel::analyze_result result = babel::analyze_base_encoding(src);
			switch (result.get_strict_result()) {
			case ASCII:
			case ShiftJIS:
				return ShiftJIS;
			case EUCJP:
			case JIS:
			case UTF8:
			case UTF16:
				return result.get_strict_result();
			default:
				break;
			}
			return Unknown;
		}
		
		/* ----------------------------------------------------------------- */
		//  Convert
		/* ----------------------------------------------------------------- */
		std::basic_string<wchar_t> Convert(const std::basic_string<char>& src, int encoding) {
			switch (encoding) {
			case ShiftJIS:
				return babel::sjis_to_unicode(src);
			case EUCJP:
				return babel::euc_to_unicode(src);
			case JIS:
				return babel::jis_to_unicode(src);
			case UTF8:
			{
				std::basic_string<char> tmp = src;
				this->ConvertForUTF8Mac(tmp);
				return babel::utf8_to_unicode(tmp);
			}
			case UTF16:
			default:
				break;
			}
			return this->Widen(src);
		}
		
		/* ----------------------------------------------------------------- */
		//  Widen
		/* ----------------------------------------------------------------- */
		std::basic_string<wchar_t> Widen(const std::basic_string<char>& src) {
			wchar_t buffer[2048] = {};
			int result = ::MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, buffer, sizeof(buffer) / sizeof(wchar_t));
			if (result == 0) std::basic_string<wchar_t>();
			return std::basic_string<wchar_t>(buffer);
		}
		
		/* ----------------------------------------------------------------- */
		//  ConvertForUTF8Mac
		/* ----------------------------------------------------------------- */
		void ConvertForUTF8Mac(std::basic_string<char>& src) {
			static const char *utf_8_mac_mapping[][2] = {
				{ "\xE3\x81\x8B\xE3\x82\x99", "\xE3\x81\x8C" },		// ��
				{ "\xE3\x81\x8D\xE3\x82\x99", "\xE3\x81\x8E" },		// ��
				{ "\xE3\x81\x8F\xE3\x82\x99", "\xE3\x81\x90" },		// ��
				{ "\xE3\x81\x91\xE3\x82\x99", "\xE3\x81\x92" },		// ��
				{ "\xE3\x81\x93\xE3\x82\x99", "\xE3\x81\x94" },		// ��
				{ "\xE3\x81\x95\xE3\x82\x99", "\xE3\x81\x96" },		// ��
				{ "\xE3\x81\x97\xE3\x82\x99", "\xE3\x81\x98" },		// ��
				{ "\xE3\x81\x99\xE3\x82\x99", "\xE3\x81\x9A" },		// ��
				{ "\xE3\x81\x9B\xE3\x82\x99", "\xE3\x81\x9C" },		// ��
				{ "\xE3\x81\x9D\xE3\x82\x99", "\xE3\x81\x9E" },		// ��
				{ "\xE3\x81\x9F\xE3\x82\x99", "\xE3\x81\xA0" },		// ��
				{ "\xE3\x81\xA1\xE3\x82\x99", "\xE3\x81\xA2" },		// ��
				{ "\xE3\x81\xA4\xE3\x82\x99", "\xE3\x81\xA5" },		// ��
				{ "\xE3\x81\xA6\xE3\x82\x99", "\xE3\x81\xA7" },		// ��
				{ "\xE3\x81\xA8\xE3\x82\x99", "\xE3\x81\xA9" },		// ��
				{ "\xE3\x81\xAF\xE3\x82\x99", "\xE3\x81\xB0" },		// ��
				{ "\xE3\x81\xB2\xE3\x82\x99", "\xE3\x81\xB3" },		// ��
				{ "\xE3\x81\xB5\xE3\x82\x99", "\xE3\x81\xB6" },		// ��
				{ "\xE3\x81\xB8\xE3\x82\x99", "\xE3\x81\xB9" },		// ��
				{ "\xE3\x81\xBB\xE3\x82\x99", "\xE3\x81\xBC" },		// ��
				{ "\xE3\x81\xAF\xE3\x82\x9A", "\xE3\x81\xB1" },		// ��
				{ "\xE3\x81\xB2\xE3\x82\x9A", "\xE3\x81\xB4" },		// ��
				{ "\xE3\x81\xB5\xE3\x82\x9A", "\xE3\x81\xB7" },		// ��
				{ "\xE3\x81\xB8\xE3\x82\x9A", "\xE3\x81\xBA" },		// ��
				{ "\xE3\x81\xBB\xE3\x82\x9A", "\xE3\x81\xBD" },		// ��
				{ "\xE3\x82\xAB\xE3\x82\x99", "\xE3\x82\xAC" },		// �K
				{ "\xE3\x82\xAD\xE3\x82\x99", "\xE3\x82\xAE" },		// �M
				{ "\xE3\x82\xAF\xE3\x82\x99", "\xE3\x82\xB0" },		// �O
				{ "\xE3\x82\xB1\xE3\x82\x99", "\xE3\x82\xB2" },		// �Q
				{ "\xE3\x82\xB3\xE3\x82\x99", "\xE3\x82\xB4" },		// �S
				{ "\xE3\x82\xB5\xE3\x82\x99", "\xE3\x82\xB6" },		// �U
				{ "\xE3\x82\xB7\xE3\x82\x99", "\xE3\x82\xB8" },		// �W
				{ "\xE3\x82\xB9\xE3\x82\x99", "\xE3\x82\xBA" },		// �Y
				{ "\xE3\x82\xBB\xE3\x82\x99", "\xE3\x82\xBC" },		// �[
				{ "\xE3\x82\xBD\xE3\x82\x99", "\xE3\x82\xBE" },		// �]
				{ "\xE3\x82\xBF\xE3\x82\x99", "\xE3\x83\x80" },		// �_
				{ "\xE3\x83\x81\xE3\x82\x99", "\xE3\x83\x82" },		// �a
				{ "\xE3\x83\x84\xE3\x82\x99", "\xE3\x83\x85" },		// �d
				{ "\xE3\x83\x86\xE3\x82\x99", "\xE3\x83\x87" },		// �f
				{ "\xE3\x83\x88\xE3\x82\x99", "\xE3\x83\x89" },		// �h
				{ "\xE3\x83\x8F\xE3\x82\x99", "\xE3\x83\x90" },		// �o
				{ "\xE3\x83\x92\xE3\x82\x99", "\xE3\x83\x93" },		// �r
				{ "\xE3\x83\x95\xE3\x82\x99", "\xE3\x83\x96" },		// �u
				{ "\xE3\x83\x98\xE3\x82\x99", "\xE3\x83\x99" },		// �x
				{ "\xE3\x83\x9B\xE3\x82\x99", "\xE3\x83\x9C" },		// �{
				{ "\xE3\x83\x8F\xE3\x82\x9A", "\xE3\x83\x91" },		// �p
				{ "\xE3\x83\x92\xE3\x82\x9A", "\xE3\x83\x94" },		// �s
				{ "\xE3\x83\x95\xE3\x82\x9A", "\xE3\x83\x97" },		// �v
				{ "\xE3\x83\x98\xE3\x82\x9A", "\xE3\x83\x9A" },		// �y
				{ "\xE3\x83\x9B\xE3\x82\x9A", "\xE3\x83\x9D" },		// �|
			};
			
			for(unsigned int i = 0 ; i < sizeof(utf_8_mac_mapping) / sizeof(utf_8_mac_mapping[0]); ++i) {
				this->ReplaceAll(src, utf_8_mac_mapping[i][0], utf_8_mac_mapping[i][1]);
			}
		}

		/* --------------------------------------------------------------------- */
		//  ReplaceAll
		/* --------------------------------------------------------------------- */
		void ReplaceAll(std::basic_string<char>& s, const std::basic_string<char>& sch, const std::basic_string<char>& rep) {
			if (s.empty()) return;
			
			std::basic_string<char>::size_type pos = 0;
			while ((pos = s.find(sch, pos)) != std::basic_string<char>::npos) {
				s.replace(pos, sch.length(), rep);
				pos += rep.length();
			}
		}
	private: // static member functions
		/* ----------------------------------------------------------------- */
		//  StaticWndProc
		/* ----------------------------------------------------------------- */
		static BOOL CALLBACK StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
			switch (uMsg) {
			case WM_INITDIALOG:
			{
				EncodingDialog* instance = reinterpret_cast<EncodingDialog*>(lParam);
				instance->Handle() = hWnd;
				::SetWindowLong(hWnd, GWL_USERDATA, reinterpret_cast<LONG>(instance));
				break;
			}
			default:
				break;
			}
			
			LONG ptr = ::GetWindowLong(hWnd, GWL_USERDATA);
			if (ptr != 0) {
				EncodingDialog* instance = reinterpret_cast<EncodingDialog*>(ptr);
				return instance->WndProc(uMsg, wParam, lParam) ? TRUE : FALSE;
			}
			return FALSE;
		}
		
	protected:
		HWND handle_;
		string_type resource_;
		std::basic_string<char> src_;
		int encoding_;
		std::vector<int> kinds_;
	};
	
	/* --------------------------------------------------------------------- */
	//  GetEncoding
	/* --------------------------------------------------------------------- */
	inline int GetEncoding() {
		TCHAR buffer[32] = {};
		DWORD result = ::GetEnvironmentVariable(_T("Encoding"), buffer, sizeof(buffer) / sizeof(TCHAR));
		if (result == 0) return EncodingDialog::Unknown;
		return _ttoi(buffer);
	}
	
	/* --------------------------------------------------------------------- */
	//  SetEncoding
	/* --------------------------------------------------------------------- */
	inline void SetEncoding(int encoding) {
		TCHAR buffer[32] = {};
		::_itot_s(encoding , buffer, sizeof(buffer) / sizeof(TCHAR), 10);
		::SetEnvironmentVariable(_T("Encoding"), buffer);
	}
	
	/* --------------------------------------------------------------------- */
	//  SetEncoding
	/* --------------------------------------------------------------------- */
	inline void SetEncoding(const std::basic_string<TCHAR>& name) {
		int encoding = EncodingDialog::Unknown;
		if (name == _T("SJIS")) encoding = EncodingDialog::ShiftJIS;
		else if (name == _T("EUC")) encoding = EncodingDialog::EUCJP;
		else if (name == _T("JIS")) encoding = EncodingDialog::JIS;
		else if (name == _T("UTF8") || name == _T("UTF-8")) encoding = EncodingDialog::UTF8;
		else if (name == _T("UTF16") || name == _T("UTF-16")) encoding = EncodingDialog::UTF16;
		else if (name == _T("IGNORE")) encoding = EncodingDialog::Ignore;
		
		if (encoding != EncodingDialog::Unknown) SetEncoding(encoding);
	}
	
	/* --------------------------------------------------------------------- */
	//  EncodingToStringA
	/* --------------------------------------------------------------------- */
	inline std::basic_string<char> EncodingToStringA(int encoding) {
		switch (encoding) {
			case EncodingDialog::ShiftJIS: return "SJIS";
			case EncodingDialog::EUCJP:    return "EUC";
			case EncodingDialog::JIS:      return "JIS";
			case EncodingDialog::UTF8:     return "UTF8";
			case EncodingDialog::UTF16:    return "UTF16";
			default: break;
		}
		return std::basic_string<char>();
	}
}

#endif // CUBEICE_ENCODING_H
