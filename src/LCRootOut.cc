/*
 * LCRootOut.cc
 *
 *  Created on: Apr 23, 2009
 *      Author: aguilar
 $Id$
 */

#include "LCRootOut.hh"
#include "Setup.hh"
#include "LCDetectorConstruction.hh"
//#include "LucasDict.h"

#include <EVENT/LCIO.h>
#include <IMPL/MCParticleImpl.h>
#include <IMPL/LCCollectionVec.h>
#include <IMPL/SimCalorimeterHitImpl.h>
#include <UTIL/CellIDEncoder.h>
#include <UTIL/BitSet32.h>

#include "G4UImanager.hh"


TFile *LCRootOut::pRootFile = NULL;

LCRootOut::LCRootOut() : 
  _fileROOT(NULL),
  _flcioWriter(NULL)
{ 
}

LCRootOut::LCRootOut(const G4String name ) : 
  _fileROOT(NULL),
  _flcioWriter(NULL)
{
  RootOutFile = name;
  _LcalData = NULL;
}

LCRootOut::~LCRootOut()
{ 
  G4cout << " LCRootOut deleted ! " << G4endl;
}

void LCRootOut::CreateNewTree()
{
    // create tree
    //+
    _LcalData = new TTree("Lcal", Setup::RunDescription );

    // create branches
    //
    // vertex pos 
    _LcalData->Branch("vX", &vX, "vX/D");       // Vertex primary particle
    _LcalData->Branch("vY", &vY, "vY/D");       // Vertex primary particle
    _LcalData->Branch("vZ", &vZ, "vZ/D");       // Vertex primary particle
    // +
    // primary particles 
    _LcalData->Branch("numPrim", &numPrim, "numPrim/I");               // number of primary particles
    _LcalData->Branch("numHits", &numHits, "numHits/I");               // number of calo hits
    _LcalData->Branch("Etot", Etot, "Etot[2]/D");                      // total  energy deposit per side 1
    _LcalData->Branch("Emax", &Emax, "Emax/D");                        //  max hit energy
    // +
    // tracks
    _LcalData->Branch("Tracks","std::vector< Track_t >", &pTracks );   // primary particles momenta
    // +
    // hits
    _LcalData->Branch("Hits","std::vector< Hit_t >", &pHits );         // calo hits
}

void LCRootOut::SetAddresses()
{
      _LcalData->SetBranchAddress("vX", &vX ); 
      _LcalData->SetBranchAddress("vY", &vY ); 
      _LcalData->SetBranchAddress("vZ", &vZ ); 
      _LcalData->SetBranchAddress("numPrim", &numPrim );
      _LcalData->SetBranchAddress("numHits", &numHits );
      _LcalData->SetBranchAddress("Etot", Etot );
      _LcalData->SetBranchAddress("Emax", &Emax );
      _LcalData->SetBranchAddress("Tracks", &pTracks ); 
      _LcalData->SetBranchAddress("Hits",&pHits );
}

void LCRootOut::Init()
{
  // 
    numHits = 0; 
    numPrim = 0;
    Etot[0] = 0. ;
    Etot[1] = 0. ;
    Emax  = 0. ;
    pTracks = &Tracks;
    pHits   = &Hits;

  // open root file 
  //
  if( !_fileROOT )  
    {
      G4cout << "LCRootOut:: Opening file: " << Setup::RootFileName 
	     << " write mode : "<< Setup::RootFileMode << G4endl;
    
    _fileROOT = new TFile( Setup::RootFileName, Setup::RootFileMode);
    LCRootOut::pRootFile = _fileROOT;

    _LcalData = (TTree*)_fileROOT->Get("Lcal");

    // the following is attempt to fix weird ROOT behaviour :
    // TFile object does not prevent against overwriting exiting file 
    // - in mode "NEW" or "CREATE" issues only warning message and continues
    //   this results in "empty run" results are not written to a file
    // - "RECREATE" mode causes overwriting exiting file without warning
    // 

    if( _fileROOT && Setup::RootFileMode == "UPDATE" )
      {
	if ( _LcalData ) {
      // this is correct situation
      // don't create new tree, use existing one
      // and set branch addresses
      //
      SetAddresses();
	}else{
	G4cout << " File : " << Setup::RootFileName << " opened " << G4endl;
	CreateNewTree();
	}

     
      }
    else if ( (_fileROOT->IsZombie()) && ( Setup::RootFileMode == "NEW" || Setup::RootFileMode == "CREATE" ))
      {
      
        // something is wrong - file exists, ROOT issues error message but run continues, 
        // user is going to waste time runing job with no output saved in root file.
	// Abort the run
	G4Exception ( " LCRootOut::Init: Attempt to override file :",
		      Setup::RootFileName,
		      RunMustBeAborted, ". Aborting !");

      }
    else if (  _fileROOT && Setup::RootFileMode == "RECREATE" )
      { 
	// G4Exception ( " Attempt to override existing file :" + Setup::RootFileName + ". Aborting !");
	G4cerr << " File : "<< Setup::RootFileName << " is being overriden !!!!!!!! " << G4endl;
	CreateNewTree();
      }    
    else
      {
	// this covers following situations:
	//         "NEW"/"CREATE"    (file not existing)
	// new file was opened
	//
	G4cout << " New empty file : " << Setup::RootFileName << " opened " << G4endl;
	CreateNewTree();
      }
    G4cout << "LCRootOut::Init completed." << G4endl;
    }else{
     _LcalData = (TTree*)_fileROOT->Get("Lcal");
     if ( !_LcalData ) 
       G4Exception( " File ",
		    Setup::RootFileName,
		    RunMustBeAborted, " does not have class <Lcal>");
  } // if(!_fileROOT)

  InitLCIO();
}


