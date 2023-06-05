#include "construction.hh"

MyDetectorConstruction::MyDetectorConstruction()
{
	fMessenger = new G4GenericMessenger(this, "/construction/", "Environment Construction");
	

	fMessenger->DeclareProperty("detLength", detLength, "Length of detector arrangement").SetUnit("mm");
	detLength = 80.*mm;

	fMessenger->DeclareProperty("detRadius", detRadius, "Radius of detector arrangement").SetUnit("mm");
	detRadius = 70.*mm;

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

	G4Material *probeMat = nist->FindOrBuildMaterial("G4_Cu");
	G4MaterialPropertiesTable *mptProbe = new G4MaterialPropertiesTable();
	G4double rindexProbe[2] = {1.0, 1.0}; //change? probe shouldnt be repulsive
	//G4double energy[2] = {2.038*eV, 4.144*eV};
	mptProbe->AddProperty("RINDEX", energy, rindexProbe, 2);
	probeMat->SetMaterialPropertiesTable(mptProbe);

	G4MaterialPropertiesTable *mptintersectMat = new G4MaterialPropertiesTable();
	G4double rindexintersectMat[2] = {1.0,1.0};
	mptintersectMat->AddProperty("RINDEX", energy, rindexintersectMat, 2);

	Mn = nist->FindOrBuildElement("Mn");
	Si = nist->FindOrBuildElement("Si");
	Cr = nist->FindOrBuildElement("Cr");
	Ni = nist->FindOrBuildElement("Ni");
	Fe = nist->FindOrBuildElement("Fe");
	intersectMat = new G4Material("intersectMat", 8.03*g/cm3, 5);
	intersectMat->AddElement(Mn, 0.02*perCent);
	intersectMat->AddElement(Si, 0.01*perCent);
	intersectMat->AddElement(Cr, 0.19*perCent);
	intersectMat->AddElement(Ni, 0.10*perCent);
	intersectMat->AddElement(Fe, 0.68*perCent);
 
	intersectMat->SetMaterialPropertiesTable(mptintersectMat); //stainless steel

	G4Material *xeMat = nist->FindOrBuildMaterial("G4_Xe");

	G4Material *windowMat = nullptr;
	if(windowMatChoice == "G4_Be"){windowMat = nist->FindOrBuildMaterial(windowMatChoice);} 
	else if(windowMatChoice == "G4_Al"){windowMat = nist->FindOrBuildMaterial(windowMatChoice);}
	G4MaterialPropertiesTable *mptWindowMat = new G4MaterialPropertiesTable();
	G4double rindexWindow[2] = {1.0, 1.0};
	mptWindowMat->AddProperty("RINDEX", energy, rindexWindow, 2);
	windowMat->SetMaterialPropertiesTable(mptWindowMat);

	G4RotationMatrix *RotProbe = new G4RotationMatrix();
	RotProbe->rotateX(90.*deg);
	RotProbe->rotateY(0.*deg);
	RotProbe->rotateZ(0.*deg);

	

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~WORLD~~~~~~~~~~~~~~~~~~~~~~~~~//

	solidWorld = new G4Box("solidWorld", 2.*m, 2.*m, 3.*m);
	logicWorld = new G4LogicalVolume(solidWorld, worldMat, "logicWorld");
	physWorld = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.*m), logicWorld, "physWorld", 0, false, 0, true);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~HOLLOW SPHERE~~~~~~~~~~~~~~~~~~~~~~//

	G4Sphere *solidShell = new G4Sphere("solidShell", 0.15*m, 0.16*m, 0, 360, 0, 360);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~CUBE~~~~~~~~~~~~~~~~~~~~~~//

	G4Box *solidCube = new G4Box("solidCube", 0.124*m, 0.124*m, 0.124*m);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~INTERSECTION~~~~~~~~~~~~~~~~~~~~~~//

	solidIntersect = new G4IntersectionSolid("Cube&&Shell", solidCube, solidShell);
	logicIntersect = new G4LogicalVolume(solidIntersect, intersectMat, "logicIntersect");
	physIntersect = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), logicIntersect, "physIntersect", logicWorld, false, 0, true); 

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~PROBE~~~~~~~~~~~~~~~~~~~~~~//

	solidProbe = new G4Tubs("solidArmOut", 0.0117*m, 0.0127*m, 1.0597*m, 0*deg, 360*deg);
	logicProbe = new G4LogicalVolume(solidProbe, probeMat, "logicProbe");
	physProbe = new G4PVPlacement(RotProbe, G4ThreeVector(0.,1.07*m,0.), logicProbe, "physProbe", logicWorld, false, 0, true);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~OUTERMOUNT~~~~~~~~~~~~~~~~~~~~~~//

	solidOuterMount = new G4Tubs("solidOuterMount", 0.07*m, 0.1135*m, 0.1135*m, 0*deg, 360*deg);
	logicOuterMount = new G4LogicalVolume(solidOuterMount, intersectMat, "logicOuterMount");
	physOuterMount = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.247*m), logicOuterMount, "physOuterMount", logicWorld, false, 0, true);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~DETECTOR ARM OUT OUTER~~~~~~~~~~~~~~~~~~~~~~//

	solidArmOut = new G4Tubs("solidArmOut", 0.07*m, 0.08*m, 0.4396*m, 0*deg, 360*deg); //inner radius, outer radius, length, starting and stopping angle
	logicArmOut = new G4LogicalVolume(solidArmOut, intersectMat, "logicArmOut");
	physArmOut = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.80*m /*half of arm length+half of cube length */), logicArmOut, "physArmOut", logicWorld, false, 0, true);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~DETECTOR ARM XENON~~~~~~~~~~~~~~~~~~~~~~//

	solidArmIn = new G4Tubs("solidArmIn", 0.*m, 0.07*m, 0.56*m, 0*deg, 360*deg); //inner radius, outer radius, length, starting and stopping angle
	logicArmIn = new G4LogicalVolume(solidArmIn, xeMat, "logicArmIn");
	physArmIn = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.68*m /*half of arm length+half of cube length */), logicArmIn, "physArmIn", logicWorld, false, 0, true);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~// //replace mother volume with Arm?
