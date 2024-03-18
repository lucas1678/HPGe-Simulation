#include <iostream>

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "QGSP_BERT.hh"

#include "construction.hh"
#include "physics.hh"
#include "action.hh"

int main(int argc, char** argv)
{
	G4RunManager *runManager = new G4RunManager();
	runManager->SetUserInitialization(new MyDetectorConstruction());
	runManager->SetUserInitialization(new MyPhysicsList());
	runManager->SetUserInitialization(new MyActionInitialization());
	
	//G4VModularPhysicsList* physics = new QGSP_BERT();
	//physics->RegisterPhysics(new G4EmStandardPhysics());
	//physics->RegisterPhysics(new G4DecayPhysics());
	//physics->RegisterPhysics(new G4OpticalPhysics());
	//runManager->SetUserInitialization(physics);
	

	runManager->Initialize();

	G4VisManager *visManager = new G4VisExecutive();
	visManager->Initialize();

	G4UImanager *UImanager = G4UImanager::GetUIpointer();

	/*G4UIExecutive *ui = 0;
	if(argc == 1) //if you just run ./sim
	{
		ui = new G4UIExecutive(argc, argv);
		UImanager->ApplyCommand("/control/execute macros/geometry.mac");
		UImanager->ApplyCommand("/control/execute macros/vis.mac");
		ui->SessionStart();
	}
	else  //if you run ./sim <fileName>, open geant4 and /control/execute fileName
	{
		G4String command = "/control/execute ";
		G4String fileName = argv[1];
		UImanager->ApplyCommand("/control/execute macros/geometry.mac");
		UImanager->ApplyCommand(command+fileName);
	}*/

	G4UIExecutive *ui = 0;
	G4cout << "test message before loop";
	if(argc == 1) //if you just run ./sim
	{
		G4String command = "/control/execute ";
		G4cout << "Which isotope file? ";
		G4String fileName;
		G4cin >> fileName;
		UImanager->ApplyCommand("/control execute macros/geometry.mac");
		UImanager->ApplyCommand(command+fileName);
	}
	else   
	{
		if(argc == 2) //if you run ./sim and one argument (now a bit useless)
		{
			G4String command = "/control/execute ";
			G4String fileName = argv[1]; 
			UImanager->ApplyCommand("/control/execute macros/geometry.mac");
			UImanager->ApplyCommand(command+fileName);
		}

		else //if you run ./sim and more than one argument, launches GUI
		{
			ui = new G4UIExecutive(argc, argv);
			UImanager->ApplyCommand("/control/execute macros/geometry.mac");
			UImanager->ApplyCommand("/control/execute macros/vis.mac");
			ui->SessionStart();

		}

	}
	return 0;

}
