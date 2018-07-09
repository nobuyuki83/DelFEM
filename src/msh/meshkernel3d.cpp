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


#if defined(__VISUALC__)
#pragma warning( disable : 4786 )   // C4786�Ȃ�ĕ\�������( ߄D�)��٧
#endif
#define for if(0);else for

#include <map>
#include <set>
#include <stack>
#include <iostream>
#include <time.h>
#include <stdio.h>

#include "delfem/msh/meshkernel3d.h"


using namespace Msh;

const unsigned int nTri5 = 10;
const unsigned int tri5[nTri5][3] = {
	{ 0, 1, 2 }, // 0
	{ 0, 1, 3 }, // 1
	{ 0, 1, 4 }, // 2
	{ 0, 2, 3 }, // 3
	{ 0, 2, 4 }, // 4
	{ 0, 3, 4 }, // 5
	{ 1, 2, 3 }, // 6
	{ 1, 2, 4 }, // 7
	{ 1, 3, 4 }, // 8
	{ 2, 3, 4 }, // 9
};

const unsigned int tri2Swap5Index[nTri5+1] = {
	 0,
	 2,
	 3,
	 5,
	 6,
	 7,
	 9,
	11,
	12,
	13,
	15
};

const unsigned int nSwap5 = 5;
const unsigned int tri2Swap5[15] = {
	0, 2,
	3,
	1, 4,
	0,
	2,
	0, 3,
	1, 3,
	4, 
	1,
	2, 4,
};

const unsigned int indexRot5[5][5] = { 
	{ 0, 1, 2, 3, 4 },
	{ 1, 2, 3, 4, 0 },
	{ 2, 3, 4, 0, 1 },
	{ 3, 4, 0, 1, 2 },
	{ 4, 0, 1, 2, 3 }
};

const unsigned int swap2SupSwapRot[nSwap5][2] = {
	{ 0, 0 },
	{ 0, 1 },
	{ 0, 2 },
	{ 0, 3 },
	{ 0, 4 },
};

const unsigned int nSupSwap5 = 1;
const unsigned int nTriInSwap5 = 3;
const unsigned int sup2Noel5[nSupSwap5][nTriInSwap5][3] = {
	{ { 0, 1, 2 },{ 0, 2, 3 },{ 0, 3, 4 } },
};

bool Msh::MakeTetSurTet(std::vector<STet>& tet)
{
	unsigned int ntetsuno;
	unsigned int* tetsuno_ind = 0;
	unsigned int* tetsuno = 0;

	unsigned int nnode;
	{	// �l�ʑ̂ɎQ�Ƃ���Ă���ߓ_�ł����Ƃ��ԍ��̑傫�����́{�P��T��
		nnode = 0;
		for(unsigned int itet=0;itet<tet.size();itet++){
		for(unsigned int inotet=0;inotet<4;inotet++){
			nnode = ( tet[itet].v[inotet] > nnode ) ? tet[itet].v[inotet] : nnode;
		}
		}
		nnode += 1;
	}

	MakeTetSurNo(tet,nnode,ntetsuno,tetsuno_ind,tetsuno);

	const unsigned int nelem = tet.size();
	const unsigned int nfael = 4;
	const unsigned int nnofa = 3;
	const unsigned int nnoel = 4;

	int* help_node = new int [nnode];
	int help_nofa[nnofa];

	const unsigned int noel_face2[4][4] = {
		{ 0, 1, 2, 3 },
		{ 1, 0, 3, 2 },
		{ 2, 0, 1, 3 },
		{ 3, 0, 2, 1 },
	};

	for(unsigned int ielem=0;ielem<nelem;ielem++){
		for(unsigned int ifael=0;ifael<nfael;ifael++){
			tet[ielem].g[ifael] = -1;
			tet[ielem].s[ifael] = 0;
			tet[ielem].f[ifael] = 0;
		}
	}

	for(unsigned int inode=0;inode<nnode;inode++){ help_node[inode] = 0; }
	for(unsigned int ielem=0;ielem<nelem;ielem++){
		for(unsigned int ifael=0;ifael<nfael;ifael++){
			if( tet[ielem].g[ifael] != -1 ){
				assert( tet[ielem].g[ifael] == -2 );
				continue;
			}
			for(unsigned int inofa=0;inofa<nnofa;inofa++){
				help_nofa[inofa] = tet[ielem].v[ Msh::noelTetFace[ifael][inofa] ];
			}

			help_node[ help_nofa[0] ] = 1;
			help_node[ help_nofa[1] ] = 2;
			help_node[ help_nofa[2] ] = 3;
			
			const int ino1 = help_nofa[0];
			bool iflag = false;
			assert( ino1 >= 0 && (unsigned int)ino1 < nnode );
			for(unsigned int itetsuno=tetsuno_ind[ino1];itetsuno<tetsuno_ind[ino1+1];itetsuno++){
				const unsigned int jelem1 = tetsuno[itetsuno];
				if( jelem1 == ielem ) continue;
				assert( jelem1>=0 && jelem1<nelem );
				for(unsigned int jfael=0;jfael<nfael;jfael++){
					unsigned int icoun1 = 0;
					for(unsigned int jnofa=0;jnofa<nnofa;jnofa++){
						icoun1 += help_node[ tet[jelem1].v[ noelTetFace[jfael][jnofa] ] ];
					}
					if( icoun1 == 6 ){
                        unsigned int jnoel2inoel[4];
						for(unsigned int jnoel=0;jnoel<nnoel;jnoel++){
							jnoel2inoel[jnoel] = noel_face2[ifael][ help_node[ tet[jelem1].v[jnoel] ] ];
						}

						assert( tet[jelem1].v[noelTetFace[jfael][0]] == tet[ielem].v[jnoel2inoel[noelTetFace[jfael][0]]] );
						assert( tet[jelem1].v[noelTetFace[jfael][1]] == tet[ielem].v[jnoel2inoel[noelTetFace[jfael][1]]] );
						assert( tet[jelem1].v[noelTetFace[jfael][2]] == tet[ielem].v[jnoel2inoel[noelTetFace[jfael][2]]] );

						int jrel1 = noel2Rel[ jnoel2inoel[0]*4+jnoel2inoel[1] ];
						assert( jrel1 != -1 );

						assert( tetRel[jrel1][0] == jnoel2inoel[0] );
						assert( tetRel[jrel1][1] == jnoel2inoel[1] );

						if( tetRel[jrel1][2] == jnoel2inoel[3] ){
							assert( tetRel[jrel1][3] == jnoel2inoel[2] );
							std::cout << "Error!-->Inverse Element" << ielem << " " << jelem1 << std::endl;
							assert(0);
							break;
						}
						assert( tetRel[jrel1][3] == jnoel2inoel[3] );
						assert( tetRel[jrel1][2] == jnoel2inoel[2] );

						int irel1 = invTetRel[jrel1];

						assert( tetRel[jrel1][jfael] == ifael );
						assert( tet[jelem1].v[noelTetFace[jfael][0]] == tet[ielem].v[tetRel[jrel1][noelTetFace[jfael][0]]] );
						assert( tet[jelem1].v[noelTetFace[jfael][1]] == tet[ielem].v[tetRel[jrel1][noelTetFace[jfael][1]]] );
						assert( tet[jelem1].v[noelTetFace[jfael][2]] == tet[ielem].v[tetRel[jrel1][noelTetFace[jfael][2]]] );

						assert( tetRel[irel1][ifael] == jfael );
						assert( tet[ielem].v[noelTetFace[ifael][0]] == tet[jelem1].v[tetRel[irel1][noelTetFace[ifael][0]]] );
						assert( tet[ielem].v[noelTetFace[ifael][1]] == tet[jelem1].v[tetRel[irel1][noelTetFace[ifael][1]]] );
						assert( tet[ielem].v[noelTetFace[ifael][2]] == tet[jelem1].v[tetRel[irel1][noelTetFace[ifael][2]]] );

						tet[ielem].s[ifael] = jelem1;
						tet[ielem].f[ifael] = irel1;
						tet[ielem].g[ifael] = -2;

						tet[jelem1].s[jfael] = ielem;
						tet[jelem1].f[jfael] = jrel1;
						tet[jelem1].g[jfael] = -2;

						iflag = true;
						break;
					}
				}
				if( iflag ) break;
			}
			for(unsigned int inofa=0;inofa<nnofa;inofa++){
				help_node[ help_nofa[inofa] ] = 0;
			}
		}
	}
	delete[] help_node;
	delete[] tetsuno_ind;
	delete[] tetsuno;

	return true;
}

bool MakeOneTetSurNo(const std::vector<STet>& tet,
					 std::vector<CPoint3D>& point)
{
	assert( point.size() > 0 );
	assert( tet.size() > 0 );

    const unsigned int npotet = 4;

	for(unsigned int ipoin=0;ipoin<point.size();ipoin++){
		point[ipoin].e = -1;
		point[ipoin].poel = -1;
	}

	for(unsigned int itet=0;itet<tet.size();itet++){
		for(unsigned int ipotet=0;ipotet<npotet;ipotet++){
			point[ tet[itet].v[ipotet] ].e = itet;
			point[ tet[itet].v[ipotet] ].poel = ipotet;
		}
	}
	return true;
}

bool Msh::MakeTetSurNo(const std::vector<STet>& tet,
	const unsigned int npoin,
	unsigned int& ntetsupo,
	unsigned int*& tetsupo_ind,
	unsigned int*& tetsupo )
{
	assert( tetsupo_ind == 0 );
	assert( tetsupo == 0 );

    const unsigned int npotet = 4;
	tetsupo_ind = new unsigned int [npoin+1];
	for(unsigned int ipoin=0;ipoin<npoin+1;ipoin++){
		tetsupo_ind[ipoin] = 0;
	}
	for(unsigned int itet=0;itet<tet.size();itet++){
		for(unsigned int inoel=0;inoel<npotet;inoel++){
			assert( tet[itet].v[inoel] >= 0 && tet[itet].v[inoel] < npoin );
			tetsupo_ind[ tet[itet].v[inoel]+1 ]++;
		}
	}
	for(unsigned int ipoin=0;ipoin<npoin;ipoin++){
		tetsupo_ind[ipoin+1] += tetsupo_ind[ipoin];
	}
	ntetsupo = tetsupo_ind[npoin];
	tetsupo = new unsigned int [ntetsupo];

	for(unsigned int itet=0;itet<tet.size();itet++){
		for(unsigned int ipoel=0;ipoel<npotet;ipoel++){
			const int ipoin1 = tet[itet].v[ipoel];
			tetsupo[ tetsupo_ind[ipoin1] ] = itet;
			tetsupo_ind[ ipoin1 ]++;
		}
	}
	for(int ipoin=npoin-1;ipoin>=0;ipoin--){
		tetsupo_ind[ipoin+1] = tetsupo_ind[ipoin];
	}
	tetsupo_ind[0] = 0;
/*
	std::cout << ntetsuno << std::endl;
	for(int inode=0;inode<nnode;inode++){
		std::cout << inode << "-->";
		for(int itetsuno=tetsuno_ind[inode];itetsuno<tetsuno_ind[inode+1];itetsuno++){
			std::cout << tetsuno[itetsuno] << " ";
		}
		std::cout << std::endl;
	}
*/
	return true;
}

bool Msh::MakeTriSurTri(std::vector<STri3D>& tri )
{
	unsigned int nnode;
	{	// �O�p�`�ɎQ�Ƃ���Ă���ߓ_�ł����Ƃ��ԍ��̑傫�����́{�P��T��
		nnode = 0;
		for(unsigned int itri=0;itri<tri.size();itri++){
		for(unsigned int inotri=0;inotri<3;inotri++){
			nnode = ( tri[itri].v[inotri] > nnode ) ? tri[itri].v[inotri] : nnode;
//			std::cout << tri[itri].v[inotri] << std::endl;
		}
		}
		nnode += 1;
	}

	unsigned int ntrisuno;
	unsigned int* trisuno_ind = 0;
	unsigned int* trisuno = 0;
	MakeTriSurNo(ntrisuno,trisuno_ind,trisuno,  tri,nnode);

	const unsigned int ntri = tri.size();
//	const unsigned int nedtri = 3;
//	const unsigned int nnoed = 2;

	// ���̂����w�b�_�t�@�C���ɏ���u��
	const unsigned int EdEd2Rel[nEdTri][nEdTri] = {
        { 0, 2, 1 },
        { 2, 1, 0 },
        { 1, 0, 2 },
	};
	// �����܂�

	for(unsigned int itri=0;itri<ntri;itri++){
	for(unsigned int iedtri=0;iedtri<nEdTri;iedtri++){
		tri[itri].g2[iedtri] = -1;
		tri[itri].s2[iedtri] = -1;
		tri[itri].r2[iedtri] = -1;
	}
	}

	unsigned int* help_node = new unsigned int [nnode];
	for(unsigned int inode=0;inode<nnode;inode++){ help_node[inode] = 0; }
	int help_noed[nNoEd];

	for(unsigned int itri=0;itri<ntri;itri++){
	for(unsigned int iedtri=0;iedtri<nEdTri;iedtri++){
        if( tri[itri].g2[iedtri] == -2 ){ continue; }
		for(unsigned int inoed=0;inoed<nNoEd;inoed++){
			help_noed[inoed] = tri[itri].v[ noelTriEdge[iedtri][inoed] ];
		}

		help_node[ help_noed[0] ] = 1;
		help_node[ help_noed[1] ] = 2;
			
		const int ino1 = help_noed[0];
		bool iflag = false;
		assert( ino1 >= 0 && (unsigned int)ino1 < nnode );
		for(unsigned int itrisuno=trisuno_ind[ino1];itrisuno<trisuno_ind[ino1+1];itrisuno++){
			const unsigned int jtri1 = trisuno[itrisuno];
			if( jtri1 == itri ) continue;
			assert( jtri1>=0 && jtri1<ntri );
			for(unsigned int jedtri=0;jedtri<nEdTri;jedtri++){
				unsigned int icoun1 = 0;
				for(unsigned int jnoed=0;jnoed<nNoEd;jnoed++){
					icoun1 += help_node[ tri[jtri1].v[ noelTriEdge[jedtri][jnoed] ] ];
				}
				if( icoun1 == 3 ){
					tri[itri].s2[iedtri] = jtri1;
					tri[itri].g2[iedtri] = -2;
					tri[itri].r2[iedtri] = EdEd2Rel[iedtri][jedtri];
					assert( tri[jtri1].g2[jedtri] == -1 );
					tri[jtri1].s2[jedtri] = itri;
					tri[jtri1].g2[jedtri] = -2;
					tri[jtri1].r2[jedtri] = EdEd2Rel[jedtri][iedtri];
					iflag = true;
					break;
				}
			}
			if( iflag ) break;
		}
		if( tri[itri].g2[iedtri] == -1 ){
//			std::cout << "edge " << itri << " " << iedtri << std::endl;
		}

		help_node[ help_noed[0] ] = 0;
		help_node[ help_noed[1] ] = 0;
	}
	}

	delete[] help_node;
	delete[] trisuno_ind;
	delete[] trisuno;

	return true;
}

bool Msh::MakeTriSurTet(std::vector<STet>& tet,
				   std::vector<STri3D>& tri,
				   const std::vector<CPoint3D>& vertex ){

	unsigned int ntrisuno;
	unsigned int* trisuno_ind = 0;
	unsigned int* trisuno = 0;
	MakeTriSurNo(ntrisuno,trisuno_ind,trisuno,  tri,vertex.size());

	const unsigned int nnode = vertex.size();
	const unsigned int ntet = tet.size();

    const unsigned int nfatet = 4;
    const unsigned int nnofa = 3;
//	const int nnotet = 4;

    const unsigned int ntri = tri.size();
    const unsigned int nfatri = 2;

	int* help_node = new int [nnode];
	int help_nofa[3];
	const unsigned int noel_tri_face[2][3] = {
		{ 0, 1, 2 },
		{ 2, 1, 0 },
	};

	for(unsigned int itri=0;itri<tri.size();itri++){
		for(unsigned int ifatri=0;ifatri<nfatri;ifatri++){
			tri[itri].sf[ifatri] = -1;
			tri[itri].gf[ifatri] = -1;
		}
	}

	for(unsigned int inode=0;inode<nnode;inode++){ help_node[inode] = 0; }

	for(unsigned int itet=0;itet<ntet;itet++){
		for(unsigned int ifatet=0;ifatet<nfatet;ifatet++){
			if( tet[itet].g[ifatet] == -2 ) continue;
			for(unsigned int inofa=0;inofa<nnofa;inofa++){
				help_nofa[inofa] = tet[itet].v[noelTetFace[ifatet][inofa]];
			}
			help_node[ help_nofa[0] ] = 1;
			help_node[ help_nofa[1] ] = 2;
			help_node[ help_nofa[2] ] = 3;
			int inode1 = help_nofa[0];
			bool iflag = false;
			for(unsigned int jtrisuno=trisuno_ind[inode1];jtrisuno<trisuno_ind[inode1+1];jtrisuno++){
                const unsigned int jtri1 = trisuno[jtrisuno];
				assert( jtri1>=0 && jtri1<ntri );
				for(unsigned int jfatri=0;jfatri<nfatri;jfatri++){
					int icoun = 0;
					for(unsigned int jnofa=0;jnofa<nnofa;jnofa++){
						icoun += help_node[ tri[jtri1].v[noel_tri_face[jfatri][jnofa]] ];
					}
					if( icoun == 6 ){
						iflag = true;
						tet[itet].s[ifatet] = jtri1;
						tet[itet].g[ifatet] = 1;

						tri[jtri1].sf[jfatri] = itet;
						tri[jtri1].gf[jfatri] = 0;

						break;
					}
				}
				if( iflag ) break;
			}
			if( !iflag ){
				std::cout << "Error!--> No Tri Outer Elem Face " << itet << " " << ifatet << std::endl;
				assert(0);
			}
			for(unsigned int inofa=0;inofa<nnofa;inofa++){
				help_node[ help_nofa[inofa] ] = 0;
			}

		}
	}

	delete[] trisuno_ind;
	delete[] trisuno;
	delete[] help_node;
	
	return true;
}

bool Msh::MakeOuterBoundTri( const std::vector<STri3D>& aTri, std::vector<SBar>& aBar )
{
	unsigned int counter;
	{
		counter = 0;
		const unsigned int ntri = aTri.size();
		for(unsigned int itri=0;itri<ntri;itri++){
			for(unsigned int iedtri=0;iedtri<nEdTri;iedtri++){
				if( aTri[itri].g2[iedtri] != -2 ) counter++;
			}
		}
	}
//	const unsigned int nbar = counter;
	aBar.reserve(counter);
	const unsigned int ntri = aTri.size();
	for(unsigned int itri=0;itri<ntri;itri++){
	for(unsigned int iedtri=0;iedtri<nEdTri;iedtri++){
		if( aTri[itri].g2[iedtri] != -2 ){
			unsigned int ibar0 = aBar.size();
			aBar.resize( aBar.size()+1 );
			aBar[ibar0].v[0] = aTri[itri].v[ noelTriEdge[iedtri][0] ];
			aBar[ibar0].v[1] = aTri[itri].v[ noelTriEdge[iedtri][1] ];
		}
	}
	}
	return true;
}

bool Msh::MakeOuterBoundTet(const std::vector<STet>& aTet, std::vector<STri3D>& aTri){
	unsigned int counter;
	{
		counter = 0;
		const unsigned int ntet = aTet.size();
		for(unsigned int itet=0;itet<ntet;itet++){
			for(unsigned int ifatet=0;ifatet<4;ifatet++){
				if( aTet[itet].g[ifatet] != -2 ) counter++;
			}
		}
	}
//	const unsigned int ntri = counter;
	aTri.reserve(counter);
	const unsigned int ntet = aTet.size();
	for(unsigned int itet=0;itet<ntet;itet++){
	for(unsigned int ifatet=0;ifatet<4;ifatet++){
		if( aTet[itet].g[ifatet] != -2 ){
			unsigned int itri0 = aTri.size();
			aTri.resize( aTri.size()+1 );
			aTri[itri0].v[0] = aTet[itet].v[ noelTetFace[ifatet][0] ];
			aTri[itri0].v[1] = aTet[itet].v[ noelTetFace[ifatet][1] ];
			aTri[itri0].v[2] = aTet[itet].v[ noelTetFace[ifatet][2] ];
		}
	}
	}
	return true;
}
bool Msh::MakeEdgeTet(unsigned int& nedge,
			  unsigned int*& edge_ind,
			  unsigned int*& edge,
			  const std::vector<STet>& tet,
			  const unsigned int nnode)
{
//	const unsigned int nnoel = 4;

	edge_ind = new unsigned int [nnode+1];
	for(unsigned int inode=0;inode<nnode+1;inode++){ edge_ind[inode] = 0; }

	unsigned int ntetsuno = 0;
	unsigned int* tetsuno_ind = NULL;
	unsigned int* tetsuno = NULL;
	MakeTetSurNo(tet,nnode,ntetsuno,tetsuno_ind,tetsuno);

	int* help_node = new int [nnode];
	for(unsigned int inode=0;inode<nnode;inode++){ help_node[inode] = -1; }

	edge_ind[0] = 0;
	for(unsigned int inode=0;inode<nnode;inode++){
		help_node[inode] = inode;
		for(unsigned int itetsuno=tetsuno_ind[inode];itetsuno<tetsuno_ind[inode+1];itetsuno++){
			const unsigned int itet1 = tetsuno[itetsuno];
			for(unsigned int inoel=0;inoel<4;inoel++){
                if( help_node[ tet[itet1].v[inoel] ] != (int)inode ){
					help_node[ tet[itet1].v[inoel] ] = inode;
					edge_ind[inode+1]++;
				}
			}
		}
	}

	for(unsigned int inode=0;inode<nnode;inode++){
		edge_ind[inode+1] += edge_ind[inode];
	}

	nedge = edge_ind[nnode];
	edge = new unsigned int [nedge];

	for(unsigned int inode=0;inode<nnode;inode++){ help_node[inode] = -1; }

	for(unsigned int inode=0;inode<nnode;inode++){
		help_node[inode] = inode;
		for(unsigned int itetsuno=tetsuno_ind[inode];itetsuno<tetsuno_ind[inode+1];itetsuno++){
			const unsigned int itet1 = tetsuno[itetsuno];
			for(unsigned int inoel=0;inoel<4;inoel++){
                if( help_node[ tet[itet1].v[inoel] ] != (int)inode ){
					help_node[ tet[itet1].v[inoel] ] = inode;
					edge[ edge_ind[inode] ] = tet[itet1].v[inoel];
					edge_ind[inode]++;
				}
			}
		}
	}

	for(unsigned int inode=nnode;inode>0;inode--){
		edge_ind[inode] = edge_ind[inode-1];
	}
	edge_ind[0] = 0;
/*
	for(int inode=0;inode<nnode;inode++){
		std::cout << inode << " " << edge_ind[inode+1] - edge_ind[inode] << "-->";
		for(int iedge=edge_ind[inode];iedge<edge_ind[inode+1];iedge++){
			std::cout << edge[iedge] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << "NEdge " << nedge << std::endl;
*/

	delete[] tetsuno_ind;
	delete[] tetsuno;

	return true;
}




////////////////////////////////////////////////////////////////

