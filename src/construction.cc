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

	G4Material *steelMat = nist->FindOrBuildMaterial("G4_STAINLESS-STEEL");
	G4MaterialPropertiesTable *mptSteel = new G4MaterialPropertiesTable();
	mptSteel->AddProperty("RINDEX", energy, rindexShield3, 2);
	steelMat->SetMaterialPropertiesTable(mptSteel);

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

	solidWorld = new G4Box("solidWorld", 1.*m, 1.*m, 1.*m);
	logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
	physWorld = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), logicWorld, "physWorld", 0, false, 0, true);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~Ge_Crystal~~~~~~~~~~~~~~~~~~~~~~//
	//detOffset = 15.*mm;

	solidDet = new G4Tubs("solidDet", 0*mm, detRadius, detLength/2., 0*deg, 360*deg); //inner radius, outer radius, length, starting and stopping angle
	logicDet = new G4LogicalVolume(solidDet, detMat, "logicDet");
	physDet = new G4PVPlacement(0, G4ThreeVector(0.,0.,7.*cm + detLength/2), logicDet, "physDet", logicWorld, false, 0, true);

	fScoringVolume = logicDet;
//~~~~~~~~~~~~~~~~~~~~~DET STEEL HOUSING + WINDOW~~~~~~~~~~~~~~//
	solidHousing = new G4Tubs("solidHousing", 29*mm, 30*mm, (279./2.)*mm, 0*deg, 360*deg);
	logicHousing = new G4LogicalVolume(solidHousing, steelMat, "logicHousing");
	physHousing = new G4PVPlacement(0, G4ThreeVector(0,0,18.5*cm), logicHousing, "physHousing", logicWorld, false, 0, true);	

	solidWindow = new G4Tubs("solidWindow", 0.*mm, 29.*mm, 0.025*mm, 0*deg, 360*deg);
	logicWindow = new G4LogicalVolume(solidWindow, windowMat, "logicWindow");
	physWindow = new G4PVPlacement(0, G4ThreeVector(0.,0.,7.*cm - 5*mm), logicWindow, "physWindow", logicWorld, false, 0, true);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/*//~~~~~~~~~~~~~~~~~~~~~Al Cylinder~~~~~~~~~~~~~~~~~~~~//

	solidShield1 = new G4Tubs("solidShield1", detRadius, detRadius + 5.*mm, detLength + 60.*mm, 0*deg, 360*deg); //inner radius, outer radius, length, starting and stopping angle
	logicShield1 = new G4LogicalVolume(solidShield1, shieldMat1, "logicShield1");
	physShield = new G4PVPlacement(0, G4ThreeVector(0.,0.,12.*cm + detLength/2), logicShield1, "physShield1", logicWorld, false, 0, true);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~Cu Cylinder~~~~~~~~~~~~~~~~~~~~//

	solidShield2 = new G4Tubs("solidShield2", detRadius + 5.*mm, detRadius + 10.*mm, detLength+ 60.*mm, 0*deg, 360*deg); //inner radius, outer radius, length, starting and stopping angle
	logicShield2 = new G4LogicalVolume(solidShield2, shieldMat2, "logicShield2");
	physShield2 = new G4PVPlacement(0, G4ThreeVector(0.,0.,12.*cm + detLength/2), logicShield2, "physShield2", logicWorld, false, 0, true);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~Pb Cylinder~~~~~~~~~~~~~~~~~~~~//

	solidShield3 = new G4Tubs("solidShield3", detRadius + 10.*mm, detRadius + 15.*mm, detLength+ 60.*mm, 0*deg, 360*deg); //inner radius, outer radius, length, starting and stopping angle
	logicShield3 = new G4LogicalVolume(solidShield3, shieldMat3, "logicShield3");
	physShield3 = new G4PVPlacement(0, G4ThreeVector(0.,0.,12.*cm + detLength/2), logicShield3, "physShield3", logicWorld, false, 0, true);
*/
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

//~~~~~~~~~~~~~~~~~~~~MAIN CHAMBER~~~~~~~~~~~~~~~~~~~~~~// //CHANGE MATERIAL!!
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

	logicChamber = new G4LogicalVolume(solidChamberFinalForReal, steelMat, "logicChamber");
	physChamber = new G4PVPlacement(0, G4ThreeVector(0,0,0), logicChamber, "physChamber", logicWorld,false,0,true);

