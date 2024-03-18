using namespace ROOT;
void backgroundScaling(TH1F *bckgHist, int &bckgEntries, int gammaEntries, double bckgPercent, int simBinNumber, double bckgCounts[])
{
	//need & in front of bckgEntries so that function actually changes the bckgEntries variable instead of making a copy of it that exists only inside this function!
	double bckgScaling = (bckgPercent/(1-bckgPercent))*((double)gammaEntries/(double)bckgEntries);
	int targetEntries = bckgScaling*bckgEntries;
	cout << "targetEntries: " << targetEntries << endl;
	
	int n=0;
	while(1)
	{
		bckgEntries = 0;
		for (int i=0; i<simBinNumber+2; i++)
		{
			bckgHist->SetBinContent(i, (bckgScaling+0.001*n)*(bckgHist->GetBinContent(i)));
			bckgEntries+=bckgHist->GetBinContent(i);
		}
		
		if (bckgEntries > targetEntries) {break;}
		
		bckgEntries = 0;
		for (int i=0; i<simBinNumber+2; i++)
		{
			bckgHist->SetBinContent(i, bckgCounts[i]);
			bckgEntries+=bckgHist->GetBinContent(i);
		}

		n++;
	}
}


void timeAnalysis()
{
	double timeFinal; //in minutes
	int simBckgEntries=0;
	int simBinNumber = 1000;
	double finalBinCounts[simBinNumber];
	int finalBckgCounts;

	bool drawBool;
	bool saveBool;

	TH1F *simGammaHist = new TH1F("simGammaHist", "Only Gamma Simulated", simBinNumber, 0, 1000); 
	TH1F *simHist = new TH1F("simHist", "Simulated Histogram", simBinNumber, 0, 1000);
	TH1F *bckgHist = new TH1F("bckgHist", "Background Histogram", simBinNumber, 0, 1000);	

	/*int BinNumber;
	TH1F *bckgHist, double bckgCounts[], int simBckgEntries, double timeFinal, bool drawBool, bool saveBool);*/
	//auto simTimeCut = [timeFinal](double time){return (time < timeFinal);};
	RDataFrame df = ROOT::RDF::MakeCsvDataFrame("gammaData.txt"); 
	//auto simGammaHist = df.Filter(simTimeCut, {"runTime"}).Histo1D({"gamma", "SimGammaHist;Energy[keV];Counts", simBinNumber, 0, 400}, "fEdep");
	

	int simGammaEntries = 0;
	for (int i=0; i<simBinNumber+2; i++)
	{
		simGammaEntries+=simGammaHist->GetBinContent(i);
	}
	cout << "Simulation gamma entries are: " << simGammaEntries << endl;	

	double bckgActivity = 32.4295998; //counts per min
	double percentBckg = (bckgActivity*timeFinal)/((bckgActivity*timeFinal) + simGammaEntries);
	cout << "The percent bckg is: " << percentBckg << endl;	
	cout << "timeFinal is: " << timeFinal << endl;
	//backgroundScaling(bckgHist, simBckgEntries, simGammaEntries, percentBckg, simBinNumber, finalBckgCounts);
	
	int simTotalEntries = 0;
	for (int i=0; i<simBinNumber+2; i++)
	{
		finalBckgCounts = bckgHist->GetBinContent(i);
		cout << "finalBinCounts: " << finalBinCounts << endl;
		simHist->SetBinContent(i, simGammaEntries + finalBckgCounts);
		//simGammaHist->SetBinContent(i, 0.5*(simGammaHist->GetBinContent(i)));
		simTotalEntries+=simHist->GetBinContent(i);
		finalBinCounts[i] = simHist->GetBinContent(i);

	}
	cout << "Simulation total entries (gamma + bckg) are: " << simTotalEntries << endl;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

	TCanvas *canvas = new TCanvas();
	simHist->SetTitle("Simulation Histogram Time: ");
	simHist->GetXaxis()->SetTitle("Energy (keV)");
	simHist->GetYaxis()->SetTitle("Counts");
	simHist->Draw();

	TCanvas *canvas2 = new TCanvas();
	bckgHist->SetTitle("BackgroundHist");
	bckgHist->GetXaxis()->SetTitle("Energy (keV)");
	bckgHist->GetYaxis()->SetTitle("Counts");
	bckgHist->Draw();

	/*if(saveBool ==  true)
	{
		string percentString = to_string(percentBckg);
		percentString.erase ( percentString.find_last_not_of('0') + 1, std::string::npos );
		percentString.erase ( percentString.find_last_not_of('.') + 1, std::string::npos );
		string timeString = to_string(timeFinal);
		string fileName = percentString + "histo" + timeString + ".png";
		dualCanvas->Print(fileName.c_str());
	}*/




//###############################OPENING FILES######################################//
	TH1F *simBackgroundHist = new TH1F("simBackgroundHist", "SimBckg", simBinNumber, 0, 400);

	double value;
	fstream file;
	file.open("realBackground.txt", ios::in);
	while(1)
	{
		file >> value;
		simBackgroundHist->Fill(value);
		if(file.eof()) break;
	}	
	file.close();

	file.open("gammaData.txt", ios::in);
	while(1)
	{
		file >> value;
		simGammaHist->Fill(value);
		if(file.eof()) break;
	}
	file.close();
//###############################READING ENTRIES######################################//
	double bckgCounts[simBinNumber];
	for (int i=0; i<simBinNumber+2; i++)
	{
		simBckgEntries+=simBackgroundHist->GetBinContent(i);
		bckgCounts[i] = simBackgroundHist->GetBinContent(i);

	}
	cout << "Simulation bckg entries are: " << simBckgEntries << endl;
}

