/* 
$Id$
*/
#include "LCBuildLHcal.hh"


LCBuildLHcal::LCBuildLHcal()
{/*-NOP-*/}

LCBuildLHcal::~LCBuildLHcal()
{/*-NOP-*/}

void LCBuildLHcal::BuildLHcal( G4LogicalVolume * logicWorld )
{
  // Globals
 G4double rotAng      = Setup::Beam_Crossing_Angle / 2.;
 G4double rotAng1     = 180.*deg - rotAng;
 G4double rotAng2     = rotAng;

 // LHcal geo consts
  G4Material *LHcalMat = Setup::Tungsten;
  G4double startPhi    = Setup::Lcal_start_phi;
  G4double endPhi      = Setup::Lcal_end_phi;
  G4double LHcal_rmin  =   93.0 *mm;
  G4double LHcal_rmax  =  330.0 *mm;
  G4double LHcal_hDZ   = (525.0 /2.) *mm;
  G4double LHcalToLCalDist = 45. *mm;

   G4double LHcal_zbegin = Setup::Lcal_z_end +  LHcalToLCalDist ;

  //
   G4cout <<  " Building LHCAL ..." << G4endl; 
    G4double zpos = LHcal_zbegin + LHcal_hDZ;

    G4Transform3D trans1( G4RotationMatrix().rotateY(rotAng1),
                          G4ThreeVector( 0., 0., zpos).rotateY(rotAng1));
    G4Transform3D trans2( G4RotationMatrix().rotateY(rotAng2),
                          G4ThreeVector( 0., 0., zpos).rotateY(rotAng2));

    G4Box *solidtmp = new G4Box ( "solidtmp", LHcal_rmax, LHcal_rmax, LHcal_hDZ );
    G4Tubs *solidpunch = new G4Tubs ( "solidpunch", 0., LHcal_rmin, 2.*LHcal_hDZ, startPhi, endPhi );
    G4SubtractionSolid *solidLHcal = new G4SubtractionSolid( "solidLHcal",
							     solidtmp, solidpunch, 0, G4ThreeVector());
    G4LogicalVolume *logicLHcal = new G4LogicalVolume( solidLHcal, LHcalMat, "logicLHcal", 0, 0, 0);
 
                  new G4PVPlacement( trans1 ,
                                     logicLHcal,
                                     "LHcal1",
                                     logicWorld,
                                     false,
                                     1);
                  new G4PVPlacement( trans2 ,
                                     logicLHcal,
                                     "LHcal2",
                                     logicWorld,
                                     false,
                                     2);
     G4VisAttributes *LHcalVisAtt = new G4VisAttributes(G4Colour(0.7, 0.3, 0.7));
  
       if(Setup::LHcal_VisSolid ) LHcalVisAtt->SetForceSolid(true);
        else LHcalVisAtt->SetForceWireframe(true);
	 logicLHcal->SetVisAttributes(LHcalVisAtt);

    // define a subdetector region for LHCAL 
    G4Region *regionLHcal = new G4Region("LHCAL");
    logicLHcal->SetRegion(regionLHcal);
    regionLHcal->AddRootLogicalVolume( logicLHcal );
    //  LHCAL
    if( Setup::Build_LHcal == "Yes" ){
    G4ProductionCuts *cutsLHcal = new G4ProductionCuts();
    cutsLHcal->SetProductionCut( Setup::LHcal_Region_Cut *mm);
   regionLHcal->SetProductionCuts(cutsLHcal);
  }

  
  G4cout <<  "                   .....done ! " << G4endl; 
}
