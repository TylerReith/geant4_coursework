#include "DetectorConstruction.hh"
#include "SensitiveDetector.hh"
#include "G4SDManager.hh"
#include "G4NistManager.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "G4OpticalSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4MaterialPropertiesTable.hh"


DetectorConstruction::DetectorConstruction() {}
DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    G4NistManager* nist = G4NistManager::Instance();

    //--------------------------------------- Materials --------------------------------------------------
    // Элементы
    G4Element* H  = nist->FindOrBuildElement("H");
    G4Element* C  = nist->FindOrBuildElement("C");
    G4Element* N  = nist->FindOrBuildElement("N");
    G4Element* O  = nist->FindOrBuildElement("O");
    G4Element* Gd = nist->FindOrBuildElement("Gd");
    G4Element* Fe = nist->FindOrBuildElement("Fe");
    G4Element* Cr = nist->FindOrBuildElement("Cr");
    G4Element* Ni = nist->FindOrBuildElement("Ni");

    // LAB (linear alkylbenzene) для баковой жидкости
    G4Material* LAB_plain = new G4Material("LAB_plain", 0.86*g/cm3, 2);
    LAB_plain->AddElement(C, 18);
    LAB_plain->AddElement(H, 30);

    // Жидкий сцинтиллятор: LAB + PPO + Bis-MSB + Gd
    G4Material* Scintillator = new G4Material("LAB_Scintillator", 0.861*g/cm3, 4);
    Scintillator->AddMaterial(LAB_plain, 99.0*perCent);
    Scintillator->AddElement(Gd, 0.1*perCent);
    Scintillator->AddElement(N, 0.45*perCent);   // усреднённо PPO/Bis-MSB
    Scintillator->AddElement(O, 0.45*perCent);

    // PMMA (сосуд сферы)
    G4Material* PMMA = new G4Material("PMMA", 1.19*g/cm3, 3);
    PMMA->AddElement(C, 5);
    PMMA->AddElement(H, 8);
    PMMA->AddElement(O, 2);

    // Нержавеющая сталь (бак)
    G4Material* StainlessSteel = new G4Material("StainlessSteel", 8.0*g/cm3, 3);
    StainlessSteel->AddElement(Fe, 70*perCent);
    StainlessSteel->AddElement(Cr, 20*perCent);
    StainlessSteel->AddElement(Ni, 10*perCent);

    // --- Оптические свойства (wavelengths in eV or nm) ---
    // Use wavelength array (nm) and convert to energy if you prefer; Geant4 expects keys vs photon energy (eV).
    const int nEntries = 3;
    G4double photonEnergies[nEntries] = {2.00*eV, 2.75*eV, 3.55*eV}; // ~620nm, ~450nm, ~350nm

    // refractive index for LAB-based scintillator ~1.49 (approx)
    G4double rindex_scint[nEntries] = {1.49, 1.49, 1.49};
    G4double abslen_scint[nEntries] = {6.0*m, 6.0*m, 6.0*m}; // attenuation length = 6 m

    G4MaterialPropertiesTable* mptScint = new G4MaterialPropertiesTable();
    mptScint->AddProperty("RINDEX", photonEnergies, rindex_scint, nEntries);
    mptScint->AddProperty("ABSLENGTH", photonEnergies, abslen_scint, nEntries);

    // scintillation emission spectrum (approx peak ~420 nm -> 2.95 eV)
    G4double scintEmission[nEntries] = {0.1, 0.8, 0.1}; // normalized shape
    mptScint->AddProperty("SCINTILLATIONCOMPONENT1", photonEnergies, scintEmission, nEntries);
    mptScint->AddConstProperty("SCINTILLATIONYIELD", 8000./MeV); // photons per MeV
    mptScint->AddConstProperty("RESOLUTIONSCALE", 1.0);
    mptScint->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 6.0*ns); // typical fast component
    mptScint->AddConstProperty("SCINTILLATIONYIELD1", 1.0);

    // attach to scintillator material
    Scintillator->SetMaterialPropertiesTable(mptScint);

    // --- LAB (non-scint) properties ---
    G4double rindex_lab[nEntries] = {1.49, 1.49, 1.49};
    G4double abslen_lab[nEntries] = {12.0*m, 12.0*m, 12.0*m};
    G4MaterialPropertiesTable* mptLAB = new G4MaterialPropertiesTable();
    mptLAB->AddProperty("RINDEX", photonEnergies, rindex_lab, nEntries);
    mptLAB->AddProperty("ABSLENGTH", photonEnergies, abslen_lab, nEntries);
    LAB_plain->SetMaterialPropertiesTable(mptLAB);

    // --- PMMA shell optical properties (transparent) ---
    G4double rindex_pmma[nEntries] = {1.49, 1.49, 1.49};
    G4double abslen_pmma[nEntries] = {10.0*m, 10.0*m, 10.0*m}; // relatively transparent
    G4MaterialPropertiesTable* mptPMMA = new G4MaterialPropertiesTable();
    mptPMMA->AddProperty("RINDEX", photonEnergies, rindex_pmma, nEntries);
    mptPMMA->AddProperty("ABSLENGTH", photonEnergies, abslen_pmma, nEntries);
    PMMA->SetMaterialPropertiesTable(mptPMMA);


    
    //--------------------------------------- World --------------------------------------------------
    G4double world_sizeXY = 2.0*m;
    G4double world_sizeZ  = 2.0*m;
    G4Box* world_solid = new G4Box("world_solid", world_sizeXY/2, world_sizeXY/2, world_sizeZ/2);
    G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");
    G4LogicalVolume* world_log = new G4LogicalVolume(world_solid, world_mat, "world_log");
    G4VPhysicalVolume* world_phys =
        new G4PVPlacement(0, G4ThreeVector(), world_log, "world_phys", 0, false, 0);

    //--------------------------------------- Stainless Steel Tank --------------------------------------------------
    G4double tank_radius = 929.0*mm; // из ТЗ: внутренний радиус бака
    G4double tank_height = 1858.0*mm;
    G4double tank_thickness = 2.0*mm;

    G4Tubs* tank_solid = new G4Tubs("tank_solid", 0, tank_radius, tank_height/2, 0, 360*deg);
    G4LogicalVolume* tank_log = new G4LogicalVolume(tank_solid, StainlessSteel, "tank_log");


    G4VisAttributes* tankVis = new G4VisAttributes(G4Colour(0.7,0.7,0.7));
    tankVis->SetVisibility(true);
    tank_log->SetVisAttributes(tankVis);

    new G4PVPlacement(0, G4ThreeVector(), tank_log, "tank_phys", world_log, false, 0);

    //--------------------------------------- LAB (non-scintillator) --------------------------------------------------
    G4double lab_radius = tank_radius - tank_thickness; // внутренний радиус бака
    G4Tubs* lab_solid = new G4Tubs("lab_solid", 0, lab_radius, tank_height/2 - tank_thickness, 0, 360*deg);
    G4LogicalVolume* lab_log = new G4LogicalVolume(lab_solid, LAB_plain, "lab_log");


    G4VisAttributes* labVis = new G4VisAttributes(G4Colour(0.9,0.9,0.6,0.2));
    labVis->SetVisibility(true);
    lab_log->SetVisAttributes(labVis);

    new G4PVPlacement(0, G4ThreeVector(), lab_log, "lab_phys", tank_log, false, 0);

    //--------------------------------------- Scintillator Sphere (PMMA vessel + LAB+Gd) --------------------------------------------------
    G4double vessel_radius = 620.35*mm; // ≈ radius for 1 m^3

    G4double vessel_thickness = 10.0*mm;

    // PMMA shell
    G4Sphere* vessel_shell = new G4Sphere("vessel_shell",
                                      vessel_radius,            // inner radius
                                      vessel_radius + vessel_thickness, // outer radius
                                      0, 360*deg, 0, 180*deg);
    G4LogicalVolume* vessel_log = new G4LogicalVolume(vessel_shell, PMMA, "vessel_log");

    G4VisAttributes* vesselVis = new G4VisAttributes(G4Colour(0.0,0.0,1.0,0.2));
    vesselVis->SetVisibility(true);
    vessel_log->SetVisAttributes(vesselVis);

    new G4PVPlacement(0, G4ThreeVector(), vessel_log, "vessel_phys", lab_log, false, 0);

    // Scintillator volume (внутри PMMA)
    G4Sphere* scint_solid = new G4Sphere("scint_solid", 0, vessel_radius, 0, 360*deg, 0, 180*deg);
    G4LogicalVolume* scint_log = new G4LogicalVolume(scint_solid, Scintillator, "scint_log");

    G4VisAttributes* scintVis = new G4VisAttributes(G4Colour(1.0,1.0,0.0,0.3));
    scintVis->SetVisibility(true);
    scint_log->SetVisAttributes(scintVis);

    auto scintSD = new SensitiveDetector("ScintillatorSD");
    G4SDManager::GetSDMpointer()->AddNewDetector(scintSD);
    scint_log->SetSensitiveDetector(scintSD);

    new G4PVPlacement(0, G4ThreeVector(), scint_log, "scint_phys", vessel_log, false, 0);

    struct PMTCoord {
    G4ThreeVector pos;
    G4ThreeVector normal; // направлен в центр
};

