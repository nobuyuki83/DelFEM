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

////////////////////////////////////////////////////////////////
// eqn_poisson.cpp : �|�A�\���������̗v�f�����쐬���̎���
////////////////////////////////////////////////////////////////

#if defined(__VISUALC__)
	#pragma warning( disable : 4786 )
#endif

#include <math.h>

#include "delfem/field_world.h"

#include "delfem/matvec/matdia_blkcrs.h"
#include "delfem/matvec/vector_blk.h"

#include "delfem/femls/linearsystem_field.h"
#include "delfem/femls/linearsystem_fieldsave.h"

#include "delfem/femeqn/ker_emat_tri.h"
#include "delfem/femeqn/ker_emat_tet.h"
#include "delfem/femeqn/ker_emat_quad.h"
#include "delfem/femeqn/ker_emat_hex.h"
#include "delfem/femeqn/eqn_poisson.h"

using namespace Fem::Eqn;
using namespace Fem::Field;
using namespace Fem::Ls;
using namespace MatVec;

static bool AddLinearSystem_Poisson2D_P1(
		double alpha, double source, 
        Fem::Ls::CLinearSystem_Field& ls, 
		const unsigned int id_field_val, const CFieldWorld& world,
		const unsigned int id_ea )
{
//	std::cout << "Poisson2D Triangle 3-point 1st order" << std::endl;

	assert( world.IsIdEA(id_ea) );
	const CElemAry& ea = world.GetEA(id_ea);
	assert( ea.ElemType() == TRI );

	if( !world.IsIdField(id_field_val) ) return false;
	const CField& field_val = world.GetField(id_field_val);

	const CElemAry::CElemSeg& es_c_va = field_val.GetElemSeg(id_ea,CORNER,true, world);
	const CElemAry::CElemSeg& es_c_co = field_val.GetElemSeg(id_ea,CORNER,false,world);

	const unsigned int nno = 3;
	const unsigned int ndim = 2;

	unsigned int no_c[nno];	// �v�f�ߓ_�̑S�̐ߓ_�ԍ�

	double value_c[nno];		// �v�f�ߓ_�̒l
	double coord_c[nno][ndim];	// �v�f�ߓ_�̍��W
				
	double emat[nno][nno];	// �v�f�����s��
	double eres_c[nno];	// �v�f�ߓ_�������́A�O�́A�c���x�N�g��
				
	CMatDia_BlkCrs& mat_cc = ls.GetMatrix(  id_field_val,CORNER,world);
	CVector_Blk&    res_c  = ls.GetResidual(id_field_val,CORNER,world);

	const CNodeAry::CNodeSeg& ns_c_val = field_val.GetNodeSeg(CORNER,true,world);
	const CNodeAry::CNodeSeg& ns_c_co  = field_val.GetNodeSeg(CORNER,false,world);

	for(unsigned int ielem=0;ielem<ea.Size();ielem++){
		// �v�f�z�񂩂�v�f�Z�O�����g�̐ߓ_�ԍ������o��
		es_c_co.GetNodes(ielem,no_c);
		for(unsigned int inoes=0;inoes<nno;inoes++){
			ns_c_co.GetValue(no_c[inoes],coord_c[inoes]);
		}
		// �ߓ_�̒l������ė���
		es_c_va.GetNodes(ielem,no_c);
		for(unsigned int inoes=0;inoes<nno;inoes++){
			ns_c_val.GetValue(no_c[inoes],&value_c[inoes]);
		}
		// �ʐς����߂�
		const double area = TriArea(coord_c[0],coord_c[1],coord_c[2]);
		// �`��֐��̔��������߂�
		double dldx[nno][ndim];	// �`��֐���xy����
		double const_term[nno];	// �`��֐��̒萔��
		TriDlDx(dldx,const_term,coord_c[0],coord_c[1],coord_c[2]);
		// �v�f�����s������
		for(unsigned int ino=0;ino<nno;ino++){
		for(unsigned int jno=0;jno<nno;jno++){
			emat[ino][jno] = alpha*area*(dldx[ino][0]*dldx[jno][0]+dldx[ino][1]*dldx[jno][1]);
		}
		}
		// �v�f�ߓ_�����O�̓x�N�g�������߂�
		for(unsigned int ino=0;ino<nno;ino++){
			eres_c[ino] = source*area*0.33333333333333333;
		}
		// �v�f�ߓ_�������̓x�N�g�������߂�
		for(unsigned int ino=0;ino<nno;ino++){
			for(unsigned int jno=0;jno<nno;jno++){
				eres_c[ino] -= emat[ino][jno]*value_c[jno];
			}	
		}
		// �v�f�����s��Ƀ}�[�W����
		mat_cc.Mearge(nno,no_c,nno,no_c,1,&emat[0][0]);
		// �c���x�N�g���Ƀ}�[�W����
		for(unsigned int inoes=0;inoes<nno;inoes++){
			res_c.AddValue( no_c[inoes],0,eres_c[inoes]);
		}
	}
	return true;
}

