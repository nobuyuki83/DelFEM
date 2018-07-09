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
@brief Interface of class (MatVec::CMat_BlkCrs)
@author Nobuyuki Umetani
*/

#if !defined(MAT_CRS_H)
#define MAT_CRS_H

#include <assert.h>
#include <vector>

namespace Com{
    class CIndexedArray;
}
	
namespace MatVec{

class CVector_Blk;
class COrdering_Blk;
class CBCFlag;

/*! 
@brief �u���b�NCRS�\���̍s��N���X
@ingroup MatVec
*/
class CMat_BlkCrs  
{
public:
	/*!
	@brief �T�C�Y�������Ƃ����R���X�g���N�^
	@param[in] nblk_col ��̃u���b�N��
	@param[in] len_col  �u���b�N�̗�̒���
	@param[in] nblk_row �s�̃u���b�N��
	@param[in] len_row  �u���b�N�̍s�̒���
	*/
	CMat_BlkCrs(unsigned int nblk_col, unsigned int len_col, 
		        unsigned int nblk_row, unsigned int len_row );
    
  CMat_BlkCrs(unsigned int nblk_col, const std::vector<unsigned int>& alen_col, 
              unsigned int nblk_row, const std::vector<unsigned int>& alen_row );

	/*!
	@brief �s����R�s�[���邽�߂̃R���X�g���N�^
	@param[in] rhs �����s��
	@param[in] is_value �l�܂ŃR�s�[���邩�ǂ���
	@param[in] isnt_trans �]�u�Ƃ��ăR�s�[���Ȃ���
	*/
	CMat_BlkCrs(const CMat_BlkCrs& rhs, bool is_value, bool isnt_trans);

	CMat_BlkCrs(); 
	virtual ~CMat_BlkCrs();

	virtual bool Initialize(unsigned int nblk_col, unsigned int len_col, 
		                    unsigned int nblk_row, unsigned int len_row );
  // Dia�ŉB�����邽�߂�virtual�ɂ��Ă���
  virtual bool Initialize(unsigned int nblk_col, const std::vector<unsigned int>& alen_col, 
                          unsigned int nblk_row, const std::vector<unsigned int>& alen_row );

	inline unsigned int NBlkMatCol() const { return m_nblk_MatCol; }	//!< �s�Ɋ�u���b�N�����邩
	inline unsigned int NBlkMatRow() const { return m_nblk_MatRow; }	//!< ��Ɋ�u���b�N�����邩
	
	//! �u���b�N�̍s�̃T�C�Y(�c�̒���)���擾.������Fix����Ė����ꍇ��-1��Ԃ�
	inline int LenBlkCol() const { return m_len_BlkCol; }
	//! �u���b�N�̍s�̃T�C�Y(�c�̒���)���擾.
	inline unsigned int LenBlkCol(unsigned int iblk) const { 
    assert( iblk < m_nblk_MatCol );
    if( m_len_BlkCol == -1 ){
      assert( m_DofPtrCol != 0 );
      return m_DofPtrCol[iblk+1]-m_DofPtrCol[iblk];
    }
    return m_len_BlkCol; 
  }
	//! �u���b�N�̗�̃T�C�Y(���̒���)���擾.������Fix����Ė����ꍇ��-1��Ԃ�
	inline int LenBlkRow() const { return m_len_BlkRow; }
	//! �u���b�N�̍s�̃T�C�Y(���̒���)���擾.
	inline unsigned int LenBlkRow(unsigned int iblk) const { 
    assert( iblk < m_nblk_MatRow );
    if( m_len_BlkRow == -1 ){
      assert( m_DofPtrRow != 0 );
      return m_DofPtrRow[iblk+1]-m_DofPtrRow[iblk];
    }
    return m_len_BlkRow; 
  }

	//! �l�ɂO��ݒ�D�f�[�^�̈悪�m�ۂ���Ă��Ȃ���΁C�m�ۂ���
	virtual bool SetZero();	
	//! �p�^�[����S�ď����@RowPtr,Val�̓��������
	virtual bool DeletePattern();	

	virtual void FillPattern();
  virtual bool AddPattern(const Com::CIndexedArray& crs);	//!< ��[���p�^�[����ǉ�����
	virtual bool AddPattern(const CMat_BlkCrs& rhs, const bool isnt_trans);	//!< ��[���p�^�[����ǉ�����
	virtual bool AddPattern(const CMat_BlkCrs& rhs, 
                          const COrdering_Blk& order_col, const COrdering_Blk& order_row);	//!< ��[���p�^�[����ǉ�����

