/*
 * LCPhysicsList.hh
 * LumiCalJA
 *
 *  Created on: Mar 2, 2009
 *      Author: aguilar
 *
 *      Ripped straight from LCLeptonPhysics.hh from Mokka:
 *      //
 *      ////////////////////////////////////////////////////////////////
 *      //                                                            //
 *      //  Title:  Physics List for a Linear Collider Detector       //
 *      //  Date:   16 June 2004                                      //
 *      //  Author: D.H. Wright (SLAC)                                //
 *      //                                                            //
 *      ////////////////////////////////////////////////////////////////
 *      //
 *
 */


#ifndef LCPHYSICSLIST_HH_
#define LCPHYSICSLIST_HH_

#include "G4VModularPhysicsList.hh"


class LCPhysicsList: public G4VModularPhysicsList
{
public:
  LCPhysicsList();
  virtual ~LCPhysicsList();

  virtual void SetCuts();
};

#endif /* LCPHYSICSLIST_HH_ */
