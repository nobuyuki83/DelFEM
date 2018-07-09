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

/*!
@brief �����]��������N���X(Fem::Field::CEval)�̎���
@author Nobuyuki Umetani
*/

#if defined(__VISUALC__)
    #pragma warning( disable : 4786 )   // C4786�Ȃ�ĕ\�������( ߄D�)��٧
#endif
#define for if(0);else for

#include <cassert>
#include <errno.h>	/* Need for Using "ERANGE" */
#include <math.h>	/* Need for Using "HUGE_VAL" */
#include <iostream>
#include <cstdlib> //(strtod)

#include "delfem/eval.h"

using namespace Fem::Field;

namespace Fem{
namespace Field{

class CCmd
{
public:
	virtual bool DoOperation(std::vector<double>& stack) = 0;
	virtual void SetValue(const double& val) = 0;
};

class COperand : public CCmd
{
public:
	COperand(const double val){ m_Value = val; }
	bool DoOperation(std::vector<double>& stack){
		stack.push_back(m_Value);
		return true;
	}
	static double GetValue(const int iopr){
		switch(iopr){
		case 0:
			return 3.14159265358979;
		default:
			assert(0);
			return 0.0;
		}
		return 0.0;
	}
	static int GetMaxOprInd(){ return 0; }
	void SetValue(const double& val){ m_Value = val; }
	static int GetOprInd(const std::string& str1){
		if( str1 == "PI" ){ return 0; }
		else{ return -1; }
		return -1;
	}
private:
	double m_Value;
};

class CUnaryOperator : public CCmd
{
public:
	CUnaryOperator(const int iopr){
		assert( iopr>=0 && iopr<=MaxOprInd() );
		m_iOpr = iopr;
	}
	bool DoOperation(std::vector<double>& stack){
		const double dright = stack.back();
		double& dleft = stack.back();

		switch(m_iOpr){
		case 0:		dleft = dright;			return true;
		case 1:		dleft = -dright;		return true;
		case 2:		dleft = fabs(dright);	return true;
		case 8:		dleft = floor(dright);	return true;
		case 3:		dleft = exp(dright);	return true;
		case 4:		dleft = sin(dright);	return true;
		case 5:		dleft = cos(dright);	return true;
		case 6:
			if( dright > 1.0e-30 ){ dleft = sqrt(dright); return true; }
			else{ dleft = 0.0; return false; }
		case 7:
			if( dright > 1.0e-30 ){ dleft = log(dright); return true; }
			else{ dleft = 0.0; return false; }
		default:
			std::cout << "Error!-->Illegal iOpr " << m_iOpr << std::endl;
			assert(0);
		}
		return true;
	}
	static int MaxOprInd(){ return 8; }
	static int GetOprInd(const std::string& str1){
		if( str1 == "+" )			return 0;
		else if( str1 == "-" )		return 1;
		else if( str1 == "abs" )	return 2;
		else if( str1 == "floor" )	return 8;
		else if( str1 == "exp" )	return 3;
		else if( str1 == "sin" )	return 4;
		else if( str1 == "cos" )	return 5;
		else if( str1 == "sqrt" )	return 6;
		else if( str1 == "log" )	return 7;
		return -1;
	}
	void SetValue(const double& val){
		assert(0);
	}
private:
	int m_iOpr;
};

class CBinaryOperator : public CCmd
{
public:
	CBinaryOperator(const unsigned int iopr){
		assert( iopr<5 );
		m_iOpr = iopr;
	}
	bool DoOperation(std::vector<double>& stack){
		const double dright = stack.back();
		stack.pop_back();
		double& dleft = stack.back();
		switch(m_iOpr){
		case 0:
			dleft = dleft+dright; return true;
		case 1:
			dleft = dright-dleft; return true;
		case 2:
			dleft = dleft*dright; return true;
		case 3:
			if( fabs(dleft) < 1.0e-20 ){ dleft=0.0; return false; }
			dleft = dright/dleft; return true;
		case 4:
			dleft = pow(dright,dleft); return true;
		default:
			std::cout << "Error!-->Illegal iOpr " << m_iOpr << std::endl;
			assert(0);
		}
		return false;
	}
	static int MaxOprInd(){ return 4; }
	void SetValue(const double& val){
		assert(0);
	};
	int m_iOpr;
};

}	// end namespace Field
}	// end namespace Fem


