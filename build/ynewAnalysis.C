//Like newAnalysis.C but with background
void ynewAnalysis()
{
//-----------------------------------------------------------------------------------------------
//--------------------------------DECLARING HISTOGRAMS-------------------------------------------
	int comparisonBinNumber = 1000;	//922
	int simBinNumber = 1000;

	TH1F *simGammaHist = new TH1F("simGammaHist", "Only Gamma Simulated", simBinNumber, 0, 700); 
	TH1F *deconvHist = new TH1F("deconvHist", "Deconvoluted Histogram", comparisonBinNumber, 0, 700);
	TH1F *simHist = new TH1F("simHist", "Simulated Histogram", simBinNumber, 0, 700);
	TH1F *bckgHist = new TH1F("bckgHist", "Background Histogram", simBinNumber, 0, 700);	
	//TH1F *bHist = new TH1F ("bHist", "Simulated Background Histogram", simBinNumber, 0, 700);
	//TH1F *noBckgHist = new TH1F("noBckgHist", "Background Removed Histogram", simBinNumber, 0, 700);
//-----------------------------------------------------------------------------------------------
//--------------------------------OPENING FILES--------------------------------------------------
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

	file.open("realBackground.txt", ios::in);
	while(1)
	{
		file >> value;
		bckgHist->Fill(value);
		if(file.eof()) break;
	}
	file.close();

	cout <<"third message" << endl;
	
//-----------------------------------------------------------------------------------------------
//--------------------------------------GETTING ENTRIES------------------------------------------
	int simGammaEntries = 0;
	int simBckgEntries = 0;
	int simEntries = 0;
	double bckgCounts[simBinNumber];

	for (int i=0; i<simBinNumber+2; i++)
	{
		simGammaEntries+=simGammaHist->GetBinContent(i);
	}
	cout << "Simulation gamma entries are: " << simGammaEntries << endl;
	
	for (int i=0; i<simBinNumber+2; i++)
	{
		simBckgEntries+=bckgHist->GetBinContent(i);
		bckgCounts[i] = bckgHist->GetBinContent(i);
	}

//-----------------------------------------------------------------------------------------------
//------------------------------ BACKGROUND SCALING----------------------------------------------
	double prctScaling = 0.01; 
	double bckgScaling = (prctScaling/(1-prctScaling))*((double)simGammaEntries/(double)simBckgEntries);
	int targetEntries = bckgScaling*simBckgEntries;
	cout << "Target entries are: " << targetEntries << endl;

	int n=0;
	while(1)
	{
		simBckgEntries = 0;
		for (int i=0; i<simBinNumber+2; i++)
		{
			bckgHist->SetBinContent(i, (bckgScaling+0.001*n)*(bckgHist->GetBinContent(i)));
			simBckgEntries+=bckgHist->GetBinContent(i);
		}
		
		if (simBckgEntries > targetEntries) break;
		
		for (int i=0;i<simBinNumber+2;i++)
		{
			bckgHist->SetBinContent(i,bckgCounts[i]);
			simBckgEntries+=bckgHist->GetBinContent(i);
		}
		
		n++;
	}	

cout << "Scaled background counts are: " << simBckgEntries << endl;
//-----------------------------------------------------------------------------------------------
//--------------------------------------ADDING BACKGROUND TO GAMMA HIST--------------------------
	int countsBeforeBckg = 0;
	int finalBckgCounts = 0;
	double finalBinCounts[simBinNumber];
	double dest[simBinNumber];

	for(int i=0;i<simBinNumber+2;i++)
	{
		countsBeforeBckg = simGammaHist->GetBinContent(i);
		finalBckgCounts = bckgHist->GetBinContent(i);
		simHist->SetBinContent(i, countsBeforeBckg+finalBckgCounts);
		simEntries+=simHist->GetBinContent(i);
		finalBinCounts[i] = simHist->GetBinContent(i);		
	}

	cout << "Simulation total entries are: "<< simEntries << endl;

//-----------------------------------------------------------------------------------------------
//--------------------------------RMV BCKGROUND--------------------------------------------------
	double source[simBinNumber];	
	TH1 *bHist = new TH1F ("bHist", "Simulated Background Histogram", simBinNumber, 0, 700);
	TH1F *noBckgHist = (TH1F*)simHist->Clone("noBckgHist");
	
	for (int i=0;i<simBinNumber;i++) source[i] = simHist->GetBinContent(i);
	
	TSpectrum *spec = new TSpectrum();	
	bHist = spec->Background(simHist, 10, ""); 
	noBckgHist->Add(bHist, -1.0);
	//for (int i=0;i<simBinNumber;i++) bHist->SetBinContent(i,source[i]);

	double noBckgBinCounts[simBinNumber];
	for (int i=0;i<simBinNumber;i++)
	{
		noBckgBinCounts[i] = noBckgHist->GetBinContent(i);
	}


//-----------------------------------------------------------------------------------------------
//------------------------------------PEAK FINDING-----------------------------------------------
	
	TSpectrum *s = new TSpectrum();
	int peaksFound = 0;
	peaksFound = s->SearchHighRes(noBckgBinCounts, dest, simBinNumber, 1, 1, kTRUE, 100, kTRUE, 4);
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
		fPositionX[i] = noBckgHist->GetBinCenter(bin);
		fPositionY[i] = noBckgHist->GetBinContent(bin);
		
	}
	
	TPolyMarker * pm = (TPolyMarker*)noBckgHist->GetListOfFunctions()->FindObject("TPolyMarker");
	if (pm) 
	{
		noBckgHist->GetListOfFunctions()->Remove(pm);
		cout << "hhh" << endl;
		delete pm;
	}

	pm = new TPolyMarker(peaksFound, fPositionX, fPositionY);
	noBckgHist->GetListOfFunctions()->Add(pm);
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
	//Double_t x3 = x[9];
	Double_t y1 = y[0] +1000;
	Double_t y2 = y[1] +1000;
	//Double_t y3 = y[9];

	cout << "x1: " << x1 << endl;
	cout << "x2: " << x2 << endl;
	//cout << "x3: " << x3 << endl;
	cout << "y1: " << y1 << endl;
	cout << "y2: " << y2 << endl;
	//cout << "y3: " << y3 << endl;

