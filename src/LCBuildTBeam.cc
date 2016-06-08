
#include "LCBuildTBeam.hh"


LCBuildTBeam::LCBuildTBeam()
{
  Silicon     = Setup::Silicon;               // Sensor Cell
  Aluminium   = Setup::Alu;                   // material for LCAL mechanical parts
  Tungsten    = Setup::Wabsorber;             // Absorber plate
  FanoutMatF  = Setup::FanoutMatF;            // Front Fanout material
  FanoutMatB  = Setup::FanoutMatB;            // Back Fanout material
  VirtualCell = Setup::Lcal_virtual_cells;                // cell type 
  nLayers     = Setup::Lcal_n_layers;
  nSectors    = Setup::Lcal_n_sectors;
  nCells      = Setup::Lcal_n_rings;
  Cell0_Rad   = Setup::Lcal_Cell0_radius;
  CellPitch   = Setup::Lcal_CellPitch;
  startPhi    = Setup::Lcal_start_phi;
  sectorPhi   = Setup::Lcal_sector_dphi;
   SensRadMin = Setup::Lcal_SensRadMin;        // silicon sensor r-min including dead edges
  SensRadMax  = Setup::Lcal_SensRadMax;        // silicon sensor r-max
  deadPhi     = Setup::Lcal_tile_gap;
  hTungstenDZ = Setup::Lcal_tungsten_hdz;      // half thickness absorber
  hSiliconDZ  = Setup::Lcal_silicon_hdz;       // half thickness of the silicon
  hMetalDZ    = Setup::Lcal_pad_metal_thickness/2.; // half thickness of the pad metallization
  zposLC      = Setup::Lcal_z_end;

}

LCBuildTBeam::~LCBuildTBeam()
{
}