bool Msh::MakeHexSurNo(unsigned int& nhexsupo,
				  unsigned int*& hexsupo_ind,
				  unsigned int*& hexsupo,
				  const std::vector<SHex>& aHex, const unsigned int npoin)
{
	assert( hexsupo_ind == 0 );
	assert( hexsupo == 0 );

	const unsigned int npohex = 8;
	hexsupo_ind = new unsigned int [npoin+1];
	for(unsigned int ipoin=0;ipoin<npoin+1;ipoin++){
		hexsupo_ind[ipoin] = 0;
	}
	for(unsigned int ihex=0;ihex<aHex.size();ihex++){
		for(unsigned int inoel=0;inoel<npohex;inoel++){
			assert( aHex[ihex].v[inoel] < npoin );
			hexsupo_ind[ aHex[ihex].v[inoel]+1 ]++;
		}
	}
	for(unsigned int ipoin=0;ipoin<npoin;ipoin++){
		hexsupo_ind[ipoin+1] += hexsupo_ind[ipoin];
	}
	nhexsupo = hexsupo_ind[npoin];
	hexsupo = new unsigned int [nhexsupo];

	for(unsigned int ihex=0;ihex<aHex.size();ihex++){
		for(unsigned int ipoel=0;ipoel<npohex;ipoel++){
			const int ipoin1 = aHex[ihex].v[ipoel];
			hexsupo[ hexsupo_ind[ipoin1] ] = ihex;
			hexsupo_ind[ ipoin1 ]++;
		}
	}
	for(int ipoin=npoin-1;ipoin>=0;ipoin--){
		hexsupo_ind[ipoin+1] = hexsupo_ind[ipoin];
	}
	hexsupo_ind[0] = 0;
	return true;
}

bool Msh::MakeHexSurHex(std::vector<SHex>& aHex)
{
	unsigned int npoin;
	{	// �l�ʑ̂ɎQ�Ƃ���Ă���ߓ_�ł����Ƃ��ԍ��̑傫�����́{�P��T��
		npoin = 0;
		for(unsigned int ihex=0;ihex<aHex.size();ihex++){
		for(unsigned int inohex=0;inohex<8;inohex++){
			npoin = ( aHex[ihex].v[inohex] > npoin ) ? aHex[ihex].v[inohex] : npoin;
		}
		}
		npoin += 1;
	}

	unsigned int nhexsuno;
	unsigned int* hexsuno_ind = 0;
	unsigned int* hexsuno = 0;

	MakeHexSurNo(nhexsuno,hexsuno_ind,hexsuno, aHex,npoin);

	const unsigned int nelem = aHex.size();
	const unsigned int nfael = 6;
	const unsigned int nnofa = 4;
//	const unsigned int nnoel = 8;

	for(unsigned int ielem=0;ielem<nelem;ielem++){
		for(unsigned int ifael=0;ifael<nfael;ifael++){
			aHex[ielem].g[ifael] = -1;
			aHex[ielem].s[ifael] = 0;
			aHex[ielem].f[ifael] = 0;
		}
	}

	int help_nofa[nnofa];
	int* help_node = new int [npoin];
	for(unsigned int ipoin=0;ipoin<npoin;ipoin++){ help_node[ipoin] = 0; }

	for(unsigned int ielem=0;ielem<nelem;ielem++){
	for(unsigned int ifael=0;ifael<nfael;ifael++){
		if( aHex[ielem].g[ifael] != -1 ){
			assert( aHex[ielem].g[ifael] == -2 );
			continue;
		}
		for(unsigned int inofa=0;inofa<nnofa;inofa++){
			help_nofa[inofa] = aHex[ielem].v[ Msh::noelHexFace[ifael][inofa] ];
		}
		for(unsigned int inofa=0;inofa<nnofa;inofa++){
			help_node[ help_nofa[inofa] ] = 1;
		}
			
		const unsigned int ino1 = help_nofa[0];
		bool iflag = false;
		assert( ino1 < npoin );
		for(unsigned int ihexsuno=hexsuno_ind[ino1];ihexsuno<hexsuno_ind[ino1+1];ihexsuno++){
			const unsigned int jelem1 = hexsuno[ihexsuno];
			if( jelem1 == ielem ) continue;
			assert( jelem1>=0 && jelem1<nelem );
			for(unsigned int jfael=0;jfael<nfael;jfael++){
				unsigned int icoun1 = 0;
				for(unsigned int jnofa=0;jnofa<nnofa;jnofa++){
					icoun1 += help_node[ aHex[jelem1].v[ Msh::noelHexFace[jfael][jnofa] ] ];
				}
				if( icoun1 == nnofa ){
					aHex[ielem].s[ifael] = jelem1;
					aHex[ielem].f[ifael] = 0;
					aHex[ielem].g[ifael] = -2;
					aHex[jelem1].s[jfael] = ielem;
					aHex[jelem1].f[jfael] = 0;
					aHex[jelem1].g[jfael] = -2;
					iflag = true;
					break;
				}
			}
			if( iflag ) break;
		}
		for(unsigned int inofa=0;inofa<nnofa;inofa++){
			help_node[ help_nofa[inofa] ] = 0;
		}
	}
	}
	delete[] help_node;
	delete[] hexsuno_ind;
	delete[] hexsuno;

	return true;
}


bool Msh::MakeBar_fromColorCodingTri( const std::vector<STri3D>& aTri, 
	const std::vector<int>& aColorTri, unsigned int nColor, std::vector<SBar>& aBar)
{
	aBar.clear();
	for(unsigned int itri=0;itri<aTri.size();itri++){
	for(unsigned int iedtri=0;iedtri<nEdTri;iedtri++){
		unsigned int itri_s = aTri[itri].s2[iedtri];
		if( itri_s > itri ) continue;
		assert( itri_s < aTri.size() );
		unsigned int irel = aTri[itri].r2[iedtri];
		unsigned int iedtri_s = relTriTri[irel][iedtri];
        assert( aTri[itri_s].s2[iedtri_s] == itri );
		unsigned int icolor0 = aColorTri[itri];
		unsigned int icolor_s = aColorTri[itri_s];
		assert( icolor0 < nColor );
		assert( icolor_s < nColor );
		if( icolor0 != icolor_s ){
			SBar bar;
			bar.v[0] = aTri[itri].v[ noelTriEdge[iedtri][0] ];
			bar.v[1] = aTri[itri].v[ noelTriEdge[iedtri][1] ];
			bar.s2[0] = itri;
			bar.s2[1] = itri_s;
			aBar.push_back( bar );
		}
	}
	}
	return true;
}
/*
bool Msh::MakeColorCodingBar( const std::vector<SBar>& aBar,  const std::vector<Com::CVector3D>& aVec, 
						  std::vector<int>& aColorBar, unsigned int& nColor)
{
	const unsigned int nbar = aBar.size();
	aColorBar.clear();
	aColorBar.resize( nbar, -1 );

	unsigned int cnt_color = 0;
	for(;;){
		int ibar_ker = -1;
		for(unsigned int ibar=0;ibar<nbar;ibar++){
			if( aColorBar[ibar] == -1 ){
				ibar_ker = ibar;
				aColorBar[ibar] = cnt_color;
				break;
			}
		}
		if( ibar_ker == -1 ) break;
		for(unsigned int inobar_ker=0;inobar_ker<2;inobar_ker++){
			unsigned int inobar0 = inobar_ker;
			unsigned int ibar0 = ibar_ker;
			for(;;){
				if( aBar[ibar0].g1[inobar0] != -2 ) break;
				unsigned int ibar1 = aBar[ibar0].s1[inobar0];
				if( aColorBar[ibar1] != -1 ) break;
				const unsigned int iv0 = aBar[ibar0].v[  inobar0];
				const unsigned int iv1 = aBar[ibar0].v[1-inobar0];
				unsigned int inobar1;
				{
					assert( aBar[ibar0].r1[inobar0] == 0 || aBar[ibar0].r1[inobar0] == 1 );
					if( aBar[ibar0].r1[inobar0] == 0 ){ inobar1 = inobar0; }
					else{ inobar1 = 1-inobar0; }
					assert( aBar[ibar1].v[inobar1] == iv1 );
				}
				const unsigned int iv2 = aBar[ibar1].v[1-inobar1];
				bool iflag = true;
				{
					Com::CVector3D vec01 = aVec[iv1]+(-1*aVec[iv0]);
					Com::CVector3D vec12 = aVec[iv2]+(-1*aVec[iv1]);
					vec01.Normalize();
					vec12.Normalize();
					const double dot = vec01.x*vec12.x + vec01.y*vec12.y + vec01.z*vec12.z;
					if( dot > 0.8 ){ iflag = true; }
					else{ iflag = false; }
				}
				if( iflag ){
					aColorBar[ibar1] = cnt_color;
					ibar0 = ibar1;
					inobar0 = inobar1;
				}
				else{ break; }
			}
		}
		cnt_color++;
	}

	nColor = cnt_color;
	return true;
}
*/
/*
bool Msh::MakeColorCodingBar( const std::vector<SBar> aBar, 
							 std::vector<int>& aColorBar, unsigned int& nColorBar)
{
	const unsigned int nbar = aBar.size();
	unsigned int npoin;
	{	
		npoin = 0;
		for(unsigned int ibar=0;ibar<nbar;ibar++){
			unsigned int ipoi0 = aBar[ibar].v[0];
			unsigned int ipoi1 = aBar[ibar].v[1];
			npoin = ( ipoi0 > npoin ) ? ipoi0 : npoin;
			npoin = ( ipoi1 > npoin ) ? ipoi1 : npoin;
		}
		npoin += 1;
	}
	std::cout << npoin << std::endl;
	unsigned int nelsup;
	unsigned int* elsup_ind = new unsigned int [npoin+1];
	unsigned int* elsup = 0;
	MakePointSurBar( aBar,npoin,   elsup_ind,nelsup,elsup );
	{
		aColorBar.resize(nbar,-1);
		nColorBar = 0;
		for(;;){
			unsigned int ibar0;
			for(ibar0=0;ibar0<nbar;ibar0++){
				if( aColorBar[ibar0] == -1 ){ break; }
			}
			if( ibar0 != nbar ){
				std::stack<unsigned int> stack_bar;
				stack_bar.push( ibar0 );
				for(;;){
					if( stack_bar.empty() ) break;
					ibar0 = stack_bar.top();
					stack_bar.pop();
					if( aColorBar[ibar0] != -1 ){ continue; }
					aColorBar[ibar0] = nColorBar;
					unsigned int iv0 = aBar[ibar0].v[0];
					unsigned int iv1 = aBar[ibar0].v[1];
					assert( elsup_ind[iv0+1]-elsup_ind[iv0] == 2 );
					assert( elsup_ind[iv1+1]-elsup_ind[iv1] == 2 );
					unsigned int ibar00 = elsup[ elsup_ind[iv0]   ];
					unsigned int ibar01 = elsup[ elsup_ind[iv0]+1 ];
					assert( ibar00 == ibar0 || ibar01 == ibar0 );
					if( ibar00 == ibar0 ){ stack_bar.push(ibar01); }
					else{ stack_bar.push(ibar00); }
					unsigned int ibar10 = elsup[ elsup_ind[iv1]   ];
					unsigned int ibar11 = elsup[ elsup_ind[iv1]+1 ];
					assert( ibar10 == ibar0 || ibar11 == ibar0 );
					if( ibar10 == ibar0 ){ stack_bar.push(ibar11); }
					else{ stack_bar.push(ibar10); }
				}
				nColorBar++;
				continue;
			}
			break;
		}
	}
	delete[] elsup_ind;
	delete[] elsup;

	return true;
}
*/

// �׍����O�p�`�̒P�ʖ@�����m��crt_dot�ȉ��Ȃ炻��͐؂藣��
bool Msh::MakeColorCoding_Tri3D( const std::vector<STri3D>& aTri, const std::vector<Com::CVector3D>& aVec, 
						   std::vector<int>& aColorTri, unsigned int& nColor, double crt_dot)
{
	const unsigned int ntri = aTri.size();
	aColorTri.clear();
	aColorTri.resize( ntri, -1 );
	unsigned int cnt_color = 0;
	for(;;){
		int itri_ini = -1;
		for(unsigned int itri=0;itri<ntri;itri++){
			if( aColorTri[itri] == -1 ){
				aColorTri[itri] = cnt_color;
				itri_ini = itri;
				break;
			}
		}
		if( itri_ini == -1 ) break;
		std::stack<unsigned int> nex_elem_stack;
		nex_elem_stack.push(itri_ini);
		for(;;){
			if( nex_elem_stack.empty() ) break;
			const unsigned int itri_cur = nex_elem_stack.top();
			nex_elem_stack.pop();
			for(unsigned int iedtri=0;iedtri<3;iedtri++){
				if( aTri[itri_cur].g2[iedtri] != -2 ) continue;
				unsigned int itri_s = aTri[itri_cur].s2[iedtri];
				if( aColorTri[itri_s] != -1 ) continue;
				bool iflag = true;	// �ׂƐڑ����Ă���true
				{
					Com::CVector3D vec0, vec1;
					UnitNormal(vec0,itri_cur,aTri,aVec);
					UnitNormal(vec1,itri_s,  aTri,aVec);
					if( Com::Dot(vec0,vec1) > crt_dot ){ iflag = true; }				
					else{ iflag = false; }
				}
				if( iflag ){ // �ׂƐڑ����Ă���
					nex_elem_stack.push(itri_s);
					aColorTri[itri_s] = cnt_color;
				}
			}
		}
		cnt_color++;
	}
	nColor = cnt_color;
	return true;
}


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////


bool Msh::CheckTri(const std::vector<STri3D>& tri )
{

	std::cout << "Check Tri" << std::endl;
	// ���̂����w�b�_�t�@�C���ɏ���u��
//	const unsigned int nNoTri = 3;
	const unsigned int nEdTri = 3;
	const unsigned int nNoEd = 2;
	const unsigned int noelTriEdge[nEdTri][nNoEd] = {
		{ 1, 2 },
		{ 2, 0 },
		{ 0, 1 },
	};
	static const unsigned int relTriTri[3][3] = {
		{ 0, 2, 1 }, //  0
		{ 2, 1, 0 }, //  1 
		{ 1, 0, 2 }, //  2
	};
	// �����܂�

//	const unsigned int npo = po.size();
	const unsigned int ntri = tri.size();

	for(unsigned int itri=0;itri<ntri;itri++){
		const STri3D& ref_tri = tri[itri];
		for(unsigned int iedtri=0;iedtri<nEdTri;iedtri++){
			if( ref_tri.g2[iedtri] == -2 || ref_tri.g2[iedtri] == -3 ){
				const unsigned int itri_s = ref_tri.s2[iedtri];
				const unsigned int irel = ref_tri.r2[iedtri];
				assert( itri_s < ntri );
				assert( irel < 3 );
				// check sorounding
				{
					const unsigned int noel_dia = relTriTri[irel][iedtri];
					assert( noel_dia < 3 );
                    assert( tri[itri_s].s2[noel_dia] == itri );
				}
				// check relation 
				for(unsigned int inoed=0;inoed<nNoEd;inoed++){
					const unsigned int inoel = noelTriEdge[iedtri][inoed];
					assert( ref_tri.v[inoel] == tri[itri_s].v[ relTriTri[irel][inoel] ] );
				}
			}
		}
	}

	return true;
}


bool Msh::CheckTet(const std::vector<STet>& tet,
			  const std::vector<CPoint3D>& vertex)
{
	std::cout << " *** CheckTet *** ";

	for(unsigned int ipoin=0;ipoin<vertex.size();ipoin++){
		if( vertex[ipoin].e!=-1 ){
			if( vertex[ipoin].e < 0 || (unsigned int)vertex[ipoin].e >= tet.size() ){
				std::cout << ipoin << " " << vertex[ipoin].e << " " << tet.size() << std::endl;
			}
			if( tet[ vertex[ipoin].e ].v[ vertex[ipoin].poel ] != ipoin ){
				std::cout << ipoin << " " << vertex[ipoin].e << " " << (int)vertex[ipoin].poel << " " << tet[ vertex[ipoin].e ].v[ vertex[ipoin].poel ] << std::endl;
			}
		}
		if( vertex[ipoin].e!=-1 ){
			assert( vertex[ipoin].e >= 0 && (unsigned int)vertex[ipoin].e < tet.size()    );
			assert( vertex[ipoin].poel >= 0 && (unsigned int)vertex[ipoin].poel < 4       );
			assert( tet[ vertex[ipoin].e ].v[ vertex[ipoin].poel ] == ipoin );
		}
	}
	
	std::cout << "p ";

	const unsigned int nnotet = 4;
	for(unsigned int itet=0;itet<tet.size();itet++){
		for(unsigned int inotet=0;inotet<nnotet;inotet++){
			unsigned int ino = tet[itet].v[inotet];
			assert( ino < vertex.size() );
		}
		if( TetVolume(itet,tet,vertex) < 1.0e-15 ){
			std::cout << " negative " << itet << " " << TetVolume(itet,tet,vertex) << std::endl;
		}
		assert( TetVolume(itet,tet,vertex) > 1.0e-15 );
	}
	
	std::cout << "v ";
	
	const unsigned int nfatet = 4;
	for(unsigned int itet=0;itet<tet.size();itet++){
		for(unsigned int ifatet=0;ifatet<nfatet;ifatet++){
			if( tet[itet].g[ifatet] != -2 ){
				if( tet[itet].g[ifatet] == 0 ){
					std::cout << "ILLEGAL GROUP " << std::endl;
					std::cout << itet << " " << ifatet << " " << tet[itet].g[ifatet] << std::endl;
					getchar();
				}
//				std::cout << itet << " " << ifatet << "-->" << tet[itet].g[ifatet] << std::endl;
				continue;
			}
			const int adj = tet[itet].s[ifatet];
			assert( adj >= 0 && (unsigned int)adj < tet.size() );
			if( tet[itet].f[ifatet] < 0 || tet[itet].f[ifatet] >= 12 ){
				std::cout << itet << " " << ifatet << " " << (int)tet[itet].f[ifatet] << std::endl;
			}
			assert( tet[itet].f[ifatet] >= 0 && tet[itet].f[ifatet] < 12 );
			const unsigned int* rel1 = tetRel[ tet[itet].f[ifatet] ];
			if( tet[adj].s[ rel1[ifatet] ] != itet ){
				std::cout << itet << " " << ifatet << "   " << adj << " " << (int)rel1[ifatet] << std::endl;
			}
			if( tet[itet].v[ noelTetFace[ifatet][0] ] != tet[adj].v[ rel1[ noelTetFace[ifatet][0] ] ] ){
				std::cout << itet << " " << ifatet << " --> 0 " << std::endl;
			}
			if( tet[itet].v[ noelTetFace[ifatet][1] ] != tet[adj].v[ rel1[ noelTetFace[ifatet][1] ] ] ){
				std::cout << itet << " " << ifatet << " --> 1 " << std::endl;
			}
			if( tet[itet].v[ noelTetFace[ifatet][2] ] != tet[adj].v[ rel1[ noelTetFace[ifatet][2] ] ] ){
				std::cout << itet << " " << ifatet << " --> 2 " << std::endl;
			}
			assert( tet[adj].s[ rel1[ifatet] ] == itet );
			assert( tet[itet].v[ noelTetFace[ifatet][0] ] == tet[adj].v[ rel1[ noelTetFace[ifatet][0] ] ] );
			assert( tet[itet].v[ noelTetFace[ifatet][1] ] == tet[adj].v[ rel1[ noelTetFace[ifatet][1] ] ] );
			assert( tet[itet].v[ noelTetFace[ifatet][2] ] == tet[adj].v[ rel1[ noelTetFace[ifatet][2] ] ] );
		}
	}
	
	std::cout << "r   OK" << std::endl;

	return true;
}


bool Msh::CheckTet(const std::vector<STet>& tet)
{
	std::cout << " *** CheckTet *** ";
	
	const unsigned int nfatet = 4;
	for(unsigned int itet=0;itet<tet.size();itet++){
		for(unsigned int ifatet=0;ifatet<nfatet;ifatet++){
			if( tet[itet].g[ifatet] != -2 ){
				if( tet[itet].g[ifatet] == 0 ){
					std::cout << "ILLEGAL GROUP " << std::endl;
					std::cout << itet << " " << ifatet << " " << tet[itet].g[ifatet] << std::endl;
					getchar();
				}
//				std::cout << itet << " " << ifatet << "-->" << tet[itet].g[ifatet] << std::endl;
				continue;
			}
			const int adj = tet[itet].s[ifatet];
			assert( adj >= 0 && (unsigned int)adj < tet.size() );
			if( tet[itet].f[ifatet] < 0 || tet[itet].f[ifatet] >= 12 ){
				std::cout << itet << " " << ifatet << " " << (int)tet[itet].f[ifatet] << std::endl;
			}
			assert( tet[itet].f[ifatet] >= 0 && tet[itet].f[ifatet] < 12 );
			const unsigned int* rel1 = tetRel[ tet[itet].f[ifatet] ];
			if( tet[adj].s[ rel1[ifatet] ] != itet ){
				std::cout << itet << " " << ifatet << "   " << adj << " " << (int)rel1[ifatet] << std::endl;
			}
			if( tet[itet].v[ noelTetFace[ifatet][0] ] != tet[adj].v[ rel1[ noelTetFace[ifatet][0] ] ] ){
				std::cout << itet << " " << ifatet << " --> 0 " << std::endl;
			}
			if( tet[itet].v[ noelTetFace[ifatet][1] ] != tet[adj].v[ rel1[ noelTetFace[ifatet][1] ] ] ){
				std::cout << itet << " " << ifatet << " --> 1 " << std::endl;
			}
			if( tet[itet].v[ noelTetFace[ifatet][2] ] != tet[adj].v[ rel1[ noelTetFace[ifatet][2] ] ] ){
				std::cout << itet << " " << ifatet << " --> 2 " << std::endl;
			}
			assert( tet[adj].s[ rel1[ifatet] ] == itet );
			assert( tet[itet].v[ noelTetFace[ifatet][0] ] == tet[adj].v[ rel1[ noelTetFace[ifatet][0] ] ] );
			assert( tet[itet].v[ noelTetFace[ifatet][1] ] == tet[adj].v[ rel1[ noelTetFace[ifatet][1] ] ] );
			assert( tet[itet].v[ noelTetFace[ifatet][2] ] == tet[adj].v[ rel1[ noelTetFace[ifatet][2] ] ] );
		}
	}
	
	std::cout << "r   OK" << std::endl;

	return true;
}

