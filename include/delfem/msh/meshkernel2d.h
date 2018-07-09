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
@brief �Q�������b�V����Utility�֐��Q
@author Nobuyuki Umetani
*/


#if !defined(MESH_KERNEL_2D_H)
#define MESH_KERNEL_2D_H

#if defined(__VISUALC__)
	#pragma warning( disable : 4786 )
#endif

#include <vector>

#include "delfem/vector2d.h"

namespace Msh
{

/*! 
@addtogroup Msh2D
*/
//! @{

////////////////////////////////////////////////
// ��������{���͋��L��������

const double MIN_TRI_AREA = 1.0e-10;

const unsigned int nNoEd  = 2;	//!< �����ɂ�����_�����邩

const unsigned int nNoTri = 3;	//!< �R�p�`�ɂ����璸�_�����邩
const unsigned int nEdTri = 3;	//!< �R�p�`�ɂ�����ӂ����邩
//! �R�p�`�̊e�ӂ̒��_�ԍ�
const unsigned int noelTriEdge[nEdTri][nNoEd] = {	
	{ 1, 2 },
	{ 2, 0 },
	{ 0, 1 },
};
//! �R�p�`�̗אڊ֌W
const unsigned int relTriTri[3][3] = {
	{ 0, 2, 1 }, //  0
	{ 2, 1, 0 }, //  1 
	{ 1, 0, 2 }, //  2
};

const unsigned int nNoQuad = 4;	//!< �S�p�`�ɂ����璸�_�����邩
const unsigned int nEdQuad = 4;	//!< �S�p�`�ɂ�����ӂ����邩
//! �S�p�`�̊e�ӂ̒��_�ԍ�
const unsigned int noelQuadEdge[nEdQuad][nNoEd] = {	
	{ 0, 1 },
	{ 1, 2 },
	{ 2, 3 },
	{ 3, 0 }
};
//! �S�p�`�̗אڊ֌W
static const unsigned int relQuadQuad[nNoQuad][nNoQuad] = {
	{ 0, 3, 2, 1 }, //  0
	{ 1, 0, 3, 2 }, //  1
	{ 2, 1, 0, 3 }, //  2
	{ 3, 2, 1, 0 }, //  3
};

//! �ӗv�f�\����
struct SBar{
	unsigned int v[2];	//!< ���_�ԍ�
	////////////////
    // �������O�E����1
	unsigned int s2[2];
	unsigned int r2[2];
};


////////////////////////////////////////////////////////////////

//! �S�p�`�v�f�\����
struct SQuad2D{
	unsigned int v[4];	//!< ���_Index
	int g2[4];			//!< �אڂ���v�f�z��ID(-1:�אڗv�f�Ȃ��A-2:�����̗v�f�z��ɗא�)
	unsigned int s2[4];	//!< �אڗv�fIndex
	unsigned int r2[4];			//!< �אڊ֌W
};


//! �Q�����R�p�`�v�f�\����
struct STri2D{
	unsigned int v[3];	//!< ���_Index
    int g2[3];			//!< �אڂ���v�f�z��ID(-1:�אڗv�f�Ȃ��A-2:�����̗v�f�z��ɗא�)
	unsigned int s2[3];	//!< �אڗv�fIndex
    unsigned int r2[3];	//!< �אڊ֌W
};

/*!
@brief �Q�����_�N���X
(CPoint2D.e!=-1)�Ȃ�(aTri[e].no[d])�����̓_�̑S�̐ߓ_�ԍ��ł���͂�
*/
class CPoint2D{
public:
	CPoint2D(){}
	CPoint2D( const CPoint2D& rhs )
		: p(rhs.p), e(rhs.e), d(rhs.d){}
	CPoint2D(double x, double y, int ielem, unsigned int idir)
		: p(x,y), e(ielem), d(idir){}
public:
    Com::CVector2D p;   //!< �_�̍��W
    int e;              //!< �_���͂ޗv�f�̂�����̔ԍ�(�Ǘ����Ă���_�Ȃ�-1������)
    unsigned int d;     //!< �_���͂ޗv�fe�̗v�f���ߓ_�ԍ�
};

////////////////////////////////////////////////////////////////

//! �_���͂ގO�p�`�̃��X�g�����
bool MakePointSurTri(const std::vector<Msh::STri2D>& aTri, const unsigned int npoin, 
		unsigned int* const elsup_ind, unsigned int& nelsup, unsigned int*& elsup );

//! �_���͂ނS�p�`�̃��X�g�����
bool MakePointSurQuad(const std::vector<Msh::SQuad2D>& aQuad, const unsigned int npoin, 
		unsigned int* const elsup_ind, unsigned int& nelsup, unsigned int*& elsup );
//! �_���͂ސ����v�f�̃��X�g�����
bool MakePointSurBar( const std::vector<Msh::SBar>& aBar, const unsigned int npoin, 
		unsigned int* const elsup_ind, unsigned int& nelsup, unsigned int*& elsup );

//! �R�p�`�����̓������(�אڊ֌W)�����
bool MakeInnerRelationTri( std::vector<Msh::STri2D>& aTri, const unsigned int npoin, 
	const unsigned int* elsup_ind, const unsigned int nelsup, const unsigned int* elsup);
//! �S�p�`�����̓������(�אڊ֌W)�����
bool MakeInnerRelationQuad( std::vector<Msh::SQuad2D>& aQuad, const unsigned int npoin, 
	const unsigned int* elsup_ind, const unsigned int nelsup, const unsigned int* elsup);
//! ���������̓������(�אڊ֌W)�����.T���A���͊܂܂Ȃ��Ƃ���
//bool MakeInnerRelationBar( std::vector<Msh::SBar>& aBar );

//! �O�p�`�����̊O�����͂ޕӗv�f�z������
bool MakeOuterBoundTri( const std::vector<Msh::STri2D>& aTri, std::vector<Msh::SBar>& aBar );
//! �S�p�`�����̊O�����͂ޕӗv�f�z������
bool MakeOuterBoundQuad( const std::vector<Msh::SQuad2D>& aQuad, std::vector<Msh::SBar>& aBar );

//! �O�p�`�����̐��������`�F�b�N
bool CheckTri( const std::vector<Msh::STri2D>& aTri );
//! �O�p�`�����̐��������`�F�b�N
bool CheckTri( const std::vector<Msh::CPoint2D>& po, const std::vector<Msh::STri2D>& tri );

/*!
@brief �_��v�f�����ɒǉ�����
@param[in] ipo_ins  �v�f�ɒǉ����钸�_�̔ԍ�
@param[in] itri_ins �ǉ�����v�f�̔ԍ�
@retval true    ����
@retval false   ���s
*/
bool InsertPoint_Elem( const unsigned int ipo_ins, const unsigned int itri_ins,
					  std::vector<Msh::CPoint2D>& po, std::vector<Msh::STri2D>& tri );
//! �_��v�f�̕ӂɒǉ�����
bool InsertPoint_ElemEdge( const unsigned int ipo_ins,
    const unsigned int itri_ins, const unsigned int ied_ins,
	std::vector<Msh::CPoint2D>& po, std::vector<Msh::STri2D>& tri );

//! �_����̃f���[�j����
bool DelaunayAroundPoint( unsigned int ipo0,
						 std::vector<Msh::CPoint2D>& po, std::vector<Msh::STri2D>& tri );

/*!
@brief �_����̃f���[�j����
num_flip�͕ӂ�Flip�����񐔂������ɑ������킹��D�O�ŏ������͂��Ȃ��D
*/
bool DelaunayAroundPoint( unsigned int itri, unsigned int inotri, 
						 std::vector<Com::CVector2D>& aVec, std::vector<Msh::STri2D>& tri,
						 unsigned int& num_flip);
//! �ӂ��t���b�v����
bool FlipEdge( unsigned int itri0, unsigned int ied0,  std::vector<Msh::STri2D>& aTri );
//! �ӂ��t���b�v����
bool FlipEdge( unsigned int itri0, unsigned int ied0,
			  std::vector<Msh::CPoint2D>& aPo, std::vector<Msh::STri2D>& aTri );
//! �ӂ��t���b�v����
bool FindEdge( const unsigned int& ipo0, const unsigned int& ipo1,
	unsigned int& itri0, unsigned int& inotri0, unsigned int& inotri1,
	const std::vector<Msh::CPoint2D>& po, const std::vector<Msh::STri2D>& tri );
//! ���������܂����ӂ�������
bool FindEdgePoint_AcrossEdge( const unsigned int& ipo0, const unsigned int& ipo1,
	unsigned int& itri0, unsigned int& inotri0, unsigned int& inotri1, double& ratio,
	std::vector<Msh::CPoint2D>& po, std::vector<Msh::STri2D>& tri );

//! �M�t�g�E���b�s���O�@�ɂ��O�p�`����
bool Tesselate_GiftWrapping(
	unsigned int ipo0, unsigned int ipo1,
	std::vector< std::pair<unsigned int,unsigned int> >& aTriEd,
	const std::vector<Com::CVector2D>& aPo, std::vector<Msh::STri2D>& aTri);

//! �O�p�`��������P�_���폜
bool DeletePointFromMesh(unsigned int ipoin, std::vector<Msh::CPoint2D>& aPo, std::vector<Msh::STri2D>& aTri);
//! �O�p�`��������P�v�f���폜
bool DeleteTri(unsigned int itri0, std::vector<Msh::CPoint2D>& aPo, std::vector<Msh::STri2D>& aTri);

//! ���v���V�A���X���[�W���O
void LaplacianSmoothing( std::vector<Msh::CPoint2D>& aPo, const std::vector<Msh::STri2D>& aTri,
	const std::vector<unsigned int>& aflg_isnt_move );
//! ���v���V�A���X���[�W���O
void LaplaceDelaunaySmoothing( std::vector<Msh::CPoint2D>& aPo, std::vector<Msh::STri2D>& aTri,
	const std::vector<unsigned int>& aflg_isnt_move );
//! �o�u�����b�V����p�����X���[�W���O
void PliantBossenHeckbertSmoothing( double elen, std::vector<Msh::CPoint2D>& aPo, std::vector<Msh::STri2D>& aTri );


//! BarAry��F��������
void ColorCodeBarAry( const std::vector<SBar>& aBar, const std::vector<Com::CVector2D>& aVec, 
					 std::vector< std::vector<unsigned int> >& aIndBarAry );

//! @}
} // end name space mesh;



#endif // MSH_H
