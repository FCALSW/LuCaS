/*
 * LCDecayPhysics.hh
 * 2ModLumiCal
 *
 *  Created on: Mar 23, 2009
 *      Author: Jonathan Aguilar
 */
//
////////////////////////////////////////////////////////////////
//                                                            //
//  Title:  Decay physics for a Linear Collider Detector      //
//  Date:   16 June 2004                                      //
//  Author: D.H. Wright (SLAC)                                //
//                                                            //
////////////////////////////////////////////////////////////////
//

#ifndef LCDECAYPHYSICS_HH_
#define LCDECAYPHYSICS_HH_

#include "G4VPhysicsConstructor.hh"


#include "G4Decay.hh"

class LCDecayPhysics : public G4VPhysicsConstructor
{
  public:
    LCDecayPhysics(const G4String& name = "decay");
    virtual ~LCDecayPhysics();

    // This method will be invoked in the Construct() method.
    // each particle type will be instantiated
    virtual void ConstructParticle();

    // This method will be invoked in the Construct() method.
    // each physics process will be instantiated and
    // registered to the process manager of each particle type
    virtual void ConstructProcess();

  protected:
    G4Decay fDecayProcess;
};

#endif /* LCDECAYPHYSICS_HH_ */
