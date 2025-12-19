#include "ActionInitialization.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"
ActionInitialization::ActionInitialization()
{}

ActionInitialization::~ActionInitialization()
{}

void ActionInitialization::Build() const
{
  SetUserAction(new PrimaryGeneratorAction());
  SetUserAction(new RunAction());
  SetUserAction(new SteppingAction());
}
