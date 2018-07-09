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


/*! @file:
@brief �l�p�`�v�f�ŗv�f�����s������ۂ�Utility�֐��̏W�܂�
@author Nobuyuki Umetani
*/

#if !defined(KER_EMAT_QUAD_H)
#define KER_EMAT_QUAD_H

#include <cassert>

#include "ker_emat_bar.h"

//! �ϕ��_�ɂ�����`��֐��̒l�Ƃ��̔��������֐�(�l�p�`�o�ꎟ�v�f)
static inline void ShapeFunc_Quad4(
	const double& r1, const double& r2,	// (����)�ϕ��_�̎��R���W�ɂ�����ʒu
	const double coords[][2],			// (����)�ߓ_���W
	double& detjac,		// �ϕ��_�ɂ����郄�R�r�A���̒l
	double dndx[][2],	// �ϕ��_�ɂ�����`��֐��̔����l
	double an[] )		// �ϕ��_�ɂ�����`��֐��̒l
{
	unsigned int inode;

	an[0] = 0.25*(1.0-r1)*(1.0-r2);
	an[1] = 0.25*(1.0+r1)*(1.0-r2);
	an[2] = 0.25*(1.0+r1)*(1.0+r2);
	an[3] = 0.25*(1.0-r1)*(1.0+r2);

	double dndr[4][2] = {
		{ -0.25*(1.0-r1), -0.25*(1.0-r2) },
		{  0.25*(1.0-r1), -0.25*(1.0+r2) },
		{  0.25*(1.0+r1),  0.25*(1.0+r2) },
		{ -0.25*(1.0+r1),  0.25*(1.0-r2) },
	};

	double dxdr[2][2]  = {
		{ 0.0, 0.0 },
		{ 0.0, 0.0 },
	};

	for(inode=0;inode<4;inode++){
		dxdr[0][0] += dndr[inode][0]*coords[inode][0];
		dxdr[0][1] += dndr[inode][1]*coords[inode][0];
		dxdr[1][0] += dndr[inode][0]*coords[inode][1];
		dxdr[1][1] += dndr[inode][1]*coords[inode][1];
	}

	detjac = dxdr[0][0]*dxdr[1][1] - dxdr[0][1]*dxdr[1][0];

	const double inv_jac = 1.0 / detjac;

	double drdx[2][2] = {
		{  inv_jac*dxdr[1][1], -inv_jac*dxdr[0][1] },
		{ -inv_jac*dxdr[1][0],  inv_jac*dxdr[0][0] }
	};

    for(inode=0;inode<4;inode++){
		dndx[inode][0] = dndr[inode][0]*drdx[0][0] + dndr[inode][1]*drdx[1][0];
		dndx[inode][1] = dndr[inode][0]*drdx[0][1] + dndr[inode][1]*drdx[1][1];
	}
}

