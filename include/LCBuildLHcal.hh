/* This class builds dummy LHcal detector 
$Id$
 */
#ifndef LCBUILD_LHCAL_HH_
#define LCBUILD_LHCAL_HH_

#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include "G4ProductionCuts.hh"
#include "G4SubtractionSolid.hh"
#include "G4VisAttributes.hh"
#include "Setup.hh"


class LCBuildLHcal
{
  public:

    LCBuildLHcal();
  ~LCBuildLHcal();

  public:
    void BuildLHcal( G4LogicalVolume * logWorld );

};

#endif //LCBUILD_LHCAL_HH_
