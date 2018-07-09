/*
DelFEM (Finite Element Analysis)
Copyright (C) 2009  Nobuyuki Umetani    n.umetani@gmail.com

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*! @file
@brief �ڗ��g�U�������̗v�f�����쐬�֐��̃C���^�[�t�F�[�X
@author Nobuyuki Umetani
@sa http://ums.futene.net/wiki/FEM/46454D20666F7220416476656374696F6E2D446966667573696F6E204571756174696F6E.html
*/


#if !defined(EQN_ADVECTION_H)
#define EQN_ADVECTION_H

#if defined(__VISUALC__)
#pragma warning( disable : 4786 )
#endif

namespace Fem{
namespace Ls
{
	class CLinearSystem_Field;
	class CLinearSystem_Save;
	class CLinearSystem_SaveDiaM_Newmark;
}
namespace Field
{
	class CField;
	class CFieldWorld;
}
namespace Eqn
{

/*! @defgroup eqn_advection_diffusion �ڗ��g�U�������̘A���ꎟ�������ւ̃}�[�W����֐��Q
@ingroup FemEqnMargeFunction

�@�@
�ÓI�Ȉڗ��g�U������
@f$ \rho\frac{\partial\phi}{\partial t} = -\mu \nabla^2 \phi + f @f$

���I�Ȉڗ��g�U������
@f$\rho\frac{\partial\phi}{\partial t}+\rho v \cdot(\nabla\phi) = -\mu \nabla^2 \phi + f @f$

�ڗ��g�U�������́C����l������ɂ��������Ĉڗ������ƂƂ��ɁC�g�U����悤�Ȍ��ۂ�\���������ł��D
�Ⴆ�΁u��̗���ɃC���N�������L�����Ă����v�C�u���˂���o�鉌�����ɂ���ė������v�Ȃǂ̗l�q��\���܂��D

@pre ���̕������̐����Ƃ��āC����̏㗬�ɌŒ苫�E�������ݒ肳��Ă��Ȃ��ƘA���ꎟ���������������Ƃ��ł��܂���D

*/
/*!@{*/


/*!
@brief �ÓI�ȂQ�����ڗ��g�U�������̘A���ꎟ�������ւ̃}�[�W
@param [in,out] ls �}�[�W�����A���ꎟ������
@param [in] myu �g�U�W��
@param [in] source �ʐςɔ�Ⴗ��\�[�X��
@param [in] world ��Ǘ��N���X
@param [in] id_field_val �l���ID
@param [in] id_field_velo ���x���ID
@pre �l��̓X�J���[��łȂ���΂Ȃ�܂���D���x��͂Q���x�N�g����łȂ���΂Ȃ�܂���D
*/
bool AddLinSys_AdvectionDiffusion_Static(
    Fem::Ls::CLinearSystem_Field& ls,
	double myu, double source,
	const Fem::Field::CFieldWorld& world,
	unsigned int id_field_val, unsigned int id_field_velo, 
	unsigned int id_ea = 0 );

/*!
@brief �ÓI�Ȉڗ��g�U�������̘A���ꎟ�������ւ̃}�[�W(�����s��ۑ�)
@param [in,out] �}�[�W�����A���ꎟ������
@param [in] myu �g�U�W��
@param [in] source �ʐςɔ�Ⴗ��\�[�X��
@param [in] world ��Ǘ��N���X
@param [in] id_field_val �l���ID
@param [in] id_field_velo ���x���ID
@pre �l��̓X�J���[��łȂ���΂Ȃ�܂���D���x��͂Q���x�N�g����łȂ���΂Ȃ�܂���D
*/
bool AddLinSys_AdvectionDiffusion_Static(
	Fem::Ls::CLinearSystem_Save& ls,
	double myu, double source,
	const Fem::Field::CFieldWorld& world,
	unsigned int id_field_val, unsigned int id_field_velo, 
	unsigned int id_ea = 0 );

/*!
@brief ���I�Ȉڗ��g�U�������̘A���ꎟ�������ւ̃}�[�W
@param [in,out] �}�[�W�����A���ꎟ������
@param [in] rho ������ @f$\rho@f$
@param [in] myu �g�U�W�� @f$\mu@f$
@param [in] source �ʐςɔ�Ⴗ��\�[�X��
@param [in] id_field_val �l���ID
@param [in] id_field_velo ���x���ID
@pre �l��̓X�J���[��łȂ���΂Ȃ�܂���D���x��͂Q���x�N�g����łȂ���΂Ȃ�܂���D
*/	
bool AddLinSys_AdvectionDiffusion_NonStatic_Newmark(
	double dt, double gamma, 
	Fem::Ls::CLinearSystem_Field& ls,
	double rho, double myu, double source,
	const Fem::Field::CFieldWorld& world,
	unsigned int id_field_val, unsigned int id_field_velo, 
	unsigned int id_ea = 0 );

/*!
@brief ���I�Ȉڗ��g�U�������̘A���ꎟ�������ւ̃}�[�W(�����s��ۑ�)
@param [in,out] �}�[�W�����A���ꎟ������(�����s��ۑ��^)
@param [in] rho ������
@param [in] myu �g�U�W��
@param [in] source �ʐςɔ�Ⴗ��\�[�X��
@param [in] id_field_val �l���ID
@param [in] id_field_velo ���x���ID
@pre �l��̓X�J���[��łȂ���΂Ȃ�܂���D���x��͂Q���x�N�g����łȂ���΂Ȃ�܂���D
*/	
bool AddLinSys_AdvectionDiffusion_NonStatic_Newmark(
	Fem::Ls::CLinearSystem_SaveDiaM_Newmark& ls,
	double rho, double myu, double source,
	const Fem::Field::CFieldWorld& world,
	unsigned int id_field_val, unsigned int id_field_velo, 
	unsigned int id_ea = 0 );

/*!@}*/
}
}

#endif
