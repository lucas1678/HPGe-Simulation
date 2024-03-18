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

	G4double energyDep = aStep->GetTotalEnergyDeposit();

	G4double particleTotalEnergy = preStepPoint->GetTotalEnergy();

	return true;
}
