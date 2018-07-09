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
@brief �g�U�������̗v�f�����쐬���̃C���^�[�t�F�[�X
@author Nobuyuki Umetani
*/

#if !defined(EQN_DIFFUSION_H)
#define EQN_DIFFUSION_H

#if defined(__VISUALC__)
#pragma warning( disable : 4786 )
#endif

#include <vector>

namespace Fem
{
namespace Ls{
	class CLinearSystem_Field;
	class CLinearSystem_SaveDiaM_Newmark;
	class CPreconditioner;
}
namespace Field{
	class CField;
	class CFieldWorld;
}

namespace Eqn
{

/*! @defgroup eqn_diffusion �g�U��������A���ꎟ�������Ƀ}�[�W����֐��Q
@ingroup FemEqnMargeFunction
�@�@
@f$ \rho \frac{\partial\phi}{\partial t} = -\mu \nabla^2\phi + f@f$

�g�U�������Ƃ́C�Z�x���z�ɔ�Ⴕ�ė���(Flux)�����肳���悤�ȏ�̕��������L�q����Δ����������ł��D
�Ⴆ�ΔM��C�ڗ��̂Ȃ���̔Z�x�ω��Ȃǂ�����ɏ]���܂��D
*/
//! @{

/*!
@brief �g�U�������̃}�[�W
@param [in,out] ls �A���ꎟ������
@param [in] rho �M�e�� @f$\rho@f$
@param [in] alpha �M�g�U�W�� @f$\mu@f$
@param [in] source �\�[�X @f$ f @f$
@param [in] world ��Ǘ��N���X
@param [in] id_field_val �l���ID
*/	
bool AddLinSys_Diffusion(
	double dt, double gamma,
	Fem::Ls::CLinearSystem_Field& ls,
	double rho, double alpha, double source,
	const Fem::Field::CFieldWorld& world, unsigned int id_field_val,
	unsigned int id_ea = 0 );

/*!
@brief ���Ώ́C�g�U�������̃}�[�W
@param [in,out] ls �A���ꎟ������
@param [in] rho �M�e�� @f$\rho@f$
@param [in] alpha �M�g�U�W�� @f$\mu@f$
@param [in] source �\�[�X @f$ f @f$
@param [in] world ��Ǘ��N���X
@param [in] id_field_val �l���ID
@remark ���͎��͂����Ɉ�v���Ă���Ƃ��邪�C���̂�����������͂ɂ���D
*/	
bool AddLinSys_Diffusion_AxSym(
	double dt, double gamma,
	Fem::Ls::CLinearSystem_Field& ls,
	double rho, double alpha, double source,
	const Fem::Field::CFieldWorld& world, unsigned int id_field_val,
	unsigned int id_ea = 0 );


/*!
@brief �g�U������(�����s��ۑ�)�̃}�[�W
@param [in,out] ls �A���ꎟ������(�����s��ۑ��^)
@param [in] rho �M�e�� @f$\rho@f$
@param [in] alpha �M�g�U�W�� @f$\mu@f$
@param [in] source �\�[�X @f$ f @f$
@param [in] world ��Ǘ��N���X
@param [in] id_field_val �l���ID
*/
bool AddLinSys_Diffusion(
	Fem::Ls::CLinearSystem_SaveDiaM_Newmark& ls,
	double rho, double alpha, double source,
	const Fem::Field::CFieldWorld& world,
	unsigned int id_field_val, unsigned int id_ea = 0 );

//! @}

}	// end namespace Eqn
}	// end namespace Fem

#endif
