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
@brief �����s��N���X(MatVec::CMatDia_BlkCrs)�̃C���^�[�t�F�C�X
*/

#if !defined(MATDIA_CRS_H)
#define MATDIA_CRS_H

#include "delfem/matvec/mat_blkcrs.h"	// ���̃N���X���p�����Ă���

namespace Com{
    class CIndexedArray;
}

namespace MatVec{

class CVector_Blk;
class CBCFlag;
class COrdering_Blk;

/*!
@brief square crs matrix class
@ingroup MatVec

�f�[�^�\��:�Ίp���������u���b�NCRS�N���X
*/
class CMatDia_BlkCrs  : public CMat_BlkCrs
{
public:
	/*!
	@brief �T�C�Y�������Ƃ���R���X�g���N�^
	@param[in] nblk_colrow �u���b�N��
	@param[in] len_colrow �u���b�N�̃T�C�Y
	*/
	CMatDia_BlkCrs(const unsigned int nblk_colrow, const unsigned int len_colrow);
	//! �f�t�H���g�R���X�g���N�^
	CMatDia_BlkCrs();
	//! �f�X�g���N�^
	virtual ~CMatDia_BlkCrs();

    
    // Mat_BlkCrs�N���X�̉B��
    virtual bool Initialize(unsigned int nblk_col, const std::vector<unsigned int>& alen_col, 
                            unsigned int nblk_row, const std::vector<unsigned int>& alen_row);
    bool Initialize(unsigned int nblk, const std::vector<unsigned int>& alen){
        return this->Initialize(nblk,alen, nblk,alen);
    }
    // Mat_BlkCrs�N���X�̉B��
    virtual bool Initialize(unsigned int nblk_col, unsigned int len_col,
                            unsigned int nblk_row, unsigned int len_row );
    bool Initialize(unsigned int nblk, unsigned int len){
        return this->Initialize(nblk,len, nblk,len);
    }

	//! �p�^�[����S�ď����@RowPtr,Val�̓��������
	virtual bool DeletePattern();
    bool AddPattern(const Com::CIndexedArray& crs);
	bool AddPattern(const CMatDia_BlkCrs& rhs, const bool isnt_trans);
	bool AddPattern(const CMatDia_BlkCrs& rhs, const COrdering_Blk& order);
	bool AddPattern(const CMat_BlkCrs& m1, const CMatDia_BlkCrs& m2, const CMat_BlkCrs& m3);

	virtual bool SetValue(const CMatDia_BlkCrs& rhs, const bool isnt_trans);
	virtual bool SetValue(const CMat_BlkCrs& m1, const CMatDia_BlkCrs& m2, const CMat_BlkCrs& m3);	// := m1*m2*m3
	virtual bool SetValue(const CMatDia_BlkCrs& rhs, const COrdering_Blk& order);

	//! �s��ɂO���Z�b�g���邽�߂̊֐�(�e�N���X�̉B��)
	virtual bool SetZero();
	//! �}�[�W���邽�߂̊֐�(�e�N���X�̉B��)
	virtual bool Mearge(
		unsigned int nblkel_col, const unsigned int* blkel_col,
		unsigned int nblkel_row, const unsigned int* blkel_row,
		unsigned int blksize, const double* emat);
	/*!
	@brief �s��x�N�g����(�e�N���X�̉B��)
	{lhs} = beta*{lhs} + alpha*[A]{rhs}
	*/
	virtual bool MatVec(double alpha, const CVector_Blk& rhs, double beta, CVector_Blk& lhs) const;

	//! bc_flag���P�̎��R�x�̍s�Ɨ���O�ɐݒ�C�A���Ίp�����͂P��ݒ�
	bool SetBoundaryCondition(const CBCFlag& bc_flag);

    ////////////////////////////////////////////////////////////////
    // �O�����p�̃N���X

	const double* GetPtrValDia(const unsigned int ipoin) const {
        assert( m_DiaValPtr == 0 );
		unsigned int blksize = this->LenBlkCol()*this->LenBlkRow();
		return &m_valDia_Blk[ipoin*blksize];
	}
	double* GetPtrValDia(const unsigned int ipoin){
        assert( m_DiaValPtr == 0 );
		unsigned int blksize = this->LenBlkCol()*this->LenBlkRow();
		return &m_valDia_Blk[ipoin*blksize];
	}

protected:
	double* m_valDia_Blk;	// �Ίp�u���b�N����
    
  // Flex���ɒ�`�����l
  unsigned int* m_DiaValPtr;
};

}	// end namespace 'Ls'

#endif // MATDIA_CRS_H
