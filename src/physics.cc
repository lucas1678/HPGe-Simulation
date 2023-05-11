#include "physics.hh"
#include "G4NuclideTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

MyPhysicsList::MyPhysicsList()
{
	RegisterPhysics (new G4EmStandardPhysics());
	//RegisterPhysics (new G4OpticalPhysics());
	RegisterPhysics (new G4DecayPhysics());
	RegisterPhysics (new G4RadioactiveDecayPhysics());
	
	G4NuclideTable::GetInstance()->SetThresholdOfHalfLife(0.1*picosecond); //for short-live isomers
  	G4NuclideTable::GetInstance()->SetLevelTolerance(1.0*eV);
	//RegisterPhysics (new G4LivermoreIonisationModel());
}
MyPhysicsList::~MyPhysicsList()
{}
