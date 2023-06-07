#include "construction.hh"

MyDetectorConstruction::MyDetectorConstruction()
{
	fMessenger = new G4GenericMessenger(this, "/construction/", "Environment Construction");
	

	fMessenger->DeclareProperty("detLength", detLength, "Length of detector arrangement").SetUnit("mm");
	detLength = 80.*mm;

	fMessenger->DeclareProperty("detRadius", detRadius, "Radius of detector arrangement").SetUnit("mm");
	detRadius = 100.*mm;

	fMessenger->DeclareProperty("sourceHeight", sourceHeight, "Height of the radioactive source (z-level)").SetUnit("mm");
	sourceHeight = 0.*mm;

	fMessenger->DeclareProperty("worldMatChoice", worldMatChoice, "Material Between Detector and Source");
	worldMatChoice = "G4_AIR";

	fMessenger->DeclareProperty("spawnBackWall", spawnBackWall, "Spawn Wall Behind Source for Backscattering Effects");
	spawnBackWall = false;

	fMessenger->DeclareProperty("windowMatChoice", windowMatChoice, "Choose window material");
	windowMatChoice = "G4_Be";

	fMessenger->DeclareProperty("spawnWindow", spawnWindow, "Spawn Window Between Detector and Source");
	spawnWindow = false;


}

MyDetectorConstruction::~MyDetectorConstruction()
{}

G4VPhysicalVolume *MyDetectorConstruction::Construct()
{

	G4cout << "Construct() called, and value of detLength is: " << detLength << G4endl;

//~~~~~~~~~~~~~~~~~~~~~~~MATERIALS~~~~~~~~~~~~~~~~~~~~~~~//

	G4NistManager *nist = G4NistManager::Instance();

	G4Material *worldMat = nist->FindOrBuildMaterial(worldMatChoice);
	G4MaterialPropertiesTable *mptWorld = new G4MaterialPropertiesTable();
	G4double rindexWorld[2] = {1.0, 1.0};
	G4double energy[2] = {2.038*eV, 4.144*eV};
	if(worldMatChoice == "G4_AIR")
	{
		energy[0] = 2.038*eV;
		energy[1] = 4.144*eV;
	}
	else if(worldMatChoice == "G4_Xe")
	{
		energy[0] = 2.038*eV;
		energy[1] = 4.144*eV;
	}
	mptWorld->AddProperty("RINDEX", energy, rindexWorld, 2);
	worldMat->SetMaterialPropertiesTable(mptWorld);


	G4Material *detMat = nist->FindOrBuildMaterial("G4_Ge");
	G4MaterialPropertiesTable *mptDet = new G4MaterialPropertiesTable();
	G4double rindexDet[2] = {1.0, 1.0};
	//G4double energy[2] = {2.038*eV, 4.144*eV};
	mptDet->AddProperty("RINDEX", energy, rindexDet, 2);
	detMat->SetMaterialPropertiesTable(mptDet);

	G4Material *shieldMat1 = nist->FindOrBuildMaterial("G4_Al");
	G4MaterialPropertiesTable *mptShield1 = new G4MaterialPropertiesTable();
	G4double rindexShield1[2] = {1.0, 1.0};
	//G4double energy[2] = {2.038*eV, 4.144*eV};
	mptShield1->AddProperty("RINDEX", energy, rindexShield1, 2);
	shieldMat1->SetMaterialPropertiesTable(mptShield1);

	G4Material *shieldMat2 = nist->FindOrBuildMaterial("G4_Cu");
	G4MaterialPropertiesTable *mptShield2 = new G4MaterialPropertiesTable();
	G4double rindexShield2[2] = {1.0, 1.0};
	//G4double energy[2] = {2.038*eV, 4.144*eV};
	mptShield2->AddProperty("RINDEX", energy, rindexShield2, 2);
	shieldMat2->SetMaterialPropertiesTable(mptShield2);

	G4Material *shieldMat3 = nist->FindOrBuildMaterial("G4_Pb");
	G4MaterialPropertiesTable *mptShield3 = new G4MaterialPropertiesTable();
	G4double rindexShield3[2] = {1.0, 1.0};
	//G4double energy[2] = {2.038*eV, 4.144*eV};
	mptShield3->AddProperty("RINDEX", energy, rindexShield3, 2);
	shieldMat3->SetMaterialPropertiesTable(mptShield3);

	G4Material *xeMat = nist->FindOrBuildMaterial("G4_Xe");

	G4Material *windowMat = nullptr;
	if(windowMatChoice == "G4_Be"){windowMat = nist->FindOrBuildMaterial(windowMatChoice);} 
	else if(windowMatChoice == "G4_Al"){windowMat = nist->FindOrBuildMaterial(windowMatChoice);}
	G4MaterialPropertiesTable *mptWindowMat = new G4MaterialPropertiesTable();
	G4double rindexWindow[2] = {1.0, 1.0};
	mptWindowMat->AddProperty("RINDEX", energy, rindexWindow, 2);
	windowMat->SetMaterialPropertiesTable(mptWindowMat);
	

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~WORLD~~~~~~~~~~~~~~~~~~~~~~~~~//

	solidWorld = new G4Box("solidWorld", 0.5*m, 0.5*m, 0.5*m);
	logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
	physWorld = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), logicWorld, "physWorld", 0, false, 0, true);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~DETECTOR~~~~~~~~~~~~~~~~~~~~~~//
	detOffset = 10.*mm;

	solidDet = new G4Tubs("solidDet", 0*mm, detRadius, detLength - detOffset, 0*deg, 360*deg); //inner radius, outer radius, length, starting and stopping angle
	logicDet = new G4LogicalVolume(solidDet, detMat, "logicDet");
	physDet = new G4PVPlacement(0, G4ThreeVector(0.,0.,7.*cm - detOffset + detLength/2), logicDet, "physDet", logicWorld, false, 0, true);

	fScoringVolume = logicDet;
	//G4cout << "DETECTOR DISTANCEEEEEEE" << 3.4*cm - detOffset + detLength/2 << G4endl;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~Al Cylinder~~~~~~~~~~~~~~~~~~~~//

	solidShield1 = new G4Tubs("solidShield1", detRadius, detRadius + 5.*mm, detLength, 0*deg, 360*deg); //inner radius, outer radius, length, starting and stopping angle
	logicShield1 = new G4LogicalVolume(solidShield1, shieldMat1, "logicShield1");
	physShield = new G4PVPlacement(0, G4ThreeVector(0.,0.,7.*cm + detLength/2), logicShield1, "physShield1", logicWorld, false, 0, true);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~Cu Cylinder~~~~~~~~~~~~~~~~~~~~//

	solidShield2 = new G4Tubs("solidShield2", detRadius + 5.*mm, detRadius + 10.*mm, detLength, 0*deg, 360*deg); //inner radius, outer radius, length, starting and stopping angle
	logicShield2 = new G4LogicalVolume(solidShield2, shieldMat2, "logicShield2");
	physShield2 = new G4PVPlacement(0, G4ThreeVector(0.,0.,7.*cm + detLength/2), logicShield2, "physShield2", logicWorld, false, 0, true);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~Pb Cylinder~~~~~~~~~~~~~~~~~~~~//

	solidShield3 = new G4Tubs("solidShield3", detRadius + 10.*mm, detRadius + 15.*mm, detLength, 0*deg, 360*deg); //inner radius, outer radius, length, starting and stopping angle
	logicShield3 = new G4LogicalVolume(solidShield3, shieldMat3, "logicShield3");
	physShield3 = new G4PVPlacement(0, G4ThreeVector(0.,0.,7.*cm + detLength/2), logicShield3, "physShield3", logicWorld, false, 0, true);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~Xe Volume~~~~~~~~~~~~~~~~~~~~~~~~~~//
	solidXeVolume = new G4Sphere("solidXeVolume", 0.0*m, 0.0614*m, 0*deg, 360*deg, 0*deg, 180*deg);
	logicXeVolume = new G4LogicalVolume(solidXeVolume, xeMat, "logicXeVolume");
	physXeVolume = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), logicXeVolume, "physXeVolume", logicWorld, false, 0, true);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~Window~~~~~~~~~~~~~~~~~~~~~~~~~~//
	if(spawnWindow == true)
	{
		solidWindow = new G4Tubs("solidWindow", 0.*mm, 80.*mm, 0.5*mm, 0*deg, 360*deg); //inner radius, outer radius, length, starting and stopping angle
		logicWindow = new G4LogicalVolume(solidWindow, windowMat, "logicWindow");
		physWindow = new G4PVPlacement(0, G4ThreeVector(0.,0.,11.*mm), logicWindow, "physWindow", logicWorld, false, 0, true);
	}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~BACKWALL~~~~~~~~~~~~~~~~~~~~~~~~//

	if(spawnBackWall == true)
	{
		G4Box *solidTest = new G4Box("solidTest", 0.4*m, 0.4*m, 0.04*m);
		G4LogicalVolume *logicTest = new G4LogicalVolume(solidTest, detMat, "logicTest");
		G4VPhysicalVolume *physTest = new G4PVPlacement(0, G4ThreeVector(0.,0.,-12.*cm), logicTest, "physTest", logicWorld, false, 0, true);
	}

