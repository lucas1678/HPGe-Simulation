void data_analysis()
{
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DECLARING HISTOGRAMS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	int comparisonBinNumber = 1000;	//922
	int simBinNumber = 1000;

	TH1F *simGammaHist = new TH1F("simGammaHist", "Just Gamma", simBinNumber, 0, 400); 
	TH1F *simBackgroundHist = new TH1F("simBackgroundHist", "SimBckg", simBinNumber, 0, 400);
	TH1F *simTotalHist = new TH1F("simTotalHist", "Simulated Spectrum", simBinNumber, 0, 400);
	TH1F *comparisonHist = new TH1F("comparisonHist", "Real Spectrum", comparisonBinNumber, 0, 400);
	TH1F *comparisonNoBckg = new TH1F("comparisonNoBckg", "comparisonNoBckg", comparisonBinNumber, 0, 400);
	TH1F *deconvHist = new TH1F("deconvHist", "deconvHist", comparisonBinNumber, 0, 400);	
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~OPENING FILES~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	double value;
	fstream file;

	file.open("gammaData.txt", ios::in);
	while(1)
	{
		file >> value;
		simGammaHist->Fill(value);
		if(file.eof()) break;
	}
	file.close();
	

	file.open("realBackground.txt", ios::in);
	while(1)
	{
		file >> value;
		simBackgroundHist->Fill(value);
		if(file.eof()) break;
	}	
	file.close();


	file.open("realBa133.txt", ios::in);
	while(1)
	{
		file >> value;
		comparisonHist->Fill(value);
		if(file.eof()) break;
	}

	file.close();
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~GETTING ENTRIES~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//to get total entries, since for some reason when u scale weights root does not calculate this:
	int realTotalEntries = 0;	
	int simGammaEntries = 0;
	int simBckgEntries = 0;
	int simTotalEntries = 0;

	for (int i=0; i<comparisonBinNumber+2; i++)
	{
		realTotalEntries+=comparisonHist->GetBinContent(i);
	}
	cout << "Real total entries are: " << realTotalEntries << endl;




	for (int i=0; i<simBinNumber+2; i++)
	{
		simGammaEntries+=simGammaHist->GetBinContent(i);
	}
	cout << "Simulation gamma entries are: " << simGammaEntries << endl;



	double bckgCounts[simBinNumber];
	for (int i=0; i<simBinNumber+2; i++)
	{
		simBckgEntries+=simBackgroundHist->GetBinContent(i);
		bckgCounts[i] = simBackgroundHist->GetBinContent(i);
		
	}
	cout << "Simulation bckg entries are: " << simBckgEntries << endl;
	cout << "BIN WIDTH gammA IS: " << simGammaHist->GetBinWidth(80) << endl;
	cout << "BIN WIDTH backg IS: " << simBackgroundHist->GetBinWidth(80) << endl;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~QUANTUM EFFICIENCY~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	/*Double_t edep;
	Double_t scaledCounts;
	Double_t p = 28967.7; //CHANGE TO MATCH REAL EFF.
	Double_t k = 0.00000059526;
	Double_t d = 11.0431;
	Double_t b = -0.0127744;
	Double_t c = 0.00000651426;	
	for (int i=0; i<simBinNumber+2; i++)
	{
		edep = (simGammaHist->GetBinWidth(i))*i;
		scaledCounts = simGammaHist->GetBinContent(i);
		scaledCounts = scaledCounts*p*(sin(k*(edep-d))*exp(b*edep)+c);
		//cout << (p*sin(b*edep - d)*exp(-f*edep)+c) << endl;
		simGammaHist->SetBinContent(i, scaledCounts);
	}*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FIRST BCKG SCALING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	double percentBckg = 0.2127; //, , 0.0855549, 0.0883125	0.0883125 from excel
	double bckgScaling = (percentBckg/(1-percentBckg))*((double)simGammaEntries/(double)simBckgEntries);
	int targetEntries = bckgScaling*simBckgEntries;
	cout << "targetEntries: " << targetEntries << endl;
	
	int n=0;
	while(1)
	{
		simBckgEntries = 0;
		for (int i=0; i<simBinNumber+2; i++)
		{
			simBackgroundHist->SetBinContent(i, (bckgScaling+0.001*n)*(simBackgroundHist->GetBinContent(i)));
			simBckgEntries+=simBackgroundHist->GetBinContent(i);
		}
		//cout << "In loop, bckg entries are now: " << simBckgEntries << endl;
		
		if (simBckgEntries > targetEntries) break;
		
		simBckgEntries = 0;
		for (int i=0; i<simBinNumber+2; i++)
		{
			simBackgroundHist->SetBinContent(i, bckgCounts[i]);
			simBckgEntries+=simBackgroundHist->GetBinContent(i);
		}
		//cout << "In loop WITH RESET, bckg entries are now: " << simBckgEntries << endl;
		n++;
	}
	cout << "Scaled bckg counts are now: " << simBckgEntries << endl;
	/*simBckgEntries = 0;
	for (int i=0; i<simBinNumber+2; i++)
	{
		simBackgroundHist->SetBinContent(i, bckgScaling*(simBackgroundHist->GetBinContent(i)));
		simBckgEntries+=simBackgroundHist->GetBinContent(i);
	}
	cout << "BckgCounts are now: " << simBckgEntries << endl;*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FINE BCKG SCALE ADJUSTMENTS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	/*int n = 0;	
	while(simBckgEntries < targetEntries)
	{
		simBckgEntries = 0;
		for (int i=0; i<simBinNumber+2; i++)
		{
			simBackgroundHist->SetBinContent(i, (bckgScaling+0.001*n)*(simBackgroundHist->GetBinContent(i)));
			simBckgEntries+=simBackgroundHist->GetBinContent(i);
		}
		n++;
		cout << "In loop, bckg entries are now: " << simBckgEntries << endl;
		cout << "In loop, scaling factor is: " << (bckgScaling+0.001*n) << endl;
	}
	n-=2;
	simBckgEntries = 0;
	for (int i=0; i<simBinNumber+2; i++)
	{
		simBackgroundHist->SetBinContent(i, (bckgScaling+0.001*n)*(simBackgroundHist->GetBinContent(i)));
		simBckgEntries+=simBackgroundHist->GetBinContent(i);
	}
	cout << "Final Bckg Entries is... " << simBckgEntries << endl;*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ADDING BACKGROUND TO GAMMA DATA~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	int countsBeforeBckg;
	int finalBckgCounts;
	double finalBinCounts[simBinNumber];
	double dest[simBinNumber];
	
	for (int i=0; i<simBinNumber+2; i++)
	{
		countsBeforeBckg = simGammaHist->GetBinContent(i);
		finalBckgCounts = simBackgroundHist->GetBinContent(i);
		simTotalHist->SetBinContent(i, countsBeforeBckg + finalBckgCounts);
		//simGammaHist->SetBinContent(i, 0.5*(simGammaHist->GetBinContent(i)));
		simTotalEntries+=simTotalHist->GetBinContent(i);
		finalBinCounts[i] = simTotalHist->GetBinContent(i);
	}

	cout << "Simulation total entries (gamma + bckg) are: " << simTotalEntries << endl;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~PEAK FITTING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	TSpectrum *s = new TSpectrum();
	int peaksFound = 0;
	//simTotalHist->GetXaxis()->SetRangeUser(0,900);
	peaksFound = s->SearchHighRes(finalBinCounts, dest, simBinNumber, 6, 10, kTRUE, 100, kTRUE, 3);
	// above goes like (...,sigma,threshold,bckgRemove,iterations,markovON/OFF, averWindow)
	Double_t *xpeaks = s->GetPositionX();
	Double_t a;
	Int_t bin;
	Double_t fPositionX[100];
   	Double_t fPositionY[100];
	for (int i = 0; i < peaksFound; i++) 
	{
		a=xpeaks[i];
		bin = 1 + Int_t(a + 0.5);
		fPositionX[i] = simTotalHist->GetBinCenter(bin);
		fPositionY[i] = simTotalHist->GetBinContent(bin);
		
	}
	
	TPolyMarker * pm = (TPolyMarker*)simTotalHist->GetListOfFunctions()->FindObject("TPolyMarker");
	if (pm) 
	{
		simTotalHist->GetListOfFunctions()->Remove(pm);
		delete pm;
	}
	pm = new TPolyMarker(peaksFound, fPositionX, fPositionY);
	simTotalHist->GetListOfFunctions()->Add(pm);
	pm->SetMarkerStyle(23);
	pm->SetMarkerColor(kRed);
	pm->SetMarkerSize(1.3);
	
	for(int i=0; i < comparisonBinNumber+2; i++) deconvHist->SetBinContent(i, dest[i]);
	deconvHist->SetLineColor(kRed);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~BACKGROUND REMOVAL~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	TH1 *estimatedBackground = s->Background(comparisonHist, 50);
	Double_t bckgBinVal;
	Double_t comparisonBinVal;
	for (int i=0; i<comparisonBinNumber+2; i++)
	{
		bckgBinVal = estimatedBackground->GetBinContent(i);
		comparisonBinVal = comparisonHist->GetBinContent(i);
		if(comparisonBinVal - bckgBinVal >= 0)
		{
		comparisonNoBckg->SetBinContent(i, comparisonBinVal - bckgBinVal);
		}
	}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~GAUSSIAN FITSSS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//estimatedBackground->GetBinContent(estimatedBackground->FindBin((FitStart+FitEnd)/2))

	Double_t FitStart1 = 31;
	Double_t FitEnd1 = 35.2;	
	Double_t FitStart2 = 163.8;
	Double_t FitEnd2 = 167.8;


	//TF1 *f1 = new TF1("f1", "gausn(0) + gausn(3) + [6]", FitStart1, FitEnd1);
	//f1->SetParameters(40, 5.6, 0.15, 40, 6.2, 0.15, simBackgroundHist->GetBinContent((FitStart1+FitEnd1)/2));
	TF1 *f1 = new TF1("f1", "gausn(0)", FitStart1, FitEnd1);
	f1->SetParameters(40, 33.20, 0.25);
	f1->SetParLimits(1, 33.0, 33.5);
	f1->SetParLimits(2, 0.2, 5.0);
	//TF1 *f1 = new TF1("f1", "gausn(0) + [3]",FitStart, FitEnd);
	//f1->SetParameters(50, 778.917, 1.005, 5);

	TF1 *f2 = new TF1("f2", "gausn(0)",FitStart2, FitEnd2);
	f2->SetParameters(50, 165.86, 0.15);
	f2->SetParLimits(1, 165.4, 166.0); //165.86
	f2->SetParLimits(2, 0.2, 5.0);


	

	deconvHist->Fit(f1, "R","", FitStart1, FitEnd1);
	deconvHist->Fit(f2, "R","", FitStart2, FitEnd2);

	Double_t gaussHeight = f1->GetParameter(0);
	Double_t gaussMean = f1->GetParameter(1);
	Double_t gaussDev = f1->GetParameter(2);
	cout << "Height: " << gaussHeight << "\nMean: " << gaussMean << "\nDev: " << gaussDev << endl;
	//gaussHeight = f1->GetParameter(3);
	//gaussMean = f1->GetParameter(4);
	//gaussDev = f1->GetParameter(5);
	//cout << "Height: " << gaussHeight << "\nMean: " << gaussMean << "\nDev: " << gaussDev << endl;
	cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
	gaussHeight = f2->GetParameter(0);
	gaussMean = f2->GetParameter(1);
	gaussDev = f2->GetParameter(2);
	cout << "Height: " << gaussHeight << "\nMean: " << gaussMean << "\nDev: " << gaussDev << endl;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~INTEGRATION!!~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*int bmin = comparisonNoBckg->GetXaxis()->FindBin(117.5);
int bmax = comparisonNoBckg->GetXaxis()->FindBin(120.4);
cout << "Total Integral is: " << comparisonNoBckg->Integral(bmin,bmax) << endl;*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DRAWING EVERYTHING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	simTotalHist->GetXaxis()->SetTitle("Energy (keV)");
	simTotalHist->GetYaxis()->SetTitle("Counts");
	comparisonHist->GetXaxis()->SetTitle("Energy (keV)");
	comparisonHist->GetYaxis()->SetTitle("Counts");


	TCanvas *c1 = new TCanvas();
	simTotalHist->Draw();
	f1->SetLineStyle(1);
	f1->Draw("SAME");
	f2->SetLineStyle(1);
	f2->Draw("SAME");
	//deconvHist->Draw("SAME");
	string percentString = to_string(percentBckg);
	percentString.erase ( percentString.find_last_not_of('0') + 1, std::string::npos );
	percentString.erase ( percentString.find_last_not_of('.') + 1, std::string::npos );
	string entriesString = to_string(simGammaEntries);
	string fileName = percentString + "histo" + entriesString + ".png";
	c1->Print(fileName.c_str());


	/*TCanvas *c2 = new TCanvas();
	c2->cd();
	comparisonHist->Draw("L");
	//estimatedBackground->Draw("SAME");
	

	TCanvas *c3 = new TCanvas();
	c3->cd();
	simGammaHist->Draw();*/
}

