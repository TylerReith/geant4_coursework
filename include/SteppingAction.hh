#ifndef SteppingAction_h
#define SteppingAction_h

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class SteppingAction : public G4UserSteppingAction {
public:
    SteppingAction();
    virtual ~SteppingAction() = default;

    virtual void UserSteppingAction(const G4Step*) override;

private:
    G4int fPhotonCount;
};

#endif
