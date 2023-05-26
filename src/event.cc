#include "event.hh"
#include "G4RunManager.hh" //need for G4RandGauss

MyEventAction::MyEventAction(MyRunAction*)
{
	fEdep = .0;
	fTotalE = .0;
}

MyEventAction::~MyEventAction()
{}

void MyEventAction::BeginOfEventAction(const G4Event*)
{
	fEdep = .0;
	fTotalE = .0;
}

void MyEventAction::EndOfEventAction(const G4Event*)
{
	auto analysisManager = G4AnalysisManager::Instance();

	fEdep = fEdep*1000;
	//if(fEdep!=0){G4cout << "fEdep is: " << fEdep << G4endl;}
	
	G4double quantumChance = QuantumEffFunction(fEdep);
	if(fEdep > 1 && G4UniformRand() <= quantumChance)
	{
		fEdep = G4RandGauss::shoot(fEdep, ResolutionFunction(fEdep));

		analysisManager->FillNtupleDColumn(0,0,fEdep);
		analysisManager->FillNtupleDColumn(0,1,fTime);
		analysisManager->AddNtupleRow(0);
	}
}

G4double MyEventAction::ResolutionFunction(G4double energy)
{
	G4double a = 0.00122673; //numbers come from my Excel analysis/fit
	G4double c = 0.143327;
	//G4double c = 0.543327;

	return a*energy + c;
}

G4double MyEventAction::QuantumEffFunction(G4double energy)
{
	G4double p = 28967.7; //CHANGE TO MATCH REAL EFF. parameters also found from excel document fit
	G4double k = 0.00000059526;
	G4double d = 11.0431;
	G4double b = -0.0127744;
	G4double c = 0.00000651426;
	return p*(sin(k*(energy-d))*exp(b*energy)+c); // - Shout out to Alexia 2022
} 