std::vector<PMTCoord> pmt_positions;

// Чтение CSV: x_mm,y_mm,z_mm,nx,ny,nz
std::ifstream infile("geodesic_freq4_coords.csv");
std::string line;
bool header_skipped = false;
while (std::getline(infile, line)) {
    if (!header_skipped) { header_skipped = true; continue; }
    std::istringstream ss(line);
    std::string token;
    double vals[6];
    int i=0;
    while (std::getline(ss, token, ',') && i<6) vals[i++] = std::stod(token);

    PMTCoord coord;
    coord.pos = G4ThreeVector(vals[0]*mm, vals[1]*mm, vals[2]*mm);
    coord.normal = G4ThreeVector(vals[3], vals[4], vals[5]);
    pmt_positions.push_back(coord);
}

// Тонкая оптическая оболочка фотокатода
G4double pmt_radius = 95.0*mm;
G4double photocathode_thickness = 1.0*mm;

G4Sphere* photocathode_solid = new G4Sphere("Photocathode",
                                            pmt_radius - photocathode_thickness,
                                            pmt_radius,
                                            0, 360*deg, 0, 180*deg);

G4LogicalVolume* photocathode_log =
    new G4LogicalVolume(photocathode_solid,
                        nist->FindOrBuildMaterial("G4_AIR"),
                        "photocathode_log");

