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
@brief �ő̂̕������N���X�̃C���^�[�t�F�[�X
@author Nobuyuki Umetani
*/

#if defined(__VISUALC__)
#pragma warning( disable : 4786 )
#endif

#if !defined(EQN_SYS_SOLID_H)
#define EQN_SYS_SOLID_H

#include <vector>
#include <iostream>

#include "delfem/eqnsys.h"

namespace Fem{

namespace Field{
	class CField;
	class CFieldWorld;
}
namespace Eqn
{

/*! 
@brief �R�����ő̕������N���X
(Linear�ƌ����Ă���͕̂����n�����`�̈Ӗ��D�􉽊w�I����`���Ӗ����Ă���̂ł͂Ȃ��D����킵���̂ŏ����I�ɕύX�\��)
@ingroup FemEqnObj
*/
class CEqn_Solid3D_Linear : public CEqnSystem
{
public:
	CEqn_Solid3D_Linear();
	CEqn_Solid3D_Linear(Fem::Field::CFieldWorld& world);
	CEqn_Solid3D_Linear(const unsigned int id_field_val, Fem::Field::CFieldWorld& world);
			
	// ���z���\�֐�
	virtual bool SetDomain_Field(unsigned int id_field, Fem::Field::CFieldWorld& world);
	virtual bool Solve(Fem::Field::CFieldWorld& world);

	// �Œ苫�E������ǉ�&�폜����
	virtual bool         AddFixField(   unsigned int id_field,                  Fem::Field::CFieldWorld& world, int idof = -1);
	virtual unsigned int AddFixElemAry( unsigned int id_ea,                     Fem::Field::CFieldWorld& world, int idof = -1);
	virtual unsigned int AddFixElemAry( const std::vector<unsigned int>& aIdEA, Fem::Field::CFieldWorld& world, int idof = -1);
	virtual bool ClearFixElemAry( unsigned int id_ea, Fem::Field::CFieldWorld& world );
	virtual void ClearFixElemAry();

	unsigned int GetIdField_Disp() const { return m_IdFieldDisp; }

	// ��virtual
	void SetYoungPoisson( double young, double poisson ){
		m_lambda = young*poisson / ( (1.0+poisson)*(1-2.0*poisson) );
		m_myu = young / ( 2.0*(1.0+poisson) );
		this->m_is_cleared_value_ls = true;
		this->m_is_cleared_value_prec = true;
	}
	//! �d�͉����x�ݒ�
	void SetGravitation( double g_x, double g_y, double g_z ){
		m_g_x = g_x;
		m_g_y = g_y;
		m_g_z = g_z;
		this->m_is_cleared_value_ls = true;
		this->m_is_cleared_value_prec = true;
	}

	void SetGeometricalNonLinear(){
		if( m_IsGeomNonlin ) return;
		m_IsGeomNonlin = true;
		m_IsSaveStiffMat = false;
		this->m_is_cleared_value_ls = true;
		this->m_is_cleared_value_prec = true;
	}
	void UnSetGeometricalNonLinear(){
		if( !m_IsGeomNonlin ) return;
		m_IsGeomNonlin = false;
		this->m_is_cleared_value_ls = true;
		this->m_is_cleared_value_prec = true;
	}

	void SetSaveStiffMat(){
		if( m_IsSaveStiffMat || m_IsGeomNonlin || !m_IsStationary ) return;
		m_IsSaveStiffMat = true;
		this->m_is_cleared_value_ls = true;
		this->m_is_cleared_value_prec = true;
	}
	void UnSetSaveStiffMat(){
	if( !m_IsSaveStiffMat ) return;
		m_IsSaveStiffMat = false;
		this->m_is_cleared_value_ls = true;
		this->m_is_cleared_value_prec = true;
	}

	void SetStationary(){
		if( m_IsStationary ) return;
		m_IsStationary = true;
		this->m_is_cleared_value_ls = true;
		this->m_is_cleared_value_prec = true;
	}
	void UnSetStationary(){
		if( !m_IsStationary ) return;
		m_IsSaveStiffMat = false;
		m_IsStationary = false;
		this->m_is_cleared_value_ls = true;
		this->m_is_cleared_value_prec = true;
		// TODO : �����x����O�ɐݒ肵�Ȃ��ƃ_�����ˁD
	}
private:
	// ���z���\�֐�
	double MakeLinearSystem(const Fem::Field::CFieldWorld& world, bool is_initial);
	bool InitializeLinearSystem(const Fem::Field::CFieldWorld& world);
private:
	bool m_IsGeomNonlin;
	bool m_IsSaveStiffMat;
	bool m_IsStationary;
	double m_lambda, m_myu, m_rho;
	double m_g_x, m_g_y, m_g_z;
	
	unsigned int m_IdFieldDisp;
	////////////////
	// ���E�����ɂ���
	std::vector< std::pair<unsigned int,int> > m_aIdFixField;

};


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

/*! 
@brief �Q�����ő̕������N���X
@ingroup FemEqnObj
*/
class CEqn_Solid2D
{
public:
	CEqn_Solid2D(unsigned int id_ea, unsigned int id_field_disp)
		: m_id_ea(id_ea), m_IdFieldDisp(id_field_disp)
	{
		m_lambda = 0.0; m_myu = 1.0; m_rho = 1.0; m_g_x = 0.0; m_g_y = 0.0;
		m_IsGeomNonlin = false;
		m_IdFieldTemperature = 0;
	}
	CEqn_Solid2D()
		: m_id_ea(0), m_IdFieldDisp(0)
	{
		m_lambda = 0.0; m_myu = 1.0; m_rho = 1.0; m_g_x = 0.0; m_g_y = 0.0;
		m_IsGeomNonlin = false;
      m_IdFieldTemperature = 0;
  }
   CEqn_Solid2D( const CEqn_Solid2D& eqn ){
       this->m_id_ea = eqn.m_id_ea;
       this->m_IdFieldDisp = eqn.m_IdFieldDisp;
       this->m_IdFieldTemperature = eqn.m_IdFieldTemperature;
       this->CopyParameters(eqn);
   }

	unsigned int GetIdField_Disp() const { return m_IdFieldDisp; }
	void CopyParameters( const CEqn_Solid2D& eqn ){
		m_lambda = eqn.m_lambda; 
		m_myu    = eqn.m_myu;
		m_rho    = eqn.m_rho;
		m_g_x    = eqn.m_g_x;
		m_g_y    = eqn.m_g_y;
		m_IdFieldTemperature = eqn.m_IdFieldTemperature;
		m_IsGeomNonlin = eqn.m_IsGeomNonlin;
	}
	// Set���\�b�h
	void SetYoungPoisson( double young, double poisson, bool is_plane_stress ){
		m_lambda = young*poisson / ( (1.0+poisson)*(1-2.0*poisson) );
		m_myu = young / ( 2.0*(1.0+poisson) );
		if( !is_plane_stress ){
			m_lambda = 2*m_lambda*m_myu / (m_lambda+2*m_myu);
		}
	}
	//! �����萔�̎擾
	void GetLambdaMyu( double& lambda, double& myu) const{
		lambda = m_lambda;
		myu    = m_myu;
	}
	//! ���ʖ��x�̎擾
	double GetRho() const{ return m_rho; }
	void GetYoungPoisson( double& young, double& poisson) const{
//		if( is_plane_stress ){
			poisson = m_lambda*0.5/(m_lambda+m_myu);
			young   = 2*m_myu*(1+poisson);
/*		}
		else{

		}*/
	}
	//! ���ʖ��x�̐ݒ�
	void SetRho(  double rho){ m_rho = rho; }
	//! �̐ϗ͂̐ݒ�
	void SetGravitation( double g_x, double g_y ){
		m_g_x = g_x;
		m_g_y = g_y;
	}
	void GetGravitation( double& g_x, double& g_y ){
		g_x = m_g_x;
		g_y = m_g_y;
	}
	void SetIdFieldDisp(unsigned int id_field_disp){ m_IdFieldDisp = id_field_disp; }
	void SetGeometricalNonlinear(bool is_nonlin){   m_IsGeomNonlin = is_nonlin;  }
	void SetThermalStress(unsigned int id_field_temp){ m_IdFieldTemperature = id_field_temp; }
	// Get���\�b�h
	unsigned int GetIdEA() const { return m_id_ea; }
	bool IsTemperature()  const { return m_IdFieldTemperature!=0;  }
	bool IsGeometricalNonlinear() const { return m_IsGeomNonlin; }

	// �A���ꎟ�������}�[�W���\�b�h
	bool AddLinSys_NewmarkBetaAPrime( double dt, double gamma, double beta, bool is_inital, Ls::CLinearSystem_Field& ls, const Field::CFieldWorld& world );
	bool AddLinSys_NewmarkBetaAPrime_Save( Ls::CLinearSystem_SaveDiaM_NewmarkBeta& ls, const Field::CFieldWorld& world );
	bool AddLinSys( Ls::CLinearSystem_Field& ls, const Field::CFieldWorld& world );
	bool AddLinSys_Save( Ls::CLinearSystem_Save& ls, const Field::CFieldWorld& world );
private:
	unsigned int m_id_ea;
	unsigned int m_IdFieldDisp;
	unsigned int m_IdFieldTemperature;

	bool m_IsGeomNonlin;
	double m_lambda, m_myu, m_rho;
	double m_g_x, m_g_y;
};


//2D��3D�͕�����D�O�͂̎������Ⴄ���A���ʉ��͕��ʘc�̐ݒ肪���邩��D


/*!
@brief �Q�����ő́C�A���������N���X
@ingroup FemEqnSystem
*/
class CEqnSystem_Solid2D : public CEqnSystem
{
public:
	//! �f�t�H���g�R���X�g���N�^
	CEqnSystem_Solid2D();
	CEqnSystem_Solid2D(Fem::Field::CFieldWorld& world);
	CEqnSystem_Solid2D(const unsigned int id_field_val, Fem::Field::CFieldWorld& world);
			
	// ���z���\�֐�
	virtual bool UpdateDomain_Field(unsigned int id_field, Fem::Field::CFieldWorld& world);
	virtual bool SetDomain_FieldEA(unsigned int id_field, unsigned int id_ea, Fem::Field::CFieldWorld& world);
	//! ������������
	virtual bool Solve(Fem::Field::CFieldWorld& world);
	virtual void Clear(){
		CEqnSystem::Clear();
		m_IdFieldDisp = 0;
		m_aIdFixField.clear();
		m_IsSaveStiffMat = false;
		m_IsStationary = true;
		m_aEqn.clear();
		m_young_back = 1.0;
		m_poisson_back = 0;
		m_is_plane_stress_back = true;
		m_rho_back = 1;
		m_is_geom_nonlin_back = false;
		m_aLoad.clear();
		m_num_iter = 100;
		m_conv_ratio = 1.0e-6;
	}

	// �Œ苫�E������ǉ�&�폜����
	virtual bool         AddFixField(   unsigned int id_field,                  Fem::Field::CFieldWorld& world, int idof = -1);
	virtual unsigned int AddFixElemAry( unsigned int id_ea,                     Fem::Field::CFieldWorld& world, int idof = -1);
	virtual unsigned int AddFixElemAry( const std::vector<unsigned int>& aIdEA, Fem::Field::CFieldWorld& world, int idof = -1);
	virtual bool ClearFixElemAry( unsigned int id_ea, Fem::Field::CFieldWorld& world );
	virtual void ClearFixElemAry();

	unsigned int GetIdField_Disp() const { return m_IdFieldDisp; }

	bool ToplogicalChangeCad_InsertLoop(Fem::Field::CFieldWorld& world, 
		unsigned int id_l_back, unsigned id_l_ins);

	////////////////////////////////
	// ��virtual
	
	//! �������̎擾�C�ݒ�
	//! @{
	bool SetEquation( const CEqn_Solid2D& eqn );
   CEqn_Solid2D GetEquation(unsigned int id_ea) const;
	//! @}

	////////////////
	// �e�̈�̕������̃p�����[�^����x�ɕς���I�v�V����

	//! @{
	void SetYoungPoisson( double young, double poisson, bool is_plane_stress ); //!< �����p�����[�^�ݒ�
	void SetRho( double rho ); //!< ���x�ݒ�
	void SetGravitation( double g_x, double g_y ); //!< �d�͉����x�ݒ�
	void SetThermalStress(unsigned int id_field_temp);	//!< �M���͂��l������O��������Ƃŉ��������
	void SetGeometricalNonlinear(bool is_nonlin);	//!< �􉽊w�I����`�����l������
	void SetStationary(bool is_stationary);	//!< �ÓI���ɐݒ�
	void SetSaveStiffMat(bool is_save);		//!< �����s���ۑ�����
	
	bool IsStationary() const { return m_IsStationary; }
	bool IsSaveStiffMat() const { return m_IsSaveStiffMat; }
	void GetYoungPoisson(double& young, double& poisson, bool& is_plane_str) const{
		young = m_young_back;
		poisson = m_poisson_back;
		is_plane_str = m_is_plane_stress_back;
	}
	bool IsGeometricalNonlinear() const{ return m_is_geom_nonlin_back; }
	double GetRho(){ return m_rho_back; }

	//! @}

	//! �׏d�̐ݒ�
	void SetLoad(double load, unsigned int id_ea, Fem::Field::CFieldWorld& world);
	void ClearLoad(unsigned int id_ea);
	void ClearLoad(){ m_aLoad.clear(); }

	//! �ψʂ��牞�͂̒l���v�Z���ď�(ID:id_field)�ɃZ�b�g����
	bool SetEquivStressValue(unsigned int id_field, Fem::Field::CFieldWorld& world);
	bool SetStressValue(unsigned int id_field, Fem::Field::CFieldWorld& world);

private:
	// ���z���\�֐�
	bool EqnationProperty(bool& is_nonlin);

	double MakeLinearSystem(const Fem::Field::CFieldWorld& world, bool is_inital);
	bool InitializeLinearSystem(const Fem::Field::CFieldWorld& world);
	bool InitializePreconditioner();
	bool MakePreconditioner();
private:
	////////////////
	// ���E�����ɂ���
	unsigned int m_IdFieldDisp;
	std::vector< std::pair<unsigned int,int> > m_aIdFixField;

	bool m_IsSaveStiffMat;
	bool m_IsStationary;

	std::vector<CEqn_Solid2D> m_aEqn;
    double m_young_back;  // �V�����������v�f���o�����Ƃ��͂��̒l�ŏ�����
    double m_poisson_back;  // �V�����������v�f���o�����Ƃ��͂��̒l�ŏ�����
    bool m_is_plane_stress_back;  // �V�����������v�f���o�����Ƃ��͂��̒l�ŏ�����
    double m_rho_back;  // �V�����������v�f���o�����Ƃ��͂��̒l�ŏ�����
	bool m_is_geom_nonlin_back;

	std::vector< std::pair<unsigned int,double> > m_aLoad;

	unsigned int m_num_iter;
	double m_conv_ratio;
};

}
}

#endif