	// �����͔h���N���X�ɔC�������
	virtual bool SetPatternBoundary(const CMat_BlkCrs& rhs, const CBCFlag& bc_flag_col, const CBCFlag& bc_flag_row);
	virtual bool SetPatternDia(const CMat_BlkCrs& rhs);
	
	virtual bool SetValue(const CMat_BlkCrs& rhs, const bool isnt_trans);
	virtual bool SetValue(const CMat_BlkCrs& rhs, 
                        const COrdering_Blk& order_col, const COrdering_Blk& order_row);
	
	//! �v�f�����s����}�[�W����
	virtual bool Mearge
  (unsigned int nblkel_col, const unsigned int* blkel_col,
   unsigned int nblkel_row, const unsigned int* blkel_row,
   unsigned int blksize, const double* emat);
  //! ��Ɨp�z��̗̈���������
  void DeleteMargeTmpBuffer(){
    if( m_marge_tmp_buffer == 0 ) return;
    delete[]  m_marge_tmp_buffer;
    m_marge_tmp_buffer = 0;
  }

	//! �s��x�N�g����
	virtual bool MatVec(double alpha, const CVector_Blk& x, double beta, CVector_Blk& b, const bool isnt_trans) const;

	//! bc_flag���P�̍s�̗v�f���O�ɂ���
	bool SetBoundaryCondition_Row(const CBCFlag& bc_flag);
	//! bc_flag���P�̗�̗v�f���O�ɂ���
	bool SetBoundaryCondition_Colum(const CBCFlag& bc_flag);
	//! bc_flag���O�̗v�f���O�ɂ���i�S�̍����s��ۑ��ɂ�鍂�����@�j
	bool SetBoundaryConditionInverse_Colum(const CBCFlag& bc_flag);
	
	////////////////////////////////////////////////
	// Crs Original Function ( Non-virtual )

	//! CRS�̃T�C�Y�擾
	const unsigned int NCrs() const { return m_ncrs_Blk; }
	const unsigned int* GetPtrIndPSuP(const unsigned int ipoin, unsigned int& npsup) const
	{
		if( m_rowPtr_Blk == 0 ){ npsup = 0; return 0; }
		assert( ipoin < m_nblk_MatCol );
		npsup = m_colInd_Blk[ipoin+1]-m_colInd_Blk[ipoin];
		const unsigned int icrs = m_colInd_Blk[ipoin];
		return &m_rowPtr_Blk[icrs];
	}
	const double* GetPtrValPSuP(const unsigned int ipoin, unsigned int& npsup) const
	{
		if( m_rowPtr_Blk == 0 ){ npsup = 0; return 0; }
		assert( ipoin < m_nblk_MatCol );
		npsup = m_colInd_Blk[ipoin+1]-m_colInd_Blk[ipoin];
		const unsigned int icrs = m_colInd_Blk[ipoin];
		return &m_valCrs_Blk[icrs*m_len_BlkCol*m_len_BlkRow];
	}
	double* GetPtrValPSuP(const unsigned int ipoin, unsigned int& npsup)
	{
		if( m_rowPtr_Blk == 0 ){ npsup = 0; return 0; }
		assert( ipoin < m_nblk_MatCol );
		npsup = m_colInd_Blk[ipoin+1]-m_colInd_Blk[ipoin];
		const unsigned int icrs = m_colInd_Blk[ipoin];
		return &m_valCrs_Blk[icrs*m_len_BlkCol*m_len_BlkRow];
	}

private:
	unsigned int m_nblk_MatCol;	//!< �s�Ɋ�u���b�N�����邩
	unsigned int m_nblk_MatRow;	//!< ��Ɋ�u���b�N�����邩

	int m_len_BlkCol;	//!< �u���b�N�̗�T�C�Y
	int m_len_BlkRow;	//!< �u���b�N�̍s�T�C�Y

protected:
	unsigned int  m_ncrs_Blk;	//!< CRS�̃T�C�Y
	unsigned int* m_colInd_Blk;	//!< CRS��Colum Index
	unsigned int* m_rowPtr_Blk;	//!< CRS��Row Pointer

  int* m_marge_tmp_buffer;	//!< �}�[�W�̎��ɕK�v�ȍ�ƃo�b�t�@

  // Flex�̎��ɒ�`�����l
  unsigned int* m_DofPtrCol;
  unsigned int* m_DofPtrRow;
  unsigned int* m_ValPtr;

	double* m_valCrs_Blk;	//!< �s��̒l
};

}	// end namespace 'Ls'

#endif 
