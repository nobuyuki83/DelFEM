/*
 *  spatial_hash_grid.cpp
 *  cad
 *
 *  Created by 梅谷 信行 on 8/27/10.
 *  Copyright 2010 The University of Tokyo. All rights reserved.
 *
 */

#include <vector>
#include <assert.h>
#include <math.h>
#include <iostream>

#include "delfem/spatial_hash_grid3d.h"
 


CSpatialHash_Grid3D::CSpatialHash_Grid3D(unsigned int ndiv, double center[3], double half_width)
{
	this->ndiv_ = ndiv;
	org_[0] = center[0]-half_width;
	org_[1] = center[1]-half_width;
	org_[2] = center[2]-half_width;
	width_ = half_width*2;
	invcellwidth_ = ndiv/width_;
	aIndTri_.reserve(8*ndiv*ndiv*ndiv*1.5);
	aIndTri_.resize(8*ndiv*ndiv*ndiv,0);
}

void CSpatialHash_Grid3D::AddTri(int itri, double p0[3], double p1[3], double p2[3])
{
	int ip0[3]; GetIndex(p0,ip0);
	int ip1[3]; GetIndex(p1,ip1);
	int ip2[3]; GetIndex(p2,ip2);			
	int imin[3] = { ip0[0], ip0[1], ip0[2] };
	{
		imin[0] = ( imin[0] < ip1[0] ) ? imin[0] : ip1[0];
		imin[0] = ( imin[0] < ip2[0] ) ? imin[0] : ip2[0];
		imin[1] = ( imin[1] < ip1[1] ) ? imin[1] : ip1[1];
		imin[1] = ( imin[1] < ip2[1] ) ? imin[1] : ip2[1];
		imin[2] = ( imin[2] < ip1[2] ) ? imin[2] : ip1[2];
		imin[2] = ( imin[2] < ip2[2] ) ? imin[2] : ip2[2];
	}
	int imax[3] = { ip0[0], ip0[1], ip0[2] };
	{
		imax[0] = ( imax[0] > ip1[0] ) ? imax[0] : ip1[0];
		imax[0] = ( imax[0] > ip2[0] ) ? imax[0] : ip2[0];
		imax[1] = ( imax[1] > ip1[1] ) ? imax[1] : ip1[1];
		imax[1] = ( imax[1] > ip2[1] ) ? imax[1] : ip2[1];
		imax[2] = ( imax[2] > ip1[2] ) ? imax[2] : ip1[2];
		imax[2] = ( imax[2] > ip2[2] ) ? imax[2] : ip2[2];
	}						
	for(unsigned int i=imin[0];i<=imax[0];i++){
		for(unsigned int j=imin[1];j<=imax[1];j++){
			for(unsigned int k=imin[2];k<=imax[2];k++){
//        std::cout << "Add Tri" << itri << "  " << i << " " << j << " " << k << std::endl;
				this->AddData(i,j,k, itri);
			}
		}
	}
}

void CSpatialHash_Grid3D::AddData(unsigned int i, unsigned int j, unsigned int k, int idata)
{
	assert( i < ndiv_ );
	assert( j < ndiv_ );
	assert( k < ndiv_ );	
	int ind = (i*ndiv_*ndiv_+j*ndiv_+k);
	for(;;){
		int isize = aIndTri_[ind*8];
		if (isize>=0 ){
			if( isize<7) {
				aIndTri_[ind*8+isize+1] = idata;
				aIndTri_[ind*8] += 1;
				return;
			}
			else if(isize==7){
				assert( aIndTri_.size() % 8 == 0 );
				unsigned int ind1 = aIndTri_.size()/8;
				assert(ind1!=0);
				aIndTri_[ind*8] = -ind1;
				aIndTri_.resize((ind1+1)*8,0);
				aIndTri_[ind1*8+0] = 1;
				aIndTri_[ind1*8+1] = idata;
				return;
			}
		}
		else {
			assert(isize<0);
			assert(aIndTri_.size()>-isize*8);
			ind = -isize;
		}		
	}
}