bool Msh::MakeElemAroundEdge( ElemAroundEdge& elared,
						const int itet0, 
						const int idedge0, 
						const std::vector<STet>& tet ){
//	std::cout << "Make Elared " << itet0 << " " << idedge0  << std::endl;
	elared.clear();

	int irel0 = noel2Rel[ dEdge2Noel[idedge0][0]*4 + dEdge2Noel[idedge0][1] ];
    assert( dEdge2Noel[idedge0][0] == tetRel[irel0][0] );
    assert( dEdge2Noel[idedge0][1] == tetRel[irel0][1] );

	elared.nod = tet[itet0].v[(int)tetRel[irel0][0]];
	elared.nou = tet[itet0].v[(int)tetRel[irel0][1]];

	std::pair<int,int> pair_ii;

	pair_ii.first = itet0;
	pair_ii.second = irel0;

	elared.e.push_back(pair_ii);
	elared.n.push_back(tet[itet0].v[ tetRel[irel0][3] ]); 

	for(;;){
		const int itet_pre = elared.e[ elared.e.size()-1 ].first;
		const int iedrel_pre = elared.e[ elared.e.size()-1 ].second;
        assert( tet[itet0].v[ tetRel[irel0][0] ] == elared.nod );
        assert( tet[itet0].v[ tetRel[irel0][1] ] == elared.nou );
		const int inoel0 = tetRel[iedrel_pre][3];
		if( tet[itet_pre].g[inoel0] != -2 ){
			elared.clear();
			elared.is_inner = false;
			return true;
		}
		const int irel_pre = tet[itet_pre].f[inoel0];
		const int itet_cur = tet[itet_pre].s[inoel0];
		if( itet_cur == itet0 ) break;
		const int iedrel_cur = noel2Rel[ tetRel[irel_pre][ tetRel[iedrel_pre][0] ]*4 + tetRel[irel_pre][ tetRel[iedrel_pre][1] ] ];
        assert( tet[itet_cur].v[ tetRel[iedrel_cur][0] ] == elared.nod );
        assert( tet[itet_cur].v[ tetRel[iedrel_cur][1] ] == elared.nou );
        assert( tet[itet_cur].v[ tetRel[iedrel_cur][3] ] == tet[itet_pre].v[ tetRel[iedrel_pre][2] ] );
		pair_ii.first = itet_cur;
		pair_ii.second = iedrel_cur;
		elared.e.push_back( pair_ii );
		elared.n.push_back(tet[itet_cur].v[ tetRel[iedrel_cur][3] ]);
	}
	elared.n.push_back(tet[itet0].v[ tetRel[irel0][3] ]);
	elared.is_inner = true;
	return true;
}

double Msh::MaxCrtElemAroundPoint(const ElemAroundPoint& elarpo,
						  const std::vector<STet>& tet,
						  const std::vector<CPoint3D>& node)
{
	double max_crt = -1.0;
	double crt1;
	assert( elarpo.size() > 0 );
    const unsigned int point_cnt = tet[elarpo.e.begin()->first].v[elarpo.e.begin()->second];
	for(std::map<int,unsigned int>::const_iterator itr_map_ic=elarpo.e.begin();itr_map_ic!=elarpo.e.end();itr_map_ic++){
		const int itet0 = itr_map_ic->first;
        const unsigned int inoel0 = itr_map_ic->second;
		assert( tet[itet0].v[inoel0] == point_cnt );
		crt1 = Criterion_PLJ(itet0,node,tet);
		if( crt1 < 0.0 || crt1 > ILL_CRT*0.9  ){
			return ILL_CRT;
		}
		if( crt1 > max_crt ) max_crt = crt1;
	}
	assert( max_crt > 0.0 );
	return max_crt;
}

double Msh::MaxCrtElemAroundEdge(const ElemAroundEdge& elared,
						  const std::vector<STet>& tet,
						  const std::vector<CPoint3D>& node){
	double max_crt = -1.0;
	double crt1;
	assert( elared.size() > 0 );
    for(unsigned int itetared=0;itetared<elared.size();itetared++){
		crt1 = Criterion_PLJ(
			node[elared.nod],
			node[elared.nou],
			node[elared.n[itetared]],
			node[elared.n[itetared+1]] );
		if( crt1 < 0.0 || crt1 > ILL_CRT*0.9  ){
			return ILL_CRT;
		}
		if( crt1 > max_crt ) max_crt = crt1;
	}
	assert( max_crt > 0.0 );
	return max_crt;
}

bool Swap3Elared(const ElemAroundEdge& elared,
		  const int ptn,
		  std::vector<STet>& tet,
		  std::vector<CPoint3D>& node ){

	assert( elared.size() == 3 );

	const int iold0 = elared.e[0].first;
	const int iold1 = elared.e[1].first;
	const int iold2 = elared.e[2].first;

	const STet old0 = tet[ iold0 ];
	const STet old1 = tet[ iold1 ];
	const STet old2 = tet[ iold2 ];

    const unsigned int noel0d = tetRel[elared.e[0].second][0];
    const unsigned int noel0u = tetRel[elared.e[0].second][1];
    const unsigned int noel1d = tetRel[elared.e[1].second][0];
    const unsigned int noel1u = tetRel[elared.e[1].second][1];
    const unsigned int noel2d = tetRel[elared.e[2].second][0];
    const unsigned int noel2u = tetRel[elared.e[2].second][1];

    const unsigned int nod = tet[iold0].v[noel0d];
    const unsigned int nou = tet[iold0].v[noel0u];

	assert( nod == tet[iold1].v[noel1d] );
	assert( nou == tet[iold1].v[noel1u] );
	assert( nod == tet[iold2].v[noel2d] );
	assert( nou == tet[iold2].v[noel2u] );

	const int inew0 = iold0;
	const int inew1 = iold1;

	STet& tet_u = tet[inew0];
	STet& tet_d = tet[inew1];

	{
		tet_u.v[0] = nou;
		tet_u.v[1] = elared.n[0];
		tet_u.v[2] = elared.n[2];
		tet_u.v[3] = elared.n[1];

		tet_u.s[0] = inew1;
		tet_u.s[1] = old1.s[noel1d];
		tet_u.s[2] = old0.s[noel0d];
		tet_u.s[3] = old2.s[noel2d];

		tet_u.g[0] = -2;
		tet_u.g[1] = old1.g[noel1d];
		tet_u.g[2] = old0.g[noel0d];
		tet_u.g[3] = old2.g[noel2d];

		tet_u.f[0] = 0;
		if( old1.g[noel1d] == -2 ){
			const unsigned int* rel1 = tetRel[ old1.f[noel1d] ];
			tet_u.f[1] = noel2Rel[ rel1[noel1u]*4+rel1[noel1d] ];
			tet[ old1.s[noel1d] ].s[ rel1[noel1d] ] = inew0;
			tet[ old1.s[noel1d] ].f[ rel1[noel1d] ] = invTetRel[tet_u.f[1]];
		}
		if( old0.g[noel0d] == -2 ){
			const unsigned int* rel1 = tetRel[ old0.f[noel0d] ];
			tet_u.f[2] = noel2Rel[ rel1[noel0u]*4+rel1[ tetRel[elared.e[0].second][3] ] ];
			tet[ old0.s[noel0d] ].s[ rel1[noel0d] ] = inew0;
			tet[ old0.s[noel0d] ].f[ rel1[noel0d] ] = invTetRel[tet_u.f[2]];
		}
		if( old2.g[noel2d] == -2 ){
			const unsigned int* rel1 = tetRel[ old2.f[noel2d] ];
			tet_u.f[3] = noel2Rel[ rel1[noel2u]*4+rel1[ tetRel[elared.e[2].second][2] ] ];
			tet[ old2.s[noel2d] ].s[ rel1[noel2d] ] = inew0;
			tet[ old2.s[noel2d] ].f[ rel1[noel2d] ] = invTetRel[tet_u.f[3]];
		}
	}

	{
		tet_d.v[0] = nod;
		tet_d.v[1] = elared.n[0];
		tet_d.v[2] = elared.n[1];
		tet_d.v[3] = elared.n[2];

		tet_d.s[0] = inew0;
		tet_d.s[1] = old1.s[noel1u];
		tet_d.s[2] = old2.s[noel2u];
		tet_d.s[3] = old0.s[noel0u];

		tet_d.g[0] = -2;
		tet_d.g[1] = old1.g[noel1u];
		tet_d.g[2] = old2.g[noel2u];
		tet_d.g[3] = old0.g[noel0u];

		tet_d.f[0] = 0;
		if( old1.g[noel1u] == -2 ){
			const unsigned int* rel1 = tetRel[ old1.f[noel1u] ];
			tet_d.f[1] = noel2Rel[ rel1[noel1d]*4+rel1[noel1u] ];
			tet[ old1.s[noel1u] ].s[ rel1[noel1u] ] = inew1;
			tet[ old1.s[noel1u] ].f[ rel1[noel1u] ] = invTetRel[tet_d.f[1]];
		}
		if( old2.g[noel2u] == -2 ){
			const unsigned int* rel1 = tetRel[ old2.f[noel2u] ];
			tet_d.f[2] = noel2Rel[ rel1[noel2d]*4+rel1[ tetRel[elared.e[2].second][2] ] ];
			tet[ old2.s[noel2u] ].s[ rel1[noel2u] ] = inew1;
			tet[ old2.s[noel2u] ].f[ rel1[noel2u] ] = invTetRel[tet_d.f[2]];
		}
		if( old0.g[noel0u] == -2 ){
			const unsigned int* rel1 = tetRel[ old0.f[noel0u] ];
			tet_d.f[3] = noel2Rel[ rel1[noel0d]*4+rel1[ tetRel[elared.e[0].second][3] ] ];
			tet[ old0.s[noel0u] ].s[ rel1[noel0u] ] = inew1;
			tet[ old0.s[noel0u] ].f[ rel1[noel0u] ] = invTetRel[tet_d.f[3]];
		}
	}

	node[nod].e = inew1;	node[nod].poel = 0;
	node[nou].e = inew0;	node[nou].poel = 0;
	node[ elared.n[0] ].e = inew0;	node[ elared.n[0] ].poel = 1;
	node[ elared.n[1] ].e = inew0;	node[ elared.n[1] ].poel = 3;
	node[ elared.n[2] ].e = inew0;	node[ elared.n[2] ].poel = 2;

	const int idist = iold2;
	{
		const int iend0 = tet.size()-1;
		if( idist != iend0 ){
			tet[idist] = tet[iend0];
			STet& dist = tet[idist];
			if( dist.g[0] == -2 ){  tet[ dist.s[0] ].s[ tetRel[dist.f[0]][0] ] = idist;  }
			if( dist.g[1] == -2 ){  tet[ dist.s[1] ].s[ tetRel[dist.f[1]][1] ] = idist;  }
			if( dist.g[2] == -2 ){  tet[ dist.s[2] ].s[ tetRel[dist.f[2]][2] ] = idist;  }
			if( dist.g[3] == -2 ){  tet[ dist.s[3] ].s[ tetRel[dist.f[3]][3] ] = idist;  }
			node[ dist.v[0] ].e = idist;	node[ dist.v[0] ].poel = 0;
			node[ dist.v[1] ].e = idist;	node[ dist.v[1] ].poel = 1;
			node[ dist.v[2] ].e = idist;	node[ dist.v[2] ].poel = 2;
			node[ dist.v[3] ].e = idist;	node[ dist.v[3] ].poel = 3;
		}
	}
	tet.resize( tet.size()-1 );

	return true;
}


bool Swap5Elared(const ElemAroundEdge& elared,
				 const int ptn,
				 std::vector<STet>& tet,
				 std::vector<CPoint3D>& node){

	assert( elared.size() == 5 );
	assert( ptn >= 0 && ptn < 5 );

	STet old_tet[5];
	old_tet[0] = tet[ elared.e[0].first ];
	old_tet[1] = tet[ elared.e[1].first ];
	old_tet[2] = tet[ elared.e[2].first ];
	old_tet[3] = tet[ elared.e[3].first ];
	old_tet[4] = tet[ elared.e[4].first ];

	const int inew_tet[nTriInSwap5][2] = {
		{ elared.e[0].first, elared.e[1].first },
		{ elared.e[2].first, elared.e[3].first },
		{ elared.e[4].first, tet.size()        }
	};
/*
	std::cout << inew_tet[0][0] << " " << inew_tet[0][1] << std::endl;
	std::cout << inew_tet[1][0] << " " << inew_tet[1][1] << std::endl;
	std::cout << inew_tet[2][0] << " " << inew_tet[2][1] << std::endl;
*/
//	std::cout << " Pattern 5 " << ptn << std::endl;

	tet.resize( tet.size()+1 );

	const int isup_swap = swap2SupSwapRot[ptn][0];
	const unsigned int* rot = indexRot5[ swap2SupSwapRot[ptn][1] ];

    for(unsigned int itri=0;itri<nTriInSwap5;itri++){
		tet[ inew_tet[itri][0] ].v[0] = elared.nod;
		tet[ inew_tet[itri][0] ].v[1] = elared.n[ rot[sup2Noel5[isup_swap][itri][0]] ];
		tet[ inew_tet[itri][0] ].v[2] = elared.n[ rot[sup2Noel5[isup_swap][itri][1]] ];
		tet[ inew_tet[itri][0] ].v[3] = elared.n[ rot[sup2Noel5[isup_swap][itri][2]] ];

		tet[ inew_tet[itri][1] ].v[0] = elared.nou;
		tet[ inew_tet[itri][1] ].v[1] = elared.n[ rot[sup2Noel5[isup_swap][itri][0]] ];
		tet[ inew_tet[itri][1] ].v[2] = elared.n[ rot[sup2Noel5[isup_swap][itri][2]] ];
		tet[ inew_tet[itri][1] ].v[3] = elared.n[ rot[sup2Noel5[isup_swap][itri][1]] ];
	}

	////////////////
	const int onetetsurpo[5][2] = {
		{ 0, 1 },
		{ 0, 2 },
		{ 0, 3 },
		{ 1, 3 },
		{ 2, 3 }
	};
	node[ elared.nod ].e = inew_tet[0][0];	node[ elared.nod ].poel = 0;
	node[ elared.nou ].e = inew_tet[0][1];	node[ elared.nou ].poel = 0;
	for(int ipo=0;ipo<5;ipo++){
		node[ elared.n[ rot[ipo] ] ].e = inew_tet[ onetetsurpo[ipo][0] ][0];
		node[ elared.n[ rot[ipo] ] ].poel = onetetsurpo[ipo][1];
	}

	////////////////
	const int outer[5][5] = {
		{ 0, 3, 2, 3, 3 },
		{ 0, 1, 1, 1, 0 },
		{ 1, 1, 1, 1, 0 },
		{ 2, 1, 1, 1, 0 },
		{ 2, 2, 3, 2, 2 }
	};
	for(int isout=0;isout<5;isout++){
		const int ilout = rot[isout];
		const int* souter = outer[isout];
		const int old_noelu = tetRel[elared.e[ilout].second][1];
		const int old_noeld = tetRel[elared.e[ilout].second][0];

		// downer tri
		const int inew_tetd = inew_tet[souter[0]][0];
		tet[inew_tetd].s[souter[1]] = old_tet[ilout].s[old_noelu];
		tet[inew_tetd].g[souter[1]] = old_tet[ilout].g[old_noelu];
		if( old_tet[ilout].g[old_noelu] == -2 ){
			assert( old_tet[ilout].s[old_noelu] >= 0 && old_tet[ilout].s[old_noelu] < tet.size() );
			const unsigned int* rel1 = tetRel[ old_tet[ilout].f[old_noelu] ];
			tet[inew_tetd].f[souter[1]] = noel2Rel[ rel1[old_noeld]*4 + rel1[tetRel[elared.e[ilout].second][souter[3]]] ];
			tet[old_tet[ilout].s[old_noelu]].s[rel1[old_noelu]] = inew_tetd;
			tet[old_tet[ilout].s[old_noelu]].f[rel1[old_noelu]] = invTetRel[ tet[inew_tetd].f[souter[1]] ];
		}

		// upper tri
		const int inew_tetu = inew_tet[souter[0]][1];
		tet[inew_tetu].s[souter[2]] = old_tet[ilout].s[old_noeld];
		tet[inew_tetu].g[souter[2]] = old_tet[ilout].g[old_noeld];
		if( old_tet[ilout].g[old_noeld] == -2 ){
			assert( old_tet[ilout].s[old_noeld] >= 0 && old_tet[ilout].s[old_noeld] < tet.size() );
			const unsigned int* rel1 = tetRel[ old_tet[ilout].f[old_noeld] ];
			tet[inew_tetu].f[souter[2]] = noel2Rel[ rel1[old_noelu]*4 + rel1[tetRel[elared.e[ilout].second][souter[4]]] ];
			tet[old_tet[ilout].s[old_noeld]].s[rel1[old_noeld]] = inew_tetu;
			tet[old_tet[ilout].s[old_noeld]].f[rel1[old_noeld]] = invTetRel[ tet[inew_tetu].f[souter[2]] ];
		}
	}

	const int inner[4][6] = {
		{ 0, 2, 3, 1, 0, 0 },
		{ 1, 2, 3, 2, 0, 0 },
		{ 1, 3, 2, 0, 0, 0 },
		{ 2, 3, 2, 1, 0, 0 }
	};

	for(int iin=0;iin<4;iin++){
		const int* linner = inner[iin];
		const int inew_tetd = inew_tet[linner[0]][0];
		const int inew_tetu = inew_tet[linner[0]][1];

		tet[inew_tetd].s[linner[1]] = inew_tet[linner[3]][0];
		tet[inew_tetd].g[linner[1]] = -2;
		tet[inew_tetd].f[linner[1]] = linner[4];
		
		tet[inew_tetu].s[linner[2]] = inew_tet[linner[3]][1];
		tet[inew_tetu].g[linner[2]] = -2;
		tet[inew_tetu].f[linner[2]] = linner[5];
	}

    for(unsigned int itri=0;itri<nTriInSwap5;itri++){
		const int inew_tetd = inew_tet[itri][0];
		const int inew_tetu = inew_tet[itri][1];

		tet[inew_tetd].s[0] = inew_tetu;
		tet[inew_tetd].g[0] = -2;
		tet[inew_tetd].f[0] = 0;

		tet[inew_tetu].s[0] = inew_tetd;
		tet[inew_tetu].g[0] = -2;
		tet[inew_tetu].f[0] = 0;
	}

	return true;
}