static bool AddLinearSystem_Poisson2D_P1b(
		double alpha, double source, 
        Fem::Ls::CLinearSystem_Field& ls, 
		const unsigned int id_field_val, const CFieldWorld& world,
		const unsigned int id_ea)
{
//	std::cout << "Poisson2D TriP1b" << std::endl;

	assert( world.IsIdEA(id_ea) );
	const CElemAry& ea = world.GetEA(id_ea);
	assert( ea.ElemType() == TRI );

	if( !world.IsIdField(id_field_val) ) return false;
	const CField& field_val = world.GetField(id_field_val);

	const CElemAry::CElemSeg& es_c = field_val.GetElemSeg(id_ea,CORNER,true,world);
	const CElemAry::CElemSeg& es_b = field_val.GetElemSeg(id_ea,BUBBLE,true,world);

	const unsigned int nno_c = 3;
	const unsigned int nno_b = 1;
	const unsigned int ndim = 2;

	unsigned int no_c[nno_c];	// �v�f�ߓ_�̑S�̐ߓ_�ԍ�
	unsigned int no_b;	// �v�f�ߓ_�̑S�̐ߓ_�ԍ�

	double value_c[nno_c], value_b;	// �v�f�ߓ_�̒l
	double coord_c[nno_c][ndim];	// �v�f�ߓ_�̍��W
				
	double dldx[nno_c][ndim];	// �`��֐���xy����
	double const_term[nno_c];	// �`��֐��̒萔��

	double emat_cc[nno_c][nno_c], emat_bb, emat_cb[nno_c], emat_bc[nno_c];	// �v�f�����s��
	double eqf_in_c[nno_c], eqf_out_c[nno_c], eres_c[nno_c];	// �v�f�ߓ_�������́A�O�́A�c���x�N�g��
	double eqf_in_b, eqf_out_b, eres_b;	// �v�f�ߓ_�������́A�O�́A�c���x�N�g��

	CMatDia_BlkCrs& mat_cc = ls.GetMatrix(id_field_val,CORNER,world);
	CMatDia_BlkCrs& mat_bb = ls.GetMatrix(id_field_val,BUBBLE,world);
	CMat_BlkCrs& mat_cb = ls.GetMatrix(id_field_val,CORNER, id_field_val,BUBBLE, world);
	CMat_BlkCrs& mat_bc = ls.GetMatrix(id_field_val,BUBBLE, id_field_val,CORNER, world);
	CVector_Blk& res_c = ls.GetResidual(id_field_val,CORNER,world);
	CVector_Blk& res_b = ls.GetResidual(id_field_val,BUBBLE,world);

	const CNodeAry::CNodeSeg& ns_c_val = field_val.GetNodeSeg(CORNER,true, world);
	const CNodeAry::CNodeSeg& ns_b_val = field_val.GetNodeSeg(BUBBLE,true, world);
	const CNodeAry::CNodeSeg& ns_c_co  = field_val.GetNodeSeg(CORNER,false,world);

	for(unsigned int ielem=0;ielem<ea.Size();ielem++)
	{
		// �v�f�z�񂩂�v�f�Z�O�����g�̐ߓ_�ԍ������o��
		es_c.GetNodes(ielem,no_c);
		es_b.GetNodes(ielem,&no_b);
		// �ߓ_�̒l������Ă���
		for(unsigned int inoes=0;inoes<nno_c;inoes++){
			ns_c_co.GetValue(no_c[inoes],coord_c[inoes]);
			ns_c_val.GetValue(no_c[inoes],&value_c[inoes]);
		}
		ns_b_val.GetValue(no_b,&value_b);

		// �ʐς����߂�
		const double area = TriArea(coord_c[0],coord_c[1],coord_c[2]);
		// �`��֐��̔��������߂�
		TriDlDx(dldx,const_term,coord_c[0],coord_c[1],coord_c[2]);
		{	// �v�f�����s������
			double vc_b[4];
			vc_b[0] = 1.0/3.0; vc_b[1] = 1.0/3.0; vc_b[2] = 1.0/3.0; vc_b[3] = 27.0;
			const double tmp_val1 = vc_b[3]*vc_b[3]*area/180.0*( 
				dldx[0][0]*dldx[0][0]+dldx[0][1]*dldx[0][1]+
				dldx[1][0]*dldx[1][0]+dldx[1][1]*dldx[1][1]+
				dldx[2][0]*dldx[2][0]+dldx[2][1]*dldx[2][1] );
			double tmp1;
            for(unsigned int ino_c=0;ino_c<nno_c;ino_c++){
            for(unsigned int jno_c=0;jno_c<nno_c;jno_c++){
				tmp1 = 
					area*(dldx[ino_c][0]*dldx[jno_c][0]+dldx[ino_c][1]*dldx[jno_c][1])
					+vc_b[ino_c]*vc_b[jno_c]*tmp_val1;
				emat_cc[ino_c][jno_c] = tmp1;
			}
			}
			for(unsigned int ino_c=0;ino_c<nno_c;ino_c++){
				tmp1 = -1.0*vc_b[ino_c]*tmp_val1;
				emat_cb[ino_c] = tmp1;
				emat_bc[ino_c] = tmp1;
			}
			emat_bb = tmp_val1;
		}
		// �v�f�ߓ_�����O�̓x�N�g�������߂�
        for(unsigned int ino_c=0;ino_c<nno_c;ino_c++){
			eqf_out_c[ino_c] = source*area*11.0/60.0;
		}
		eqf_out_b = source*area*27.0/60.0;
		// �v�f�ߓ_�������̓x�N�g�������߂�
		for(unsigned int ino_c=0;ino_c<nno_c;ino_c++){
			eqf_in_c[ino_c] = 0.0;
			for(unsigned int jno_c=0;jno_c<nno_c;jno_c++){
				eqf_in_c[ino_c] += emat_cc[ino_c][jno_c]*value_c[jno_c];
			}
			eqf_in_c[ino_c] += emat_cb[ino_c]*value_b;
			}
		eqf_in_b = 0.0;
		for(unsigned int jno_c=0;jno_c<nno_c;jno_c++){
			eqf_in_b += emat_bc[jno_c]*value_c[jno_c];
		}
		eqf_in_b += emat_bb*value_b;
		// �v�f�ߓ_�����c���x�N�g�������߂�
        for(unsigned int ino_c=0;ino_c<nno_c;ino_c++){
			eres_c[ino_c] = eqf_out_c[ino_c] - eqf_in_c[ino_c];
		}
		eres_b = eqf_out_b - eqf_in_b;
		// �v�f�����s��̑S�̍����s��ւ̃}�[�W
		mat_cc.Mearge(nno_c,no_c,nno_c,no_c,	1,&emat_cc[0][0]);
		mat_cb.Mearge(nno_c,no_c,nno_b,&no_b,	1,&emat_cb[0]   );
		mat_bc.Mearge(nno_b,&no_b,nno_c,no_c,	1,&emat_bc[0]   );
		mat_bb.Mearge(nno_b,&no_b,nno_b,&no_b,	1,&emat_bb      );
		// �c���x�N�g���̃}�[�W
		for(unsigned int inoes=0;inoes<nno_c;inoes++){
			res_c.AddValue( no_c[inoes],0,eres_c[inoes]);
		}
		res_b.AddValue( no_b,0,eres_b );
	}
	return true;
}

