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
@brief �X�J���[�^�̕������N���X(Fem::Eqn::CEqnSystem_Scalar2D, Fem::Eqn::CEqn_Scalar2D, Fem::Eqn::CEqn_Scalar3D)�̃C���^�[�t�F�[�X
@author Nobyuki Umetani

@li �|�A�\��������
@li �g�U������
@li �ÓI�ڗ��g�U������
@li ���I�ڗ��g�U������
*/



#if !defined(EQN_OBJ_SCALAR_H)
#define EQN_OBJ_SCALAR_H

#if defined(__VISUALC__)
#pragma warning( disable : 4786 )
#endif

#include <vector>
#include "delfem/eqnsys.h"

namespace Fem{

namespace Ls{
	class CLinearSystem_Field;
	class CLinearSystem_SaveDiaM_Newmark;
	class CPreconditioner;
}

namespace Field{
	class CField;
	class CFieldWorld;
}
namespace Eqn{

/*! 
@brief 2D�̃X�J���[�^�������N���X
@ingroup FemEqnObj
*/
class CEqn_Scalar2D
{
public:
	CEqn_Scalar2D(unsigned int id_ea, unsigned int id_field_val)
		: m_id_ea(id_ea), m_IdFieldVal(id_field_val)
	{
		m_alpha = 1.0; 
		m_capa = 1.0;
		m_source = 0.0;
		m_IdFieldAdvec = 0;
	}
	// Set���\�b�h
	//! @{
	void SetAlpha(   double alpha ){ m_alpha  = alpha;  }
	void SetCapacity(double capa  ){ m_capa   = capa;   }
	void SetSource(  double source){ m_source = source; }
	void SetAdvection(unsigned int id_field_advec){ m_IdFieldAdvec = id_field_advec; }
	//! @}
        double GetAlpha() const { return m_alpha; }
        double GetCapacity() const { return m_capa; }
        double GetSource() const { return m_source; }
	// Get���\�b�h
	unsigned int GetIdEA() const { return m_id_ea; }
	bool IsAdvection()  const { return m_IdFieldAdvec != 0; }

	// �A���ꎟ�������}�[�W���\�b�h
	bool AddLinSys( Ls::CLinearSystem_Field& ls, const Field::CFieldWorld& world );
	bool AddLinSys_Newmark( double dt, double gamma, Ls::CLinearSystem_Field& ls, 
		bool is_ax_sym,
		const Field::CFieldWorld& world );
	bool AddLinSys_Save( Ls::CLinearSystem_Save& ls, const Field::CFieldWorld& world );
	bool AddLinSys_SaveKDiaC( Ls::CLinearSystem_SaveDiaM_Newmark& ls, const Field::CFieldWorld& world );
private:
	unsigned int m_id_ea;
	unsigned int m_IdFieldVal;
	unsigned int m_IdFieldAdvec;

	double m_alpha;
	double m_capa;
	double m_source;
};
	
////////////////////////////////////////////////////////////////

/*! 
@ingroup FemEqnSystem
@brief 2D�̃X�J���[�^,�A���������N���X

�������Ƃ��ł��������
@li �|�A�\��������
@li �g�U������
@li �ÓI�ڗ��g�U������
@li ���I�ڗ��g�U������
*/
class CEqnSystem_Scalar2D : public CEqnSystem
{
public:
	//! �f�t�H���g�E�R���X�g���N�^�̐ݒ�
	CEqnSystem_Scalar2D();
	CEqnSystem_Scalar2D(const unsigned int id_field_val, Fem::Field::CFieldWorld& world);
	// vritual �֐�
	virtual bool SetDomain_Field(unsigned int id_field, Fem::Field::CFieldWorld& world);
	virtual bool SetDomain_FieldElemAry(unsigned int id_field, unsigned int id_ea, Fem::Field::CFieldWorld& world);

	//! ������������
	virtual bool Solve(Fem::Field::CFieldWorld& world);
	
	////////////////////////////////
	// �Œ苫�E������ǉ�&�폜����
	virtual bool         AddFixField(   unsigned int id_field,                  Fem::Field::CFieldWorld& world, int idof = -1);
	virtual unsigned int AddFixElemAry( unsigned int id_ea,                     Fem::Field::CFieldWorld& world, int idof = -1);
	virtual unsigned int AddFixElemAry( const std::vector<unsigned int>& aIdEA, Fem::Field::CFieldWorld& world, int idof = -1);
	virtual bool ClearFixElemAry( unsigned int id_ea, Fem::Field::CFieldWorld& world );
	virtual void ClearFixElemAry();

	virtual unsigned int GetIdField_Value() const { return m_IdFieldVal; }
	
	//! ��������ݒ肷��
	bool SetEquation( const CEqn_Scalar2D& eqn );
	//! ���������擾����
   CEqn_Scalar2D GetEquation(unsigned int id_ea) const;

	//! �g�U�W���̐ݒ�
	void SetAlpha(double alpha);
	//! �e�ʂ̐ݒ�
	void SetCapacity(double capa);
	//! �ʐςɔ�Ⴗ��\�[�X���̐ݒ�
	void SetSource(double source);
	/*! 
	@brief �ÓI���
	@param [in] is_stat �ÓI��肩�ǂ���
	*/
	void SetStationary(bool is_stat);	
	/*! 
	@brief �ڗ����̐ݒ�
	@param [in] id_field_advec �l���ڗ������鑬�x���ID
	@remark ������id_field_advec���O�܂��͑��݂��Ȃ�ID�̏ꍇ�͈ڗ����͖��������
	*/
	void SetAdvection(unsigned int id_field_advec);
	void SetAxialSymmetry(bool is_ax_sym);
	
	void SetSaveStiffMat(bool is_save){
		m_IsSaveStiffMat = is_save;
		this->ClearLinearSystemPreconditioner();
	}

private:
	virtual double MakeLinearSystem(const Fem::Field::CFieldWorld& world);
	virtual bool InitializeLinearSystem(const Fem::Field::CFieldWorld& world);
	virtual bool MatrixProperty(bool& is_c, bool& is_m, bool& is_asym );
private:
	unsigned int m_IdFieldVal;
	////////////////
	// ���E�����ɂ���
	std::vector< std::pair<unsigned int,int> > m_aIdFixField;

	std::vector<CEqn_Scalar2D> m_aEqn;
	bool m_IsAxialSymmetry;
	bool m_IsStationary;
	bool m_IsSaveStiffMat;
};

////////////////

/*! 
@brief 3D�̃X�J���[�^�������N���X
@ingroup FemEqnObj
*/
class CEqn_Scalar3D : public CEqnSystem
{
public:
	//! �f�t�H���g�E�R���X�g���N�^
	CEqn_Scalar3D();
	CEqn_Scalar3D(const unsigned int id_field_val, Fem::Field::CFieldWorld& world);
	
	virtual bool SetDomain(unsigned int id_base, Fem::Field::CFieldWorld& world);
	//! ����
	virtual bool Solve(Fem::Field::CFieldWorld& world);

	////////////////////////////////
	// �Œ苫�E������ǉ�&�폜����
	virtual bool         AddFixField(   unsigned int id_field,                  Fem::Field::CFieldWorld& world, int idof = -1);
	virtual unsigned int AddFixElemAry( unsigned int id_ea,                     Fem::Field::CFieldWorld& world, int idof = -1);
	virtual unsigned int AddFixElemAry( const std::vector<unsigned int>& aIdEA, Fem::Field::CFieldWorld& world, int idof = -1);
	virtual bool ClearFixElemAry( unsigned int id_ea, Fem::Field::CFieldWorld& world );
	virtual void ClearFixElemAry();

	virtual unsigned int GetIdField_Value() const { return m_IdFieldVal; }

	//! �g�U�W����ݒ肷��
	void SetAlpha(double alpha){ 
		m_alpha = alpha; 
		this->m_is_cleared_value_ls   = true;
		this->m_is_cleared_value_prec = true;
	}
	//! �̐ςɔ�Ⴗ��\�[�X����ݒ肷��
	void SetSource(double source){ 
		m_source = source; 
		this->m_is_cleared_value_ls   = true;
		this->m_is_cleared_value_prec = true;
	}
private:
	virtual double MakeLinearSystem(const Fem::Field::CFieldWorld& world);
	virtual bool InitializeLinearSystem(const Fem::Field::CFieldWorld& world);
private:
	unsigned int m_IdFieldVal;
	////////////////
	// ���E�����ɂ���
	std::vector< std::pair<unsigned int,int> > m_aIdFixField;

	double m_alpha;
	double m_source;
};

}
}

#endif
