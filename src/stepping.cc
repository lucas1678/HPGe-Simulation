#include "stepping.hh"

MySteppingAction::MySteppingAction(MyEventAction *eventAction)
{
	fMessenger = new G4GenericMessenger(this, "/stepping/", "Stepping Action Controls");
	fMessenger->DeclareProperty("StopTime", stopTime, "How long (min) to let the simulation run for.");
	stopTime = 100;


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


	//if(step->GetTrack()->GetTrackID() < 2010) {G4cout << step->GetTrack()->GetParticleDefinition()->GetParticleName() << G4endl;}

	//if((volume != fScoringVolume) || (gtime > stopTime))
	if((volume != fScoringVolume))
	{
		return; //close function, everything below here only runs if step is on detector
	}
	
	G4double parentID = step->GetTrack()->GetParentID();
	G4double trackID = step->GetTrack()->GetTrackID();
	G4String particleName = step->GetTrack()->GetParticleDefinition()->GetParticleName();
	

	/*if(parentID == prevParentID || prevParentID == 0 || parentID == prevTrackID || (gtime - prevGtime) < 5*ns)
	{
		fEdepo += step->GetTotalEnergyDeposit();
		prevParentID = parentID;
		prevTrackID = trackID;
		prevGtime = gtime;
	}
	else
	{
		prevParentID = parentID;
		prevTrackID = trackID;
		prevGtime = gtime;

		fEdepo = (fEdepo / keV);
		if(fEdepo > 10)
		{
			auto analysisManager = G4AnalysisManager::Instance();
			analysisManager->FillNtupleDColumn(0,0,fEdepo);
			analysisManager->AddNtupleRow(0);
		}

		fEdepo = step->GetTotalEnergyDeposit(); //maybe add before analysismanager

	}*/

	//G4cout << "A " << particleName << " with parentID: " << parentID << " deposited: " << step->GetTotalEnergyDeposit() /keV << " with total eDep: " << fEdepo / keV << " gtime " << gtime << "\n" << G4endl;
	//if((step->GetTotalEnergyDeposit() /keV) > 154 &&  (step->GetTotalEnergyDeposit() /keV) < 156) {G4cout << "YOOOOOOOOO!!!!!!!!!!" << G4endl;}


	if(step->GetTrack() != 0) //why did I add this? Hmm
	{
		G4double preEdep = step->GetTotalEnergyDeposit();
		fEventAction->AddEdep(preEdep);
		//G4cout << "Hit with energy dep: " << preEdep / keV << " from " << particleName << " with track ID " << trackID << " and parent ID " << parentID << " and position " << particlePosition << G4endl;
		//if((preEdep / keV) > 154 &&  (preEdep / keV) < 156) {G4cout << "YOOOOOOOOO!!!!!!!!!!" << G4endl;}

	}
	
	if(step->GetTrack()->GetTrackStatus() == fStopAndKill)
	{
		fEventAction->Get_fTime(gtime);
	}



}