//~~~~~~~~~~~~~~~~~~~~MAIN CHAMBER~~~~~~~~~~~~~~~~~~~~~~//
	G4RotationMatrix *rotationMatrix = new G4RotationMatrix();

	G4VSolid *solidSphereChamber = new G4Sphere("solidSphereChamber", 0.0614*m, 0.0812*m, 0*deg, 360*deg, 0*deg, 180*deg);
	G4VSolid *solidBoxChamber = new G4Box("solidBoxChamber", 0.062*m, 0.062*m, 0.062*m);
	G4VSolid *solidCylinderChamber = new G4Tubs("solidCylinderChamber", 0.0*m, 0.037*m, 0.124*m, 0*deg, 360*deg);
	
	G4VSolid *pre0_solidChamber = new G4IntersectionSolid("pre0_solidChamber", solidSphereChamber, solidBoxChamber, 0, G4ThreeVector(0,0,0)); //chamber with no holes
	rotationMatrix->rotateX(90.0 * degree);
	G4VSolid *solidChamber = new G4SubtractionSolid("solidChamber", pre0_solidChamber, solidCylinderChamber, rotationMatrix, G4ThreeVector(0,0,0)); //subtractin cylinder from chamber
	rotationMatrix->rotateY(90.0 * degree);
	G4VSolid *solidChamberFinal = new G4SubtractionSolid("solidChamberFinal", solidChamber, solidCylinderChamber, rotationMatrix, G4ThreeVector(0,0,0));
	G4VSolid *solidChamberFinalForReal = new G4SubtractionSolid("solidChamberFinalForReal", solidChamberFinal, solidCylinderChamber, 0, G4ThreeVector(0,0,0));

	logicChamber = new G4LogicalVolume(solidChamberFinalForReal, shieldMat2, "logicChamber");
	physChamber = new G4PVPlacement(0, G4ThreeVector(0,0,0), logicChamber, "physChamber", logicWorld,false,0,true);





	return physWorld;
}

void MyDetectorConstruction::ConstructSDandField()
{
	MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector");
	logicDet->SetSensitiveDetector(sensDet); //sets the logical volume we created for the detectors to have the properties of a sensitive detector, which we write in detector.cc!
}
