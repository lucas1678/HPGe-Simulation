#include "stepping.hh"

MySteppingAction::MySteppingAction(MyEventAction *eventAction)
{
	/*fMessenger = new G4GenericMessenger(this, "/stepping/", "Stepping Action Controls");
	fMessenger->DeclareProperty("StopTime", stopTime, "How long (min) to let the simulation run for.");
	stopTime = 100;*/


	fEventAction = eventAction;

	quantumEffVec = new G4PhysicsOrderedFreeVector();

	std::ifstream datafile;
	datafile.open("res_efficiency.dat");

	while(1)
	{
		G4double wavelength, quantumEff;
		datafile >> wavelength >> quantumEff; //reads in data file with first column wlen, second eff.

		if(datafile.eof())
			break;

		G4cout << wavelength << " " << quantumEff << G4endl;

		quantumEffVec->InsertValues(wavelength, quantumEff/100.);
	}

}

MySteppingAction::~MySteppingAction()
{}

void MySteppingAction::UserSteppingAction(const G4Step *step)
{

	G4LogicalVolume *volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();

	const MyDetectorConstruction *detectorConstruction = static_cast<const MyDetectorConstruction*> (G4RunManager::GetRunManager()->GetUserDetectorConstruction());

	G4LogicalVolume *fScoringVolume = detectorConstruction->GetScoringVolume();

	G4double gtime = ((step->GetPostStepPoint()->GetGlobalTime()) / minute);
	
	//G4double ltime = ((step->GetPostStepPoint()->GetLocalTime()) / s)/60;

	//G4cout << "Global Time is: " << gtime << G4endl;


	//if(step->GetTrack()->GetTrackID() < 2010) {G4cout << step->GetTrack()->GetParticleDefinition()->GetParticleName() << G4endl;}

	//if((volume != fScoringVolume) || (gtime > stopTime))
	
	G4double parentID = step->GetTrack()->GetParentID();
	G4Track* track = (G4Track*)(step->GetTrack());
	G4double trackID = step->GetTrack()->GetTrackID();
	const G4ParticleDefinition *particleName = step->GetTrack()->GetParticleDefinition();//->GetParticleName();


	if(volume != fScoringVolume && particleName == G4Electron::Electron() && parentID > 0)
	{
		track->SetTrackStatus(fStopAndKill);
		return; //close function, everything below here only runs if step is on detector
	}

	if(volume != fScoringVolume && particleName == G4Electron::Electron() && parentID == 2)
	{
		track->SetTrackStatus(fStopAndKill);
		return;
	}

	/*if(volume!=fScoringVolume)
	{
		return;
	}*/

	if(step->GetTrack() != 0) //why did I add this? Hmm
	{
		G4double preEdep = step->GetTotalEnergyDeposit();
		fEventAction->AddEdep(preEdep);

	}

	/*if(step->GetTrack()->GetTrackStatus()==fStopAndKill)
	{
		fEventAction->Get_fTime(gtime);
	}*/

	


}