//set up everything in a for loop for gaussian fits?



//-----------------------------------------------------------------------------------------------
//-----------------------------------GAUSSIAN FITS-----------------------------------------------	

	Double_t FitStart1 = x1-10; //+/- 10 seems to work well
	Double_t FitEnd1 = x1+10;
	Double_t FitStart2 = x2-10;
	Double_t FitEnd2 = x2+10;
	/*Double_t FitStart3 = 627-10;
	Double_t FitEnd3 = 627+10;*/

	TF1 *gaus1 = new TF1("gaus1", "gausn(0)", FitStart1, FitEnd1);
	gaus1->SetParameters(y1, x1, 0.4); 
	gaus1->SetParLimits(1, x1-5, x1+5); //To change too. seems to work now though
	gaus1->SetParLimits(2, 0.1, 10);

	TF1 *gaus2 = new TF1("gaus2", "gaus(0)", FitStart2, FitEnd2);
	gaus2->SetParameters(y2, x2, 0.15);
	//gaus2->SetParLimits(1, x2-10, x2+10);
	//gaus2->SetParLimits(2, 1, 2);

	/*TF1 *gaus3 = new TF1("gaus3", "gaus(0)", FitStart3, FitEnd3);
	gaus3->SetParameters(850, 627, 0.25);
	gaus3->SetParLimits(1, 627-10, 627+10);
	gaus3->SetParLimits(2, 0.2,0.5);*/

	//deconvHist->Fit(gaus1, "R", "L", FitStart1, FitEnd1);
	//deconvHist->Fit(gaus2, "R", "L", FitStart2, FitEnd2); 
	//deconvHist->Fit(gaus3, "R", "L", FitStart3, FitEnd3);

	Double_t gaussHeight1 = gaus1->GetParameter(0);
	Double_t gaussMean1 = gaus1->GetParameter(1);
	Double_t gaussDev1 = gaus1->GetParameter(2);

	Double_t gaussHeight2 = gaus2->GetParameter(0);
	Double_t gaussMean2 = gaus2->GetParameter(1);
	Double_t gaussDev2 = gaus2->GetParameter(2);

	/*Double_t gaussHeight3 = gaus3->GetParameter(0);
	Double_t gaussMean3 = gaus3->GetParameter(1);
	Double_t gaussDev3 = gaus3->GetParameter(2);*/
	
	cout << "For gaus1, height--mean--dev: " << gaussHeight1 << "--" << gaussMean1 << "--" << gaussDev1 << endl;
	cout << "For gaus2, height--mean--dev: " << gaussHeight2 << "--" << gaussMean2 << "--" << gaussDev2 << endl;
	//cout << "For gaus3, height--mean--dev: " << gaussHeight3 << "--" << gaussMean3 << "--" << gaussDev3 << endl;