static bool AddLinearSystem_Poisson3D_P1(
		double alpha, double source, 
        Fem::Ls::CLinearSystem_Field& ls, 
		unsigned int id_field_val, const CFieldWorld& world,
		unsigned int id_ea)
{
//	std::cout << "Poisson3D Tet 4-point 1st order" << std::endl;

	assert( world.IsIdEA(id_ea) );
	const CElemAry& ea = world.GetEA(id_ea);
	assert( ea.ElemType() == TET );

	if( !world.IsIdField(id_field_val) ) return false;
	const CField& field_val = world.GetField(id_field_val);

	const CElemAry::CElemSeg& es_c = field_val.GetElemSeg(id_ea,CORNER,false,world);

	const unsigned int nno = 4;
	const unsigned int ndim = 3;

	unsigned int no_c[nno];	// �v�f�ߓ_�̑S�̐ߓ_�ԍ�

	double value_c[nno];		// �v�f�ߓ_�̒l
	double coord_c[nno][ndim];	// �v�f�ߓ_�̍��W
			
	double emat[nno][nno];	// �v�f�����s��
	double eres_c[nno];	// �v�f�ߓ_�������́A�O�́A�c���x�N�g��
				
	CMatDia_BlkCrs& mat_cc = ls.GetMatrix(  id_field_val,CORNER,world);
	CVector_Blk&    res_c  = ls.GetResidual(id_field_val,CORNER,world);

	const CNodeAry::CNodeSeg& ns_c_val = field_val.GetNodeSeg(CORNER,true,world);
	const CNodeAry::CNodeSeg& ns_c_co  = field_val.GetNodeSeg(CORNER,false,world);

	for(unsigned int ielem=0;ielem<ea.Size();ielem++)
	{
		// �v�f�z�񂩂�v�f�Z�O�����g�̐ߓ_�ԍ������o��
		es_c.GetNodes(ielem,no_c);
		// �ߓ_�̒l������ė���
		for(unsigned int inoes=0;inoes<nno;inoes++){
			ns_c_co.GetValue(no_c[inoes],coord_c[inoes]);
			ns_c_val.GetValue(no_c[inoes],&value_c[inoes]);
		}

		// �ʐς����߂�
		const double vol = TetVolume(coord_c[0],coord_c[1],coord_c[2], coord_c[3]);
		// �`��֐��̔��������߂�	
		double dldx[nno][ndim];	// �`��֐���xy����
		double const_term[nno];	// �`��֐��̒萔��
		TetDlDx(dldx,const_term,coord_c[0],coord_c[1],coord_c[2], coord_c[3]);
		// �v�f�����s������
		for(unsigned int ino=0;ino<nno;ino++){
		for(unsigned int jno=0;jno<nno;jno++){
			emat[ino][jno] = alpha*vol*(dldx[ino][0]*dldx[jno][0]+dldx[ino][1]*dldx[jno][1]+dldx[ino][2]*dldx[jno][2]);
		}
		}
		// �v�f�ߓ_�����O�̓x�N�g�������߂�
		for(unsigned int ino=0;ino<nno;ino++){
			eres_c[ino] = source*vol*0.25;
		}
		// �v�f�ߓ_�������̓x�N�g�������߂�
		for(unsigned int ino=0;ino<nno;ino++){
		for(unsigned int jno=0;jno<nno;jno++){
			eres_c[ino] -= emat[ino][jno]*value_c[jno];
		}	
		}
		// �v�f�����s��Ƀ}�[�W����
		mat_cc.Mearge(nno,no_c,nno,no_c,1,&emat[0][0]);
		// �c���x�N�g���Ƀ}�[�W����
		for(unsigned int inoes=0;inoes<nno;inoes++){
			res_c.AddValue( no_c[inoes],0,eres_c[inoes]);
		}
	}
	return true;
}

