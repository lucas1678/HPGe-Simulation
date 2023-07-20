#include "run.hh"

MyRunAction::MyRunAction()
{
	auto analysisManager = G4AnalysisManager::Instance();
	analysisManager->SetVerboseLevel(1);
	
	analysisManager->CreateNtuple("Data", "Data");
	analysisManager->CreateNtupleDColumn("fEdep");
	analysisManager->CreateNtupleSColumn("runTime"); //have to save as string to avoid
	analysisManager->FinishNtuple(0);			//scientific notation!
	
	

}

MyRunAction::~MyRunAction()
{}

void MyRunAction::BeginOfRunAction(const G4Run* run)
{
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
