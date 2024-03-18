#ifndef EVENT_HH
#define EVENT_HH

#include "G4UserEventAction.hh"
#include "G4Event.hh"
#include "G4AnalysisManager.hh"
#include <math.h>

#include "run.hh"
//#include "stackingaction.hh"
//#include "detector.hh"

class MyEventAction : public G4UserEventAction
{
public:
	MyEventAction(MyRunAction*);
	~MyEventAction();

	virtual void BeginOfEventAction(const G4Event*);
	virtual void EndOfEventAction(const G4Event*);
	
	void AddEdep(G4double edep) { fEdep += edep; }
	void AddTotalE(G4double totalE) { fTotalE += totalE; }
	void Get_fTime(G4double time){fTime = time;}

private:
	G4double fEdep;
	G4double fTotalE;	
	G4double fTime;

	
	G4double ResolutionFunction(G4double energy);
	G4double QuantumEffFunction(G4double energy);
};


#endif
