#include "SensitiveDetector.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"
#include "RunAction.hh"
#include <cmath>

SensitiveDetector::SensitiveDetector(const G4String& name)
    : G4VSensitiveDetector(name), fPhotoelectrons(0) {}

SensitiveDetector::~SensitiveDetector() {}

void SensitiveDetector::Initialize(G4HCofThisEvent* /*hce*/) {
    fPhotoelectrons = 0;
}

G4bool SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory* /*history*/) {
    G4Track* track = step->GetTrack();

    // Дебаг: информация о треке
    std::cout << GetName() <<" [SD] Track ID: " << track->GetTrackID()
              << ", Particle: " << track->GetDefinition()->GetParticleName()
              << ", Energy: " << track->GetTotalEnergy()/eV << " eV"
              << ", Position: " << track->GetPosition() << G4endl;

    // Проверяем, что это оптический фотон
    if(track->GetDefinition()->GetParticleType() != "opticalphoton") {
        return false;
    }

    // Преобразуем энергию в длину волны (нм)
    G4double wavelength_nm = 1239.84193 / (track->GetTotalEnergy() / eV); // энергия в eV -> длина волны в nm


    std::cout << "[SD] Optical photon wavelength: " << wavelength_nm << " nm" << G4endl;

    // Квантовая эффективность (QE)
    G4double qe = GetQE(wavelength_nm);
    std::cout << "[SD] QE = " << qe << G4endl;

    // Случайное срабатывание фотокатода
    if(G4UniformRand() < qe) {
        fPhotoelectrons++;
        std::cout << GetName()<< " [SD] Photoelectron generated! Total so far: " << fPhotoelectrons << G4endl;
    }

    // Останавливаем фотон
    track->SetTrackStatus(fStopAndKill);

    return true;
}


void SensitiveDetector::EndOfEvent(G4HCofThisEvent* /*hce*/) {
    RunAction* runAction = (RunAction*)G4RunManager::GetRunManager()->GetUserRunAction();
    std::cout << "[SD] EndOfEvent: " << GetName() << " total p.e.: " << fPhotoelectrons << std::endl;

    if(runAction) {
        runAction->SavePhotoelectrons(fPhotoelectrons, GetName());
    }
}

G4double SensitiveDetector::GetQE(G4double wavelength_nm) const {
    if(wavelength_nm < 350.0 || wavelength_nm > 600.0) return 0.0;
    if(wavelength_nm <= 420.0) return 0.28 * (wavelength_nm-350.0)/(420.0-350.0);
    else return 0.28 * (600.0-wavelength_nm)/(600.0-420.0);
}
