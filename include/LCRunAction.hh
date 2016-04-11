/* $Id$
 * LCRunAction.hh
 */

#ifndef LCRUNACTION_HH_
#define LCRUNACTION_HH_

#include "LCRootOut.hh"
#include "Setup.hh"

#include "G4UserRunAction.hh"
#include "globals.hh"
#include <iostream>
#include <fstream>



class G4Run;

class LCRunAction : public G4UserRunAction
{
public:
  LCRunAction( LCRootOut *RO);
  LCRunAction();
    ~LCRunAction();

public:
  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction  (const G4Run*);
  void Print( G4String flag, const G4Run* );
  void PrintLucasGearXML();

    // Handles writing to a ROOT file
    LCRootOut *RootOut;
private:
  std::ofstream *fGearFile;
};

#endif /* LCRUNACTION_HH_ */