void LCBuildTBeam::BuildTBeam( G4LogicalVolume *logicWorld )
{

  // alu box for sensors
  G4double aluhx = 110.0 *mm;
  G4double aluhy = 110.0 *mm;
  G4double aluhz = 10.5 *mm;

  G4Box *solidSpacer = new G4Box ( "solidSpacer", aluhx, aluhy, aluhz );
  G4LogicalVolume *logicSpacer = new G4LogicalVolume (solidSpacer, Aluminium, "logicSpacer", 0, 0, 0);

  // PCB
  G4double pcbhx = 110.0 *mm;
  G4double pcbhy = 110.0 *mm;
  G4double pcbhz = 1.238 *mm;

  G4Box *solidPCB = new G4Box ( "solidPCB", pcbhx, pcbhy, pcbhz );
  G4LogicalVolume *logicPCB = new G4LogicalVolume (solidPCB, Setup::FR4, "logicPCB", 0, 0, 0);

  // copper surface on PCB
  G4double Cuhz = 0.019 *mm;
  G4Box *solidCu = new G4Box ( "solidCu", pcbhx, pcbhy, Cuhz );
  G4LogicalVolume *logicCu = new G4LogicalVolume (solidCu, Setup::Copper, "logicCu", 0, 0, 0);
  // Copper layers on PCB
  new G4PVPlacement ( 0, G4ThreeVector(0., 0., (pcbhz - Cuhz)), logicCu, "CuLayer", logicPCB, false, 1);
  new G4PVPlacement ( 0, G4ThreeVector(0., 0., (-pcbhz + Cuhz)), logicCu, "CuLayer", logicPCB, false, 0);

  // PCB into alu box
  G4double PCBdistZ = 8.0*mm;
  G4double zposPCB0 = -aluhz + pcbhz;
  G4double zposPCB1 = -aluhz + 2.*pcbhz + PCBdistZ + pcbhz ; 
  new G4PVPlacement ( 0, G4ThreeVector(0., 0., zposPCB0 ), logicPCB, "PCB0", logicSpacer, false, 0);
  new G4PVPlacement ( 0, G4ThreeVector(0., 0., zposPCB1 ), logicPCB, "PCB1", logicSpacer, false, 0);

  // sensor air spaces
  G4double space0hz = 1.0 *mm;
  G4double space1hz = 4.0 *mm;
  G4double spaceWhz = 2.25 *mm;
  G4double space0hy = 105.0 *mm;
  G4double space0hx = 105.0 *mm;
  G4Box *solidLayer0 = new G4Box ( "solidLayer0", space0hx, space0hy, space0hz );
  G4Box *solidLayer1 = new G4Box ( "solidLayer1", space0hx, space0hy, space1hz );
  G4Box *solidLayerW = new G4Box ( "solidLayerW", space0hx, space0hy, spaceWhz );
  //
  G4LogicalVolume *logicLayer0 = new G4LogicalVolume (solidLayer0, Air, "logicLayer0", 0, 0, 0);
  G4LogicalVolume *logicLayer1 = new G4LogicalVolume (solidLayer1, Air, "logicLayer1", 0, 0, 0);
  G4LogicalVolume *logicLayerW = new G4LogicalVolume (solidLayerW, Air, "logicLayerW", 0, 0, 0);

  // sensor
  G4double sPhi = 75. *deg;
  G4double dPhi = 30. *deg;
     G4Tubs *solidSensV = new G4Tubs("solidSensorV", SensRadMin + deadPhi ,SensRadMax - deadPhi, hSiliconDZ, sPhi, dPhi);
     G4Tubs *solidMetal = new G4Tubs("solidMetal", SensRadMin + deadPhi ,SensRadMax - deadPhi, hMetalDZ, sPhi, dPhi);
     G4Tubs *solidFanoutFrnt  = new G4Tubs("solidFanoutFrnt",SensRadMin, SensRadMax, hFanoutFrontDZ, sPhi,dPhi);
     G4LogicalVolume *logicSensorV = new G4LogicalVolume(solidSensV, Silicon, "logicSensorV", 0,0,0);
     G4LogicalVolume *logicMetalV = new G4LogicalVolume(solidMetal, Aluminium, "logicMetalV", 0,0,0);
     G4LogicalVolume *logicFanoutFrnt = new G4LogicalVolume(solidFanoutFrnt, FanoutMatF,"logicFanoutFront", 0, 0, 0);  
     //
     // sensor layer 0
     //
     G4double zSi0 = -space0hz + hSiliconDZ;
     G4double zAl0 = zSi0 + hSiliconDZ + hMetalDZ;
     G4double zFun0 = zAl0 + hMetalDZ + hFanoutFrontDZ;
     G4double ypos = -( SensRadMin + 0.5*(SensRadMax-SensRadMin));
     new G4PVPlacement ( 0, G4ThreeVector( 0., ypos, zSi0), logicSensorV, "SensorV0", logicLayer0, false, 0);
     new G4PVPlacement ( 0, G4ThreeVector( 0., ypos, zAl0), logicMetalV, "PadMetal0", logicLayer0, false, 0);
     new G4PVPlacement ( 0, G4ThreeVector( 0., ypos, zFun0), logicFanoutFrnt, "FunOut0", logicLayer0, false, 0);
     //
     // sensor layer 1
     //
     G4double zSi1 = -space1hz + hSiliconDZ;
     G4double zAl1 = zSi1+ hSiliconDZ + hMetalDZ;
     G4double zFun1 = zAl1 + hMetalDZ + hFanoutFrontDZ;
     new G4PVPlacement ( 0, G4ThreeVector( 0., ypos, zSi1), logicSensorV, "SensorV1", logicLayer1, false, 1);
     new G4PVPlacement ( 0, G4ThreeVector( 0., ypos, zAl1), logicMetalV, "PadMetal1", logicLayer1, false, 1);
     new G4PVPlacement ( 0, G4ThreeVector( 0., ypos, zFun1), logicFanoutFrnt, "FunOut1", logicLayer1, false, 1);
     //
     // absorber (fixed) non existent in run 2 of 2011
     //
     if ( nLayers > 0){
     G4double zposAbs0 = -spaceWhz + hTungstenDZ;
     G4Box *solidAbs0 = new G4Box("solidAbs0", 60.*mm, 60.*mm, hTungstenDZ );
     G4LogicalVolume *logicAbs0 = new G4LogicalVolume(solidAbs0, Tungsten, "logicAbs0", 0, 0, 0);
     new G4PVPlacement ( 0, G4ThreeVector( 0., 0., zposAbs0 ), logicAbs0, "Absorber0", logicLayerW, false, 1);
     }
     //
     // put layers to alubox
     //
     G4double dwall = 1.5 *mm;
     G4double zpos0 = zposPCB0 + pcbhz + space0hz;
     G4double zpos1 = zpos0 + space0hz + dwall + spaceWhz;
     G4double zpos2 = zposPCB1 + pcbhz + space1hz;
     new G4PVPlacement ( 0, G4ThreeVector( 0., 0., zpos0 ), logicLayer0, "layer0", logicSpacer, false, 0);
     new G4PVPlacement ( 0, G4ThreeVector( 0., 0., zpos1 ), logicLayerW, "layerW", logicSpacer, false, 0);
     new G4PVPlacement ( 0, G4ThreeVector( 0., 0., zpos2 ), logicLayer1, "layer1", logicSpacer, false, 1);
     //
     // put LCAL DUT to world
     //
     new G4PVPlacement ( 0, G4ThreeVector( 0., 0., zposLC ), logicSpacer, "DUT", logicWorld, false, 1);

     //
     // absorber ( variable thicknes )
     //
     if ( abs(nLayers) > 0 ) {
       G4double absorberhz = fabs((double)nLayers) * hTungstenDZ ;
       G4Box *solidAbs1 = new G4Box("solidAbs1", 60.*mm, 60.*mm, absorberhz );
       G4LogicalVolume *logicAbs1 = new G4LogicalVolume(solidAbs1, Tungsten, "logicAbs1", 0, 0, 0);
       new G4PVPlacement ( 0, G4ThreeVector( 0., 0., zposLC+aluhz+absorberhz ), logicAbs1, "Absorber1", logicWorld, false, 0);
     }

   //---------------
    // SENSITIVE DETECTOR
    //---------------
    G4SDManager* SDman = G4SDManager::GetSDMpointer();

    // Initialize the Sensitive Detector
    LCSensitiveDetector *SensDet = new LCSensitiveDetector("LumiCalSD",  // name
                                      Cell0_Rad,    // inner LC radius
                                      startPhi,     // start angle
                                      CellPitch,    // radial cell size
                                      sectorPhi,    // angular cell width
                                      nCells,       // # cells in the rad dir
                                      nSectors,     // # cells in the phi dir
				      VirtualCell); // cell type real/virtual =  false/true
        
    SDman->AddNewDetector(SensDet);
    // the Cells are the sensitive detectors
   
    logicSensorV->SetSensitiveDetector( SensDet );



  G4cout <<  " Test Beam setup done !  "  << G4endl;

}
