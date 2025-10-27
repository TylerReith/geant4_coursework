#ifndef SENSITIVE_DETECTOR_HH
#define SENSITIVE_DETECTOR_HH

#include "G4VSensitiveDetector.hh"
#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

class SensitiveDetector : public G4VSensitiveDetector {
public:
    explicit SensitiveDetector(const G4String& name);
    virtual ~SensitiveDetector();

    void Initialize(G4HCofThisEvent* hce) override;
    G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
    void EndOfEvent(G4HCofThisEvent* hce) override;

private:
    G4int fPhotoelectrons;  // количество фотоэлектронов за событие
    G4double GetQE(G4double wavelength_nm) const;
};

#endif // SENSITIVE_DETECTOR_HH
