#include "G4VUserActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"

class ActionInitialization : public G4VUserActionInitialization
{
  public:
    ActionInitialization();
    ~ActionInitialization();
    virtual void Build() const override;
};
