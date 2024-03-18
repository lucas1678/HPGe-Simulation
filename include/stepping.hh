#ifndef STEPPING_HH
#define STEPPING_HH

#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4PhysicsOrderedFreeVector.hh"
#include "G4AnalysisManager.hh"
#include "G4Electron.hh"

#include "construction.hh"
#include "event.hh"

class MySteppingAction : public G4UserSteppingAction
{
public:
	MySteppingAction(MyEventAction *eventAction);
	~MySteppingAction();

	virtual void UserSteppingAction(const G4Step*);

private:
	G4GenericMessenger *fMessenger;

	MyEventAction *fEventAction;
	G4PhysicsOrderedFreeVector *quantumEffVec;
	G4double fEdepo = 0.;
	G4double prevParentID = 0.;
	G4double prevTrackID = 0.;
	G4double prevGtime = 0.;
	G4double stopTime;
	//G4String particleName;
	G4double parentID;
};

#endif
