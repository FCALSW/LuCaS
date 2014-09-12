//============================================================================
// Name        : LumiCalJA.cpp
// Author      : Jonathan Aguilar
// Version     : 1
// Copyright   :
// Description : LumiCal stand-alone application for Geant4
//============================================================================
/*
 $Id$
*/

// 
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/times.h>
#include <string.h>
#include <unistd.h>

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UItcsh.hh"
#include "G4UIterminal.hh"

#include "PrimaryGeneratorAction.hh"
#include "SteppingAction.hh"

#include "LCDetectorConstruction.hh"
#include "PhysicsListFactory.hh"
#include "LCRunAction.hh"
#include "LCEventAction.hh"
#include "LCRootOut.hh"
#include "Setup.hh"
#include "banner.hh"


#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif


#include <iostream>
using namespace std;

int main( int argc, char* argv[] ) {
 //
 //
  char *Me = strrchr(argv[0], '/');
  if (Me == NULL) Me = argv[0];
    else Me++;
  print_lucas_banner();
  LCRootOut *theLCRootOut = NULL;
  G4RunManager* runManager = new G4RunManager;
    //
    Setup* theSetup = Setup::GetSetup();
    theSetup -> SetupInit( argc, argv );
    // User initialization classes
    runManager->SetUserInitialization(new LCDetectorConstruction);
    // 
    G4VUserPhysicsList *theList = PhysicsListFactory::create( Setup::PhysicsListName );
    theList -> SetDefaultCutValue( Setup::rangeCut );
    runManager->SetUserInitialization( theList );

 
    tms fStartTime;
    clock_t StartTime = times (&fStartTime);    // times returns time in miliseconds
 

    // Initialize the run manager
    // 
    runManager->Initialize();

    // User Action Classes
    runManager->SetUserAction(new PrimaryGeneratorAction);
    //

    if( Setup::batchMode ){   // Initialize the ROOT output class
      LCRootOut *theRootOut = new LCRootOut();
      LCEventAction *theEventAction = new LCEventAction( theRootOut );
    runManager->SetUserAction(new LCRunAction  ( theRootOut ));
    runManager->SetUserAction( theEventAction );
    runManager->SetUserAction(new SteppingAction ( theEventAction) );
    }else{
      LCEventAction *theEventAction = new LCEventAction();
    runManager->SetUserAction(new LCRunAction());
    runManager->SetUserAction( theEventAction );
    runManager->SetUserAction(new SteppingAction ( theEventAction ) );
    }

    //
    // Get pointer to the UI 
    //
    G4UImanager* UI = G4UImanager::GetUIpointer();
    G4cout << "Setup::batchMode " << Setup::batchMode << G4endl;
    G4cout << "/control/execute " + Setup::macroName  << G4endl;


    if ( Setup::batchMode )    // batch mode
      {
     G4String command = "/control/execute ";
     UI->ApplyCommand(command + Setup::macroName);
      }
    else               // interactive mode
      {
#ifdef G4VIS_USE
    // visualization manager
    G4VisManager* visManager = new G4VisExecutive;
    visManager->Initialize();
#endif
//    G4int numberOfEvent = 3;
//    runManager->BeamOn(numberOfEvent);

    // Run from UI
    G4UIsession *session = 0;
#ifdef G4UI_USE_TCSH
    session = new G4UIterminal(new G4UItcsh);
#else
    session = new G4UIterminal();
#endif
    // send output of G4cout and G4cerr to external files
    // Specifically, session will use methods ReceiveG4cout and "G4err

    UI->SetCoutDestination(session);
    G4String command = "/control/execute ";
    if( Setup::macroName != ""  )UI->ApplyCommand(command + Setup::macroName);
    session->SessionStart();
    delete session;

#ifdef G4VIS_USE
    delete visManager;
#endif
      }
    // Job termination - free the store. Do not delete physicslist, detectorconstruction,
    // other user actions, because those are deleted by the run manager
    if ( Setup::AccumulateEvents ) {
      if ( LCRootOut::pRootFile )  {
       G4cout<< " Writing to and closing file : "<< Setup::RootFileName << G4endl;
	 LCRootOut::pRootFile->Write();
	 LCRootOut::pRootFile->Close();
       G4cout <<Me<< " main::Closed file: " << Setup::RootFileName << G4endl;
       }
       delete theLCRootOut;
     }
    delete runManager;
    //
    time_t now; time(&now);
    tms fEndTime;
    clock_t EndTime = times (&fEndTime);    // times returns time in 10 miliseconds units
    G4double diff = 10.*( EndTime - StartTime ) *ms ; 
    
     G4cout << " End Job -  time elapsed : " << diff / s << " seconds " << G4endl; 
     delete theSetup;
     G4cout << "\n**** "<< Me <<" run ended  at " << ctime(&now) << G4endl;;
   return 0;
 }
