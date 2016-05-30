// -*- coding: shift-jis -*-
/* ------------------------------------------------------------------------- */
/*
 *  cubeice/user-setting/user-filtering-setting.h
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
#ifndef CUBEICE_USER_SETTING_USER_FILTERING_SETTING_H
#define CUBEICE_USER_SETTING_USER_FILTERING_SETTING_H

#include <cubeice/config.h>
#include <cubeice/user-setting/user-setting-base.h>
#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>

namespace CubeICE {
	/* --------------------------------------------------------------------- */
	///
	/// UserFilteringSetting
	///
	/// <summary>
	/// CubeICE �ň��k�E�𓀂��s���ۂɃt�B���^�����O����t�@�C�����̈ꗗ��
	/// �擾�E�ۑ�����N���X�ł��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	class UserFilteringSetting : public UserSettingBase {
	public:
		typedef std::vector<string_type> container_type;
		
	public:
		/* ----------------------------------------------------------------- */
		/// constructor
		/* ----------------------------------------------------------------- */
		UserFilteringSetting() :
			UserSettingBase(), v_() {
			this->Reset();
		}
		
		/* ----------------------------------------------------------------- */
		/// constructor
		/* ----------------------------------------------------------------- */
		virtual ~UserFilteringSetting() {}
		
		/* ----------------------------------------------------------------- */
		///
		/// Reset
		///
		/// <summary>
		/// CubeICE �ň��k�E�𓀂���ۂɃt�B���^�����O����t�@�C�����ꗗ��
		/// �����l��ݒ肵�܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		virtual void Reset() {
			v_.clear();
			v_.push_back(_T(".DS_Store"));
			v_.push_back(_T("Thumbs.db"));
			v_.push_back(_T("__MACOSX"));
			v_.push_back(_T("desktop.ini"));
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// Upgrade
		///
		/// <summary>
		/// ���o�[�W�����̃f�[�^�\�����猻�݂̂��̂ɕϊ����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		virtual void Upgrade(const Version1& old) {
			v_.clear();
			BOOST_FOREACH(const string_type& str, old.filters()) {
				if (!str.empty()) v_.push_back(str);
			}
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// Empty
		///
		/// <summary>
		/// �t�B���^�����O����t�@�C����t�H���_��\���������� 1 ��
		/// ���݂��Ă��Ȃ����ǂ������擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		bool Empty() const { return v_.empty(); }
		
		/* ----------------------------------------------------------------- */
		///
		/// Count
		///
		/// <summary>
		/// �t�B���^�����O����t�@�C����t�H���_��\����������̐����擾
		/// ���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		container_type::size_type Count() const { return v_.size(); }
		
		/* ----------------------------------------------------------------- */
		///
		/// Contains
		///
		/// <summary>
		/// �w�肵�������񂪃t�B���^�����O����t�@�C����t�H���_��\����
		/// ������Q�̒��Ɋ܂܂�Ă��邩�ǂ������m�F���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		bool Contains(const string_type& str) const {
			return std::find(v_.begin(), v_.end(), str) != v_.end();
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// Add
		///
		/// <summary>
		/// �w�肵����������t�B���^�����O����t�@�C����t�H���_��\����
		/// ������Q�ɒǉ����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void Add(const string_type& str) { if (!this->Contains(str)) v_.push_back(str); }
		
		/* ----------------------------------------------------------------- */
		///
		/// Delete
		///
		/// <summary>
		/// �w�肵����������t�B���^�����O����t�@�C����t�H���_��\����
		/// ������Q����폜���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void Delete(const string_type& str) {
			container_type::iterator pos = std::find(v_.begin(), v_.end(), str);
			if (pos != v_.end()) v_.erase(pos);
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// Clear
		///
		/// <summary>
		/// �t�B���^�����O����t�@�C����t�H���_��\����������Q��S��
		/// �������܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void Clear() { v_.clear(); }
		
		/* ----------------------------------------------------------------- */
		///
		/// Data
		///
		/// <summary>
		/// �t�B���^�����O����t�@�C����t�H���_��\����������Q���擾
		/// ���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		const container_type& Data() const { return v_; }
		
	protected:
		/* ----------------------------------------------------------------- */
		/// LoadFromParameterExec
		/* ----------------------------------------------------------------- */
		virtual void LoadFromParameterExec(const NodeSet& src) {
			optional<const Node&> node = PsdotNet::Parameter::FindOptional(src, _T("Filtering"));
			if (!node) return;
			
			v_.clear();
			container_type tmp;
			boost::algorithm::split(tmp, node->Value<ValueKind::String>(), boost::is_any_of(_T("|")));
			BOOST_FOREACH(const string_type& str, tmp) {
				if (!tmp.empty()) v_.push_back(str);
			}
		}
		
		/* ----------------------------------------------------------------- */
		/// SaveToParameterExec
		/* ----------------------------------------------------------------- */
		virtual void SaveToParameterExec(NodeSet& dest) {
			dest.push_back(Node(_T("Filtering"), boost::algorithm::join(v_, _T("|"))));
		}
		
	private:
		container_type v_;
	}; // class UserFilteringSetting
} // namespace CubeICE

#endif // CUBEICE_USER_SETTING_USER_FILTERING_SETTING_H
