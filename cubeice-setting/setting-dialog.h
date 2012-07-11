// -*- coding: shift-jis -*-
/* ------------------------------------------------------------------------- */
/*
 *  cubeice-setting/setting-dialog.h
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
#ifndef CUBEICE_SETTING_SETTING_DIALOG_H
#define CUBEICE_SETTING_SETTING_DIALOG_H

#include "cubeice-setting.h"
#include <boost/shared_ptr.hpp>
#include <psdotnet/forms/property-dialog.h>
#include "general-page.h"
#include "compression-page.h"
#include "decompression-page.h"
#include "filtering-page.h"
#include "version-page.h"

namespace CubeICE {
	/* --------------------------------------------------------------------- */
	///
	/// SettingDialog
	///
	/// <summary>
	/// CubeICE の設定画面を表すクラスです。
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	class SettingDialog : public PsdotNet::Forms::PropertyDialog {
	private:
		typedef PsdotNet::Forms::PropertyDialog super;
		
	public:
		typedef boost::shared_ptr<bool> bool_ptr;
		
	public:
		/* ----------------------------------------------------------------- */
		/// constructor
		/* ----------------------------------------------------------------- */
		explicit SettingDialog(UserSetting& data) :
			data_(data), changed_(new bool(false)),
			general_(data_, changed_),
			compression_(data_, changed_),
			decompression_(data_, changed_),
			filtering_(data_, changed_),
			version_(data_, changed_) {
			this->InitializeComponent();
		}
		
		/* ----------------------------------------------------------------- */
		/// destructor
		/* ----------------------------------------------------------------- */
		virtual ~SettingDialog() {}
		
	private:
		/* ----------------------------------------------------------------- */
		///
		/// InitializeComponent
		///
		/// <summary>
		/// 初期設定を行います。
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void InitializeComponent() {
			this->Text(_T("CubeICE 設定 "));
			this->Icon(PsdotNet::Drawing::Icon(PsdotNet::Drawing::Icon::Type::Resource, _T("IDI_APP")));
			
			this->Add(general_);
			this->Add(compression_);
			this->Add(decompression_);
			this->Add(filtering_);
			this->Add(version_);
		}
		
	private:
		UserSetting& data_;
		bool_ptr changed_;
		GeneralPage general_;
		CompressionPage compression_;
		DecompressionPage decompression_;
		FilteringPage filtering_;
		VersionPage version_;
	};
} // namespace CubeICE

#endif // CUBEICE_SETTING_SETTING_DIALOG_H
