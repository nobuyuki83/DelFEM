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
@brief �Q�����b�`�c�N���X(Cad::CCadObj2D)�̃C���^�[�t�F�[�X
@author Nobuyuki Umetani
*/

#if !defined(TRUSS_2D_H)
#define TRUSS_2D_H

#include <assert.h>
#include <vector>

class CTriDiaMat3;
// �g���X�N���X
class CTruss2D{
public:
    CTruss2D(unsigned int ndiv, double sx, double sy,  double ex, double ey);
    CTruss2D(const std::vector<double>& aXYs);
    ~CTruss2D();
	void StepTime(double dt){	// ���Ԃ�i�W������
		StepTime_BackWardEular(dt);
	}
    void SolveLinearStatic();
	void ClearValue(){	// �l���O�ɃZ�b�g����
		unsigned int i;
		for(i=0;i<nno*3;i++){ ut[i] = 0; }
		for(i=0;i<nno*3;i++){ vt[i] = 0; }
    }
    void SetFixedBoundaryFlag(unsigned int ino, unsigned int idim){
        assert( ino < nno );
        assert( idim < 3 );
        assert( bc_flag != 0 );
        bc_flag[ino*3+idim] = 1;
    }
    unsigned int GetSizeNode() const { return nno; }
    void ProjectPoint(double x_in, double y_in, int& idiv_min,
        double& alpha, double& ndist, double& norm_x, double& norm_y);
    void SetDisp(unsigned int ino, unsigned int idim, double disp){
        assert( ino < nno );
        assert( idim < 3 );
        assert( ut != 0 );
        ut[ino*3+idim] = disp;
    }
    void GetValueNode(unsigned int ino, double& x, double& y, double&t ) const
    {
        assert( ino < nno );
        x = ut[ino*3+0]+ini_x[ino*2+0];
        y = ut[ino*3+1]+ini_x[ino*2+1];
        t = ut[ino*3+2];
    }
private:
	void StepTime_BackWardEular(double dt);
    inline double FindNearestPointParam_Line_Point(double pc[2], double ps[2], double pe[2])
    {
        const double es[2] = { pe[0]-ps[0], pe[1]-ps[1] };
        const double sc[2] = { ps[0]-pc[0], ps[1]-pc[1] };
        const double a = es[0]*es[0]+es[1]*es[1];
        const double b = es[0]*sc[0]+es[1]*sc[1];
        return - b/a;
    }

private:
	unsigned int nno;	// �ߓ_��
	unsigned int ndiv;	// �v�f��
	double *ut, *vt;	// �ψʁC�ψʑ��x
	double *ini_x;	// �����ʒu
	int* bc_flag;	// ���E����
	////////////////
	double *ut0;	// BackWardEular�Ŏg����C�O���ԃX�e�b�v�̕ψ�
	////////////////
	// �����l
	double EI;		// �Ȃ��̍���
	double ARho;	// ����
	double AE;		// �L�k�̍���
	double color[3];
	double g[2];
private:
	// �����s��
	double *dut, *Res;	// �ψʂ̑����ʁC�c��
    CTriDiaMat3* m_mat;	// �W���s��
};

#endif
