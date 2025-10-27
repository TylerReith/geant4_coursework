#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"

#include "FTFP_BERT.hh"
#include "G4OpticalPhysics.hh"
#include "G4OpticalParameters.hh"

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

int main(int argc, char *argv[])
{
    // --- Инициализация ядра Geant4 ---
    G4RunManager *runManager = new G4RunManager();

    // --- Геометрия ---
    runManager->SetUserInitialization(new DetectorConstruction());

    // --- Физика ---
    auto physicsList = new FTFP_BERT;

    // Добавляем оптическую физику
    auto opticalPhysics = new G4OpticalPhysics();
    opticalPhysics->SetVerboseLevel(1);
    physicsList->RegisterPhysics(opticalPhysics);

    // Настройка глобальных оптических параметров (без SetScintYieldFactor)
    auto opticalParams = G4OpticalParameters::Instance();
    opticalParams->SetVerboseLevel(1);
    opticalParams->SetScintTrackSecondariesFirst(true);
    opticalParams->SetCerenkovTrackSecondariesFirst(true);
    opticalParams->SetCerenkovMaxPhotonsPerStep(100);
    opticalParams->SetCerenkovMaxBetaChange(10.0);

    runManager->SetUserInitialization(physicsList);

    // --- Действия ---
    runManager->SetUserInitialization(new ActionInitialization());

    // --- Инициализация ядра ---
    runManager->Initialize();

    // --- Визуализация и интерфейс ---
    G4VisManager *visManager = new G4VisExecutive();
    visManager->Initialize();

    G4UImanager *UImanager = G4UImanager::GetUIpointer();
    G4UIExecutive *UI = new G4UIExecutive(argc, argv);

    UImanager->ApplyCommand("/control/execute vis.mac");
    UI->SessionStart();

    delete UI;
    delete visManager;
    delete runManager;

    return 0;
}
