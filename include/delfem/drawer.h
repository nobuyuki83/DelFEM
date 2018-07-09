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
@brief ���ە`��N���X(Com::View::CDrawer)�̃C���^�[�t�F�[�X
@remark ���̃t�@�C���̒��̃I�u�W�F�N�g�͊撣����OpenGL��ˑ��ɂ��Ă���
@author Nobuyuki Umetani
*/



#if !defined(DRAWER_H)
#define DRAWER_H

#if defined(__VISUALC__)
	#pragma warning( disable : 4786 )
#endif

#include <vector>
#include <assert.h>

namespace Com{
class CBoundingBox3D;

namespace View{

//! Abstract class of drawing something
class CDrawer
{
public:
	CDrawer(){ is_show = true; sutable_rot_mode=1; }
	virtual ~CDrawer(){}
	
	/*! 
	@brief �Z���N�V�����o�b�t�@�ւ̏����o��
	@param[in] idraw ���O�t���̍ŏ��ɕt�����鐔
	*/
	virtual void DrawSelection(unsigned int idraw) const = 0;
	//! draw in openGL frame buffer
	virtual void Draw() const = 0;
	/*!
	@brief Obtain 3D Bounding Box (Com::CBoundingBox)
	@param[in] rot : 3x3 rotation matrix
	*/
	virtual Com::CBoundingBox3D GetBoundingBox(double rot[]) const = 0;
	//! �I�����ꂽ�I�u�W�F�N�g��ǉ����āC�n�C���C�g������
	virtual void AddSelected(const int selec_flag[]) = 0;
	//! �I������������
	virtual void ClearSelected() = 0;

  virtual void SetAntiAliasing(bool is_aa){
    this->m_is_anti_aliasing = is_aa;
  }

	// return suitable rotation mode ( 0:2D, 1:2DH, 2:3D )
	virtual unsigned int GetSutableRotMode(){ return sutable_rot_mode; }
protected:
	bool is_show;
	unsigned int sutable_rot_mode;	// 1:2D  2:2DH  3:3D
  bool m_is_anti_aliasing;  // �A���`�G���A�V���O���邩�ǂ����D�f�t�H��false
};

class CCamera;

//! CDrawer�̔h���N���X�̃|�C���^��z��Ƃ��Ċi�[���邽�߂̃N���X
class CDrawerArray
{
public:
	void PushBack(CDrawer* pDrawer){
		assert( pDrawer != 0 );
		this->m_drawer_ary.push_back(pDrawer);
	}
	void Draw() const{
		for(unsigned int idraw=0;idraw<m_drawer_ary.size();idraw++){
			m_drawer_ary[idraw]->Draw();
		}
	}
	void DrawSelection() const{
		for(unsigned int idraw=0;idraw<m_drawer_ary.size();idraw++){
			m_drawer_ary[idraw]->DrawSelection(idraw);
		}	
	}
	void AddSelected(const int selec_flg[]){
		for(unsigned int idraw=0;idraw<m_drawer_ary.size();idraw++){
			m_drawer_ary[idraw]->AddSelected(selec_flg);
		}	
	}
	void ClearSelected(){
		for(unsigned int idraw=0;idraw<m_drawer_ary.size();idraw++){
			m_drawer_ary[idraw]->ClearSelected();
		}	
	}
	void Clear(){
		for(unsigned int idraw=0;idraw<m_drawer_ary.size();idraw++){ delete m_drawer_ary[idraw]; }
		m_drawer_ary.clear();
	}
	Com::CBoundingBox3D GetBoundingBox( double rot[] ) const;
	void InitTrans(Com::View::CCamera& mvp_trans);
public:
	std::vector<CDrawer*> m_drawer_ary;
};


/*! 
@brief OpenGL�ɒ��_�z��Ƃ��ēn���z��
@remark ���̃N���X��OpenGL��ˑ��Ȃ̂ŁC���[�U�[�����f�[�^(public�ϐ�orz)�Ŏ���Ă���OpenGL�ɓ�����d�l�i����܂�ǂ��Ȃ��j
*/
class CVertexArray
{
public:
	CVertexArray(const unsigned int np, const unsigned int nd)
		: npoin(np), ndim(nd)
	{
		pVertexArray = new double [npoin*ndim];
    pUVArray = 0;
	}
	CVertexArray() : pVertexArray(0), npoin(0), ndim(0), pUVArray(0){}
	virtual ~CVertexArray(){
		if( pVertexArray != 0 ){ delete[] pVertexArray; }
    if( pUVArray     != 0 ){ delete[] pUVArray;     }
	}
	void SetSize(unsigned int npoin, unsigned int ndim){
		if( this->npoin == npoin && ndim == ndim ) return;
		if( pVertexArray != 0 ) delete[] pVertexArray; 
		this->npoin = npoin;
		this->ndim = ndim;
		pVertexArray = new double [npoin*ndim];
    if(pUVArray != 0){
      delete[] pUVArray;
      pUVArray = new double [npoin*2];
    }
  }
  // rot == 0 : return object axis bounding box
  // rot != 0 : return rotated axis bounding box (rot is 3x3 matrix)
	Com::CBoundingBox3D GetBoundingBox( double rot[] ) const;
	inline unsigned int NDim() const { return ndim; }
	inline unsigned int NPoin() const { return npoin; }
  void EnableUVMap(bool is_uv_map){
    if( (pUVArray!=0) == is_uv_map ){ return; }
    if( is_uv_map ){
      const unsigned int nno = NPoin();
      pUVArray = new double [nno*2];
      for(unsigned int i=0;i<nno*2;i++){ pUVArray[i] = 0; }
    }
    else{
      delete pUVArray;
      pUVArray = 0;
    }
  }
public:
	double* pVertexArray;
  double* pUVArray;  
private:
	unsigned int npoin;
	unsigned int ndim;
};
	
}	// end namespace View
}	// end namespace Com


#endif
