#ifndef RUNACTION_HH
#define RUNACTION_HH

#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include "G4String.hh"
#include <fstream>
#include <map>

class RunAction : public G4UserRunAction {
public:
    RunAction();
    virtual ~RunAction();

    virtual void EndOfRunAction(const G4Run* run) override;

    // Сохраняем количество фотоэлектронов за событие для данного SD
    void SavePhotoelectrons(G4int nPE, const G4String& sdName);

private:
    std::ofstream fOutFile;
    std::map<G4String, G4int> fTotalPE;  // суммарные p.e. по каждому SD
};

#endif