void CSpatialHash_Grid3D::BuildOutFlg()
{	
	aFlgOut.clear();
	aFlgOut.resize(ndiv_*ndiv_*ndiv_,0);	// in:0
	for(unsigned int i=0;i<ndiv_;i++){
		for(unsigned int j=0;j<ndiv_;j++){
			int k;
			for(k=0;k<ndiv_;k++){
				if( aIndTri_[(i*ndiv_*ndiv_+j*ndiv_+k)*8] != 0 ) break;
				aFlgOut[i*ndiv_*ndiv_+j*ndiv_+k] = 1;
			}
			if( k == ndiv_ ) continue;
			for(k=ndiv_-1;k>=0;k--){
				if( aIndTri_[(i*ndiv_*ndiv_+j*ndiv_+k)*8] != 0 ) break;
				aFlgOut[i*ndiv_*ndiv_+j*ndiv_+k] = 1;
			}		
		}
	}
	////
	for(unsigned int i=0;i<ndiv_;i++){
		for(unsigned int k=0;k<ndiv_;k++){
			int j;
			for(j=0;j<ndiv_;j++){
				if( aIndTri_[(i*ndiv_*ndiv_+j*ndiv_+k)*8] != 0 ) break;
				aFlgOut[i*ndiv_*ndiv_+j*ndiv_+k] = 1;
			}
			if( j == ndiv_ ) continue;
			for(j=ndiv_-1;j>=0;j--){
				if( aIndTri_[(i*ndiv_*ndiv_+j*ndiv_+k)*8] != 0 ) break;
				aFlgOut[i*ndiv_*ndiv_+j*ndiv_+k] = 1;
			}		
		}
	}	
	////
	for(unsigned int j=0;j<ndiv_;j++){
		for(unsigned int k=0;k<ndiv_;k++){
			int i;
			for(i=0;i<ndiv_;i++){
				if( aIndTri_[(i*ndiv_*ndiv_+j*ndiv_+k)*8] != 0 ) break;
				aFlgOut[i*ndiv_*ndiv_+j*ndiv_+k] = 1;
			}
			if( i == ndiv_ ) continue;
			for(i=ndiv_-1;i>=0;i--){
				if( aIndTri_[(i*ndiv_*ndiv_+j*ndiv_+k)*8] != 0 ) break;
				aFlgOut[i*ndiv_*ndiv_+j*ndiv_+k] = 1;
			}		
		}
	}		
}

// true:out  false:not obvious
bool CSpatialHash_Grid3D::IsOut(double p[3]) const
{
	if( this->aFlgOut.size() != ndiv_*ndiv_*ndiv_ ) return false;
	int ip[3];
	this->GetIndex(p,ip);
//  std::cout << ndiv_ << " " << ip[0] << " " << ip[1] << " " << ip[2] << std::endl;
  if( ip[0] < 0 || ip[0] >= ndiv_ ) return true;
  if( ip[1] < 0 || ip[1] >= ndiv_ ) return true;  
  if( ip[2] < 0 || ip[2] >= ndiv_ ) return true;
	if( aFlgOut[ip[0]*ndiv_*ndiv_ + ip[1]*ndiv_ + ip[2] ] == 1 ) return true;	
	return false;
}



void CSpatialHash_Grid3D::SetData
(int i, int j, int k,  
 std::vector<unsigned int>& aIndTriCand)
{	
	if( i < 0 || i >= ndiv_ ) return;
	if( j < 0 || j >= ndiv_ ) return;	
	if( k < 0 || k >= ndiv_ ) return;	
  //  std::cout << i << " " << j << " " << k << " " << aIndTri_[(i*ndiv_*ndiv_+j*ndiv_+k)*8] << std::endl;
	if( aIndTri_[(i*ndiv_*ndiv_+j*ndiv_+k)*8] == 0 ) return;
	// extract cell if the minimum distance between a certain point in ijk and in ip is smaller than idist+1
	int ind = (i*ndiv_*ndiv_+j*ndiv_+k);
	for(;;){
		int isize = aIndTri_[ind*8];
		assert( isize != 0 );
		if( isize > 0 ){
			assert( isize < 8 );	// the first is used for size
			for(unsigned int idat=0;idat<isize;idat++){
				aIndTriCand.push_back(aIndTri_[ind*8+idat+1]);
			}
			break;
		}
		else{
			for(unsigned int idat=0;idat<7;idat++){
				aIndTriCand.push_back(aIndTri_[ind*8+idat+1]);						
			}
			ind = -isize;
		}
	}	
}