//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

CEval::~CEval(){
	for(unsigned int icmd=0;icmd<m_apCmd.size();icmd++){
		delete m_apCmd[icmd];
	}
}

void CEval::SetKey(const std::string& key_name, double key_val)
{
	for(unsigned int ikey=0;ikey<m_aKey.size();ikey++){
		if( m_aKey[ikey].m_Name == key_name){
			m_aKey[ikey].m_Val = key_val;
			for(unsigned int icmd=0;icmd<m_aKey[ikey].m_aiCmd.size();icmd++){	// ����Key�������Ă���S�Ă�Cmd�ɒl���Z�b�g
				const unsigned int icmd0 = m_aKey[ikey].m_aiCmd[icmd];
				assert( icmd0 < m_apCmd.size() );
				m_apCmd[icmd0]->SetValue( key_val );
			}
			return;
		}
	}
	// ���o�^��Key�̏ꍇ�͈ȉ��̃��[�e�B��
	m_aKey.push_back( CKey(key_name,key_val) );
}

bool CEval::SetExp(const std::string& exp){
	m_is_valid = false;
	m_sExp = exp;
	// ����
	for(unsigned int icmd=0;icmd<m_apCmd.size();icmd++){ delete m_apCmd[icmd]; }
	for(unsigned int ikey=0;ikey<m_aKey.size();ikey++){ m_aKey[ikey].m_aiCmd.clear(); }
	m_apCmd.clear();

	////////////////
	std::string tmp_exp = exp;
	RemoveExpressionSpaceNewline(tmp_exp);
	RemoveExpressionBracket(tmp_exp);
	if( tmp_exp.size() == 0 ){
		m_is_valid = false;
		return false;
	}
	{
		std::vector<SExpCompo> exp_vec;
		exp_vec.reserve(exp.size());
		exp_vec.resize(1);
		exp_vec[0].sOpe = tmp_exp;
		if( !MakeRPN(0,exp_vec) ){
			m_is_valid = false;
			return false;
		}

/*		{	// �t�|�[�����h�\�L��\��
			for(unsigned int iexno=0;iexno<exp_vec.size();iexno++){
				std::cout << exp_vec[iexno].sOpe << " ";
			}
			std::cout << std::endl;
		}*/

		if( !MakeCmdAry(m_apCmd,exp_vec) ){
			m_is_valid = false;
			return false;
		}
	
/*
		int iflg = true;
		int stack_size = 0;
		for(int icmd=0;icmd<m_apCmd.size();icmd++){
			if( dynamic_cast<COperand*>(m_apCmd[icmd]) != NULL ){
				stack_size++;
			}
			else if( dynamic_cast<CUnaryOperator*>(m_apCmd[icmd]) != NULL ){
				if( stack_size <= 0 ) iflg = false; break;
			}
			else if( dynamic_cast<CBinaryOperator*>(m_apCmd[icmd]) != NULL ){
				if( stack_size <= 1 ) iflg = false;	break;
			}
		}
		if( !iflg ){
			std::cout << "Error!-->Operator and Operand mismatch" << std::endl;
			m_bValid = false;
			return false;
		}
*/	
	}
	{	// �o�^����Ă���L�[�S�Ă̒l��ݒ�
		for(unsigned int ikey=0;ikey<m_aKey.size();ikey++){
			const CKey& key = m_aKey[ikey];
			double val = key.m_Val;
			for(unsigned int icmd=0;icmd<key.m_aiCmd.size();icmd++){	// ����Key�������Ă���S�Ă�Cmd�ɒl���Z�b�g
				const unsigned int icmd0 = key.m_aiCmd[icmd];
				assert( icmd0 < m_apCmd.size() );
				m_apCmd[icmd0]->SetValue( val );
			}
		}	
	}
	m_is_valid = true;
	return true;
}