static bool AddLinearSystem_Poisson3D_Q1(
		double alpha, double source, 
        Fem::Ls::CLinearSystem_Field& ls, 
		const unsigned int id_field_val, const CFieldWorld& world,
		const unsigned int id_ea)
{
//	std::cout << "Poisson3D Hex" << std::endl;

	assert( world.IsIdEA(id_ea) );
	const CElemAry& ea = world.GetEA(id_ea);
	assert( ea.ElemType() == HEX );

	if( !world.IsIdField(id_field_val) ) return false;
	const CField& field_val = world.GetField(id_field_val);

	const CElemAry::CElemSeg& es_c_co = field_val.GetElemSeg(id_ea,CORNER,false,world);
	const CElemAry::CElemSeg& es_c_va = field_val.GetElemSeg(id_ea,CORNER,true, world);

	unsigned int num_integral = 1;
	const unsigned int nInt = NIntLineGauss[num_integral];
	const double (*Gauss)[2] = LineGauss[num_integral];

	const unsigned int nno = 8;
	const unsigned int ndim = 3;

	unsigned int no_c_co[nno];	// �v�f�ߓ_�̑S�̐ߓ_�ԍ�
	unsigned int no_c_va[nno];	// �v�f�ߓ_�̑S�̐ߓ_�ԍ�

	double value_c[nno];		// �v�f�ߓ_�̒l
	double coord_c[nno][ndim];	// �v�f�ߓ_�̍��W
				
	double dndx[nno][ndim];	// �`��֐���xy����
	double an_c[nno];

	double emat[nno][nno];	// �v�f�����s��
	double eres_c[nno];	// �v�f�ߓ_�������́A�O�́A�c���x�N�g��
				
	CMatDia_BlkCrs& mat_cc = ls.GetMatrix(  id_field_val,CORNER,world);
	CVector_Blk&    res_c  = ls.GetResidual(id_field_val,CORNER,world);

	const CNodeAry::CNodeSeg& ns_c_val = field_val.GetNodeSeg(CORNER,true, world);
	const CNodeAry::CNodeSeg& ns_c_co  = field_val.GetNodeSeg(CORNER,false,world);

	for(unsigned int ielem=0;ielem<ea.Size();ielem++){
		// �v�f�z�񂩂�v�f�Z�O�����g�̐ߓ_�ԍ������o��
		es_c_co.GetNodes(ielem,no_c_co);
		for(unsigned int ino=0;ino<nno;ino++){ 
			ns_c_co.GetValue(no_c_co[ino],coord_c[ino]);
		}
		es_c_va.GetNodes(ielem,no_c_va);
		for(unsigned int ino=0;ino<nno;ino++){ 
			ns_c_val.GetValue(no_c_va[ino],&value_c[ino]);
		}
		
		for(unsigned int ino=0;ino<nno;ino++){
		for(unsigned int jno=0;jno<nno;jno++){
			emat[ino][jno] = 0.0;
		}
		}
		for(unsigned int ino=0;ino<nno;ino++){
			eres_c[ino] = 0.0;
		}
		double vol = 0.0;
		for(unsigned int ir1=0;ir1<nInt;ir1++){
		for(unsigned int ir2=0;ir2<nInt;ir2++){
		for(unsigned int ir3=0;ir3<nInt;ir3++){
			const double r1 = Gauss[ir1][0];
			const double r2 = Gauss[ir2][0];
			const double r3 = Gauss[ir3][0];
			double detjac;
			ShapeFunc_Hex8(r1,r2,r3,coord_c,detjac,dndx,an_c);
			const double detwei = detjac*Gauss[ir1][1]*Gauss[ir2][1]*Gauss[ir3][1];
			vol += detwei;
			for(unsigned int ino=0;ino<nno;ino++){
			for(unsigned int jno=0;jno<nno;jno++){
				emat[ino][jno] += alpha*detwei*(dndx[ino][0]*dndx[jno][0]+dndx[ino][1]*dndx[jno][1]+dndx[ino][2]*dndx[jno][2]);
			}
			}
			// �v�f�ߓ_�����O�̓x�N�g�����n�����ɑ������킹��
			for(unsigned int ino=0;ino<nno;ino++){
				eres_c[ino] += detwei*source*an_c[ino];
			}
		}
		}
		}

		// �v�f�ߓ_�������̓x�N�g�������߂�
		for(unsigned int ino=0;ino<nno;ino++){
		for(unsigned int jno=0;jno<nno;jno++){
			eres_c[ino] -= emat[ino][jno]*value_c[jno];
		}	
		}
		// �v�f�����s��Ƀ}�[�W����
		mat_cc.Mearge(nno,no_c_va,nno,no_c_va,1,&emat[0][0]);
		// �c���x�N�g���Ƀ}�[�W����
		for(unsigned int ino=0;ino<nno;ino++){
			res_c.AddValue( no_c_va[ino],0,eres_c[ino]);
		}
	}
	return true;
}


