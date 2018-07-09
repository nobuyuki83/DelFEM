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
@brief interface of the class (Cad::CBRep) wich represents topology with B-Rep data strcutre
@author Nobuyuki Umetani
*/

#if !defined(B_REP_H)
#define B_REP_H

#ifdef __VISUALC__
	#pragma warning( disable : 4786 )
#endif

#include "delfem/cad/objset_cad.h"

namespace Cad{

//! @addtogroup CAD
//! @{

//! topology loop class
class CUseLoop{
public:
	CUseLoop(const CUseLoop& rhs)
  : id(rhs.id), id_l(rhs.id_l), id_he(rhs.id_he), id_ul_c(rhs.id_ul_c), id_ul_p(rhs.id_ul_p){}
  CUseLoop(const unsigned int id,
           const unsigned int id_he, const unsigned int id_ul_c, const unsigned int id_ul_p)
  : id(id), id_l(0), id_he(id_he), id_ul_c(id_ul_c), id_ul_p(id_ul_p){}
public:
  unsigned int id;    //!< ID
  
	// geometry element ID (brep.cpp��brep2d.cpp�ł͎Q�Ƃ���Ȃ��͂��H)
  unsigned int id_l;	//!< �O���̃��[�v�̏ꍇ�͂O
  
	// topology elemnet ID
  unsigned int id_he;   //!< HalfEdge��ID
  unsigned int id_ul_c;	//!< �q���[�v�Aid_ul_c=0�̏ꍇ�̓��X�g�̏I���
  unsigned int id_ul_p;	//!< �e���[�v�Bid_l��id_ul_p�������Ă���Did_ul_p==id�̏ꍇ�͎������e�Cid_ul_p==0�̏ꍇ�͊O���̃��[�v
};

//! topoloty HalfEdge class
class CHalfEdge{
public:
	CHalfEdge(const CHalfEdge& rhs)
		: id(rhs.id),  
		id_uv(rhs.id_uv),
		id_he_f(rhs.id_he_f), id_he_b(rhs.id_he_b), id_he_o(rhs.id_he_o),
		id_ul(rhs.id_ul),
		id_e(rhs.id_e), is_same_dir(rhs.is_same_dir){}
  CHalfEdge(const unsigned int id,
            const unsigned int id_uv,
            const unsigned int id_he_f, const unsigned int id_he_b, const unsigned int id_he_o,
            const unsigned int id_ul )
		: id(id), 
		id_uv(id_uv), 
		id_he_f(id_he_f), id_he_b(id_he_b), id_he_o(id_he_o), 
		id_ul(id_ul), 
		id_e(0), is_same_dir(true){}
public:
  unsigned int id;        //!< ID
  unsigned int id_uv;     //!< ID of UseVertex
  unsigned int id_he_f;   //!< ID of previous HalfEdge
  unsigned int id_he_b;   //!< ID of later HalfEdge
  unsigned int id_he_o;	  //!< ID of opposite side HalfEdge (if UV is floating point, this become self )
  unsigned int id_ul;     //!< ID of UseLoop
  
  ////
  unsigned int id_e;      //!< id of edge geometry. 0 if uv is a floating vertex
  bool is_same_dir;       //!< is the geometry edge goes same direction as topology edge
};

//! �ʑ����_�N���X
class CUseVertex{
public:
  CUseVertex(const unsigned int id, const unsigned int id_he)
		: id(id), id_he(id_he), id_v(0){}	
	CUseVertex(const CUseVertex& rhs)
		: id(rhs.id), id_he(rhs.id_he), id_v(rhs.id_v){}	
public:
  unsigned int id;    //!< ID
  unsigned int id_he; //!< HalfEdge��ID
	//��������͊􉽗v�fID
  unsigned int id_v;  //!< �􉽒��_��ID
};

//! B-rep��p�����ʑ����i�[�N���X
class CBRep{
public:
	//! �S�Ă̗v�f���폜���āC������Ԃɖ߂�
	void Clear();

	bool IsID_UseLoop(unsigned int id_ul) const { return m_UseLoopSet.IsObjID(id_ul); }
	bool IsID_HalfEdge(unsigned int id_he) const { return m_HalfEdgeSet.IsObjID(id_he); }
	bool IsID_UseVertex(unsigned int id_uv) const { return m_UseVertexSet.IsObjID(id_uv); }
	std::vector<unsigned int> GetAry_UseVertexID() const { return m_UseVertexSet.GetAry_ObjID(); }

	const CUseLoop& GetUseLoop(unsigned int id_ul) const;
	const CUseVertex& GetUseVertex(unsigned int id_uv) const;
	const CHalfEdge& GetHalfEdge(unsigned int id_he) const;

	bool SetLoopIDtoUseLoop(unsigned int id_ul, unsigned int id_l);
	bool SetVertexIDtoUseVertex(unsigned int id_uv, unsigned int id_v);
	bool SetEdgeIDtoHalfEdge(unsigned int id_he, unsigned int id_e, bool is_same_dir);
	