void CSpatialHash_Grid3D::Find_NearestTriCand
(double p[3], 
 std::vector<unsigned int>& aIndTriCand)
{	
	aIndTriCand.resize(0);
	int ip[3];	this->GetIndex(p,ip);	
	bool flg = false;
	//	std::cout << "search near" << ip[0] << " " << ip[1] << " " << ip[2] << std::endl;
	for(int idist=0;idist<ndiv_*3;idist++){	// idist must be int (negative appear intermidiate calc)
		unsigned int imin = (ip[0]-idist <  0    ) ? 0       : ip[0]-idist;
		unsigned int imax = (ip[0]+idist >= ndiv_) ? ndiv_-1 : ip[0]+idist;
		unsigned int jmin = (ip[1]-idist <  0    ) ? 0       : ip[1]-idist;
		unsigned int jmax = (ip[1]+idist >= ndiv_) ? ndiv_-1 : ip[1]+idist;
		unsigned int kmin = (ip[2]-idist <  0    ) ? 0       : ip[2]-idist;
		unsigned int kmax = (ip[2]+idist >= ndiv_) ? ndiv_-1 : ip[2]+idist;		
		//		std::cout << "dist range " << idist << "   " << imin << " " << imax << "  " << jmin << " " << jmax << "  " << kmin << " " << kmax << std::endl;
		for(unsigned int i=imin;i<=imax;i++){
			for(unsigned int j=jmin;j<=jmax;j++){
				for(unsigned int k=kmin;k<=kmax;k++){
					const unsigned int sqidist_min = (i-ip[0])*(i-ip[0]) + (j-ip[1])*(j-ip[1]) + (k-ip[2])*(k-ip[2]);
					if( sqidist_min >= (idist+1)*(idist+1) ) continue;
					if( sqidist_min < idist*idist ) continue;			
					if( aIndTri_[(i*ndiv_*ndiv_+j*ndiv_+k)*8] == 0 ) continue;	
					// extract cell if the minimum distance between a certain point in ijk and in ip is smaller than idist+1
					SetData(i,j,k, aIndTriCand);
				}
			}
		}
		if( flg ) break;
		if( !aIndTriCand.empty() ){ flg = true; }
	}
}

void CSpatialHash_Grid3D::Find_Intersec_Layer_X
(unsigned int i,
 const double p0[2], 
 const double p1[2],
 std::vector<unsigned int>& aIndTriCand)
{
	if( i < 0 || i >= ndiv_ ) return;
	const double h = width_/ndiv_;
	int j0 = floor(p0[0]*invcellwidth_);
	int k0 = floor(p0[1]*invcellwidth_);
	int j1 = floor(p1[0]*invcellwidth_);
	int k1 = floor(p1[1]*invcellwidth_);
	//	std::cout << j0 << " " << k0 << "  " << j1 << " " << k1 << std::endl;
	if( j0 == j1 ){		
		if( k0 <= k1 ){ for(int k=k0;k<=k1;k++){ SetData(i,j0,k, aIndTriCand); } return; }
		else {			    for(int k=k1;k<=k0;k++){ SetData(i,j0,k, aIndTriCand); } return; }
	}
	if( k0 == k1 ){		
		if( j0 <= j1 ){ for(int j=j0;j<=j1;j++){ SetData(i,j,k0, aIndTriCand); } return; }
		else {			    for(int j=j1;j<=j0;j++){ SetData(i,j,k0, aIndTriCand); } return; }
	}	
	if( j0 < j1 ){
		for(int j=j0;j<=j1;j++){
			int k_0 = k0;
			if(j!=j0){			
				const double r = (h*(j+0)-p0[0])/(p1[0]-p0[0]); assert( r>=0 && r<=1 );
				const double z_0 = (1.0-r)*p0[1]+r*p1[1];
				k_0 = floor(z_0*invcellwidth_);
			}
			int k_1 = k1;
			if(j!=j1){			
				const double r = (h*(j+1)-p0[0])/(p1[0]-p0[0]); assert( r>=0 && r<=1 );
				const double z_0 = (1.0-r)*p0[1]+r*p1[1];
				k_1 = floor(z_0*invcellwidth_);
			}
			if( k_0 <= k_1 ){ for(int k=k_0;k<=k_1;k++){ SetData(i,j,k, aIndTriCand); } }
			else {			      for(int k=k_1;k<=k_0;k++){ SetData(i,j,k, aIndTriCand); } }
		}
	}
	else {
		for(int j=j1;j<=j0;j++){
			int k_0 = k0;
			if(j!=j1){
				const double r = (h*(j+0)-p0[0])/(p1[0]-p0[0]); assert( r>=0 && r<=1 );
				const double z_0 = (1.0-r)*p0[1]+r*p1[1];
				k_0 = floor(z_0*invcellwidth_);
			}
			int k_1 = k1;
			if(j!=j0){								
				const double r = (h*(j+1)-p0[0])/(p1[0]-p0[0]); assert( r>=0 && r<=1 );
				const double z_0 = (1.0-r)*p0[1]+r*p1[1];
				k_1 = floor(z_0*invcellwidth_);
			}
			if( k_0 <= k_1 ){ for(int k=k_0;k<=k_1;k++){ SetData(i,j,k, aIndTriCand); } }
			else {			      for(int k=k_1;k<=k_0;k++){ SetData(i,j,k, aIndTriCand); } }			
		}
	}
}


