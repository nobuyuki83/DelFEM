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


#if !defined(KER_MAT_H)
#define KER_MAT_H

#include <cassert>
#include <math.h>

//! �R�~�R�̍s��̋t�s������߂�
static void CalcInvMat3(const double a[][3], double a_inv[][3], double& det){
	det = a[0][0]*a[1][1]*a[2][2] + a[1][0]*a[2][1]*a[0][2] + a[2][0]*a[0][1]*a[1][2]
		- a[0][0]*a[2][1]*a[1][2] - a[2][0]*a[1][1]*a[0][2] - a[1][0]*a[0][1]*a[2][2];

	const double inv_det = 1.0/det;

	a_inv[0][0] = inv_det*(a[1][1]*a[2][2]-a[1][2]*a[2][1]);
	a_inv[0][1] = inv_det*(a[0][2]*a[2][1]-a[0][1]*a[2][2]);
	a_inv[0][2] = inv_det*(a[0][1]*a[1][2]-a[0][2]*a[1][1]);

	a_inv[1][0] = inv_det*(a[1][2]*a[2][0]-a[1][0]*a[2][2]);
	a_inv[1][1] = inv_det*(a[0][0]*a[2][2]-a[0][2]*a[2][0]);
	a_inv[1][2] = inv_det*(a[0][2]*a[1][0]-a[0][0]*a[1][2]);

	a_inv[2][0] = inv_det*(a[1][0]*a[2][1]-a[1][1]*a[2][0]);
	a_inv[2][1] = inv_det*(a[0][1]*a[2][0]-a[0][0]*a[2][1]);
	a_inv[2][2] = inv_det*(a[0][0]*a[1][1]-a[0][1]*a[1][0]);
}

/*! 
@brief �s��̋t�s������߂�
���̃A���S���Y����n^3�̃I�[�_�[�����̂Ŗ��s��ł����������ȍs��ȊO�g��Ȃ�����
@param a �s��̒l���~n
@param n �s��̃T�C�Y
@param info �P�Ȃ���ٍs��C�|�Ȃ�s��s��C�O�Ȃ琳��s��
*/
static void CalcInvMat(double* a, const unsigned int& n, int& info )
{
	double tmp1;

	info = 0;
	unsigned int i,j,k;
	for(i=0;i<n;i++){
		if( fabs(a[i+i*n]) < 1.0e-30 ){
			info = 1;
			return;
		}
		if( a[i+i*n] < 0.0 ){
			info--;
		}
		tmp1 = 1.0 / a[i+i*n];
		a[i+i*n] = 1.0;
		for(k=0;k<n;k++){
			a[i+k*n] *= tmp1;
		}
		for(j=0;j<n;j++){
			if( j!=i ){
				tmp1 = a[j+i*n];
				a[j+i*n] = 0.0;
				for(k=0;k<n;k++){
					a[j+k*n] -= tmp1*a[i+k*n];
				}
			}
		}
	}
}
	

#endif
