#include "SteppingAction.hh"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4OpticalPhoton.hh"
#include "G4VPhysicalVolume.hh"
#include "G4RunManager.hh"
#include "RunAction.hh"

SteppingAction::SteppingAction()
: G4UserSteppingAction(), fPhotonCount(0) {}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    G4Track* track = step->GetTrack();

    if (track->GetDefinition() != G4OpticalPhoton::Definition())
        return;

    auto post = step->GetPostStepPoint();
    if (post->GetStepStatus() != fGeomBoundary)
        return;

    auto physVol = post->GetPhysicalVolume();
    if (!physVol)
        return;

    const G4String& name = physVol->GetName();
    if (name.find("PMT_phys_") == std::string::npos)
        return;
    std::cout << fPhotonCount  << G4endl;

    // --- фотон достиг фотокатода ---
    fPhotonCount++;

    track->SetTrackStatus(fStopAndKill);
}

