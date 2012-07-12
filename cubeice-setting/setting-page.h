// -*- coding: shift-jis -*-
/* ------------------------------------------------------------------------- */
/*
 *  cubeice-setting/setting-page.h
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
#ifndef CUBEICE_SETTING_SETTING_PAGE_H
#define CUBEICE_SETTING_SETTING_PAGE_H

#include "cubeice-setting.h"
#include <boost/shared_ptr.hpp>
#include <psdotnet/forms/property-page.h>

namespace CubeICE {
	/* --------------------------------------------------------------------- */
	/// ChangeKind
	/* --------------------------------------------------------------------- */
	namespace ChangeKind {
		enum { None, Normal, RequireAdmin };
	}
	
	/* --------------------------------------------------------------------- */
	///
	/// SettingPage
	///
	/// <summary>
	/// SettingDialog �ɕ\��������e�y�[�W�̊��N���X�ł��B
	/// �v���p�e�B�V�[�g�ł́AOK �{�^����K�p�{�^���̂̃C�x���g���V�[�g����
	/// �\������Ă���S�Ẵy�[�W�ɒʒm����邽�߁Achanged �ƌ������L
	/// �ϐ���p���āA�ۑ�������������s���Ȃ��悤�ɂ��܂��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	class SettingPage : public PsdotNet::Forms::PropertyPage {
	private:
		typedef PsdotNet::Forms::PropertyPage super;
		
	public:
		typedef boost::shared_ptr<int> int_ptr;
		
	public:
		/* ----------------------------------------------------------------- */
		/// constructor
		/* ----------------------------------------------------------------- */
		SettingPage(const string_type& resource_name, UserSetting& data, int_ptr changed) :
			super(resource_name), data_(data), changed_(changed) {}
		
		/* ----------------------------------------------------------------- */
		/// destructor
		/* ----------------------------------------------------------------- */
		virtual ~SettingPage() {}
		
		/* ----------------------------------------------------------------- */
		///
		/// Data
		///
		/// <summary>
		/// �e�탆�[�U�ݒ��ێ����Ă���f�[�^���擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		UserSetting& Data() { return data_; }
		const UserSetting& Data() const { return data_; }
		
		/* ----------------------------------------------------------------- */
		///
		/// Changed
		///
		/// <summary>
		/// �f�[�^�ɑ΂��āA���炩�̏C�����s��ꂽ���ǂ������擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		int Changed() const { return *changed_; }
		
		/* ----------------------------------------------------------------- */
		///
		/// Changed
		///
		/// <summary>
		/// �f�[�^�ɑ΂��āA���炩�̕ύX���s��ꂽ���ǂ�����ݒ肵�܂��B
		/// �ݒ肷��l�́A�ȉ��̒ʂ�ł��B
		///
		///   0: �ۑ����K�v�ȕύX�Ȃ�
		///   1: ���[�U�����ŕۑ����K�v�ȕύX������
		///   2: �Ǘ��Ҍ����ŕۑ����K�v�ȕύX������
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void Changed(int level) { *changed_ = level; }
		
	protected:
		/* ----------------------------------------------------------------- */
		///
		/// OnNotify
		///
		/// <summary>
		/// ���݂̃��[�U�ݒ��ۑ����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		virtual void OnNotify(PsdotNet::Forms::Message& m) {
			NMHDR* nmhdr = reinterpret_cast<NMHDR*>(m.LParam());
			if (nmhdr->code == PSN_APPLY && this->Changed() >= ChangeKind::Normal) {
				data_.Save();
				if (this->Changed() >= ChangeKind::RequireAdmin) this->SaveAsAdministrator();
				this->Changed(ChangeKind::None);
			}
			m.Result(TRUE);
		}
		
		/* ----------------------------------------------------------------- */
		/// WndProc
		/* ----------------------------------------------------------------- */
		virtual void WndProc(PsdotNet::Forms::Message& m) {
			switch (m.Msg()) {
			case WM_COMMAND:
				this->OnCommand(m);
				if (m.Result()) {
					PropSheet_Changed(::GetParent(this->Handle()) , this->Handle());
					if (this->Changed() == 0) this->Changed(ChangeKind::Normal);
				}
				break;
			default:
				super::WndProc(m);
				break;
			}
		}
		
	private:
		/* ----------------------------------------------------------------- */
		///
		/// SaveAsAdministrator
		///
		/// <summary>
		/// �Ǘ��Ҍ����ŕύX���ꂽ���[�U�ݒ��ۑ����܂��B���݁A�Ǘ��Ҍ���
		/// �̕K�v�ȕύX�́A�֘A�t���̕ύX�݂̂ł��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void SaveAsAdministrator() {
			::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
			string_type associate = data_.InstallDirectory() + _T("\\") + CUBEICE_ASSOCIATE_NAME;
			SHELLEXECUTEINFO info = {};
			info.cbSize = sizeof(SHELLEXECUTEINFO);
			info.lpFile = associate.c_str();
			::ShellExecuteEx(&info);
			::CoUninitialize();
		}
		
	private:
		UserSetting& data_;
		int_ptr changed_;
	}; // class SettingPage
} // namespace CubeICE

#endif // CUBEICE_SETTING_SETTING_PAGE_H
