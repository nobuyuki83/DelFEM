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
@brief ���ە������I�u�W�F�N�g�̃C���^�[�t�F�[�X
@author Nobuyuki Umetani
*/

#if !defined(EQN_SYS_H)
#define EQN_SYS_H

#include <cassert>

#include <vector>
#include <map>

#if defined(__VISUALC__)
#pragma warning( disable : 4786 )
#endif

namespace LsSol{
	class CPreconditioner;	// �O�����s��
}
namespace Fem{
namespace Ls{
	class CLinearSystem_Field;					// �A���ꎟ������
	class CLinearSystem_Save;					// �A���ꎟ������(�����s��ۑ�)
	class CLinearSystem_SaveDiaM_NewmarkBeta;	// �A���ꎟ������(NewmarkBeta�@�ō����s��ۑ�)
}
namespace Field{
	class CField;
	class CFieldWorld;
}
namespace Eqn{

/*! 
@brief ���ۘA���������N���X
@ingroup FemEqnSystem
*/
class CEqnSystem
{
public:		
	//! �f�t�H���g�E�R���X�g���N�^
	CEqnSystem() : m_gamma_newmark(0.6), m_beta_newmark(0.3025), m_dt(0.1), pLS(0), pPrec(0){}
	//! �f�X�g���N�^
	virtual ~CEqnSystem(){ this->Clear(); }
	virtual void Clear();

	//! ������������
	virtual bool Solve(Fem::Field::CFieldWorld& world) = 0;

	const std::vector< std::pair<unsigned int, double> >& GetAry_ItrNormRes() const{ return m_aItrNormRes; }

	////////////////////////////////
	// �Œ苫�E������ǉ�����

	//! @{
	//! ��(id_field)�̗v�f�z���S�ČŒ苫�E�ɂ���
	virtual bool         AddFixField(                    unsigned int id_field, Fem::Field::CFieldWorld& world, int idof = -1) = 0;
	//! ��(m_id_val)�̗v�f�z��(id_ea)���Œ苫�E�ɂ���
	virtual unsigned int AddFixElemAry(                  unsigned int id_ea,    Fem::Field::CFieldWorld& world, int idof = -1) = 0;
	//! ��(m_id_val)�̗v�f�z��̔z��(aIdEA)���Œ苫�E�ɂ���
	virtual unsigned int AddFixElemAry( const std::vector<unsigned int>& aIdEA, Fem::Field::CFieldWorld& world, int idof = -1) = 0;
	//! �Œ苫�E�������폜����
	virtual bool ClearFixElemAry( unsigned int id_ea, Fem::Field::CFieldWorld& world ) = 0;
	//! �S�Ă̌Œ苫�E�������폜����
	virtual void ClearFixElemAry() = 0;
	//! @}

	/*! 
	@brief ���Ԑϕ��ɂ��Ẵp�����[�^��ݒ肷��
	@param [in] dt ���ԍ���
	@param [in] gamma Newmark�@��gamma(�ȗ������0.6�ɃZ�b�g)
	@param [in] beta Newmark�@��beta(gamma������Ώȗ���)
	*/
	void SetTimeIntegrationParameter(double dt, double gamma=0.6, double beta=-1.0){
		m_dt = dt;
		m_gamma_newmark = gamma;
		if( beta < 0.0 ){ m_beta_newmark = 0.25*(m_gamma_newmark+0.5)*(m_gamma_newmark+0.5); }
		else{             m_beta_newmark = beta; }
	}

	////////////////////////////////
	// �A���ꎟ�������N���X��O�����N���X�̍ĕ]���C�č\�z�w��֐�

	//! @{
	virtual void ClearValueLinearSystemPreconditioner(){ 
		this->ClearValueLinearSystem();
		this->ClearPreconditioner();
	}
	virtual void ClearValueLinearSystem(){   m_is_cleared_value_ls   = true; }	// �t���O�𗧂Ă��Solve�̎��ɒl���ĕ]�������
	virtual void ClearValuePreconditioner(){ m_is_cleared_value_prec = true; }	// �t���O�𗧂Ă��Solve�̎��ɒl���ĕ]�������

	virtual void ClearLinearSystemPreconditioner(){
		this->ClearLinearSystem(); 
		this->ClearPreconditioner();
	}
	virtual void ClearPreconditioner();
	virtual void ClearLinearSystem();
	//! @}
protected:
	std::vector< std::pair<unsigned int, double> > m_aItrNormRes;
	////////////////
	double m_gamma_newmark, m_beta_newmark, m_dt;
	Fem::Ls::CLinearSystem_Field* pLS;	// �A���ꎟ�������N���X
	LsSol::CPreconditioner* pPrec;	// �O�����N���X
	bool m_is_cleared_value_ls;
	bool m_is_cleared_value_prec;
};

}	// end namespace Eqn
}	// end namespace Fem

#endif
