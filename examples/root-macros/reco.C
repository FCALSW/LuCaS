//#define rot3_hh
#define rot0_hh
//#define si500
#include "reco.hh"
#include <TSystem.h>
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TProfile.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>

void reco( Double_t dcut , Long64_t nwanted, TString fname )
{
  // attache dictionaries before file
	gSystem->Load("./lib/libLucasDict.so");
        TFile fin ( fname );
        TTree *Lcal = (TTree*)fin.Get("Lcal");
        if (Lcal == 0) return;
	pTracks = 0; 
        pHits   = 0;

       Lcal->SetBranchAddress("vX", &vX ); 
       Lcal->SetBranchAddress("vY", &vY ); 
       Lcal->SetBranchAddress("vZ", &vZ ); 
       Lcal->SetBranchAddress("numPrim", &numPrim );
       Lcal->SetBranchAddress("numHits", &numHits );
       Lcal->SetBranchAddress("Etot", Etot );
       Lcal->SetBranchAddress("Emax", &Emax );
       Lcal->SetBranchAddress("Tracks", &pTracks ); 
       Lcal->SetBranchAddress("Hits", &pHits );
   
   Int_t Nbins = 300;
   Double_t Xlo=   2.5;
   Double_t Xup= 1502.5;
   //
   TF1 *res1 = new TF1("res1","sqrt([0]*[0]/x+[1]*[1])",1.,1501.);
   res1->SetParNames("a","b");
   TF1 *cfun1 = new TF1("cfun1","1.+[0]*exp(-[1]*x*x)+[2]/(x*x+[3])",-45.,45.);
   cfun1->SetParNames("A1","W1","A2","W2");
   TF1 *cfun2 = new TF1("cfun2","1.+ [0]/(x*x+[1])",-45.,45.);
   cfun2->SetParNames("A2","W2");
   //

   TProfile *hpr1 = new TProfile("hpr1"," ;E_{beam} [GeV]; E_{vis} [GeV]",Nbins,Xlo,Xup,0.,1000.,"S");
   TH1D *sigeove =  new TH1D("sigeove"," ;E_{beam} [GeV]; #sigma_{E}/E",Nbins,Xlo,Xup);
   TH1D *rmseove =  new TH1D("rmseove"," ;E_{beam} [GeV]; RMS_{E}/E",Nbins,Xlo,Xup);
   TH1D *dErec[9];
   TH1D *euse = new TH1D ( "euse"," ; E_{used}  [GeV] ; Number of events",600, 0., 6. );
   TH1D *edep[9];
   TH1D *dphi[9], *Qmax[9];
   //   TProfile *dgap[9];
   TH2D *dgap[9];
   TH1D *dtheta[9];
   double qmax[9];
   TProfile *phioff = new TProfile("phioff","#phi offset;1/P  [GeV^{-1}];<#Delta#Phi>   [rad]",500,0.,0.25,-TWO_PI,TWO_PI,"E");
   for( int ih=0;ih<9;ih++){
     TString nam1("dphi");
     TString nam2("edep");
     TString nam3("dErec");
     TString nam4("dgap");
     TString nam5("dtheta");
     TString nam6("Qmax");
     nam1 += ih;
     nam2 += ih;
     nam3 += ih;
     nam4 += ih;
     nam5 += ih;
     nam6 += ih;
     dphi[ih] = new TH1D(nam1,"; #Delta#Phi   [rad]; Number of events", 300, -0.03, 0.03 );
     dtheta[ih] = new TH1D(nam5,"; #Delta#theta   [rad]; Number of events", 150, -0.00015, 0.00015 );
     edep[ih] = new TH1D(nam2," ; E_{VIS}  [GeV] ; Number of events",500,Elo[ih],Eup[ih]);   
     dErec[ih] = new TH1D( nam3 ," ; #DeltaE   [GeV]; Number of events",60, -25., 25. );
     //     dgap[ih] = new TProfile(nam4,"; distance from the gap [mm]; <E_{VIS}>/E_{VIS}",900,-45.,45.,0.,1000.,"E");
     dgap[ih] = new TH2D(nam4,"; distance from the gap [mm]; <E_{VIS}>/E_{VIS}",900,-45.,45.,100,0.5,1.5);
     Qmax[ih] = new TH1D(nam6,"; Q_{max}    pC; number of events ",40,0.,20.);
     //     dErec[ih]->SetBit(TH1::kCanRebin);
     //     edep[ih]->SetBit(TH1::kCanRebin);
     //     dphi[ih]->SetBit(TH1::kCanRebin);
   }
   //
   //
   std::vector < myHit > hits_P;  // list of hits z > 0
   std::vector < myHit > hits_N;  //              z < 0
   //
   cout << " Energy saturation at "<< E_MAX << " [MeV] " << endl;
   cout << " Energy treshold at   "<< E_MIN << " [MeV] " << endl;
   Long64_t nentries = Lcal->GetEntries();
#ifdef si500
     Double_t p0 =  6.13e-4;   // si 500
     Double_t p1 =  1.71e-2;;
#else
#ifdef rot3_hh
   Double_t p0 = 1.771e-3;  // si=320
   Double_t p1 = 1.122e-2;
#else
   Double_t p0 = 2.145e-3;  // si=320
   Double_t p1 = 1.131e-2;
#endif
#endif
   Double_t C_fac = 0.01138;
  
     /*     Double_t g0 = 1.006e-2;
     Double_t g1 = 2.367e-7;
     Double_t g2 = -7.476e-10;
     */
     // Long64_t nskip = 0;
   nentries = ( nwanted > 0 && nwanted < nentries ) ? nwanted : nentries ;
   cout << " Nentries "<< nentries << endl;
   for (Long64_t jentry=0; jentry< nentries ;jentry++) {
     Double_t theta_rec1 = 0., phi_rec1=0. ;
     Double_t theta_rec2 = 0., phi_rec2=0. ;
     Double_t     e_use1 = 0.,   e_use2=0. ;
     Double_t     e_vis1 = 0.,   e_vis2=0. ;
      hits_N.clear();
      hits_P.clear();
      e_vis2 = 0.;

      Lcal->GetEntry(jentry);
      if ( Etot[1] <= 0.) continue;
      //
      double E_gen=0., theta_gen=0., phi_gen=0.;
      int numtr = pTracks->size();
      for ( int it=0; it< numtr; it++){
      Track_t track = (*pTracks)[it];
      E_gen = sqrt( track.pX*track.pX + track.pY*track.pY + track.pZ*track.pZ )/1000.;
      theta_gen = atan ( sqrt( track.pX*track.pX + track.pY*track.pY)/fabs(track.pZ));
      phi_gen = atan2 ( track.pY , track.pX);
      phi_gen = ( phi_gen > 0. ) ? phi_gen : phi_gen + TWO_PI;
      }
      //
      if ( theta_gen < 0.041 || theta_gen > 0.068 ){ continue;}
      //
      int sample=0;
      switch( (int)E_gen ){
      case 5:
	sample = 0;
        break;
      case 25:
	sample = 1;
        break;
      case 50:
	sample = 2;
        break;
      case 100:
	sample = 3;
        break;
      case 150:
	sample = 4;
        break;
      case 200:
	sample = 5;
        break;
      case 250:
	sample = 6;
        break;
      case 500:
	sample = 7;
        break;
      case 1500:
	sample = 8;
        break;
      default:
	cout<< "uknown case " << (int)E_gen << endl;
      }
      //
      // Fill the list of hits
      //
      int nHits = pHits->size();
      qmax[sample] = 0.;
      for ( Int_t ih=0; ih<nHits ; ih++) {
	// Int_t layer   = (cellID[ih] >> 16) & 0xff ;
	//
        Hit_t hit = (*pHits)[ih];
	double ehit = ( hit.eHit > E_MAX ) ? E_MAX : hit.eHit ;
	if ( ehit < E_MIN ) continue ;
        if ( qmax[sample] < ehit ) qmax[sample]=ehit; 
	// 
        Double_t rCell = sqrt( hit.xCell*hit.xCell+hit.yCell*hit.yCell);
        Double_t phiCell = atan2( hit.yCell, hit.xCell );
        if ( hit.zCell < 0. ) {
          hits_N.push_back ( myHit( ehit, rCell, phiCell, hit.zCell) );
	}
        else {
          hits_P.push_back ( myHit( ehit, rCell, phiCell, hit.zCell) );
	} 

      } // end hit loop
      //
      // sort hits in descending order
      //
      //      cout<< " accepted " << nskip << " " << hits_P.size()<< " " << hits_N.size() << endl;
     
     if ( hits_N.begin() != hits_N.end() ) {
	std::sort ( hits_N.begin(), hits_N.end(), SortByEnergy );
	theta_rec1 = GetTheta ( hits_N );
	phi_rec1 = GetPhi ( hits_N );
	    e_vis1 = GetEnergy( theta_rec1, hits_N );
      }

      if ( hits_P.begin() != hits_P.end() ) {
	std::sort ( hits_P.begin(), hits_P.end(), SortByEnergy );
	theta_rec2 = GetTheta( hits_P );
	phi_rec2 = GetPhi ( hits_P );
#ifdef rot3_hh
	double dist = GetDistance( theta_rec2, phi_rec2, 3.75);
#else
	double dist = GetDistance( theta_rec2, phi_rec2, 0.);
#endif
      
	//       	if ( CheckSector( phi_rec2 ) )continue;
      	if ( fabs(dist) < dcut ) continue;
	double avrEvis = p0 + p1*E_gen;
	    e_vis2 = GetEnergy(  hits_P )/1000.;
	    //       	    e_vis2 *= GetCorrection( sample, dist);
	    dgap[sample]->Fill( dist, avrEvis/e_vis2, 1.  );
      }

      //
      // fill histograms
      //
      C_fac = (e_vis2 - p0)/p1;
      Double_t delta_phi = phi_gen - (phi_rec2 + PHI_OFFSET[sample]);
      Double_t pT = 1./(E_gen*sin(theta_rec2));
      Double_t E_rec = C_fac ;
      //      double delta = p1*p1 - 4.*p2*(p0-e_vis2);
      //      if( delta >= 0. ) E_rec = (-p1+sqrt(delta))/(2.*p2);
      Qmax[sample]->Fill( qmax[sample]*E2Q );
      dtheta[sample] -> Fill ( theta_rec2 - theta_gen );
      dphi[sample]-> Fill ( delta_phi);
      phioff->Fill( pT , delta_phi );
      if ( e_vis2 > 0. ) {
             edep [sample]-> Fill ( e_vis2 );
	     hpr1->Fill(E_gen,e_vis2,1.);
	     dErec[sample]  -> Fill ( ( E_rec - E_gen));
	     euse -> Fill ( e_use2 );
      }
      if  ( e_use1 > 0.) euse -> Fill ( e_use1 );
      //
      //
      // end of event
      // 
   }
   // final operations with histograms 
   //
   int binum[9] = { 0, 1, 2, 3, 4, 5, 6, 7, 8};
   int ii = 0;
   for ( int ib=0; ib < Nbins; ib++ ){
     double binc = hpr1->GetBinContent(ib+1);
     if( binc > 0.){
      cout << " " <<ib+1 << " " << binc << endl;
      binum[ii] = ib+1;
      ii++;
     }
   }
   TFile *out = new TFile("reco-plots-rot0-uncor.root","RECREATE");
  //
   //
   for ( int ih=0;ih<9;ih++){
     if( edep[ih]->Integral() < 1. ) continue;
    cout<< " fitting " << ebeam[ih] << endl;
    edep[ih]->Fit("gaus","QW");
    edep[ih]->Write();
    TF1 *f = edep[ih]->GetFunction("gaus");
    double mean = f->GetParameter(1);
    double rmse = f->GetParameter(2);
    double ratio = rmse/mean;
    double e1 = f->GetParError(1);
    double e2 = f->GetParError(2);
    double err = sqrt(e1*e1/(mean*mean)+e2*e2/(rmse*rmse))*ratio; 
    cout << "<E> " << mean << " sigma "<< rmse << endl;
    sigeove->SetBinContent( binum[ih], ratio );
    sigeove->SetBinError( binum[ih], err );
    //
      mean = edep[ih]->GetMean();
      rmse = edep[ih]->GetRMS();
      e1 = edep[ih]->GetMeanError();
      e2 = edep[ih]->GetRMSError();
      ratio = rmse/mean;
      err = sqrt(e1*e1/(mean*mean)+e2*e2/(rmse*rmse))*ratio; 
       rmseove->SetBinContent( binum[ih], ratio);
       rmseove->SetBinError( binum[ih],2.*err);
   dErec[ih]->Fit("gaus","QW");
   dErec[ih]->Write();
    }
   sigeove->Fit("res1");
   rmseove->Fit("res1");
   //
   //
   Double_t P0[9],P1[9],P2[9],P3[9];
   for ( int ih=0; ih<9; ih++){
     P0[ih] = 0.;
     P1[ih] = 0.;
     P2[ih] = 0.;
     P3[ih] = 0.;
   }

     for( int ih=0; ih<9; ih++){
       if( dphi[ih]->Integral() < 1. ) continue;
     dphi[ih]->Fit("gaus","Q");
     dphi[ih]->Write();
     dtheta[ih]->Fit("gaus","Q");
     dtheta[ih]->Write();
     Qmax[ih]->Write();
 
     dgap[ih]->Fit("cfun2","QW");
     TF1 *f=dgap[ih]->GetFunction("cfun2");
     // P0[ih]=f->GetParameter(0);
     // P1[ih]=f->GetParameter(1);
     P2[ih]=f->GetParameter(0);
     P3[ih]=f->GetParameter(1);
    
     dgap[ih]->SetMinimum(0.9);
     dgap[ih]->SetMaximum(2.0);
     dgap[ih]->Write();
     }
     //
     
     cout << "const double a1[9] = {" ;
     for (int ih=0;ih<9;ih++) printf("%5.3e%1s",P0[ih],", ");
     cout<<endl;
     cout << "const double w1[9] = {" ;
     for (int ih=0;ih<9;ih++) printf("%5.3e%1s",P1[ih],", ");
     cout<<endl;
     cout << "const double a2[9] = {" ;
     for (int ih=0;ih<9;ih++) printf("%5.3e%1s",P2[ih],", ");
     cout<<endl;
     cout << "const double w2[9] = {" ;
      for (int ih=0;ih<9;ih++) printf("%5.3e%1s",P3[ih],", ");
     cout<<endl;
     
     //

     hpr1->Write();
     phioff->Write();
     sigeove->Write();
     rmseove->Write();
   //
     //    out->Close();
}