double CEval::Calc() const{
	// �����]��
	std::vector<double> stack;
	stack.reserve(128);
	for(unsigned int icmd=0;icmd<m_apCmd.size();icmd++){
		m_apCmd[icmd]->DoOperation(stack);
	}
	assert( stack.size() == 1 );
	return stack[0];
}

bool CEval::MakeCmdAry(std::vector<CCmd*>& cmd_vec, const std::vector<SExpCompo>& exp_node_vec)
{
	cmd_vec.resize( exp_node_vec.size(), 0 );
	for(unsigned int iexp=0;iexp<exp_node_vec.size();iexp++){
		const SExpCompo& compo = exp_node_vec[iexp];
		if( compo.iOpeType == 0 ){ // numeric
			char* e;
			double val = strtod(compo.sOpe.c_str(),&e);
			if (errno != ERANGE){
				cmd_vec[iexp] = new COperand(val);
			}
			else if (val == HUGE_VAL){
				std::cout << "Exceeding the range of (double)" << std::endl;
				return false;
			}
		}
		else if( compo.iOpeType == 1 ){ // symbol
			if( compo.iOpe == -1 ){
				bool bflg = false;
				for(unsigned int ikey=0;ikey<m_aKey.size();ikey++){
					if( m_aKey[ikey].m_Name == compo.sOpe ){
						m_aKey[ikey].m_aiCmd.push_back(iexp);
						bflg = true;
						break;
					}
				}
				if( !bflg ){
					std::cout << "���̃I�y�����h�����߂ł��܂���ł���:" << compo.sOpe << std::endl;
					return false;
				}
				cmd_vec[iexp] = new COperand(0.0);
			}
			else{
				int iopr0 = compo.iOpe;
				if( iopr0 >=0 && iopr0 <= COperand::GetMaxOprInd() ){
					cmd_vec[iexp] = new COperand( COperand::GetValue(compo.iOpe) );
				}
				else{
					std::cout << "Not assumed Operand" << std::endl;
					assert(0);
				}
			}
		}
		else if( compo.iOpeType == 2 ){ // unary 
			cmd_vec[iexp] = new CUnaryOperator( compo.iOpe );
		}
		else if( compo.iOpeType == 3 ){ // binary
			cmd_vec[iexp] = new CBinaryOperator( compo.iOpe );
		}
		else{
			std::cout << "Error!--> " << compo.sOpe << " " << compo.iOpeType << std::endl;
			assert(0);
		}
	}
	return true;
}

bool CEval::MakeRPN(unsigned int icur_old, std::vector<SExpCompo>& exp_node_vec)
{
	assert( icur_old < exp_node_vec.size() );

	unsigned int ibegin0,iend0,itype0,iopr0;
	{
		std::string& cur_exp = exp_node_vec[icur_old].sOpe;
		int itmp0, itmp1, itmp2, itmp3;
		int ierr = GetLowestPriorityOperator(itmp0, itmp1, itmp2, itmp3, cur_exp);
		if( ierr != 0 ){
			std::cout << "Error!-->Cannot interprit this expression : " << cur_exp << std::endl;
			return false;
		}
        if( itmp0 == 0 && itmp1 == (int)cur_exp.size() ){	// case operand �Z�p���l(PI�݂����Ȃ�)
			assert( itmp2 == 0 || itmp2 == 1 );
			assert( itmp3>=-1 && itmp3<=COperand::GetMaxOprInd() );
			exp_node_vec[icur_old].iOpeType = itmp2;
			exp_node_vec[icur_old].iOpe = itmp3;
			return true;
		}
		assert( itmp2 != 0 && itmp2 != 1 );
		ibegin0 = static_cast<unsigned int>(itmp0);
		iend0 = static_cast<unsigned int>(itmp1);
		assert( itmp2 >=0 && itmp2 <= 3 );
		itype0 = static_cast<unsigned int>(itmp2);
		assert( ( itmp2==2 && itmp3>=0 && itmp3<=CUnaryOperator::MaxOprInd() ) || 
			( itmp2==3 && itmp3>=0 && itmp3<=CBinaryOperator::MaxOprInd() ) );
		iopr0 = static_cast<unsigned int>(itmp3);
	}

	std::string cur_old_exp = exp_node_vec[icur_old].sOpe;

	{
		int ileft = icur_old;
		SExpCompo& left_compo = exp_node_vec[ileft];
		left_compo.sOpe.assign( cur_old_exp, iend0, cur_old_exp.size()-iend0 );
		RemoveExpressionBracket(left_compo.sOpe);
		if( left_compo.sOpe.size() == 0 ){ return false; }
		if( !MakeRPN(ileft,exp_node_vec) ){ return false; }
	}

	if( ibegin0 > 0 ){
		int iright = exp_node_vec.size();
		exp_node_vec.resize( exp_node_vec.size()+1 );
		SExpCompo& right_compo = exp_node_vec[iright];
		right_compo.sOpe.assign( cur_old_exp, 0, ibegin0 );
		RemoveExpressionBracket(right_compo.sOpe);
		if( right_compo.sOpe.size() == 0 ){ return false; }
		if( !MakeRPN(iright,exp_node_vec) ){ return false; }
	}

	{
		const int icur_new = exp_node_vec.size();
		exp_node_vec.resize( exp_node_vec.size()+1 );
		SExpCompo& new_compo = exp_node_vec[icur_new];
		new_compo.sOpe.assign( cur_old_exp, ibegin0, iend0-ibegin0 );
		new_compo.iOpeType = itype0;
		new_compo.iOpe = iopr0;
	}

	return true;
}

