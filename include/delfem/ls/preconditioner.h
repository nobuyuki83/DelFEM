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
@brief �O�����s��N���X
@author Nobuyuki Umetani
*/

#if !defined(PRECONDITIONER_H)
#define PRECONDITIONER_H

#include <assert.h>
#include <time.h>

#include "delfem/ls/linearsystem.h"
#include "delfem/ls/linearsystem_interface_solver.h"

#include "delfem/matvec/matdia_blkcrs.h"
#include "delfem/matvec/matdiafrac_blkcrs.h"
#include "delfem/matvec/matfrac_blkcrs.h"
#include "delfem/matvec/vector_blk.h"
#include "delfem/matvec/solver_mg.h"
#include "delfem/matvec/ordering_blk.h"

namespace LsSol{

/*! 
@brief �O�����s��N���X�̒��ۃN���X
@ingroup LsSol
*/
class CPreconditioner
{
public:
	virtual ~CPreconditioner(){};
	virtual void SetLinearSystem(const CLinearSystem& ls) = 0;
	virtual bool SetValue(const CLinearSystem& ls) = 0;
	virtual bool SolvePrecond(CLinearSystem& ls, unsigned int iv) = 0;
};

/*! 
@brief ILU�ɂ��O�����s��N���X
@ingroup LsSol
*/
class CPreconditioner_ILU : public CPreconditioner
{
public:
	CPreconditioner_ILU(){
		m_is_ordering = false;
	}
	CPreconditioner_ILU(const CLinearSystem& ls, unsigned int nlev = 0){ 
    m_is_ordering = false;
    this->SetFillInLevel(nlev);
		this->SetLinearSystem(ls); 
	}
	virtual ~CPreconditioner_ILU(){
		this->Clear();
	}
	//! �f�[�^��S�ď�������
	void Clear();

	//! fill_in�̃��x���ݒ�
	void SetFillInLevel(int lev, int ilss0 = -1){ m_alev_input.push_back( std::make_pair(lev,ilss0) ); }

  // ���̃m�[�h�ɂ͕K��Fill_In������
  void SetFillBlk(const std::vector<unsigned int>& aBlk){ m_afill_blk = aBlk; }

	// ILU(0)�̃p�^�[��������
	virtual void SetLinearSystem(const CLinearSystem& ls);

	// �l��ݒ肵��ILU�������s���֐�
	// ILU�������������Ă��邩�ǂ����͂����Əڍׂȃf�[�^��Ԃ�����
	virtual bool SetValue(const CLinearSystem& ls);

	// Solve Preconditioning System
	virtual bool SolvePrecond(CLinearSystem& ls, unsigned int iv);
  
	//! Ordering�̗L����ݒ�
	void SetOrdering(const std::vector<int>& aind){ 
    if( aind.empty() ){ m_is_ordering = false; return; }
    m_is_ordering = true;
    m_order.SetOrdering(aind);
  }
private:
  std::vector< std::pair<int,int> > m_alev_input;
  std::vector< unsigned int > m_afill_blk;
  std::vector< std::vector< MatVec::CMatFrac_BlkCrs* > > m_Matrix_NonDia;
  std::vector< MatVec::CMatDiaFrac_BlkCrs* > m_Matrix_Dia;
  
  // Ordering 
	bool m_is_ordering;  
  MatVec::COrdering_Blk m_order;
  MatVec::CVector_Blk m_vec;  // �I�[�_�����O�̎��Ɏg��TMP�s��
};


/*! 
@brief �A���ꎟ�������ƑO�����N���X�̒��ۃN���X
@ingroup LsSol
*/
class CLinearSystemPreconditioner : public LsSol::ILinearSystemPreconditioner_Sol
{
public:
    CLinearSystemPreconditioner( CLinearSystem& ls, CPreconditioner& prec )
        : ls(ls), prec(prec){}

	////////////////////////////////
	// function for linear solver
	// v=-1:residual    v=-2:update

	//! �\���o�ɕK�v�ȍ�ƃx�N�g���̐��𓾂�
    virtual unsigned int GetTmpVectorArySize() const{ return ls.GetTmpVectorArySize(); }
	//! �\���o�ɕK�v�ȍ�ƃx�N�g���̐���ݒ�
    virtual bool ReSizeTmpVecSolver(unsigned int size_new){ return ls.ReSizeTmpVecSolver(size_new); }

    //! �x�N�g���̓��� (return {v1} * {v2})
    virtual double DOT(int iv1, int iv2){ return ls.DOT(iv1,iv2); } 
    //! �x�N�g���̃R�s�[ ({v2} := {v1})
    virtual bool COPY(int iv1, int iv2){ return ls.COPY(iv1,iv2); }
    //! �x�N�g���̃X�J���[�{ ({v1} := alpha * {v1})
    virtual bool SCAL(double alpha, int iv1){ return ls.SCAL(alpha,iv1); }
    //! �x�N�g���̑����Z({v2} := alpha*{v1} +�@{v2})	
    virtual bool AXPY(double alpha, int iv1, int iv2){ return ls.AXPY(alpha,iv1,iv2); }
    //! �s��x�N�g���� ({v2} := alpha*[MATRIX]*{v1} + beta*{v2})
    virtual bool MATVEC(double alpha, int iv1, double beta, int iv2){ return ls.MATVEC(alpha, iv1, beta, iv2); }

    virtual bool SolvePrecond(int iv){ return prec.SolvePrecond(ls,iv); }
private:
    CLinearSystem& ls;
    CPreconditioner& prec;
};

}	// end namespace LsSol

#endif