//! �ϕ��_�ɂ�����`��֐��̒l�Ƃ��̔��������֐�(�S�p�`�o�u���ߓ_���T�ߓ_�v�f)
static inline void ShapeFunc_Quad5(
	const double& r1, const double& r2,	// (����)�ϕ��_�̎��R���W�ɂ�����ʒu
	const double coords[][2],			// (����)�ߓ_���W
	double& detjac,		// �ϕ��_�ɂ����郄�R�r�A���̒l
	double dndx_c[][2],	// �ϕ��_�ɂ�����`��֐��̔����l
	double dndx_b[2],	// �ϕ��_�ɂ�����`��֐��̔����l
	double an_c[],		// �ϕ��_�ɂ�����`��֐��̒l
	double& an_b )		// �ϕ��_�ɂ�����`��֐��̒l
{
	unsigned int inode;

	const double an[4] = {
		0.25*(1.0-r1)*(1.0-r2),
		0.25*(1.0+r1)*(1.0-r2),
		0.25*(1.0+r1)*(1.0+r2),
		0.25*(1.0-r1)*(1.0+r2) 
	};
	const double dndr[4][2] = {
		{ -0.25*(1.0-r1), -0.25*(1.0-r2) },
		{  0.25*(1.0-r1), -0.25*(1.0+r2) },
		{  0.25*(1.0+r1),  0.25*(1.0+r2) },
		{ -0.25*(1.0+r1),  0.25*(1.0-r2) },
	};
	double dxdr[2][2]  = { { 0.0, 0.0 }, { 0.0, 0.0 } };
	for(inode=0;inode<4;inode++){
		dxdr[0][0] += dndr[inode][0]*coords[inode][0];
		dxdr[0][1] += dndr[inode][1]*coords[inode][0];
		dxdr[1][0] += dndr[inode][0]*coords[inode][1];
		dxdr[1][1] += dndr[inode][1]*coords[inode][1];
	}
	detjac = dxdr[0][0]*dxdr[1][1] - dxdr[0][1]*dxdr[1][0];
	const double inv_jac = 1.0 / detjac;
	double drdx[2][2] = {
		{  inv_jac*dxdr[1][1], -inv_jac*dxdr[0][1] },
		{ -inv_jac*dxdr[1][0],  inv_jac*dxdr[0][0] }
	};

	////////////////
	an_b = 256.0*an[0]*an[1]*an[2]*an[3];
	const double dndr_b[2] = {
		256.0*(dndr[0][0]*an[1]*an[2]*an[3]+an[0]*dndr[1][0]*an[2]*an[3]+an[0]*an[1]*dndr[2][0]*an[3]+an[0]*an[1]*an[2]*dndr[3][0]),
		256.0*(dndr[0][1]*an[1]*an[2]*an[3]+an[0]*dndr[1][1]*an[2]*an[3]+an[0]*an[1]*dndr[2][1]*an[3]+an[0]*an[1]*an[2]*dndr[3][1])
	};
	dndx_b[0] = dndr_b[0]*drdx[0][0] + dndr_b[1]*drdx[1][0];
	dndx_b[1] = dndr_b[0]*drdx[0][1] + dndr_b[1]*drdx[1][1];

	////////////////
	an_c[0] = an[0]-0.25*an_b;
	an_c[1] = an[1]-0.25*an_b;
	an_c[2] = an[2]-0.25*an_b;
	an_c[3] = an[3]-0.25*an_b;
	double dndr_c[4][2] = {
		{ dndr[0][0]-0.25*dndr_b[0], dndr[0][1]-0.25*dndr_b[1] },
		{ dndr[1][0]-0.25*dndr_b[0], dndr[1][1]-0.25*dndr_b[1] },
		{ dndr[2][0]-0.25*dndr_b[0], dndr[2][1]-0.25*dndr_b[1] },
		{ dndr[3][0]-0.25*dndr_b[0], dndr[3][1]-0.25*dndr_b[1] }
	};
    for(inode=0;inode<4;inode++){
		dndx_c[inode][0] = dndr_c[inode][0]*drdx[0][0] + dndr_c[inode][1]*drdx[1][0];
		dndx_c[inode][1] = dndr_c[inode][0]*drdx[0][1] + dndr_c[inode][1]*drdx[1][1];
	}
}