//~~~~~~~~~~~~~~~~~~~~~~~DETECTOR~~~~~~~~~~~~~~~~~~~~~~//
	detOffset = 7.*mm;

	solidDet = new G4Tubs("solidDet", 0*mm, detRadius, detLength - detOffset, 0*deg, 360*deg); //inner radius, outer radius, length, starting and stopping angle
	logicDet = new G4LogicalVolume(solidDet, detMat, "logicDet");
	physDet = new G4PVPlacement(0, G4ThreeVector(0.,0.,1.17*m - detOffset + detLength/2), logicDet, "physDet", logicWorld, false, 0, true);

	fScoringVolume = logicDet;
	//G4cout << "DETECTOR DISTANCEEEEEEE" << 3.4*cm - detOffset + detLength/2 << G4endl;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~Al Cylinder~~~~~~~~~~~~~~~~~~~~//

	solidShield1 = new G4Tubs("solidShield1", detRadius, detRadius + 5.*mm, detLength, 0*deg, 360*deg); //inner radius, outer radius, length, starting and stopping angle
	logicShield1 = new G4LogicalVolume(solidShield1, shieldMat1, "logicShield1");
	physShield = new G4PVPlacement(0, G4ThreeVector(0.,0.,1.17*m + detLength/2), logicShield1, "physShield1", logicWorld, false, 0, true);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~Cu Cylinder~~~~~~~~~~~~~~~~~~~~//

	solidShield2 = new G4Tubs("solidShield2", detRadius + 5.*mm, detRadius + 10.*mm, detLength, 0*deg, 360*deg); //inner radius, outer radius, length, starting and stopping angle
	logicShield2 = new G4LogicalVolume(solidShield2, shieldMat2, "logicShield2");
	physShield2 = new G4PVPlacement(0, G4ThreeVector(0.,0.,1.17*m + detLength/2), logicShield2, "physShield2", logicWorld, false, 0, true);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~Pb Cylinder~~~~~~~~~~~~~~~~~~~~//

	solidShield3 = new G4Tubs("solidShield3", detRadius + 10.*mm, detRadius + 15.*mm, detLength, 0*deg, 360*deg); //inner radius, outer radius, length, starting and stopping angle
	logicShield3 = new G4LogicalVolume(solidShield3, shieldMat3, "logicShield3");
	physShield3 = new G4PVPlacement(0, G4ThreeVector(0.,0.,1.17*m + detLength/2), logicShield3, "physShield3", logicWorld, false, 0, true);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~Xe BOX~~~~~~~~~~~~~~~~~~~~~~~~~~//
	solidXeBox = new G4Box("solidXeBox", 0.08*m, 0.08*m, 0.08*m);
	logicXeBox = new G4LogicalVolume(solidXeBox, xeMat, "logicXeBox");
	physXeBox = new G4PVPlacement(0, G4ThreeVector(0.,0.,0.), logicXeBox, "physXeBox", logicWorld, false, 0, true);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~Window~~~~~~~~~~~~~~~~~~~~~~~~~~//
	if(spawnWindow == true)
	{
		solidWindow = new G4Tubs("solidWindow", 0.*mm, 60.*mm, 0.5*mm, 0*deg, 360*deg); //inner radius, outer radius, length, starting and stopping angle
		logicWindow = new G4LogicalVolume(solidWindow, windowMat, "logicWindow");
		physWindow = new G4PVPlacement(0, G4ThreeVector(0.,0.,1.13*m ), logicWindow, "physWindow", logicWorld, false, 0, true);
	}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~BACKWALL~~~~~~~~~~~~~~~~~~~~~~~~//

	if(spawnBackWall == true)
	{
		G4Box *solidTest = new G4Box("solidTest", 0.4*m, 0.4*m, 0.04*m);
		G4LogicalVolume *logicTest = new G4LogicalVolume(solidTest, detMat, "logicTest");
		G4VPhysicalVolume *physTest = new G4PVPlacement(0, G4ThreeVector(0.,0.,-12.*cm), logicTest, "physTest", logicWorld, false, 0, true);

	}


	/*G4OpticalSurface* OpWaterSurface = new G4OpticalSurface("WaterSurface");
	OpWaterSurface->SetModel(glisur);
	OpWaterSurface->SetType(dielectric_metal);
	OpWaterSurface->SetFinish(polished);
	G4LogicalSkinSurface* WaterSurface = new G4LogicalSkinSurface("WaterSurface",logicDet, OpWaterSurface);*/

	/*G4double pp[2] = {2.038*eV, 4.144*eV};
	G4double specularlobe[2] = {0.3, 0.3};
	G4double specularspike[2] = {0.2, 0.2};
	G4double backscatter[2] = {0.1, 0.1};
	G4double rindex[2] = {1.35, 1.40};
	G4double reflectivity[2] = {1.0, 1.0};
	G4double efficiency[2] = {1.0, 1.0};
	G4MaterialPropertiesTable *SurfaceMPT = new G4MaterialPropertiesTable();
	SurfaceMPT->AddProperty("RINDEX", pp, rindex, 2);
	SurfaceMPT->AddProperty("SPECULARLOBECONSTANT",pp,specularlobe,2);
	SurfaceMPT->AddProperty("SPECULARSPIKECONSTANT",pp,specularspike,2);
	SurfaceMPT->AddProperty("BACKSCATTERCONSTANT",pp,backscatter,2);
	SurfaceMPT->AddProperty("REFLECTIVITY",pp,reflectivity,2);
	SurfaceMPT->AddProperty("EFFICIENCY",pp,efficiency,2);
	OpWaterSurface->SetMaterialPropertiesTable(SurfaceMPT);*/

	/*G4double density0 = 1.29*kg/m3;
	G4Element *N = new G4Element("Nitrogen", "N", 7, 14.01*g/mole);	
	G4Element *O = new G4Element("Oxygen", "O", 8, 16.00*g/mole);	
	G4double f = 3.;
	G4double R = 8.1344626181532;
	G4double g0 = 9.81;
	G4double kappa = (f+2.)/f;
	G4double T = 293.15;
	G4double M = (0.3*16.00*g/mole + 0.7*14.01*g/mole)/1000.;
	for(G4int i=0; i<10; i++)
	{
		std::stringstream stri;
		stri << i;
		G4double h = 40e3/10.*i;
		G4double density = density0*pow((1-(kappa)/kappa*M*g0*h/(R*T)), (1/(kappa-1)));
		Air[i] = new G4Material("G4_AIR_"+stri.str(), density, 2);
		Air[i]->AddElement(N, 70*perCent);
		Air[i]->AddElement(O, 30*perCent);
	}
	solidAtmosphere = new G4Box("solidAtmosphere", 40*km, 40*km, 2*km);
	for(G4int i = 0; i<10; i++)
	{
		logicAtmosphere[i] = new G4LogicalVolume(solidAtmosphere, Air[i], "logicAtmosphere");
		physAtmosphere[i] = new G4PVPlacement(0, G4ThreeVector(0.,0.,(20.*km)/10.*2.*i - 20.*km + 2.*km), logicAtmosphere[i], "physAtmosphere", logicWorld, false, i, true);
	}*/

	/*G4VSolid* testcoatingsolid = new G4Trap("testcoatingsolid",5*cm,60*deg,60*deg,20*cm,20*cm,20*cm,0*deg,20*cm,20*cm,20*cm,0*deg);      
        G4LogicalVolume* testcoatinglog = new G4LogicalVolume(testcoatingsolid,detMat,"testcoatinglog");
        G4VPhysicalVolume* testcoatingphys = new G4PVPlacement(0,G4ThreeVector(0.,0.,10.*cm),testcoatinglog,"testcoatingphys",logicWorld,false,1,false);*/



        //making and placing the scintillator tile

        //G4VSolid* testsolid        = new G4Trap("testsolid",scintthickness/2,taperanglex,taperangley,squarelong/2,squarelong/2,squarelong/2,zeroangle,scintsquarelength/2,scintsquarelength/2,scintsquarelength/2,zeroangle);

	//G4VSolid* testsolid        = new G4Trap("testsolid",scintthickness/2,taperanglex,taperangley,squarelong/2,squarelong/2,squarelong/2,zeroangle,scintsquarelength/2,scintsquarelength/2,scintsquarelength/2,zeroangle);
        //G4LogicalVolume* testlog   = new G4LogicalVolume(testsolid,detMat,"testlog");
        //G4VPhysicalVolume* testphys = new G4PVPlacement(rotateNull,moveNull,testlog,"testphys",testcoatinglog,false,1,false);


        //Adding the SiPMs to the tiles

	/*G4OpticalSurface* OpWaterSurface = new G4OpticalSurface("WaterSurface");
	OpWaterSurface->SetModel(glisur);
	OpWaterSurface->SetType(dielectric_metal);
	OpWaterSurface->SetFinish(polished);
        G4LogicalSkinSurface* skinn = new G4LogicalSkinSurface("skinn",testcoatinglog,OpWaterSurface);*/


	return physWorld;
}

void MyDetectorConstruction::ConstructSDandField()
{
	MySensitiveDetector *sensDet = new MySensitiveDetector("SensitiveDetector");
	logicDet->SetSensitiveDetector(sensDet); //sets the logical volume we created for the detectors to have the properties of a sensitive detector, which we write in detector.cc!
}