//-----------------------------------------------------------------------------------------------
//-----------------------------------INTEGRAL: COUNTS--------------------------------------------
	Double_t counts1 = gaus1->Integral(x1-10,x1+10);
	Double_t counts2 = gaus2->Integral(x2-10,x2+10);
	///Double_t counts3 = gaus3->Integral(627-10,627+10);
	cout << "Counts 1 is: " << counts1 << endl;
	cout << "Counts 2 is: " << counts2 << endl;
	//cout << "Counts 3 is: " << counts3 << endl;

	/*Double_t error1 = gaus1->IntegralError(x1-10,x1+10);
	Double_t error2 = gaus2->IntegralError(x2-10,x2+10);
	Double_t error3 = gaus3->IntegralError(627-10, 627+10);
	cout << "Error 1 is: " << error1 << endl;
	cout << "Error 2 is: " << error2 << endl;
	cout << "Error 3 is: " << error3 << endl;*/
//------------------------------------------------------------------------------------------------
//-----------------------------------DRAWING EVERYTHING-------------------------------------------
	simGammaHist->GetXaxis()->SetTitle("Energy (keV)");
	simGammaHist->GetYaxis()->SetTitle("Counts");

	simHist->GetXaxis()->SetTitle("Energy (keV)");
	simHist->GetYaxis()->SetTitle("Counts");

	Double_t max= noBckgHist->GetMaximum();

	TCanvas *graph4 = new TCanvas();
	simHist->Draw();
	bHist->SetLineColor(kMagenta);
	bHist->Draw("SAME");

	TCanvas *graph3 = new TCanvas();
	noBckgHist->SetAxisRange(0., max + 1500, "Y");	
	noBckgHist->Draw();
	noBckgHist->SetLineColor(kMagenta);
	/*gaus1->SetLineStyle(1);	
	gaus1->SetLineColor(1);
	gaus1->Draw("SAME");
	gaus2->SetLineStyle(1);	
	gaus2->SetLineColor(1);
	gaus2->Draw("SAME");*/
	/*gaus3->SetLineStyle(1);
	gaus3->SetLineColor(1);
	gaus3->Draw("SAME");*/

	//TCanvas *graph1 = new TCanvas();
	//simGammaHist->Draw();

	/*TCanvas *graph2 = new TCanvas();
	graph2->cd();
	deconvHist->GetXaxis()->SetTitle("Energy (keV)");
	deconvHist->GetYaxis()->SetTitle("Counts");
	deconvHist->Draw();*/
	//gaus3->SetLineStyle(1);
	//gaus3->SetLineColor(1);
	//gaus3->Draw("SAME");

}

