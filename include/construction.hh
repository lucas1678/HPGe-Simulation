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
#include "G4VSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4Cons.hh"
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
	G4double detLength, detRadius, detOffset, sourceHeight;
	G4String worldMatChoice, windowMatChoice;
	G4bool spawnBackWall, spawnWindow;
	G4LogicalVolume *fScoringVolume;
	
	G4Box *solidAtmosphere, *solidWorld, *solidTip3;
	G4Sphere *solidXeVolume, *solidIonSphere;
	G4VSolid *solidTip2_1, *solidTip2_2, *solidTip2, *solidTipConnector1, *solidTipConnector2, *solidTipConnector, *solidTipConnector_2, *solidTipConnector3, *solidTipConnector4, *solidChamberSide3_1, *solidChamberSide3_2, *solidChamberSide3;
	
	G4LogicalVolume *logicDet, *logicAtmosphere[10], *logicWorld, *logicShield1, *logicShield2, *logicShield3, *logicXeVolume, *logicWindow, *logicChamber, *logicProbe, *logicTip, *logicTip2, *logicTip3, *logicTipConnector, *logicTipConnector_2, *logicHousing, *logicChamberSide, *logicChamberSide2, *logicChamberSide3, *logicChamberSide4, *logicGear, *logicIonSphere;
	G4VPhysicalVolume *physAtmosphere[10], *physWorld, *physDet, *physShield, *physShield2, *physShield3, *physXeVolume, *physWindow, *physChamber, *physChamber2, *physProbe, *physTip, *physTip2, *physTip3, *physTipConnector, *physTipConnector_2, *physHousing, *physChamberSide, *physChamberSide2, *physChamberSide3, *physChamberSide4, *physGear, *physIonSphere;
	
	G4Material *Galactic[10];
	G4Tubs *solidDet, *solidShield1, *solidShield2, *solidShield3, *solidWindow, *solidProbe, *solidHousing, *solidChamberSide, *solidChamberSide2, *solidChamberSide4, *solidGearBase;
	G4Cons *solidTip;

	virtual void ConstructSDandField();

};


#endif
