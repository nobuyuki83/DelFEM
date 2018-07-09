/*
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
@brief �ۑ����A���ꎟ�������N���X�̃C���^�[�t�F�[�X
@author Nobuyuki Umetani
*/

#if !defined(LINEAR_SYSTEM_FIELD_SAVE_H)
#define LINEAR_SYSTEM_FIELD_SAVE_H

#if defined(__VISUALC__)
#pragma warning( disable : 4786 )
#endif

#include <assert.h>

#include "delfem/field.h"
#include "delfem/ls/linearsystem_interface_solver.h"
#include "delfem/ls/linearsystem.h"
#include "delfem/femls/linearsystem_field.h"

namespace MatVec{
class CVector_Blk;
class CZVector_Blk;
class CMat_BlkCrs;
class CMatDia_BlkCrs;
class CDiaMat_Blk;
class CBCFlag;
}

namespace Fem{
namespace Field{
	class CFieldWorld;
}
namespace Ls{

////////////////////////////////////////////////////////////////

/*! 
@brief �Œ苫�E���������Ă��Ȃ��s���ۑ����邱�Ƃɂ���āC�c�����v�Z�ł���悤�ɂ����N���X
@ingroup FemLs
*/
class CLinearSystem_Save : public CLinearSystem_Field
{
public:
    CLinearSystem_Save(){}
	virtual ~CLinearSystem_Save();
	//! field�ŏ���������Afield�̒��̔�[���p�^�[�������
	virtual bool AddPattern_Field(const unsigned int id_field, const Field::CFieldWorld& world);
	//! field�ŏ���������Afield��field-field2�̒��̔�[���p�^�[�������
	virtual bool AddPattern_Field(unsigned int id_field, unsigned int id_field2, const Field::CFieldWorld& world);
	//! field��field2���p�^�[�����������Ƃ��āC�u���b�N���������ꂽ��̍s������
	virtual bool AddPattern_CombinedField(unsigned id_field, unsigned int id_field2, const Field::CFieldWorld& world);

	//! �Ίp�s����Q�b�g����
    MatVec::CMat_BlkCrs& GetMatrix_Boundary(
		unsigned int id_field_col, Field::ELSEG_TYPE elseg_type_col,
		unsigned int id_field_row, Field::ELSEG_TYPE elseg_type_row,
		const Field::CFieldWorld& world);
	
    MatVec::CVector_Blk& GetForce(
		unsigned int id_field, Field::ELSEG_TYPE elseg_type,
		const Field::CFieldWorld& world);

	//! �}�[�W�O�̏������i���N���X�̉B���j
	virtual void InitializeMarge();
	//! �}�[�W��̏����i���N���X�̉B���j
	virtual double FinalizeMarge();
	//! �c�������
	virtual double MakeResidual(const Fem::Field::CFieldWorld& world);
	virtual bool UpdateValueOfField( unsigned int id_field, Field::CFieldWorld& world, Fem::Field::FIELD_DERIVATION_TYPE);

protected:
	void MakeBoundaryPattern();
private:
	void AddBoundaryMatrixForce(unsigned int id_field, const Fem::Field::CFieldWorld& world, unsigned int nlss0, unsigned int nlss1);
protected:
    std::vector< std::vector< MatVec::CMat_BlkCrs* > > m_Matrix_Boundary;
    std::vector< MatVec::CVector_Blk* > m_Force;
};

////////////////////////////////////////////////////////////////

/*!
TODO : ���ʍ��ɂ��ď��������̕K�v����
     : �f�X�g���N�^�ł̃�������@�����Ȃ���΂Ȃ�Ȃ�
*/
/*! 
@brief �Œ苫�E���������Ă��Ȃ��s���ۑ����邱�Ƃɂ���āC�c���������v�Z�ł���悤�ɂ���KC�V�X�e���̘A���ꎟ������
@ingroup FemLs
*/
class CLinearSystem_SaveDiaM_Newmark : public CLinearSystem_Save
{
public:	
	CLinearSystem_SaveDiaM_Newmark() : gamma(0.5), dt(0.0) {}
	void SetNewmarkParameter(double gamma, double dt){
		this->gamma = gamma;
		this->dt = dt;
	}
	double GetGamma(){ return gamma; }
	double GetDt(){ return dt; }