void CSpatialHash_Grid3D::Find_Intersec_Layer_Y
(unsigned int j,  // y
 const double p0[2], // zx
 const double p1[2], // zx
 std::vector<unsigned int>& aIndTriCand)
{
	if( j < 0 || j >= ndiv_ ) return;
	const double h = width_/ndiv_;
	int k0 = floor(p0[0]*invcellwidth_);
	int i0 = floor(p0[1]*invcellwidth_);
	int k1 = floor(p1[0]*invcellwidth_);
	int i1 = floor(p1[1]*invcellwidth_);
	if( k0 == k1 ){		
		if( i0 <= i1 ){ for(int i=i0;i<=i1;i++){ SetData(i,j,k0, aIndTriCand); } return; }
		else {			    for(int i=i1;i<=i0;i++){ SetData(i,j,k0, aIndTriCand); } return; }
	}
	if( i0 == i1 ){		
		if( k0 <= k1 ){ for(int k=k0;k<=k1;k++){ SetData(i0,j,k, aIndTriCand); } return; }
		else {			    for(int k=k1;k<=k0;k++){ SetData(i0,j,k, aIndTriCand); } return; }
	}	
	if( k0 < k1 ){
		for(int k=k0;k<=k1;k++){
			int i_0 = i0;
			if(k!=k0){
				const double r = (h*(k+0)-p0[0])/(p1[0]-p0[0]); assert( r>=0 && r<=1 );
				const double x_0 = (1.0-r)*p0[1]+r*p1[1];
				i_0 = floor(x_0*invcellwidth_);
			}
			int i_1 = i1;
			if(k!=k1){			
				const double r = (h*(k+1)-p0[0])/(p1[0]-p0[0]); assert( r>=0 && r<=1 );
				const double x_0 = (1.0-r)*p0[1]+r*p1[1];
				i_1 = floor(x_0*invcellwidth_);
			}
			if( i_0 <= i_1 ){ for(int i=i_0;i<=i_1;i++){ SetData(i,j,k, aIndTriCand); } }
			else {			      for(int i=i_1;i<=i_0;i++){ SetData(i,j,k, aIndTriCand); } }
		}
	}
	else {
		for(int k=k1;k<=k0;k++){
			int i_0 = i0;
			if(k!=k1){
				const double r = (h*(k+0)-p0[0])/(p1[0]-p0[0]); assert( r>=0 && r<=1 );
				const double x_0 = (1.0-r)*p0[1]+r*p1[1];
				i_0 = floor(x_0*invcellwidth_);
			}
			int i_1 = i1;
			if(k!=k0){								
				const double r = (h*(k+1)-p0[0])/(p1[0]-p0[0]); assert( r>=0 && r<=1 );
				const double x_0 = (1.0-r)*p0[1]+r*p1[1];
				i_1 = floor(x_0*invcellwidth_);
			}
			if( i_0 <= i_1 ){ for(int i=i_0;i<=i_1;i++){ SetData(i,j,k, aIndTriCand); } }
			else {			      for(int i=i_1;i<=i_0;i++){ SetData(i,j,k, aIndTriCand); } }			
		}
	}
}



