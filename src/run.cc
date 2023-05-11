#include "run.hh"

MyRunAction::MyRunAction()
{
	auto analysisManager = G4AnalysisManager::Instance();
	analysisManager->SetVerboseLevel(1);
	
	analysisManager->CreateNtuple("Energy Deposition", "Energy Deposition");
	analysisManager->CreateNtupleDColumn("fEdep");
	analysisManager->FinishNtuple(0);
	
	

}

MyRunAction::~MyRunAction()
{}

void MyRunAction::BeginOfRunAction(const G4Run* run)
{
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

	auto analysisManager = G4AnalysisManager::Instance();
	analysisManager->Write();
	analysisManager->CloseFile();

}