	//! �}�[�W�O�̏������i���N���X�̉B���j
	virtual bool AddPattern_Field(const unsigned int id_field, const Fem::Field::CFieldWorld& world);
	virtual void InitializeMarge();
	//! �}�[�W��̏����i���N���X�̉B���j
	virtual double FinalizeMarge();
	virtual double MakeResidual(const Fem::Field::CFieldWorld& world);
	virtual bool UpdateValueOfField(unsigned int id_field_val, Field::CFieldWorld& world, Fem::Field::FIELD_DERIVATION_TYPE fdt );
    MatVec::CDiaMat_Blk& GetDiaMassMatrix(unsigned int id_field, Field::ELSEG_TYPE elseg_type, const Field::CFieldWorld& world);
private:
    std::vector< MatVec::CDiaMat_Blk* > m_DiaMassMatrix;
	double gamma;
	double dt;
};


////////////////////////////////////////////////////////////////

/*!
TODO : ���ʍ��ɂ��ď��������̕K�v����
*/
/*! 
@brief �Œ苫�E���������Ă��Ȃ��s���ۑ����邱�Ƃɂ���āC�c�����v�Z�ł���悤�ɂ���KM�V�X�e���̘A���ꎟ�������N���X
@ingroup FemLs
*/
class CLinearSystem_SaveDiaM_NewmarkBeta : public CLinearSystem_Save
{
public:	
	CLinearSystem_SaveDiaM_NewmarkBeta() : beta(0.625), gamma(0.5), dt(0.0) {}
	virtual ~CLinearSystem_SaveDiaM_NewmarkBeta();
	void SetNewmarkParameter(double beta, double gamma, double dt){
		this->beta = beta;
		this->gamma = gamma;
		this->dt = dt;
	}

	double GetGamma(){ return gamma; }
	double GetDt(){ return dt; }
	double GetBeta(){ return beta; }

	//! �}�[�W�O�̏������i���N���X�̉B���j
    MatVec::CDiaMat_Blk& GetDiaMassMatrix(unsigned int id_field, Field::ELSEG_TYPE elseg_type, const Field::CFieldWorld& world);

	virtual bool AddPattern_Field(const unsigned int id_field, const Fem::Field::CFieldWorld& world);
	//! field��field2���p�^�[�����������Ƃ��āC�u���b�N���������ꂽ��̍s������
	virtual bool AddPattern_CombinedField(unsigned id_field, unsigned int id_field2, const Field::CFieldWorld& world);

	virtual void InitializeMarge();
	//! �}�[�W��̏����i���N���X�̉B���j
	virtual double FinalizeMarge();
	virtual double MakeResidual(const Fem::Field::CFieldWorld& world);
	virtual bool UpdateValueOfField(unsigned int id_field_val, Field::CFieldWorld& world, Fem::Field::FIELD_DERIVATION_TYPE fdt );
private:
	void AddBoundaryMatrixForce(unsigned int id_field, const Fem::Field::CFieldWorld& world, 
												unsigned int nlss0, unsigned int nlss1);
private:
    std::vector< MatVec::CDiaMat_Blk* > m_DiaMassMatrix;
	double beta;
	double gamma;
	double dt;
};


/*! 
@brief �ŗL�l�v�Z�p�̃N���X
@ingroup FemLs
*/
class CLinearSystem_Eigen : public CLinearSystem_Field
{
public:
	virtual void Clear();
	// field�ŏ���������Afield�̒��̔�[���p�^�[�������
	virtual bool AddPattern_Field(const unsigned int id_field, const Field::CFieldWorld& world);
	virtual void InitializeMarge();
    MatVec::CDiaMat_Blk& GetDiaMassMatrix(unsigned int id_field, Field::ELSEG_TYPE elseg_type, const Field::CFieldWorld& world);
	bool SetVector_fromField(int iv, unsigned int id_field_val, const Field::CFieldWorld& world, Fem::Field::FIELD_DERIVATION_TYPE fdt );
	bool DecompMultMassMatrix();
	bool MultUpdateInvMassDecomp();
	bool MultVecMassDecomp(int ivec);
	void OffsetDiagonal(double lambda);
//	void RemoveConstant(int iv);
private:
    std::vector< MatVec::CDiaMat_Blk* > m_DiaMassMatrix;
};

}	// end namespace Ls
}	// end namespace Fem

#endif