void CSpatialHash_Grid3D::Find_Intersec_Layer_Z
(unsigned int k,  // z
 const double p0[2], // xy
 const double p1[2], // xy
 std::vector<unsigned int>& aIndTriCand)
{
	if( k < 0 || k >= ndiv_ ) return;
	const double h = width_/ndiv_;
	int i0 = floor(p0[0]*invcellwidth_);
	int j0 = floor(p0[1]*invcellwidth_);
	int i1 = floor(p1[0]*invcellwidth_);
	int j1 = floor(p1[1]*invcellwidth_);
	if( i0 == i1 ){
		if( j0 <= j1 ){ for(int j=j0;j<=j1;j++){ SetData(i0,j,k, aIndTriCand); } return; }
		else {			    for(int j=j1;j<=j0;j++){ SetData(i0,j,k, aIndTriCand); } return; }
	}
	if( j0 == j1 ){		
		if( i0 <= i1 ){ for(int i=i0;i<=i1;i++){ SetData(i,j0,k, aIndTriCand); } return; }
		else {			    for(int i=i1;i<=i0;i++){ SetData(i,j0,k, aIndTriCand); } return; }
	}	
	if( i0 < i1 ){
		for(int i=i0;i<=i1;i++){
			int j_0 = j0;
			if(i!=i0){
				const double r = (h*(i+0)-p0[0])/(p1[0]-p0[0]); assert( r>=0 && r<=1 );
				const double y_0 = (1.0-r)*p0[1]+r*p1[1];
				j_0 = floor(y_0*invcellwidth_);
			}
			int j_1 = j1;
			if(i!=i1){			
				const double r = (h*(i+1)-p0[0])/(p1[0]-p0[0]); assert( r>=0 && r<=1 );
				const double y_0 = (1.0-r)*p0[1]+r*p1[1];
				j_1 = floor(y_0*invcellwidth_);
			}
			if( j_0 <= j_1 ){ for(int j=j_0;j<=j_1;j++){ SetData(i,j,k, aIndTriCand); } }
			else {			      for(int j=j_1;j<=j_0;j++){ SetData(i,j,k, aIndTriCand); } }
		}
	}
	else {
		for(int i=i1;i<=i0;i++){
			int j_0 = j0;
			if(i!=i1){
				const double r = (h*(i+0)-p0[0])/(p1[0]-p0[0]); assert( r>=0 && r<=1 );
				const double y_0 = (1.0-r)*p0[1]+r*p1[1];
				j_0 = floor(y_0*invcellwidth_);
			}
			int j_1 = j1;
			if(i!=i0){								
				const double r = (h*(i+1)-p0[0])/(p1[0]-p0[0]); assert( r>=0 && r<=1 );
				const double y_0 = (1.0-r)*p0[1]+r*p1[1];
				j_1 = floor(y_0*invcellwidth_);
			}
			if( j_0 <= j_1 ){ for(int j=j_0;j<=j_1;j++){ SetData(i,j,k, aIndTriCand); } }
			else {			      for(int j=j_1;j<=j_0;j++){ SetData(i,j,k, aIndTriCand); } }			
		}
	}
}



