{
	//sadeh TStyle
	TStyle * sadehStyle = new  TStyle("sadehStyle", "sadehStyle");

	//set the background color to white
	sadehStyle->SetFillColor(10);
	sadehStyle->SetFrameFillColor(10);
	sadehStyle->SetCanvasColor(10);
	sadehStyle->SetPadColor(10);
	sadehStyle->SetTitleFillColor(10);
	sadehStyle->SetStatColor(10);

	//dont put a colored frame around the plots
	sadehStyle->SetFrameBorderMode(0);
	sadehStyle->SetCanvasBorderMode(0);
	sadehStyle->SetPadBorderMode(0);
	sadehStyle->SetLegendBorderSize(0);

	//set the color palette
	sadehStyle->SetPalette(1,0);

	//set the default line color for a histogram to be black
	sadehStyle->SetHistLineColor(kBlue+3);

	//set the default line color/style for a fit function
	sadehStyle->SetFuncColor(kRed);
	sadehStyle->SetFuncStyle(1);

	//make the axis labels black
	sadehStyle->SetLabelColor(kBlack,"xyz");

	//set the default title color to be black
	sadehStyle->SetTitleColor(kBlack);
 
	//set the margins
	sadehStyle->SetPadBottomMargin(0.15);
	sadehStyle->SetPadTopMargin(0.15);
	sadehStyle->SetPadRightMargin(0.15);
	sadehStyle->SetPadLeftMargin(0.15);

  //set axis label and title text sizes
  sadehStyle->SetTitleOffset(1.4,"yz");
  sadehStyle->SetTitleOffset(1.2,"x");
  sadehStyle->SetTitleSize(0.05,"x");
  sadehStyle->SetTitleSize(0.05,"yz");

	//set line widths
	sadehStyle->SetFrameLineWidth(1);
	sadehStyle->SetFuncWidth(3);
	sadehStyle->SetHistLineWidth(3);

	//set the number of divisions to show
	sadehStyle->SetNdivisions(506, "xy");

	//turn off xy grids
	sadehStyle->SetPadGridX(0);
	sadehStyle->SetPadGridY(0);

	//set the tick mark style
	sadehStyle->SetPadTickX(1);
	sadehStyle->SetPadTickY(1);

	//turn on/off stats
	sadehStyle->SetOptStat(0);
	sadehStyle->SetOptTitle(0);
	sadehStyle->SetOptFit(0111);
         sadehStyle->SetFitFormat("1.1e");

	//marker settings
	sadehStyle->SetMarkerStyle(22);
	sadehStyle->SetMarkerSize(1.2);
	sadehStyle->SetMarkerColor(kBlue+3);

  //surrounding box
  sadehStyle->SetLineWidth(1); 

  // unmber of axis digits
  //TGaxis::SetMaxDigits(3);

	//done
	sadehStyle->cd();
	gROOT->ForceStyle();
}
