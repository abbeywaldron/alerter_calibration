#include <iostream>
#include <fstream>
#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TLegend.h>
#include <TText.h>
#include <TAttFill.h>


Double_t myPoisson(Double_t *x, Double_t *par){

  // Poisson
  Double_t k = x[0];
  Double_t res = par[0]*TMath::Power((par[1]/par[2]),k/par[2]);
  res *= TMath::Exp(-(par[1]/par[2]));
  res /= TMath::Gamma((k/par[2])+1);
  
  return res;
}

Double_t myGaussian(Double_t *x, Double_t *par){

  // Gaussian
  Double_t k = x[0];
  Double_t gaus = par[0]*TMath::Gaus(k,par[1],par[2]);

  return gaus;
}


double getQuant(double prob){
  TF1* f = new TF1("f","TMath::Gaus(x,0,1)",-20,20);
  

  // compute the quantile (corresponding to <prob> LHS)
  Double_t quantiles[1];
  const Double_t probSums[1] = {prob};
  Int_t nProbSum = 1;
  f->GetQuantiles(nProbSum,&quantiles[0],&probSums[0]);
  
  return quantiles[0];
}


double getProb(double n){
  TF1* f = new TF1("f","TMath::Gaus(x,0,1)",-10,10);
  TCanvas* c3 = new TCanvas("c3","c3",1);
  //c3->SetLogy();
  f->DrawCopy();
  double denom = f->Integral(-10.,10.);
  double prob = f->Integral(-10.,n);
  return prob/denom;
}


void myFit(TH1D* &hist, double &mean, double &mean_error, double &sigma, double &q95, double &q3, double &q4, double &q6){

  // no range restrictions on the fit function if possible?
  //  TF1* f = new TF1("f",myGaussian,0,hist->GetXaxis()->GetXmax(),3);
  TF1* f = new TF1("f",myPoisson,0,hist->GetXaxis()->GetXmax()+100.,3);
  //TF1* f = new TF1("f","[0]*TMath::Poisson(TMath::Gaus(x,0,[2]),[1])",0,hist->GetXaxis()->GetXmax());
  //TF1* f = new TF1("f","[0]*TMath::Poisson(x,[1])",0,hist->GetXaxis()->GetXmax());
  
  f->SetParameter(0,hist->GetMaximum());
  f->SetParameter(1,hist->GetMean());
  f->SetParameter(2,hist->GetRMS());
  // f->SetParameter(2,hist->GetMean()/hist->GetMaximum());
  //  f->SetParameter(2,hist->GetMeanError());
  
  hist->Fit("f","");


  mean = f->GetParameter(1);
  mean_error = f->GetParError(1);
  sigma = f->GetParameter(2);
  //sigma = f->GetParameter(1); // Poisson...  


  std::cout << "Fitted rate: " << mean <<  " p/m "
	    << sigma << std::endl;


  std::cout << "PROBABILITY: " << getProb(4.) << std::endl;
  std::cout << "QUANTILE: " << getQuant(0.99999999901) << std::endl;

  // compute the quantiles (corresponding to 0.9987, 0.99997 LHS)
  Double_t quantiles[3];
  const Double_t probSums[4] = {0.95,0.9987, 0.99997, 0.99999999901};
  Int_t nProbSum = 4;
  f->GetQuantiles(nProbSum,&quantiles[0],&probSums[0]);

  q95 = quantiles[0];
  q3 = quantiles[1];
  q4 = quantiles[2];
  q6 = quantiles[3];


  return;
}