//~~~~~~~~~~~~~~~CHAMBER SIDE DETECTOR HOUSING~~~~~~~~~~~~~~~//
	solidChamberSide = new G4Tubs("solidChamberSide", (0.074/2.)*m, (0.105/2.)*m, (5.645/3)*cm, 0*deg, 360*deg);
	logicChamberSide = new G4LogicalVolume(solidChamberSide, steelMat, "logicChamberSide");
	physChamberSide = new G4PVPlacement(0, G4ThreeVector(0,0,11.*cm), logicChamberSide, "physChamberSide", logicWorld,false,0,true);
	
	solidChamberSide2 = new G4Tubs("solidChamberSide2", (0.074/2.)*m, (0.105/2.)*m, (3.816/2.)*cm, 0*deg, 360*deg);
	logicChamberSide2 = new G4LogicalVolume(solidChamberSide2, steelMat, "logicChamberSide2");
	physChamberSide2 = new G4PVPlacement(0, G4ThreeVector(0,0,((2.697)+(5.645/2.)+9+(3.816/2.))*cm), logicChamberSide2, "physChamberSide2", logicWorld,false,0,true);

	solidChamberSide3_1 = new G4Box("solidChamberSide3_1", (23.515/2.)*cm, (11.015/2.)*cm, (2.645/2.)*cm);
	solidChamberSide3_2 = new G4Tubs("solidChamberSide3_2", 0*m, (0.105/2.)*m, (2.697/2.)*cm, 0*deg, 360*deg);
	solidChamberSide3 = new G4SubtractionSolid("solidChamberSide3", solidChamberSide3_1, solidChamberSide3_2, 0, G4ThreeVector(6.5*cm,0,0));
	logicChamberSide3 = new G4LogicalVolume(solidChamberSide3, steelMat, "logicChamberSide3");
	physChamberSide3 = new G4PVPlacement(0, G4ThreeVector(-6.5*cm,0,((2.697/2.)+(5.645/2.)+9)*cm), logicChamberSide3, "physChamberSide3", logicWorld,false,0,true);

	solidChamberSide4 = new G4Tubs("solidChamberSide4", (7.07/2.)*cm, (7.4/2.)*cm, (43.01/2.)*cm, 0*deg, 360*deg);
	logicChamberSide4 = new G4LogicalVolume(solidChamberSide4, steelMat, "logicChamberSide4");
	physChamberSide4 = new G4PVPlacement(0, G4ThreeVector(0,0,((2.697)+(5.645/2.)+9+3.816+(43.01/2.))*cm), logicChamberSide4, "physChamberSide4", logicWorld,false,0,true);

	solidGearBase = new G4Tubs("solidGearBase", (7.366/2.)*cm, (10.49/2.)*cm, (3.505/2.)*cm, 0*deg, 360*deg);
	G4VSolid *solidGearSub1 = new G4Tubs("solidGearSub1", ((7.366/2.)+1.1175)*cm, (11/2.)*cm, (3.6/2.)*cm, 0*deg, 36*deg);
	G4VSolid *solidGearSub2 = new G4Tubs("solidGearSub2", ((7.366/2.)+1.1175)*cm, (11/2.)*cm, (3.6/2.)*cm, 72*deg, 36*deg);
	G4VSolid *solidGearSub3 = new G4Tubs("solidGearSub3", ((7.366/2.)+1.1175)*cm, (11/2.)*cm, (3.6/2.)*cm, 144*deg, 36*deg);
	G4VSolid *solidGearSub4 = new G4Tubs("solidGearSub4", ((7.366/2.)+1.1175)*cm, (11/2.)*cm, (3.6/2.)*cm, 216*deg, 36*deg);
	G4VSolid *solidGearSub5 = new G4Tubs("solidGearSub5", ((7.366/2.)+1.1175)*cm, (11/2.)*cm, (3.6/2.)*cm, 288*deg, 36*deg);
	G4VSolid *solidGearSub6 = new G4Tubs("solidGearSub6", ((7.366/2.)+1.1175)*cm, (11/2.)*cm, (3.6/2.)*cm, 225*deg, 15*deg);
	G4VSolid *solidGearSub7 = new G4Tubs("solidGearSub7", ((7.366/2.)+1.1175)*cm, (10.6/2.)*cm, (3.6/2.)*cm, 270*deg, 15*deg);
	G4VSolid *solidGearSub8 = new G4Tubs("solidGearSub8", ((7.366/2.)+1.1175)*cm, (10.6/2.)*cm, (3.6/2.)*cm, 315*deg, 15*deg);
	G4VSolid *solidGear1 = new G4SubtractionSolid("solidGear1", solidGearBase, solidGearSub1);
	G4VSolid *solidGear2 = new G4SubtractionSolid("solidGear2", solidGear1, solidGearSub2);
	G4VSolid *solidGear3 = new G4SubtractionSolid("solidGear3", solidGear2, solidGearSub3);
	G4VSolid *solidGear4 = new G4SubtractionSolid("solidGear4", solidGear3, solidGearSub4);
	G4VSolid *solidGear5 = new G4SubtractionSolid("solidGear5", solidGear4, solidGearSub5);
	G4VSolid *solidGear6 = new G4SubtractionSolid("solidGear6", solidGear5, solidGearSub6);
	G4VSolid *solidGear7 = new G4SubtractionSolid("solidGear7", solidGear6, solidGearSub7);
	G4VSolid *solidGear8 = new G4SubtractionSolid("solidGear8", solidGear7, solidGearSub8);
	logicGear = new G4LogicalVolume(solidGear5, steelMat, "logicGear");
	physGear = new G4PVPlacement(0, G4ThreeVector(0,0, 7.5*cm), logicGear, "physGear", logicWorld,false,0,true);

	
	

