
#ifndef reco_hh
#define reco_hh

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>
#include "TMath.h"
#include "TROOT.h"

#include <iostream>
#include <vector>
#include <algorithm>

#include "Track_t.hh"
#include "Hit_t.hh"
#include "myConstants.hh"



struct myHit {
  double E, r, phi, z;
  myHit (double E = 0, double r = 0., double phi=0., double z=0.):
    E ( E ), 
    r( r) , 
    phi( phi ),
    z( z ) 
  {}
     
};
  bool SortByEnergy ( const myHit & lhs, const myHit & rhs ) {
  return (lhs.E > rhs.E) ;
  }

   // my variables

  // Declaration of leaf types
   Double_t        vX;
   Double_t        vY;
   Double_t        vZ;
   Int_t           numPrim;
   Int_t           numHits;   
   Double_t        Etot[2];
   Double_t        Emax;
   std::vector<Track_t> *pTracks;
   std::vector<Hit_t> *pHits;


double GetThetaNoVx( const std::vector< myHit > &hits)
{ double S   = 0.;
  double Sx  = 0.;
  double Sy  = 0.;
  double Sxx = 0.;
  double Sxy = 0.;
  double Etot2 = ( hits[0].z > 0. )? Etot[1] : Etot[0];
  double wt = 0., zh = 0. , yh = 0.;
  for ( unsigned ih=0; ih < hits.size(); ih++){
    wt = C_THETA + log10( hits[ih].E/Etot2);
    wt = ( wt > 0. ) ? 1./wt : 0.;
    if ( wt <= 0. ) continue;
    zh = (hits[ih].z-Lcal_Z);
    yh = hits[ih].r;
    S += wt; Sx += zh*wt; Sy+= wt*yh;
    Sxx += zh*zh*wt; Sxy += zh*yh*wt;
  }
  double delta = S*Sxx - Sx*Sx;
  double theta = ( fabs(delta)>=0. ) ? (S*Sxy-Sx*Sy)/delta : -9999.;
  return theta;  
}

double GetTheta ( const std::vector< myHit > & hits )
{
  double theta = 0., sumwt = 0., wt;
  double thsum = 0.;
  double Etot2 = ( hits[0].z > 0. )? Etot[1] : Etot[0];
  for ( unsigned ih = 0; ih < hits.size(); ih++ ){
    wt = C_THETA + log10( hits[ih].E/Etot2);
    wt = ( wt > 0. ) ? wt : 0.;
    thsum += wt*atan( hits[ih].r/hits[ih].z );
    sumwt += wt;
  }
  theta = ( sumwt > 0.) ? thsum/sumwt : -9999. ;
  return theta;
}

double GetUsedEnergy ( const std::vector< myHit > & hits ){
  double esum = 0., wt;
  double Etot2 = ( hits[0].z > 0. )? Etot[1] : Etot[0];
  for ( unsigned ih = 0; ih < hits.size(); ih++ ){
    wt = log10( C_THETA*hits[ih].E/Etot2);
    esum  += ( wt > 0. ) ? hits[ih].E : 0. ;
  }
  return esum;
}

double GetPhi   ( const std::vector< myHit > & hits ){
  double phi = 0., xsum =0., ysum = 0.;
  double sumwt = 0., wt = 0.;
  double sumE = ( hits[0].z > 0. )? Etot[1] : Etot[0];
  for ( unsigned ih = 0; ih < hits.size(); ih++ ){
        wt = hits[ih].E/sumE;
        wt = C_PHI + log10( wt );
        wt = ( wt > 0. ) ? wt : 0.;
    xsum += wt*hits[ih].r*cos( hits[ih].phi );
    ysum += wt*hits[ih].r*sin( hits[ih].phi );
    sumwt += wt;
  }
  if ( sumwt > 0. ) {
    phi = atan2 ( ysum/sumwt, xsum/sumwt );
    phi = ( phi > 0. ) ? phi : phi + TWO_PI ;
    return phi ;
  }else{
    return -9999. ;
  }
}

double GetEnergy ( double theta, const std::vector< myHit > & hits){
  // returns energy contained within Moliere radius
  double sumE = 0.;
  double rp, dx, dy, dist ;
  for ( unsigned ih = 0; ih < hits.size(); ih++ ){
    rp = hits[ih].z*tan( theta );
    dx = rp*cos( hits[ih].phi ) - hits[ih].r*cos( hits[ih].phi ) ;
    dy = rp*sin( hits[ih].phi ) - hits[ih].r*sin( hits[ih].phi ) ;
    dist = sqrt( dx*dx + dy*dy );
    sumE += ( dist < R_MOL ) ? hits[ih].E : 0. ;
  }
  return sumE ;
}
double GetEnergy ( const std::vector< myHit > & hits){
  // returns total energy energy of hits on one side
  double sumE = 0.;
  for ( unsigned ih = 0; ih < hits.size(); ih++ ){
    sumE += hits[ih].E;
  }
  return sumE ;
}
int GetChan( int id ) {  return  ((id >> 0 ) &0xff) ;}
int GetSect( int id ) {  return  ((id >> 8 ) &0xff) ;}
int GetPlan( int id ) {  return  ((id >> 16) &0xff) ;}

Double_t GetDistance(Double_t theta, Double_t phi, Double_t rotAng)
{
  Double_t R_pos  = Lcal_Z*tan(theta);
  Double_t ltil   = R_pos*Tile_dPhi;
  // 
  Int_t tile  =  ((Int_t)floor(phi/Sector_dPhi))/4;
  Double_t dist0 = R_pos*(phi - Tile_dPhi*(Double_t)(tile));
  if ( (ltil-dist0) < dist0 ) dist0 = -(ltil-dist0);
  //
  Double_t phirot = phi-rotAng*TWO_PI/360.;
  if ( phirot < 0. ) phirot += TWO_PI;
  tile  =  ((Int_t)floor(phirot/Sector_dPhi))/4;
  Double_t distR = R_pos*(phirot - Tile_dPhi*(Double_t)(tile));
  if ( (ltil-distR) < distR ) distR = -(ltil-distR);
  //
  if ( fabs(dist0) <= fabs(distR) ) return dist0;
  else return distR;
}
Bool_t CheckSector( Double_t phi )
{
  Int_t sector  =  (Int_t)floor(phi/Sector_dPhi);
  Int_t lsec    =  sector%4;
  if ( (lsec == 0 ) || ( lsec == 3 ) ) return kTRUE; 
  return kFALSE;
}

Double_t GetCorrection( Int_t sample, Double_t dist )
{
  return  (1.+ a1[sample]*exp( -w1[sample]*dist*dist)
	     + a2[sample]/( dist*dist + w2[sample]) ) ;
}
 

#endif
