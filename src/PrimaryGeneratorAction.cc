#include "PrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(nullptr)
{
    G4int n_particle = 1;
    fParticleGun = new G4ParticleGun(n_particle);

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle("e-");
    fParticleGun->SetParticleDefinition(particle);

    fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., 0.*cm));

    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));

    fParticleGun->SetParticleEnergy(1.*MeV);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    G4cout << "Генерация электрона с энергией: "
           << fParticleGun->GetParticleEnergy() / MeV << " MeV"
           << G4endl;

    fParticleGun->GeneratePrimaryVertex(anEvent);
}