bool Swap4Elared(const ElemAroundEdge& elared,
		  const int ptn,
		  std::vector<STet>& tet,
		  std::vector<CPoint3D>& node )
{
	assert( elared.size() == 4 );

	const int iold0 = elared.e[0].first;
	const int iold1 = elared.e[1].first;
	const int iold2 = elared.e[2].first;
	const int iold3 = elared.e[3].first;

	const STet old0 = tet[ iold0 ];
	const STet old1 = tet[ iold1 ];
	const STet old2 = tet[ iold2 ];
	const STet old3 = tet[ iold3 ];

    const unsigned int noel0d = tetRel[elared.e[0].second][0];
    const unsigned int noel0u = tetRel[elared.e[0].second][1];
    const unsigned int noel1d = tetRel[elared.e[1].second][0];
    const unsigned int noel1u = tetRel[elared.e[1].second][1];
    const unsigned int noel2d = tetRel[elared.e[2].second][0];
    const unsigned int noel2u = tetRel[elared.e[2].second][1];
    const unsigned int noel3d = tetRel[elared.e[3].second][0];
    const unsigned int noel3u = tetRel[elared.e[3].second][1];

    const unsigned int nod = elared.nod;
    const unsigned int nou = elared.nou;

	assert( nod == tet[iold0].v[noel0d] );
	assert( nou == tet[iold0].v[noel0u] );
	assert( nod == tet[iold1].v[noel1d] );
	assert( nou == tet[iold1].v[noel1u] );
	assert( nod == tet[iold2].v[noel2d] );
	assert( nou == tet[iold2].v[noel2u] );
	assert( nod == tet[iold3].v[noel3d] );
	assert( nou == tet[iold3].v[noel3u] );

	const int inew_u0 = iold0;
	const int inew_d0 = iold1;
	const int inew_u1 = iold2;
	const int inew_d1 = iold3;
/*
	std::cout << iold0 << " " << tet[iold0].s[noel0d] << " " << tet[iold0].s[noel0u] << std::endl;
	std::cout << iold1 << " " << tet[iold1].s[noel1d] << " " << tet[iold1].s[noel1u] << std::endl;
	std::cout << iold2 << " " << tet[iold2].s[noel2d] << " " << tet[iold2].s[noel2u] << std::endl;
	std::cout << iold3 << " " << tet[iold3].s[noel3d] << " " << tet[iold3].s[noel3u] << std::endl;
	std::cout << " pattern " << ptn << std::endl;
*/
	if( ptn == 0 ){
		{
			STet& new_tet = tet[inew_u0];

			new_tet.v[0]=nou;			
			new_tet.v[1]=elared.n[0];	
			new_tet.v[2]=elared.n[2];	
			new_tet.v[3]=elared.n[1];

			new_tet.s[0]=inew_d0;
			new_tet.s[1]=old1.s[noel1d];
			new_tet.s[2]=old0.s[noel0d];
			new_tet.s[3]=inew_u1;

			new_tet.g[0]=-2;
			new_tet.g[1]=old1.g[noel1d];
			new_tet.g[2]=old0.g[noel0d];
			new_tet.g[3]=-2;

			new_tet.f[0] = 0;
			if( old1.g[noel1d] == -2 ){
				const unsigned int* rel1 = tetRel[ old1.f[noel1d] ];
				new_tet.f[1] = noel2Rel[ rel1[noel1u]*4+rel1[noel1d] ];
				tet[ old1.s[noel1d] ].s[ rel1[noel1d] ] = inew_u0;
				tet[ old1.s[noel1d] ].f[ rel1[noel1d] ] = invTetRel[new_tet.f[1]];
			}
			if( old0.g[noel0d] == -2 ){
				const unsigned int* rel1 = tetRel[ old0.f[noel0d] ];
				new_tet.f[2] = noel2Rel[ rel1[noel0u]*4+rel1[ tetRel[elared.e[0].second][3] ] ];
				tet[ old0.s[noel0d] ].s[ rel1[noel0d] ] = inew_u0;
				tet[ old0.s[noel0d] ].f[ rel1[noel0d] ] = invTetRel[new_tet.f[2]];
			}
			new_tet.f[3] = 0;
		}

		{
			STet& new_tet = tet[inew_d0];

			new_tet.v[0]=nod;			
			new_tet.v[1]=elared.n[0];	
			new_tet.v[2]=elared.n[1];	
			new_tet.v[3]=elared.n[2];

			new_tet.s[0]=inew_u0;
			new_tet.s[1]=old1.s[noel1u];
			new_tet.s[2]=inew_d1;
			new_tet.s[3]=old0.s[noel0u];

			new_tet.g[0]=-2;
			new_tet.g[1]=old1.g[noel1u];
			new_tet.g[2]=-2;
			new_tet.g[3]=old0.g[noel0u];

			new_tet.f[0] = 0;
			if( old1.g[noel1u] == -2 ){
				const unsigned int* rel1 = tetRel[ old1.f[noel1u] ];
				new_tet.f[1] = noel2Rel[ rel1[noel1d]*4+rel1[noel1u] ];
				tet[ old1.s[noel1u] ].s[ rel1[noel1u] ] = inew_d0;
				tet[ old1.s[noel1u] ].f[ rel1[noel1u] ] = invTetRel[new_tet.f[1]];
			}
			new_tet.f[2] = 0;
			if( old0.g[noel0u] == -2 ){
				const unsigned int* rel1 = tetRel[ old0.f[noel0u] ];
				new_tet.f[3] = noel2Rel[ rel1[noel0d]*4+rel1[ tetRel[elared.e[0].second][3] ] ];
				tet[ old0.s[noel0u] ].s[ rel1[noel0u] ] = inew_d0;
				tet[ old0.s[noel0u] ].f[ rel1[noel0u] ] = invTetRel[new_tet.f[3]];
			}
		}

		{
			STet& new_tet = tet[inew_u1];

			new_tet.v[0]=nou;			
			new_tet.v[1]=elared.n[0];	
			new_tet.v[2]=elared.n[3];	
			new_tet.v[3]=elared.n[2];

			new_tet.s[0]=inew_d1;
			new_tet.s[1]=old2.s[noel2d];
			new_tet.s[2]=inew_u0;
			new_tet.s[3]=old3.s[noel3d];

			new_tet.g[0]=-2;
			new_tet.g[1]=old2.g[noel2d];
			new_tet.g[2]=-2;
			new_tet.g[3]=old3.g[noel3d];

			new_tet.f[0] = 0;
			if( old2.g[noel2d] == -2 ){
				const unsigned int* rel1 = tetRel[ old2.f[noel2d] ];
				new_tet.f[1] = noel2Rel[ rel1[noel2u]*4+rel1[noel2d] ];
				tet[ old2.s[noel2d] ].s[ rel1[noel2d] ] = inew_u1;
				tet[ old2.s[noel2d] ].f[ rel1[noel2d] ] = invTetRel[new_tet.f[1]];
			}
			new_tet.f[2] = 0;
			if( old3.g[noel3d] == -2 ){
				const unsigned int* rel1 = tetRel[ old3.f[noel3d] ];
				new_tet.f[3] = noel2Rel[ rel1[noel3u]*4+rel1[ tetRel[elared.e[3].second][2] ] ];
				tet[ old3.s[noel3d] ].s[ rel1[noel3d] ] = inew_u1;
				tet[ old3.s[noel3d] ].f[ rel1[noel3d] ] = invTetRel[new_tet.f[3]];
			}
		}

		{
			STet& new_tet = tet[inew_d1];

			new_tet.v[0]=nod;			
			new_tet.v[1]=elared.n[0];	
			new_tet.v[2]=elared.n[2];	
			new_tet.v[3]=elared.n[3];

			new_tet.s[0]=inew_u1;
			new_tet.s[1]=old2.s[noel2u];
			new_tet.s[2]=old3.s[noel3u];
			new_tet.s[3]=inew_d0;

			new_tet.g[0]=-2;
			new_tet.g[1]=old2.g[noel2u];
			new_tet.g[2]=old3.g[noel3u];
			new_tet.g[3]=-2;

			new_tet.f[0] = 0;
			if( old2.g[noel2u] == -2 ){
				const unsigned int* rel1 = tetRel[ old2.f[noel2u] ];
				new_tet.f[1] = noel2Rel[ rel1[noel2d]*4+rel1[noel2u] ];
				tet[ old2.s[noel2u] ].s[ rel1[noel2u] ] = inew_d1;
				tet[ old2.s[noel2u] ].f[ rel1[noel2u] ] = invTetRel[new_tet.f[1]];
			}
			if( old3.g[noel3u] == -2 ){
				const unsigned int* rel1 = tetRel[ old3.f[noel3u] ];
				new_tet.f[2] = noel2Rel[ rel1[noel3d]*4+rel1[ tetRel[elared.e[3].second][2] ] ];
				tet[ old3.s[noel3u] ].s[ rel1[noel3u] ] = inew_d1;
				tet[ old3.s[noel3u] ].f[ rel1[noel3u] ] = invTetRel[new_tet.f[2]];
			}
			new_tet.f[3] = 0;
		}
		node[nod].e = inew_d0;	node[nod].poel = 0;
		node[nou].e = inew_u0;	node[nou].poel = 0;
		node[ elared.n[0] ].e = inew_u0;	node[ elared.n[0] ].poel = 1;
		node[ elared.n[1] ].e = inew_u0;	node[ elared.n[1] ].poel = 3;
		node[ elared.n[2] ].e = inew_u0;	node[ elared.n[2] ].poel = 2;
		node[ elared.n[3] ].e = inew_u1;	node[ elared.n[3] ].poel = 2;
	}
	else if( ptn == 1 ){
		{
			STet& new_tet = tet[inew_u0];

			new_tet.v[0]=nou;			
			new_tet.v[1]=elared.n[0];	
			new_tet.v[2]=elared.n[3];	
			new_tet.v[3]=elared.n[1];

			new_tet.s[0]=inew_d0;
			new_tet.s[1]=inew_u1;
			new_tet.s[2]=old0.s[noel0d];
			new_tet.s[3]=old3.s[noel3d];

			new_tet.g[0]=-2;
			new_tet.g[1]=-2;
			new_tet.g[2]=old0.g[noel0d];
			new_tet.g[3]=old3.g[noel3d];

			new_tet.f[0] = 0;
			new_tet.f[1] = 2;
			if( old0.g[noel0d] == -2 ){
				const unsigned int* rel1 = tetRel[ old0.f[noel0d] ];
				new_tet.f[2] = noel2Rel[ rel1[noel0u]*4+rel1[ tetRel[elared.e[0].second][3] ] ];
				tet[ old0.s[noel0d] ].s[ rel1[noel0d] ] = inew_u0;
				tet[ old0.s[noel0d] ].f[ rel1[noel0d] ] = invTetRel[new_tet.f[2]];
			}
			if( old3.g[noel3d] == -2 ){
				const unsigned int* rel1 = tetRel[ old3.f[noel3d] ];
				new_tet.f[3] = noel2Rel[ rel1[noel3u]*4+rel1[ tetRel[elared.e[3].second][2] ] ];
				tet[ old3.s[noel3d] ].s[ rel1[noel3d] ] = inew_u0;
				tet[ old3.s[noel3d] ].f[ rel1[noel3d] ] = invTetRel[new_tet.f[3]];
			}
		}

		{
			STet& new_tet = tet[inew_d0];

			new_tet.v[0]=nod;			
			new_tet.v[1]=elared.n[0];	
			new_tet.v[2]=elared.n[1];	
			new_tet.v[3]=elared.n[3];

			new_tet.s[0]=inew_u0;
			new_tet.s[1]=inew_d1;
			new_tet.s[2]=old3.s[noel3u];
			new_tet.s[3]=old0.s[noel0u];

			new_tet.g[0]=-2;
			new_tet.g[1]=-2;
			new_tet.g[2]=old3.g[noel3u];
			new_tet.g[3]=old0.g[noel0u];

			new_tet.f[0] = 0;
			new_tet.f[1] = 1;
			if( old3.g[noel3u] == -2 ){
				const unsigned int* rel1 = tetRel[ old3.f[noel3u] ];
				new_tet.f[2] = noel2Rel[ rel1[noel3d]*4+rel1[ tetRel[elared.e[3].second][2] ] ];
				tet[ old3.s[noel3u] ].s[ rel1[noel3u] ] = inew_d0;
				tet[ old3.s[noel3u] ].f[ rel1[noel3u] ] = invTetRel[new_tet.f[2]];
			}
			if( old0.g[noel0u] == -2 ){
				const unsigned int* rel1 = tetRel[ old0.f[noel0u] ];
				new_tet.f[3] = noel2Rel[ rel1[noel0d]*4+rel1[ tetRel[elared.e[0].second][3] ] ];
				tet[ old0.s[noel0u] ].s[ rel1[noel0u] ] = inew_d0;
				tet[ old0.s[noel0u] ].f[ rel1[noel0u] ] = invTetRel[new_tet.f[3]];
			}
		}

		{
			STet& new_tet = tet[inew_u1];

			new_tet.v[0]=nou;			
			new_tet.v[1]=elared.n[3];	
			new_tet.v[2]=elared.n[2];	
			new_tet.v[3]=elared.n[1];

			new_tet.s[0]=inew_d1;
			new_tet.s[1]=old1.s[noel1d];
			new_tet.s[2]=inew_u0;
			new_tet.s[3]=old2.s[noel2d];

			new_tet.g[0]=-2;
			new_tet.g[1]=old1.g[noel1d];
			new_tet.g[2]=-2;
			new_tet.g[3]=old2.g[noel2d];

			new_tet.f[0] = 0;
			if( old1.g[noel1d] == -2 ){
				const unsigned int* rel1 = tetRel[ old1.f[noel1d] ];
				new_tet.f[1] = noel2Rel[ rel1[noel1u]*4+rel1[noel1d] ];
				tet[ old1.s[noel1d] ].s[ rel1[noel1d] ] = inew_u1;
				tet[ old1.s[noel1d] ].f[ rel1[noel1d] ] = invTetRel[new_tet.f[1]];
			}
			new_tet.f[2] = 2;
			if( old2.g[noel2d] == -2 ){
				const unsigned int* rel1 = tetRel[ old2.f[noel2d] ];
				new_tet.f[3] = noel2Rel[ rel1[noel2u]*4+rel1[ tetRel[elared.e[2].second][2] ] ];
				tet[ old2.s[noel2d] ].s[ rel1[noel2d] ] = inew_u1;
				tet[ old2.s[noel2d] ].f[ rel1[noel2d] ] = invTetRel[new_tet.f[3]];
			}
		}

		{
			STet& new_tet = tet[inew_d1];

			new_tet.v[0]=nod;			
			new_tet.v[1]=elared.n[3];	
			new_tet.v[2]=elared.n[1];	
			new_tet.v[3]=elared.n[2];

			new_tet.s[0]=inew_u1;
			new_tet.s[1]=old1.s[noel1u];
			new_tet.s[2]=old2.s[noel2u];
			new_tet.s[3]=inew_d0;

			new_tet.g[0]=-2;
			new_tet.g[1]=old1.g[noel1u];
			new_tet.g[2]=old2.g[noel2u];
			new_tet.g[3]=-2;

			new_tet.f[0] = 0;
			if( old1.g[noel1u] == -2 ){
				const unsigned int* rel1 = tetRel[ old1.f[noel1u] ];
				new_tet.f[1] = noel2Rel[ rel1[noel1d]*4+rel1[noel1u] ];
				tet[ old1.s[noel1u] ].s[ rel1[noel1u] ] = inew_d1;
				tet[ old1.s[noel1u] ].f[ rel1[noel1u] ] = invTetRel[new_tet.f[1]];
			}
			if( old2.g[noel2u] == -2 ){
				const unsigned int* rel1 = tetRel[ old2.f[noel2u] ];
				new_tet.f[2] = noel2Rel[ rel1[noel2d]*4+rel1[ tetRel[elared.e[2].second][2] ] ];
				tet[ old2.s[noel2u] ].s[ rel1[noel2u] ] = inew_d1;
				tet[ old2.s[noel2u] ].f[ rel1[noel2u] ] = invTetRel[new_tet.f[2]];
			}
			new_tet.f[3] = 1;
		}
		node[nod].e = inew_d0;	node[nod].poel = 0;
		node[nou].e = inew_u0;	node[nou].poel = 0;
		node[ elared.n[0] ].e = inew_u0;	node[ elared.n[0] ].poel = 1;
		node[ elared.n[1] ].e = inew_u0;	node[ elared.n[1] ].poel = 3;
		node[ elared.n[2] ].e = inew_u1;	node[ elared.n[2] ].poel = 2;
		node[ elared.n[3] ].e = inew_u0;	node[ elared.n[3] ].poel = 2;
		return false;
	}
	else{
		return false;
	}

	return true;
}


bool GetEdgeSwapPtnCrt5Elared(const ElemAroundEdge& elared,
				int& ptn,
				double& min_max_crt,
				const std::vector<CPoint3D>& node )
{
	assert( elared.size() == 5 );

	double crt1,crt2;
	std::map<double,int> map_crt_tri;
    unsigned int false_tri_size = 0;
	int false_tri[nTri5];
	std::pair<double,int> pair_di;
    for(unsigned int itri=0;itri<nTri5;itri++){
		crt1 = Criterion_PLJ(node[elared.nod],
			node[elared.n[ tri5[itri][0] ]],
			node[elared.n[ tri5[itri][1] ]],
			node[elared.n[ tri5[itri][2] ]] );
		if( crt1<0.0 || crt1 > ILL_CRT*0.9 ){
			false_tri[false_tri_size] = itri;
			false_tri_size++;
			continue;
		}
		crt2 = Criterion_PLJ(node[elared.nou],
			node[elared.n[ tri5[itri][2] ]],
			node[elared.n[ tri5[itri][1] ]],
			node[elared.n[ tri5[itri][0] ]] );
		if( crt2<0.0 || crt2 > ILL_CRT*0.9  ){
			false_tri[false_tri_size] = itri;
			false_tri_size++;
			continue;
		}
		crt1 = ( crt1 > crt2 ) ? crt1 : crt2;
		assert( crt1 > 0.0 && crt1 < ILL_CRT*0.9 );
		pair_di.first = -1.0*crt1;
		pair_di.second = itri;
		map_crt_tri.insert( pair_di );
	}
	if( map_crt_tri.size() < nTriInSwap5 ){
		ptn = -1;
		min_max_crt = ILL_CRT;
		return true;
	}

	int swap_flag[nSwap5] = { 1, 1, 1, 1, 1 };
    for(unsigned int itri=0;itri<false_tri_size;itri++){
        for(unsigned int itriswap=tri2Swap5Index[ false_tri[itri] ];itriswap<tri2Swap5Index[ false_tri[itri]+1];itriswap++){
			swap_flag[ tri2Swap5[itriswap] ] = 0;
		}
	}

	std::set<int> swap_alive;
    for(unsigned int iswap=0;iswap<nSwap5;iswap++){
		if( swap_flag[iswap] == 1 ){
			swap_alive.insert(iswap);
		}
	}
	if( swap_alive.empty() ){
		ptn = -1;
		min_max_crt = ILL_CRT;
		return true;
	}
	
	int iswap_ptn = -1;
	double max_crt = -ILL_CRT;
	for(std::map<double,int>::const_iterator itr_map_di=map_crt_tri.begin();itr_map_di!=map_crt_tri.end();itr_map_di++){
		if( swap_alive.size() == 0 ) break;
		max_crt = itr_map_di->first;
		assert( max_crt < 0.0 && max_crt > -ILL_CRT*0.9 );
		iswap_ptn = *(swap_alive.begin());
		if( swap_alive.size() == 1 ) break;
		const int itri0 = itr_map_di->second;
        for(unsigned int itriswap=tri2Swap5Index[itri0];itriswap<tri2Swap5Index[itri0+1];itriswap++){
			swap_alive.erase( tri2Swap5[itriswap] );
		}
	}
/*
	const int isup_swap = swap2SupSwapRot[iswap_ptn][0];
	const int irot = swap2SupSwapRot[iswap_ptn][1];
	for(int itri=0;itri<nTriInSwap5;itri++){
		crt1 = Criterion_PLG(node[elared.nod],
			node[elared.n[ rotIndex5[irot][ sup2Noel5[isup_swap][itri][0] ] ]],
			node[elared.n[ rotIndex5[irot][ sup2Noel5[isup_swap][itri][1] ] ]],
			node[elared.n[ rotIndex5[irot][ sup2Noel5[isup_swap][itri][2] ] ]] );
		crt2 = Criterion_PLG(node[elared.nou],
			node[elared.n[ rotIndex5[irot][ sup2Noel5[isup_swap][itri][2] ] ]],
			node[elared.n[ rotIndex5[irot][ sup2Noel5[isup_swap][itri][1] ] ]],
			node[elared.n[ rotIndex5[irot][ sup2Noel5[isup_swap][itri][0] ] ]] );
		std::cout << " kkk " << crt1 << " " << crt2 << std::endl;
	}
*/
	ptn = iswap_ptn;
	min_max_crt = -max_crt;

	assert( ( ptn >= 0 && ptn < 5 && min_max_crt > 0.0 && min_max_crt < ILL_CRT-0.1 ) 
		|| ( ptn == -1 && min_max_crt > ILL_CRT*0.9 ) );
	assert( min_max_crt > 0.0 );

	return true;
}


bool Msh::MakeElemAroundPoint( ElemAroundPoint& elarpo,
						const unsigned int itet0, 
						const unsigned int inoel0, 
						const std::vector<STet>& tet ){
	assert( itet0 < tet.size() );
	assert( inoel0 < 4 );

	std::set< std::pair<int,unsigned int> > next;
	std::pair<int,unsigned int> pair_ic;

	elarpo.clear();

	const int cnt_point = tet[itet0].v[inoel0];

	elarpo.is_inner = true;

	pair_ic.first = itet0;
	pair_ic.second = inoel0;
	elarpo.e.insert( pair_ic );

	pair_ic.first = itet0;
	pair_ic.second = noelTetFace[inoel0][0];
	next.insert( pair_ic );

	pair_ic.first = itet0;
	pair_ic.second = noelTetFace[inoel0][1];
	next.insert( pair_ic );

	pair_ic.first = itet0;
	pair_ic.second = noelTetFace[inoel0][2];
	next.insert( pair_ic );

	for(;;){
		bool flg1 = true;
		for(;;){
			std::set< std::pair<int,unsigned int> >::iterator itr_set_ic = next.begin();
			if( itr_set_ic == next.end() ) break;
			const int icur_tet= itr_set_ic->first;
			const unsigned int icur_noel_adj = itr_set_ic->second;
			next.erase( itr_set_ic );
			if( tet[icur_tet].g[icur_noel_adj] != -2 ){
				elarpo.is_inner = false;
				continue;
			}
			const int iadj_tet = tet[icur_tet].s[icur_noel_adj];
			if( elarpo.e.find(iadj_tet) != elarpo.e.end() ) continue;
			flg1 = false;

			std::map<int,unsigned int>::const_iterator itr_map_ic = elarpo.e.find( icur_tet );
			assert( itr_map_ic != elarpo.e.end() );
			const unsigned int icur_noel_cnt = itr_map_ic->second;
            assert( (int)tet[icur_tet].v[icur_noel_cnt] == cnt_point );

			const unsigned int* rel_curadj = tetRel[ tet[icur_tet].f[icur_noel_adj] ];

			const unsigned int iadj_noel_cnt = rel_curadj[icur_noel_cnt];
            assert( (int)tet[iadj_tet].v[iadj_noel_cnt] == cnt_point );
			pair_ic.first = iadj_tet;
			pair_ic.second = iadj_noel_cnt;
			elarpo.e.insert( pair_ic );

			const unsigned int iadj_noel_adj = rel_curadj[icur_noel_adj];
            assert( (int)tet[iadj_tet].s[iadj_noel_adj] == icur_tet );
			const int irel0 = noel2Rel[ iadj_noel_cnt*4 + iadj_noel_adj ];

			assert( tetRel[ irel0 ][0] == iadj_noel_cnt );
			assert( tetRel[ irel0 ][1] == iadj_noel_adj );

			const int jnoel0 = tetRel[ irel0 ][2];
			pair_ic.first = iadj_tet;
			pair_ic.second = jnoel0;
			next.insert( pair_ic );


			const int jnoel1 = tetRel[ irel0 ][3];
			pair_ic.first = iadj_tet;
			pair_ic.second = jnoel1;
			next.insert( pair_ic );
		}
		if( flg1 ) break;
	}

	for(std::map<int,unsigned int>::const_iterator itr_map_ic = elarpo.e.begin();itr_map_ic!=elarpo.e.end();itr_map_ic++){
		const int itet1 = itr_map_ic->first;
		const int inoel1 = itr_map_ic->second;
        assert( (int)tet[itet1].v[inoel1] == cnt_point );
		if( tet[itet1].g[ noelTetFace[inoel1][0] ] == -2 ){
			assert( elarpo.e.find( tet[itet1].s[ noelTetFace[inoel1][0] ] ) != elarpo.e.end() );
		}
		if( tet[itet1].g[ noelTetFace[inoel1][1] ] == -2 ){
			assert( elarpo.e.find( tet[itet1].s[ noelTetFace[inoel1][1] ] ) != elarpo.e.end() );
		}
		if( tet[itet1].g[ noelTetFace[inoel1][2] ] == -2 ){
			assert( elarpo.e.find( tet[itet1].s[ noelTetFace[inoel1][2] ] ) != elarpo.e.end() );
		}
	}

	return true;
}

