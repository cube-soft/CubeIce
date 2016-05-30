// -*- coding: shift-jis -*-
/* ------------------------------------------------------------------------- */
/*
 *  cubeice/user-setting/user-setting-base.h
 *
 *  Copyright (c) 2010 CubeSoft, Inc.
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
#ifndef CUBEICE_USER_SETTING_USER_SETTING_BASE_H
#define CUBEICE_USER_SETTING_USER_SETTING_BASE_H

#include <cubeice/config.h>
#include <cubeice/user-setting/v1.h>
#include <psdotnet/parameter.h>

namespace CubeICE {
	/* --------------------------------------------------------------------- */
	///
	/// UserSettingBase
	///
	/// <summary>
	/// �e�탆�[�U�ݒ�𑀍삷��N���X�̊��N���X�ł��B
	/// </summary>
	///
	/* --------------------------------------------------------------------- */
	class UserSettingBase {
	public:
		typedef CubeICE::char_type char_type;
		typedef CubeICE::string_type string_type;
		typedef CubeICE::v1::user_setting Version1;
		typedef PsdotNet::Parameter::ValueKind ValueKind;
		typedef PsdotNet::Parameter::Node Node;
		typedef PsdotNet::Parameter::NodeSet NodeSet;
		typedef PsdotNet::Parameter::Document Document;
		
	public:
		/* ----------------------------------------------------------------- */
		///
		/// constructor
		///
		/// <summary>
		/// CubeICE �̊e��ݒ荀�ڂ��K��̒l�ŏ��������܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		UserSettingBase() :
			parameters_() {
			this->Reset();
		}
		
		/* ----------------------------------------------------------------- */
		/// constructor
		/* ----------------------------------------------------------------- */
		UserSettingBase(const UserSettingBase& cp) :
			parameters_(cp.parameters_) {}
		
		/* ----------------------------------------------------------------- */
		/// destructor
		/* ----------------------------------------------------------------- */
		virtual ~UserSettingBase() {}
		
		/* ----------------------------------------------------------------- */
		/// Reset
		/* ----------------------------------------------------------------- */
		virtual void Reset() { parameters_.clear(); }
		
		/* ----------------------------------------------------------------- */
		///
		/// Upgrade
		///
		/// <summary>
		/// ���o�[�W�����̃f�[�^�\�����猻�݂̂��̂ɕϊ����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		virtual void Upgrade(const Version1& old) {}
		
		/* ----------------------------------------------------------------- */
		///
		/// LoadFromParameter
		///
		/// <summary>
		/// �����Ɏw�肳�ꂽ�p�����[�^�ۑ��p�N���X����e��ݒ����
		/// �ǂݍ��݂܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void LoadFromParameter(const NodeSet& src) {
			this->LoadFromParameterExec(src);
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// SaveToParameter
		///
		/// <summary>
		/// �����Ɏw�肳�ꂽ�p�����[�^�ۑ��p�N���X�Ɍ��݂̏���ۑ����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		void SaveToParameter(NodeSet& dest) {
			this->SaveToParameterExec(dest);
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// Parameters
		///
		/// <summary>
		/// CubeICE �̊e��ݒ荀�ڂ��擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		NodeSet& Parameters() { return parameters_; }
		const NodeSet& Parameters() const { return parameters_; }
		
		/* ----------------------------------------------------------------- */
		///
		/// FindOptional
		///
		/// <summary>
		/// name �Ŏw�肳�ꂽ CubeICE �̐ݒ荀�ڂ��擾���܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		optional<Node&> FindOptional(const string_type& name) {
			return PsdotNet::Parameter::FindOptional(parameters_, name);
		}
		
		optional<const Node&> FindOptional(const string_type& name) const {
			return PsdotNet::Parameter::FindOptional(parameters_, name);
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// Find
		///
		/// <summary>
		/// name �Ŏw�肳�ꂽ CubeICE �̐ݒ荀�ڂ��擾���܂��B�Y�����鍀�ڂ�
		/// ���݂��Ȃ��ꍇ�͗�O�����o����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		Node& Find(const string_type& name) { return PsdotNet::Parameter::Find(parameters_, name); }
		const Node& Find(const string_type& name) const { return PsdotNet::Parameter::Find(parameters_, name); }
		
	protected:
		/* ----------------------------------------------------------------- */
		///
		/// LoadFromParameterExec
		///
		/// <summary>
		/// �ėp�f�[�^�\���ł��� PsdotNet::Parameter::NodeSet ����K�v��
		/// �����𒊏o���āA���݂̃��[�U�ݒ��ǂݍ��݂܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		virtual void LoadFromParameterExec(const NodeSet& src) {
			if (src.empty()) return;
			
			BOOST_FOREACH(Node& element, parameters_) {
				optional<const Node&> node = PsdotNet::Parameter::FindOptional(src, element.Name());
				if (node) element = *node;
			}
		}
		
		/* ----------------------------------------------------------------- */
		///
		/// SaveToParameterExec
		///
		/// <summary>
		/// �ėp�I�ȃf�[�^�\���ł��� PsdotNet::Parameter::NodeSet �֌��݂�
		/// ���[�U�ݒ��ۑ����܂��B
		/// </summary>
		///
		/* ----------------------------------------------------------------- */
		virtual void SaveToParameterExec(NodeSet& dest) {
			dest.insert(dest.end(), parameters_.begin(), parameters_.end());
		}
		
	private:
		NodeSet parameters_;
	}; // class UserSettingBase
} // namespace CubeICE

#endif // CUBEICE_USER_SETTING_USER_SETTING_BASE_H