	int AssertValid_Use() const;
	std::vector<unsigned int> FindHalfEdge_Edge(const unsigned int& id_e) const;
	std::vector<unsigned int> FindHalfEdge_Vertex(const unsigned int& id_v) const;
	
	////////////////////////////////
	// �I�C���[����

  //! �_�Ɠ_���q���ă��[�v�ƃG�b�W�����
	bool MEVVL(unsigned int& id_he_add1, unsigned int& id_he_add2,
             unsigned int& id_uv_add1, unsigned int& id_uv_add2, unsigned int& id_ul_add );
	//! ���[�v���Q�ɕ�����
	bool MEL(unsigned int& id_he_add1, unsigned int& id_he_add2, unsigned int& id_ul_add,
           const unsigned int id_he1, const unsigned int id_he2);
	/*!
	@brief �ӂ��������ĂQ�̃��[�v���P�ɂ��� ����he_rem�̔��Α��̔��ӂ��ڂ��锼���[�v����������
	*/
	bool KEL(const unsigned int id_he_rem1);
	//! id_he�̋N�_����A������L�΂�
	bool MEV(unsigned int& id_he_add1,unsigned int& id_he_add2, unsigned int& id_uv_add, 
           const unsigned int id_he);
	//! id_he�̋N�_���������ĂQ�̕ӂ��P�ɂ���B
	bool KVE( unsigned int& id_he_rem1 );
	/*!
	@brief id_he���Q�ɕ�����
	���͂ł�he2��he�Ɍ������������ӂƂ����
	�o�͂ł�he�̑O��he_add1,he�̌�������he_add2,he_add2�̌���he2
	*/
	bool MVE( unsigned int& id_he_add1, unsigned int& id_he_add2, unsigned int& id_uv_add, 
           const unsigned int id_he);
	/*!
	@brief he1�̋N�_uv1��he2�̋N�_uv2������ŁA�Q�̃��[�v���Ȃ���
	he1��[uv1-uv2]�Ahe2��[uv2-uv1]
	*/
	bool MEKL(unsigned int& id_he_add1, unsigned int& id_he_add2,  
            const unsigned int id_he1, const unsigned int id_he2 );
	//! ���[�v���Ȃ���
  bool KEML(unsigned int& id_ul_add1,
            const unsigned int& id_he1 );
	/*!
	@brief ���[�v�ƕ��V�_���Ȃ���,he1��Loop���Edge��he2�����V�_Edge
	he2��[uv2-uv1], he_add1��[uv1-uv2]��HalfEdge�ƂȂ�
	*/
	bool MEKL_OneFloatingVertex(unsigned int& id_he_add1,
                              const unsigned int id_he1, const unsigned int id_he2 );
	/*!
	@brief ���[�v�ƕ��V�_���Ȃ���,he1,he2�����V�_Edge
	he1��[uv1-uv2],he2��[uv2-uv1]��HalfEdge�ƂȂ�
	*/
	bool MEKL_TwoFloatingVertex( const unsigned int id_he1, const unsigned int id_he2 );
	/*! 
	@brief �Е����[�_�ł���Edge���폜����B
	he1�̋N�_uv1�͑��̕ӂɂȂ����Ă��Ȃ��[�_�ł���
	*/
	bool KEML_OneFloatingVertex(unsigned int& id_ul_add, 
                              const unsigned int id_he1);
	//! �������[�_�ł���Edge���폜����B
	bool KEML_TwoFloatingVertex(unsigned int& id_ul_add,
                              const unsigned int id_he1);
	//! ���V�_�����
	bool MVEL(unsigned int& id_uv_add, unsigned int& id_he_add, unsigned int& id_ul_add, 
            const unsigned int id_ul_p);
	//! ���V�_������
	bool KVEL(const unsigned int id_uv_rem);

	//! �ʑ����[�v�����ւ���
	bool SwapUseLoop(unsigned int id_ul1, unsigned int id_ul2);
	//! �ʑ����[�v�𓮂����Aid_ul1�̃��[�v��id_ul2�̐e�̎q�ɂ���B
	bool MoveUseLoop(unsigned int id_ul1, unsigned int id_ul2);
private:
	bool SwapUseLoop_CP_SameLoop(unsigned int id_ul1,unsigned int id_ul2);
	bool SwapUseLoop_CP_DifferentLoop(unsigned int id_ul1,unsigned int id_ul2);

//private:	// ���̂���private�ɂ���\��(�c���Serializ)
public:
  CObjSetCad<CUseLoop>   m_UseLoopSet;   //!< UseLoop��ID�ŊǗ����ꂽ�W��
  CObjSetCad<CHalfEdge>  m_HalfEdgeSet;  //!< HalfEdge��ID�ŊǗ����ꂽ�W��
  CObjSetCad<CUseVertex> m_UseVertexSet; //!< UseVertex��ID�ŊǗ����ꂽ�W��
};

//! @}

}

#endif

