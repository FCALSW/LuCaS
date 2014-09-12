/*
 * LCPhysicsList.cc
 * 2ModLumiCal
 *
 *  Created on: Mar 23, 2009
 *      Author: aguilar
 *
 *      Taken from file of the same name in Mokka framework
 */

//
/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//  Title:  Physics List for a Linear Collider Detector                    //
//  Date:   16 June 2004                                                   //
//  Author: D.H. Wright (SLAC)                                             //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
//

#include "LCPhysicsList.hh"


#include "LCDecayPhysics.hh"
#include "LCBosonPhysics.hh"
#include "LCLeptonPhysics.hh"
#include "LCHadronPhysics.hh"
#include "LCIonPhysics.hh"

LCPhysicsList::LCPhysicsList():  G4VModularPhysicsList()
{

  // fg: document the usage of LCPhys in the logfile
  G4cout << "You are using the simulation engine: LCPhys" << G4endl ;
  G4cout  << G4endl << G4endl ;

  // default cut value  (1.0mm)
  defaultCutValue = 5*um;
  // SetVerboseLevel(1);

  // Particle decays
  RegisterPhysics( new LCDecayPhysics("decay"));

  // Bosons (gamma + geantinos)
  RegisterPhysics( new LCBosonPhysics("boson"));

  // Leptons
  RegisterPhysics( new LCLeptonPhysics("lepton"));

  // Hadron Physics
  RegisterPhysics( new LCHadronPhysics("hadron"));

  // Ion Physics
  RegisterPhysics( new LCIonPhysics("ion"));
}

LCPhysicsList::~LCPhysicsList()
{;}

void LCPhysicsList::SetCuts()
{
  // Use default cut values gamma and e processes
  SetCutsWithDefault();
}