bool Fem::Eqn::AddLinSys_Poisson(
        Fem::Ls::CLinearSystem_Field& ls,
		double alpha, double source,
		const CFieldWorld& world,
		const unsigned int id_field_val,
		unsigned int id_ea )
{
	if( !world.IsIdField(id_field_val) ) return false;
	const CField& val_field = world.GetField(id_field_val);

	if( val_field.GetFieldType() != SCALAR ) return false;

	if( id_ea != 0 ){
		if( val_field.GetInterpolationType(id_ea,world) == TRI11 ){
			AddLinearSystem_Poisson2D_P1(alpha,source,ls,id_field_val,world,id_ea);
		}
		else if( val_field.GetInterpolationType(id_ea,world) == TRI1011 ){
			AddLinearSystem_Poisson2D_P1b(alpha,source,ls,id_field_val,world,id_ea);
		}
		else if( val_field.GetInterpolationType(id_ea,world) == TET11 ){
			AddLinearSystem_Poisson3D_P1(alpha,source,ls,id_field_val,world,id_ea);
		}
		else if( val_field.GetInterpolationType(id_ea,world) == HEX11 ){
			AddLinearSystem_Poisson3D_Q1(alpha,source,ls,id_field_val,world,id_ea);
		}
	}
	else{
		const std::vector<unsigned int> aIdEA = val_field.GetAryIdEA();
		for(unsigned int iiea=0;iiea<aIdEA.size();iiea++){
			const unsigned int id_ea = aIdEA[iiea];
			bool res = Fem::Eqn::AddLinSys_Poisson(
					ls,
					alpha, source,
					world,
					id_field_val,
					id_ea );
			if( !res ) return false;
		}
		return true;
	}

	return true;
}









////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////











bool AddLinearSystem_Poisson2D_P1(
		double alpha, double source, 
		CLinearSystem_Save& ls, 
		unsigned int id_field_val, const CFieldWorld& world,
		unsigned int id_ea)
{
//	std::cout << "Poisson2D Tri P1" << std::endl;

	assert( world.IsIdEA(id_ea) );
	const CElemAry& ea = world.GetEA(id_ea);
	assert( ea.ElemType() == TRI );

	if( !world.IsIdField(id_field_val) ) return false;
	const CField& field_val = world.GetField(id_field_val);

	const CElemAry::CElemSeg& es_c_va = field_val.GetElemSeg(id_ea,CORNER,true, world);
	const CElemAry::CElemSeg& es_c_co = field_val.GetElemSeg(id_ea,CORNER,false,world);

	const unsigned int nno = 3;
	const unsigned int ndim = 2;

	unsigned int no_c[nno];	// �v�f�ߓ_�̑S�̐ߓ_�ԍ�

	double value_c[nno];		// �v�f�ߓ_�̒l
	double coord_c[nno][ndim];	// �v�f�ߓ_�̍��W
				
	double emat[nno][nno];	// �v�f�����s��
	double eqf_out_c[nno];	// �v�f�ߓ_�������́A�O�́A�c���x�N�g��
				
	CMatDia_BlkCrs&      mat_cc  = ls.GetMatrix(id_field_val,CORNER,world);
	CVector_Blk&         force_c = ls.GetForce(id_field_val,CORNER,world);
	CMat_BlkCrs& mat_cc_boundary = ls.GetMatrix_Boundary(id_field_val,CORNER,id_field_val,CORNER,world);

	const CNodeAry::CNodeSeg& ns_c_val = field_val.GetNodeSeg(CORNER,true, world);//na_c_val.GetSeg(id_ns_c_val);
	const CNodeAry::CNodeSeg& ns_c_co  = field_val.GetNodeSeg(CORNER,false,world);//na_c_co.GetSeg(id_ns_c_co);

	for(unsigned int ielem=0;ielem<ea.Size();ielem++)
	{
		// �v�f�z�񂩂�v�f�Z�O�����g�̐ߓ_�ԍ������o��
		es_c_co.GetNodes(ielem,no_c);
		// �ߓ_�̒l������ė���
		for(unsigned int ino=0;ino<nno;ino++){
			ns_c_co.GetValue(no_c[ino],coord_c[ino]);
		}
		es_c_va.GetNodes(ielem,no_c);
		// �ߓ_�̒l������ė���
		for(unsigned int ino=0;ino<nno;ino++){
			ns_c_val.GetValue(no_c[ino],&value_c[ino]);
		}

		// �ʐς����߂�
		const double area = TriArea(coord_c[0],coord_c[1],coord_c[2]);
		// �`��֐��̔��������߂�
		double dldx[nno][ndim];	// �`��֐���xy����
		double const_term[nno];	// �`��֐��̒萔��
		TriDlDx(dldx,const_term,coord_c[0],coord_c[1],coord_c[2]);
		// �v�f�����s������
		for(unsigned int ino=0;ino<nno;ino++){
		for(unsigned int jno=0;jno<nno;jno++){
			emat[ino][jno] = alpha*area*(dldx[ino][0]*dldx[jno][0]+dldx[ino][1]*dldx[jno][1]);
		}
		}
		// �v�f�ߓ_�����O�̓x�N�g�������߂�
		for(unsigned int ino=0;ino<nno;ino++){
			eqf_out_c[ino] = source*area*0.333333333333333333;
		}
		// �v�f�����s��Ƀ}�[�W����
		mat_cc.Mearge(         nno,no_c,nno,no_c,1,&emat[0][0]);
		mat_cc_boundary.Mearge(nno,no_c,nno,no_c,1,&emat[0][0]);
		// �c���x�N�g���Ƀ}�[�W����
		for(unsigned int ino=0;ino<nno;ino++){
			force_c.AddValue( no_c[ino],0,eqf_out_c[ino]);
		}
	}
	return true;
}