bool Msh::EdgeDel(const ElemAroundEdge& elared,
			 const ElemAroundPoint& elarpo,
			 std::vector<STet>& tet,
             std::vector<CPoint3D>& node)
{
	assert( elared.is_inner && elarpo.is_inner );
	assert( elarpo.size() > 0 );

    const unsigned int del_point = tet[elarpo.e.begin()->first].v[elarpo.e.begin()->second];
    unsigned int cnt_point = 0;
	if( del_point == elared.nod ){
		cnt_point = elared.nou;
	}
	else if( del_point == elared.nou ){
		cnt_point = elared.nod;
	}
	else{
		assert(0);
	}

//	std::cout << cnt_point << " " << del_point << " " << node.size()-1 << std::endl;

	for(std::map<int,unsigned int>::const_iterator itr_map_ic=elarpo.e.begin();itr_map_ic!=elarpo.e.end();itr_map_ic++){
		const int itet0 = itr_map_ic->first;
		const int inoel0 = itr_map_ic->second;
		assert( tet[itet0].v[inoel0] == del_point );
		tet[itet0].v[inoel0] = cnt_point;
	}

    for(unsigned int ielared=0;ielared<elared.size();ielared++){
        assert( elared.e[ielared].first >= 0 );
        const unsigned int iold_tet = (unsigned int)elared.e[ielared].first;
		const int iold_edge_rel = elared.e[ielared].second;
        const unsigned int iold_noel_d = tetRel[iold_edge_rel][0];
        const unsigned int iold_noel_u = tetRel[iold_edge_rel][1];
		assert( tet[iold_tet].v[iold_noel_d] == cnt_point );
		assert( tet[iold_tet].v[iold_noel_u] == cnt_point );
		assert( tet[iold_tet].g[iold_noel_u] == -2 || tet[iold_tet].g[iold_noel_d] == -2 );

//		std::cout << elared.n[ielared] << " " << iold_tet << " ";
		if( tet[iold_tet].g[iold_noel_u] == -2 ){
			const int iadj_d_tet = tet[iold_tet].s[iold_noel_u];
			const int iadj_d_noel = tetRel[ tet[iold_tet].f[iold_noel_u] ][iold_noel_u];
//			std::cout << iadj_d_tet << " " << iadj_d_noel << "   ";
			assert( tet[iadj_d_tet].s[iadj_d_noel] == iold_tet );
			tet[iadj_d_tet].s[iadj_d_noel] = tet[iold_tet].s[iold_noel_d];
			tet[iadj_d_tet].g[iadj_d_noel] = tet[iold_tet].g[iold_noel_d];
			if( tet[iold_tet].g[iold_noel_d] == -2 ){
				const int irel0 = tet[iadj_d_tet].f[iadj_d_noel];
				assert( irel0 >= 0 && irel0 < 12 );
				unsigned int inoel0 = tetRel[irel0][0];
				if( inoel0 == iold_noel_d ) inoel0 = iold_noel_u;
				else if( inoel0 == iold_noel_u ) inoel0 = iold_noel_d;
                ////////////////
                const int irel1 = tet[iold_tet].f[iold_noel_d];
                assert( irel1 >= 0 && irel1 < 12 );
				unsigned int inoel1 = tetRel[irel0][1];
				if( inoel1 == iold_noel_d ) inoel1 = iold_noel_u;
				else if( inoel1 == iold_noel_u ) inoel1 = iold_noel_d;
                ////////////////
				tet[iadj_d_tet].f[iadj_d_noel] 
					= noel2Rel[ tetRel[irel1][inoel0]*4 + tetRel[irel1][inoel1] ];
			}
			{
				 const int noel0 = tetRel[ elared.e[ielared].second ][3];
				 assert( elared.n[ielared] == tet[iold_tet].v[noel0] );
				 const int noel1 = tetRel[ tet[iold_tet].f[iold_noel_u] ][noel0];
				 assert( elared.n[ielared] == tet[iadj_d_tet].v[noel1] );
				 node[ elared.n[ielared] ].e = iadj_d_tet;
				 node[ elared.n[ielared] ].poel = noel1;
			}
			{
				const int noel0 = tetRel[ tet[iold_tet].f[iold_noel_u] ][iold_noel_d];
				assert( cnt_point == tet[iadj_d_tet].v[noel0] );
				node[ cnt_point ].e = iadj_d_tet;
				node[ cnt_point ].poel = noel0;
			}
		}
//		else{
//			std::cout << "outer   ";
//		}

		if( tet[iold_tet].g[iold_noel_d] == -2 ){
            const unsigned int iadj_u_tet = tet[iold_tet].s[iold_noel_d];
            const unsigned int iadj_u_noel = tetRel[ tet[iold_tet].f[iold_noel_d] ][iold_noel_d];
//			std::cout << iadj_u_tet << " " << iadj_u_noel << std::endl;
			assert( tet[iadj_u_tet].s[iadj_u_noel] == iold_tet );
			tet[iadj_u_tet].s[iadj_u_noel] = tet[iold_tet].s[iold_noel_u];
			tet[iadj_u_tet].g[iadj_u_noel] = tet[iold_tet].g[iold_noel_u];
			if( tet[iold_tet].g[iold_noel_u] == -2 ){
				const int irel0 = tet[iadj_u_tet].f[iadj_u_noel];
				assert( irel0 >= 0 && irel0 < 12 );
				unsigned int inoel0 = tetRel[irel0][0];
				if( inoel0 == iold_noel_d ) inoel0 = iold_noel_u;
				else if( inoel0 == iold_noel_u ) inoel0 = iold_noel_d;
				unsigned int inoel1 = tetRel[irel0][1];
				if( inoel1 == iold_noel_d ) inoel1 = iold_noel_u;
				else if( inoel1 == iold_noel_u ) inoel1 = iold_noel_d;
				const int irel1 = tet[iold_tet].f[iold_noel_u];
				assert( irel1 >= 0 && irel1 < 12 );
				tet[iadj_u_tet].f[iadj_u_noel] 
					= noel2Rel[ tetRel[irel1][inoel0]*4 + tetRel[irel1][inoel1] ];
			}
			{
				 const int noel0 = tetRel[ elared.e[ielared].second ][3];
				 assert( elared.n[ielared] == tet[iold_tet].v[noel0] );
				 const int noel1 = tetRel[ tet[iold_tet].f[iold_noel_d] ][noel0];
				 assert( elared.n[ielared] == tet[iadj_u_tet].v[noel1] );
				 node[ elared.n[ielared] ].e = iadj_u_tet;
				 node[ elared.n[ielared] ].poel = noel1;
			}
			{
				const int noel0 = tetRel[ tet[iold_tet].f[iold_noel_d] ][iold_noel_u];
				assert( cnt_point == tet[iadj_u_tet].v[noel0] );
				node[ cnt_point ].e = iadj_u_tet;
				node[ cnt_point ].poel = noel0;
			}
		}
//		else{
//			std::cout << "outer   " << std::endl;
//		}
	}

	{
		std::map<int,int> map_ii;
        for(unsigned int ielared=0;ielared<elared.size();ielared++){
			map_ii.insert( std::make_pair(-1*elared.e[ielared].first,ielared) );
		}
		for(std::map<int,int>::const_iterator itr_map_ii=map_ii.begin();itr_map_ii!=map_ii.end();itr_map_ii++){
			const int ielared0 = itr_map_ii->second;
			const int idist = elared.e[ielared0].first;
			const int iend0 = tet.size()-1;
			if(idist != iend0){
				tet[idist] = tet[iend0];
				STet& dist = tet[idist];
				if( dist.g[0] == -2 ){  tet[ dist.s[0] ].s[ tetRel[dist.f[0]][0] ] = idist;  }
				if( dist.g[1] == -2 ){  tet[ dist.s[1] ].s[ tetRel[dist.f[1]][1] ] = idist;  }
				if( dist.g[2] == -2 ){  tet[ dist.s[2] ].s[ tetRel[dist.f[2]][2] ] = idist;  }
				if( dist.g[3] == -2 ){  tet[ dist.s[3] ].s[ tetRel[dist.f[3]][3] ] = idist;  }
				node[ dist.v[0] ].e = idist;	node[ dist.v[0] ].poel = 0;
				node[ dist.v[1] ].e = idist;	node[ dist.v[1] ].poel = 1;
				node[ dist.v[2] ].e = idist;	node[ dist.v[2] ].poel = 2;
				node[ dist.v[3] ].e = idist;	node[ dist.v[3] ].poel = 3;
//				std::cout << idist << " " << iend0 << "-->" << dist.v[0] << " " << dist.v[1] << " " << dist.v[2] << " " << dist.v[3] << std::endl;
			}
			else{
//				std::cout << idist << " " << iend0 << "     dist = end " << std::endl;
			}
			tet.resize( tet.size()-1 );
		}
		map_ii.clear();
	}

	{
        const unsigned int idist = del_point;
        assert( node.size() > 0 );
        const unsigned int iend = node.size()-1;
		if( idist != iend ){
			node[idist] = node[iend];
//			CPoint3D& dist = node[idist];
			const int itet0 = node[idist].e;
			const int inoel0 = node[idist].poel;
//			std::cout << itet0 << " " << tet.size() << " " << inoel0 << std::endl;
			assert( itet0 >= 0 && (unsigned int)itet0 < tet.size() );
			assert( inoel0 >= 0 && (unsigned int)inoel0 < 4 );
			assert( tet[itet0].v[inoel0] == iend );
			ElemAroundPoint elar_delpo;
			Msh::MakeElemAroundPoint(elar_delpo,itet0,inoel0,tet);
			for(std::map<int,unsigned int>::const_iterator itr_map_ic=elar_delpo.e.begin();itr_map_ic!=elar_delpo.e.end();itr_map_ic++){
				assert( tet[ itr_map_ic->first ].v[ itr_map_ic->second ] == iend );
				tet[ itr_map_ic->first ].v[ itr_map_ic->second ] = idist;
			}
		}
		node.resize( node.size()-1 );
	}

	return true;
}

bool Msh::GetEdgeDelCrt(const ElemAroundEdge& elared,
				   const ElemAroundPoint& elarpo,
				   double& max_crt,
				   const std::vector<STet>& tet,
				   const std::vector<CPoint3D>& node ){

	if( !elared.is_inner || !elarpo.is_inner ){
		max_crt = ILL_CRT;
		return true;
	}
	
	assert( elarpo.size() > 0 );

    const unsigned int del_point = tet[elarpo.e.begin()->first].v[elarpo.e.begin()->second];
	assert( del_point >= 0 && (unsigned int)del_point < node.size() );
    unsigned int cnt_point = 0;
	if( del_point == elared.nod ){
		cnt_point = elared.nou;
	}
	else if( del_point == elared.nou ){
		cnt_point = elared.nod;
	}
	else{
		assert(0);
		std::cout << "Error!-->" << std::endl;
		max_crt = ILL_CRT;
		return false;
	}
	assert( cnt_point >= 0 && (unsigned int)cnt_point < node.size() );

	std::map<int,unsigned int> diff_ball = elarpo.e;
    for(unsigned int ielared=0;ielared<elared.size();ielared++){
		std::map<int,unsigned int>::iterator itr_map_ic = diff_ball.find( elared.e[ielared].first );
		assert( itr_map_ic != diff_ball.end() );
		diff_ball.erase( itr_map_ic );
	}
	assert( diff_ball.size() > 0 );

	double crt1;
	max_crt = -1.0;
	for(std::map<int,unsigned int>::const_iterator itr_map_ic=diff_ball.begin();itr_map_ic!=diff_ball.end();itr_map_ic++){
		const int itet0 = itr_map_ic->first;
		const int inoel0 = itr_map_ic->second;
		assert( tet[itet0].v[inoel0] == del_point );
		crt1 = Criterion_PLJ(node[cnt_point],
			node[ tet[itet0].v[ noelTetFace[inoel0][0] ] ],
			node[ tet[itet0].v[ noelTetFace[inoel0][1] ] ],
			node[ tet[itet0].v[ noelTetFace[inoel0][2] ] ] );
		if( crt1 < 0.0 ){
			max_crt = ILL_CRT;
			return true;
		}
		if( crt1 > max_crt ) max_crt = crt1;
	}
	assert( max_crt > 0.0 );

	return true;
}

bool Msh::GetAddPointEdgeCrt(const ElemAroundEdge& elared,
						const Com::CVector3D& add_point,
						double& max_crt,
						const std::vector<CPoint3D>& node)
{
	assert( elared.size() >= 3 );

	max_crt = -1.0;

	double crt1;
	const int inod = elared.nod;
	const int inou = elared.nou;
    for(unsigned int ielared=0;ielared<elared.size();ielared++){
		crt1 = Criterion_PLJ(node[inod].p, add_point, node[elared.n[ielared]].p, node[elared.n[ielared+1]].p);
		if( crt1 < 0.0 || crt1 > ILL_CRT*0.9 ){
			max_crt = ILL_CRT;
			return true;
		}
		max_crt = ( crt1 > max_crt ) ? crt1 : max_crt;
		crt1 = Criterion_PLJ(add_point, node[inou].p, node[elared.n[ielared]].p, node[elared.n[ielared+1]].p);
		if( crt1 < 0.0 || crt1 > ILL_CRT*0.9 ){
			max_crt = ILL_CRT;
			return true;
		}
		max_crt = ( crt1 > max_crt ) ? crt1 : max_crt;
	}
//	std::cout << "max_crt add_point edge " << max_crt << std::endl;
	assert( max_crt > 0.0 && max_crt < ILL_CRT*0.9 );

	return true;
}


bool Msh::AddPointTet_Elem(const unsigned int itet_ins,
					   const unsigned int ipo_ins,
					   std::vector<CPoint3D>& aPo,
					   std::vector<STet>& tet)
{	
	assert( itet_ins < tet.size() );
	assert( ipo_ins < aPo.size() );

	const int itet0 = itet_ins;
	const int itet1 = tet.size();
	const int itet2 = tet.size()+1;
	const int itet3 = tet.size()+2;

	tet.resize( tet.size()+3 );

	const STet old_tet = tet[itet_ins];

	aPo[ipo_ins].e = itet0;			aPo[ipo_ins].poel = 0;
	aPo[ old_tet.v[0] ].e = itet1;	aPo[ old_tet.v[0] ].poel = 0;
	aPo[ old_tet.v[1] ].e = itet0;	aPo[ old_tet.v[1] ].poel = 1;
	aPo[ old_tet.v[2] ].e = itet0;	aPo[ old_tet.v[2] ].poel = 2;
	aPo[ old_tet.v[3] ].e = itet0;	aPo[ old_tet.v[3] ].poel = 3;

	{
		STet& ref_tet = tet[itet0];

		ref_tet.v[0] = ipo_ins;			ref_tet.v[1] = old_tet.v[1];	ref_tet.v[2] = old_tet.v[2];	ref_tet.v[3] = old_tet.v[3];
		ref_tet.g[0] = old_tet.g[0];	ref_tet.g[1] = -2;				ref_tet.g[2] = -2;				ref_tet.g[3] = -2;
		ref_tet.s[0] = old_tet.s[0];	ref_tet.s[1] = itet1;			ref_tet.s[2] = itet2;			ref_tet.s[3] = itet3;

		if( old_tet.g[0] == -2 ){
			assert( old_tet.f[0] < 12 );
			const unsigned int* rel = tetRel[ old_tet.f[0] ];
			ref_tet.f[0] = noel2Rel[ rel[0]*4 + rel[1] ];
			assert( ref_tet.f[0] >= 0 && ref_tet.f[0] < 12 );
			assert( old_tet.s[0] < tet.size() );
			tet[ old_tet.s[0] ].s[ rel[0] ] = itet0;
			tet[ old_tet.s[0] ].f[ rel[0] ] = invTetRel[ ref_tet.f[0] ];
		}
		ref_tet.f[1] = 3;
		ref_tet.f[2] = 8;
		ref_tet.f[3] = 10;
	}
	{
		STet& ref_tet = tet[itet1];

		ref_tet.v[0] = old_tet.v[0];	ref_tet.v[1] = ipo_ins;			ref_tet.v[2] = old_tet.v[2];	ref_tet.v[3] = old_tet.v[3];
		ref_tet.g[0] = -2;				ref_tet.g[1] = old_tet.g[1];	ref_tet.g[2] = -2;				ref_tet.g[3] = -2;
		ref_tet.s[0] = itet0;			ref_tet.s[1] = old_tet.s[1];	ref_tet.s[2] = itet2;			ref_tet.s[3] = itet3;

		ref_tet.f[0] = 3;
		if( old_tet.g[1] == -2 ){
			assert( old_tet.f[1] < 12 );
			const unsigned int* rel = tetRel[ old_tet.f[1] ];
			ref_tet.f[1] = noel2Rel[ rel[0]*4 + rel[1] ];
			assert( ref_tet.f[1] >= 0 && ref_tet.f[1] < 12 );
			assert( old_tet.s[1] < tet.size() );
			tet[ old_tet.s[1] ].s[ rel[1] ] = itet1;
			tet[ old_tet.s[1] ].f[ rel[1] ] = invTetRel[ ref_tet.f[1] ];
		}
		ref_tet.f[2] = 2;
		ref_tet.f[3] = 1;
	}
	{
		STet& ref_tet = tet[itet2];
		ref_tet.v[0] = old_tet.v[0];	ref_tet.v[1] = old_tet.v[1];	ref_tet.v[2] = ipo_ins;			ref_tet.v[3] = old_tet.v[3];
		ref_tet.g[0] = -2;				ref_tet.g[1] = -2;				ref_tet.g[2] = old_tet.g[2];	ref_tet.g[3] = -2;
		ref_tet.s[0] = itet0;			ref_tet.s[1] = itet1;			ref_tet.s[2] = old_tet.s[2];	ref_tet.s[3] = itet3;
		
		ref_tet.f[0] = 8;
		ref_tet.f[1] = 2;
		if( old_tet.g[2] == -2 ){
			assert( old_tet.f[2] < 12 );
			const unsigned int* rel = tetRel[ old_tet.f[2] ];
			ref_tet.f[2] = noel2Rel[ rel[0]*4 + rel[1] ];
			assert( ref_tet.f[2] >= 0 && ref_tet.f[2] < 12 );
			assert( old_tet.s[2] < tet.size() );
			tet[ old_tet.s[2] ].s[ rel[2] ] = itet2;
			tet[ old_tet.s[2] ].f[ rel[2] ] = invTetRel[ ref_tet.f[2] ];
		}
		ref_tet.f[3] = 0;
	}
	{
		STet& ref_tet = tet[itet3];
		ref_tet.v[0] = old_tet.v[0];	ref_tet.v[1] = old_tet.v[1];	ref_tet.v[2] = old_tet.v[2];	ref_tet.v[3] = ipo_ins;
		ref_tet.g[0] = -2;				ref_tet.g[1] = -2;				ref_tet.g[2] = -2;				ref_tet.g[3] = old_tet.g[3];
		ref_tet.s[0] = itet0;			ref_tet.s[1] = itet1;			ref_tet.s[2] = itet2;			ref_tet.s[3] = old_tet.s[3];
		
		ref_tet.f[0] = 10;
		ref_tet.f[1] = 1;
		ref_tet.f[2] = 0;
		if( old_tet.g[3] == -2 ){
			assert( old_tet.f[3] < 12 );
			const unsigned int* rel = tetRel[ old_tet.f[3] ];
			ref_tet.f[3] = noel2Rel[ rel[0]*4 + rel[1] ];
			assert( ref_tet.f[3] >= 0 && ref_tet.f[3] < 12 );
			assert( old_tet.s[3] < tet.size() );
			tet[ old_tet.s[3] ].s[ rel[3] ] = itet3;
			tet[ old_tet.s[3] ].f[ rel[3] ] = invTetRel[ ref_tet.f[3] ];
		}
	}

	return true;
}


bool Msh::AddPointTet_Face(const unsigned int itet_ins, 
				  const unsigned int ifatet_ins,
				  const unsigned int ipo_ins,
				  std::vector<CPoint3D>& aPo,
				  std::vector<STet>& aTet)
{
	assert( itet_ins < aTet.size() );
	assert( ifatet_ins < 4 );
	assert( ipo_ins < aPo.size() );
	assert( aTet[itet_ins].g[ifatet_ins] == -2 );
	
	const unsigned int ino0_i = ifatet_ins;
	const unsigned int ino1_i = noelTetFace[ifatet_ins][0];
	const unsigned int ino2_i = noelTetFace[ifatet_ins][1];
	const unsigned int ino3_i = noelTetFace[ifatet_ins][2];

	const unsigned int itet_adj = aTet[itet_ins].s[ifatet_ins];
	assert( itet_adj < aTet.size() );

	const unsigned int ino0_a = tetRel[ aTet[itet_ins].f[ifatet_ins] ][ino0_i];
	const unsigned int ino1_a = tetRel[ aTet[itet_ins].f[ifatet_ins] ][ino2_i];
	const unsigned int ino2_a = tetRel[ aTet[itet_ins].f[ifatet_ins] ][ino1_i];
	const unsigned int ino3_a = tetRel[ aTet[itet_ins].f[ifatet_ins] ][ino3_i];

	const STet old_i = aTet[itet_ins];
	const STet old_a = aTet[itet_adj];

	assert( old_i.s[ino0_i] == itet_adj );
	assert( old_a.s[ino0_a] == itet_ins );
	assert( old_i.v[ino1_i] == old_a.v[ino2_a] );
	assert( old_i.v[ino2_i] == old_a.v[ino1_a] );
	assert( old_i.v[ino3_i] == old_a.v[ino3_a] );

	const unsigned int itet0 = itet_ins;
	const unsigned int itet1 = itet_adj;
	const unsigned int itet2 = aTet.size();
	const unsigned int itet3 = aTet.size()+1;
	const unsigned int itet4 = aTet.size()+2;
	const unsigned int itet5 = aTet.size()+3;
	aTet.resize( aTet.size()+4 );

	aPo[ ipo_ins ].e = itet0;			aPo[ ipo_ins ].poel = 0;
	aPo[ old_i.v[ino0_i] ].e = itet0;	aPo[ old_i.v[ino0_i] ].poel = 1;
	aPo[ old_i.v[ino1_i] ].e = itet0;	aPo[ old_i.v[ino1_i] ].poel = 2;
	aPo[ old_i.v[ino2_i] ].e = itet1;	aPo[ old_i.v[ino2_i] ].poel = 3;
	aPo[ old_i.v[ino3_i] ].e = itet0;	aPo[ old_i.v[ino3_i] ].poel = 3;
	aPo[ old_a.v[ino0_a] ].e = itet3;	aPo[ old_a.v[ino0_a] ].poel = 1;

	{
		STet& ref_tet = aTet[itet0];
		ref_tet.v[0] = ipo_ins;			ref_tet.v[1] = old_i.v[ino0_i];	ref_tet.v[2] = old_i.v[ino1_i];	ref_tet.v[3] = old_i.v[ino3_i];
		ref_tet.g[0] = old_i.g[ino2_i];	ref_tet.g[1] = -2;				ref_tet.g[2] = -2;				ref_tet.g[3] = -2;
		ref_tet.s[0] = old_i.s[ino2_i];	ref_tet.s[1] = itet3;			ref_tet.s[2] = itet1;			ref_tet.s[3] = itet2;
		
		if( old_i.g[ino2_i] == -2 ){
			assert( old_i.f[ino2_i] < 12 );
			const unsigned int* rel = tetRel[ old_i.f[ino2_i] ];
			ref_tet.f[0] = noel2Rel[ rel[ino2_i]*4 + rel[ino0_i] ];
			assert( ref_tet.f[0] >= 0 && ref_tet.f[0] < 12 );
			assert( old_i.s[ino2_i] < aTet.size() );
			aTet[ old_i.s[ino2_i] ].s[ rel[ino2_i] ] = itet0;
			aTet[ old_i.s[ino2_i] ].f[ rel[ino2_i] ] = invTetRel[ ref_tet.f[0] ];
		}
		ref_tet.f[1] = 0;
		ref_tet.f[2] = 0;
		ref_tet.f[3] = 0;
	}
	{
		STet& ref_tet = aTet[itet1];
		ref_tet.v[0] = ipo_ins;			ref_tet.v[1] = old_i.v[ino0_i];	ref_tet.v[2] = old_i.v[ino3_i];	ref_tet.v[3] = old_i.v[ino2_i];
		ref_tet.g[0] = old_i.g[ino1_i];	ref_tet.g[1] = -2;				ref_tet.g[2] = -2;				ref_tet.g[3] = -2;
		ref_tet.s[0] = old_i.s[ino1_i];	ref_tet.s[1] = itet4;			ref_tet.s[2] = itet2;			ref_tet.s[3] = itet0;
		
		if( old_i.g[ino1_i] == -2 ){
			assert( old_i.f[ino1_i] < 12 );
			const unsigned int* rel = tetRel[ old_i.f[ino1_i] ];
			ref_tet.f[0] = noel2Rel[ rel[ino1_i]*4 + rel[ino0_i] ];
			assert( ref_tet.f[0] >= 0 && ref_tet.f[0] < 12 );
			assert( old_i.s[ino1_i] < aTet.size() );
			aTet[ old_i.s[ino1_i] ].s[ rel[ino1_i] ] = itet1;
			aTet[ old_i.s[ino1_i] ].f[ rel[ino1_i] ] = invTetRel[ ref_tet.f[0] ];
		}
		ref_tet.f[1] = 0;
		ref_tet.f[2] = 0;
		ref_tet.f[3] = 0;
	}
	{
		STet& ref_tet = aTet[itet2];
		ref_tet.v[0] = ipo_ins;			ref_tet.v[1] = old_i.v[ino0_i];	ref_tet.v[2] = old_i.v[ino2_i];	ref_tet.v[3] = old_i.v[ino1_i];
		ref_tet.g[0] = old_i.g[ino3_i];	ref_tet.g[1] = -2;				ref_tet.g[2] = -2;				ref_tet.g[3] = -2;
		ref_tet.s[0] = old_i.s[ino3_i];	ref_tet.s[1] = itet5;			ref_tet.s[2] = itet0;			ref_tet.s[3] = itet1;
		
		if( old_i.g[ino3_i] == -2 ){
			assert( old_i.f[ino3_i] < 12 );
			const unsigned int* rel = tetRel[ old_i.f[ino3_i] ];
			ref_tet.f[0] = noel2Rel[ rel[ino3_i]*4 + rel[ino0_i] ];
			assert( ref_tet.f[0] >= 0 && ref_tet.f[0] < 12 );
			assert( old_i.s[ino3_i] < aTet.size() );
			aTet[ old_i.s[ino3_i] ].s[ rel[ino3_i] ] = itet2;
			aTet[ old_i.s[ino3_i] ].f[ rel[ino3_i] ] = invTetRel[ ref_tet.f[0] ];
		}
		ref_tet.f[1] = 0;
		ref_tet.f[2] = 0;
		ref_tet.f[3] = 0;
	}
	
	{
		STet& ref_tet = aTet[itet3];
		ref_tet.v[0] = ipo_ins;			ref_tet.v[1] = old_a.v[ino0_a];	ref_tet.v[2] = old_a.v[ino3_a];	ref_tet.v[3] = old_a.v[ino2_a];
		ref_tet.g[0] = old_a.g[ino1_a];	ref_tet.g[1] = -2;				ref_tet.g[2] = -2;				ref_tet.g[3] = -2;
		ref_tet.s[0] = old_a.s[ino1_a];	ref_tet.s[1] = itet0;			ref_tet.s[2] = itet5;			ref_tet.s[3] = itet4;
		
		if( old_a.g[ino1_a] == -2 ){
			assert( old_a.f[ino1_a] < 12 );
			const unsigned int* rel = tetRel[ old_a.f[ino1_a] ];
			ref_tet.f[0] = noel2Rel[ rel[ino1_a]*4 + rel[ino0_a] ];
			assert( ref_tet.f[0] >= 0 && ref_tet.f[0] < 12 );
			assert( old_a.s[ino1_a] < aTet.size() );
			aTet[ old_a.s[ino1_a] ].s[ rel[ino1_a] ] = itet3;
			aTet[ old_a.s[ino1_a] ].f[ rel[ino1_a] ] = invTetRel[ ref_tet.f[0] ];
		}
		ref_tet.f[1] = 0;
		ref_tet.f[2] = 0;
		ref_tet.f[3] = 0;
	}
	{
		STet& ref_tet = aTet[itet4];
		ref_tet.v[0] = ipo_ins;			ref_tet.v[1] = old_a.v[ino0_a];	ref_tet.v[2] = old_a.v[ino1_a];	ref_tet.v[3] = old_a.v[ino3_a];
		ref_tet.g[0] = old_a.g[ino2_a];	ref_tet.g[1] = -2;				ref_tet.g[2] = -2;				ref_tet.g[3] = -2;
		ref_tet.s[0] = old_a.s[ino2_a];	ref_tet.s[1] = itet1;			ref_tet.s[2] = itet3;			ref_tet.s[3] = itet5;

		if( old_a.g[ino2_a] == -2 ){
			assert( old_a.f[ino2_a] < 12 );
			const unsigned int* rel = tetRel[ old_a.f[ino2_a] ];
			ref_tet.f[0] = noel2Rel[ rel[ino2_a]*4 + rel[ino0_a] ];
			assert( ref_tet.f[0] >= 0 && ref_tet.f[0] < 12 );
			assert( old_a.s[ino2_a] < aTet.size() );
			aTet[ old_a.s[ino2_a] ].s[ rel[ino2_a] ] = itet4;
			aTet[ old_a.s[ino2_a] ].f[ rel[ino2_a] ] = invTetRel[ ref_tet.f[0] ];
		}
		ref_tet.f[1] = 0;
		ref_tet.f[2] = 0;
		ref_tet.f[3] = 0;
	}
	{
		STet& ref_tet = aTet[itet5];
		ref_tet.v[0] = ipo_ins;			ref_tet.v[1] = old_a.v[ino0_a];	ref_tet.v[2] = old_a.v[ino2_a];	ref_tet.v[3] = old_a.v[ino1_a];
		ref_tet.g[0] = old_a.g[ino3_a];	ref_tet.g[1] = -2;				ref_tet.g[2] = -2;				ref_tet.g[3] = -2;
		ref_tet.s[0] = old_a.s[ino3_a];	ref_tet.s[1] = itet2;			ref_tet.s[2] = itet4;			ref_tet.s[3] = itet3;
		
		if( old_a.g[ino3_a] == -2 ){
			assert( old_a.f[ino3_a] < 12 );
			const unsigned int* rel = tetRel[ old_a.f[ino3_a] ];
			ref_tet.f[0] = noel2Rel[ rel[ino3_a]*4 + rel[ino0_a] ];
			assert( ref_tet.f[0] >= 0 && ref_tet.f[0] < 12 );
			assert( old_a.s[ino3_a] < aTet.size() );
			aTet[ old_a.s[ino3_a] ].s[ rel[ino3_a] ] = itet5;
			aTet[ old_a.s[ino3_a] ].f[ rel[ino3_a] ] = invTetRel[ ref_tet.f[0] ];
		}
		ref_tet.f[1] = 0;
		ref_tet.f[2] = 0;
		ref_tet.f[3] = 0;
	}


	assert( CheckTet(aTet,aPo) );

	return true;
}

