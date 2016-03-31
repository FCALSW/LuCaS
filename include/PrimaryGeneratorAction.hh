// *********************************************************
// *                         Mokka                         *
// *    -- A detailed Geant 4 simulation for the ILC --    *
// *                                                       *
// *  polywww.in2p3.fr/geant4/tesla/www/mokka/mokka.html   *
// *********************************************************
//
// $Id$
// $Name: mokka-06-07-patch02 $

#ifndef PrimaryGeneratorAction_hh
#define PrimaryGeneratorAction_hh 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

class PrimaryGeneratorMessenger;
class VPrimaryGenerator;
class G4Event;

class PrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction(void);
  ~PrimaryGeneratorAction(void);

  void GeneratePrimaries(G4Event *evt);
  void SetGeneratorWithName(G4String generatorName);
  void ApplyLorentzTransformation(G4Event *evt);



  VPrimaryGenerator* GetPrimaryGenerator(void) { return fPrimaryGenerator; }

private:

  PrimaryGeneratorMessenger *fMessenger;
  VPrimaryGenerator *fPrimaryGenerator;

};

#endif
