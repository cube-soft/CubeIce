// -*- coding: shift-jis -*-
/* ------------------------------------------------------------------------- */
/*
 *  psdotnet/forms/msgbox.h
 *
 *  Copyright (c) 2011, clown.
 *
 *  Distributed under the Boost Software License,
 *  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 */
/* ------------------------------------------------------------------------- */
#ifndef PSDOTNET_FORMS_MEGBOX_H
#define PSDOTNET_FORMS_MGGBOX_H

#include <psdotnet/config.h>
#include <string>
#include <windows.h>
#include <tchar.h>
#include <boost/utility.hpp>
#include <psdotnet/typesafe-enum.h>
#include <psdotnet/forms/dialog-result.h>

namespace PsdotNet {
	namespace Forms {
		/* ----------------------------------------------------------------- */
		//  MsgBoxButtons
		/* ----------------------------------------------------------------- */
		namespace Detail {
			struct MsgBoxButtons_Def {
				enum type {
					OK                      = MB_OK,
					OKCancel                = MB_OKCANCEL,
					AbortRetryIgnore        = MB_ABORTRETRYIGNORE,
					YesNoCancel             = MB_YESNOCANCEL,
					YesNo                   = MB_YESNO,
					RetryCancel             = MB_RETRYCANCEL
				};
			};
		}
		typedef TypesafeEnum<Detail::MsgBoxButtons_Def> MsgBoxButtons;
		
		/* ----------------------------------------------------------------- */
		//  MsgBoxDefaultButton
		/* ----------------------------------------------------------------- */
		namespace Detail {
			struct MsgBoxDefaultButton_Def {
				enum type {
					Button1                 = MB_DEFBUTTON1,
					Button2                 = MB_DEFBUTTON2,
					Button3                 = MB_DEFBUTTON3
					//Button4                 = MB_DEFBUTTON4
				};
			};
		}
		typedef TypesafeEnum<Detail::MsgBoxDefaultButton_Def> MsgBoxDefaultButton;
		
		/* ----------------------------------------------------------------- */
		//  MsgBoxIcon
		/* ----------------------------------------------------------------- */
		namespace Detail {
			struct MsgBoxIcon_Def {
				enum type {
					None                    = 0,
					Hand                    = MB_ICONHAND,
					Question                = MB_ICONQUESTION,
					Exclamation             = MB_ICONEXCLAMATION,
					Asterisk                = MB_ICONASTERISK,
					Stop                    = MB_ICONSTOP,
					Error                   = MB_ICONERROR,
					Warning                 = MB_ICONWARNING,
					Information             = MB_ICONINFORMATION
				};
			};
		}
		typedef TypesafeEnum<Detail::MsgBoxIcon_Def> MsgBoxIcon;
		
		/* ----------------------------------------------------------------- */
		/*
		 *  MsgBox
		 *
		 *  NOTE: MessageBox �́CWin32 API ���}�N���Ƃ��Ē�`���Ă��邽��
		 *  ���p�ł��Ȃ��D���̂��߁CPsdotNet �ł́C"MessageBox" �ɑ�������
		 *  ���̂� "MsgBox" �ƕ\�L����D
		 */
		/* ----------------------------------------------------------------- */
		class MsgBox : boost::noncopyable{
		public:
			typedef TCHAR char_type;
			typedef std::basic_string<TCHAR> string_type;
			typedef std::size_t size_type;
			
		public:
			/* ------------------------------------------------------------- */
			/*
			 *  Show
			 *
			 *  �w�肵���e�L�X�g��\�����郁�b�Z�[�W �{�b�N�X��\������D
			 */
			/* ------------------------------------------------------------- */
			static DialogResult Show(const string_type& text) {
				return ShowExec(NULL, text, _T(""), MsgBoxButtons::OK);
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  Show
			 *
			 *  �w�肵���e�L�X�g�ƃL���v�V������\�����郁�b�Z�[�W�{�b�N�X
			 *  ��\������D
			 */
			/* ------------------------------------------------------------- */
			static DialogResult Show(const string_type& text, const string_type& caption) {
				return ShowExec(NULL, text, caption, MsgBoxButtons::OK);
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  Show
			 *
			 *  �w�肵���e�L�X�g�C�L���v�V�����C����у{�^����\������
			 *  ���b�Z�[�W �{�b�N�X��\������D
			 */
			/* ------------------------------------------------------------- */
			static DialogResult Show(const string_type& text, const string_type& caption,
				const MsgBoxButtons& buttons) {
				return ShowExec(NULL, text, caption, buttons.ToEnum());
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  Show
			 *
			 *  �w�肵���e�L�X�g�C�L���v�V�����C�{�^���C����уA�C�R����
			 *  �\�����郁�b�Z�[�W �{�b�N�X��\������D
			 */
			/* ------------------------------------------------------------- */
			static DialogResult Show(const string_type& text, const string_type& caption,
				const MsgBoxButtons& buttons, const MsgBoxIcon& icon) {
				return ShowExec(NULL, text, caption, buttons.ToEnum() | icon.ToEnum());
			}
			
			/* ------------------------------------------------------------- */
			/*
			 *  Show
			 *
			 *  �w�肵���e�L�X�g�C�L���v�V�����C�{�^���C�A�C�R���C�����
			 *  ����̃{�^����\�����郁�b�Z�[�W�{�b�N�X��\������D
			 */
			/* ------------------------------------------------------------- */
			static DialogResult Show(const string_type& text, const string_type& caption,
				const MsgBoxButtons& buttons, const MsgBoxIcon& icon, const MsgBoxDefaultButton& default_button) {
				return ShowExec(NULL, text, caption, buttons.ToEnum() | icon.ToEnum() | default_button.ToEnum());
			}
			
		private:
			/* ------------------------------------------------------------- */
			//  NOTE: MsgBox �N���X�̓C���X�^���X�𐶐����Ȃ��D
			/* ------------------------------------------------------------- */
			MsgBox();
			~MsgBox() {}
			
			/* ------------------------------------------------------------- */
			//  ShowExec
			/* ------------------------------------------------------------- */
			static DialogResult ShowExec(HWND owner, const string_type& text, const string_type& caption, size_type flag) {
				size_type result = MessageBox(owner, text.c_str(), caption.c_str(), flag);
				return PsdotNet::Forms::Detail::ToDialogResult(result);
			}
		};
	} // namespace Forms
} // namespace PsdotNet

#endif // PSDOTNET_FORMS_MSGBOX_H
