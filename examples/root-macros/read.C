//
#include <math.h>
#include <stdio.h>
#include <vector>
#include <iostream>
//
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TMath.h"
#include "TString.h"
#include "TROOT.h"
#include "TCanvas.h"
//
#include "../include/Track_t.hh"
#include "../include/Hit_t.hh"
//
const double E2Q = 44.4;  //  [fC/MeV]
const int    NBITS = 14;  //  ADC bits
const double NBINS = 100;
const double QMIPS = 3.9; //  [fC]
const double Q2ADC = NBINS/QMIPS;
const double E2MIP = E2Q/QMIPS;
const double XCENT = 186.95;
//
int GetChan( int id ) {  return  ((id >> 0 ) &0xff) ;}
int GetSect( int id ) {  return  ((id >> 8 ) &0xff) ;}
int GetPlan( int id ) {  return  ((id >> 16) &0xff) ;}
//
void read( TString filename, TString dataset )
{
  double qdep[30];
  //book histos
  TH1F *h1  = new TH1F("nhit","Number of hits;nhits;counts",500,-0.5,499.5);
  TH1D *h2  = new TH1D("shower1","Shower Profile;x_{cell}-x_{beam};counts",50,-25.,25.);
  TH1F *h21 = new TH1F("shower2","Shower Profile;channel;counts",64,-0.5,63.5);
  TProfile *h22 = new TProfile("shower3","Shower Profile;X0;<Q_{dep}",30,-0.5,29.5,0.,500.,"E");
  TProfile *h23 = new TProfile("shower4","Shower Profile;dx;<Q_{dep}",50,-25.,25.,0.,500.,"E");
  TH1D *h3 = new TH1D("esum","Tot energy; Q_{tot} [MIP]; counts",500,0.,1000.);
  TH2D *h4 = new TH2D("beam","Beam profile",190,-190.,190.,190,-190.,190.);
  TH1D *pad[16];
  for ( int k=0; k<16; k++){
    TString name  = dataset + TString("_pad");
    name += k;
    TString title = name + TString(";Q [MIP];counts");
    pad[k] = new TH1D(name,title,150,0.,15.);
  }
  // attache dictionaries
  gROOT->ProcessLine(".L ../root-macros/loadDict.C+");
  //open file
  TFile fin( filename );
  // get the tree
  TTree *LcalData = (TTree*)fin.Get("Lcal");
  int nentr = LcalData->GetEntries();
  //
  // set addresses
  double vX, vY, vZ, Emax, Etot[2];
  int numPrim, numHits;     
  std::vector<Track_t> *pTracks = 0;
  std::vector<Hit_t> *pHits = 0;

       LcalData->SetBranchAddress("vX", &vX ); 
       LcalData->SetBranchAddress("vY", &vY ); 
       LcalData->SetBranchAddress("vZ", &vZ ); 
       LcalData->SetBranchAddress("numPrim", &numPrim );
       LcalData->SetBranchAddress("numHits", &numHits );
       LcalData->SetBranchAddress("Etot", Etot );
       LcalData->SetBranchAddress("Emax", &Emax );
       LcalData->SetBranchAddress("Tracks", &pTracks ); 
       LcalData->SetBranchAddress("Hits",&pHits );
  //
  // start loop over events
  for ( int ientr =0; ientr<nentr; ientr++ ){
    LcalData->GetEntry(ientr);
    int nhits = pHits->size();
    double sum = 0.;
    float vhits = 0.;
    for(int ipl=0; ipl<30; ipl++) qdep[ipl]=0.;
    for (int ih=0; ih<nhits; ih++){
      Hit_t hit = (*pHits)[ih];
      int id = hit.cellID;
      int ich = GetChan(id);
      int ipl = GetPlan(id)-1;
      h21->Fill( float(ich));
	double qhit = hit.eHit * E2MIP;
        h23->Fill(hit.xHit-vX,qhit);
	vhits++; 
	sum += qhit;
        qdep[ipl] += qhit;
	h2->Fill( hit.xCell-vX );
     
    }// end loop over hits
    for( int ip=0; ip<30; ip++) h22->Fill(double(ip),qdep[ip]);
    h1->Fill(vhits);
    h3->Fill( sum );
    h4->Fill(vX,vY);
  }// end loop over events
  TString title =TString("Plots ") + dataset;
  TCanvas *c1 = new TCanvas("c1",title,10,10,600,600);
  c1->Divide(2,2);
  c1->cd(1);
  h23->Draw();
  c1->cd(2);
  h2->Draw();
  c1->cd(3);
  h22->Draw();
  c1->cd(4);
  h21->Draw();
}
