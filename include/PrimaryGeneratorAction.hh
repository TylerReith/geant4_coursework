#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"

#include "G4SystemOfUnits.hh"
#include "G4Proton.hh"

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();
    ~PrimaryGeneratorAction();
    virtual void GeneratePrimaries(G4Event *anEvent) override;

  private:
    G4ParticleGun *fParticleGun;
};
