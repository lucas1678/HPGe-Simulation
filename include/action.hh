#ifndef ACTION_HH
#define ACTION_HH

#include "G4VUserActionInitialization.hh"

#include "generator.hh"
#include "stepping.hh"
#include "run.hh"
//#include "stackingaction.hh"
#include "event.hh"


class MyActionInitialization : public G4VUserActionInitialization
{
public:
	MyActionInitialization();
	~MyActionInitialization();

	virtual void Build() const;
};


#endif
