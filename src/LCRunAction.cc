/* $Id$
 * LCRunAction.cc
 */
#include <sys/times.h>
#include "LCRunAction.hh"
#include "Setup.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4ios.hh"
#include "G4HCtable.hh"
#include "Randomize.hh"


LCRunAction::LCRunAction(LCRootOut *RO)
{
    RootOut = RO;
}
LCRunAction::LCRunAction()
{
    RootOut = 0;
}

LCRunAction::~LCRunAction()
{;}

void LCRunAction::BeginOfRunAction(const G4Run* Run)
{
  tms fTimeNow;
  Setup::GetSetup()->SetRunNumber( Run->GetRunID() );
    G4cout << "Run " << Setup::RunNumber << " start." << G4endl;
    G4cout << "To stop the run gently, any time you wish do:  shell> touch StopTheRunNow" << G4endl;
 
    Setup::StartTime = times( &fTimeNow );
    Setup::NoOfEventsToProcess = Run->GetNumberOfEventToBeProcessed() - Setup::EventStartNumber;
    if( Setup::Particle_Generator.contains("pairs.") ) Setup::AccumulateEvents = true ;
   //
    // set random seed manually
    //
    G4long seed = Setup::StartTime;
    G4RandGauss::setTheSeed(seed);
    //    CLHEP::HepRandom::setTheSeed(seed);
    Setup::StartRandomSeed = seed;
    G4cout << " Starting with random seed " << seed << G4endl;

    if( RootOut ) RootOut->Init();

    Print("RUN_BEGIN", Run);
    PrintLucasGearXML();

}

void LCRunAction::EndOfRunAction(const G4Run* Run )
{
 
  Print("END_OF_RUN", Run);

  if( RootOut )  RootOut->End();

}

void LCRunAction::Print(G4String now, const G4Run* Run)
{
  std::string YesNo[2] = { "No", "Yes" };
  time_t tnow = time(NULL);
  G4cout << "======================================================================"<<G4endl;
  if ( now == "END_OF_RUN") {
      tms fTimeNow;
      clock_t EndRunT = times( &fTimeNow );
      G4double diff = 10.*( EndRunT - Setup::StartTime ) *ms ;
      G4int EventsProcessed = Run->GetNumberOfEvent(); 
      G4cout << "|                End of Run  :  "<< Setup::RunNumber << G4endl;
      G4cout << "|                  time now  :  "<< ctime(&tnow) ;
      G4cout << "|            Events Processed:  "<< EventsProcessed<< G4endl;
      if ( Setup::batchMode ) {
      G4cout << "|             written to file:  "<< Setup::RootFileName << G4endl;
      }
      G4cout << "|                Time elapsed:  "<< diff / s << " seconds."<< G4endl;
      G4cout << "|    Time to process an event:  "<< diff/G4double(EventsProcessed) / s << " seconds."<< G4endl;
  } else {
      G4cout << "|                     Begin of Run  : "<< Setup::RunNumber << G4endl;
      G4cout << "|                         time now  : "<< ctime(&tnow) ;
      G4cout << "|                      Random Seed  : "<< Setup::StartRandomSeed << G4endl;
      G4cout << "| Global Parameters for the Run are : "           <<G4endl;
      G4cout << "---------------------------------------------------------------------"<<G4endl;
      G4cout << "|                   batchMode:  "<< Setup::batchMode << G4endl;
      G4cout << "|                   macroName:  "<< Setup::macroName << G4endl;
      G4cout << "|                  PrintLevel:  "<< Setup::PrintLevel << G4endl;
      G4cout << "|           Logging frequency:  "<< Setup::LogFreq << G4endl;
      G4cout << "|             PhysicsListName:  "<< Setup::PhysicsListName << G4endl;
      if ( Setup::batchMode ) {
	G4cout << "|       ROOT output file name:  "<< Setup::RootFileName << G4endl;
	G4cout << "|       ROOT output open mode:  "<< Setup::RootFileMode << G4endl;
	G4cout << "|           accumulate events:  "<< YesNo[Setup::AccumulateEvents] << G4endl;
      }
      G4cout << "|                   SetupFile:  "<< Setup::SetupFile << G4endl;
      G4cout << "|         Beam_Crossing_Angle:  "<< Setup::Beam_Crossing_Angle / mrad << " [mrad]" << G4endl;
      G4cout << "|         Nominal field value:  "<< Setup::Nominal_Field_value / tesla << " [T]"<< G4endl;
      G4cout << "|          Particle generator:  "<< Setup::Particle_Generator<< G4endl;
      G4cout << "| Number of events to process:  "<< Setup::NoOfEventsToProcess << G4endl;
      G4cout << "|   Detector components build:  "<< G4endl;
      G4cout << "|                   Beam Tube:  "<< Setup:: Build_Beampipe << G4endl;
      G4cout << "|                        LCAL:  "<< Setup:: Build_LCal << G4endl;
      G4cout << "|                       LHCAL:  "<< Setup:: Build_LHcal << G4endl;
      G4cout << "|                        BCAL:  "<< Setup:: Build_BCal << G4endl;
      G4cout << "|                        MASK:  "<< Setup:: Build_Mask << G4endl;
      G4cout << "|                    rangeCut:  "<< Setup::rangeCut/ mm << " [mm]"<<G4endl;
      G4cout << "|   Region Production Cuts:     "<< G4endl;
      G4cout << "|                        LCAL:  "<<  Setup::LCal_Region_Cut / mm <<" [mm]"<< G4endl;
      G4cout << "|                       LHCAL:  "<<  Setup::LHcal_Region_Cut / mm <<" [mm]"<< G4endl;
      G4cout << "|                        BCAL:  "<<  Setup::BCal_Region_Cut / mm <<" [mm]"<< G4endl;
      G4cout << "|                        MASK:  "<<  Setup::Mask_Region_Cut / mm <<" [mm]"<< G4endl;
  }
  G4cout << "========================================================================"<<G4endl;
}