void CSpatialHash_Grid3D::Find_IntersecTriCand
(const double _p[3], 
 const double d[3],
 std::vector<unsigned int>& aIndTriCand)
{	
	aIndTriCand.resize(0);
	const double p[3] = {_p[0]-org_[0], _p[1]-org_[1], _p[2]-org_[2] };
  const double h = width_/ndiv_;  
  if( fabs(d[0]) > fabs(d[1]) && fabs(d[0]) > fabs(d[2]) ){
    int ix0 = floor(p[0]*invcellwidth_);
    bool iflg = false;
    if(      d[0] > +1.0e-10 ){
      if( ix0 < 0 ){ ix0 = 0; }
      for(int i=ix0;i<ndiv_;i++){
        double p0[2] = { ((i+0)*h-p[0])/d[0]*d[1]+p[1], ((i+0)*h-p[0])/d[0]*d[2]+p[2] };
        double p1[2] = { ((i+1)*h-p[0])/d[0]*d[1]+p[1], ((i+1)*h-p[0])/d[0]*d[2]+p[2] };
        if( i==ix0 ){ p0[0] = p[1]; p0[1] = p[2]; }
        else if( p0[0] < 0 || p0[0] > width_ || p0[1] < 0 || p0[1] > width_ ){
          if( iflg ){ break; }
          else{ continue; }
        }
        if( i!=ix0 ){ iflg = true; }
        Find_Intersec_Layer_X(i,p0,p1,aIndTriCand);
      }
    }
    else if( d[0] < -1.0e-10 ){
      if( ix0 >= ndiv_ ){ ix0 = ndiv_-1; }
      for(int i=ix0;i>=0;i--){
        double p0[2] = { ((i+1)*h-p[0])/d[0]*d[1]+p[1], ((i+1)*h-p[0])/d[0]*d[2]+p[2] };
        double p1[2] = { ((i+0)*h-p[0])/d[0]*d[1]+p[1], ((i+0)*h-p[0])/d[0]*d[2]+p[2] };
        if( i==ix0 ){ p0[0] = p[1]; p0[1] = p[2]; }
        else if( p0[0] < 0 || p0[0] > width_ || p0[1] < 0 || p0[1] > width_ ){
          if( iflg ){ break; }
          else{ continue; }
        }
        if( i!=ix0 ){ iflg = true; }
        Find_Intersec_Layer_X(i,p0,p1,aIndTriCand);				
      }
    }
  }
  ////
  else if( fabs(d[1]) > fabs(d[0]) && fabs(d[1]) > fabs(d[2]) ){
    int iy0 = floor(p[1]*invcellwidth_);
    bool iflg = false;
    if(      d[1] > +1.0e-10 ){
      if( iy0 < 0 ){ iy0 = 0; }
      for(int i=iy0;i<ndiv_;i++){
        double p0[2] = { ((i+0)*h-p[1])/d[1]*d[2]+p[2], ((i+0)*h-p[1])/d[1]*d[0]+p[0] };
        double p1[2] = { ((i+1)*h-p[1])/d[1]*d[2]+p[2], ((i+1)*h-p[1])/d[1]*d[0]+p[0] };
        if( i==iy0 ){ p0[0] = p[2]; p0[1] = p[0]; }
        else if( p0[0] < 0 || p0[0] > width_ || p0[1] < 0 || p0[1] > width_ ){
          if( iflg ){ break; }
          else{ continue; }
        }
        if( i!=iy0 ){ iflg = true; }
        Find_Intersec_Layer_Y(i,p0,p1,aIndTriCand);
      }
    }
    else if( d[1] < -1.0e-10 ){
      if( iy0 >= ndiv_ ){ iy0 = ndiv_-1; }
      for(int i=iy0;i>=0;i--){
        double p0[2] = { ((i+1)*h-p[1])/d[1]*d[2]+p[2], ((i+1)*h-p[1])/d[1]*d[0]+p[0] };
        double p1[2] = { ((i+0)*h-p[1])/d[1]*d[2]+p[2], ((i+0)*h-p[1])/d[1]*d[0]+p[0] };
        if( i==iy0 ){ p0[0] = p[2]; p0[1] = p[0]; }
        else if( p0[0] < 0 || p0[0] > width_ || p0[1] < 0 || p0[1] > width_ ){
          if( iflg ){ break; }
          else{ continue; }
        }
        if( i!=iy0 ){ iflg = true; }
        Find_Intersec_Layer_Y(i,p0,p1,aIndTriCand);				
      }
    }
  }
  ////
  else if( fabs(d[2]) > fabs(d[1]) && fabs(d[2]) > fabs(d[0]) ){
    int iz0 = floor(p[2]*invcellwidth_);
    bool iflg = false;
    if(      d[2] > +1.0e-10 ){
      if( iz0 < 0 ){ iz0 = 0; }
      for(int i=iz0;i<ndiv_;i++){
        double p0[2] = { ((i+0)*h-p[2])/d[2]*d[0]+p[0], ((i+0)*h-p[2])/d[2]*d[1]+p[1] };
        double p1[2] = { ((i+1)*h-p[2])/d[2]*d[0]+p[0], ((i+1)*h-p[2])/d[2]*d[1]+p[1] };
        if( i==iz0 ){ p0[0] = p[0]; p0[1] = p[1]; }
        else if( p0[0] < 0 || p0[0] > width_ || p0[1] < 0 || p0[1] > width_ ){
          if( iflg ){ break; }
          else{ continue; }
        }
        if( i!=iz0 ){ iflg = true; }
        Find_Intersec_Layer_Z(i,p0,p1,aIndTriCand);
      }
    }
    else if( d[2] < -1.0e-10 ){
      if( iz0 >= ndiv_ ){ iz0 = ndiv_-1; }
      for(int i=iz0;i>=0;i--){
        double p0[2] = { ((i+1)*h-p[2])/d[2]*d[0]+p[0], ((i+1)*h-p[2])/d[2]*d[1]+p[1] };
        double p1[2] = { ((i+0)*h-p[2])/d[2]*d[0]+p[0], ((i+0)*h-p[2])/d[2]*d[1]+p[1] };
        if( i==iz0 ){ p0[0] = p[0]; p0[1] = p[1]; }
        else if( p0[0] < 0 || p0[0] > width_ || p0[1] < 0 || p0[1] > width_ ){
          if( iflg ){ break; }
          else{ continue; }
        }
        if( i!=iz0 ){ iflg = true; }
        Find_Intersec_Layer_Z(i,p0,p1,aIndTriCand);				
      }
    }
  }
  
}



