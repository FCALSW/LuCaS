/*
 * LCIonPhysics.hh
 * 2ModLumiCal
 *
 *  Created on: Mar 23, 2009
 *      Author: Jonathan Aguilar
 */
//
/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//  Title:  Ion physics for a Linear Collider Detector                     //
//  Date:   6 July 2004                                                    //
//  Author: D.H. Wright (SLAC)                                             //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
//

#ifndef LCIONPHYSICS_HH_
#define LCIONPHYSICS_HH_

#include "G4VPhysicsConstructor.hh"


class LCIonPhysics : public G4VPhysicsConstructor
{
  public:
    LCIonPhysics(const G4String& name ="ion");
    virtual ~LCIonPhysics();

    // This method will be invoked in the Construct() method.
    // each particle type will be instantiated
    virtual void ConstructParticle();

    // This method will be invoked in the Construct() method.
    // each physics process will be instantiated and
    // registered to the process manager of each particle type
    virtual void ConstructProcess();

};

#endif /* LCIONPHYSICS_HH_ */