void LCRootOut::InitLCIO()
{
  G4String lcioMode[2] = { "NEW", "APPEND" };
  G4String lciofname(Setup::RootFileName);
  unsigned int extpos = lciofname.find_last_of(".");
  if (extpos != lciofname.length() ) lciofname.erase(extpos, lciofname.length() - extpos);
  lciofname += ".slcio";
  
  _flcioWriter = IOIMPL::LCFactory::getInstance()->createLCWriter() ;
  int openMode = EVENT::LCIO::WRITE_NEW;
  if( Setup::RootFileMode == "UPDATE" ) openMode =  EVENT::LCIO::WRITE_APPEND;
  if ( openMode == EVENT::LCIO::WRITE_APPEND ){
  // BP. for APPEND mode check if file exists to avoid crash
    FILE *fc;
    std::string lsCmd("find -name ");
    lsCmd += lciofname;
    lsCmd += " -print";
    fc = popen( lsCmd.c_str(),"r");
    char ret[80] ;
    if( fgets( ret, 80, fc ) == NULL ) {
      openMode = EVENT::LCIO::WRITE_NEW; 
    }
    pclose(fc);
  }

  _flcioWriter->open( lciofname.c_str(), openMode ) ;
  
  G4cout << "Writing LCIO data to " << lciofname << " openMode " << lcioMode[openMode] <<  G4endl;
  
  _flcioRunHdr = new IMPL::LCRunHeaderImpl ; 
  _flcioRunHdr->setRunNumber( Setup::RunNumber ) ;
  _flcioRunHdr->setDetectorName( Setup::GlobalDetectorName ) ;
  
  _flcioRunHdr->setDescription (Setup::RunDescription);
  _flcioRunHdr->addActiveSubdetector ("LumiCal");
  // BP. add LcalParamaters
  // 
  float layer_thickness = 2.*(float)Setup::Lcal_layer_hdz;
  float absorber_thicness = 2.*(float)Setup::Lcal_tungsten_hdz;
  float bxing = 1000.*(float)Setup::Beam_Crossing_Angle;       // mrads are default
  float lstagger = (float)Setup::Lcal_layers_phi_offset / deg;
  _flcioRunHdr->parameters().setValue("SimulatingProgram", Setup::LCVersion );
  _flcioRunHdr->parameters().setValue("Beam_Crossing_Angle", bxing);
  _flcioRunHdr->parameters().setValue( "Lcal_layers_phi_offset", lstagger );
  _flcioRunHdr->parameters().setValue( "Lcal_n_layers",     Setup::Lcal_n_layers);
  _flcioRunHdr->parameters().setValue( "Lcal_n_sectors",    Setup::Lcal_n_sectors);
  _flcioRunHdr->parameters().setValue( "Lcal_n_rings",      Setup::Lcal_n_rings);
  _flcioRunHdr->parameters().setValue( "Lcal_SensRadMin",   (float)Setup::Lcal_SensRadMin);
  _flcioRunHdr->parameters().setValue( "Lcal_Cell0_radius", (float)Setup::Lcal_Cell0_radius);
  _flcioRunHdr->parameters().setValue( "Lcal_SensRadMax",   (float)Setup::Lcal_SensRadMax);
  _flcioRunHdr->parameters().setValue( "Lcal_Zbegin ",             (float)Setup::Lcal_sens_Z0);           
  _flcioRunHdr->parameters().setValue( "Lcal_layer_thickness", layer_thickness   );      
  _flcioRunHdr->parameters().setValue( "Lcal_absorber_thickness", absorber_thicness ); 
  _flcioRunHdr->parameters().setValue( "Lcal_dPhi",    (float)Setup::Lcal_sector_dphi);
  _flcioRunHdr->parameters().setValue( "Lcal_dRho",    (float)Setup::Lcal_CellPitch);

  _flcioWriter->writeRunHeader( _flcioRunHdr ) ;
}


