#include "RunAction.hh"
#include "G4Run.hh"
#include "G4SystemOfUnits.hh"
#include <iostream>

RunAction::RunAction() {
    fOutFile.open("PE_output.csv");
    fOutFile << "SDName,TotalPhotoelectrons\n"; // заголовок CSV
}

RunAction::~RunAction() {
    if (!fOutFile.is_open())
    G4cerr << "Error: Cannot open PE_output.csv for writing!" << G4endl;

}

void RunAction::SavePhotoelectrons(G4int nPE, const G4String& sdName) {
    fTotalPE[sdName] += nPE;
}

void RunAction::EndOfRunAction(const G4Run* /*run*/) {
    for(auto const& pair : fTotalPE) {
        fOutFile << pair.first << "," << pair.second << "\n";
        std::cout << "[RunAction] " << pair.first << " total p.e.: " << pair.second << G4endl;
    }
    fTotalPE.clear(); // обнуляем после записи
}
