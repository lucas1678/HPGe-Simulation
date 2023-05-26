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



void peakFitting(int binNumber, TH1F *bckgHist, double bckgCounts[], int simBckgEntries, double timeFinal, bool drawBool, bool saveBool) //timeFinal in MINUTES
{
	auto simTimeCut = [timeFinal](double time){return (time < timeFinal);};
	RDataFrame df = ROOT::RDF::MakeCsvDataFrame("gammaData.csv"); 
	auto simGammaHist = df.Filter(simTimeCut, {"runTime"}).Histo1D({"gamma", "SimGammaHist;Energy[keV];Counts", binNumber, 0, 400}, "fEdep");
	
	TH1F *simTotalHist = new TH1F("simTotalHist", "Simulated Spectrum", binNumber, 0, 400);
	TH1F *deconvHist = new TH1F("deconvHist", "deconvHist", binNumber, 0, 400);

	int simGammaEntries = 0;
	for (int i=0; i<binNumber+2; i++)
	{
		simGammaEntries+=simGammaHist->GetBinContent(i);
	}
	cout << "Simulation gamma entries are: " << simGammaEntries << endl;	

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
		//simGammaHist->SetBinContent(i, 0.5*(simGammaHist->GetBinContent(i)));
		simTotalEntries+=simTotalHist->GetBinContent(i);
		finalBinCounts[i] = simTotalHist->GetBinContent(i);
	}
	cout << "Simulation total entries (gamma + bckg) are: " << simTotalEntries << endl;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~PEAK FITTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	TSpectrum *s = new TSpectrum();
	int peaksFound = 0;
	peaksFound = s->SearchHighRes(finalBinCounts, dest, binNumber, 6, 10, kTRUE, 100, kTRUE, 3);
	// above goes like (...,sigma,threshold,bckgRemove,iterations,markovON/OFF, averWindow)
	// …The value in the center of the peak value[i] minus the average value in two symmetrically positioned channels (channels i-3*sigma, i+3*sigma) must be greater than threshold. Otherwise the peak is ignored…
	for(int i=0; i < binNumber+2; i++) deconvHist->SetBinContent(i, dest[i]);
	deconvHist->SetLineColor(kRed);

	Double_t *xpeaks = s->GetPositionX();
	Double_t a;
	Int_t bin;
	Double_t fPositionX[100];
   	Double_t fPositionY[100];
	for (int i = 0; i < peaksFound; i++) 
	{
		a=xpeaks[i];
		bin = 0 + Int_t(a + 0.5);
		fPositionX[i] = deconvHist->GetBinCenter(bin);
		fPositionY[i] = deconvHist->GetBinContent(bin);
		
	}
	
	TPolyMarker * pm = (TPolyMarker*)deconvHist->GetListOfFunctions()->FindObject("TPolyMarker");
	if (pm) 
	{
		deconvHist->GetListOfFunctions()->Remove(pm);
		delete pm;
	}
	pm = new TPolyMarker(peaksFound, fPositionX, fPositionY);
	deconvHist->GetListOfFunctions()->Add(pm);
	pm->SetMarkerStyle(23);
	pm->SetMarkerColor(kRed);
	pm->SetMarkerSize(1.3);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~GAUSSIAN FITS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	Double_t FitStart1 = 31;
	Double_t FitEnd1 = 35.2;	
	Double_t FitStart2 = 163.8;
	Double_t FitEnd2 = 167.8;

	TF1 *f1 = new TF1("f1", "gausn(0)", FitStart1, FitEnd1);
	f1->SetParameters(40, 33.20, 0.25);
	f1->SetParLimits(1, 33.0, 33.5);
	f1->SetParLimits(2, 0.2, 5.0);

	TF1 *f2 = new TF1("f2", "gausn(0)",FitStart2, FitEnd2);
	f2->SetParameters(50, 165.86, 0.15);
	f2->SetParLimits(1, 165.4, 166.0); //165.86
	f2->SetParLimits(2, 0.2, 5.0);



	deconvHist->Fit(f1, "N","0", FitStart1, FitEnd1);
	deconvHist->Fit(f2, "N","0", FitStart2, FitEnd2);

	/*Double_t gaussHeight = f1->GetParameter(0);
	Double_t gaussMean = f1->GetParameter(1);
	Double_t gaussDev = f1->GetParameter(2);
	cout << "Height: " << gaussHeight << "\nMean: " << gaussMean << "\nDev: " << gaussDev << endl;
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
	gaussHeight = f2->GetParameter(0);
	gaussMean = f2->GetParameter(1);
	gaussDev = f2->GetParameter(2);
	cout << "Height: " << gaussHeight << "\nMean: " << gaussMean << "\nDev: " << gaussDev << endl;*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

	TCanvas *dualCanvas = new TCanvas("DualCanvas", "DualCanvas", 1280, 700); //last 2 num are width and height
	if(drawBool == true)
	{
		simTotalHist->SetTitle(("Simulation Histogram Time: " + std::to_string(timeFinal)).c_str());
		simTotalHist->GetXaxis()->SetTitle("Energy (keV)");
		simTotalHist->GetYaxis()->SetTitle("Counts");
		deconvHist->SetTitle(("Deconvoluted Histogram Time: " + std::to_string(timeFinal)).c_str());
		deconvHist->GetXaxis()->SetTitle("Energy (keV)");
		deconvHist->GetYaxis()->SetTitle("Counts");

		/*TCanvas *c1 = new TCanvas();
		simTotalHist->Draw();

		TCanvas *c2 = new TCanvas();
		deconvHist->Draw();
		f1->SetLineStyle(1);
		f1->SetLineColor(1);
		f1->Draw("SAME");
		f2->SetLineStyle(1);
		f2->SetLineColor(1);
		f2->Draw("SAME");*/

	
		dualCanvas->Divide(2, 1);
		/*auto leftPad = dualCanvas->cd(1);
		leftPad->SetLogx();
		leftPad->SetLogy();*/
		dualCanvas->cd(1);
		simTotalHist->Draw();
		dualCanvas->cd(2);
		deconvHist->Draw();
		f1->SetLineStyle(1);
		f1->SetLineColor(1);
		f1->Draw("SAME");
		f2->SetLineStyle(1);
		f2->SetLineColor(1);
		f2->Draw("SAME");

	}
	if(saveBool ==  true)
	{
		string percentString = to_string(percentBckg);
		percentString.erase ( percentString.find_last_not_of('0') + 1, std::string::npos );
		percentString.erase ( percentString.find_last_not_of('.') + 1, std::string::npos );
		string timeString = to_string(timeFinal);
		string fileName = percentString + "histo" + timeString + ".png";
		dualCanvas->Print(fileName.c_str());
	}
}




