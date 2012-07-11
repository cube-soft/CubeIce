// -*- coding: shift-jis -*-
/* ------------------------------------------------------------------------- */
/*
 *  cubeice-detail/listview-helper.h
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
#ifndef CUBEICE_DETAIL_LISTVIEW_HELPER_H
#define CUBEICE_DETAIL_LISTVIEW_HELPER_H

#include <cubeice/config.h>
#include <cubeice/user-setting.h>
#include <psdotnet/forms/listview.h>
#include <boost/lexical_cast.hpp>

#define CUBEICE_DETAIL_STR_DEFAULT      _T("�����l")
#define CUBEICE_DETAIL_STR_CUSTOM       _T("���[�U�ݒ�")
#define CUBEICE_DETAIL_STR_STRING       _T("������")
#define CUBEICE_DETAIL_STR_NUMBER       _T("�����l")
#define CUBEICE_DETAIL_STR_BOOL         _T("�^�U�l")
#define CUBEICE_DETAIL_STR_NODESET      _T("�z��")
#define CUBEICE_DETAIL_STR_CONTEXT      _T("�R���e�L�X�g���j���[")
#define CUBEICE_DETAIL_STR_SHORTCUT     _T("�V���[�g�J�b�g")
#define CUBEICE_DETAIL_STR_UNKNOWN      _T("�s��")
#define CUBEICE_DETAIL_STR_TRUE         _T("True")
#define CUBEICE_DETAIL_STR_FALSE        _T("False")
#define CUBEICE_DETAIL_STR_STRUCT       _T("{ ... }")

namespace CubeICE {
	/* --------------------------------------------------------------------- */
	/// IsDefaultValue
	/* --------------------------------------------------------------------- */
	inline bool IsDefaultValue(const string_type& condition) {
		return condition == CUBEICE_DETAIL_STR_DEFAULT;
	}
	
	/* --------------------------------------------------------------------- */
	/// IsContextKind
	/* --------------------------------------------------------------------- */
	inline bool IsContextKind(const string_type& kind) {
		return kind == CUBEICE_DETAIL_STR_CONTEXT;
	}
	
	/* --------------------------------------------------------------------- */
	/// IsShortcutKind
	/* --------------------------------------------------------------------- */
	inline bool IsShortcutKind(const string_type& kind) {
		return kind == CUBEICE_DETAIL_STR_SHORTCUT;
	}
	
	/* --------------------------------------------------------------------- */
	///
	/// IsDefaultValue
	///
	/// <summary>
	/// �w�肵�����[�U�ݒ�̒l�������l���ǂ������擾���܂��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	inline bool IsDefaultValue(const PsdotNet::Parameter::Node& src, const PsdotNet::Parameter::NodeSet& compared) {
		optional<const PsdotNet::Parameter::Node&> opt = PsdotNet::Parameter::FindOptional(compared, src.Name());
		if (!opt || src.Kind() != opt->Kind()) return false;
		
		switch (src.Kind().ToEnum()) {
		case PsdotNet::Parameter::ValueKind::String:
			return src.Value<PsdotNet::Parameter::ValueKind::String>() == opt->Value<PsdotNet::Parameter::ValueKind::String>();
		case PsdotNet::Parameter::ValueKind::Number:
			return src.Value<PsdotNet::Parameter::ValueKind::Number>() == opt->Value<PsdotNet::Parameter::ValueKind::Number>();
		case PsdotNet::Parameter::ValueKind::Bool:
			return src.Value<PsdotNet::Parameter::ValueKind::Bool>() == opt->Value<PsdotNet::Parameter::ValueKind::Bool>();
		default:
			break;
		}
		return false;
	}
	
	/* --------------------------------------------------------------------- */
	/// GetBool
	/* --------------------------------------------------------------------- */
	inline bool GetBool(const string_type& str) {
		return str == CUBEICE_DETAIL_STR_TRUE;
	}
	
	/* --------------------------------------------------------------------- */
	///
	/// GetInverse
	///
	/// <summary>
	/// �����Ɏw�肳�ꂽ������́u�t�v�̊֌W�ɂ��镶������擾���܂��B
	/// ���݁A�t�̊֌W�ɂ�����͈̂ȉ��̒ʂ�ł��B
	///
	/// - True <=> False
	/// - �����l <=> ���[�U�ݒ�
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	inline string_type GetInverse(const string_type& str) {
		// True or False
		if (str == CUBEICE_DETAIL_STR_TRUE) return CUBEICE_DETAIL_STR_FALSE;
		if (str == CUBEICE_DETAIL_STR_FALSE) return CUBEICE_DETAIL_STR_TRUE;
		
		// �����l or ���[�U�ݒ�
		if (str == CUBEICE_DETAIL_STR_DEFAULT) return CUBEICE_DETAIL_STR_CUSTOM;
		if (str == CUBEICE_DETAIL_STR_CUSTOM) return CUBEICE_DETAIL_STR_DEFAULT;
		
		return _T("");
	}
	
	/* --------------------------------------------------------------------- */
	/// GetValueKind
	/* --------------------------------------------------------------------- */
	inline PsdotNet::Parameter::ValueKind GetValueKind(const string_type& str) {
		if (str == CUBEICE_DETAIL_STR_STRING) return PsdotNet::Parameter::ValueKind::String;
		if (str == CUBEICE_DETAIL_STR_NUMBER) return PsdotNet::Parameter::ValueKind::Number;
		if (str == CUBEICE_DETAIL_STR_BOOL) return PsdotNet::Parameter::ValueKind::Bool;
		if (str == CUBEICE_DETAIL_STR_NODESET) return PsdotNet::Parameter::ValueKind::NodeSet;
		return PsdotNet::Parameter::ValueKind::Unknown;
	}
	
	/* --------------------------------------------------------------------- */
	///
	/// AddCondition
	///
	/// <summary>
	/// �w�肵�����[�U�ݒ�̒l�̏�ԁi�����l�A�܂��̓��[�U�ݒ�j��
	/// ListViewItem �ɒǉ����܂��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	inline void AddCondition(const PsdotNet::Parameter::Node& src, const PsdotNet::Parameter::NodeSet& compared, PsdotNet::Forms::ListViewItem& dest) {
		dest.SubItems().Add(PsdotNet::Forms::ListViewSubItem(IsDefaultValue(src, compared) ? CUBEICE_DETAIL_STR_DEFAULT : CUBEICE_DETAIL_STR_CUSTOM));
	}
	
	/* --------------------------------------------------------------------- */
	///
	/// AddValue
	///
	/// <summary>
	/// �l�̎�ނƓ��e�� ListViewItem �ɒǉ����܂��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	inline void AddValue(const PsdotNet::Parameter::Node& src, PsdotNet::Forms::ListViewItem& dest) {
		typedef PsdotNet::Forms::ListViewSubItem SubItemT;
		
		switch (src.Kind().ToEnum()) {
		case PsdotNet::Parameter::ValueKind::String:
			dest.SubItems().Add(SubItemT(CUBEICE_DETAIL_STR_STRING));
			dest.SubItems().Add(SubItemT(src.Value<PsdotNet::Parameter::ValueKind::String>()));
			break;
		case PsdotNet::Parameter::ValueKind::Number:
			dest.SubItems().Add(SubItemT(CUBEICE_DETAIL_STR_NUMBER));
			dest.SubItems().Add(SubItemT(boost::lexical_cast<string_type>(src.Value<PsdotNet::Parameter::ValueKind::Number>())));
			break;
		case PsdotNet::Parameter::ValueKind::Bool:
			dest.SubItems().Add(SubItemT(CUBEICE_DETAIL_STR_BOOL));
			dest.SubItems().Add(SubItemT(src.Value<PsdotNet::Parameter::ValueKind::Bool>() ? CUBEICE_DETAIL_STR_TRUE : CUBEICE_DETAIL_STR_FALSE));
			break;
		default:
			dest.SubItems().Add(SubItemT(CUBEICE_DETAIL_STR_UNKNOWN));
			dest.SubItems().Add(SubItemT());
			break;
		}
	}
} // namespace CubeICE

#endif // CUBEICE_DETAIL_LISTVIEW_HELPER_H