static bool AddLinearSystem_Poisson2D_P1b(
		double alpha, double source, 
        Fem::Ls::CLinearSystem_Save& ls, 
		const unsigned int id_field_val, const CFieldWorld& world,
		const unsigned int id_ea)
{
	std::cout << "Poisson2D Triangle 3-point 1st order" << std::endl;

	assert( world.IsIdEA(id_ea) );
	const CElemAry& ea = world.GetEA(id_ea);
	assert( ea.ElemType() == TRI );

	if( !world.IsIdField(id_field_val) ) return false;
	const CField& field_val = world.GetField(id_field_val);

	const CElemAry::CElemSeg& es_c = field_val.GetElemSeg(id_ea,CORNER,true,world);
	const CElemAry::CElemSeg& es_b = field_val.GetElemSeg(id_ea,BUBBLE,true,world);

	const unsigned int nno_c = 3;
	const unsigned int nno_b = 1;
	const unsigned int ndim = 2;

	unsigned int no_c[nno_c];	// �v�f�ߓ_�̑S�̐ߓ_�ԍ�
	unsigned int no_b;	// �v�f�ߓ_�̑S�̐ߓ_�ԍ�

	double value_c[nno_c], value_b;	// �v�f�ߓ_�̒l
	double coord_c[nno_c][ndim];	// �v�f�ߓ_�̍��W
				
	double dldx[nno_c][ndim];	// �`��֐���xy����
	double const_term[nno_c];	// �`��֐��̒萔��

	double emat_cc[nno_c][nno_c], emat_bb, emat_cb[nno_c], emat_bc[nno_c];	// �v�f�����s��
	double eqf_out_c[nno_c];	// �v�f�ߓ_�������́A�O�́A�c���x�N�g��
	double eqf_out_b;	// �v�f�ߓ_�������́A�O�́A�c���x�N�g��

	CMatDia_BlkCrs& mat_cc = ls.GetMatrix(id_field_val,CORNER,world);
	CMatDia_BlkCrs& mat_bb = ls.GetMatrix(id_field_val,BUBBLE,world);
	CMat_BlkCrs& mat_cb = ls.GetMatrix(id_field_val,CORNER, id_field_val,BUBBLE, world);
	CMat_BlkCrs& mat_bc = ls.GetMatrix(id_field_val,BUBBLE, id_field_val,CORNER, world);
	CMat_BlkCrs& mat_cc_bound = ls.GetMatrix_Boundary(id_field_val,CORNER, id_field_val,CORNER, world);
	CMat_BlkCrs& mat_bb_bound = ls.GetMatrix_Boundary(id_field_val,BUBBLE, id_field_val,BUBBLE, world);
	CMat_BlkCrs& mat_cb_bound = ls.GetMatrix_Boundary(id_field_val,CORNER, id_field_val,BUBBLE, world);
	CMat_BlkCrs& mat_bc_bound = ls.GetMatrix_Boundary(id_field_val,BUBBLE, id_field_val,CORNER, world);
	CVector_Blk& force_c = ls.GetForce(id_field_val,CORNER,world);
	CVector_Blk& force_b = ls.GetForce(id_field_val,BUBBLE,world);

	const CNodeAry::CNodeSeg& ns_c_val = field_val.GetNodeSeg(CORNER,true, world);
	const CNodeAry::CNodeSeg& ns_b_val = field_val.GetNodeSeg(BUBBLE,true, world);
	const CNodeAry::CNodeSeg& ns_c_co  = field_val.GetNodeSeg(CORNER,false,world);

	for(unsigned int ielem=0;ielem<ea.Size();ielem++)
	{
		// �v�f�z�񂩂�v�f�Z�O�����g�̐ߓ_�ԍ������o��
		es_c.GetNodes(ielem,no_c);
		es_b.GetNodes(ielem,&no_b);
		// �ߓ_�̒l������Ă���
		for(unsigned int inoes=0;inoes<nno_c;inoes++){
			ns_c_co.GetValue(no_c[inoes],coord_c[inoes]);
			ns_c_val.GetValue(no_c[inoes],&value_c[inoes]);
		}
		ns_b_val.GetValue(no_b,&value_b);

		// �ʐς����߂�
		const double area = TriArea(coord_c[0],coord_c[1],coord_c[2]);
		// �`��֐��̔��������߂�
		TriDlDx(dldx,const_term,coord_c[0],coord_c[1],coord_c[2]);
		{	// �v�f�����s������
			double vc_b[4];
			vc_b[0] = 1.0/3.0; vc_b[1] = 1.0/3.0; vc_b[2] = 1.0/3.0; vc_b[3] = 27.0;
			const double tmp_val1 = vc_b[3]*vc_b[3]*area/180.0*( 
				dldx[0][0]*dldx[0][0]+dldx[0][1]*dldx[0][1]+
				dldx[1][0]*dldx[1][0]+dldx[1][1]*dldx[1][1]+
				dldx[2][0]*dldx[2][0]+dldx[2][1]*dldx[2][1] );
			double tmp1;
			for(unsigned int ino_c=0;ino_c<nno_c;ino_c++){
			for(unsigned int jno_c=0;jno_c<nno_c;jno_c++){
				tmp1 = 
					area*(dldx[ino_c][0]*dldx[jno_c][0]+dldx[ino_c][1]*dldx[jno_c][1])
					+vc_b[ino_c]*vc_b[jno_c]*tmp_val1;
				emat_cc[ino_c][jno_c] = tmp1;
			}
			}
			for(unsigned int ino_c=0;ino_c<nno_c;ino_c++){
				tmp1 = -1.0*vc_b[ino_c]*tmp_val1;
				emat_cb[ino_c] = tmp1;
				emat_bc[ino_c] = tmp1;
			}
			emat_bb = tmp_val1;
		}
		// �v�f�ߓ_�����O�̓x�N�g�������߂�
		for(unsigned int ino_c=0;ino_c<nno_c;ino_c++){
			eqf_out_c[ino_c] = source*area*11.0/60.0;
		}
		eqf_out_b = source*area*27.0/60.0;
		// �v�f�����s��̑S�̍����s��ւ̃}�[�W
		mat_cc.Mearge(nno_c,no_c,nno_c,no_c,	1,&emat_cc[0][0]);
		mat_cb.Mearge(nno_c,no_c,nno_b,&no_b,	1,&emat_cb[0]   );
		mat_bc.Mearge(nno_b,&no_b,nno_c,no_c,	1,&emat_bc[0]   );
		mat_bb.Mearge(nno_b,&no_b,nno_b,&no_b,	1,&emat_bb      );
		////////////////
		mat_cc_bound.Mearge(nno_c, no_c,nno_c, no_c,	1,&emat_cc[0][0]);
		mat_cb_bound.Mearge(nno_c, no_c,nno_b,&no_b,	1,&emat_cb[0]   );
		mat_bc_bound.Mearge(nno_b,&no_b,nno_c, no_c,	1,&emat_bc[0]   );
		mat_bb_bound.Mearge(nno_b,&no_b,nno_b,&no_b,	1,&emat_bb      );
		// �c���x�N�g���̃}�[�W
		for(unsigned int inoes=0;inoes<nno_c;inoes++){
			force_c.AddValue( no_c[inoes],0,eqf_out_c[inoes]);
		}
		force_b.AddValue( no_b,0,eqf_out_b );
	}
	return true;
}