bool Msh::AddPointTri_Face(const unsigned int ipo_ins,
					  const unsigned int itri_ins,
					  std::vector<STri3D>& aTri )
{

	// ��������͋��L�t�@�C���������Ă������D
	const unsigned int relTriTri[3][3] = {
		{ 0, 2, 1 }, //  0
		{ 2, 1, 0 }, //  1 
		{ 1, 0, 2 }, //  2
	};
	const unsigned int invRelTriTri[3] = {
		0, 1, 2
	};
	const int noel2RelTriTri[9] = {
		-1,	// 0 00
		-1,	// 1 01
		 0,	// 2 02
		 2, // 3 10
		-1, // 4 11
		-1,	// 5 12
		-1,	// 6 20
		 1,	// 7 21
		-1, // 8 22
	};
	// �����܂�


	assert( itri_ins < aTri.size() );

	const int itri0 = itri_ins;
	const int itri1 = aTri.size();
	const int itri2 = aTri.size()+1;

	aTri.resize( aTri.size()+2 );

	const STri3D old_tri = aTri[itri_ins];

	{
		STri3D& ref_tri = aTri[itri0];

		ref_tri.v[0] = ipo_ins;			ref_tri.v[1] = old_tri.v[1];	ref_tri.v[2] = old_tri.v[2];
		ref_tri.g2[0] = old_tri.g2[0];	ref_tri.g2[1] = -2;				ref_tri.g2[2] = -2;
		ref_tri.s2[0] = old_tri.s2[0];	ref_tri.s2[1] = itri1;			ref_tri.s2[2] = itri2;

		if( old_tri.g2[0] == -2 || old_tri.g2[0] == -3 ){
			assert( old_tri.r2[0] < 3 );
			const unsigned int* rel = relTriTri[ old_tri.r2[0] ];
			ref_tri.r2[0] = noel2RelTriTri[ rel[0]*3 + rel[1] ];
			assert( ref_tri.r2[0] >= 0 && ref_tri.r2[0] < 3 );
			assert( old_tri.s2[0] < aTri.size() );
			aTri[ old_tri.s2[0] ].s2[ rel[0] ] = itri0;
			aTri[ old_tri.s2[0] ].r2[ rel[0] ] = invRelTriTri[ ref_tri.r2[0] ];
		}
		ref_tri.r2[1] = 0;
		ref_tri.r2[2] = 0;
	}
	{
		STri3D& ref_tri = aTri[itri1];

		ref_tri.v[0] = ipo_ins;			ref_tri.v[1] = old_tri.v[2];	ref_tri.v[2] = old_tri.v[0];
		ref_tri.g2[0] = old_tri.g2[1];	ref_tri.g2[1] = -2;				ref_tri.g2[2] = -2;
		ref_tri.s2[0] = old_tri.s2[1];	ref_tri.s2[1] = itri2;			ref_tri.s2[2] = itri0;

		if( old_tri.g2[1] == -2 || old_tri.g2[1] == -3 ){
			assert( old_tri.r2[1] < 3 );
			const unsigned int* rel = relTriTri[ old_tri.r2[1] ];
			ref_tri.r2[0] = noel2RelTriTri[ rel[1]*3 + rel[2] ];
			assert( ref_tri.r2[0] >= 0 && ref_tri.r2[0] < 3 );
			assert( old_tri.s2[1] < aTri.size() );
			aTri[ old_tri.s2[1] ].s2[ rel[1] ] = itri1;
			aTri[ old_tri.s2[1] ].r2[ rel[1] ] = invRelTriTri[ ref_tri.r2[0] ];
		}
		ref_tri.r2[1] = 0;
		ref_tri.r2[2] = 0;
	}
	{
		STri3D& ref_tri = aTri[itri2];

		ref_tri.v[0] = ipo_ins; 		ref_tri.v[1] = old_tri.v[0];	ref_tri.v[2] = old_tri.v[1];
		ref_tri.g2[0] = old_tri.g2[2];  ref_tri.g2[1] = -2;				ref_tri.g2[2] = -2;
		ref_tri.s2[0] = old_tri.s2[2];	ref_tri.s2[1] = itri0;			ref_tri.s2[2] = itri1;

		if( old_tri.g2[2] == -2 || old_tri.g2[2] == -3 ){
			assert( old_tri.r2[2] < 3 );
			const unsigned int* rel = relTriTri[ old_tri.r2[2] ];
			ref_tri.r2[0] = noel2RelTriTri[ rel[2]*3 + rel[0] ];
			assert( ref_tri.r2[0] >= 0 && ref_tri.r2[0] < 3 );
			assert( old_tri.s2[2] < aTri.size() );
			aTri[ old_tri.s2[2] ].s2[ rel[2] ] = itri2;
			aTri[ old_tri.s2[2] ].r2[ rel[2] ] = invRelTriTri[ ref_tri.r2[0] ];
		}
		ref_tri.r2[1] = 0;
		ref_tri.r2[2] = 0;
	}
	return true;
}


bool Msh::AddPointTri_Edge( const unsigned int ipo_ins,
					const unsigned int itri_ins,
					const unsigned int ied_ins,
					std::vector<STri3D>& aTri )
{
	assert( itri_ins < aTri.size() );
	assert( ied_ins < 3 );

	if( aTri[itri_ins].g2[ied_ins] != -2 ){
//		std::cout << "������" << std::endl;
		assert(0);
	}
	
	// ��������͋��L�t�@�C���������Ă������D
	const unsigned int nNoEd  = 2;
//	const unsigned int nNoTri = 3;
	const unsigned int nEdTri = 3;
	const unsigned int noelTriEdge[nEdTri][nNoEd] = {
		{ 1, 2 },
		{ 2, 0 },
		{ 0, 1 },
	};
	const unsigned int relTriTri[3][3] = {
		{ 0, 2, 1 }, //  0
		{ 2, 1, 0 }, //  1 
		{ 1, 0, 2 }, //  2
	};
	const unsigned int invRelTriTri[3] = {
		0, 1, 2
	};
	const int noel2RelTriTri[9] = {
		-1,	// 0 00
		-1,	// 1 01
		 0,	// 2 02
		 2, // 3 10
		-1, // 4 11
		-1,	// 5 12
		-1,	// 6 20
		 1,	// 7 21
		-1, // 8 22
	};
	// �����܂�

	const unsigned int itri_adj = aTri[itri_ins].s2[ied_ins];
	const unsigned int ied_adj  = relTriTri[ aTri[itri_ins].r2[ied_ins] ][ied_ins];
	assert( itri_adj < aTri.size() );
	assert( ied_ins < 3 );

    const unsigned int itri0 = itri_ins;
    const unsigned int itri1 = itri_adj;
    const unsigned int itri2 = aTri.size();
    const unsigned int itri3 = aTri.size()+1;

	aTri.resize( aTri.size()+2 );

	STri3D old0 = aTri[itri_ins];
	STri3D old1 = aTri[itri_adj];

	const unsigned int ino0_0 = ied_ins;
	const unsigned int ino1_0 = noelTriEdge[ied_ins][0];
	const unsigned int ino2_0 = noelTriEdge[ied_ins][1];

	const unsigned int ino0_1 = ied_adj;
	const unsigned int ino1_1 = noelTriEdge[ied_adj][0];
	const unsigned int ino2_1 = noelTriEdge[ied_adj][1];
	
	assert( old0.v[ino1_0] == old1.v[ino2_1] );
	assert( old0.v[ino2_0] == old1.v[ino1_1] );
	assert( old0.s2[ino0_0 ] == itri1 );
	assert( old1.s2[ino0_1 ] == itri0 );

	{
		STri3D& ref_tri = aTri[itri0];
		////////////////
		ref_tri.v[0]  = ipo_ins;			ref_tri.v[1]  = old0.v[ino2_0];	ref_tri.v[2]  = old0.v[ino0_0];
		ref_tri.g2[0] = old0.g2[ino1_0];	ref_tri.g2[1] = -2;				ref_tri.g2[2] = -2;
		ref_tri.s2[0] = old0.s2[ino1_0];	ref_tri.s2[1] = itri1;			ref_tri.s2[2] = itri3;
		////////////////
		if( old0.g2[ino1_0] == -2 || old0.g2[ino1_0] == -3 ){
			assert( old0.r2[ino1_0] < 3 );
			const unsigned int* rel = relTriTri[ old0.r2[ino1_0] ];
			ref_tri.r2[0] = noel2RelTriTri[ rel[ino1_0]*3 + rel[ino2_0] ];
			assert( ref_tri.r2[0] >= 0 && ref_tri.r2[0] < 3 );
			assert( old0.s2[ino1_0] < aTri.size() );
			aTri[ old0.s2[ino1_0] ].s2[ rel[ino1_0] ] = itri0;
			aTri[ old0.s2[ino1_0] ].r2[ rel[ino1_0] ] = invRelTriTri[ ref_tri.r2[0] ];
		}
		ref_tri.r2[1] = 0;
		ref_tri.r2[2] = 0;
	}
	{
		STri3D& ref_tri = aTri[itri1];
		////////////////
		ref_tri.v[0]  = ipo_ins;			ref_tri.v[1]  = old0.v[ino0_0];	ref_tri.v[2]  = old0.v[ino1_0];
		ref_tri.g2[0] = old0.g2[ino2_0];	ref_tri.g2[1] = -2;				ref_tri.g2[2] = -2;
		ref_tri.s2[0] = old0.s2[ino2_0];	ref_tri.s2[1] = itri2;			ref_tri.s2[2] = itri0;
		////////////////
		if( old0.g2[ino2_0] == -2 || old0.g2[ino2_0] == -3 ){
			assert( old0.r2[ino2_0] < 3 );
			const unsigned int* rel = relTriTri[ old0.r2[ino2_0] ];
			ref_tri.r2[0] = noel2RelTriTri[ rel[ino2_0]*3 + rel[ino0_0] ];
			assert( ref_tri.r2[0] >= 0 && ref_tri.r2[0] < 3 );
			assert( old0.s2[ino2_0] < aTri.size() );
			aTri[ old0.s2[ino2_0] ].s2[ rel[ino2_0] ] = itri1;
			aTri[ old0.s2[ino2_0] ].r2[ rel[ino2_0] ] = invRelTriTri[ ref_tri.r2[0] ];
		}
		ref_tri.r2[1] = 0;
		ref_tri.r2[2] = 0;
	}
	{
		STri3D& ref_tri = aTri[itri2];
		////////////////
		ref_tri.v[0]  = ipo_ins;			ref_tri.v[1]  = old1.v[ino2_1];	ref_tri.v[2]  = old1.v[ino0_1];
		ref_tri.g2[0] = old1.g2[ino1_1];	ref_tri.g2[1] = -2;				ref_tri.g2[2] = -2;
		ref_tri.s2[0] = old1.s2[ino1_1];	ref_tri.s2[1] = itri3;			ref_tri.s2[2] = itri1;
		////////////////
		if( old1.g2[ino1_1] == -2 || old0.g2[ino2_0] == -3 ){
			assert( old1.r2[ino1_1] < 3 );
			const unsigned int* rel = relTriTri[ old1.r2[ino1_1] ];
			ref_tri.r2[0] = noel2RelTriTri[ rel[ino1_1]*3 + rel[ino2_1] ];
			assert( ref_tri.r2[0] >= 0 && ref_tri.r2[0] < 3 );
			assert( old1.s2[ino1_1] < aTri.size() );
			aTri[ old1.s2[ino1_1] ].s2[ rel[ino1_1] ] = itri2;
			aTri[ old1.s2[ino1_1] ].r2[ rel[ino1_1] ] = invRelTriTri[ ref_tri.r2[0] ];
		}
		ref_tri.r2[1] = 0;
		ref_tri.r2[2] = 0;
	}
	{
		STri3D& ref_tri = aTri[itri3];
		ref_tri.v[0]  = ipo_ins;			ref_tri.v[1]  = old1.v[ino0_1];	ref_tri.v[2]  = old1.v[ino1_1];
		ref_tri.g2[0] = old1.g2[ino2_1];	ref_tri.g2[1] = -2;				ref_tri.g2[2] = -2;
		ref_tri.s2[0] = old1.s2[ino2_1];	ref_tri.s2[1] = itri0;			ref_tri.s2[2] = itri2;
		if( old1.g2[ino2_1] == -2 || old1.g2[ino2_1] == -3 ){
			assert( old1.r2[ino2_1] < 3 );
			const unsigned int* rel = relTriTri[ old1.r2[ino2_1] ];
			ref_tri.r2[0] = noel2RelTriTri[ rel[ino2_1]*3 + rel[ino0_1] ];
			assert( ref_tri.r2[0] >= 0 && ref_tri.r2[0] < 3 );
			assert( old1.s2[ino2_1] < aTri.size() );
			aTri[ old1.s2[ino2_1] ].s2[ rel[ino2_1] ] = itri3;
			aTri[ old1.s2[ino2_1] ].r2[ rel[ino2_1] ] = invRelTriTri[ ref_tri.r2[0] ];
		}
		ref_tri.r2[1] = 0;
		ref_tri.r2[2] = 0;
	}
	return true;
}

bool Msh::AddPointTet_Edge(const ElemAroundEdge& elared,
				  const unsigned int ino_ins,
				  std::vector<CPoint3D>& node,
				  std::vector<STet>& tet)
{
	assert( elared.size() >= 3 );

	std::vector<STet> old_tet;
	old_tet.resize( elared.size() );
    for(unsigned int ielared=0;ielared<elared.size();ielared++){
		old_tet[ielared] = tet[ elared.e[ielared].first ];
	}

	std::vector< std::pair<int,int> > inew_tet;
	inew_tet.resize( elared.size() );
    for(unsigned int ielared=0;ielared<elared.size();ielared++){
		inew_tet[ielared].first = elared.e[ielared].first;
		inew_tet[ielared].second = tet.size()+ielared;
//		std::cout << ielared << "    " << elared.n[ielared] << "     " << inew_tet[ielared].first << "     " << inew_tet[ielared].second << std::endl;
	}
	tet.resize( tet.size()+elared.size() );

	assert( ino_ins < node.size() );

    const unsigned int inod = elared.nod;
    const unsigned int inou = elared.nou;
//	std::cout << " nodu " << inod << "   " << inou << std::endl;

	{
		const struct STet& old = old_tet[0];
		const unsigned int* edge_rel = tetRel[elared.e[0].second];
		const int itet_d = inew_tet[0].first;
		const int itet_u = inew_tet[0].second;
		const int noel_d = edge_rel[0];
		const int noel_u = edge_rel[1];
		assert( old.v[ noel_d ] == inod );
		assert( old.v[ noel_u ] == inou );
		{	
			STet& ref_tet = tet[itet_d];
			ref_tet.v[0] = ino_ins;
			ref_tet.v[1] = inod;
			ref_tet.v[2] = elared.n[1];
			ref_tet.v[3] = elared.n[0];

			ref_tet.s[0] = old.s[noel_u];
			ref_tet.s[1] = itet_u;
			ref_tet.s[2] = inew_tet[elared.size()-1].first;
			ref_tet.s[3] = inew_tet[1].first;

			ref_tet.g[0] = old.g[noel_u];
			ref_tet.g[1] = -2;
			ref_tet.g[2] = -2;
			ref_tet.g[3] = -2;

			if( ref_tet.g[0] == -2 ){
				const unsigned int* rel0 = tetRel[old.f[noel_u]];
				ref_tet.f[0] = noel2Rel[ rel0[edge_rel[1]]*4 + rel0[edge_rel[0]] ];
				tet[old.s[noel_u]].f[rel0[noel_u]] = invTetRel[ ref_tet.f[0] ];
				tet[old.s[noel_u]].s[rel0[noel_u]] = itet_d;
			}
			ref_tet.f[1] = 0;
			ref_tet.f[2] = 0;
			ref_tet.f[3] = 0;
		}
		{	
			STet& ref_tet = tet[itet_u];
			ref_tet.v[0] = ino_ins;
			ref_tet.v[1] = inou;
			ref_tet.v[2] = elared.n[0];
			ref_tet.v[3] = elared.n[1];

			ref_tet.s[0] = old.s[noel_d];
			ref_tet.s[1] = itet_d;
			ref_tet.s[2] = inew_tet[1].second;
			ref_tet.s[3] = inew_tet[elared.size()-1].second;

			ref_tet.g[0] = old.g[noel_d];
			ref_tet.g[1] = -2;
			ref_tet.g[2] = -2;
			ref_tet.g[3] = -2;

			if( ref_tet.g[0] == -2 ){
				const unsigned int* rel0 = tetRel[old.f[noel_d]];
				ref_tet.f[0] = noel2Rel[ rel0[edge_rel[0]]*4 + rel0[edge_rel[1]] ];
				tet[old.s[noel_d]].f[rel0[noel_d]] = invTetRel[ ref_tet.f[0] ];
				tet[old.s[noel_d]].s[rel0[noel_d]] = itet_u;
			}
			ref_tet.f[1] = 0;
			ref_tet.f[2] = 0;
			ref_tet.f[3] = 0;
		}

		node[ elared.n[0] ].e = itet_d;
		node[ elared.n[0] ].poel = 3;
	}
    assert( elared.size() > 0 );
    for(unsigned int ielared=1;ielared<elared.size()-1;ielared++){
		const int itet_d = inew_tet[ielared].first;
		const int itet_u = inew_tet[ielared].second;
		const struct STet& old = old_tet[ielared];
		const unsigned int* edge_rel = tetRel[elared.e[ielared].second];
		const int noel_d = edge_rel[0];
		const int noel_u = edge_rel[1];
		assert( old.v[ noel_d ] == inod );
		assert( old.v[ noel_u ] == inou );
		{	
			STet& ref_tet = tet[itet_d];
			ref_tet.v[0] = ino_ins;
			ref_tet.v[1] = inod;
			ref_tet.v[2] = elared.n[ielared+1];
			ref_tet.v[3] = elared.n[ielared];

			ref_tet.s[0] = old.s[noel_u];
			ref_tet.s[1] = itet_u;
			ref_tet.s[2] = inew_tet[ielared-1].first;
			ref_tet.s[3] = inew_tet[ielared+1].first;

			ref_tet.g[0] = old.g[noel_u];
			ref_tet.g[1] = -2;
			ref_tet.g[2] = -2;
			ref_tet.g[3] = -2;

			if( ref_tet.g[0] == -2 ){
				const unsigned int* rel0 = tetRel[old.f[noel_u]];
				ref_tet.f[0] = noel2Rel[ rel0[edge_rel[1]]*4 + rel0[edge_rel[0]] ];
				tet[old.s[noel_u]].f[rel0[noel_u]] = invTetRel[ ref_tet.f[0] ];
				tet[old.s[noel_u]].s[rel0[noel_u]] = itet_d;
			}
			ref_tet.f[1] = 0;
			ref_tet.f[2] = 0;
			ref_tet.f[3] = 0;
		}
		{	
			STet& ref_tet = tet[itet_u];
			ref_tet.v[0] = ino_ins;
			ref_tet.v[1] = inou;
			ref_tet.v[2] = elared.n[ielared];
			ref_tet.v[3] = elared.n[ielared+1];

			ref_tet.s[0] = old.s[noel_d];
			ref_tet.s[1] = itet_d;
			ref_tet.s[2] = inew_tet[ielared+1].second;
			ref_tet.s[3] = inew_tet[ielared-1].second;

			ref_tet.g[0] = old.g[noel_d];
			ref_tet.g[1] = -2;
			ref_tet.g[2] = -2;
			ref_tet.g[3] = -2;

			if( ref_tet.g[0] == -2 ){
				const unsigned int* rel0 = tetRel[old.f[noel_d]];
				ref_tet.f[0] = noel2Rel[ rel0[edge_rel[0]]*4 + rel0[edge_rel[1]] ];
				tet[old.s[noel_d]].f[rel0[noel_d]] = invTetRel[ ref_tet.f[0] ];
				tet[old.s[noel_d]].s[rel0[noel_d]] = itet_u;
			}
			ref_tet.f[1] = 0;
			ref_tet.f[2] = 0;
			ref_tet.f[3] = 0;
		}
		node[ elared.n[ielared] ].e = itet_d;
		node[ elared.n[ielared] ].poel = 3;
	}
	{
		const int ilast = elared.size()-1;
		const struct STet& old = old_tet[ilast];
		const unsigned int* edge_rel = tetRel[elared.e[ilast].second];
		const int itet_d = inew_tet[ilast].first;
		const int itet_u = inew_tet[ilast].second;
		const int noel_d = edge_rel[0];
		const int noel_u = edge_rel[1];
		assert( old.v[ edge_rel[0] ] == inod );
		assert( old.v[ edge_rel[1] ] == inou );
		{	
			STet& ref_tet = tet[itet_d];
			ref_tet.v[0] = ino_ins;
			ref_tet.v[1] = inod;
			ref_tet.v[2] = elared.n[0];
			ref_tet.v[3] = elared.n[ilast];

			ref_tet.s[0] = old.s[noel_u];
			ref_tet.s[1] = itet_u;
			ref_tet.s[2] = inew_tet[ilast-1].first;
			ref_tet.s[3] = inew_tet[0].first;

			ref_tet.g[0] = old.g[noel_u];
			ref_tet.g[1] = -2;
			ref_tet.g[2] = -2;
			ref_tet.g[3] = -2;

			if( ref_tet.g[0] == -2 ){
				const unsigned int* rel0 = tetRel[old.f[noel_u]];
				ref_tet.f[0] = noel2Rel[ rel0[edge_rel[1]]*4 + rel0[edge_rel[0]] ];
				tet[old.s[noel_u]].f[rel0[noel_u]] = invTetRel[ ref_tet.f[0] ];
				tet[old.s[noel_u]].s[rel0[noel_u]] = itet_d;
			}
			ref_tet.f[1] = 0;
			ref_tet.f[2] = 0;
			ref_tet.f[3] = 0;
		}
		{	
			STet& ref_tet = tet[itet_u];
			ref_tet.v[0] = ino_ins;
			ref_tet.v[1] = inou;
			ref_tet.v[2] = elared.n[ilast];
			ref_tet.v[3] = elared.n[0];

			ref_tet.s[0] = old.s[noel_d];
			ref_tet.s[1] = itet_d;
			ref_tet.s[2] = inew_tet[0].second;
			ref_tet.s[3] = inew_tet[ilast-1].second;

			ref_tet.g[0] = old.g[noel_d];
			ref_tet.g[1] = -2;
			ref_tet.g[2] = -2;
			ref_tet.g[3] = -2;

			if( ref_tet.g[0] == -2 ){
				const unsigned int* rel0 = tetRel[old.f[noel_d]];
				ref_tet.f[0] = noel2Rel[ rel0[edge_rel[0]]*4 + rel0[edge_rel[1]] ];
				tet[old.s[noel_d]].f[rel0[noel_d]] = invTetRel[ ref_tet.f[0] ];
				tet[old.s[noel_d]].s[rel0[noel_d]] = itet_u;
			}
			ref_tet.f[1] = 0;
			ref_tet.f[2] = 0;
			ref_tet.f[3] = 0;
		}
		node[ elared.n[ilast] ].e = itet_d;
		node[ elared.n[ilast] ].poel = 3;
	}
	old_tet.clear();


	node[ inod ].e = inew_tet[0].first;
	node[ inod ].poel = 1;

	node[ inou ].e = inew_tet[0].second;
	node[ inou ].poel = 1;

	node[ ino_ins ].old_p = -1;
	node[ ino_ins ].e = inew_tet[0].first;
	node[ ino_ins ].poel = 0;

	inew_tet.clear();
	return true;
}