//~~~~~~~~~~~~~~~~~~~~~~PROBE~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	rotationMatrix->rotateX(0.*deg);
	rotationMatrix->rotateY(90.*deg);
	rotationMatrix->rotateZ(0.*deg);


	solidProbe = new G4Tubs("solidArmOut", (0.0117/2.0)*m, (0.0127/2.0)*m, (1.0597/2.0)*m, 0*deg, 360*deg);
	logicProbe = new G4LogicalVolume(solidProbe, steelMat, "logicProbe");
	physProbe = new G4PVPlacement(rotationMatrix, G4ThreeVector(0.,((1.0597/2.0)+0.053)*m,0.), logicProbe, "physProbe", logicWorld, false, 0, true);

	solidTip = new G4Cons("solidTube", (0.0117/2.0)*m, (0.0127/2.0)*m, 0*m, (0.01/2.0)*m, 0.006*m, 0*deg, 360*deg);
	logicTip = new G4LogicalVolume(solidTip, steelMat, "logicTip");
	physTip = new G4PVPlacement(rotationMatrix, G4ThreeVector(0.,0.047*m,0.), logicTip, "physTip", logicWorld, false, 0, true);

	solidTip2_1 = new G4Box("solidTip2_1", 0.003*m, 0.001*m, 0.018*m);
	solidTip2_2 = new G4Tubs("solidTip2_2", 0.*m, 0.003*m, 0.018*m, 180*deg, 180*deg);
	solidTip2 = new G4UnionSolid("solidTip2", solidTip2_1, solidTip2_2, 0, G4ThreeVector(0,-0.0005*m,0));
	logicTip2 = new G4LogicalVolume(solidTip2, steelMat, "logicTip2");
	physTip2 = new G4PVPlacement(rotationMatrix, G4ThreeVector(0,0.023*m,0.001*m), logicTip2, "physTip2", logicWorld, false, 0, true);

	solidTip3 = new G4Box("solidTip3", 0.003*m, 0.0005*m, 0.019*m);
	logicTip3 = new G4LogicalVolume(solidTip3, steelMat, "logicTip3");
	physTip3 = new G4PVPlacement(rotationMatrix, G4ThreeVector(0,0.010*m,-0.0025*m), logicTip3, "physTip3", logicWorld, false, 0, true);

	solidTipConnector1 = new G4Tubs("solidTipConnector1", 0.001*m, 0.0015*m, 0.001*m, 0*deg, 360*deg);
	solidTipConnector2 = new G4Tubs("solidTipConnector2", 0.001*m, 0.0015*m, 0.001*m, 0*deg, 360*deg);
	solidTipConnector = new G4UnionSolid("solidTipConnector", solidTipConnector1, solidTipConnector2, 0, G4ThreeVector(0,0.014*m,0));

	logicTipConnector = new G4LogicalVolume(solidTipConnector, steelMat, "logicTipConnector");
	physTipConnector = new G4PVPlacement(0, G4ThreeVector(0,0.012*m,-0.001*m), logicTipConnector, "physTipConnector", logicWorld, false, 0, true);

	solidTipConnector3 = new G4Tubs("solidTipConnector3", 0.0009*m, 0.0010*m, 0.0015*m, 0*deg, 360*deg);
	solidTipConnector4 = new G4Tubs("solidTipConnector4", 0.0009*m, 0.0010*m, 0.0015*m, 0*deg, 360*deg);
	solidTipConnector_2 = new G4UnionSolid("solidTipConnector_2", solidTipConnector3, solidTipConnector4, 0, G4ThreeVector(0,0.014*m,0));

	logicTipConnector_2 = new G4LogicalVolume(solidTipConnector_2, steelMat, "logicTipConnector_2");
	physTipConnector_2 = new G4PVPlacement(0, G4ThreeVector(0,0.012*m,-0.0045*m), logicTipConnector_2, "physTipConnector_2", logicWorld, false, 0, true);


//~~~~~~~~~~~~~PLACEHOLDER TO SHOW WHERE SPHERICAL SOURCE GOES~~~~~~~~~~~~~~~~~~~~~~~~//
	/*G4Sphere *testy = new G4Sphere("testy", 0*m, 0.635*cm, 0*deg, 360*deg, 0*deg, 180*deg);
	G4LogicalVolume *logicTesty = new G4LogicalVolume(testy, steelMat, "logicTesty");
	G4PVPlacement *physTesty = new G4PVPlacement(0, G4ThreeVector(0,-0.25*cm,0.45*cm), logicTesty, "physTesty", logicWorld, false, 0, true);*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

	return physWorld;
}

void MyDetectorConstruction::ConstructSDandField()
{
	MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector");
	logicDet->SetSensitiveDetector(sensDet); //sets the logical volume we created for the detectors to have the properties of a sensitive detector, which we write in detector.cc!
}
