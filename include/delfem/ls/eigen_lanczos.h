
#if !defined(EIGEN_LANCZOS_H)
#define EIGEN_LANCZOS_H

#include <vector>

namespace LsSol{
class CLinearSystem;
class CPreconditioner;

//bool ArnoldiQR(Fem::Ls::CLinearSystem_Eigen& ls );
//bool EigenValue_Lanczos( unsigned int nlambda, std::vector<double>& aLambda, unsigned int num_iter, Fem::Ls::CLinearSystem_Eigen& ls);
double MinimumEigenValueVector_InvPower(
	LsSol::CLinearSystem& ls,
	LsSol::CPreconditioner& pls,
	const std::vector<unsigned int>& aIdVec, 
	unsigned int itr_invp,	// �t�ׂ���@�̍ő唽����
	unsigned int itr_lssol, // ICCG�@�̍ő唽����
	double conv_res_lssol,	// ICCG�@�̎�����̑��Ύc��
	int& iflag_conv );		// 0:����ɏI���@1:ICCG���������Ȃ�����
}

#endif
