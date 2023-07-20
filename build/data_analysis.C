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




unordered_map<string, double> peakFitting(double timeFinal, string filename, double binWidth, double E_i, double E_f, int bckgFinderIterations, bool peakFit, bool makePlots, bool saveImages)
{
//########################LOADING HISTOGRAMS#################################//
	int binNumber = (E_f - E_i)/binWidth;
	double value;
	fstream file;
	TH1F *bckgHist = new TH1F("bckgHist", "SimBckg", binNumber, E_i, E_f);
	file.open("realBackground.txt", ios::in);
	while(1)
	{
		file >> value;
		bckgHist->Fill(value);
		if(file.eof()) break;
	}	
	file.close();

	auto simTimeCut = [timeFinal](double time){return (time < timeFinal);};
	RDataFrame df = ROOT::RDF::MakeCsvDataFrame(filename); 
	auto simGammaHist = df.Filter(simTimeCut, {"runTime"}).Histo1D({"gamma", "SimGammaHist;Energy[keV];Counts", binNumber, E_i, E_f}, "fEdep");
	TH1F *simTotalHist = new TH1F("simTotalHist", "Simulated Spectrum", binNumber, E_i, E_f);
	TH1F *noBckgHist = new TH1F("noBckgHist", "noBckgHist", binNumber, E_i, E_f);
//###########################################################################//
//###############################READING ENTRIES#################################//
	int simGammaEntries = 0;
	for (int i=0; i<binNumber+2; i++)
	{
		simGammaEntries+=simGammaHist->GetBinContent(i);
	}
	int simBckgEntries = 0;
	double bckgCounts[binNumber];
	for (int i=0; i<binNumber+2; i++)
	{
		simBckgEntries+=bckgHist->GetBinContent(i);
		bckgCounts[i] = bckgHist->GetBinContent(i);
		
	}
//##############################################################################//
//###############################BCKG SCALING#################################//
	double bckgActivity = 32.4295998; //counts per min
	double percentBckg = (bckgActivity*timeFinal)/((bckgActivity*timeFinal) + simGammaEntries);
	cout << "The percent bckg is: " << percentBckg << endl;	
	backgroundScaling(bckgHist, simBckgEntries, simGammaEntries, percentBckg, binNumber, bckgCounts);
	int countsBeforeBckg;
	int finalBckgCounts;
	double finalBinCounts[binNumber];
	double dest[binNumber];
	int simTotalEntries = 0;
	for (int i=0; i<binNumber+2; i++)
	{
		countsBeforeBckg = simGammaHist->GetBinContent(i);
		finalBckgCounts = bckgHist->GetBinContent(i);
		simTotalHist->SetBinContent(i, countsBeforeBckg + finalBckgCounts);
		simTotalEntries+=simTotalHist->GetBinContent(i);
		finalBinCounts[i] = simTotalHist->GetBinContent(i);
	}	
//##############################################################################//
	if(peakFit==true)
	{
		TSpectrum *s = new TSpectrum();
		TSpectrum *s2 = new TSpectrum(); 
		TSpectrum *s3 = new TSpectrum();

		TH1 *estBckgHist = s->Background(simTotalHist, bckgFinderIterations, "");
		for (int i=0; i<binNumber+2; i++)
		{
			double noBckgValue = ((simTotalHist->GetBinContent(i)) - estBckgHist->GetBinContent(i));
			if(noBckgValue<0) noBckgValue = 0; 
			noBckgHist->SetBinContent(i, noBckgValue);
		}

		int peaksFoundOriginal = 0;	
		int peaksFoundNoBckg = 0;
		peaksFoundOriginal = s2->Search(simTotalHist, 2, "", 0.20); /////////////
		peaksFoundNoBckg = s3->Search(noBckgHist, 1, "", 0.20);


		Double_t FitStart1 = 32.;
		Double_t FitEnd1 = 35.;	
		Double_t FitStart2 = 164;
		Double_t FitEnd2 = 168;
		double Amp33 = noBckgHist->GetBinContent(noBckgHist->GetXaxis()->FindBin(33.2));
		double Amp166 = noBckgHist->GetBinContent(noBckgHist->GetXaxis()->FindBin(165.86));

		double bckgValue33 = estBckgHist->GetBinContent(estBckgHist->FindBin(33.2));
		TF1 *f1 = new TF1("f1", "gaus(0)+[3]", FitStart1, FitEnd1);
		f1->SetParLimits(0, Amp33*0.92, Amp33*1.08);
		f1->SetParLimits(1, 33.0, 33.5);
		f1->SetParLimits(2, 0.2, 5.0);
		f1->SetParLimits(3, bckgValue33-0.01, bckgValue33+0.01);
		f1->SetParameters(Amp33, 33.20, 0.25, bckgValue33);

		double bckgValue166 = estBckgHist->GetBinContent(estBckgHist->FindBin(165.86));
		TF1 *f2 = new TF1("f2", "gaus(0)+[3]",FitStart2, FitEnd2);
		f2->SetParLimits(0, Amp166*0.92, Amp166*1.08);
		f2->SetParLimits(1, 165.4, 166.0); //165.86
		f2->SetParLimits(2, 0.35, 5.0);
		f2->SetParLimits(3, bckgValue166-0.01, bckgValue166+0.01);
		f2->SetParameters(Amp166, 165.86, 0.15, bckgValue166);

		simTotalHist->Fit(f1, "N","0", FitStart1, FitEnd1);
		simTotalHist->Fit(f2, "N","0", FitStart2, FitEnd2);

		double counts166 = (f2->Integral(FitStart2, FitEnd2))/binWidth;
		double counts33 = (f1->Integral(FitStart1, FitEnd1))/binWidth;
		double gaussDev33 = f1->GetParameter(2);
		double gaussDev166 = f2->GetParameter(2);
		
		if(makePlots==true)
		{
			TCanvas *dualCanvas = new TCanvas("DualCanvas", "DualCanvas", 1280, 700); //last 2 num are width and height
			simTotalHist->SetTitle(("Simulation Histogram Time: " + std::to_string(timeFinal)).c_str());
			simTotalHist->GetXaxis()->SetTitle("Energy (keV)");
			simTotalHist->GetYaxis()->SetTitle("Counts");
			noBckgHist->SetTitle(("Background Removed Histogram Time: " + std::to_string(timeFinal)).c_str());
			noBckgHist->GetXaxis()->SetTitle("Energy (keV)");
			noBckgHist->GetYaxis()->SetTitle("Counts");
			noBckgHist->GetYaxis()->SetRangeUser(0, 1.1*(noBckgHist->GetMaximum()));

		
			dualCanvas->Divide(2, 1);
			dualCanvas->cd(1);
			simTotalHist->Draw();
			estBckgHist->Draw("SAME");
			f1->SetLineStyle(1);
			f1->SetLineColor(1);
			f1->Draw("SAME");
			f2->SetLineStyle(1);
			f2->SetLineColor(1);
			f2->Draw("SAME");
			dualCanvas->cd(2);
			noBckgHist->Draw();
			if(saveImages ==  true)
			{
				string percentString = to_string(percentBckg);
				percentString.erase (percentString.find_last_not_of('0') + 1, std::string::npos );
				percentString.erase (percentString.find_last_not_of('.') + 1, std::string::npos );
				string timeString = to_string(timeFinal);
				string fileName = "histo" + timeString + ".png";
				dualCanvas->Print(fileName.c_str());
			}
		}
	}
	else {double counts166=0; double counts33=0; double gaussDev33=0; double gaussDev166=0;}



	unordered_map<string, double> statMap;
	statMap["Sigma33"] =  3;
	statMap["Sigma166"] = 14;
	statMap["PeaksFound"] = 15;
	statMap["Counts166"] = 16;
	statMap["Counts33"] = 12;
	
	return statMap;

}






void data_analysis()
{
	vector<double> Times;
	vector<double> Sigma33;
	vector<double> Sigma166;
	vector<double> PeaksFound;
	vector<double> Counts166;
	vector<double> Counts33; 
	unordered_map<string, double> statMap;

	double upperTime = 180.0; //IN MINUTES
	double incrementSize = 180.0;
	string filename = "gammaData/gammaData10_8.csv";
	double binWidth = 0.5;
	double E_i = 0.0; //in keV
	double E_f = 200.0; //in keV
	int bckgFinderIterations = 10;
	bool peakFit = true;
	bool makePlots = true; //these next two booleans only apply when peakFit is ON
	bool saveImages = false;
	
	for(double i=incrementSize; i<=upperTime; i+=incrementSize)
	{
		statMap = peakFitting(i, filename, binWidth, E_i, E_f, bckgFinderIterations, peakFit, makePlots, saveImages);

		Times.push_back(i);
		Sigma33.push_back(statMap["Sigma33"]);
		Sigma166.push_back(statMap["Sigma166"]);
		PeaksFound.push_back(statMap["PeaksFound"]);
		Counts166.push_back(statMap["Counts166"]);
		Counts33.push_back(statMap["Counts33"]);
	}


}

