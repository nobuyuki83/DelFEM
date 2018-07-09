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
@brief CAD���������邽�߂̃N���X(Cad::View::CDrawer_Cad2D)�̃C���^�[�t�F�[�X
@author Nobuyuki Umetani
*/

#if !defined(DRAWER_CAD_H)
#define DRAWER_CAD_H

#include <vector>

#include "delfem/cad_obj2d.h"
#include "delfem/drawer.h"
#include "delfem/vector3d.h"

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

namespace Msh{
	class CBarAry;
	class CTriAry2D;
}

namespace Cad{
namespace View{

/*! 
@brief Cad��OpenGL�ŉ�������N���X
@ingroup CAD
*/
class CDrawer_Cad2D : public Com::View::CDrawer
{
public : 
	/*!
   @brief �R���X�g���N�^
   @param[in] cad �b�`�c�̌`��
   @param[in] imode �\�����[�h{0:�ʂ�`�悷��,1:�ʂ�`�悵�Ȃ�(�Z���N�V�����ł͖ʂ�`��)}(�f�t�H���g��0)
   @remaks �ʂ�`�悵�Ȃ����[�h�ɐݒ肵�Ă��Z���N�V�����͂����̂ŁC�s�b�N�͉\
	*/
  CDrawer_Cad2D(const Cad::CCadObj2D& cad)
  {    
    tex_scale = 1;
    tex_cent_x=0, tex_cent_y=0;    
		this->m_is_anti_aliasing = false;
    m_linewidth = 3;
    m_pointsize = 5;
		// create check pattern with 2pt interval
		for(unsigned int j=0;j<4;j++){
			unsigned int i;
			for(i=0; i<8 ;i++) m_mask[j*32   +i] = 0x33;
			for(i=0; i<8 ;i++) m_mask[j*32+ 8+i] = 0xcc;
			for(i=0; i<8 ;i++) m_mask[j*32+16+i] = 0x33;
			for(i=0; i<8 ;i++) m_mask[j*32+24+i] = 0xcc;
		}	
    this->UpdateCAD_TopologyGeometry(cad);
	}  
  CDrawer_Cad2D(){
    tex_scale = 1;  
    tex_cent_x=0, tex_cent_y=0;        
		this->m_is_anti_aliasing = false;
    m_linewidth = 3;
    m_pointsize = 5;   
 		// create check pattern with 2pt interval
    for(unsigned int j=0;j<4;j++){
      unsigned int i;
      for(i=0; i<8 ;i++) m_mask[j*32   +i] = 0x33;
      for(i=0; i<8 ;i++) m_mask[j*32+ 8+i] = 0xcc;
      for(i=0; i<8 ;i++) m_mask[j*32+16+i] = 0x33;
      for(i=0; i<8 ;i++) m_mask[j*32+24+i] = 0xcc;
    }
  }
	virtual ~CDrawer_Cad2D(){
		for(unsigned int i=0;i<m_apIndexAry.size();i++){ delete m_apIndexAry[i]; }
	}
	// virtual�֐�

  //! �􉽌`����X�V����(�g�|���W�[�̕ω��͑��݂��Ȃ��Ƃ���)
  void UpdateCAD_Geometry(const Cad::CCadObj2D&);
  //! �g�|���W�[�Ɗ􉽂��X�V����
  bool UpdateCAD_TopologyGeometry(const Cad::CCadObj2D&);

	//! @{
	//! �`��
	virtual void Draw() const;
	//! �s�b�L���O�̂��߂̕`�������
	virtual void DrawSelection(unsigned int idraw) const;
  //! ���̑�����ݒ�
  void SetLineWidth(unsigned int linewidth){ m_linewidth = linewidth; }
  //! �_�̑傫����ݒ�
  void SetPointSize(unsigned int pointsize){ m_pointsize = pointsize; }
  void SetTextureScale(double tex_scale);
  void SetTexCenter(double cent_x, double cent_y){
    this->tex_cent_x = cent_x;
    this->tex_cent_y = cent_y;    
  }  
  void GetTexCenter(double& cent_x, double& cent_y){
    cent_x = this->tex_cent_x;
    cent_y = this->tex_cent_y;
  }    
	//! �o�E���f�B���O�E�{�b�N�X�𓾂�
	virtual Com::CBoundingBox3D GetBoundingBox( double rot[] ) const {
		return m_vertex_ary.GetBoundingBox(	rot );
	}
	//! Hilight�\������v�f�ɉ�����(selection_flag����)
	virtual void AddSelected(const int selec_flag[]);
	//! Hilight�\������v�f�ɉ�����
	virtual void AddSelected(Cad::CAD_ELEM_TYPE itype, unsigned int id);
	//! Hilight�\������߂�
	virtual void ClearSelected();
    //! @}

	void GetCadPartID(const int selec_flag[], Cad::CAD_ELEM_TYPE& part_type, unsigned int& part_id);
  void HideEffected(const Cad::CCadObj2D& cad_2d, Cad::CAD_ELEM_TYPE part_type, unsigned int part_id);
  void ShowEffected(const Cad::CCadObj2D& cad_2d, Cad::CAD_ELEM_TYPE part_type, unsigned int part_id);
  void SetIsShow(bool is_show, Cad::CAD_ELEM_TYPE part_type, unsigned int part_id);
  void SetIsShow(bool is_show, Cad::CAD_ELEM_TYPE part_type, const std::vector<unsigned int>& aIdPart );
  void SetRigidDisp(unsigned int id_l, double xdisp, double ydisp);
//    void Show(Cad::CAD_ELEM_TYPE part_type, unsigned int part_id);
  void EnableUVMap(bool is_uv_map);
private:	
	////////////////
	class CDrawPart{
	public : 
		CDrawPart(){
			nelem = 0; npoel = 0; pIndexArray = 0; 
			is_selected = false; itype = Cad::VERTEX; is_show=true;
      color[0]=0; color[1]=0; color[2]=0;
			height = 0;
			xdisp=0; ydisp=0;
		}
		virtual ~CDrawPart(){
			this->Clear();
		}
		void Clear(){
			delete[] pIndexArray;
			pIndexArray = 0;
			nelem = 0;
			npoel = 0;
		}
		void DrawElements() const;
		bool Set(const Msh::CBarAry& BarAry);
		bool Set(const Msh::CTriAry2D& TriAry);
		void SetHeight(double h){ height = h; }
	public:
		bool is_show;
		bool is_selected;
    float color[3];
    ////////////////
		Cad::CAD_ELEM_TYPE itype;
		unsigned int id_cad;
    unsigned int id_msh;
    ////////////////
		unsigned int nelem;
		unsigned int npoel;
    unsigned int* pIndexArray;
		////////////////
		double height;
		double xdisp, ydisp;
	};
	////////////////
	class CDrawPart_CadVertex{
	public:
		unsigned int id_v;
		unsigned int id_cad;
		unsigned int id_msh;
        ////////////////
		bool is_selected;
		bool is_show;
		////////////////
		double height;
	};	
private:
  unsigned char m_mask[128];	//!< �I��̈���h�b�g�Ō�����̂��߂̃}�X�N
  std::vector<CDrawPart*> m_apIndexAry;				//! �ӂ�ʂȂ�
  std::vector<CDrawPart_CadVertex> m_aIndexVertex;	//! ���_
  Com::View::CVertexArray m_vertex_ary;	//! vertex array
  unsigned int m_linewidth;   //!< the width of the line
  unsigned int m_pointsize;   //!< the size of vertices
  double tex_cent_x, tex_cent_y;
  double tex_scale;
};


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

/*!
@brief ���o�[�o���h�N���X
@remark ������CVector3D����Ȃ��āCCVector2D���g���ׂ��D
*/
class CDrawerRubberBand : public Com::View::CDrawer
{
public:
	CDrawerRubberBand(const Com::CVector3D& initial_position){
		this->initial = initial_position;
		imode = 0;
	}
	CDrawerRubberBand(const Cad::CCadObj2D& cad, unsigned int id_v_cad);
	CDrawerRubberBand(const Cad::CCadObj2D& cad, unsigned int id_v, const Com::CVector3D& initial);
	virtual void Draw() const;
	virtual void DrawSelection(unsigned int idraw) const{}
	virtual Com::CBoundingBox3D GetBoundingBox( double rot[] ) const;
	virtual void AddSelected(const int selec_flag[]){}
	virtual void ClearSelected(){}
	virtual unsigned int WhatKindOfYou() const { return 0; }

	void SetMousePosition(const Com::CVector3D& mouse){
		this->mouse = mouse;
	}
private:
	// 0 : connect inital and mouse;
	// 1 : connect fix[] and mouse
	// 2 : connect fix_stat[].second and fix[ fix_stat[].first ]
	int imode;
	Com::CVector3D mouse;
	Com::CVector3D initial;
	std::vector<Com::CVector3D> fix;
	std::vector< std::pair<unsigned int,Com::CVector3D> > fix_stat;
};


} // end namespace View
} // end namespace Cad

#endif
