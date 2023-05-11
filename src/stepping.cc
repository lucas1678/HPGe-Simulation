#include "stepping.hh"

MySteppingAction::MySteppingAction(MyEventAction *eventAction)
{


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

	if(volume != fScoringVolume)
	{
		return; //close function
	}
	
	G4ThreeVector momPhoton = step->GetPreStepPoint()->GetMomentum();
	G4double wavelength = (1.239841939*eV/momPhoton.mag())*1E+03; //wavelenght in nm
	G4int evt = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
	//G4cout << "Global time: " << 0.000000001*(step->GetPreStepPoint()->GetGlobalTime()) << "and eventID: " << evt << G4endl;

	if(step->GetTrack() != 0)
	{
		G4double preEdep = step->GetTotalEnergyDeposit();
		//G4cout << "step energy dep: " << preEdep*1000 << G4endl;
		fEventAction->AddEdep(preEdep);
	}



}