void LCRunAction::PrintLucasGearXML(){

  fGearFile = new std::ofstream( "LucasGear.xml", std::ios::out);

  *fGearFile << "<gear>" << std::endl;
  *fGearFile << "\t <global detectorName=\"" << Setup::GlobalDetectorName << "\" />" << std::endl;    
  *fGearFile << "\t <BField type=\"ConstantBField\" x=\"0.000000000e+00\" y=\"0.000000000e+00\" z=\"3.500000000e+00\" />" << std::endl
         << "\t<detectors>" << std::endl;
  float lThickness = 2.*(float)Setup::Lcal_layer_hdz;
  float aThickness = 2.*(float)Setup::Lcal_tungsten_hdz;
  *fGearFile << "\t<detector name=\"Lcal\" geartype=\"CalorimeterParameters\">" << std::endl
         << "\t\t<layout type=\"Endcap\" symmetry=\"1\" phi0=\"0.000000000e+00\" />" << std::endl
	     << "\t\t<dimensions inner_r=\"" << Setup::Lcal_Cell0_radius << "\" outer_r=\"" << Setup::Lcal_SensRadMax << "\" inner_z=\"" << Setup::Lcal_sens_Z0 << "\" />\n"
	     << "\t\t<layer repeat=\"" << Setup::Lcal_n_layers << "\" thickness=\"" << lThickness << "\" absorberThickness=\"" << aThickness 
	     << "\" cellSize0=\"" << Setup::Lcal_CellPitch << "\" cellSize1=\"" << Setup::Lcal_sector_dphi << "\" /> \n"
         << "\t\t<parameter name=\"beam_crossing_angle\" type=\"double\" value=\"" << 1000.*Setup::Beam_Crossing_Angle << "\" /> \n"
	 << "\t</detector>"<< std::endl;
   
 *fGearFile << "<detector name=\"LucasParameters\" geartype=\"GearParameters\"> \n"
            << "\t\t<parameter name=\"LucasVersion\" type=\"string\" value=\"tag "<< Setup::LCVersion << "\" /> \n"
    /*
            << "\t\t<parameter name="Lcal_beam_pipe_clearance" type="string" value="0.5" />
            << "\t\t<parameter name="Lcal_extra_size" type="string" value="26" />
            << "\t\t<parameter name="Lcal_inner_radius" type="string" value="80.0" />
            << "\t\t<parameter name="Lcal_layer_gap" type="string" value="0.25" />
            << "\t\t<parameter name="Lcal_n_layers" type="string" value="30" />
            << "\t\t<parameter name="Lcal_n_tiles" type="string" value="12" />
            << "\t\t<parameter name="Lcal_nstrips_phi" type="string" value="48" />
            << "\t\t<parameter name="Lcal_nstrips_theta" type="string" value="64" />
            << "\t\t<parameter name="Lcal_outer_radius" type="string" value="195.2" />
            << "\t\t<parameter name="Lcal_phi_offset" type="string" value="0" />
            << "\t\t<parameter name="Lcal_sensor_phi_offset" type="string" value="3.75" />
            << "\t\t<parameter name="Lcal_silicon_thickness" type="string" value="0.32" />
            << "\t\t<parameter name="Lcal_support_thickness" type="string" value="0.2" />
            << "\t\t<parameter name="Lcal_tile_gap" type="string" value="1.2" />
            << "\t\t<parameter name="Lcal_tungsten_thickness" type="string" value="3.5" />
            << "\t\t<parameter name="Lcal_z_begin" type="string" value="2506.9" />
            << "\t\t<parameter name="Lcal_z_thickness" type="string" value="128.1" />
    */
            << "\t</detector>" << std::endl;
    
 *fGearFile << "\t</detectors>  \n";
 *fGearFile<< "</gear>" << std::endl;
 fGearFile->close();
 std::cout << " LCRunAction::BeginOfRunAction: Geometry parameters saved in: LucasGear.xml ."<<std::endl;

}