bool Msh::GetEdgeSwapPtnCrt(const ElemAroundEdge& elared,
				int& ptn,
				double& min_max_crt,
				const std::vector<CPoint3D>& node ){

	if( elared.size() == 3 ){
		double crt1 = Criterion_PLJ(node[elared.nod],node[elared.n[0]],node[elared.n[1]],node[elared.n[2]] );
		if( crt1<0.0 ){ ptn=-1; min_max_crt=ILL_CRT; return false; }
		double crt2 = Criterion_PLJ(node[elared.nou],node[elared.n[2]],node[elared.n[1]],node[elared.n[0]] );
		if( crt2<0.0 ){ ptn=-1; min_max_crt=ILL_CRT; return false; }
		ptn = 0;
		min_max_crt = ( crt1 > crt2 ) ?  crt1 : crt2;
		return true;
	}
	if( elared.size() == 4 ){
		double crt1;
		double max_crt0,max_crt1;
		for(;;){
			crt1 = Criterion_PLJ(node[elared.nou],node[elared.n[0]],node[elared.n[2]],node[elared.n[1]] );
			if( crt1<0.0 ){ max_crt0=ILL_CRT; break; }
			max_crt0 = crt1;
			crt1 = Criterion_PLJ(node[elared.nod],node[elared.n[0]],node[elared.n[1]],node[elared.n[2]] );
			if( crt1<0.0 ){ max_crt0=ILL_CRT; break; }
			max_crt0 = ( max_crt0 > crt1 ) ? max_crt0 : crt1;
			crt1 = Criterion_PLJ(node[elared.nou],node[elared.n[0]],node[elared.n[3]],node[elared.n[2]] );
			if( crt1<0.0 ){ max_crt0=ILL_CRT; break; }
			max_crt0 = ( max_crt0 > crt1 ) ? max_crt0 : crt1;
			crt1 = Criterion_PLJ(node[elared.nod],node[elared.n[0]],node[elared.n[2]],node[elared.n[3]] );
			if( crt1<0.0 ){ max_crt0=ILL_CRT; break; }
			max_crt0 = ( max_crt0 > crt1 ) ? max_crt0 : crt1;
			break;
		}
		for(;;){
			crt1 = Criterion_PLJ(node[elared.nou],node[elared.n[0]],node[elared.n[3]],node[elared.n[1]] );
			if( crt1<0.0 ){ max_crt1=ILL_CRT; break; }
			max_crt1 = crt1;
			crt1 = Criterion_PLJ(node[elared.nod],node[elared.n[0]],node[elared.n[1]],node[elared.n[3]] );
			if( crt1<0.0 ){ max_crt1=ILL_CRT; break; }
			max_crt1 = ( max_crt1 > crt1 ) ? max_crt1 : crt1;
			crt1 = Criterion_PLJ(node[elared.nou],node[elared.n[3]],node[elared.n[2]],node[elared.n[1]] );
			if( crt1<0.0 ){ max_crt1=ILL_CRT; break; }
			max_crt1 = ( max_crt1 > crt1 ) ? max_crt1 : crt1;
			crt1 = Criterion_PLJ(node[elared.nod],node[elared.n[3]],node[elared.n[1]],node[elared.n[2]] );
			if( crt1<0.0 ){ max_crt1=ILL_CRT; break; }
			max_crt1 = ( max_crt1 > crt1 ) ? max_crt1 : crt1;
			break;
		}
		min_max_crt = ( max_crt0 < max_crt1 ) ?  max_crt0 : max_crt1;
		if( min_max_crt > ILL_CRT*0.9 ){
			ptn = -1;
			return true;
		}
		ptn = ( max_crt0 < max_crt1 ) ?  0 : 1;
		assert( min_max_crt < ILL_CRT*0.9 );
		return true;
	}
	if( elared.size() == 5 ){
		GetEdgeSwapPtnCrt5Elared(elared,ptn,min_max_crt,node);
//		std::cout << " -- > " << ptn << " " << min_max_crt << std::endl;
		return true;
	}

	ptn = -1;
	min_max_crt = ILL_CRT;

	return true;
}

bool Msh::FaceSwapTet(const unsigned int itet0,
				  const unsigned int iface0, 
				  std::vector<STet>& tet,
				  std::vector<CPoint3D>& node)
{
	assert( itet0 < tet.size() );
	assert( iface0 < 4 );
	assert( tet[itet0].g[iface0] == -2 );

	const int itet1 = tet[itet0].s[iface0];
	assert( itet1 >= 0 && (unsigned int)itet1 < tet.size() );

	const STet old_d = tet[itet0];
	const STet old_u = tet[itet1];

	const int itet2 = tet.size();
	tet.resize( tet.size()+1 );

	STet& tet0 = tet[itet0];
	STet& tet1 = tet[itet1];
	STet& tet2 = tet[itet2];

	const unsigned int* rel_du = tetRel[ tet[itet0].f[iface0] ];

	const int noel_d0 = iface0;
	const int noel_d1 = noelTetFace[iface0][0];
	const int noel_d2 = noelTetFace[iface0][1];
	const int noel_d3 = noelTetFace[iface0][2];

	const int noel_u0 = rel_du[ iface0 ];
	const int noel_u1 = rel_du[ noelTetFace[iface0][0] ];
	const int noel_u2 = rel_du[ noelTetFace[iface0][1] ];
	const int noel_u3 = rel_du[	noelTetFace[iface0][2] ];

	assert( old_d.v[noel_d1] == old_u.v[noel_u1] );
	assert( old_d.v[noel_d2] == old_u.v[noel_u2] );
	assert( old_d.v[noel_d3] == old_u.v[noel_u3] );

	{
		tet0.v[0] = old_d.v[ noel_d0 ];
		tet0.v[1] = old_u.v[ noel_u0 ];
		tet0.v[2] = old_d.v[ noel_d2 ];
		tet0.v[3] = old_d.v[ noel_d3 ];

		tet0.s[0] =	old_u.s[ noel_u1 ];		
		tet0.s[1] =	old_d.s[ noel_d1 ];
		tet0.s[2] = itet1;
		tet0.s[3] = itet2;

		tet0.g[0] =	old_u.g[ noel_u1 ];		
		tet0.g[1] =	old_d.g[ noel_d1 ];
		tet0.g[2] = -2;
		tet0.g[3] = -2;

		if( old_u.g[ noel_u1 ] == -2 ){
			const unsigned int* rel1 = tetRel[ old_u.f[ noel_u1 ] ];
			tet0.f[0] = noel2Rel[ rel1[noel_u1]*4+rel1[noel_u0] ];
			tet[ old_u.s[noel_u1] ].s[ rel1[noel_u1] ] = itet0;
			tet[ old_u.s[noel_u1] ].f[ rel1[noel_u1] ] = invTetRel[tet0.f[0]];
		}
		if( old_d.g[ noel_d1 ] == -2 ){
			const unsigned int* rel1 = tetRel[ old_d.f[ noel_d1 ] ];
			tet0.f[1] = noel2Rel[ rel1[noel_d0]*4+rel1[noel_d1] ];
			tet[ old_d.s[noel_d1] ].s[ rel1[noel_d1] ] = itet0;
			tet[ old_d.s[noel_d1] ].f[ rel1[noel_d1] ] = invTetRel[tet0.f[1]];
		}
		tet0.f[2] = 0;
		tet0.f[3] = 0;
	}

	{
		tet1.v[0] = old_d.v[ noel_d0 ];
		tet1.v[1] = old_u.v[ noel_u0 ];
		tet1.v[2] = old_d.v[ noel_d3 ];	
		tet1.v[3] = old_d.v[ noel_d1 ];

		tet1.s[0] =	old_u.s[ noel_u2 ];		
		tet1.s[1] =	old_d.s[ noel_d2 ];
		tet1.s[2] = itet2;	
		tet1.s[3] = itet0;

		tet1.g[0] =	old_u.g[ noel_u2 ];		
		tet1.g[1] =	old_d.g[ noel_d2 ];
		tet1.g[2] = -2;
		tet1.g[3] = -2;

		if( old_u.g[ noel_u2 ] == -2 ){
			const unsigned int* rel1 = tetRel[ old_u.f[ noel_u2 ] ];
			tet1.f[0] = noel2Rel[ rel1[noel_u2]*4+rel1[noel_u0] ];
			tet[ old_u.s[noel_u2] ].s[ rel1[noel_u2] ] = itet1;
			tet[ old_u.s[noel_u2] ].f[ rel1[noel_u2] ] = invTetRel[tet1.f[0]];
		}
		if( old_d.g[ noel_d2 ] == -2 ){
			const unsigned int* rel1 = tetRel[ old_d.f[ noel_d2 ] ];
			tet1.f[1] = noel2Rel[ rel1[noel_d0]*4+rel1[noel_d2] ];
			tet[ old_d.s[noel_d2] ].s[ rel1[noel_d2] ] = itet1;
			tet[ old_d.s[noel_d2] ].f[ rel1[noel_d2] ] = invTetRel[tet1.f[1]];
		}
		tet1.f[2] = 0;
		tet1.f[3] = 0;
	}

	{
		tet2.v[0] = old_d.v[ noel_d0 ];
		tet2.v[1] = old_u.v[ noel_u0 ];
		tet2.v[2] = old_d.v[ noel_d1 ];
		tet2.v[3] = old_d.v[ noel_d2 ];

		tet2.s[0] =	old_u.s[ noel_u3 ];		
		tet2.s[1] =	old_d.s[ noel_d3 ];
		tet2.s[2] =	itet0;
		tet2.s[3] = itet1;

		tet2.g[0] =	old_u.g[ noel_u3 ];		
		tet2.g[1] =	old_d.g[ noel_d3 ];
		tet2.g[2] = -2;
		tet2.g[3] = -2;
		
		if( old_u.g[ noel_u3 ] == -2 ){
			const unsigned int* rel1 = tetRel[ old_u.f[ noel_u3 ] ];
			tet2.f[0] = noel2Rel[ rel1[noel_u3]*4+rel1[noel_u0] ];
			tet[ old_u.s[noel_u3] ].s[ rel1[noel_u3] ] = itet2;
			tet[ old_u.s[noel_u3] ].f[ rel1[noel_u3] ] = invTetRel[tet2.f[0]];
		}
		if( old_d.g[ noel_d3 ] == -2 ){
			const unsigned int* rel1 = tetRel[ old_d.f[ noel_d3 ] ];
			tet2.f[1] = noel2Rel[ rel1[noel_d0]*4+rel1[noel_d3] ];
			tet[ old_d.s[noel_d3] ].s[ rel1[noel_d3] ] = itet2;
			tet[ old_d.s[noel_d3] ].f[ rel1[noel_d3] ] = invTetRel[tet2.f[1]];
		}
		tet2.f[2] = 0;
		tet2.f[3] = 0;
	}

//	std::cout << tet0.v[0] << " " << itet0 << " " << 0 << std::endl;
//	std::cout << tet0.v[1] << " " << itet0 << " " << 1 << std::endl;
//	std::cout << tet1.v[3] << " " << itet1 << " " << 3 << std::endl;
//	std::cout << tet0.v[2] << " " << itet0 << " " << 2 << std::endl;
//	std::cout << tet0.v[3] << " " << itet0 << " " << 3 << std::endl;

	node[ tet0.v[0] ].e = itet0;	node[ tet0.v[0] ].poel = 0;
	node[ tet0.v[1] ].e = itet0;	node[ tet0.v[1] ].poel = 1;
	node[ tet1.v[3] ].e = itet1;	node[ tet1.v[3] ].poel = 3;
	node[ tet0.v[2] ].e = itet0;	node[ tet0.v[2] ].poel = 2;
	node[ tet0.v[3] ].e = itet0;	node[ tet0.v[3] ].poel = 3;

	return true;
}

bool Msh::GetFaceSwapCrt(const int itet0,
					const int iface0,
					double& max_crt,
					const std::vector<STet>& tet,
					const std::vector<CPoint3D>& node )
{

	assert( itet0 >= 0 && (unsigned int)itet0 < tet.size() );
	assert( iface0 >= 0 && (unsigned int)iface0 < 4 );

	const int nod = tet[itet0].v[iface0];
	const int nou = tet[ tet[itet0].s[iface0] ].v[ tetRel[ tet[itet0].f[iface0] ][iface0] ];

	double crt1;
	crt1 = Criterion_PLJ(node[nod],node[nou],node[ tet[itet0].v[noelTetFace[iface0][0]] ],node[ tet[itet0].v[noelTetFace[iface0][1]] ]);
	if( crt1<0.0 ){ max_crt=ILL_CRT; return false; }
	max_crt = crt1;
	crt1 = Criterion_PLJ(node[nod],node[nou],node[ tet[itet0].v[noelTetFace[iface0][1]] ],node[ tet[itet0].v[noelTetFace[iface0][2]] ]);
	if( crt1<0.0 ){ max_crt=ILL_CRT; return false; }
	max_crt = ( max_crt > crt1 ) ? max_crt : crt1;
	crt1 = Criterion_PLJ(node[nod],node[nou],node[ tet[itet0].v[noelTetFace[iface0][2]] ],node[ tet[itet0].v[noelTetFace[iface0][0]] ]);
	if( crt1<0.0 ){ max_crt=ILL_CRT; return false; }
	max_crt = ( max_crt > crt1 ) ? max_crt : crt1;

	return true;
}

bool Msh::MakeTriSurNo(unsigned int& ntrisuno,
				  unsigned int*& trisuno_ind,
				  unsigned int*& trisuno,
				  const std::vector<STri3D>& tri,
				  const unsigned int nnode )
{
	assert( trisuno_ind == 0 );
	assert( trisuno == 0 );
	assert( nnode > 0 );

    const unsigned int nnotri = 3;

	trisuno_ind = new unsigned int [nnode+1];
	for(unsigned int inode=0;inode<nnode+1;inode++){
		trisuno_ind[inode] = 0;
	}
	for(unsigned int itri=0;itri<tri.size();itri++){
		for(unsigned int inotri=0;inotri<nnotri;inotri++){
			assert( tri[itri].v[inotri] >= 0 && tri[itri].v[inotri] < nnode );
			trisuno_ind[ tri[itri].v[inotri]+1 ]++;
		}
	}
	for(unsigned int inode=0;inode<nnode;inode++){
		trisuno_ind[inode+1] += trisuno_ind[inode];
	}
	ntrisuno = trisuno_ind[nnode];
	trisuno = new unsigned int [ntrisuno];

	for(unsigned int itri=0;itri<tri.size();itri++){
		for(unsigned int inotri=0;inotri<nnotri;inotri++){
			const int inode1 = tri[itri].v[inotri];
			trisuno[ trisuno_ind[inode1] ] = itri;
			trisuno_ind[ inode1 ]++;
		}
	}
	for(int inode=nnode-1;inode>=0;inode--){
		trisuno_ind[inode+1] = trisuno_ind[inode];
	}
	trisuno_ind[0] = 0;
/*
	std::cout << ntrisuno << std::endl;
	for(int inode=0;inode<nnode;inode++){
		std::cout << inode <<  " " << trisuno_ind[inode] << " " << trisuno_ind[inode+1] << "-->";
		for(int itrisuno=trisuno_ind[inode];itrisuno<trisuno_ind[inode+1];itrisuno++){
			std::cout << trisuno[itrisuno] << " ";
		}
		std::cout << std::endl;
	}
*/

	return true;
}

bool Msh::EdgeSwapTet(const ElemAroundEdge& elared,
		  const int ptn,
		  std::vector<STet>& tet,
		  std::vector<CPoint3D>& point )
{
	if( elared.size() == 3 ){
		return Swap3Elared(elared,ptn,tet,point);
	}
	if( elared.size() == 4 ){
		return Swap4Elared(elared,ptn,tet,point);
	}
	if( elared.size() == 5 ){
		return Swap5Elared(elared,ptn,tet,point);
	}
	return true;
}

bool Msh::FlipEdgeTri( const unsigned int itri0, const unsigned int ied0,
			   std::vector<STri3D>& tri )
{
	// ��������w�b�_�[�t�@�C���ɂ܂Ƃ߂�
	const unsigned int relTriTri[3][3] = {
		{ 0, 2, 1 }, //  0
		{ 2, 1, 0 }, //  1 
		{ 1, 0, 2 }, //  2
	};
	const unsigned int noelTriEdge[3][2] = {
		{ 1, 2 },
		{ 2, 0 },
		{ 0, 1 },
	};
	const unsigned int invRelTriTri[3] = {
		0, 1, 2
	};
	const int noel2RelTriTri[9] = {
		-1,	// 0 00
		-1,	// 1 01
		 0,	// 2 02
		 2, // 3 10
		-1, // 4 11
		-1,	// 5 12
		-1,	// 6 20
		 1,	// 7 21
		-1, // 8 22
	};
	// �����܂�

	assert( itri0 < tri.size() );
	assert( ied0 >=0 && ied0 < 3 );
	assert( tri[itri0].g2[ied0] == -2 );

	const unsigned int itri1 = tri[itri0].s2[ied0];
	const unsigned int ied1  = relTriTri[ tri[itri0].r2[ied0] ][ied0];
	assert( itri1 < tri.size() );
	assert( ied1 < 3 );
	assert( tri[itri1].g2[ied1] == -2 );
	assert( tri[itri1].s2[ied1] == itri0 );

//	std::cout << itri0 << "-" << ied0 << "    " << itri1 << "-" << ied1 << std::endl;

	STri3D old0 = tri[itri0];
	STri3D old1 = tri[itri1];

	const unsigned int no0_0 = ied0;
	const unsigned int no1_0 = noelTriEdge[ied0][0];
	const unsigned int no2_0 = noelTriEdge[ied0][1];

	const unsigned int no0_1 = ied1;
	const unsigned int no1_1 = noelTriEdge[ied1][0];
	const unsigned int no2_1 = noelTriEdge[ied1][1];
	
	assert( old0.v[no1_0] == old1.v[no2_1] );
	assert( old0.v[no2_0] == old1.v[no1_1] );

	{
		STri3D& ref_tri = tri[itri0];
		////////////////
		ref_tri.v[0]  = old0.v[no1_0];	ref_tri.v[1]  = old1.v[no0_1];	ref_tri.v[2]  = old0.v[no0_0];
		ref_tri.g2[0] = -2;				ref_tri.g2[1] = old0.g2[no2_0];	ref_tri.g2[2] = old1.g2[no1_1];
		ref_tri.s2[0] = itri1;			ref_tri.s2[1] = old0.s2[no2_0];	ref_tri.s2[2] = old1.s2[no1_1];
		////////////////
		ref_tri.r2[0] = 0;
		if( old0.g2[no2_0] == -2 || old0.g2[no2_0] == -3 ){
			assert( old0.r2[no2_0] < 3 );
			const unsigned int* rel = relTriTri[ old0.r2[no2_0] ];
			assert( old0.s2[no2_0] < tri.size() );
			assert( old0.s2[no2_0] != itri0 );
			assert( old0.s2[no2_0] != itri1 );
			ref_tri.r2[1] = noel2RelTriTri[ rel[no1_0]*3 + rel[no2_0] ];
			assert( ref_tri.r2[1] >= 0 && ref_tri.r2[1] < 3 );
			tri[ old0.s2[no2_0] ].s2[ rel[no2_0] ] = itri0;
			tri[ old0.s2[no2_0] ].r2[ rel[no2_0] ] = invRelTriTri[ ref_tri.r2[1] ];
		}
		if( old1.g2[no1_1] == -2 || old1.g2[no1_1] == -3 ){
			assert( old1.r2[no1_1] < 3 );
			const unsigned int* rel = relTriTri[ old1.r2[no1_1] ];
			assert( old1.s2[no1_1] < tri.size() );
			ref_tri.r2[2] = noel2RelTriTri[ rel[no2_1]*3 + rel[no0_1] ];
			assert( ref_tri.r2[2] >= 0 && ref_tri.r2[2] < 3 );
			tri[ old1.s2[no1_1] ].s2[ rel[no1_1] ] = itri0;
			tri[ old1.s2[no1_1] ].r2[ rel[no1_1] ] = invRelTriTri[ ref_tri.r2[2] ];			
		}
	}

	{
		STri3D& ref_tri = tri[itri1];
		////////////////
		ref_tri.v[0] = old1.v[no1_1];	ref_tri.v[1]  = old0.v[no0_0];	ref_tri.v[2]  = old1.v[no0_1];
		ref_tri.g2[0] = -2;				ref_tri.g2[1] = old1.g2[no2_1];	ref_tri.g2[2] = old0.g2[no1_0];
		ref_tri.s2[0] = itri0;			ref_tri.s2[1] = old1.s2[no2_1];	ref_tri.s2[2] = old0.s2[no1_0];
		////////////////
		ref_tri.r2[0] = 0;
		if( old1.g2[no2_1] == -2 || old1.g2[no2_1] == -3 ){
			assert( old1.r2[no2_1] < 3 );
			const unsigned int* rel = relTriTri[ old1.r2[no2_1] ];
			assert( old1.s2[no2_1] < tri.size() );
			ref_tri.r2[1] = noel2RelTriTri[ rel[no1_1]*3 + rel[no2_1] ];
			assert( ref_tri.r2[1] >= 0 && ref_tri.r2[1] < 3 );
			tri[ old1.s2[no2_1] ].s2[ rel[no2_1] ] = itri1;
			tri[ old1.s2[no2_1] ].r2[ rel[no2_1] ] = invRelTriTri[ ref_tri.r2[1] ];
		}
		if( old0.g2[no1_0] == -2 || old0.g2[no1_0] == -3 ){
			assert( old0.r2[no1_0] < 3 );
			const unsigned int* rel = relTriTri[ old0.r2[no1_0] ];
			assert( old0.s2[no1_0] < tri.size() );
			ref_tri.r2[2] = noel2RelTriTri[ rel[no2_0]*3 + rel[no0_0] ];
			assert( ref_tri.r2[2] >= 0 && ref_tri.r2[2] < 3 );
			tri[ old0.s2[no1_0] ].s2[ rel[no1_0] ] = itri1;
			tri[ old0.s2[no1_0] ].r2[ rel[no1_0] ] = invRelTriTri[ ref_tri.r2[2] ];
		}
	}
	return true;
}