bool Fem::Eqn::AddLinSys_Poisson(
		Fem::Ls::CLinearSystem_Save& ls,
		double alpha, double source,
		const Fem::Field::CFieldWorld& world,
		const unsigned int id_field_val, 
		unsigned int id_ea )
{
	if( !world.IsIdField(id_field_val) ) return false;
	const CField& val_field = world.GetField(id_field_val);

	if( val_field.GetFieldType() != SCALAR ) return false;

	if( id_ea != 0 ){
		if( val_field.GetInterpolationType(id_ea,world) == TRI11 ){
			AddLinearSystem_Poisson2D_P1(alpha,source,ls,id_field_val,world,id_ea);
		}
		else if( val_field.GetInterpolationType(id_ea,world) == TRI1011 ){
			AddLinearSystem_Poisson2D_P1b(alpha,source,ls,id_field_val,world,id_ea);
		}
		else if( val_field.GetInterpolationType(id_ea,world) == TET11 ){
			std::cout << "Error!-->Not Implimented" << std::endl;
			assert(0);
		}
	}
	else{
		const std::vector<unsigned int>& aIdEA = val_field.GetAryIdEA();
		for(unsigned int iiea=0;iiea<aIdEA.size();iiea++){
			const unsigned int id_ea = aIdEA[iiea];
			bool res = Fem::Eqn::AddLinSys_Poisson(
					ls,
					alpha, source,
					world,
					id_field_val, 
					id_ea );
			if( !res ) return false;
		}
		return true;
	}

	return true;
}






