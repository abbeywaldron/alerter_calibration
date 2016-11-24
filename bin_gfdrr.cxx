#include <iostream>
#include <fstream>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TProfile2D.h>
#include <TCanvas.h>
#include <string>
#include <TLegend.h>
#include <TFile.h>





void bin_gfdrr(){
  
  TH2D* hour_tags = new TH2D("hour_tags","hour_tags",24,0,24,50,0,50);
  

  ifstream myfile ("../data/gfdrr-tagalog-small.txt");
  


  if( myfile.is_open() ){
    int hour, tag_count;
   
    while( myfile >>  hour >> tag_count){
      hour_tags->Fill(hour,tag_count);
    } 
  }



 
  TCanvas* c = new TCanvas("c","c");
  hour_tags->GetXaxis()->SetTitle("Hour (UTC)");
  hour_tags->GetXaxis()->CenterTitle();
  hour_tags->GetYaxis()->SetTitle("N Tags/10 min");
  hour_tags->GetYaxis()->CenterTitle();
  hour_tags->DrawCopy("colz");


  // output the histograms for fitting program
  TFile f("gfdrr_tagalog_small.root","recreate");
  hour_tags->Write();


  return;
}