//! �ϕ��_�ɂ�����`��֐��̒l�Ƃ��̔��������֐�(�S�p�`�W�ߓ_�Z�����s�f�B�e�B�[�v�f(�T�u�p�����g���b�N))
static inline void ShapeFunc_Quad8(
	const double& r1, const double& r2,	// (����)�ϕ��_�̎��R���W�ɂ�����ʒu
	const double coords[][2],			// (����)�ߓ_���W
	double& detjac,		// �ϕ��_�ɂ����郄�R�r�A���̒l
	double dndx_c[][2],	// �ϕ��_�ɂ�����`��֐��̔����l
	double dndx_e[][2],	// �ϕ��_�ɂ�����`��֐��̔����l
	double an_c[], 		// �ϕ��_�ɂ�����`��֐��̒l
	double an_e[] )		// �ϕ��_�ɂ�����`��֐��̒l
{
	unsigned int inode;
/*	double an[4] = {
		0.25*(1.0-r1)*(1.0-r2),
		0.25*(1.0+r1)*(1.0-r2),
		0.25*(1.0+r1)*(1.0+r2),
		0.25*(1.0-r1)*(1.0+r2) 
    };*/
	double dndr[4][2] = {
		{ -0.25*(1.0-r1), -0.25*(1.0-r2) },
		{  0.25*(1.0-r1), -0.25*(1.0+r2) },
		{  0.25*(1.0+r1),  0.25*(1.0+r2) },
		{ -0.25*(1.0+r1),  0.25*(1.0-r2) },
	};
	double dxdr[2][2]  = {
		{ 0.0, 0.0 },
		{ 0.0, 0.0 },
	};
	for(inode=0;inode<4;inode++){
		dxdr[0][0] += dndr[inode][0]*coords[inode][0];
		dxdr[0][1] += dndr[inode][1]*coords[inode][0];
		dxdr[1][0] += dndr[inode][0]*coords[inode][1];
		dxdr[1][1] += dndr[inode][1]*coords[inode][1];
	}
	detjac = dxdr[0][0]*dxdr[1][1] - dxdr[0][1]*dxdr[1][0];
	const double inv_jac = 1.0 / detjac;
	double drdx[2][2] = {
		{  inv_jac*dxdr[1][1], -inv_jac*dxdr[0][1] },
		{ -inv_jac*dxdr[1][0],  inv_jac*dxdr[0][0] }
	};

	////////////////
	an_c[0] = 0.25*(1.0-r1)*(1.0-r2)*(-1.0-r1-r2);
	an_c[1] = 0.25*(1.0+r1)*(1.0-r2)*(-1.0+r1-r2);
	an_c[2] = 0.25*(1.0+r1)*(1.0+r2)*(-1.0+r1+r2);
	an_c[3] = 0.25*(1.0-r1)*(1.0+r2)*(-1.0-r1+r2);
	double dndr_c[4][2] = {
		{ 0.25*( 2.0*r1+r2)*(1.0-r2), 0.25*( r1+2.0*r2)*(1.0-r1) },
		{ 0.25*( 2.0*r1-r2)*(1.0-r2), 0.25*(-r1+2.0*r2)*(1.0+r1) },
		{ 0.25*( 2.0*r1+r2)*(1.0+r2), 0.25*( r1+2.0*r2)*(1.0+r1) },
		{ 0.25*( 2.0*r1-r2)*(1.0+r2), 0.25*(-r1+2.0*r2)*(1.0-r1) }
	};
	for(inode=0;inode<4;inode++){
		dndx_c[inode][0] = dndr_c[inode][0]*drdx[0][0] + dndr_c[inode][1]*drdx[1][0];
		dndx_c[inode][1] = dndr_c[inode][0]*drdx[0][1] + dndr_c[inode][1]*drdx[1][1];
	}

	////////////////
	an_e[0] = 0.5*(1.0-r1*r1)*(1.0-r2   );
	an_e[1] = 0.5*(1.0+r1   )*(1.0-r2*r2);
	an_e[2] = 0.5*(1.0-r1*r1)*(1.0+r2   );
	an_e[3] = 0.5*(1.0-r1   )*(1.0-r2*r2);
	double dndr_e[4][2] = {
		{ 0.5*(-2.0*r1)*(1.0-r2), 0.5*(1.0-r1*r1)*(-1.0) },
		{ 0.5*(1.0-r2*r2)       , 0.5*(1.0+r1)*(-2.0*r2) },
		{ 0.5*(-2.0*r1)*(1.0+r2), 0.5*(1.0-r1*r1)        },
		{ 0.5*(-1.0)*(1.0-r2*r2), 0.5*(1.0-r1)*(-2.0*r2) }
	};
	for(inode=0;inode<4;inode++){
		dndx_e[inode][0] = dndr_e[inode][0]*drdx[0][0] + dndr_e[inode][1]*drdx[1][0];
		dndx_e[inode][1] = dndr_e[inode][0]*drdx[0][1] + dndr_e[inode][1]*drdx[1][1];
	}
}