#include "delfem/matvec/diamat_blk.h"

static bool AddLinearSystem_Wave2D_P1(
		double rho, double alpha, double source, 
		CLinearSystem_Eigen& ls, 
		const unsigned int id_field_val, const CFieldWorld& world,
		const unsigned int id_ea )
{
//	std::cout << "Wave2D TriP1" << std::endl;

	assert( world.IsIdEA(id_ea) );
	const CElemAry& ea = world.GetEA(id_ea);
	assert( ea.ElemType() == TRI );

	if( !world.IsIdField(id_field_val) ) return false;
	const CField& field_val = world.GetField(id_field_val);

	const CElemAry::CElemSeg& es_c_co = field_val.GetElemSeg(id_ea,CORNER,false,world);

	const unsigned int nno = 3;
	const unsigned int ndim = 2;

	unsigned int no_c[nno];	// �v�f�ߓ_�̑S�̐ߓ_�ԍ�
	double coord_c[nno][ndim];	// �v�f�ߓ_�̍��W
				
	double eKmat[nno][nno];	// �v�f�����s��
	double eMmat[nno];	// �v�f�ߓ_�������́A�O�́A�c���x�N�g��
				
	CMatDia_BlkCrs& Kmat_cc = ls.GetMatrix(       id_field_val,CORNER,world);
	CDiaMat_Blk&    Mmat_cc = ls.GetDiaMassMatrix(id_field_val,CORNER,world);

	const CNodeAry::CNodeSeg& ns_c_co  = field_val.GetNodeSeg(CORNER,false,world);

	for(unsigned int ielem=0;ielem<ea.Size();ielem++)
	{
		// �v�f�z�񂩂�v�f�Z�O�����g�̐ߓ_�ԍ������o��
		es_c_co.GetNodes(ielem,no_c);
		for(unsigned int inoes=0;inoes<nno;inoes++){
			ns_c_co.GetValue(no_c[inoes],coord_c[inoes]);
		}
		// �ʐς����߂�
		const double area = TriArea(coord_c[0],coord_c[1],coord_c[2]);
		// �`��֐��̔��������߂�
		double dldx[nno][ndim];	// �`��֐���xy����
		double const_term[nno];	// �`��֐��̒萔��
		TriDlDx(dldx,const_term,coord_c[0],coord_c[1],coord_c[2]);
		// �v�f�����s������
		for(unsigned int ino=0;ino<nno;ino++){
		for(unsigned int jno=0;jno<nno;jno++){
			eKmat[ino][jno] = alpha*area*(dldx[ino][0]*dldx[jno][0]+dldx[ino][1]*dldx[jno][1]);
		}
		}
		// �v�f�ߓ_�����O�̓x�N�g�������߂�
		for(unsigned int ino=0;ino<nno;ino++){
			eMmat[ino] = rho*area*0.33333333333333333;
		}
		// �v�f�����s��Ƀ}�[�W����
		Kmat_cc.Mearge(nno,no_c,nno,no_c,1,&eKmat[0][0]);
		// �c���x�N�g���Ƀ}�[�W����
		for(unsigned int ino=0;ino<nno;ino++){
			Mmat_cc.Mearge(no_c[ino],1,&eMmat[ino]);
		}
	}
	return true;
}






bool Fem::Eqn::AddLinearSystem_Wave(
		Fem::Ls::CLinearSystem_Eigen& ls,
		double rho, double alpha, double source,
		const Fem::Field::CFieldWorld& world,
		const unsigned int id_field_val )
{
	if( !world.IsIdField(id_field_val) ) return false;
	const CField& val_field = world.GetField(id_field_val);

	if( val_field.GetFieldType() != SCALAR ) return false;

	const std::vector<unsigned int>& aIdEA = val_field.GetAryIdEA();
	for(unsigned int iiea=0;iiea<aIdEA.size();iiea++){
		const unsigned int id_ea = aIdEA[iiea];
		if( val_field.GetInterpolationType(id_ea,world) == TRI11 ){
			AddLinearSystem_Wave2D_P1(rho,alpha,source,ls,id_field_val,world,id_ea);
		}
	}
	return true;
}