void fit_gfdrr(){

  TFile f("gfdrr_tagalog_small.root","read");
  TH2D* hour_tags = (TH2D*)f.Get("hour_tags");


  // ---------- fitting ------------
  const int n_hours = 24;
  TH1D* proj[n_hours];
  double mean[n_hours];
  double mean_error[n_hours];
  double sigma[n_hours];
  double q95[n_hours];
  double q3[n_hours];
  double q4[n_hours];
  double q6[n_hours];

  for( int i=0; i<n_hours; ++i ){
    char name[100];
    sprintf(name,"proj_%i",i+1);
    proj[i] = hour_tags->ProjectionY(name,i+1,i+1);
    myFit(proj[i],mean[i],mean_error[i],sigma[i],q95[i],q3[i],q4[i],q6[i]);
  }

  TCanvas* c = new TCanvas("c","c",1200,800);
  c->Divide(6,4);
 
  
  // check the fits
  TH1D* means = new TH1D("means","means",24,0,24);
  TH1D* sigmas = new TH1D("sigmas","sigmas",24,0,24);
  TH1D* ranges = new TH1D("ranges","ranges",24,0,24);
  TH1D* thresholds = new TH1D("thresholds","thresholds",24,0,24);
  TH1D* low_thresholds = new TH1D("low_thresholds","low_thresholds",24,0,24);
  TH1D* mid_thresholds = new TH1D("mid_thresholds","mid_thresholds",24,0,24);



  for( int i=0; i<n_hours; ++i ){
    c->cd(i+1);
    //gPad->SetLogy(1);
    char title[100];
    sprintf(title,"%i-%i h",i,i+1);
    TText text(0.5,0.7,title);
    char stat_text[100];
    sprintf(stat_text,"%.2f",mean[i]);
    std::cout << mean[i] << " " << mean_error[i] << " " 
	      << q3[i] << " " << q4[i] << " " << q6[i] << std::endl;
    TText stat(0.5,0.6,stat_text);
    text.SetNDC();
    stat.SetNDC();
    text.SetTextSize(0.1);
    stat.SetTextSize(0.1);
    proj[i]->DrawCopy("");
    text.DrawClone();
    stat.DrawClone();
    ranges->SetBinContent(i+1,q95[i]);
    means->SetBinContent(i+1,mean[i]);
    sigmas->SetBinContent(i+1,sigma[i]);
    thresholds->SetBinContent(i+1,q6[i]);
    low_thresholds->SetBinContent(i+1,q3[i]);
    mid_thresholds->SetBinContent(i+1,q4[i]);
  }


  TCanvas* c2 = new TCanvas("c2","c2");
  means->GetXaxis()->SetTitle("Hour of Day");
  means->GetXaxis()->CenterTitle();
  means->GetYaxis()->SetTitle("N Tags/10 min");
  means->GetYaxis()->CenterTitle();
  means->GetYaxis()->SetRangeUser(0.,50.);
  means->SetLineColor(2);
  means->DrawCopy();
  //  sigmas->SetLineColor(2);
  // sigmas->DrawCopy("same");
  //  ranges->SetFillColor(2);
  ranges->SetFillColor(2);
  ranges->SetLineColor(0);
  ranges->SetFillStyle(3004);
  ranges->DrawCopy("same");
  thresholds->DrawCopy("same");
  low_thresholds->DrawCopy("same");
  //mid_thresholds->DrawCopy("same");

  TLegend leg(0.6,0.7,0.82,0.82);
  leg.AddEntry(means,"Mean","l");
  leg.AddEntry(ranges,"95% CL","f");
  leg.AddEntry(thresholds,"Threshold","l");
  leg.DrawClone();


  TFile output("fit.root","recreate");
  c->Write();
  means->Write();
  sigmas->Write();
  ranges->Write();


  // write output values to text file
  ofstream outfile;
  outfile.open("gfdrr_calibration_tagalog_small.txt");
  for( int i=0; i<24; ++i ){
    //   outfile << i << " " << mean[i] << " "
    //    << q95[i] << " " << floor(q6[i]) << std::endl;
    outfile << i << " " 
      //	    << mean[i] << " " << q95[i] << " "
	    << floor(q3[i]) << " "
	    << floor(q6[i]) << std::endl;
  }  
  outfile.close();
  

  return;
}