void LCRootOut::ProcessEvent(const G4Event* event, LCHitsCollection *collection)
{
    numHits = 0; 
    numPrim = 0;
    Etot[0] = 0. ;
    Etot[1] = 0. ;
    Emax  = 0. ;
    //
    // get all primary MC particles
    G4int nv = event->GetNumberOfPrimaryVertex();
    for (int v = 0; v < nv ; v++) {
        G4PrimaryVertex *pv = event->GetPrimaryVertex(v);
	vX = pv->GetX0();
	vY = pv->GetY0();
	vZ = pv->GetZ0();
	G4PrimaryParticle *pp = pv->GetPrimary();
        while (pp) {
	 
	  Track_t t;
	  t.pX = (pp->GetMomentum()).getX();
	  t.pY = (pp->GetMomentum()).getY();
	  t.pZ = (pp->GetMomentum()).getZ();
	  t.ID =  pp->GetTrackID();
	  t.PDG = pp->GetPDGcode();
	  //
	  pTracks->push_back( t );
	  numPrim++;
	  pp = pp->GetNext();
        }
    }
  
    //    G4cout<<" Number of primary particles: "<<numPrim << G4endl;

    if( collection ){
    numHits = collection->entries();
    //
    G4int i = 0;
    while ( i < numHits){
      Hit_t hit;
      hit.cellID =(*collection)[i]->GetCellCode();
      G4int    side = ( (hit.cellID >> 24) & 0xff ) ;

      hit.eHit  = (*collection)[i]->GetEnergy();
      hit.xCell = (*collection)[i]->GetXcell();
      hit.yCell = (*collection)[i]->GetYcell();
      hit.zCell = (*collection)[i]->GetZcell();
      hit.xHit  = (*collection)[i]->GetXhit();
      hit.yHit  = (*collection)[i]->GetYhit();
      hit.zHit  = (*collection)[i]->GetZhit();
      hit.TOF   = (*collection)[i]->GetTOF();
      Etot[side-1] += hit.eHit ;
      if ( Emax < hit.eHit  ) Emax = hit.eHit;
      pHits->push_back( hit ); 
      //
      i++;
    }
  }
  _LcalData->Fill();

  ProcessEventLCIO(event, collection);

   //
    // clear vectors
    // 
    pTracks->clear();
    pHits->clear();


    /*    G4cout << "LCRootOut::ProcessEvent completed." << G4endl;
    G4cout << " Number of Hits "<< numHits << G4endl;
    G4cout << " Energy " << Etot1  / GeV << " [GeV]"<< G4endl; */
}



