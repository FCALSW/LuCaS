/*
 * LCHadronPhysics.hh
 * 2ModLumiCal
 *
 *  Created on: Mar 23, 2009
 *      Author: Jonathan Aguilar
 */
//
/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//  Title:  Hadron physics for a Linear Collider Detector                  //
//  Date:   23 June 2004                                                   //
//  Author: D.H. Wright (SLAC)                                             //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
//

#ifndef LCHADRONPHYSICS_HH_
#define LCHADRONPHYSICS_HH_

#include "G4VPhysicsConstructor.hh"

class LCHadronPhysics : public G4VPhysicsConstructor
{
  public:
    LCHadronPhysics(const G4String& name ="hadron");
    virtual ~LCHadronPhysics();

    // This method will be invoked in the Construct() method.
    // each particle type will be instantiated
    virtual void ConstructParticle();

    // This method will be invoked in the Construct() method.
    // each physics process will be instantiated and
    // registered to the process manager of each particle type
    virtual void ConstructProcess();

};

#endif /* LCHADRONPHYSICS_HH_ */