void CEval::RemoveExpressionBracket(std::string& exp)
{
    if( exp[0] != '(' || exp[exp.size()-1] != ')' ) return;
    {
        int iBracketDepth = 1;
        for ( size_t ipos=1; ipos<exp.size()-1; ipos++ )
        {
            if ( exp[ipos] == '(' ){ iBracketDepth++; }
            if ( exp[ipos] == ')' ){ iBracketDepth--; }
            if ( iBracketDepth == 0 ) return;
        }
    }
    {
		std::string stemp(exp);
        for ( size_t ipos=1; ipos<exp.size()-1; ipos++ ){
            exp[ipos-1] = stemp[ipos];
        }
		exp.resize( exp.size()-2 );
    }
    RemoveExpressionBracket(exp);
}


void CEval::RemoveExpressionSpaceNewline( std::string& exp )
{
	const std::string stemp(exp);
	unsigned int iexp=0;
    for( unsigned int itemp=0; itemp<stemp.size(); itemp++ ){
		if( stemp[itemp] == ' ' || stemp[itemp] == 0x0d || stemp[itemp] == 0x0a )	continue;
        exp[iexp] = stemp[itemp];
		iexp++;
    }
	exp.resize(iexp);
}

////////////////////////////////////////////////////////////////
// ����exp�̒��̂����Ƃ��D�揇�ʂ̒Ⴂ�I�y���^�[����������B
// itype�̓I�y���^�[�̎��
// itype : 0 --> Numerical variable
// itype : 1 --> Argebric variable
// itype : 2 --> Unary Operator
// itype : 3 --> Binary Operator
int CEval::GetLowestPriorityOperator(int& ibegin, int& iend, int& itype, int& iopr, 
										const std::string& exp )
{
	itype = -1; iopr = -1;

    const int ipriority_max = 10;

    int ipos_min = -1;
    int ipriority_min = ipriority_max;
	int itype_min = -1;
	int iopr_min = -1;

	bool is_numeric = true;
    int iBracketDepth = 0;
	int iFirstBracket = -1;
    for(unsigned int ipos_curr=0 ;ipos_curr<exp.size(); ipos_curr++ ){
		// ���ʓ���ǂݔ�΂�
		switch ( exp[ipos_curr] )
		{
		case '(': 
			if( iFirstBracket == -1 ) iFirstBracket = ipos_curr;
			iBracketDepth++; continue;
		case ')':
			iBracketDepth--; continue;
		default:
			if( iBracketDepth != 0 ) continue;
		}
		assert( iBracketDepth == 0 );

		// ipriority_curr���v�Z����
		int ipriority_curr;
        int itype_curr = -1;
        int iopr_curr = -1;
		switch( exp[ipos_curr] ){
		case '+':
			ibegin = ipos_curr; iend = ipos_curr+1;
			if( ipos_curr == 0 ){ // sign operator
				ipriority_curr=5; itype_curr=2; iopr_curr=0; break; 
			}
			else if( exp[ipos_curr-1] != '*' && exp[ipos_curr-1] != '/' && exp[ipos_curr-1] != '^' ){
				iopr=0; itype=3; return 0;	// binary operator
			}
			continue;
		case '-': 
			ibegin = ipos_curr; iend = ipos_curr+1;
			if( ipos_curr == 0 ){ // sign operator
				ipriority_curr=5; itype_curr=2; iopr_curr=1; break; 
			}
			else if( exp[ipos_curr-1] != '*' && exp[ipos_curr-1] != '/' && exp[ipos_curr-1] != '^' ){
				iopr=1; itype=3; return 0;	// binary operator
			}
			continue;
		case '*': 
			ipriority_curr=3; itype_curr=3; iopr_curr=2; break;
		case '/': 
			ipriority_curr=4; itype_curr=3; iopr_curr=3; break;
		case '^': 
			ipriority_curr=6; itype_curr=3; iopr_curr=4; break;
		default:
			ipriority_curr = ipriority_max;
			if( ( exp[ipos_curr] < '0' || exp[ipos_curr] > '9' ) && exp[ipos_curr] != '.' ){
				is_numeric = false; 
			}
			break;
		}
        if ( ipriority_curr < ipriority_min ){
            ipos_min = ipos_curr;
            ipriority_min = ipriority_curr;
            itype_min = itype_curr;
            iopr_min = iopr_curr;
        }
    }

	if( iBracketDepth != 0 ){
		std::cout << "Error!-->Not corresponding bracket" << std::endl;
		return 2;
	}
    if( ( ipos_min==0 || ipos_min==(int)exp.size()-1 ) && itype_min==3 ){
		std::cout << "Error!-->Binary operator misplaced " << std::endl;
		return 9;
	}
	if( iFirstBracket == 0 && ipos_min==-1 ){
		std::cout << "Error!-->(hoge)foo" << std::endl;
		return 3;
	}
	// binary operator(+�Ƃ�-)
	if( ipos_min > 0 ){	
		assert( itype_min == 3 );
		assert( iopr_min != -1 );
		ibegin = ipos_min; iend = ipos_min+1;
		itype = itype_min; iopr = iopr_min;
		return 0;
	}
	// sign operator( -hoge )
	if( ipos_min == 0 ){
		assert( itype_min == 2 );
		assert( iopr_min != -1 );
		ibegin = ipos_min; iend = ipos_min+1;
		itype = itype_min; iopr = iopr_min;
		return 0;
	}
	// preposition operator ( sin, tan�݂����� )
	if( !is_numeric && iFirstBracket != -1 ){
		if( exp[ exp.size()-1 ] != ')' ){	// avoid  sin(x+y)a
			std::cout << "Error!-->hoge1" << std::endl;
			return 1;
		}
        if( iFirstBracket == (int)exp.size()-2 ){	// avoid sin()
			std::cout << "Error!-->hoge2" << std::endl;
			return 2;
		}
		std::string str1(exp,0,iFirstBracket);
		int iopr0 = CUnaryOperator::GetOprInd(str1);
		if( iopr0 != -1 ){
			ibegin = 0; iend = iFirstBracket;
			itype = 2; iopr = iopr0;
			return 0;
		}
		else{
			std::cout << "Error!-->Unregistered Unary Operator" << std::endl;
			return 1;
		}
	}
	// variable
	ibegin = 0; iend = exp.size();
	if( is_numeric ){ itype=0; iopr=-1; }	// ���l(5.4321�݂�����)
	else{
		int iopr0 = COperand::GetOprInd(exp);
		if( iopr0 != -1 ){ itype = 1; iopr = iopr0; }	// PI�݂����Ȃ�
		else{ itype=1; iopr=-1; }
	}
	return 0;
}
