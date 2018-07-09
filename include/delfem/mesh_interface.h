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
@brief ���ۃ��b�V���N���X(Msh::CMesh_Interface)�̃C���^�[�t�F�[�X
@author Nobuyuki Umetani
*/

#if !defined(MESH_INTERFACE_H)
#define MESH_INTERFACE_H

#if defined(__VISUALC__)
	#pragma warning( disable : 4786 )
#endif

#include <vector>
#include <map>

////////////////////////////////////////////////

namespace Msh{

/*!
@addtogroup Msh
*/
//@{
enum MSH_TYPE{
    VERTEX,	//!< ���_�v�f
    BAR,	//!< �����v�f
    TRI,	//!< �O�p�`�v�f
    QUAD,	//!< �S�p�`�v�f
    TET,	//!< �l�ʑ̗v�f
    HEX		//!< �Z�ʑ̗v�f
};

////////////////////////////////////////////////

/*!
@brief ���b�V���C���^�[�t�F�[�X�N���X
@ingroup Msh
*/
class  IMesh
{
public:
	//! ���W�̎����𓾂�
	virtual unsigned int GetDimention() const = 0;
	virtual void GetInfo(unsigned int id_msh,
        unsigned int& id_cad_part, unsigned int& id_msh_before_ext, unsigned int& inum_ext,
		int& ilayer) const = 0;
	//! ���W�̔z��𓾂�
	virtual void GetCoord(std::vector<double>& coord) const = 0;
	//! �R�l�N�e�B�r�e�B�̔z��𓾂�
	virtual MSH_TYPE GetConnectivity(unsigned int id_msh, std::vector<int>& lnods) const = 0;
	//! �v�f�z��ID�̔z�񓾂�
	virtual std::vector<unsigned int> GetAry_ID() const = 0;
	//! ��܊֌W�𓾂�
	virtual std::vector<unsigned int> GetIncludeElemIDAry(unsigned int id_msh) const = 0;
};

//! �Q�������b�V�����R�����Ɏˉe�������b�V���̃N���X
class CMeshProjector2Dto3D : public IMesh
{
public:
    CMeshProjector2Dto3D(const IMesh& msh_input) : msh_2d(msh_input){
        if( msh_input.GetDimention() != 2 ){ is_valid = false; }
        else{ is_valid = true; }
    }
public:
    virtual unsigned int GetDimention() const {
        if( !is_valid ){ return 0; }
        return 3;
    }
	virtual void GetInfo(unsigned int id_msh,
        unsigned int& id_cad, unsigned int& id_msh_before_ext, unsigned int& inum_ext,
		int& ilayer ) const {
        if( !is_valid ){ id_cad=0; id_msh_before_ext=0; inum_ext=0; ilayer=0; return; }
        msh_2d.GetInfo(id_msh,id_cad,id_msh_before_ext,inum_ext,ilayer);
    }
    virtual void GetCoord(std::vector<double>& coord) const {
        if( !is_valid ){ coord.clear(); return; }
        std::vector<double> co2;
        msh_2d.GetCoord(co2);
        assert( msh_2d.GetDimention() == 2 );
        assert( co2.size() % 2 == 0 );
        const unsigned int nno = co2.size() / 2;
        coord.resize( nno*3 );
        for(unsigned int ino=0;ino<nno;ino++){
            coord[ino*3  ] = co2[ino*2  ];
            coord[ino*3+1] = co2[ino*2+1];
            coord[ino*3+2] = 0;
        }
    }
    virtual MSH_TYPE GetConnectivity(unsigned int id_msh, std::vector<int>& lnods) const{
        if( !is_valid ){ lnods.clear(); return (MSH_TYPE)0;}
        return msh_2d.GetConnectivity(id_msh,lnods);
    }
    virtual std::vector<unsigned int> GetAry_ID() const{
        if( !is_valid ){
            std::vector<unsigned int> hoge;
            return hoge;
        }
        return msh_2d.GetAry_ID();
    }
    virtual std::vector<unsigned int> GetIncludeElemIDAry(unsigned int id_msh) const{
        if( !is_valid ){
            std::vector<unsigned int> hoge;
            return hoge;
        }
        return msh_2d.GetIncludeElemIDAry(id_msh);
    }
private:
    bool is_valid;
    const IMesh& msh_2d;
};

// @}
};

#endif
