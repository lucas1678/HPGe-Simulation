void newAnalysis()
{
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DECLARING HISTOGRAMS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	int comparisonBinNumber = 1000;	//922
	int simBinNumber = 1000;

	TH1F *simGammaHist = new TH1F("simGammaHist", "Only Gamma Simulated", simBinNumber, 0, 1000); 
	TH1F *deconvHist = new TH1F("deconvHist", "deconvHist", comparisonBinNumber, 0, 1000);
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~OPENING FILES~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	double value;
	fstream file;
	cout << "first message" << endl;

	file.open("gammaData.txt", ios::in);
	while(1)
	{
		file >> value;
		simGammaHist->Fill(value);
		if(file.eof()) break;
	}
	file.close();

	cout << "second message" << endl;
	
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~GETTING ENTRIES~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	int simGammaEntries = 0;

	for (int i=0; i<simBinNumber+2; i++)
	{
		simGammaEntries+=simGammaHist->GetBinContent(i);
	}
	cout << "Simulation gamma entries are: " << simGammaEntries << endl;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ GAMMA DATA~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	double finalBinCounts[simBinNumber];
	double dest[simBinNumber];
	
	for (int i=0; i<simBinNumber+2; i++)
	{
		finalBinCounts[i] = simGammaHist->GetBinContent(i);
	}

	cout << "Success " << endl;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~PEAK FINDING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	TSpectrum *s = new TSpectrum();
	int peaksFound = 0;
	peaksFound = s->SearchHighRes(finalBinCounts, dest, simBinNumber, 2, 6, kTRUE, 100, kTRUE, 6);
	// above goes like (...,sigma,threshold,bckgRemove,iterations,markovON/OFF, averWindow)
	Double_t *xpeaks = s->GetPositionX();
	Double_t a;
	Int_t bin;
	Double_t fPositionX[100];
   	Double_t fPositionY[100];
	for (int i = 0; i < peaksFound; i++) 
	{
		a=xpeaks[i];
		bin = Int_t(a + 0.5);
		fPositionX[i] = simGammaHist->GetBinCenter(bin);
		fPositionY[i] = simGammaHist->GetBinContent(bin);
		
	}
	
	TPolyMarker * pm = (TPolyMarker*)simGammaHist->GetListOfFunctions()->FindObject("TPolyMarker");
	if (pm) 
	{
		simGammaHist->GetListOfFunctions()->Remove(pm);
		cout << "hhh" << endl;
		delete pm;
	}

	pm = new TPolyMarker(peaksFound, fPositionX, fPositionY);
	simGammaHist->GetListOfFunctions()->Add(pm);
	pm->SetMarkerStyle(23);
	pm->SetMarkerColor(kRed);
	pm->SetMarkerSize(1.3);

	for(int i=0; i < comparisonBinNumber+2; i++) deconvHist->SetBinContent(i, dest[i]);
	deconvHist->SetLineColor(kRed);

	Double_t* x = pm->GetX();
	Double_t* y = pm->GetY();

	for(Int_t i=0;i<peaksFound;i++) 
	{
   	cout << "X Position " << i+1 << " " << x[i] << endl ; // print Polymarkers position //letsgoooo
	cout << "Y Position " << i+1 << " " << y[i] << endl;
	}
	
	Double_t x1 = x[0];
	Double_t x2 = x[1];
	Double_t x3 = x[9];
	Double_t y1 = y[0];
	Double_t y2 = y[1];
	Double_t y3 = y[9];

	cout << "x1: " << x1 << endl;
	cout << "x2: " << x2 << endl;
	cout << "x3: " << x3 << endl;
	cout << "y1: " << y1 << endl;
	cout << "y2: " << y2 << endl;
	cout << "y3: " << y3 << endl;

//set up everything in a for loop for gaussian fits?



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~GAUSSIAN FITS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Double_t FitStart1 = x1-10; //+/- 10 seems to work well
	Double_t FitEnd1 = x1+10;
	Double_t FitStart2 = x2-10;
	Double_t FitEnd2 = x2+10;
	Double_t FitStart3 = x3-10;
	Double_t FitEnd3 = x3+10;

	TF1 *gaus1 = new TF1("gaus1", "gausn(0)", FitStart1, FitEnd1);
	gaus1->SetParameters(y1, x1, 0.25); 
	gaus1->SetParLimits(1, x1-10, x1+10); //To change too. seems to work now though
	gaus1->SetParLimits(2, 0.2, 5.0);//i don't think i need to change that, stdev is the same

	TF1 *gaus2 = new TF1("gaus2", "gaus(0)", FitStart2, FitEnd2);
	gaus2->SetParameters(y2, x2, 0.15);
	gaus2->SetParLimits(1, x2-10, x2+10);
	gaus2->SetParLimits(2, 0.2, 0.5);

	TF1 *gaus3 = new TF1("gaus3", "gaus(0)", FitStart3, FitEnd3);
	gaus3->SetParameters(y1, x1, 0.25);
	gaus3->SetParLimits(1, x3-10, x3+10);
	gaus3->SetParLimits(2, 0.2,0.5);

	deconvHist->Fit(gaus1, "R", "L", FitStart1, FitEnd1);
	deconvHist->Fit(gaus2, "R", "L", FitStart2, FitEnd2); 
	deconvHist->Fit(gaus3, "R", "L", FitStart3, FitEnd3);

	Double_t gaussHeight1 = gaus1->GetParameter(0);
	Double_t gaussMean1 = gaus1->GetParameter(1);
	Double_t gaussDev1 = gaus1->GetParameter(2);

	Double_t gaussHeight2 = gaus2->GetParameter(0);
	Double_t gaussMean2 = gaus2->GetParameter(1);
	Double_t gaussDev2 = gaus2->GetParameter(2);

	Double_t gaussHeight3 = gaus3->GetParameter(0);
	Double_t gaussMean3 = gaus3->GetParameter(1);
	Double_t gaussDev3 = gaus3->GetParameter(2);
	
	cout << "For gaus1, height--mean--dev: " << gaussHeight1 << "--" << gaussMean1 << "--" << gaussDev1 << endl;
	cout << "For gaus2, height--mean--dev: " << gaussHeight2 << "--" << gaussMean2 << "--" << gaussDev2 << endl;
	cout << "For gaus3, height--mean--dev: " << gaussHeight3 << "--" << gaussMean3 << "--" << gaussDev3 << endl;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~INTEGRAL: COUNTS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	Double_t counts1 = gaus1->Integral(x1-10,x1+10);
	Double_t counts2 = gaus2->Integral(x2-10,x2+10);
	Double_t counts3 = gaus3->Integral(x3-10,x3+10);
	cout << "Counts 1 is: " << counts1 << endl;
	cout << "Counts 2 is: " << counts2 << endl;
	cout << "Counts 3 is: " << counts3 << endl;

	Double_t error1 = gaus1->IntegralError(x1-10,x1+10);
	Double_t error2 = gaus2->IntegralError(x2-10,x2+10);
	Double_t error3 = gaus3->IntegralError(x3-10, x2+10);
	cout << "Error 1 is: " << error1 << endl;
	cout << "Error 2 is: " << error2 << endl;
	cout << "Error 3 is: " << error3 << endl;
	
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DRAWING EVERYTHING~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	simGammaHist->GetXaxis()->SetTitle("Energy (keV)");
	simGammaHist->GetYaxis()->SetTitle("Counts");

	//TCanvas *graph1 = new TCanvas(); //his just makes an uglier version of deconvHist. idk why
	//graph1->cd();
	simGammaHist->Draw();

	TCanvas *graph2 = new TCanvas();
	graph2->cd();
	deconvHist->GetXaxis()->SetTitle("Energy (keV)");
	deconvHist->GetYaxis()->SetTitle("Counts");
	deconvHist->Draw();
	gaus1->SetLineStyle(1);	
	gaus1->SetLineColor(1);
	gaus1->Draw("SAME");
	gaus2->SetLineStyle(1);	
	gaus2->SetLineColor(1);
	gaus2->Draw("SAME");
	gaus3->SetLineStyle(1);
	gaus3->SetLineColor(1);
	gaus3->Draw("SAME");

}

