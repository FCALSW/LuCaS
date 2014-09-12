//
#include <math.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>
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
#include "Track_t.hh"
#include "Hit_t.hh"
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
void tb2011( TString filename, TString dataset )
{
  double qdep[30];
  float vhits[30];
  // open file for histograms
  TString title =TString("plots-") + dataset;
  title += TString(".root");
  TFile fhout(title,"UPDATE");

  // open ASCII file for writing charge deposit ( Szymon like it this way )
  ofstream myfile;
  TString fname = TString("charge_deposit_") + dataset + TString(".txt");
   myfile.open(fname);
  //book histos
  TString name0 = TString("nhit0_") + dataset;
  TString name1 = TString("nhit1_") + dataset;
  TH1F *h10  = new TH1F(name0,"Number of hits plane 0;nhits;counts",50,-0.5,49.5);
  TH1F *h11  = new TH1F(name1,"Number of hits plane 1;nhits;counts",50,-0.5,49.5);
  h10->SetBit(TH1::kCanRebin);
  h11->SetBit(TH1::kCanRebin);

  TH1D *h2  = new TH1D("shower1","Shower Profile X;x_{cell}-x_{beam};counts",50,-25.,25.);
  TH1D *h24  = new TH1D("shower5","Shower Profile Y;y_{cell}-y_{beam};counts",50,-25.,25.);
  TH1F *h21 = new TH1F("shower2","Shower Profile;channel;counts",23,40.5,63.5);
  TProfile *h22 = new TProfile("shower3",";plane number;<Q_{dep}>  [MIP]",20,-0.5,1.5,0.,500.,"E");
  TProfile *h23 = new TProfile("shower4","Shower Profile;dx;<Q_{dep}>",50,-25.,25.,0.,500.,"E");
  TH1D *h31 = new TH1D("esum1","Tot energy; E_{dep} [MIP]; counts",500,0.,100.);
  TH1D *h30 = new TH1D("esum0","Tot energy; E_{dep} [MIP]; counts",500,0.,100.);
  //h31->SetBit(TH1::kCanRebin);
  //h30->SetBit(TH1::kCanRebin);
  TH2D *h4 = new TH2D("beam","Beam profile",80,-20.,20.,80,20.,60.);

  /*  TH1D *pad[16];
  for ( int k=0; k<16; k++){
    TString name  = dataset + TString("_pad");
    name += k;
    TString title = name + TString(";Q [MIP];counts");
    pad[k] = new TH1D(name,title,150,0.,15.);
  }
  */
  TH1D *pads = new TH1D("pads",";pad number;counts",32,-0.5,31.5);
  // attache dictionaries
  gROOT->ProcessLine(".L loadDict.C+");
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
       LcalData->SetBranchAddress("Hits", &pHits );
  //
  // start loop over events
       double pad[32];
       int npads=32;
       int padnum = 0;
  for ( int ientr =0; ientr<nentr; ientr++ ){
    LcalData->GetEntry(ientr);
    int nhits = pHits->size();
    double sum = 0.;
    for(int ipl=0; ipl<30; ipl++) { qdep[ipl]=0.; vhits[ipl] = 0.;}
    for(int ipad=0; ipad<npads; ipad++) pad[ipad]=0.;
    for (int ih=0; ih<nhits; ih++){
      Hit_t hit = (*pHits)[ih];
      int id = hit.cellID;
      int isec = GetSect(id);
      int ich = GetChan(id);
      int ipl = GetPlan(id);
      //	double qhit = hit.eHit * E2MIP;
  	double qhit = hit.eHit;
        if ( (ich > 49 && isec==13) || (isec == 12 && ich > 45 )){
	  h23->Fill(hit.xHit-vX,qhit);
	  h21->Fill( float(ich));
	  // map LCal channel number onto Szymon pad numbering scheme 
	  if ( isec == 13 && ipl == 1) padnum = ich-50;
	  if ( isec == 12 && ipl == 1) padnum = 77 -ich; 
	  //        
	  if ( ipl == 1){ pads->Fill(float(padnum));  pad[padnum]= qhit*E2Q;}
	  vhits[ipl]++; 
	  sum += qhit;
	  qdep[ipl] += qhit;
	  h2->Fill( hit.xCell-vX );
	  if ( ipl == 1 ) h24->Fill( hit.yCell-vY );
	}
     
    }// end loop over hits
    for( int ipad=0; ipad < npads ; ipad++) myfile<< pad[ipad] << " ";
    myfile << endl;
    for( int ip=0; ip<2; ip++){
      if( qdep[ip] > 0. ){
     	double nmip = qdep[ip] * E2MIP;
	h22->Fill(double(ip),nmip);
	switch( ip){
	case 0:
	  h30->Fill(nmip);
	break;
	case 1:
	  h31->Fill(nmip);
	}

      }

    }
    //    myfile << endl;
    h10->Fill(vhits[0]);
    h11->Fill(vhits[1]);
    h4->Fill(vX,vY);
  }// end loop over events
  myfile.close();
  //
  // save histograms

  fhout.cd();
  fhout.Write();

  /*  h22->Write();
  h30->Write();
  h31->Write();
  pads->Write();
  */
}
