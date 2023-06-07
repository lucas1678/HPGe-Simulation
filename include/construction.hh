#ifndef CONSTRUCTION_HH
#define CONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4Trap.hh"
#include "G4GenericMessenger.hh"
#include "G4Sphere.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4VSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4RotationMatrix.hh"

//#include "G4CMP.hh"

#include "detector.hh"



class MyDetectorConstruction : public G4VUserDetectorConstruction
{
public:
	MyDetectorConstruction();
	~MyDetectorConstruction();

	G4LogicalVolume *GetScoringVolume() const {return fScoringVolume;}

	virtual G4VPhysicalVolume *Construct();

private:
	G4GenericMessenger *fMessenger;
	G4double detLength, detRadius, detOffset;
	G4String worldMatChoice, windowMatChoice;
	G4bool spawnBackWall, spawnWindow;
	G4LogicalVolume *fScoringVolume;
	
	G4Box *solidAtmosphere, *solidWorld, *solidXeBox;
	G4LogicalVolume *logicDet, *logicAtmosphere[10], *logicWorld, *logicShield1, *logicShield2, *logicShield3, *logicXeBox, *logicWindow, *logicSubtract, *logicIntersect, *logicProbe, *logicArmOut, *logicArmIn, *logicOuterMount, *logicTip;
	G4VPhysicalVolume *physAtmosphere[10], *physWorld, *physDet, *physShield, *physShield2, *physShield3, *physXeBox, *physWindow, *physSubtract, *physIntersect,*physProbe, *physArmOut, *physArmIn, *physOuterMount, *physTip;
	G4Material *Air[10];
	G4Tubs *solidDet, *solidShield1, *solidShield2, *solidShield3, *solidWindow, *solidArmOut, *solidArmIn, *solidProbe, *solidOuterMount, *solidTip;
	G4VSolid *solidIntersect;

	G4Material *intersectMat;
	G4Element *Mn, *Si, *Ni, *Fe, *Cr;

	virtual void ConstructSDandField();

};


#endif