void LCRootOut::ProcessEventLCIO(const G4Event* event, LCHitsCollection *collection)
{
  IMPL::LCEventImpl*  evt = new IMPL::LCEventImpl() ;     // create the event 

  evt->setRunNumber(Setup::RunNumber);
  evt->setEventNumber(event->GetEventID());                  // set the event attributes
  evt->setDetectorName( "LumiCal" ) ;
  

  IMPL::LCCollectionVec* mcpVec = new IMPL::LCCollectionVec( EVENT::LCIO::MCPARTICLE );
  IMPL::MCParticleImpl  *mcp;
  
  numHits = 0; 
  numPrim = 0;
  Etot[0] = 0. ;
  Etot[1] = 0. ;
  Emax  = 0. ;
  //
  // get all primary MC particles
  G4int nv = event->GetNumberOfPrimaryVertex();
  for (int v = 0; v < nv ; v++) {
    G4PrimaryVertex *pv = event->GetPrimaryVertex(v);
    double vtc[3];
    vtc[0] = pv->GetX0();
    vtc[1] = pv->GetY0();
    vtc[2] = pv->GetZ0();
    G4PrimaryParticle *pp = pv->GetPrimary();
    while (pp) {
      mcp = new IMPL::MCParticleImpl;
      mcp->setVertex(vtc);

      double par[3];
      // BP. LCIO expects momentum in GeV
      par[0] = (pp->GetMomentum()).getX() / GeV;
      par[1] = (pp->GetMomentum()).getY() / GeV;
      par[2] = (pp->GetMomentum()).getZ() / GeV;
      mcp->setMomentum(par);
      mcp->setPDG(pp->GetPDGcode());
      mcp->setMass(pp->GetMass());
      mcp->setCharge(pp->GetCharge());
      mcp->setCreatedInSimulation(false);
      mcp->setDecayedInCalorimeter(true);

      mcpVec->push_back( mcp );
      numPrim++;
      pp = pp->GetNext();
    }
  }
  evt->addCollection(mcpVec,"MCParticle");

  //    G4cout<<" Number of primary particles: "<<numPrim << G4endl;

  IMPL::LCCollectionVec* calVec = new IMPL::LCCollectionVec( EVENT::LCIO::SIMCALORIMETERHIT );
  // BP. Must be set to get hit position on output
  calVec->setFlag( UTIL::make_bitset32(  LCIO::CHBIT_LONG, LCIO::CHBIT_STEP ) );

  // BP. Default Encoding : rCell 0-63, phiCell 0-47, layer 1-30, side 0-1 (LuCaS uses: 1-2)
  UTIL::CellIDEncoder<IMPL::SimCalorimeterHitImpl> cellid( "I:10,J:10,K:10,S-1:2" ,calVec ) ; 

  if( collection ) {
    numHits = collection->entries();
    //
   for ( int i=0; i<numHits; i++){
      IMPL::SimCalorimeterHitImpl* hit = new IMPL::SimCalorimeterHitImpl ;

      G4int cellcode = (*collection)[i]->GetCellCode();
      cellid["I"]   = (cellcode >> 0 ) & 0xff ;    // rCell 
      cellid["J"]   = (cellcode >> 8 ) & 0xff ;    // sector number, phiCell 
      cellid["K"]   = (cellcode >> 16) & 0xff ;    // layer number 
      cellid["S-1"] = ((cellcode >> 24) & 0xff) - 1 ;    // side, arm;
  
      cellid.setCellID( hit ) ;
      hit->setEnergy( (static_cast<float>((*collection)[i]->GetEnergy())) / GeV );

      float x= static_cast<float>((*collection)[i]->GetXcell());
      float y= static_cast<float>((*collection)[i]->GetYcell());
      float z= static_cast<float>((*collection)[i]->GetZcell());

      const float pos[3]={ x, y, z };
      hit -> setPosition( pos );
 
      calVec -> push_back( hit ) ;                // add hit objects to the collection
      //
    }
  }
  evt->addCollection(calVec,"LumiCalCollection");    // add the collection with a name

  _flcioWriter->writeEvent( evt ) ;                  // write the event to the file
  //  LCTOOLS::dumpEventDetailed( evt ) ;                         // dump the event to stdout
  delete evt ;
}



void LCRootOut::ProcEventAccumulate( LCHitsCollection *collection )
{
  // - B.P. 29.05.2009
  // this method is devoted to merge events from entire run
  // into one tree. It is to be used solely for beam-background data.
  // Primaries are not stored in the tree ( typically there is 10^5
  // primaries per event ). The list of tracks contributing to
  // a hit is neglected as well.  

    if( collection ){

    G4int       i = 0;
    G4int   nHits = collection->entries();
 
    while ( i < nHits){

        G4int address = (*collection)[i]->GetCellCode();
	G4int    side = (address >> 24) & 0xff ;
	G4double eH  = (*collection)[i]->GetEnergy();
	unsigned int it = 0;
	while ( it < theUsedCells.size() ){
	  if( theUsedCells[it] == address ) break;
	  it++;
	}

	if ( it == theUsedCells.size() ) { // cellID not in the list, add

	  theUsedCells.push_back( address );
	  Hit_t hit;
	  hit.eHit  = eH;
	  hit.xCell = (*collection)[i]->GetXcell();
	  hit.yCell = (*collection)[i]->GetYcell();
	  hit.zCell = (*collection)[i]->GetZcell();
	  hit.xHit  = (*collection)[i]->GetXhit();
	  hit.yHit  = (*collection)[i]->GetYhit();
	  hit.zHit  = (*collection)[i]->GetZhit();
	  hit.TOF   = (*collection)[i]->GetTOF();
	  if ( Emax < eH ) Emax = eH;
	  pHits->push_back( hit ); 
	  numHits++;
	}else{        // cell in list, increase energy deposit only
	  (*pHits)[it].eHit += eH;
	  if ( (*pHits)[it].eHit > Emax ) Emax = (*pHits)[it].eHit;
	}
        Etot[side-1] += eH ;
        i++;
    }

    }

}

void LCRootOut::End()
{
  // end of run action
  // ROOT
  if ( Setup::AccumulateEvents ){          //fill tree, do not close the file
    _LcalData->Fill();                     // it will be closed by main()
    theUsedCells.clear();
  }else{
    _fileROOT->Write();
    _fileROOT->Close();
    G4cout << "LCRootOut::Closed file: " << Setup::RootFileName << G4endl;
    delete _fileROOT;
    _fileROOT = NULL;
    _LcalData = NULL;
  }
  // LCIO
  delete _flcioRunHdr  ;   
  _flcioWriter->close();
  delete _flcioWriter;

}
