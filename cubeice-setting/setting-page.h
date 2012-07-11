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
	///
	/// SettingPage
	///
	/// <summary>
	/// SettingDialog �ɕ\��������e�y�[�W�̊��N���X�ł��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	class SettingPage : public PsdotNet::Forms::PropertyPage {
	private:
		typedef PsdotNet::Forms::PropertyPage super;
		
	public:
		typedef boost::shared_ptr<bool> bool_ptr;
		
	public:
		/* ----------------------------------------------------------------- */
		/// constructor
		/* ----------------------------------------------------------------- */
		SettingPage(const string_type& resource_name, UserSetting& data, bool_ptr changed) :
			super(resource_name), data_(data), changed_(changed) {
			this->InitializeComponent();
		}
		
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
		bool Changed() const { return *changed_; }
		
		/* ----------------------------------------------------------------- */
		///
		/// Changed
		///
		/// <summary>
		/// �f�[�^�ɑ΂��āA���炩�̏C�����s��ꂽ���ǂ�����ݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void Changed(bool x) { *changed_ = x; }
		
	protected:
		/* ----------------------------------------------------------------- */
		///
		/// InitializeComponent
		///
		/// <summary>
		/// �E�B���h�E��������i�n���h���̎擾�ς݁j�̏����������s���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		virtual void OnCreateControl() {}
		
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
			if (nmhdr->code == PSN_APPLY && this->Changed()) {
				data_.Save();
				this->Changed(false);
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
					this->Changed(true);
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
		/// InitializeComponent
		///
		/// <summary>
		/// �����ݒ���s���܂��B�E�B���h�E��������i�n���h���̎擾�ς݁j
		/// �̏��������ɂ��Ă� OnCreateControl() �ɋL�q���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void InitializeComponent() {}
		
	private:
		UserSetting& data_;
		bool_ptr changed_;
	}; // class SettingPage
} // namespace CubeICE

#endif // CUBEICE_SETTING_SETTING_PAGE_H