void testAnalysis()
{
//###############################OPENING FILES######################################//
	int simBinNumber = 1000;
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
//###############################READING ENTRIES######################################//
	int simBckgEntries = 0;

	double bckgCounts[simBinNumber];
	for (int i=0; i<simBinNumber+2; i++)
	{
		simBckgEntries+=simBackgroundHist->GetBinContent(i);
		bckgCounts[i] = simBackgroundHist->GetBinContent(i);
		
	}
	cout << "Simulation bckg entries are: " << simBckgEntries << endl;


	double runTime = 60.0; //IN MINUTES
	peakFitting(simBinNumber, simBackgroundHist, bckgCounts, simBckgEntries, runTime, true, false);
	runTime = 45.0;
	peakFitting(simBinNumber, simBackgroundHist, bckgCounts, simBckgEntries, runTime, true, false);
	runTime = 30.0;
	peakFitting(simBinNumber, simBackgroundHist, bckgCounts, simBckgEntries, runTime, true, false);
	runTime = 15.0;
	peakFitting(simBinNumber, simBackgroundHist, bckgCounts, simBckgEntries, runTime, true, false);
	runTime = 5.0;
	peakFitting(simBinNumber, simBackgroundHist, bckgCounts, simBckgEntries, runTime, true, false);

}
