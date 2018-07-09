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
@brief interface of element array class (Fem::Field::CElemAry)
@author Nobuyuki Umetani
*/

#if defined(__VISUALC__)
    #pragma warning( disable : 4786 )
#endif

#if !defined(ELEM_ARY_H)
#define ELEM_ARY_H

#include <vector>
#include <assert.h>
#include <cstdlib> //(abs)
#include <cstring> //(strspn, strlen, strncmp, strtok)

#include "delfem/objset.h"
#include "delfem/indexed_array.h"

namespace Fem{
namespace Field{

//! type of element segment
enum ELSEG_TYPE{ 
	CORNER=1,	//!< corner node
	EDGE=2,		//!< edge node
	BUBBLE=4	//!< in the element node
};

// this integet index will be used in "elem_ary.cpp". Please don't change without consideration.
// ���������͕ʂ̃w�b�_�t�@�C���Ɉڂ����ق������������D����Index�����~�����N���X(drawer_field.h�Ƃ�)�̂��߂�
//! type of element
enum ELEM_TYPE{	
	ELEM_TYPE_NOT_SET=0,
	POINT=1,	//!< point element
	LINE=2,		//!< line element
	TRI=3, 		//!< triangle element
	QUAD=4, 	//!< quadratic element
	TET=5, 		//!< tetrahedra element
	HEX=6 		//!< hexagonal element
};

/*! 
@brief class of Element Array
@ingroup Fem
*/
class CElemAry
{
	
public:
	//! �v�f�Z�O�����g�D�v�f�̏ꏊ(Corner,Bubble..etc)���Ƃ̐ߓ_�ԍ����擾����C���^�[�t�F�[�X�֐�
	class CElemSeg{
		friend class CElemAry;
	public:
		CElemSeg(unsigned int id_na, ELSEG_TYPE elseg_type)
			: m_id_na(id_na), m_elseg_type(elseg_type){}

		unsigned int GetMaxNoes() const { return max_noes; }	//!< �m�[�h�ԍ��̈�ԑ傫�Ȃ��̂𓾂�i����noes���i�[���邽�߂ɂ͈�傫�Ȕz�񂪕K�v�Ȃ̂Œ��Ӂj
		unsigned int Length() const { return m_nnoes; }	//!< return the node size per elem seg  ( will be renamed to Length() );
		unsigned int Size() const { return nelem; }	  //!< get the number of elements ( will be renamed to Size() )
		unsigned int GetIdNA() const { return m_id_na; }	    //!< get ID of node array this element segment refers
		ELSEG_TYPE GetElSegType() const { return m_elseg_type; }	//!< �v�f�Z�O�����g�^�C�v(Fem::Field::CORNER,Fem::Field::BUBBLE,Fem::Field::EDGE)�𓾂�

		//! get node indexes of element (ielem)
		void GetNodes(const unsigned int& ielem, unsigned int* noes ) const {
			for(unsigned int inoes=0;inoes<m_nnoes;inoes++){ 
				assert( ielem*npoel+begin+inoes < nelem*npoel );
				noes[inoes] = abs(pLnods[ielem*npoel+begin+inoes]); 
			}
		}
		//! �ߓ_�ԍ���ݒ�
		void SetNodes(unsigned int ielem, unsigned int idofes, int ino ){
			pLnods[ielem*npoel+begin+idofes] = ino;
		}
		//! �e�ꏊ(Corner,Bubble)�ɒ�`����Ă���v�f�ߓ_�̐����o��
		static unsigned GetLength(ELSEG_TYPE elseg_type, ELEM_TYPE elem_type)
		{	// �X�^�e�B�b�N�Ȋ֐��Ȃ̂Ŏ��͎̂����Ȃ��ėǂ�
			if( elem_type == POINT ){
				if( elseg_type == CORNER ){ return 1; }
				if( elseg_type == EDGE   ){ return 2; }
				if( elseg_type == BUBBLE ){ return 2; }	// �_�Ɠ_������Lagrange����搔���`����Ƃ��Ɏg������O����_��
				assert(0);
			}
			else if( elem_type == LINE ){
				if( elseg_type == CORNER ){ return 2; }
				if( elseg_type == EDGE   ){ return 1; }
				if( elseg_type == BUBBLE ){ return 0; }
				assert(0);
			}
			else if( elem_type == TRI ){
				if( elseg_type == CORNER ){ return 3; }
				if( elseg_type == EDGE   ){ return 3; }
				if( elseg_type == BUBBLE ){ return 1; }
				assert(0);
			}
			else if( elem_type == QUAD ){
				if( elseg_type == CORNER ){ return 4; }
				if( elseg_type == BUBBLE ){ return 1; }
				if( elseg_type == EDGE   ){ return 4; }
				assert(0);
			}
			else if( elem_type == TET ){
				if( elseg_type == CORNER ){ return 4; }
				if( elseg_type == BUBBLE ){ return 1; }
				if( elseg_type == EDGE   ){ return 6; }
				assert(0);
			}
			else if( elem_type == HEX ){
				if( elseg_type == CORNER ){ return 8; }
				if( elseg_type == BUBBLE ){ return 1; }
				if( elseg_type == EDGE   ){ return 12; }
				assert(0);
			}
			else{ assert(0); }
			return 0;
		}
		//! �v�f�̎������擾����
    static unsigned GetElemDim(ELEM_TYPE elem_type){
      if(      elem_type == POINT ){ return 0; }
      else if( elem_type == LINE  ){ return 1; }
      else if( elem_type == TRI   ){ return 2; }
      else if( elem_type == QUAD  ){ return 2; }
      else if( elem_type == TET   ){ return 3; }
      else if( elem_type == HEX   ){ return 3; }
			else{ assert(0); }      
			return 0;
    }
	private: // variable given at construction
		unsigned int m_id_na; // <- �����A�Ȃ��Ƒ�ςȂ��ƂɂȂ����B
		enum ELSEG_TYPE m_elseg_type;
		// int id_es_corner <- ���̂����ǉ��\��
	private: // 
		unsigned int max_noes;	// ����ElementSegment�ɑ�����ߓ_�ԍ��̍ő�̂���
		unsigned int begin;		// m_aLnods�̂ǂ�����n�܂邩 (<npoel)
		unsigned int m_nnoes;		// ElementSegment�̒���
	private: // variable given by CElemAry (CELemAry is friend class)
		mutable unsigned int* pLnods;
		mutable unsigned int npoel;
		mutable unsigned int nelem;
	};
public:
	//! default constructor
	CElemAry(){
		m_nElem = 0; npoel = 0; m_pLnods = 0;
	}
  CElemAry(const CElemAry& ea);
  