//! �ϕ��_�ɂ�����`��֐��̒l�Ƃ��̔��������֐�(�S�p�`�Q���v�f(�T�u�p�����g���b�N))
static inline void ShapeFunc_Quad9(
	const double& r1, const double& r2,	// (����)�ϕ��_�̎��R���W�ɂ�����ʒu
	const double coords[][2],			// (����)�ߓ_���W
	double& detjac,		// �ϕ��_�ɂ����郄�R�r�A���̒l
	double dndx_c[][2],	// �ϕ��_�ɂ�����`��֐��̔����l
	double dndx_e[][2],	// �ϕ��_�ɂ�����`��֐��̔����l
	double dndx_b[2],	// �ϕ��_�ɂ�����`��֐��̔����l
	double an_c[], 		// �ϕ��_�ɂ�����`��֐��̒l
	double an_e[],		// �ϕ��_�ɂ�����`��֐��̒l
	double& an_b )		// �ϕ��_�ɂ�����`��֐��̒l
{
	unsigned int inode;
/*	double an[4] = {
		0.25*(1.0-r1)*(1.0-r2),
		0.25*(1.0+r1)*(1.0-r2),
		0.25*(1.0+r1)*(1.0+r2),
		0.25*(1.0-r1)*(1.0+r2) 
    };*/
	double dndr[4][2] = {
		{ -0.25*(1.0-r1), -0.25*(1.0-r2) },
		{  0.25*(1.0-r1), -0.25*(1.0+r2) },
		{  0.25*(1.0+r1),  0.25*(1.0+r2) },
		{ -0.25*(1.0+r1),  0.25*(1.0-r2) },
	};
	double dxdr[2][2]  = {
		{ 0.0, 0.0 },
		{ 0.0, 0.0 },
	};
	for(inode=0;inode<4;inode++){
		dxdr[0][0] += dndr[inode][0]*coords[inode][0];
		dxdr[0][1] += dndr[inode][1]*coords[inode][0];
		dxdr[1][0] += dndr[inode][0]*coords[inode][1];
		dxdr[1][1] += dndr[inode][1]*coords[inode][1];
	}
	detjac = dxdr[0][0]*dxdr[1][1] - dxdr[0][1]*dxdr[1][0];
	const double inv_jac = 1.0 / detjac;
	double drdx[2][2] = {
		{  inv_jac*dxdr[1][1], -inv_jac*dxdr[0][1] },
		{ -inv_jac*dxdr[1][0],  inv_jac*dxdr[0][0] }
	};

	////////////////
	an_c[0] =  0.25*(1.0-r1)*(1.0-r2)*r1*r2;
	an_c[1] = -0.25*(1.0+r1)*(1.0-r2)*r1*r2;
	an_c[2] =  0.25*(1.0+r1)*(1.0+r2)*r1*r2;
	an_c[3] = -0.25*(1.0-r1)*(1.0+r2)*r1*r2;
	double dndr_c[4][2] = {
		{  0.25*(1.0-r2)*r2*(1.0-2.0*r1),  0.25*(1.0-r1)*r1*(1.0-2.0*r2) },
		{ -0.25*(1.0-r2)*r2*(1.0+2.0*r1), -0.25*(1.0+r1)*r1*(1.0-2.0*r2) },
		{  0.25*(1.0+r2)*r2*(1.0+2.0*r1),  0.25*(1.0+r1)*r1*(1.0+2.0*r2) },
		{ -0.25*(1.0+r2)*r2*(1.0-2.0*r1), -0.25*(1.0-r1)*r1*(1.0+2.0*r2) }
	};
	for(inode=0;inode<4;inode++){
		dndx_c[inode][0] = dndr_c[inode][0]*drdx[0][0] + dndr_c[inode][1]*drdx[1][0];
		dndx_c[inode][1] = dndr_c[inode][0]*drdx[0][1] + dndr_c[inode][1]*drdx[1][1];
	}

	////////////////
	an_e[0] = -0.5*(1.0-r1*r1)*(1.0-r2)*r2;
	an_e[1] =  0.5*(1.0+r1)*r1*(1.0-r2*r2);
	an_e[2] =  0.5*(1.0-r1*r1)*(1.0+r2)*r2;
	an_e[3] = -0.5*(1.0-r1)*r1*(1.0-r2*r2);
	double dndr_e[4][2] = {
		{ -0.5*(-2.0*r1)*(1.0-r2)*r2   , -0.5*(1.0-r1*r1)*(1.0-2.0*r2) },
		{  0.5*(1.0+2.0*r1)*(1.0-r2*r2),  0.5*(1.0+r1)*r1*(-2.0*r2)    },
		{  0.5*(-2.0*r1)*(1.0+r2)*r2   ,  0.5*(1.0-r1*r1)*(1.0+2.0*r2) },
		{ -0.5*(1.0-2.0*r1)*(1.0-r2*r2), -0.5*(1.0-r1)*r1*(-2.0*r2)    }
	};
	for(inode=0;inode<4;inode++){
		dndx_e[inode][0] = dndr_e[inode][0]*drdx[0][0] + dndr_e[inode][1]*drdx[1][0];
		dndx_e[inode][1] = dndr_e[inode][0]*drdx[0][1] + dndr_e[inode][1]*drdx[1][1];
	}
	
	////////////////
	an_b = (1.0-r1*r1)*(1.0-r2*r2);
	double dndr_b[2] = {
		-2.0*r1*(1.0-r2*r2), (1.0-r1*r1)*-2.0*r2
	};
	dndx_b[0] = dndr_b[0]*drdx[0][0] + dndr_b[1]*drdx[1][0];
	dndx_b[1] = dndr_b[0]*drdx[0][1] + dndr_b[1]*drdx[1][1];
}

#endif // !defined(KER_EMAT_Quad_H)
