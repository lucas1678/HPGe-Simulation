#include "run.hh"

MyRunAction::MyRunAction()
{
	auto analysisManager = G4AnalysisManager::Instance();
	analysisManager->SetVerboseLevel(1);
	
	analysisManager->CreateNtuple("Data", "Data");
	analysisManager->CreateNtupleDColumn("fEdep");
	analysisManager->CreateNtupleDColumn("runTime"); //new
	analysisManager->FinishNtuple(0);
	
	

}

MyRunAction::~MyRunAction()
{}

void MyRunAction::BeginOfRunAction(const G4Run* run)
{
	//gtimer = new G4Timer();
	//gtimer->Start(); 


	//G4String testString = "hello";
	//auto hum = testString.find();
	//G4cout << testString.index();
	G4int runID = run->GetRunID();
	std::stringstream strRunID;
	strRunID << runID;

	auto analysisManager = G4AnalysisManager::Instance();
	analysisManager->SetVerboseLevel(1);

	analysisManager->OpenFile("outputData"+strRunID.str()+".csv");



}

void MyRunAction::EndOfRunAction(const G4Run* run)
{
	/*gtimer->Stop();
	G4double elapsedTime = gtimer->GetRealElapsed();
	G4double sTime = gtimer->GetSystemElapsed(); 
	G4double uTime = gtimer->GetUserElapsed();  	
	G4cout << "REAL ELAPSED TIME (min) IS: " << (elapsedTime / s)/60 << G4endl;
	G4cout << "REAL SYSTEM TIME (min) IS: " << (sTime / s)/60 << G4endl;
	G4cout << "REAL USER TIME (min) IS: " << (uTime / s)/60 << G4endl;*/

	auto analysisManager = G4AnalysisManager::Instance();
	analysisManager->Write();
	analysisManager->CloseFile();


}
