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
@brief �R�������b�V����Utility�֐��Q
@author Nobuyuki Umetani
*/


#if !defined(MESH_KERNEL_3D_H)
#define MESH_KERNEL_3D_H

#include <math.h>
#include <vector>
#include <cassert>
#include <map>

#include "delfem/vector3d.h"
#include "delfem/msh/meshkernel2d.h"	// SBar���Q�Ƃ��邽�߂�����include�B�{����SBar�̒�`�������ȊO�Ɉڂ����ق����ǂ��B

namespace Msh{

/*! 
@addtogroup Msh3D
*/
//! @{

const double ILL_CRT = 1.0e+20;

////////////////////////////////////////////////////////////////

/*! 
�l�ʑ̂̂���ʂ̔ԍ�
�O����݂Ĕ����v����ɂȂ�悤�ɔԍ������ׂ��Ă���D
*/
const unsigned int noelTetFace[4][3] = {
	{ 1, 2, 3 },
	{ 0, 3, 2 },
	{ 0, 1, 3 },
	{ 0, 2, 1 },
};

//! �l�ʑ̗̂אڊ֌W
static const unsigned int tetRel[12][4] = {
	{ 0, 1, 3, 2 }, //  0
	{ 0, 3, 2, 1 }, //  1 
	{ 0, 2, 1, 3 }, //  2

	{ 1, 0, 2, 3 }, //  3
	{ 1, 2, 3, 0 }, //  4
	{ 1, 3, 0, 2 }, //  5

	{ 2, 0, 3, 1 }, //  6
	{ 2, 3, 1, 0 }, //  7
	{ 2, 1, 0, 3 }, //  8

	{ 3, 0, 1, 2 }, //  9
	{ 3, 1, 2, 0 }, // 10
	{ 3, 2, 0, 1 }, // 11
};

//! �l�ʑ̗̂אڊ֌W�̋t
const unsigned int invTetRel[12] = {
	 0, //  0
	 1, //  1
	 2, //  2
	 3, //  3
	 9, //  4
	 6, //  5
	 5, //  6 
	11, //  7
	 8, //  8
	 4, //  9
	10, // 10
	 7, // 11
};

//! ���_�O�̑Ή��֌Wa�ƒ��_�P�̑Ή��֌Wb�ɂ�����a*4+b�������Ƃ��ėאڊ֌W�𓾂�z��
const int noel2Rel[16] = {
	 -1, //  0 00
	  0, //  1 01
	  2, //  2 02
	  1, //  3 03
	  3, //  4 10
	 -1, //  5 11
	  4, //  6 12
	  5, //  7 13
	  6, //  8 20
	  8, //  9 21
	 -1, // 10 22
	  7, // 11 23
	  9, // 12 30
	 10, // 13 31
	 11, // 14 32
	 -1, // 15 33
};

//! �l�ʑ̒��̖����ӂ̐�
const unsigned int nSEdgeTet = 6;
//! �l�ʑ̒��̖����ӂ̒��_(�������ɕ���ł���)
const unsigned int sEdge2Noel[6][2] = {
	{ 0, 1 },
	{ 0, 2 },
	{ 0, 3 },
	{ 1, 2 },
	{ 1, 3 },
	{ 2, 3 }
};
//! �l�ʑ̖̂����ӂ̗����̖ʔԍ�
const unsigned int sEdge2FaTet[6][2] = {
	{ 3, 2 },
	{ 1, 3 },
	{ 2, 1 },
	{ 3, 0 },
	{ 0, 2 },
	{ 1, 0 }
};

//! �l�ʑ̒��̗L���ӂ̐�
const unsigned int nDEdgeTet = 12;
//! �l�ʑ̒��̗L���ӂ̒��_
const unsigned int dEdge2Noel[12][2] = {
	{ 0, 1 },	//  0 
	{ 0, 2 },	//  1
	{ 0, 3 },	//  2
	{ 1, 0 },	//  3
	{ 1, 2 },	//  4
	{ 1, 3 },	//  5
	{ 2, 0 },	//  6
	{ 2, 1 },	//  7
	{ 2, 3 },	//  8
	{ 3, 0 },	//  9
	{ 3, 1 },	// 10
	{ 3, 2 },	// 11
};
//! �l�ʑ̒��̂Q���_ab�����ԗL���ӂ̔ԍ���a*4+b���瓾��z��
const int noel2DEdge[16] = {
	 -1, //  0 00
	  0, //  1 01	
	  1, //  2 02
	  2, //  3 03
	  3, //  4 10
	 -1, //  5 11
	  4, //  6 12
	  5, //  7 13
	  6, //  8 20
	  7, //  9 21
	 -1, // 10 22
	  8, // 11 23
	  9, // 12 30
	 10, // 13 31
	 11, // 14 32
	 -1, // 15 33
};
//! �����ӂ���L���ӂւ̃}�b�s���O�z��
const unsigned int sEdge2DEdge[6] = {
	0,
	1,
	2,
	4,
	5,
	8
};

////////////////////////////////////////////////////////////////
/*!
�l�ʑ̂̂���ʂ̔ԍ�
�O����݂Ĕ����v����ɂȂ�悤�ɔԍ������ׂ��Ă���D
*/
const unsigned int noelHexFace[6][4] = {
	{ 0, 3, 2, 1 },
	{ 0, 1, 5, 4 },
	{ 1, 2, 6, 5 }, 
	{ 2, 3, 7, 6 },
	{ 3, 0, 4, 7 },
	{ 4, 5, 6, 7 }
};

////////////////////////////////////////////////////////////////


//! �ӎ���̗v�f
class ElemAroundEdge{
public:
	ElemAroundEdge(){
		e.reserve(32);
		n.reserve(32);
	}
    std::vector< std::pair<unsigned int,unsigned int> > e;
    std::vector< unsigned int > n;
    unsigned int nod;
    unsigned int nou;
	bool is_inner;
public:
	void clear(){
		e.clear();
		n.clear();
	}
    unsigned int size() const {
		return e.size();
	}
};

//! �_����̗v�f
class ElemAroundPoint{
public:
	std::map<int,unsigned int> e;
	bool is_inner;
public:
	void clear(){
		e.clear();
	}
	int size() const {
		return e.size();
	}
};

////////////////////////////////

class CFlipCrtPrePosPtn
{
public:
	double pre;
	double pos;
	int ptn;
	bool ok_flg;
public:
	CFlipCrtPrePosPtn(const CFlipCrtPrePosPtn& rhs){
		this->pre = rhs.pre;
		this->pos = rhs.pos;
		this->ptn = rhs.ptn;
		this->ok_flg = rhs.ok_flg;
	}
	CFlipCrtPrePosPtn( double pre, double pos, int ptn ){
		this->pre = pre;	this->pos = pos;	this->ptn = ptn;
		if( pos > pre || ptn < 0 || pos > ILL_CRT*0.9 ) ok_flg = false;
		else ok_flg = true;
	}
	CFlipCrtPrePosPtn(){
		this->pre = -1.0;	this->pos = ILL_CRT;	this->ptn = -1;
		ok_flg = false;
	}
	void SetData( double pre, double pos, int ptn ){
		this->pre = pre;	this->pos = pos;	this->ptn = ptn;
		if( pos > pre || ptn < 0 || pos > ILL_CRT*0.9 ) ok_flg = false;
		else ok_flg = true;
	}
	bool operator < (const CFlipCrtPrePosPtn& rhs){
		if(  this->ok_flg && !rhs.ok_flg ) return true;
		if( !this->ok_flg &&  rhs.ok_flg ) return false;
		if( fabs( this->pre - rhs.pre ) > 1.0e-8 ){
			return this->pre > rhs.pre;
		}
		if( fabs( this->pos - rhs.pos ) > 1.0e-8 ){
			return this->pos < rhs.pos;
		}
		return true;
	}
	bool operator > (const CFlipCrtPrePosPtn& rhs){
		return !( *this < rhs );
	}
};

bool operator < (const CFlipCrtPrePosPtn& lhs, const CFlipCrtPrePosPtn& rhs);



//! �R�����̓_�N���X
class CPoint3D{
public:
	CPoint3D(){}
	CPoint3D( const CPoint3D& rhs ) 
        : p(rhs.p), e(rhs.e),poel(rhs.poel),old_p(rhs.old_p){}
	CPoint3D(double x, double y, double z) 
        : p(x,y,z), e(-1), poel(0), old_p(-1){}
	bool operator < (const CPoint3D& rhs){
		if( fabs( this->p.x - rhs.p.x ) > 1.0e-5 ){
			return this->p.x < rhs.p.x;
		}
		if( fabs( this->p.y - rhs.p.y ) > 1.0e-5 ){
			return this->p.y < rhs.p.y;
		}
		if( fabs( this->p.z - rhs.p.z ) > 1.0e-5 ){
			return this->p.z < rhs.p.z;
		}
		return false;
	}
public:
    Com::CVector3D p;
	int e;		//!< �v�f�ԍ�
	unsigned int poel;	//!< �v�f�̂ǂ̐ߓ_�ɐڂ��Ă��邩
    int old_p;	//!< �ύX�O�̐ߓ_�ԍ�
};

//! �l�ʑ̗v�f�\����
struct STet{
	unsigned int v[4];	//<! ���_��Index
	unsigned int s[4];	//<! �אڗv�f��Index
	int g[4];
	unsigned int f[4];
	int old_t;
};

//! �U�ʑ̗v�f�\����
struct SHex{
	unsigned int v[8];	//<! ���_��Index
	unsigned int s[6];	//<! �אڗv�f��Index
	int g[6];
	unsigned int f[6];
	int old_h;
};

//! �R�����O�p�`�v�f�\����
struct STri3D{
	unsigned int v[3];	//<! ���_��Index
	////////////////
    unsigned int s2[3];	//<! �אڗv�f��Index
	int g2[3];
	unsigned int r2[3];
	////////////////
	int sf[2];	//<! �אڗv�f��Index
	int gf[2];
	unsigned int df[2];
};

//! �R�����S�p�`�v�f�\����
struct SQuad3D{
	unsigned int v[4];	//<! ���_��Index
	////////////////
	int se[4];	//<! �אڗv�f��Index
	int ge[4];
	unsigned int de[4];
	////////////////
	int sf[2];	//<! �אڗv�f��Index
	int gf[2];
	unsigned int df[2];
};



////////////////////////////////////////////////////////////////

//! �l�ʑ̕����̐��������`�F�b�N
bool CheckTet(const std::vector<Msh::STet>& tet,
			  const std::vector<Msh::CPoint3D>& vertex);
//! �l�ʑ̕����̐��������`�F�b�N
bool CheckTet(const std::vector<Msh::STet>& tet);
//! �R�p�`�����̐��������`�F�b�N
bool CheckTri(const std::vector<Msh::STri3D>& tri);

////////////////////////////////////////////////////////////////


//! �l�ʑ̕����̕ӂ̃��X�g�𓾂�
bool MakeEdgeTet(unsigned int& nedge,
			  unsigned int*& edge_ind,
			  unsigned int*& edge,
			  const std::vector<Msh::STet>& tet,
			  const unsigned int nnode);
//! �e�m�[�h���͂ގl�ʑ̂̂P�����
bool MakeOneTetSurNo(const std::vector<Msh::STet>& tet,
				  std::vector<Msh::CPoint3D>& point);

////////////////

//! �l�ʑ̂��͂ގl�ʑ̂����
bool MakeTetSurTet(std::vector<Msh::STet>& tet);
//! �U�ʑ̂��͂ނU�ʑ̂����
bool MakeHexSurHex(std::vector<Msh::SHex>& aHex);
//! �R�p�`���͂ނR�p�`�����
bool MakeTriSurTri(std::vector<Msh::STri3D>& tri);

////////////////

//! �_���͂ނU�ʑ̂̃��X�g�����
bool MakeHexSurNo(unsigned int& ntetsuno,
				  unsigned int*& tetsuno_ind,
				  unsigned int*& tetsuno,
				  const std::vector<Msh::SHex>& aHex,
				  const unsigned int nnode );
//! �_���͂ގl�ʑ̂̃��X�g�����
bool MakeTetSurNo(const std::vector<Msh::STet>& aTet,
				  const unsigned int nnode,
				  unsigned int& ntetsuno,
				  unsigned int*& tetsuno_ind,
				  unsigned int*& tetsuno );
//! �_���͂ނR�p�`�̃��X�g�����
bool MakeTriSurNo(unsigned int& ntrisuno,
				  unsigned int*& trisuno_ind,
				  unsigned int*& trisuno,
				  const std::vector<Msh::STri3D>& tri,
				  const unsigned int nnode );

////////////////

//! �l�ʑ̂��͂ނR�p�`�����
bool MakeTriSurTet(std::vector<Msh::STet>& in_tet,
				   std::vector<Msh::STri3D>& in_tri,
				   const std::vector<Msh::CPoint3D>& vertex );

////////////////

//! �l�ʑ̕����̋��E�̂R�p�`�����
bool MakeOuterBoundTet( const std::vector<Msh::STet  >& aTet, std::vector<Msh::STri3D>& aTri );
//! �U�ʑ̕����̋��E�̂S�p�`�����
bool MakeOuterBoundTri( const std::vector<Msh::STri3D>& aTri, std::vector<Msh::SBar  >& aBar );

////////////////

// �׍����O�p�`�̒P�ʖ@�����m��crt_dot�ȉ��Ȃ炻��͐؂藣��
bool MakeColorCoding_Tri3D( const std::vector<Msh::STri3D>& aTri, const std::vector<Com::CVector3D>& aVec3D, 
						   std::vector<int>& aColorTri, unsigned int& nColor, double dot_crt);

bool MakeBar_fromColorCodingTri( const std::vector<Msh::STri3D>& aTri, 
								const std::vector<int>& aColorTri, unsigned int nColor, std::vector<Msh::SBar>& aBar);

/*
bool MakeColorCodingBar( const std::vector<Msh::SBar> aBar, 
							 std::vector<int>& aColorBar, unsigned int& nColorBar);

bool MakeColorCodingBar( const std::vector<Msh::SBar>& aBar,  const std::vector<Com::CVector3D>& aVec, 
						  std::vector<int>& aColorBar, unsigned int& nColor);
*/						  


////////////////////////////////////////////////////////////////


//! �R�p�`�̕ӂɓ_��������
bool AddPointTri_Edge(const unsigned int ipo_ins,
					  const unsigned int itr0,
					  const unsigned int iedtri0,
					  std::vector<Msh::STri3D>& aTri );

//! �R�p�`�̖ʂɓ_��������
bool AddPointTri_Face(const unsigned int ipo_ins,
					  const unsigned int itr0,
					  std::vector<Msh::STri3D>& aTri );

bool GetAddPointEdgeCrt(const ElemAroundEdge& elared,
						const Com::CVector3D& add_point,
						double& max_crt,
						const std::vector<Msh::CPoint3D>& node);

//! �l�ʑ̂̕ӂɓ_��������
bool AddPointTet_Edge(const ElemAroundEdge& elared,
				  const unsigned int ipo_ins,
				  std::vector<Msh::CPoint3D>& node,
				  std::vector<Msh::STet>& tet);

//! �l�ʑ̂̒��ɓ_��������
bool AddPointTet_Elem(const unsigned int itet0,
				  const unsigned int ipo_ins,
				  std::vector<Msh::CPoint3D>& po,
				  std::vector<Msh::STet>& tet);

//! �l�ʑ̖̂ʂɓ_��������
bool AddPointTet_Face(const unsigned int itet0, 
				  const unsigned int ifatet,
				  const unsigned int ipo_ins,
				  std::vector<Msh::CPoint3D>& po,
				  std::vector<Msh::STet>& tet);

//! �ӂ���������
bool EdgeDel(const ElemAroundEdge& elared,
			 const ElemAroundPoint& elarpo0,
			 std::vector<Msh::STet>& tet,
			 std::vector<Msh::CPoint3D>& node);

bool GetEdgeDelCrt(const ElemAroundEdge& elared,
				   const ElemAroundPoint& elarpo,
				   double& max_crt,
				   const std::vector<Msh::STet>& tet,
				   const std::vector<Msh::CPoint3D>& node );

//! �ӎ���̗v�f���擾
bool MakeElemAroundEdge( ElemAroundEdge& elared,
						const int itet0, 
						const int idedge0, 
						const std::vector<Msh::STet>& tet );

double MaxCrtElemAroundPoint(const ElemAroundPoint& elarpo,
						  const std::vector<Msh::STet>& tet,
						  const std::vector<Msh::CPoint3D>& node);

//! �_����̗v�f���擾
bool MakeElemAroundPoint( ElemAroundPoint& elarpo,
						const unsigned int itet0, 
						const unsigned int inoel0, 
						const std::vector<Msh::STet>& tet );

////////////////

double MaxCrtElemAroundEdge(const ElemAroundEdge& elared,
						  const std::vector<Msh::STet>& tet,
						  const std::vector<Msh::CPoint3D>& node);

bool GetEdgeSwapPtnCrt(const ElemAroundEdge& elared,
				int& ptn,
				double& min_max_crt,
				const std::vector<Msh::CPoint3D>& node );

bool EdgeSwapTet(const ElemAroundEdge& elared,
		  const int ptn,
		  std::vector<Msh::STet>& tet,
		  std::vector<Msh::CPoint3D>& point );

////////////////

bool GetFaceSwapCrt(const int itet0,
					const int iface0,
					double& max_crt,
					const std::vector<Msh::STet>& tet,
					const std::vector<Msh::CPoint3D>& node );

bool FaceSwapTet(const unsigned int itet0, 
				  const unsigned int iface0, 
				  std::vector<Msh::STet>& tet,
				  std::vector<Msh::CPoint3D>& node);


bool DelaunayAroundPointTet(const unsigned int ipoin,
						 std::vector<Msh::CPoint3D>& aPo3D,
						 std::vector<Msh::STet>& aTet);


bool DelaunayAroundPointTri(unsigned int itri, 
							unsigned int inotri, 
							std::vector<Com::CVector3D>& aVec,
							std::vector<Msh::STri3D>& aTri);

bool FlipEdgeTri( const unsigned int itri0, const unsigned int ied0,
			   std::vector<Msh::STri3D>& tri );

////////////////////////////////////////////////////////////////
						 
bool ReconnectCap(const unsigned int itet0,
					std::vector<Msh::STet>& tet,
					std::vector<Msh::CPoint3D>& node);

bool ReconnectSliver(const unsigned int itet0,
					std::vector<Msh::STet>& tet,
					std::vector<Msh::CPoint3D>& node);

bool Reconnect(std::vector<Msh::STet>& tet,
			   std::vector<Msh::CPoint3D>& node);





////////////////////////////////////////////////////////////////

//! �l�ʑ̂̑̐�
inline double TetVolume(unsigned int iv1, unsigned int iv2, unsigned int iv3, unsigned int iv4,
		const std::vector<Msh::CPoint3D>& point)
{
	return Com::TetVolume( point[iv1].p, point[iv2].p, point[iv3].p, point[iv4].p );
}

//! �l�ʑ̂̑̐�
inline double TetVolume(const Msh::STet& tet, 
		const std::vector<Msh::CPoint3D>& node)
{
	return Com::TetVolume( 
		node[ tet.v[0] ].p,
		node[ tet.v[1] ].p, 
		node[ tet.v[2] ].p, 
		node[ tet.v[3] ].p );
}

//! �l�ʑ̂̑̐�
inline double TetVolume(int ielem, 
		const std::vector<Msh::STet>& tet, 
		const std::vector<Msh::CPoint3D>& node)
{
	return TetVolume( tet[ielem], node );
}

////////////////////////////////////////////////

//! �R�p�`�̖ʐ�
inline double TriArea(const Msh::STet& tet, 
		const int iface, 
		const std::vector<Msh::CPoint3D>& node )
{
	return Com::TriArea( 
        node[ tet.v[(int)noelTetFace[iface][0]] ].p,
        node[ tet.v[(int)noelTetFace[iface][1]] ].p,
        node[ tet.v[(int)noelTetFace[iface][2]] ].p );
}

//! �R�p�`�̖ʐ�
inline double TriArea(const int itet,
		const int iface,
		const std::vector<Msh::STet>& tet,
		const std::vector<Msh::CPoint3D>& node )
{
	return TriArea(tet[itet],iface,node);
}

////////////////////////////////////////////////

inline double SquareLongestEdgeLength(const int itet,
		const std::vector<Msh::CPoint3D>& node,
		const std::vector<Msh::STet>& tet )
{
	return Com::SqareLongestEdgeLength(
		node[tet[itet].v[0]].p,
		node[tet[itet].v[1]].p,
		node[tet[itet].v[2]].p,
		node[tet[itet].v[3]].p );
}

////////////////////////////////////////////////

inline double LongestEdgeLength(const int itet,
		const std::vector<Msh::CPoint3D>& node,
		const std::vector<Msh::STet>& tet )
{
	return sqrt( Com::SqareLongestEdgeLength(
			node[tet[itet].v[0]].p,
			node[tet[itet].v[1]].p,
			node[tet[itet].v[2]].p,
			node[tet[itet].v[3]].p ) );
}

////////////////////////////////////////////////

inline double SquareShortestEdgeLength(const int itet,
		const std::vector<Msh::CPoint3D>& node,
		const std::vector<Msh::STet>& tet )
{
	return Com::SqareShortestEdgeLength(
		node[tet[itet].v[0]].p,
		node[tet[itet].v[1]].p,
		node[tet[itet].v[2]].p,
		node[tet[itet].v[3]].p );
}

////////////////////////////////////////////////

inline double ShortestEdgeLength(const int itet,
		const std::vector<Msh::CPoint3D>& node,
		const std::vector<Msh::STet>& tet )
{
	return sqrt( Com::SqareShortestEdgeLength(
		node[tet[itet].v[0]].p,
		node[tet[itet].v[1]].p,
		node[tet[itet].v[2]].p,
		node[tet[itet].v[3]].p ) );
}

////////////////////////////////////////////////

//! �@���̎擾(�����P�Ƃ͌���Ȃ�)
inline void Normal(
		Com::CVector3D& vnorm, 
		const unsigned int itet0, const unsigned int iface0, 
		const std::vector<Msh::STet>& tet, const std::vector<Msh::CPoint3D>& node)
{
	assert( itet0 < tet.size() );
	assert( iface0 < 4 );
	Com::Normal(vnorm,
        node[ tet[itet0].v[(int)noelTetFace[iface0][0] ] ].p,
        node[ tet[itet0].v[(int)noelTetFace[iface0][1] ] ].p,
        node[ tet[itet0].v[(int)noelTetFace[iface0][2] ] ].p );
}

////////////////////////////////////////////////

//! �R�p�`�̒P�ʖ@��
inline void UnitNormal(
		Com::CVector3D& vnorm,
		const int itet0,
		const int iface0,
		const std::vector<Msh::STet>& aTet,
		const std::vector<Msh::CPoint3D>& aPo )
{
	Com::UnitNormal(vnorm,
        aPo[ aTet[itet0].v[ (int)noelTetFace[iface0][0] ] ].p,
        aPo[ aTet[itet0].v[ (int)noelTetFace[iface0][1] ] ].p,
        aPo[ aTet[itet0].v[ (int)noelTetFace[iface0][2] ] ].p );
}

//! �R�p�`�̒P�ʖ@��
inline void UnitNormal(
		Com::CVector3D& vnorm,
		const int itri,
		const std::vector<Msh::STri3D>& aTri,
		const std::vector<Com::CVector3D>& aVec )
{
	Com::UnitNormal(vnorm,
		aVec[ aTri[itri].v[0] ],
		aVec[ aTri[itri].v[1] ],
		aVec[ aTri[itri].v[2] ] );
}

////////////////////////////////////////////////

//! �R�p�`�̊O���~
inline double Circumradius(
		const int itet0, 
		const std::vector<Msh::CPoint3D>& node,
		const std::vector<Msh::STet>& tet )
{
	return sqrt( Com::SquareCircumradius(
		node[ tet[itet0].v[0] ].p,
		node[ tet[itet0].v[1] ].p,
		node[ tet[itet0].v[2] ].p,
		node[ tet[itet0].v[3] ].p) );
}

////////////////////////////////////////////////

inline double Criterion_Asp(
		const Com::CVector3D& po0, 
		const Com::CVector3D& po1, 
		const Com::CVector3D& po2, 
		const Com::CVector3D& po3)
{
	double saface = Com::TriArea( po1, po3, po2 )
				+	Com::TriArea( po0, po2, po3 )
				+	Com::TriArea( po0, po3, po1 )
				+   Com::TriArea( po0, po1, po2 );
	double inscribed_radius = Com::TetVolume(po0,po1,po2,po3) * 3.0 / saface;
	double circum_radius = Com::Circumradius(po0,po1,po2,po3);
	return circum_radius / inscribed_radius;
}

inline double Criterion_Asp(const Msh::STet& tet,
		const std::vector<Msh::CPoint3D>& node)
{
	return Criterion_Asp(
		node[tet.v[0]].p,
		node[tet.v[1]].p,
		node[tet.v[2]].p,
		node[tet.v[3]].p);
}

inline double Criterion_Asp(int ielem,
		const std::vector<Msh::CPoint3D>& node,
		const std::vector<Msh::STet>& tet)
{
	return Criterion_Asp(tet[ielem],node);
}

////////////////////////////////////////////////

inline double Criterion_PLJ(
		const Com::CVector3D& po0, 
		const Com::CVector3D& po1, 
		const Com::CVector3D& po2, 
		const Com::CVector3D& po3)
{
	const double longest_edge = Com::LongestEdgeLength(po0,po1,po2,po3);
	double saface = Com::TriArea( po1, po3, po2 )
				+	Com::TriArea( po0, po2, po3 )
				+	Com::TriArea( po0, po3, po1 )
				+   Com::TriArea( po0, po1, po2 );
	return (saface*longest_edge)/(Com::TetVolume(po0,po1,po2,po3)*3.0);
}

inline double Criterion_PLJ(
		const STet& tet,
		const std::vector<CPoint3D>& node)
{
	return Criterion_PLJ(
		node[tet.v[0]].p,
		node[tet.v[1]].p,
		node[tet.v[2]].p,
		node[tet.v[3]].p );
}

inline double Criterion_PLJ(
		int ielem,
		const std::vector<CPoint3D>& node,
		const std::vector<STet>& tet)
{
	return Criterion_PLJ(tet[ielem],node);
}

inline double Criterion_PLJ(
		const CPoint3D& po0, 
		const CPoint3D& po1, 
		const CPoint3D& po2, 
		const CPoint3D& po3)
{
	return Criterion_PLJ(po0.p, po1.p, po2.p, po3.p);
}

////////////////////////////////////////////////

/*! 
@brief �h���l�[�����̃`�F�b�N
v1, v2, v3�̊O�ڋ��̒��ł����Ƃ����a�̑傫�ȋ�(���S��v1,v2,v3�̊O�S�j�̒���v4���܂܂�Ă��邩�ǂ������ׂ�
@param[in] v1 �_
@param[in] v2 �_
@param[in] v3 �_
@param[in] v4 �_
@retval 1 �����Ă��Ȃ����
@retval 0 �����Ȃ�
@retval -1 �����Ă����
*/
inline int DetDelaunay(const Com::CVector3D& v1, 
					   const Com::CVector3D& v2, 
					   const Com::CVector3D& v3, 
					   const Com::CVector3D& v4){
	
	// �R�p�`v1,v2,v3�̊O�ډ~�̒��S�����߂�B
	const double dtmp1 = (v2.x-v3.x)*(v2.x-v3.x)+(v2.y-v3.y)*(v2.y-v3.y)+(v2.z-v3.z)*(v2.z-v3.z);
	const double dtmp2 = (v3.x-v1.x)*(v3.x-v1.x)+(v3.y-v1.y)*(v3.y-v1.y)+(v3.z-v1.z)*(v3.z-v1.z);
	const double dtmp3 = (v1.x-v2.x)*(v1.x-v2.x)+(v1.y-v2.y)*(v1.y-v2.y)+(v1.z-v2.z)*(v1.z-v2.z);

	double qarea = Com::SquareTriArea(v1,v2,v3);
	const double etmp1 = dtmp1*(dtmp2+dtmp3-dtmp1) / (16.0 * qarea );
	const double etmp2 = dtmp2*(dtmp3+dtmp1-dtmp2) / (16.0 * qarea );
	const double etmp3 = dtmp3*(dtmp1+dtmp2-dtmp3) / (16.0 * qarea );

	Com::CVector3D out_center(
		etmp1*v1.x + etmp2*v2.x + etmp3*v3.x,
		etmp1*v1.y + etmp2*v2.y + etmp3*v3.y,
		etmp1*v1.z + etmp2*v2.z + etmp3*v3.z );
	
	const double qradius = Com::SquareDistance( out_center, v1 );
	assert( fabs( qradius - Com::SquareDistance(out_center,v2) ) < 1.0e-10 );
	assert( fabs( qradius - Com::SquareDistance(out_center,v3) ) < 1.0e-10 );
	assert( fabs( Com::Height(v1,v2,v3,out_center) ) < 1.0e-10 );

	const double distance = Com::SquareDistance( out_center, v4 );
	if( distance > qradius + 1.0e-6 ){
		return 1;
	}
	else{
		if( distance < qradius - 1.0e-6 ){
			return -1;
		}
		else{
			return 0;
		}
	}
	assert(0);
	return -1;
}

/*! 
@brief �h���l�[�����̃`�F�b�N
v1, v2, v3�Cv4�̊O�ڋ��̒���v5���܂܂�Ă��邩�ǂ������ׂ�
@param[in] v1 �_
@param[in] v2 �_
@param[in] v3 �_
@param[in] v4 �_
@param[in] v5 �_
@retval >0 �����Ă��Ȃ����
@retval <0 �����Ă����
*/
inline double DetDelaunay3D(
		const Com::CVector3D& v1, 
		const Com::CVector3D& v2, 
		const Com::CVector3D& v3, 
		const Com::CVector3D& v4, 
		const Com::CVector3D& v5)
{
	const double a[12] = {
		v1.x-v5.x,	//  0
		v2.x-v5.x,	//  1
		v3.x-v5.x,	//  2
		v4.x-v5.x,	//  3
		v1.y-v5.y,	//  4
		v2.y-v5.y,	//  5
		v3.y-v5.y,	//  6
		v4.y-v5.y,	//  7
		v1.z-v5.z,	//  8
		v2.z-v5.z,	//  9
		v3.z-v5.z,	// 10
		v4.z-v5.z,	// 11
	};
	const double b[6] = {
		a[ 6]*a[11]-a[ 7]*a[10],	// 0
		a[ 5]*a[11]-a[ 7]*a[ 9],	// 1
		a[ 5]*a[10]-a[ 6]*a[ 9],	// 2
		a[ 7]*a[ 8]-a[ 4]*a[11],	// 3
		a[ 6]*a[ 8]-a[ 4]*a[10],	// 4
		a[ 4]*a[ 9]-a[ 5]*a[ 8],	// 5 
	};
	return	-( a[0]*(v1.x+v5.x)+a[4]*(v1.y+v5.y)+a[ 8]*(v1.z+v5.z) )*( a[ 1]*b[0]-a[ 2]*b[1]+a[ 3]*b[2] )
			+( a[1]*(v2.x+v5.x)+a[5]*(v2.y+v5.y)+a[ 9]*(v2.z+v5.z) )*( a[ 0]*b[0]+a[ 2]*b[3]-a[ 3]*b[4] )
			-( a[2]*(v3.x+v5.x)+a[6]*(v3.y+v5.y)+a[10]*(v3.z+v5.z) )*( a[ 0]*b[1]+a[ 1]*b[3]+a[ 3]*b[5] )
			+( a[3]*(v4.x+v5.x)+a[7]*(v4.y+v5.y)+a[11]*(v4.z+v5.z) )*( a[ 0]*b[2]+a[ 1]*b[4]+a[ 2]*b[5] );
}

/*! 
@brief �h���l�[�����̃`�F�b�N
v1, v2, v3�Cv4�̊O�ڋ��̒���v5���܂܂�Ă��邩�ǂ������ׂ�
@param[in] v1 �_��Index
@param[in] v2 �_��Index
@param[in] v3 �_��Index
@param[in] v4 �_��Index
@param[in] v5 �_��Index
@param[in] node ���W�z��
@retval >0 �����Ă��Ȃ����
@retval <0 �����Ă����
*/
inline double DetDelaunay3D( const int v1, const int v2, const int v3, const int v4, const int v5, 
				  const std::vector<Com::CVector3D>& node )
{
	return DetDelaunay3D(node[v1],node[v2],node[v3],node[v4],node[v5]);
}

//! @}

} // end namespace Msh;


////////////////////////////////////////////////////////////////
#endif //MESH_KER_H
