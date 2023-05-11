#include "detector.hh"

//I DONT THINK WE NEED THIS CLASS WHEN WE USE A SCORING VOLUME WITH STEPPING CLASS

MySensitiveDetector::MySensitiveDetector(G4String name) : G4VSensitiveDetector(name)
{}

MySensitiveDetector::~MySensitiveDetector()
{}

G4bool MySensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist)
{
	G4Track *track = aStep->GetTrack();
	//track->SetTrackStatus(fStopAndKill); //kills particle as soon as they are registered by detector
	G4StepPoint *preStepPoint = aStep->GetPreStepPoint();
	G4StepPoint *postStepPoint = aStep->GetPostStepPoint();

	G4ThreeVector momParticle = preStepPoint->GetMomentum();
	//G4cout << "Particle Momentum: " << momParticle << G4endl;

	G4double energyDep = aStep->GetTotalEnergyDeposit();
	//G4cout << "Particle EDEP: " << energyDep << G4endl;

	G4double particleTotalEnergy = preStepPoint->GetTotalEnergy();
	//G4cout << "Particle Total Energy: " << particleTotalEnergy << G4endl;

	//totalE += particleTotalEnergy;
	//G4cout << "TOTAL E IS AT: " << totalE << G4endl;

	return true;
}