bool Msh::DelaunayAroundPointTri(unsigned int itri_st, 
	unsigned int inotri_st, 
	std::vector<Com::CVector3D>& aVec,
	std::vector<STri3D>& aTri)
{

	/*
	�܂����R�ꂪ����悤�ȂR�p�`���b�V���ɂ͑Ή����Ă��Ȃ��D
	*/

	// ��������w�b�_�[�t�@�C���ɂ܂Ƃ߂�
	static const unsigned int relTriTri[3][3] = {
		{ 0, 2, 1 }, //  0
		{ 2, 1, 0 }, //  1 
		{ 1, 0, 2 }, //  2
	};
	const unsigned int indexRot3[3][3] = { 
		{ 0, 1, 2 },
		{ 1, 2, 0 },
		{ 2, 0, 1 },
	};
	// �����܂�

	const unsigned int ipo0 = aTri[itri_st].v[inotri_st];
	int itri_prev = -1;
    unsigned int inotri_prev = 0;
	unsigned int itri_cur = itri_st;
	unsigned int inotri_cur = inotri_st;
	for(;;){
		assert( aTri[itri_cur].v[inotri_cur] == ipo0 );
		assert( aTri[itri_cur].g2[inotri_cur] == -2 );
		{
			const unsigned int inotri1 = indexRot3[1][inotri_cur];
			const unsigned int inotri2 = indexRot3[2][inotri_cur];
			const unsigned int ipo1 = aTri[itri_cur].v[inotri1];
			const unsigned int ipo2 = aTri[itri_cur].v[inotri2];
			const unsigned int itri_d = aTri[itri_cur].s2[inotri_cur];
			const unsigned int* rel_d = relTriTri[ aTri[itri_cur].r2[inotri_cur] ];
			const unsigned int inotri_d = rel_d[inotri_cur];
			const unsigned int ipo_d = aTri[itri_d].v[inotri_d];
			bool iflg = false;
			{
				Com::CVector3D norm1, norm2;
				Com::UnitNormal(norm1,aVec[ipo0],aVec[ipo1],aVec[ipo2]);
				Com::UnitNormal(norm2,aVec[ipo_d],aVec[ipo2],aVec[ipo1]);
				const double dot = Com::Dot(norm1,norm2);
				if( dot > 1.0-1.0e-5 ) iflg = true;
			}
			if( iflg && DetDelaunay(aVec[ipo0],aVec[ipo1],aVec[ipo2],aVec[ipo_d]) == -1 ){
				FlipEdgeTri(itri_cur,inotri_cur,aTri);
//				assert( CheckTri(aTri) );
				if( itri_prev == -1 ){	// ��Ԏn�߂̗v�f���������󂯂��ꍇ
					inotri_cur = 2;
					assert( aTri[itri_cur].v[inotri_cur] == ipo0 );
					continue;
				}
				else{
					itri_cur = itri_prev;
					inotri_cur = inotri_prev;
					assert( aTri[itri_cur].v[inotri_cur] == ipo0 );
				}
			}
		}

		{	// ���̗v�f�֐i�߂�
			const unsigned int inotri1 = indexRot3[1][inotri_cur];
			if( aTri[itri_cur].g2[inotri1] != -2 && aTri[itri_cur].g2[inotri1] != -3 ){
//				std::cout << "������" << std::endl;
				assert(0);
			}
			unsigned int itri_nex = aTri[itri_cur].s2[inotri1];
			const unsigned int* rel_nex = relTriTri[ aTri[itri_cur].r2[inotri1] ];
			const unsigned int inotri_nex = rel_nex[inotri_cur];
			assert( aTri[itri_nex].v[inotri_nex] == ipo0 );
			if( itri_nex == itri_st ) break;	// ���������I���
			itri_prev = itri_cur;		// ���̗v�f��DelaunayOK�Ȃ̂łP�O�ɃZ�b�g
			inotri_prev = inotri_cur;
			itri_cur = itri_nex;
			inotri_cur = inotri_nex;
		}
	}

	return true;
}


bool Msh::DelaunayAroundPointTet(const unsigned int ipoin,
						 std::vector<CPoint3D>& aPo,
						 std::vector<STet>& aTet){

	std::cout << " Delaunay Aruound Point 3D" << std::endl;

	std::stack< std::pair<unsigned int,unsigned int> > ball;

	{
		unsigned int itet0 = aPo[ipoin].e;
		unsigned int inoel0 = aPo[ipoin].poel;
		assert( aTet[itet0].v[inoel0] == ipoin );
		ElemAroundPoint elarpo;
		Msh::MakeElemAroundPoint(elarpo,itet0,inoel0,aTet);
		for(std::map<int,unsigned int>::iterator itr = elarpo.e.begin();itr!=elarpo.e.end();itr++){
			unsigned int itet1 = itr->first;
			unsigned int inoel1 = itr->second;
			assert( aTet[itet1].v[inoel1] == ipoin );
			ball.push( std::make_pair(itet1,inoel1) );
		}
	}

	for(;!ball.empty();){
		unsigned int itet1 = ball.top().first;
		unsigned int inoel1 = ball.top().second;
		assert( aTet[itet1].v[inoel1] == ipoin );
		ball.pop();
		if( aTet[itet1].g[inoel1] != -2 ){ continue; }
		unsigned int itet_a = aTet[itet1].s[inoel1];
		assert( itet_a < aTet.size() );
		unsigned int inoel_a = tetRel[ aTet[itet1].f[inoel1] ][inoel1];
		assert( aTet[itet_a].g[inoel_a] == -2 );
		assert( aTet[itet_a].s[inoel_a] == itet1 );
		unsigned int ipo_dia = aTet[itet_a].v[inoel_a];
		bool is_swap = false;
/*		{
			double det = Msh::DetDelaunay3D(
				aPo[ aTet[itet1].v[0] ].p,
				aPo[ aTet[itet1].v[1] ].p,
				aPo[ aTet[itet1].v[2] ].p,
				aPo[ aTet[itet1].v[3] ].p,
				aPo[ ipo_dia ].p);
			if( det < -1.0e-10 ){
				double vol0 = Msh::TetVolume( aTet[itet1].v[inoel1], ipo_dia,  aTet[itet1].v[ noelTetFace[inoel1][2] ], aTet[itet1].v[ noelTetFace[inoel1][0] ], aPo );
				double vol1 = Msh::TetVolume( aTet[itet1].v[inoel1], ipo_dia,  aTet[itet1].v[ noelTetFace[inoel1][1] ], aTet[itet1].v[ noelTetFace[inoel1][2] ], aPo );
				double vol2 = Msh::TetVolume( aTet[itet1].v[inoel1], ipo_dia,  aTet[itet1].v[ noelTetFace[inoel1][0] ], aTet[itet1].v[ noelTetFace[inoel1][1] ], aPo );
				if( vol0 > 1.0e-10 && vol1 > 1.0e-10 && vol2 > 1.0e-10 ){ is_swap = true; }
			}
		}*/
		{
			const double crt1 = Criterion_PLJ(itet1,aPo,aTet);
			const double crt_a = Criterion_PLJ(itet_a,aPo,aTet);
			double max_crt;
			GetFaceSwapCrt(itet1,inoel1,max_crt,aTet,aPo);
			if( max_crt < crt1 && max_crt < crt_a ){ is_swap = true; }
		}
		if( is_swap ){
			Msh::FaceSwapTet(itet1,inoel1,aTet,aPo);
			assert( CheckTet(aTet,aPo) );
			ball.push( std::make_pair(itet1,0) );
			ball.push( std::make_pair(itet_a,0) );
			ball.push( std::make_pair(aTet.size()-1,0) );
			continue;
		}

		continue;

		double vol0 = Msh::TetVolume( aTet[itet1].v[inoel1], ipo_dia,  aTet[itet1].v[ noelTetFace[inoel1][2] ], aTet[itet1].v[ noelTetFace[inoel1][0] ], aPo );
		double vol1 = Msh::TetVolume( aTet[itet1].v[inoel1], ipo_dia,  aTet[itet1].v[ noelTetFace[inoel1][1] ], aTet[itet1].v[ noelTetFace[inoel1][2] ], aPo );
		double vol2 = Msh::TetVolume( aTet[itet1].v[inoel1], ipo_dia,  aTet[itet1].v[ noelTetFace[inoel1][0] ], aTet[itet1].v[ noelTetFace[inoel1][1] ], aPo );

		unsigned int idedge0;
		if( vol0 < 0 && vol1 > 0 && vol2 > 0 ){
			idedge0 = noel2DEdge[ noelTetFace[inoel1][2]*3+noelTetFace[inoel1][0] ];
		}
		if( vol0 > 0 && vol1 < 0 && vol2 > 0 ){
			idedge0 = noel2DEdge[ noelTetFace[inoel1][1]*3+noelTetFace[inoel1][2] ];
		}
		if( vol0 > 0 && vol1 > 0 && vol2 < 0 ){
			idedge0 = noel2DEdge[ noelTetFace[inoel1][0]*3+noelTetFace[inoel1][1] ];
		}
		else{ continue; }

		ElemAroundEdge elared;
		MakeElemAroundEdge(elared,itet1,idedge0,aTet);
		int iptn;
		double max_crt1;
		if( !GetEdgeSwapPtnCrt(elared,iptn,max_crt1,aPo) ) continue;
		if( iptn == -1 ) continue;
		const double max_crt0 = MaxCrtElemAroundEdge(elared,aTet,aPo);
		if( max_crt0 < max_crt1*2 ) continue;
		std::cout << elared.size() << " " << max_crt0 << " " << max_crt1 << " " << iptn << std::endl;
		EdgeSwapTet(elared,iptn,aTet,aPo);
		getchar();
	}

	return true;
}















bool Msh::operator < (const CFlipCrtPrePosPtn& lhs, const CFlipCrtPrePosPtn& rhs)
{
	if(  lhs.ok_flg && !rhs.ok_flg ) return true;
	if( !lhs.ok_flg &&  rhs.ok_flg ) return false;
	if( fabs( lhs.pre - rhs.pre ) > 1.0e-10 ){
		return lhs.pre > rhs.pre;
	}
	if( fabs( lhs.pos - rhs.pos ) > 1.0e-10 ){
		return lhs.pos < rhs.pos;
	}
	return false;
}

////////////////////////////////////////////////
inline int TetType_Sugi(const int itet0,
				   const std::vector<CPoint3D>& node,
				   const std::vector<STet>& tet )
{
	const double sq_circumradius = Com::SquareCircumradius(
		node[ tet[itet0].v[0] ].p,
		node[ tet[itet0].v[1] ].p,
		node[ tet[itet0].v[2] ].p,
		node[ tet[itet0].v[3] ].p );
	const double sq_longest = SquareLongestEdgeLength(itet0,node,tet);
	const double alpha = 3.0;
	if( sq_circumradius > alpha*alpha*sq_longest ){ return 2; }
	else{
		const double sq_shortest = SquareShortestEdgeLength(itet0,node,tet);
		if( sq_longest > alpha*alpha*sq_shortest ){ return 1; }
		else{ return 3; }
	}
	return -1;
}


bool Msh::ReconnectCap(
					const unsigned int itet0,
					std::vector<STet>& tet,
					std::vector<CPoint3D>& node){
	const int nfatet = 4;
	
	Com::CVector3D fnorm[nfatet];
	for(int ifatet=0;ifatet<nfatet;ifatet++){
		UnitNormal(fnorm[ifatet],itet0,ifatet,tet,node);
	}
	std::map<double, int> map_angle_edge;
	{
		std::pair<double,int> pair_di; 
        for(unsigned int isedge=0;isedge<nSEdgeTet;isedge++){
			pair_di.first = -1.0*Com::Dot( fnorm[ sEdge2FaTet[isedge][0] ],fnorm[ sEdge2FaTet[isedge][1] ] );
			pair_di.second = isedge;
			map_angle_edge.insert( pair_di );
		}
	}
	int swap_face = -1;
	{
		int swap_face_flag[nfatet] = { 1, 1, 1, 1 };
		std::map<double,int>::const_iterator itr = map_angle_edge.begin();
		int isedge0 = itr->second;
		swap_face_flag[ sEdge2FaTet[isedge0][0] ] = 0;
		swap_face_flag[ sEdge2FaTet[isedge0][1] ] = 0;
		itr++;
		isedge0 = itr->second;
		swap_face_flag[ sEdge2FaTet[isedge0][0] ] = 0;
		swap_face_flag[ sEdge2FaTet[isedge0][1] ] = 0;
		itr++;
		isedge0 = itr->second;
		swap_face_flag[ sEdge2FaTet[isedge0][0] ] = 0;
		swap_face_flag[ sEdge2FaTet[isedge0][1] ] = 0;
		for(int iface=0;iface<nfatet;iface++){
			if( swap_face_flag[iface] == 1 ){ 
				if( swap_face != -1 ){
					swap_face = -1;
				}
				swap_face = iface;	
			}
		}
	}
	if( swap_face != -1 ){
		assert( swap_face >= 0 && swap_face < 4 );
		if( tet[itet0].g[swap_face] == -2 ){
			const int itet_adj = tet[itet0].s[swap_face];
			const double tet_crt_pre = Criterion_PLJ(tet[itet0],node);
			const double adj_crt_pre = Criterion_PLJ(tet[itet_adj],node);
			const double max_crt_pre = ( tet_crt_pre > adj_crt_pre ) ? tet_crt_pre : adj_crt_pre;
			double max_crt_pos;
			GetFaceSwapCrt(itet0,swap_face,max_crt_pos,tet,node);
//			std::cout << "Face Swap " << swap_face << " " << max_crt_pre << " " << max_crt_pos << std::endl;
			if( max_crt_pos < max_crt_pre ){
				std::cout << "Face Swap " << itet0 << " " << swap_face << " " << " " << max_crt_pre << " " << max_crt_pos << std::endl;
				Msh::FaceSwapTet(itet0,swap_face,tet,node);
//				CheckTet(tet,node);
				return true;
			}
		}
	}
	return false;
}

bool Msh::ReconnectSliver(
					const unsigned int itet0,
					std::vector<STet>& tet,
					std::vector<CPoint3D>& node)
{

	const unsigned int nfatet = 4;

	Com::CVector3D fnorm[4];
	for(unsigned int ifatet=0;ifatet<nfatet;ifatet++){
		UnitNormal(fnorm[ifatet],itet0,ifatet,tet,node);
	}
	std::map<double, int> map_angle_edge;
	{
		std::pair<double,int> pair_di; 
        for(unsigned int isedge=0;isedge<nSEdgeTet;isedge++){
			pair_di.first = -1.0*Com::Dot( fnorm[ sEdge2FaTet[isedge][0] ],fnorm[ sEdge2FaTet[isedge][1] ] );
			pair_di.second = isedge;
			map_angle_edge.insert( pair_di );
		}
	}
	int idedge0,idedge1;
	{
		std::map<double,int>::const_iterator itr = map_angle_edge.begin();
		idedge0 = sEdge2DEdge[itr->second];
		itr++;
		idedge1 = sEdge2DEdge[itr->second];
	}

	ElemAroundEdge elared0, elared1;
	CFlipCrtPrePosPtn fcppp0,fcppp1;
	{
		int ptn0;
		double max_crt_pre0,max_crt_pos0;
		MakeElemAroundEdge(elared0,itet0,idedge0,tet);
		if( elared0.is_inner ){
			max_crt_pre0 = MaxCrtElemAroundEdge(elared0,tet,node);
			GetEdgeSwapPtnCrt(elared0,ptn0,max_crt_pos0,node);
		}
		else{
			max_crt_pre0 = -1.0;
			max_crt_pos0 = 1.0e+20;
			ptn0 = -1;
		}
		fcppp0.SetData(max_crt_pre0, max_crt_pos0, ptn0);
	}
	{
		int ptn1;
		double max_crt_pre1,max_crt_pos1;
		MakeElemAroundEdge(elared1,itet0,idedge1,tet);
		if( elared1.is_inner ){
			max_crt_pre1 = MaxCrtElemAroundEdge(elared1,tet,node);
			GetEdgeSwapPtnCrt(elared1,ptn1,max_crt_pos1,node);
		}
		else{
			max_crt_pre1 = -1.0;
			max_crt_pos1 = 1.0e+20;
			ptn1 = -1;
		}
		fcppp1.SetData(max_crt_pre1, max_crt_pos1, ptn1);
	}

//	std::cout << " --> " << itet << " " << elared0.size() << " " << elared1.size() << " " << Criterion_PLJ(tet[itet],node) << " " << max_crt_pos0 << " " << max_crt_pos1 << std::endl; 
	if( fcppp0<fcppp1 && fcppp0.ok_flg ){
		std::cout << "Reconect0   " << elared0.size() << " " << fcppp0.pre << "-->" << fcppp0.pos << " " << itet0 << " " << tet.size() << std::endl;
		EdgeSwapTet(elared0,fcppp0.ptn,tet,node);
//		CheckTet(tet,node);
		return true;
	}
	if( fcppp1<fcppp0 && fcppp1.ok_flg ){
		std::cout << "Reconect1   " << elared1.size() << " " << fcppp1.pre << "-->" << fcppp1.pos << " " << itet0 << " " << tet.size() << std::endl;
		EdgeSwapTet(elared1,fcppp1.ptn,tet,node);
//		CheckTet(tet,node);
		return true;
	}

	return false;
}


bool Msh::Reconnect(std::vector<STet>& aTet,
			   std::vector<CPoint3D>& aPo)
{
	double threthold = 15.0;
	for(unsigned int istep=0;istep<3;istep++){
		for(unsigned int itet=0;itet<aTet.size();itet++){
			const double tet_crt_pre = Criterion_PLJ(aTet[itet],aPo);
			if( tet_crt_pre < threthold ) continue;
			const int itype = TetType_Sugi(itet,aPo,aTet);
			switch( itype ){
				case 1 : 
					break;
				case 2 :
				 	ReconnectCap(itet,aTet,aPo);
					break;
				case 3 :
					ReconnectSliver(itet,aTet,aPo);
					break;
				default :
					assert(0);
			}
		}
	}

	{
		ElemAroundEdge elared[6];
		std::map< CFlipCrtPrePosPtn, int > map_fcppp;
		for(unsigned int itet=0;itet<aTet.size();itet++){
			const double tet_crt_pre = Criterion_PLJ(aTet[itet],aPo);
			if( tet_crt_pre < threthold ) continue;
			const int itype = TetType_Sugi(itet,aPo,aTet);
			if( tet_crt_pre > 50.0 ){
				std::cout << itet << " " << itype << " " << tet_crt_pre << std::endl;
			}

			map_fcppp.clear();
			for(int isedge=0;isedge<6;isedge++){
				double max_crt_pre;
				double max_crt_pos;
				int ptn;
				MakeElemAroundEdge(elared[isedge],itet,sEdge2DEdge[isedge],aTet);
				std::cout << itet << " " << isedge << " " << elared[isedge].size() << std::endl;
				if( elared[isedge].is_inner ){
					max_crt_pre = MaxCrtElemAroundEdge(elared[isedge],aTet,aPo);
					GetEdgeSwapPtnCrt(elared[isedge],ptn,max_crt_pos,aPo);
				}
				else{
					ptn = -1;
					max_crt_pre = -1.0;
					max_crt_pos = ILL_CRT;
				}
				std::pair< CFlipCrtPrePosPtn, int > pair_fi( CFlipCrtPrePosPtn(max_crt_pre,max_crt_pos,ptn), isedge );
//				std::cout << "  " << isedge << " " << max_crt_pre << " " << max_crt_pos << std::endl;
				map_fcppp.insert( pair_fi );
			}
			{
				std::map< CFlipCrtPrePosPtn , int >::const_iterator itr_fcppp = map_fcppp.begin();
//				std::cout << "        swap foo	" << itet << " " << itr_fcppp->second << " " << elared[itr_fcppp->second].size() << " " << itr_fcppp->first.ptn << " --> " << itr_fcppp->first.pre << " " << itr_fcppp->first.pos << std::endl;
				if( itr_fcppp->first.ok_flg ){
					const int isedge0 = itr_fcppp->second;
					const int iptn0 = itr_fcppp->first.ptn;
					std::cout << "Reconect   " << isedge0 << " " << elared[isedge0].size() << " " << itr_fcppp->first.pre << "-->" << itr_fcppp->first.pos << std::endl;
					EdgeSwapTet(elared[isedge0],iptn0,aTet,aPo);
//					CheckTet(tet,node);
					continue;
				}
			}
		}
	}

	return true;
}