// SD для генерации п.э.
auto pmtSD = new SensitiveDetector("PMT_SD");
G4SDManager::GetSDMpointer()->AddNewDetector(pmtSD);
photocathode_log->SetSensitiveDetector(pmtSD);

// Оптическая поверхность с QE
G4OpticalSurface* pmtSurface = new G4OpticalSurface("PMT_Surface");
pmtSurface->SetType(dielectric_dielectric);
pmtSurface->SetFinish(polished);
pmtSurface->SetModel(glisur);

G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();
G4double photonE[2] = {2.0*eV, 3.5*eV};
G4double qe[2]      = {0.28, 0.28}; // 28% QE
mpt->AddProperty("EFFICIENCY", photonE, qe, 2);
pmtSurface->SetMaterialPropertiesTable(mpt);

new G4LogicalSkinSurface("PMT_Surface", photocathode_log, pmtSurface);

// Размещение всех ФЭУ
for (size_t i=0; i<pmt_positions.size(); ++i) {
    G4ThreeVector localZ(0,0,1);
    G4ThreeVector targetDir = -pmt_positions[i].normal;
    targetDir = targetDir.unit();

    G4RotationMatrix* rot = new G4RotationMatrix();
    if (localZ.cross(targetDir).mag() > 1e-6) {
        G4double angle = std::acos(localZ.dot(targetDir));
        G4ThreeVector axis = localZ.cross(targetDir).unit();
        rot->rotate(angle, axis);
    }

    new G4PVPlacement(rot,
                      pmt_positions[i].pos,
                      photocathode_log,
                      "PMT_phys_" + std::to_string(i),
                      lab_log,
                      true,
                      i);
}



    //--------------------------------------- Return -------------------------------------------------
    return world_phys;
}