  // Constructor with elem number (nelem) and elem type (elemtype)
	CElemAry(unsigned int nelem, ELEM_TYPE elem_type) : m_nElem(nelem), m_ElemType(elem_type){
		npoel = 0; m_pLnods = 0;
	}
	//! destructor
	virtual ~CElemAry(){
		if( this->m_pLnods != 0 ) delete[] m_pLnods;
	}

	bool IsSegID( unsigned int id_es ) const { return m_aSeg.IsObjID(id_es); }	//!< Check if there are elem segment with ID:id_es
	const std::vector<unsigned int>& GetAry_SegID() const { return this->m_aSeg.GetAry_ObjID(); } //!< �v�f�Z�O�����gID�̔z��𓾂�֐�
	unsigned int GetFreeSegID() const{ return m_aSeg.GetFreeObjID(); }	//!< �g���Ă��Ȃ��v�f�Z�O�����gID���擾����֐�
  
	const CElemSeg& GetSeg(unsigned int id_es) const{			
		assert( this->m_aSeg.IsObjID(id_es) );
		if( !m_aSeg.IsObjID(id_es) ) throw;
		const CElemSeg& es = m_aSeg.GetObj(id_es);
		es.pLnods = this->m_pLnods;
		es.npoel = this->npoel;
		es.nelem = this->m_nElem;
		return es;
	}
	CElemSeg& GetSeg(unsigned int id_es){
		assert( this->m_aSeg.IsObjID(id_es) );
		if( !m_aSeg.IsObjID(id_es) ) throw;
		CElemSeg& es = m_aSeg.GetObj(id_es);
		es.pLnods = this->m_pLnods;
		es.npoel = this->npoel;
		es.nelem = this->m_nElem;
		return es;
	}

	virtual ELEM_TYPE ElemType() const { return m_ElemType; }	//!< get type of element
	virtual unsigned int Size() const { return m_nElem; }	//!< get number of elements

	//! make CRS data (for the off-diaglnal block)
	virtual bool MakePattern_FEM(	
		const unsigned int& id_es0, const unsigned int& id_es1, 
        Com::CIndexedArray& crs ) const;

	//! make CRS data (for the diaglnal)
	virtual bool MakePattern_FEM(	
		const unsigned int& id_es0, 
        Com::CIndexedArray& crs ) const;

	// make edge ( for 2nd order interporation )
	virtual bool MakeEdge(const unsigned int& id_es_co, unsigned int& nedge, std::vector<unsigned int>& edge_ary) const;
	
	// make map from elem to edge
	virtual bool MakeElemToEdge(const unsigned int& id_es_corner, 
		const unsigned int& nedge, const std::vector<unsigned int>& edge_ary,
		std::vector<int>& el2ed ) const;
	/*!
	@brief ���E�v�f�����(�����̂��߂̊֐�)
	*/
	virtual CElemAry* MakeBoundElemAry(unsigned int id_es_corner, unsigned int& id_es_add, std::vector<unsigned int>& aIndElemFace) const;
	
	//! IO functions
	int InitializeFromFile(const std::string& file_name, long& offset);
	int WriteToFile(       const std::string& file_name, long& offset, unsigned int id) const;

	// lnods should be unsigned int?
	//! Add element segment
	std::vector<int> AddSegment(const std::vector< std::pair<unsigned int,CElemSeg> >& es_ary, const std::vector<int>& lnods );
	int              AddSegment(unsigned int id, const CElemSeg& es,					 const std::vector<int>& lnods );

	// �v�f���͂ޗv�f�����D������elsuel�̃������̈�m�ۂ͂��Ȃ��̂ŁC�ŏ�����m�ۂ��Ă���
	bool MakeElemSurElem( const unsigned int& id_es_corner, int* elsuel) const;
private:

	bool MakePointSurElem( const unsigned int id_es, Com::CIndexedArray& elsup ) const;

	bool MakePointSurPoint
  (const unsigned int id_es, const Com::CIndexedArray& elsup, bool isnt_self,
   Com::CIndexedArray& psup ) const;

  bool MakeElemSurElem( const unsigned int& id_es_corner, const Com::CIndexedArray& elsup, int* elsuel) const;
private:


protected:
  unsigned int m_nElem;	//!< number of elements
  ELEM_TYPE m_ElemType;	//!< type of element
	unsigned int npoel;		//!< the total number of nodes including in one elemement
	unsigned int * m_pLnods;			//!< connectivity
	Com::CObjSet<CElemSeg> m_aSeg;	//!< the set of elem segment
};

}
}

#endif
