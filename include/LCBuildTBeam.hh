
/* $Id$ */
/* * This class builds Lumical Test Beam detector 
   * DUT version 2011
 */

#ifndef LCBUILD_TBEAM_HH_
#define LCBUILD_TBEAM_HH_

#include "G4SDManager.hh"
#include "LCSensitiveDetector.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include "G4ProductionCuts.hh"
#include "G4SubtractionSolid.hh"
#include "G4VisAttributes.hh"
#include "Setup.hh"


class LCBuildTBeam
{

public: 
   LCBuildTBeam();
  ~LCBuildTBeam();

private:  
   G4Material  *Air, *Silicon, *Aluminium, *Tungsten;
   G4Material  *FanoutMatF, *FanoutMatB;

     G4bool    VirtualCell;
     G4double  Cell0_Rad, CellPitch;
     G4double  startPhi, sectorPhi, SensRadMin, SensRadMax, deadPhi;
     G4double  hSiliconDZ, hMetalDZ, hFanoutBackDZ, hFanoutFrontDZ;
     G4double  hTungstenDZ;
     G4double  zposLC;
     G4int     nLayers, nSectors, nCells;


public: 
    void BuildTBeam( G4LogicalVolume * logWorld );

};

#endif //LCBUILD_TBEAM_HH_
