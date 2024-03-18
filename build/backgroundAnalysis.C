void backgroundAnalysis()
{
	Double_t binNumber = 1000;
	TH1F *backgroundHist = new TH1F("backgroundHist", "Real Background", binNumber, 0, 922);


	Double_t value;
	fstream file;
	file.open("gammaData.txt", ios::in);
	while(1)
	{
		file >> value;
		backgroundHist->Fill(value);
		if(file.eof()) break;
	}	
	file.close();

	TCanvas *c1 = new TCanvas();
	backgroundHist->Draw();

}
