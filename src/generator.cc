#include "generator.hh"

MyPrimaryGenerator::MyPrimaryGenerator()
{
	//invoked only once

	/*fParticleGun = new G4ParticleGun(1); //one primary vertex per event created
	G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
	G4ParticleDefinition *particle = particleTable->FindParticle("geantino"); //can replace this placeholder particle later with macro files

	fParticleGun->SetParticlePosition(G4ThreeVector(0.,0.,0.));
	fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,2.,1.));
	fParticleGun->SetParticleEnergy(0*keV);
	fParticleGun->SetParticleDefinition(particle);*/

	fParticleGun = new G4GeneralParticleSource(); 

}

MyPrimaryGenerator::~MyPrimaryGenerator()
{
	delete fParticleGun;
}

void MyPrimaryGenerator::GeneratePrimaries(G4Event *anEvent)
{

	//invoked every event
	/*G4ParticleDefinition *particle = fParticleGun->GetParticleDefinition();
	if(particle == G4Geantino::Geantino())
	{
		G4int Z = 55; //number of protons
		G4int A = 137; //mass number

		G4double charge = 0.*eplus; 
		G4double energy = 0.*keV;

		G4ParticleDefinition *ion = G4IonTable::GetIonTable()->GetIon(Z, A, energy);

		fParticleGun->SetParticleDefinition(ion);
		fParticleGun->SetParticleCharge(charge);
		fParticleGun->SetParticleMomentum(G4ThreeVector(0.,0.,0.));
	}


	fParticleGun->GeneratePrimaryVertex(anEvent);*/
	fParticleGun->GeneratePrimaryVertex(anEvent);

}
