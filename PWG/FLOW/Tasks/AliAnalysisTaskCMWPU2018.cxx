 /**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/
/* $Id: AliAnalysisTaskCMW.cxx  Rihan Haque, 18/09/2019 (ver1) $ */
/* This class is derived from AliAnalysisTaskCVE           */
/* This analysis Task contains trivial corrections to CMW */

//-- general include---
#include "TChain.h"
#include "TTree.h"
#include "TGrid.h"
#include "TROOT.h"
#include "TArrayI.h"
#include "TObjArray.h"
#include "TMatrixDSym.h"
#include "TParticle.h"
#include "TMath.h"
#include "stdio.h"
#include "Riostream.h"




//---- manager and handler---
#include "AliAnalysisManager.h"
#include "AliInputEventHandler.h"

//---V0 and ZDC info---
#include "AliAODZDC.h"
#include "AliAODVZERO.h"
#include "AliAODVertex.h"

//---AOD,ESD event--
#include "AliESDEvent.h"
#include "AliAODHeader.h"
#include "AliAODEvent.h"
#include "AliAODTrack.h"
#include "AliTimeRangeCut.h"

//----- For MC event------
#include "AliMCEvent.h"
#include "AliStack.h"
#include "AliAODMCParticle.h"
#include "AliAODMCHeader.h"

//----for PID-----
#include "AliPIDResponse.h"
#include "AliPIDCombined.h"

//----- Vevent and tracks
#include "AliVEventHandler.h"
#include "AliVEvent.h"
#include "AliVTrack.h"
#include "AliVParticle.h"
#include "AliCentrality.h"


//----- must include-------
#include "AliMultSelection.h"
#include "AliAnalysisUtils.h"
#include "AliPhysicsSelection.h"
#include "AliFlowEventSimple.h"
#include "AliAnalysisTaskSE.h"
#include "AliAnalysisTaskCMWPU2018.h"

using std::cout;
using std::endl;
using std::vector;


ClassImp(AliAnalysisTaskCMWPU2018)

AliAnalysisTaskCMWPU2018::AliAnalysisTaskCMWPU2018(const char *name): AliAnalysisTaskSE(name),
  fVevent(NULL),
  fESD(NULL),
  fAOD(NULL),
  fPIDResponse(NULL),
  fMultSelection(NULL),
  fAnalysisUtil(NULL),
  fListHist(NULL),
  fListTRKCorr(NULL),
  fListNUACorr(NULL),
  fListV0MCorr(NULL),  
  fCentralityMin(0),
  fCentralityMax(90),
  gHarmonic(2),
  fParticle(1),
  fFilterBit(1),
  fTPCclustMin(70),
  bUseKinkTracks(kFALSE),
  fNSigmaTPCCut(2.0),
  fNSigmaTOFCut(2.0),
  fMinPtCut(0.2),
  fMaxPtCut(5.0),
  fDCAxyMax(2.4),
  fDCAzMax(3.2),
  fChi2(4.0),
  fPileUpSlopeParm(3.43),
  fPileUpConstParm(43),
  bSkipPileUpCut(kFALSE), 
  fEtaGapNeg(-0.1),
  fEtaGapPos(0.1),
  fMinEtaCut(-0.8),
  fMaxEtaCut(0.8),
  fTrkChi2Min(0.1),    
  fdEdxMin(10.0),
  fMinVzCut(-10.0),
  fMaxVzCut(10.0),
  sCentrEstimator("V0M"),
  fCentDistBeforCut(NULL),
  fCentDistAfterCut(NULL),
  fHistEtaPtBeforCut(NULL),
  fHistEtaPhiBeforCut(NULL),
  fHistEtaPhiAfterCut(NULL),  
  fHCorrectMCposChrg(NULL),
  fHCorrectMCposPion(NULL),
  fHCorrectMCposKaon(NULL),
  fHCorrectMCposProt(NULL),
  fHCorrectMCnegChrg(NULL),
  fHCorrectMCnegPion(NULL),
  fHCorrectMCnegKaon(NULL),
  fHCorrectMCnegProt(NULL),      
  fHistAChrgVsCent(NULL),

  fHistTPConlyVsCL1Before(NULL),
  fHistTPConlyVsV0MBefore(NULL),
  fHistCL0VsV0MBefore(NULL),  
  fHistTPConlyVsCL1After(NULL),
  fHistTPConlyVsV0MAfter(NULL),
  fHistCL0VsV0MAfter(NULL),
  fHistGlobalVsV0MBefore(NULL),
  fHistGlobalVsV0MAfter(NULL),
  fTPCvsGlobalTrkBefore(NULL),
  fTPCvsGlobalTrkAfter(NULL),
  fHistTPCVsESDTrkBefore(NULL),
  fHistTPCVsESDTrkAfter(NULL),

  fHistPileUpCount(NULL),
  fHistEventCount(NULL),
  fHCorrectEVNTWGTChrg(NULL)
  
{

  for(int i=0;i<2;i++){
    for(int j=0;j<10;j++){
      fHistv2AchChrgPos[i][j] = NULL;
      fHistv2AchKaonPos[i][j] = NULL;    
      fHistv2AchProtPos[i][j] = NULL;      
      fHistv2AchPionPos[i][j] = NULL;

      fHistv2AchChrgPosChrgNeg[i][j] = NULL;
      fHistv2AchPionPosPionNeg[i][j] = NULL;
      fHistv2AchKaonPosKaonNeg[i][j] = NULL;
      fHistv2AchProtPosProtNeg[i][j] = NULL;
      
      fHistv2AchChrgNeg[i][j] = NULL;
      fHistv2AchPionNeg[i][j] = NULL;      
      fHistv2AchKaonNeg[i][j] = NULL;
      fHistv2AchProtNeg[i][j] = NULL;

      fHistv2AchChrgNegChrgPos[i][j] = NULL;
      fHistv2AchPionNegPionPos[i][j] = NULL;
      fHistv2AchKaonNegKaonPos[i][j] = NULL;
      fHistv2AchProtNegProtPos[i][j] = NULL;
    }
  }
  
  for(int i=0; i<5; i++){
    fHCorrectNUAposChrg[i] = NULL;  
    fHCorrectNUAnegChrg[i] = NULL;  
    fHCorrectNUAposPion[i] = NULL;  
    fHCorrectNUAnegPion[i] = NULL;  
    fHCorrectNUAposKaon[i] = NULL;  
    fHCorrectNUAnegKaon[i] = NULL;  
    fHCorrectNUAposProt[i] = NULL;  
    fHCorrectNUAnegProt[i] = NULL;    
  }
  

  for(int i=0; i<5; i++){
    fHFillNUAPosPID[i]  = NULL;
    fHFillNUANegPID[i]  = NULL; 
  }

  for(int i=0; i<2; i++){
    fHistEPResolution[i] = NULL;
  }

  for(int i=0; i<10; i++){
    fHistEPResolutionAch[i] = NULL;
  }
  
  for(int i=0; i<10; i++){
    fHistv2cumAchChrgAll[i] = NULL;
  }
  
  //Must be here:
  DefineInput(0,TChain::Class());
  DefineOutput(1,TList::Class());
}

//_______________________empty constructor_______________________
AliAnalysisTaskCMWPU2018::AliAnalysisTaskCMWPU2018():
  fVevent(NULL),
  fESD(NULL),
  fAOD(NULL),
  fPIDResponse(NULL),
  fMultSelection(NULL),
  fAnalysisUtil(NULL),
  fListHist(NULL),
  fListTRKCorr(NULL),
  fListNUACorr(NULL),
  fListV0MCorr(NULL),
  fCentralityMin(0),
  fCentralityMax(90),
  gHarmonic(2),
  fParticle(1),
  fFilterBit(1),
  fTPCclustMin(70),
  bUseKinkTracks(kFALSE),
  fNSigmaTPCCut(2.0),
  fNSigmaTOFCut(2.0),
  fMinPtCut(0.2),
  fMaxPtCut(5.0),
  fDCAxyMax(2.4),
  fDCAzMax(3.2),
  fChi2(4.0),
  fPileUpSlopeParm(3.43),
  fPileUpConstParm(43),
  bSkipPileUpCut(kFALSE), 
  fEtaGapNeg(-0.1),
  fEtaGapPos(0.1),
  fMinEtaCut(-0.8),
  fMaxEtaCut(0.8),
  fTrkChi2Min(0.1),    
  fdEdxMin(10.0),
  fMinVzCut(-10.0),
  fMaxVzCut(10.0),
  sCentrEstimator("V0M"),
  fCentDistBeforCut(NULL),
  fCentDistAfterCut(NULL),
  fHistEtaPtBeforCut(NULL),
  fHistEtaPhiBeforCut(NULL),
  fHistEtaPhiAfterCut(NULL),  
  fHCorrectMCposChrg(NULL),
  fHCorrectMCposPion(NULL),
  fHCorrectMCposKaon(NULL),
  fHCorrectMCposProt(NULL),
  fHCorrectMCnegChrg(NULL),
  fHCorrectMCnegPion(NULL),
  fHCorrectMCnegKaon(NULL),
  fHCorrectMCnegProt(NULL),    
  fHistAChrgVsCent(NULL),

  fHistTPConlyVsCL1Before(NULL),
  fHistTPConlyVsV0MBefore(NULL),
  fHistCL0VsV0MBefore(NULL),
  fHistTPConlyVsCL1After(NULL),
  fHistTPConlyVsV0MAfter(NULL),
  fHistCL0VsV0MAfter(NULL),
  fHistGlobalVsV0MBefore(NULL),
  fHistGlobalVsV0MAfter(NULL),
  fTPCvsGlobalTrkBefore(NULL),
  fTPCvsGlobalTrkAfter(NULL),
  fHistTPCVsESDTrkBefore(NULL),
  fHistTPCVsESDTrkAfter(NULL),

  fHistPileUpCount(NULL),
  fHistEventCount(NULL),
  fHCorrectEVNTWGTChrg(NULL)
{


  for(int i=0;i<2;i++){
    for(int j=0;j<10;j++){
      fHistv2AchChrgPos[i][j] = NULL;
      fHistv2AchKaonPos[i][j] = NULL;    
      fHistv2AchProtPos[i][j] = NULL;      
      fHistv2AchPionPos[i][j] = NULL;

      fHistv2AchChrgPosChrgNeg[i][j] = NULL;
      fHistv2AchPionPosPionNeg[i][j] = NULL;
      fHistv2AchKaonPosKaonNeg[i][j] = NULL;
      fHistv2AchProtPosProtNeg[i][j] = NULL;
      
      fHistv2AchChrgNeg[i][j] = NULL;
      fHistv2AchPionNeg[i][j] = NULL;      
      fHistv2AchKaonNeg[i][j] = NULL;
      fHistv2AchProtNeg[i][j] = NULL;

      fHistv2AchChrgNegChrgPos[i][j] = NULL;
      fHistv2AchPionNegPionPos[i][j] = NULL;
      fHistv2AchKaonNegKaonPos[i][j] = NULL;
      fHistv2AchProtNegProtPos[i][j] = NULL;
    }
  }

  for(int i=0; i<5; i++){
    fHCorrectNUAposChrg[i] = NULL;  
    fHCorrectNUAnegChrg[i] = NULL;  
    fHCorrectNUAposPion[i] = NULL;  
    fHCorrectNUAnegPion[i] = NULL;  
    fHCorrectNUAposKaon[i] = NULL;  
    fHCorrectNUAnegKaon[i] = NULL;  
    fHCorrectNUAposProt[i] = NULL;  
    fHCorrectNUAnegProt[i] = NULL;  
  }

  for(int i=0; i<5; i++){
    fHFillNUAPosPID[i]  = NULL;
    fHFillNUANegPID[i]  = NULL; 
  }

  for(int i=0; i<2; i++){
    fHistEPResolution[i] = NULL;
  }

  for(int i=0; i<10; i++){
    fHistEPResolutionAch[i] = NULL;
  }
    
  for(int i=0; i<10; i++){
    fHistv2cumAchChrgAll[i] = NULL;
  }

  
  //Not needed for Empty Constructor:
  //DefineInput(0,TChain::Class());
  //DefineOutput(1,TList::Class());
}
  
//__________________ destructor ___________________
AliAnalysisTaskCMWPU2018::~AliAnalysisTaskCMWPU2018()
{
  if(fListHist)      delete fListHist;  
  if(fAnalysisUtil)  delete fAnalysisUtil;   // because its 'new' !!

  //Delete the clones
  if(fListTRKCorr)  delete fListTRKCorr;
  if(fListNUACorr)  delete fListNUACorr;
  if(fListV0MCorr)  delete fListV0MCorr;
      
}










//________________ Define Histograms _______________
void AliAnalysisTaskCMWPU2018::UserCreateOutputObjects()
{
  //std::cout<<"\n UserCreateOutputObject: function begins...\n"<<endl; 
  //Get The Input Hander:
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  AliInputEventHandler *inputHandler=dynamic_cast<AliInputEventHandler*>(mgr->GetInputEventHandler());
  
  if (!inputHandler) {  printf("\n***** ERROR *****\n Input handler missing, Status:QUIT!\n");    exit(1);}




  //PileUp Multi-Vertex
  fAnalysisUtil = new AliAnalysisUtils();
  fAnalysisUtil->SetUseMVPlpSelection(kTRUE);
  fAnalysisUtil->SetUseOutOfBunchPileUp(kTRUE);
  
  //// obtain the PID response object if needed:
  fPIDResponse=inputHandler->GetPIDResponse();
  if (!fPIDResponse) {  printf("\n***** ERROR *****\n fPIDResponse missing, Status:QUIT!\n");    exit(1);}    
    
  fListHist = new TList();
  fListHist->SetOwner(kTRUE);

  SetupEventAndTaskConfigInfo();


  /// Bunch of QA histograms:  
  fCentDistBeforCut = new TH1F("fCentDistBeforCut","Cent w/o any Cuts; Cent (%); no.Events ",100,0,100);
  fListHist->Add(fCentDistBeforCut);
  fCentDistAfterCut = new TH1F("fCentDistAfterCut","Cent with all Cut; Cent (%); no.Events ",100,0,100);
  fListHist->Add(fCentDistAfterCut);
  
  fHistEtaPtBeforCut = new TH2F("fHistEtaPtBeforCut","#eta vs p_{T} (wFB, w/o  cut)",24,-1.2,1.2,50,0,5);
  fListHist->Add(fHistEtaPtBeforCut);
  fHistEtaPhiBeforCut = new TH2F("fHistPhiEtaBeforCut","#phi vs #eta (wFB, w/o cut)",50,0,6.2835,24,-1.2,1.2);
  fListHist->Add(fHistEtaPhiBeforCut);  
  fHistEtaPhiAfterCut = new TH2F("fHistPhiEtaAfterCut","#phi vs #eta (with Wgts)",50,0,6.2835,24,-1.2,1.2);
  fListHist->Add(fHistEtaPhiAfterCut);
  



  Int_t gMaxGlobalmult  = 4000;
  Int_t gMaxTPCcorrmult = 5000;
  Int_t gMaxESDtracks   = 20000;





  fHistGlobalVsV0MBefore = new TH2F("fHistGlobalVsV0MBefore","Before;Cent(V0M);Global",100,0,100,500,0,gMaxGlobalmult);
  fListHist->Add(fHistGlobalVsV0MBefore);
  fHistGlobalVsV0MAfter  = new TH2F("fHistGlobalVsV0MAfter"," After; Cent(V0M);Global",100,0,100,500,0,gMaxGlobalmult);
  fListHist->Add(fHistGlobalVsV0MAfter);

  fHistTPConlyVsCL1Before = new TH2F("fHistTPConlyVsCL1Before","Before;Cent(CL1); TPC(FB128)",100,0,100,250,0,gMaxTPCcorrmult);
  fListHist->Add(fHistTPConlyVsCL1Before);
  fHistTPConlyVsCL1After  = new TH2F("fHistTPConlyVsCL1After","After; Cent(CL1); TPC(FB128) ",100,0,100,250,0,gMaxTPCcorrmult);
  fListHist->Add(fHistTPConlyVsCL1After);

  fHistTPConlyVsV0MBefore = new TH2F("fHistTPConlyVsV0MBefore","Before;Cent(V0M); TPC(FB128)",100,0,100,250,0,gMaxTPCcorrmult);
  fListHist->Add(fHistTPConlyVsV0MBefore);
  fHistTPConlyVsV0MAfter  = new TH2F("fHistTPConlyVsV0MAfter","After; Cent(V0M); TPC(FB128) ",100,0,100,250,0,gMaxTPCcorrmult);
  fListHist->Add(fHistTPConlyVsV0MAfter);


  fHistCL0VsV0MBefore = new TH2F("fHistCL0VsV0MBefore","Before;Cent(V0M); Cent(CL0)",100,0,100,100,0,100);
  fListHist->Add(fHistCL0VsV0MBefore);
  fHistCL0VsV0MAfter  = new TH2F("fHistCL0VsV0MAfter","After; Cent(V0M); Cent(CL0) ",100,0,100,100,0,100);
  fListHist->Add(fHistCL0VsV0MAfter);


  fTPCvsGlobalTrkBefore = new TH2F("fTPCvsGlobalTrkBefore","Global(FB32) vs TPC(FB128)",250,0,5000,250,0,5000);
  fListHist->Add(fTPCvsGlobalTrkBefore);
  fTPCvsGlobalTrkAfter = new TH2F("fTPCvsGlobalTrkAfter","Global(FB32) vs TPC(FB128)",250,0,5000,250,0,5000);
  fListHist->Add(fTPCvsGlobalTrkAfter);
  
  fHistTPCVsESDTrkBefore = new TH2F("fHistTPCVsESDTrkBefore","Before; TPC1; ESD trk",500,0,gMaxTPCcorrmult,200,0,gMaxESDtracks);
  fListHist->Add(fHistTPCVsESDTrkBefore);
  fHistTPCVsESDTrkAfter  = new TH2F("fHistTPCVsESDTrkAfter"," After;  TPC1; ESD trk",500,0,gMaxTPCcorrmult,200,0,gMaxESDtracks);
  fListHist->Add(fHistTPCVsESDTrkAfter);






  
  //----------- User's histograms: --------------
  Char_t  name[1000];
  Char_t title[1000];
 

  Double_t centRange[11] = {0,5,10,20,30,40,50,60,70,80,90};

  fHistAChrgVsCent = new TH2F("fHistAChrgVsCent","Ach vs Cent;Cent;Ach",18,0,90,50,-0.5,0.5);
  fListHist->Add(fHistAChrgVsCent);

		 
  // v2 vs Ach
  for(int i=0;i<2;i++){
    for(int j=0;j<10;j++){
      ////Charge:
      sprintf(name,"fHistv2AchChrgPos_Method%d_Cent%d",i,j);
      sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[j],centRange[j+1]);
      fHistv2AchChrgPos[i][j] = new TProfile(name,title,10,-0.1,0.1,"");
      fHistv2AchChrgPos[i][j]->Sumw2();
      fListHist->Add(fHistv2AchChrgPos[i][j]);
      sprintf(name,"fHistv2AchChrgNeg_Method%d_Cent%d",i,j);
      sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[j],centRange[j+1]);
      fHistv2AchChrgNeg[i][j] = new TProfile(name,title,10,-0.1,0.1,"");
      fHistv2AchChrgNeg[i][j]->Sumw2();
      fListHist->Add(fHistv2AchChrgNeg[i][j]);



      sprintf(name,"fHistv2AchChrgPosChrgNeg_Method%d_Cent%d",i,j);
      sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[j],centRange[j+1]);
      fHistv2AchChrgPosChrgNeg[i][j] = new TProfile(name,title,10,-0.1,0.1,"");
      fHistv2AchChrgPosChrgNeg[i][j]->Sumw2();
      fListHist->Add(fHistv2AchChrgPosChrgNeg[i][j]);
      sprintf(name,"fHistv2AchChrgNegChrgPos_Method%d_Cent%d",i,j);
      sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[j],centRange[j+1]);
      fHistv2AchChrgNegChrgPos[i][j] = new TProfile(name,title,10,-0.1,0.1,"");
      fHistv2AchChrgNegChrgPos[i][j]->Sumw2();
      fListHist->Add(fHistv2AchChrgNegChrgPos[i][j]);


      

      //// Pion:
      sprintf(name,"fHistv2AchPionPos_Method%d_Cent%d",i,j);
      sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[j],centRange[j+1]);
      fHistv2AchPionPos[i][j] = new TProfile(name,title,10,-0.1,0.1,"");
      fHistv2AchPionPos[i][j]->Sumw2();
      fListHist->Add(fHistv2AchPionPos[i][j]);
      sprintf(name,"fHistv2AchPionNeg_Method%d_Cent%d",i,j);
      sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[j],centRange[j+1]);
      fHistv2AchPionNeg[i][j] = new TProfile(name,title,10,-0.1,0.1,"");
      fHistv2AchPionNeg[i][j]->Sumw2();
      fListHist->Add(fHistv2AchPionNeg[i][j]);


      
      sprintf(name,"fHistv2AchPionPosPionNeg_Method%d_Cent%d",i,j);
      sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[j],centRange[j+1]);
      fHistv2AchPionPosPionNeg[i][j] = new TProfile(name,title,10,-0.1,0.1,"");
      fHistv2AchPionPosPionNeg[i][j]->Sumw2();
      fListHist->Add(fHistv2AchPionPosPionNeg[i][j]);
      sprintf(name,"fHistv2AchPionNegPionPos_Method%d_Cent%d",i,j);
      sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[j],centRange[j+1]);
      fHistv2AchPionNegPionPos[i][j] = new TProfile(name,title,10,-0.1,0.1,"");
      fHistv2AchPionNegPionPos[i][j]->Sumw2();
      fListHist->Add(fHistv2AchPionNegPionPos[i][j]);      

      
 
      //// Kaon:
      sprintf(name,"fHistv2AchKaonPos_Method%d_Cent%d",i,j);
      sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[j],centRange[j+1]);
      fHistv2AchKaonPos[i][j] = new TProfile(name,title,10,-0.1,0.1,"");
      fHistv2AchKaonPos[i][j]->Sumw2();
      fListHist->Add(fHistv2AchKaonPos[i][j]);
      sprintf(name,"fHistv2AchKaonNeg_Method%d_Cent%d",i,j);
      sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[j],centRange[j+1]);
      fHistv2AchKaonNeg[i][j] = new TProfile(name,title,10,-0.1,0.1,"");
      fHistv2AchKaonNeg[i][j]->Sumw2();
      fListHist->Add(fHistv2AchKaonNeg[i][j]);


      sprintf(name,"fHistv2AchKaonPosKaonNeg_Method%d_Cent%d",i,j);
      sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[j],centRange[j+1]);
      fHistv2AchKaonPosKaonNeg[i][j] = new TProfile(name,title,10,-0.1,0.1,"");
      fHistv2AchKaonPosKaonNeg[i][j]->Sumw2();
      fListHist->Add(fHistv2AchKaonPosKaonNeg[i][j]);
      sprintf(name,"fHistv2AchKaonNegKaonPos_Method%d_Cent%d",i,j);
      sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[j],centRange[j+1]);
      fHistv2AchKaonNegKaonPos[i][j] = new TProfile(name,title,10,-0.1,0.1,"");
      fHistv2AchKaonNegKaonPos[i][j]->Sumw2();
      fListHist->Add(fHistv2AchKaonNegKaonPos[i][j]);      
      

      //// Proton:
      sprintf(name,"fHistv2AchProtPos_Method%d_Cent%d",i,j);
      sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[j],centRange[j+1]);
      fHistv2AchProtPos[i][j] = new TProfile(name,title,10,-0.1,0.1,"");
      fHistv2AchProtPos[i][j]->Sumw2();
      fListHist->Add(fHistv2AchProtPos[i][j]);
      sprintf(name,"fHistv2AchProtNeg_Method%d_Cent%d",i,j);
      sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[j],centRange[j+1]);
      fHistv2AchProtNeg[i][j] = new TProfile(name,title,10,-0.1,0.1,"");
      fHistv2AchProtNeg[i][j]->Sumw2();
      fListHist->Add(fHistv2AchProtNeg[i][j]);


      sprintf(name,"fHistv2AchProtPosProtNeg_Method%d_Cent%d",i,j);
      sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[j],centRange[j+1]);
      fHistv2AchProtPosProtNeg[i][j] = new TProfile(name,title,10,-0.1,0.1,"");
      fHistv2AchProtPosProtNeg[i][j]->Sumw2();
      fListHist->Add(fHistv2AchProtPosProtNeg[i][j]);
      sprintf(name,"fHistv2AchProtNegProtPos_Method%d_Cent%d",i,j);
      sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[j],centRange[j+1]);
      fHistv2AchProtNegProtPos[i][j] = new TProfile(name,title,10,-0.1,0.1,"");
      fHistv2AchProtNegProtPos[i][j]->Sumw2();
      fListHist->Add(fHistv2AchProtNegProtPos[i][j]);      
      
    }
  }



  Int_t gCentForNUA[6] = {0,5,10,20,40,90};
  Char_t cpid[10];

  if(fParticle==1)      sprintf(cpid,"Pion,Id %d",fParticle);
  else if(fParticle==2) sprintf(cpid,"Kaon,Id %d",fParticle);
  else if(fParticle==3) sprintf(cpid,"Prot,Id %d",fParticle);
  else  sprintf(cpid,"Charge,Id %d",fParticle);
  
  for(int i=0; i<5; i++){
    sprintf(name,"fHistEtaPhiVz_%d_Pos_Cent%d_Run%d",fParticle,i,1); 
    sprintf(title,"%s Pos, Cent%d-%d, FB %d",cpid,gCentForNUA[i],gCentForNUA[i+1],fFilterBit);
    fHFillNUAPosPID[i] = new TH3F(name,title,10,-10,10,50,0,6.283185,16,-0.8,0.8); 
    fListHist->Add(fHFillNUAPosPID[i]);

    sprintf(name,"fHistEtaPhiVz_%d_Neg_Cent%d_Run%d",fParticle,i,1); 
    sprintf(title,"%s Neg, Cent%d-%d, FB %d",cpid,gCentForNUA[i],gCentForNUA[i+1],fFilterBit);
    fHFillNUANegPID[i] = new TH3F(name,title,10,-10,10,50,0,6.283185,16,-0.8,0.8); 
    fListHist->Add(fHFillNUANegPID[i]);    
  }
      



  
  for(int i=0; i<10; i++){
    ////Charge:
    sprintf(name,"fHistResolutionvsAch_Cent%d",i);
    sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; Resolution",centRange[i],centRange[i+1]);
    fHistEPResolutionAch[i] = new TProfile(name,title,10,-0.1,0.1,"");
    fHistEPResolutionAch[i]->Sumw2();
    fListHist->Add(fHistEPResolutionAch[i]);
  }

  
  for(int i=0; i<10; i++){
    ////Charge:
    sprintf(name,"fHistv2cumAchChrgAllQcumCent%d",i);
    sprintf(title,"Cent %2.0f-%2.0f; A_{ch}; v_{2}",centRange[i],centRange[i+1]);
    fHistv2cumAchChrgAll[i] = new TProfile(name,title,10,-0.1,0.1,"");
    fHistv2cumAchChrgAll[i]->Sumw2();
    fListHist->Add(fHistv2cumAchChrgAll[i]);
  }

  
  for(int i=0; i<2; i++){
    sprintf(name,"fHistEPResolution_Method%d",i);
    sprintf(title,"EP Resolution; centrality; Reso");
    fHistEPResolution[i] = new TProfile(name,title,10,centRange,"");
    fHistEPResolution[i]->Sumw2();
    fListHist->Add(fHistEPResolution[i]);
  }



  



  if(fListTRKCorr){
    std::cout<<"\n UserCreateOutputObject::Info() Tlist for MC tracking Efficiency Found.!!\n"<<std::endl;
  }
  else{
    std::cout<<"\n\n ******* WARNING No TList for Trk Efficiency Correction!!\n using TrkWgt = 1.0 \n "<<std::endl;
  }

  if(fListNUACorr){
    std::cout<<"\n UserCreateOutputObject::Info() Tlist for NUA Correction Found.!!\n"<<std::endl;
    //fListNUACorr->ls(); 
 }
  else{
    std::cout<<"\n\n ******* WARNING No TList NUA Correction!!\n using NUAWgt = 1.0 \n "<<std::endl;
  }

  if(fListV0MCorr){
    std::cout<<"\n UserCreateOutputObject::Info() Tlist for EVNTWGT Correction Found.!!\n"<<std::endl;
    //fListV0MCorr->ls();
 
}
  else{
    std::cout<<"\n\n ******* WARNING No TList EVNTWGT Correction!!\n using EVNTWGTWgt = 1.0 \n "<<std::endl;
  }
  
  //fParticle = 3;
  
  std::cout<<"\n UserCreateOutputObject; PID = "<<fParticle<<" FB = "<<fFilterBit<<" harmonic = "<<gHarmonic<<"...\n"<<endl;
  
  
  PostData(1,fListHist);
  
}










//____________________________ Call Event by Event ___________________________________
void AliAnalysisTaskCMWPU2018::UserExec(Option_t*) {
 
  //std::cout<<" Info:UserExec() called ..!!!\n";

  
  Float_t stepCount = 0.5;

  fHistEventCount->Fill(stepCount);   //1
  stepCount++;





  fAOD = dynamic_cast <AliAODEvent*> (InputEvent());
  fESD = dynamic_cast <AliESDEvent*> (InputEvent());
  if(!(fESD || fAOD)) {
    printf("ERROR: fESD & fAOD not available\n");
    return;
  }

  //std::cout<<" Info:UserExec()  AOD Exist ..!!!\n";

  fVevent = dynamic_cast<AliVEvent*>(InputEvent());
  if (!fVevent){
    printf("ERROR: fVevent not available\n");
    return;
  }





  fHistEventCount->Fill(stepCount); //2
  stepCount++;

  
  //fHistEventCount->Fill(stepCount); //2
  //stepCount++;


  

    

  //-------------- Vtx cuts ---------------
  const AliVVertex *pVtx = fVevent->GetPrimaryVertex();
  Double_t pVtxZ = -999;
  pVtxZ  = pVtx->GetZ();

  //std::cout<<" Info:UserExec()  AliVEvent Exist pVtxZ = "<<pVtxZ<<"..!!!\n";
    
  if(pVtxZ < fMinVzCut || pVtxZ > fMaxVzCut){
    return;
  }


  //std::cout<<" Info:UserExec()  Vz<10cm check ..!!!\n";
  
  fHistEventCount->Fill(stepCount); //3
  stepCount++;

  Float_t centrality = -99.0;
  Float_t centrV0M   = -99.0;
  //Float_t centrCL1   = -99.0;

  fMultSelection = (AliMultSelection*) InputEvent()->FindListObject("MultSelection");  // Must never comment this
  if(!fMultSelection) {
    printf("\n...**ERROR**...\n UserExec() AliMultSelection object not found\n Status:Quit!! \n");
    exit(1);
  }


  centrV0M = fMultSelection->GetMultiplicityPercentile("V0M");
  //centrCL1 = fMultSelection->GetMultiplicityPercentile("CL1");

 
  centrality = centrV0M;  // This Is Always Default, changes below for other options:
  
  if(sCentrEstimator=="CL0"){
    centrality = fMultSelection->GetMultiplicityPercentile("CL0");;
  }
  else if(sCentrEstimator=="CL1"){
    centrality = fMultSelection->GetMultiplicityPercentile("CL1");;
  }
  else if(sCentrEstimator=="V0C"){
    centrality = fMultSelection->GetMultiplicityPercentile("V0C");
  }
  else if(sCentrEstimator=="V0A"){
    centrality = fMultSelection->GetMultiplicityPercentile("V0A");
  }
  else if(sCentrEstimator=="TRK"){
    centrality = fMultSelection->GetMultiplicityPercentile("TRK");
  }
  else{
    centrality = centrV0M;
  }

  
  fCentDistBeforCut->Fill(centrality);
  




  //Pile up cut function called--------------------------

  Bool_t kPileupEvent = kFALSE;

  kPileupEvent = CheckEventIsPileUp2018(fAOD);
  if (!bSkipPileUpCut)
    if(kPileupEvent)  return;
    
  
  /*
  AliTimeRangeCut  *fTimeRangeCut;
  fTimeRangeCut = new AliTimeRangeCut;
  fTimeRangeCut->InitFromEvent(fAOD);
  if (fTimeRangeCut->CutEvent(fAOD))
    return;
  */



  //////////////////////////////////////////////////



  if(centrality<fCentralityMin || centrality>fCentralityMax){ 
    return;
  }

  //std::cout<<" Info:UserExec()  Centrality checkd..!!!\n";
  
  ///----> Get 0-10 index of centrality:
  Int_t cent10bin = -99, iCent = -99;

  if(centrality<5.0) {
    cent10bin  = 0; 
  }
  else if(centrality>=5.0 && centrality<10){
    cent10bin  = 1;
  }
  else if(centrality>=10.0) {
    cent10bin = abs(centrality/10.0)+1;
  }

  iCent = cent10bin;


  //Centrality array index for NUA correcion
  Int_t cForNUA = 0;  

  if(centrality<5.0) {
    cForNUA = 0;
  }
  else if(centrality>=5.0 && centrality<10){
    cForNUA = 1; // 1=5-10,
  }
  else if(centrality>=10.0 && centrality<20) {
    cForNUA = 2; // 2 = 10-20,
  }
  else if(centrality>=20 && centrality<40){ 
    cForNUA = 3; // 3=20-40
  }
  else if(centrality>=40){
    cForNUA = 4; // 4=40-90
  }









  

  Int_t ntracks = fAOD->GetNumberOfTracks();
  if(ntracks < 4) return;                        // Minimum 4 tracks per event. 

  fHistEventCount->Fill(stepCount); //7
  stepCount++;

  //std::cout<<" Info:UserExec()  minimum ntracks checkd..!!!\n";
  
  

  //////----> Get Magnetic field and RunNo.---------
  // Float_t fMagField = fAOD->GetMagneticField();
  // const Int_t QAindex = (fMagField > 0) ? 1 : 0;
  Int_t runNumber = fAOD->GetRunNumber();
  //------------------------------------------------

  //if(fListTRKCorr) GetMCCorrectionHist(runNumber);
  if(fListTRKCorr) GetMCCorrectionHist(runNumber,centrality);  //use centrality dependent MC efficiency (Temporary!!)

  

  if(fListNUACorr){
     GetNUACorrectionHist(runNumber,0);         //Charge
     GetNUACorrectionHist(runNumber,fParticle); //1=pion, 2=kaon, 3=proton
  }
  
  if(fListV0MCorr){
     GetV0MCorrectionHist(runNumber,0);         //Charge
     //GetEVNTWGTCorrectionHist(runNumber,fParticle); //1=pion, 2=kaon, 3=proton
  }
  




  
  Float_t fMultTPCFull = 0;  // TPC mult estimate
  Float_t fMultGlobal  = 0;  // global track multiplicity
  
  //Float_t fMultwRawFB  = 0;  // Uncorrected Multiplicity
  //Float_t fMultCorrFB  = 0;  // Corrected Multiplicity  
  //Int_t   gMultEtaNeg  = 0;
  //Int_t   gMultEtaPos  = 0;
  //Int_t   gMultEtaAll  = 0;

  Float_t trkPt=0,trkPhi=0,trkEta=0,trkDCAxy=0.0, trkDCAz=0.0;
  Float_t trkChi2=0,trkdEdx=0,trkWgt=1.0;
  Int_t   trkChrg=0, trkTpcNC=0;
  ////PID variables:
  Double_t nSigTOFpion=-99, nSigTPCpion=-99;
  Double_t nSigTOFkaon=-99, nSigTPCkaon=-99;
  Double_t nSigTOFprot=-99, nSigTPCprot=-99;  
  //Bool_t   bTOFmatch= kFALSE;
  Bool_t   isItPion = kFALSE, isItKaon= kFALSE, isItProt= kFALSE;
	  
  ////User's variable:
  Double_t fSumTPCQn2xNeg=0, fSumTPCQn2yNeg=0, fSumTPCQn2xPos=0, fSumTPCQn2yPos=0;
  Double_t fSumTPCQn2xNegWhole=0, fSumTPCQn2yNegWhole=0, fSumTPCQn2xPosWhole=0, fSumTPCQn2yPosWhole=0;
  Double_t fSumTPCQn2xNegChNeg=0, fSumTPCQn2yNegChNeg=0, fSumTPCQn2xPosChNeg=0, fSumTPCQn2yPosChNeg=0;
  Double_t fSumWgtEtaNeg=0, fSumWgtEtaPos=0;
  Double_t fSumWgtEtaNegWhole=0, fSumWgtEtaPosWhole=0;
  Double_t fSumWgtEtaNegChNeg=0, fSumWgtEtaPosChNeg=0;
  Double_t fNumOfPos = 0;
  Double_t fNumOfNeg = 0;
  Double_t ptWgtMC = 1.0, WgtNUA = 1.0;
  Int_t ptBinMC = 1, iBinNUA = 1;
  Double_t binCont = 1.0;
  
  Float_t fWgtEvent = 1.0; //Event Weight if Any.

  /// TO be passed as Argument:
  //Double_t fEtaGapNeg = -0.100;
  //Double_t fEtaGapPos =  0.100;
  Double_t gPsiN = gHarmonic;

  
  //std::cout<<" Info:UserExec()  Starting track loop 1..!!!\n";
  
  ////////-----> Starting 1st track Loop -----------
  
  for(Int_t iTrack = 0; iTrack < ntracks; iTrack++) { 

    AliAODTrack* AODtrack = dynamic_cast <AliAODTrack*> (fVevent->GetTrack(iTrack));
    if(!AODtrack) continue;
    
    if(AODtrack->TestFilterBit(fFilterBit)) {  //// Only use FB tracks. 

      trkPt    = AODtrack->Pt();
      trkPhi   = AODtrack->Phi();
      trkEta   = AODtrack->Eta();
      trkChrg  = AODtrack->Charge();
      trkChi2  = AODtrack->Chi2perNDF();
      trkTpcNC = AODtrack->GetTPCNcls();
      trkDCAxy=  AODtrack->DCA();
      trkDCAz=   AODtrack->ZAtDCA();

      
      fHistEtaPtBeforCut->Fill(trkEta, trkPt);
      fHistEtaPhiBeforCut->Fill(trkPhi,trkEta);
      
      /// This Next function is called After Filter bit is validated!! (Otherwise code breaks!)
      trkdEdx  = AODtrack->GetDetPid()->GetTPCsignal();
      /*
      Double_t dTrackXYZ[3] = {0};
      Double_t dVertexXYZ[3] = {0.};
      Double_t dDCAXYZ[3] = {0.};
        
      AODtrack->GetXYZ(dTrackXYZ);
      pVtx->GetXYZ(dVertexXYZ);
        
      for(Short_t i(0); i < 3; i++)
	dDCAXYZ[i] = dTrackXYZ[i] - dVertexXYZ[i];

      trkDCAxy=TMath::Sqrt(dDCAXYZ[0]*dDCAXYZ[0] + dDCAXYZ[1]*dDCAXYZ[1]);
      trkDCAz=dDCAXYZ[2];
      */
      

      //Apply track cuts here:
      //     if((trkPt <= fMaxPtCut) && (trkPt >= fMinPtCut) && (trkEta <= fMaxEtaCut) && (trkEta >= fMinEtaCut) && (trkdEdx >= fdEdxMin) && (trkTpcNC >= fTPCclustMin) && (trkChi2 >= fTrkChi2Min) && (trkChi2 <= 4.0) && TMath::Abs(trkChrg)) {

      if((trkPt <= 10) && (trkPt >= fMinPtCut) && (trkEta <= fMaxEtaCut) && (trkEta >= fMinEtaCut) && (trkdEdx >= fdEdxMin) && (trkTpcNC >= fTPCclustMin) && (trkChi2 >= fTrkChi2Min) && (trkChi2 <= fChi2) && TMath::Abs(trkChrg)) {


	  
      //if((trkPt <= 10) && (trkPt >= fMinPtCut) && (trkEta <= fMaxEtaCut) && (trkEta >= fMinEtaCut) && (trkdEdx >= fdEdxMin) && (trkTpcNC >= fTPCclustMin) && (trkChi2 >= fTrkChi2Min) && (trkChi2 <= fChi2) && TMath::Abs(trkChrg) && ((trkDCAxy)<fDCAxyMax) && ((trkDCAz)<fDCAzMax)){
	  

	  
	//dcaXY  = track->DCA();
	//dcaZ   = track->ZAtDCA();
        
	//---------->  Here I do All my track level analysis:
    



	  

	//------> Get NUA weights for EP <----------

	
	
	WgtNUA = 1.0;
	ptWgtMC = 1.0;
	
	if(trkChrg>0){

	  if(fHCorrectMCposChrg){
	    ptBinMC = fHCorrectMCposChrg->FindBin(trkPt);    //Charge independent MC correction atm.
	    binCont = fHCorrectMCposChrg->GetBinContent(ptBinMC);
	    if(binCont!=0) ptWgtMC = 1.0/binCont;      
	  }
	  
	  if(fHCorrectNUAposChrg[cForNUA]){
	    iBinNUA = fHCorrectNUAposChrg[cForNUA]->FindBin(pVtxZ,trkPhi,trkEta);
	    WgtNUA  = fHCorrectNUAposChrg[cForNUA]->GetBinContent(iBinNUA);	    
	  }
	}
	else{
	  
	  if(fHCorrectMCnegChrg){
	    ptBinMC = fHCorrectMCnegChrg->FindBin(trkPt);    //Charge independent MC correction atm.	    
	    binCont = fHCorrectMCnegChrg->GetBinContent(ptBinMC);
	    if(binCont!=0) ptWgtMC = 1.0/binCont;  
	  }
	  
	  if(fHCorrectNUAnegChrg[cForNUA]){
	    iBinNUA = fHCorrectNUAnegChrg[cForNUA]->FindBin(pVtxZ,trkPhi,trkEta);
	    WgtNUA  = fHCorrectNUAnegChrg[cForNUA]->GetBinContent(iBinNUA);  
	  }
	}

	//RefMultRawFB++;
	//RefMultCorrFB += ptWgtMC;
	if(WgtNUA>1e3) WgtNUA = 1;   // For empty bins which cause Wgt = infinity !!

	trkWgt = WgtNUA*ptWgtMC;

	if(trkChrg > 0){	  
	  fNumOfPos += trkWgt;
	}
	else{
	  fNumOfNeg += trkWgt;
	}
	
       
	if(trkPt < 2.0)  ///// *********  Trk cut for Event Plane: 0.2 < pT < 2.0; *********
	  {
	  


	if(trkEta < fEtaGapNeg){
	  fSumTPCQn2xNegWhole += trkWgt*TMath::Cos(gPsiN*trkPhi);
	  fSumTPCQn2yNegWhole += trkWgt*TMath::Sin(gPsiN*trkPhi);
	  fSumWgtEtaNegWhole  += trkWgt;
	}
	else if(trkEta > fEtaGapPos){
	  fSumTPCQn2xPosWhole += trkWgt*TMath::Cos(gPsiN*trkPhi);
	  fSumTPCQn2yPosWhole += trkWgt*TMath::Sin(gPsiN*trkPhi);
	  fSumWgtEtaPosWhole  += trkWgt;
	}




  if(trkEta < fEtaGapNeg){
	  if (trkChrg>0) //added by me for trivial term correction
	  {
	  fSumTPCQn2xNeg += trkWgt*TMath::Cos(gPsiN*trkPhi);
	  fSumTPCQn2yNeg += trkWgt*TMath::Sin(gPsiN*trkPhi);
	  fSumWgtEtaNeg  += trkWgt;
	  }
	  
	  //added by me for trivial term correction
	else if (trkChrg<0)
	  {
	  fSumTPCQn2xNegChNeg += trkWgt*TMath::Cos(gPsiN*trkPhi);
	  fSumTPCQn2yNegChNeg += trkWgt*TMath::Sin(gPsiN*trkPhi);
	  fSumWgtEtaNegChNeg  += trkWgt;
	  }
	  ///////////////////////////////
	
	}
	else if(trkEta > fEtaGapPos){
	  
	  if (trkChrg>0)  //added by me for trivial term correction
	    {
	  fSumTPCQn2xPos += trkWgt*TMath::Cos(gPsiN*trkPhi);
	  fSumTPCQn2yPos += trkWgt*TMath::Sin(gPsiN*trkPhi);
	  fSumWgtEtaPos  += trkWgt;
	    }
	  
	   //added by me for trivial term correction
	  else if (trkChrg<0) 
	    {
	      fSumTPCQn2xPosChNeg += trkWgt*TMath::Cos(gPsiN*trkPhi);
	      fSumTPCQn2yPosChNeg += trkWgt*TMath::Sin(gPsiN*trkPhi);
	      fSumWgtEtaPosChNeg  += trkWgt;
	    }
	  ////////////////////////////////////////
	  
	}


	  }

	
	
	//<---------- User track analysis Done---------------

      }// when all trackCuts applied      
    }//-------> when FB is validated


  
  }///------> 1st Track loop Ends here.<--------




  if(iCent > 9) return;

  fHistEventCount->Fill(stepCount); //8
  stepCount++;


  if(fSumWgtEtaNeg <= 0 || fSumWgtEtaPos <= 0) return;
  
  fHistEventCount->Fill(stepCount); //9
  stepCount++;
  

  //// Decide If Pile-Up cut to be applied.
  //fHistEventCount->Fill(stepCount); //7
  //stepCount++;

  
  
  Float_t fAchrgNet = (fNumOfPos - fNumOfNeg)/(fNumOfPos + fNumOfNeg); // Efficiency & NUA Corrected!

  fHistAChrgVsCent->Fill(centrality, fAchrgNet, fWgtEvent);
  
  // Do Event plane defination, Fill Resolution Etc.
  // Double_t Psi2EtaPos =  Formula for PsiN:
  
  Double_t ResolutionEP = fSumTPCQn2xNeg*fSumTPCQn2xPos + fSumTPCQn2yNeg*fSumTPCQn2yPos;
  ResolutionEP  = ResolutionEP/(fSumWgtEtaPos*fSumWgtEtaNeg);
  Double_t ResWgt = TMath::Sqrt(fSumWgtEtaPos*fSumWgtEtaNeg);

  /// Resolution vs cent:
  fHistEPResolution[0]->Fill(centrality,ResolutionEP,ResWgt*fWgtEvent);

  /// Resolution vs Ach for each cent:
  fHistEPResolutionAch[iCent]->Fill(fAchrgNet,ResolutionEP,ResWgt*fWgtEvent);
















  
  ////////-----> Starting 2nd track Loop -----------
  /// Track loop variable (not used in previous loop)
  Double_t sumQxTPCneg=0, sumQyTPCneg=0, sumQxTPCpos=0, sumQyTPCpos=0;
  Double_t sumWgtneg=0, sumWgtpos=0;
  Double_t uqRe=0, uqIm=0;

  //// Variable for Eta-Pos:
  Double_t sumQ2xChrgPosEtaPos=0, sumQ2yChrgNegEtaPos=0;
  Double_t sumQ2xPionPosEtaPos=0, sumQ2yPionNegEtaPos=0;
  Double_t sumQ2xKaonPosEtaPos=0, sumQ2yKaonNegEtaPos=0;
  Double_t sumQ2xProtPosEtaPos=0, sumQ2yProtNegEtaPos=0;
  Double_t sumQ2xChrgNegEtaPos=0, sumQ2yChrgPosEtaPos=0;
  Double_t sumQ2xPionNegEtaPos=0, sumQ2yPionPosEtaPos=0;
  Double_t sumQ2xKaonNegEtaPos=0, sumQ2yKaonPosEtaPos=0;
  Double_t sumQ2xProtNegEtaPos=0, sumQ2yProtPosEtaPos=0;
  
  Double_t  NumOfChrgPosEtaPos=0,  NumOfChrgNegEtaPos=0;    
  Double_t  NumOfPionPosEtaPos=0,  NumOfPionNegEtaPos=0;
  Double_t  NumOfKaonPosEtaPos=0,  NumOfKaonNegEtaPos=0;
  Double_t  NumOfProtPosEtaPos=0,  NumOfProtNegEtaPos=0;

  //// Now for Eta-Neg:
  Double_t sumQ2xChrgPosEtaNeg=0, sumQ2yChrgNegEtaNeg=0;
  Double_t sumQ2xPionPosEtaNeg=0, sumQ2yPionNegEtaNeg=0;
  Double_t sumQ2xKaonPosEtaNeg=0, sumQ2yKaonNegEtaNeg=0;
  Double_t sumQ2xProtPosEtaNeg=0, sumQ2yProtNegEtaNeg=0;
  Double_t sumQ2xChrgNegEtaNeg=0, sumQ2yChrgPosEtaNeg=0;
  Double_t sumQ2xPionNegEtaNeg=0, sumQ2yPionPosEtaNeg=0;
  Double_t sumQ2xKaonNegEtaNeg=0, sumQ2yKaonPosEtaNeg=0;
  Double_t sumQ2xProtNegEtaNeg=0, sumQ2yProtPosEtaNeg=0;

  Double_t  NumOfChrgPosEtaNeg=0,  NumOfChrgNegEtaNeg=0;   
  Double_t  NumOfPionPosEtaNeg=0,  NumOfPionNegEtaNeg=0;
  Double_t  NumOfKaonPosEtaNeg=0,  NumOfKaonNegEtaNeg=0;
  Double_t  NumOfProtPosEtaNeg=0,  NumOfProtNegEtaNeg=0;




  

  
  Double_t trkWgtPion=1.0, trkWgtKaon=1.0, trkWgtProt=1.0;
  Double_t WgtNUAPion=1.0, WgtNUAKaon=1.0, WgtNUAProt=1.0;
  Double_t ptWgtMCPion=1.0, ptWgtMCKaon=1.0, ptWgtMCProt=1.0;



  
  for(Int_t iTrack = 0; iTrack < ntracks; iTrack++) { 

    AliAODTrack* AODtrack = dynamic_cast <AliAODTrack*> (fVevent->GetTrack(iTrack));
    if(!AODtrack) continue;
    
    if(AODtrack->TestFilterBit(fFilterBit)) {  //// Only use FB tracks. 

      trkPt    = AODtrack->Pt();
      trkPhi   = AODtrack->Phi();
      trkEta   = AODtrack->Eta();
      trkChrg  = AODtrack->Charge();
      trkChi2  = AODtrack->Chi2perNDF();
      trkTpcNC = AODtrack->GetTPCNcls();
      trkDCAxy=  AODtrack->DCA();
      trkDCAz=   AODtrack->ZAtDCA();
      
      fHistEtaPtBeforCut->Fill(trkEta, trkPt);
      fHistEtaPhiBeforCut->Fill(trkPhi,trkEta);
      
      /// This Next function is called After Filter bit is validated!! (Otherwise code breaks!)
      trkdEdx  = AODtrack->GetDetPid()->GetTPCsignal();  

      /*
      Double_t dTrackXYZ[3] = {0};
      Double_t dVertexXYZ[3] = {0.};
      Double_t dDCAXYZ[3] = {0.};
        
      AODtrack->GetXYZ(dTrackXYZ);
      pVtx->GetXYZ(dVertexXYZ);
        
      for(Short_t i(0); i < 3; i++)
	dDCAXYZ[i] = dTrackXYZ[i] - dVertexXYZ[i];

      trkDCAxy=TMath::Sqrt(dDCAXYZ[0]*dDCAXYZ[0] + dDCAXYZ[1]*dDCAXYZ[1]);
      trkDCAz=dDCAXYZ[2];
      */

      
      //Apply track cuts here:
      if((trkPt <= fMaxPtCut) && (trkPt >= fMinPtCut) && (trkEta <= fMaxEtaCut) && (trkEta >= fMinEtaCut) && (trkdEdx >= fdEdxMin) && (trkTpcNC >= fTPCclustMin) && (trkChi2 >= fTrkChi2Min) && (trkChi2 <= fChi2) && TMath::Abs(trkChrg)) {



      //if((trkPt <= fMaxPtCut) && (trkPt >= fMinPtCut) && (trkEta <= fMaxEtaCut) && (trkEta >= fMinEtaCut) && (trkdEdx >= fdEdxMin) && (trkTpcNC >= fTPCclustMin) && (trkChi2 >= fTrkChi2Min) && (trkChi2 <= fChi2) && TMath::Abs(trkChrg) && ((trkDCAxy)<fDCAxyMax) && ((trkDCAz)<fDCAzMax)){

	
	//dcaXY  = track->DCA();
	//dcaZ   = track->ZAtDCA();
        
	//---------->  Here I do All my track level analysis:

	
	isItPion = kFALSE;
	isItKaon = kFALSE;
	isItProt = kFALSE;
     
	///=========> Get TPC/TOF nSigma for PID


	//----- Pion
	if(fParticle==1){
	  nSigTPCpion = fPIDResponse->NumberOfSigmasTPC(AODtrack, AliPID::kPion);
	  nSigTOFpion = fPIDResponse->NumberOfSigmasTOF(AODtrack, AliPID::kPion);
	
	  if(trkPt<=0.6 && TMath::Abs(nSigTPCpion)<=fNSigmaTPCCut){
	    isItPion = kTRUE;
	  }
	  //else if(trkPt>0.6 && trkPt<=10.0 && TMath::Abs(nSigTPCpion)<=fNSigmaTPCCut && TMath::Abs(nSigTOFpion)<=fNSigmaTOFCut){
	  // Using Circular cut for Pion: 	  
          else if(trkPt>0.6 && trkPt<=10.0 && TMath::Sqrt(nSigTPCpion*nSigTPCpion + nSigTOFpion*nSigTOFpion)<=fNSigmaTOFCut){
	  isItPion = kTRUE;
	  }
	}
	//----- Kaon
	else if(fParticle==2){
	  nSigTPCkaon = fPIDResponse->NumberOfSigmasTPC(AODtrack, AliPID::kKaon);	
	  nSigTOFkaon = fPIDResponse->NumberOfSigmasTOF(AODtrack, AliPID::kKaon);

	  if(trkPt<=0.45 && TMath::Abs(nSigTPCkaon)<=fNSigmaTPCCut){
	    isItKaon = kTRUE;
	  }
	  else if(trkPt>0.45 && trkPt<=10.0 && TMath::Abs(nSigTPCkaon)<=fNSigmaTPCCut && TMath::Abs(nSigTOFkaon)<=fNSigmaTOFCut){
	    isItKaon = kTRUE;
	  }
	}
	else if(fParticle==3){
	  nSigTPCprot = fPIDResponse->NumberOfSigmasTPC(AODtrack, AliPID::kProton);    
	  nSigTOFprot = fPIDResponse->NumberOfSigmasTOF(AODtrack, AliPID::kProton);
      
	  //----- Proton 
	  if(trkPt<=0.8 && TMath::Abs(nSigTPCprot)<=fNSigmaTPCCut){
	    isItProt = kTRUE;
	    if(trkChrg>0 && trkPt<0.4) isItProt = kFALSE;  //Proton below 0.4 GeV has beam Pipe Contamination
	  }
	  else if(trkPt>0.8 && trkPt<=10.0 && TMath::Abs(nSigTPCprot)<=fNSigmaTPCCut && TMath::Abs(nSigTOFprot)<=fNSigmaTOFCut){  
	    isItProt = kTRUE;
	  }
	}
    	//-------- PID selection is done ---------

	ptWgtMC = 1.0;
	WgtNUA  = 1.0;
	
	trkWgtPion = 1.0;  trkWgtKaon = 1.0;  trkWgtProt = 1.0;
	WgtNUAPion = 1.0;  WgtNUAKaon = 1.0;  WgtNUAProt = 1.0; 
	ptWgtMCPion = 1.0; ptWgtMCKaon = 1.0; ptWgtMCProt = 1.0;
	

	
	if(trkChrg>0){
	  
	  ///Tracking:
	  if(fHCorrectMCposChrg){
	    ptBinMC = fHCorrectMCposChrg->FindBin(trkPt);    	    
	    binCont = fHCorrectMCposChrg->GetBinContent(ptBinMC);
	    if(binCont!=0) ptWgtMC = 1.0/binCont;  
	  }
	  if(fParticle==1 && isItPion && fHCorrectMCposPion){
	    ptBinMC = fHCorrectMCposPion->FindBin(trkPt);    
	    binCont = fHCorrectMCposPion->GetBinContent(ptBinMC);
	    if(binCont!=0)  ptWgtMCPion = 1.0/binCont; 	      
	  }
	  if(fParticle==2 && isItKaon && fHCorrectMCposKaon){
	    ptBinMC = fHCorrectMCposKaon->FindBin(trkPt);
	    binCont = fHCorrectMCposKaon->GetBinContent(ptBinMC);
	    if(binCont!=0)  ptWgtMCKaon = 1.0/binCont;
	  }
	  if(fParticle==3 && isItProt && fHCorrectMCposProt){
	    ptBinMC     = fHCorrectMCposProt->FindBin(trkPt);
	    binCont = fHCorrectMCposProt->GetBinContent(ptBinMC);
	    if(binCont!=0)  ptWgtMCProt = 1.0/binCont; 	    
	  }

	  ///NUA:
	  if(fHCorrectNUAposChrg[cForNUA]){
	    iBinNUA = fHCorrectNUAposChrg[cForNUA]->FindBin(pVtxZ,trkPhi,trkEta);
	    WgtNUA  = fHCorrectNUAposChrg[cForNUA]->GetBinContent(iBinNUA);
	  }
	  if(fParticle==1 && isItPion && fHCorrectNUAposPion[cForNUA]){
	    iBinNUA     = fHCorrectNUAposPion[cForNUA]->FindBin(pVtxZ,trkPhi,trkEta);
	    WgtNUAPion  = fHCorrectNUAposPion[cForNUA]->GetBinContent(iBinNUA);
	  }
	  if(fParticle==2 && isItKaon && fHCorrectNUAposKaon[cForNUA]){
	    iBinNUA     = fHCorrectNUAposKaon[cForNUA]->FindBin(pVtxZ,trkPhi,trkEta);
	    WgtNUAKaon  = fHCorrectNUAposKaon[cForNUA]->GetBinContent(iBinNUA);
	  }
	  if(fParticle==3 && isItProt && fHCorrectNUAposProt[cForNUA]){
	    iBinNUA     = fHCorrectNUAposProt[cForNUA]->FindBin(pVtxZ,trkPhi,trkEta);
	    WgtNUAProt  = fHCorrectNUAposProt[cForNUA]->GetBinContent(iBinNUA);
	  }       	  
	}
	else{
	  ///Tracking:
	  if(fHCorrectMCnegChrg){
	    ptBinMC = fHCorrectMCnegChrg->FindBin(trkPt);    	   
	    binCont = fHCorrectMCnegChrg->GetBinContent(ptBinMC);
	    if(binCont!=0) ptWgtMC = 1.0/binCont; 
	  }
	  if(fParticle==1 && isItPion && fHCorrectMCnegPion){
	    ptBinMC = fHCorrectMCnegPion->FindBin(trkPt);    
	    binCont = fHCorrectMCnegPion->GetBinContent(ptBinMC);
	    if(binCont!=0)  ptWgtMCPion = 1.0/binCont; 	    
	  }
	  if(fParticle==2 && isItKaon && fHCorrectMCnegKaon){
	    ptBinMC = fHCorrectMCnegKaon->FindBin(trkPt);
	    binCont = fHCorrectMCnegKaon->GetBinContent(ptBinMC);
	    if(binCont!=0)  ptWgtMCKaon = 1.0/binCont;
	  }
	  if(fParticle==3 && isItProt && fHCorrectMCnegProt){
	    ptBinMC     = fHCorrectMCnegProt->FindBin(trkPt);
	    binCont = fHCorrectMCnegProt->GetBinContent(ptBinMC);
	    if(binCont!=0)  ptWgtMCProt = 1.0/binCont; 	    
	  }
	  
	  ////NUA:
	  if(fHCorrectNUAnegChrg[cForNUA]){
	    iBinNUA = fHCorrectNUAnegChrg[cForNUA]->FindBin(pVtxZ,trkPhi,trkEta);
	    WgtNUA  = fHCorrectNUAnegChrg[cForNUA]->GetBinContent(iBinNUA);  
	  }
	  if(fParticle==1 && isItPion && fHCorrectNUAnegPion[cForNUA]){
	    iBinNUA     = fHCorrectNUAnegPion[cForNUA]->FindBin(pVtxZ,trkPhi,trkEta);
	    WgtNUAPion  = fHCorrectNUAnegPion[cForNUA]->GetBinContent(iBinNUA);
	  }
	  if(fParticle==2 && isItKaon && fHCorrectNUAnegKaon[cForNUA]){
	    iBinNUA     = fHCorrectNUAnegKaon[cForNUA]->FindBin(pVtxZ,trkPhi,trkEta);
	    WgtNUAKaon  = fHCorrectNUAnegKaon[cForNUA]->GetBinContent(iBinNUA);
	  }
	  if(fParticle==3 && isItProt && fHCorrectNUAnegProt[cForNUA]){
	    iBinNUA     = fHCorrectNUAnegProt[cForNUA]->FindBin(pVtxZ,trkPhi,trkEta);
	    WgtNUAProt  = fHCorrectNUAnegProt[cForNUA]->GetBinContent(iBinNUA);
	  }	  
	}

	/// infinity Weight protection:
	if(WgtNUA>1e3)     WgtNUA = 1.0;
	if(WgtNUAPion>1e3) WgtNUAPion = 1.0;
	if(WgtNUAKaon>1e3) WgtNUAPion = 1.0;
	if(WgtNUAProt>1e3) WgtNUAPion = 1.0;

	
	
	trkWgt     = WgtNUA*ptWgtMC;
	trkWgtPion = WgtNUAPion*ptWgtMCPion;
	trkWgtKaon = WgtNUAKaon*ptWgtMCKaon;
	trkWgtProt = WgtNUAProt*ptWgtMCProt;


	// if(iTrack%10==0){
	//  std::cout<<" pT = "<<trkPt<<"\t MCPi = "<<ptWgtMCPion<<"\t MCK = "<<ptWgtMCKaon<<"\t MCProt = "<<ptWgtMCProt<<std::endl;
	//  std::cout<<" Eta = "<<trkEta<<"\t NUAPi = "<<WgtNUAPion<<"\t NUAK = "<<WgtNUAKaon<<"\t NUAProt = "<<WgtNUAProt<<std::endl;
	//  std::cout<<" totPi = "<<trkWgtPion<<"\t totK = "<<trkWgtKaon<<"\t totP = "<<trkWgtProt<<"\t totChrg = "<<trkWgt<<std::endl;	 
	// }
	///----------------------------------------------------------------	




    
	uqRe = TMath::Cos(gPsiN*trkPhi);
	uqIm = TMath::Sin(gPsiN*trkPhi);
	
	//----> Remove Track from EP calculation ------
	sumQxTPCneg = fSumTPCQn2xNeg;   
	sumQyTPCneg = fSumTPCQn2yNeg;
	sumQxTPCpos = fSumTPCQn2xPos;
	sumQyTPCpos = fSumTPCQn2yPos;
    
	sumWgtneg = fSumWgtEtaNeg;
	sumWgtpos = fSumWgtEtaPos;

	//// remove AutoCorrelation:
	if(trkEta < fEtaGapNeg){
	  sumQxTPCneg -= trkWgt*uqRe;
	  sumQyTPCneg -= trkWgt*uqIm; 
	  sumWgtneg   -= trkWgt;
	}
	else if(trkEta > fEtaGapPos){
	  sumQxTPCpos -= trkWgt*uqRe;
	  sumQyTPCpos -= trkWgt*uqIm; 
	  sumWgtpos   -= trkWgt;
	}
	//---------------------------------------------

	
	fHistEtaPhiAfterCut->Fill(trkPhi,trkEta,trkWgt);




	///---------- pT cut for v2 vs Ach -------------
	
	//if(trkPt > 2.0) continue;


	
	
	if(trkChrg > 0){
	
	  fHistv2AchChrgPos[0][iCent]->Fill(fAchrgNet,  (uqRe*sumQxTPCneg + uqIm*sumQyTPCneg)/sumWgtneg, trkWgt); //This Trk weigth is for uQ	  

	  if(fParticle==0)
	    fHFillNUAPosPID[cForNUA]->Fill(pVtxZ,trkPhi,trkEta);


	  if(trkEta > fEtaGapPos){
	    sumQ2xChrgPosEtaPos += trkWgt*uqRe;
	    sumQ2yChrgPosEtaPos += trkWgt*uqIm;
	    NumOfChrgPosEtaPos  += trkWgt;
	  }
	  else if(trkEta < fEtaGapNeg){
	    sumQ2xChrgPosEtaNeg += trkWgt*uqRe;
	    sumQ2yChrgPosEtaNeg += trkWgt*uqIm;
	    NumOfChrgPosEtaNeg  += trkWgt;
	  }

	 
	  
	  if(fParticle==1 && isItPion){
	    fHistv2AchPionPos[0][iCent]->Fill(fAchrgNet, (uqRe*sumQxTPCneg + uqIm*sumQyTPCneg)/sumWgtneg, trkWgtPion);

	    if(trkEta > fEtaGapPos){
	      sumQ2xPionPosEtaPos += trkWgtPion*uqRe;
	      sumQ2yPionPosEtaPos += trkWgtPion*uqIm;
	      NumOfPionPosEtaPos  += trkWgtPion;
	    }
	    else if(trkEta < fEtaGapNeg){
	      sumQ2xPionPosEtaNeg += trkWgtPion*uqRe;
	      sumQ2yPionPosEtaNeg += trkWgtPion*uqIm;
	      NumOfPionPosEtaNeg  += trkWgtPion;
	    }
	    fHFillNUAPosPID[cForNUA]->Fill(pVtxZ,trkPhi,trkEta);
	  }
	  if(fParticle==2 && isItKaon){
	    fHistv2AchKaonPos[0][iCent]->Fill(fAchrgNet, (uqRe*sumQxTPCneg + uqIm*sumQyTPCneg)/sumWgtneg, trkWgtKaon);
	      
	    if(trkEta > fEtaGapPos){
	      sumQ2xKaonPosEtaPos += trkWgtKaon*uqRe;
	      sumQ2yKaonPosEtaPos += trkWgtKaon*uqIm;
	      NumOfKaonPosEtaPos  += trkWgtKaon;
	    }
	    else if(trkEta < fEtaGapNeg){
	      sumQ2xKaonPosEtaNeg += trkWgtKaon*uqRe;
	      sumQ2yKaonPosEtaNeg += trkWgtKaon*uqIm;
	      NumOfKaonPosEtaNeg  += trkWgtKaon;
	    }
	    fHFillNUAPosPID[cForNUA]->Fill(pVtxZ,trkPhi,trkEta);
	  }
	  if(fParticle==3 && isItProt){
	    fHistv2AchProtPos[0][iCent]->Fill(fAchrgNet, (uqRe*sumQxTPCneg + uqIm*sumQyTPCneg)/sumWgtneg, trkWgtProt);
	      
	    if(trkEta > fEtaGapPos){
	      sumQ2xProtPosEtaPos += trkWgtProt*uqRe;
	      sumQ2yProtPosEtaPos += trkWgtProt*uqIm;
	      NumOfProtPosEtaPos  += trkWgtProt;
	    }
	    else if(trkEta < fEtaGapNeg){
	      sumQ2xProtPosEtaNeg += trkWgtProt*uqRe;
	      sumQ2yProtPosEtaNeg += trkWgtProt*uqIm;
	      NumOfProtPosEtaNeg  += trkWgtProt;
	    }
	    fHFillNUAPosPID[cForNUA]->Fill(pVtxZ,trkPhi,trkEta);
	  }

	  
	  
	}///+ve Ch done	
	else{  //-Ve charge
	  
	  fHistv2AchChrgNeg[0][iCent]->Fill(fAchrgNet,   (uqRe*sumQxTPCneg + uqIm*sumQyTPCneg)/sumWgtneg, trkWgt);

	  fHFillNUANegPID[cForNUA]->Fill(pVtxZ,trkPhi,trkEta);
		    
	  if(trkEta > fEtaGapPos){
	    sumQ2xChrgNegEtaPos += trkWgt*uqRe;
	    sumQ2yChrgNegEtaPos += trkWgt*uqIm;
	    NumOfChrgNegEtaPos  += trkWgt;
	  }
	  else if(trkEta < fEtaGapNeg){
	    sumQ2xChrgNegEtaNeg += trkWgt*uqRe;
	    sumQ2yChrgNegEtaNeg += trkWgt*uqIm;
	    NumOfChrgNegEtaNeg  += trkWgt;
	  }
	   
	  if(fParticle==1 && isItPion){
	    fHistv2AchPionNeg[0][iCent]->Fill(fAchrgNet, (uqRe*sumQxTPCneg + uqIm*sumQyTPCneg)/sumWgtneg, trkWgtPion);

	    if(trkEta > fEtaGapPos){
	      sumQ2xPionNegEtaPos += trkWgtPion*uqRe;
	      sumQ2yPionNegEtaPos += trkWgtPion*uqIm;
	      NumOfPionNegEtaPos  += trkWgtPion;
	    }
	    else if(trkEta < fEtaGapNeg){
	      sumQ2xPionNegEtaNeg += trkWgtPion*uqRe;
	      sumQ2yPionNegEtaNeg += trkWgtPion*uqIm;
	      NumOfPionNegEtaNeg  += trkWgtPion;
	    }	   
	    fHFillNUANegPID[cForNUA]->Fill(pVtxZ,trkPhi,trkEta);	  
	  }
	  if(fParticle==2 && isItKaon){
	    fHistv2AchKaonNeg[0][iCent]->Fill(fAchrgNet, (uqRe*sumQxTPCneg + uqIm*sumQyTPCneg)/sumWgtneg, trkWgtKaon);

	    if(trkEta > fEtaGapPos){
	      sumQ2xKaonNegEtaPos += trkWgtKaon*uqRe;
	      sumQ2yKaonNegEtaPos += trkWgtKaon*uqIm;
	      NumOfKaonNegEtaPos  += trkWgtKaon;
	    }
	    else if(trkEta < fEtaGapNeg){
	      sumQ2xKaonNegEtaNeg += trkWgtKaon*uqRe;
	      sumQ2yKaonNegEtaNeg += trkWgtKaon*uqIm;
	      NumOfKaonNegEtaNeg  += trkWgtKaon;
	    }	  
	    fHFillNUANegPID[cForNUA]->Fill(pVtxZ,trkPhi,trkEta);	  
	  }
	  if(fParticle==3 && isItProt){
	    fHistv2AchProtNeg[0][iCent]->Fill(fAchrgNet, (uqRe*sumQxTPCneg + uqIm*sumQyTPCneg)/sumWgtneg, trkWgtProt);

	    if(trkEta > fEtaGapPos){
	      sumQ2xProtNegEtaPos += trkWgtProt*uqRe;
	      sumQ2yProtNegEtaPos += trkWgtProt*uqIm;
	      NumOfProtNegEtaPos  += trkWgtProt;
	    }
	    else if(trkEta < fEtaGapNeg){
	      sumQ2xProtNegEtaNeg += trkWgtProt*uqRe;
	      sumQ2yProtNegEtaNeg += trkWgtProt*uqIm;
	      NumOfProtNegEtaNeg  += trkWgtProt;
	    }
	    fHFillNUANegPID[cForNUA]->Fill(pVtxZ,trkPhi,trkEta);	  	    
	  }



	}/// if -ve Particle
	//----------- v2 vs Ach filled ---------


	
       
       

      }//with all trackCuts applied      
    }//-------> if FB is validated
    
  }///------> 2nd Track loop Ends here.<--------
 

  /// For cumulant method:


  /// Charge All(+-):

  Float_t eventwgtcharge=1.0; 
   if(fHCorrectEVNTWGTChrg){
     eventwgtcharge=fHCorrectEVNTWGTChrg->GetBinContent(fHCorrectEVNTWGTChrg->GetXaxis()->FindBin(centrality));
  }

  

   //cout<<"centrality is ------------------------>:"<<centrality<<" Event weight-------------------:"<<eventwgtcharge<<endl;                     

  
  //Double_t c2WeightChrg     = fSumWgtEtaPosWhole*fSumWgtEtaNegWhole;//without eventwgt
  Double_t c2WeightChrg     = fSumWgtEtaPosWhole*fSumWgtEtaNegWhole; //with eventwgt
  if (c2WeightChrg!=0.0)
    {

  Double_t c2cumulantChrg   = (fSumTPCQn2xPosWhole*fSumTPCQn2xNegWhole + fSumTPCQn2yPosWhole*fSumTPCQn2yNegWhole)/c2WeightChrg;
  fHistv2cumAchChrgAll[iCent]->Fill(fAchrgNet, c2cumulantChrg, c2WeightChrg*eventwgtcharge);   /// for denominator
    }

  // Double_t c2WeightChrgPos   =  NumOfChrgPosEtaPos*fSumWgtEtaNeg;   
  //Double_t c2WeightChrgNeg   =  NumOfChrgNegEtaPos*fSumWgtEtaNegChNeg;  //without eventwgt

  Double_t c2WeightChrgPos   =  NumOfChrgPosEtaPos*fSumWgtEtaNeg;
  Double_t c2WeightChrgNeg   =  NumOfChrgNegEtaPos*fSumWgtEtaNegChNeg;
  
  //  Double_t c2WeightChrgPosChrgNeg   =  NumOfChrgPosEtaPos*fSumWgtEtaNegChNeg; //positive charge correlation with negative charge in opp subevent
  //Double_t c2WeightChrgNegChrgPos   =  NumOfChrgNegEtaPos*fSumWgtEtaNeg;

    Double_t c2WeightChrgPosChrgNeg   =  NumOfChrgPosEtaPos*fSumWgtEtaNegChNeg; //positive charge correlation with negative charge in opp subevent
  Double_t c2WeightChrgNegChrgPos   =  NumOfChrgNegEtaPos*fSumWgtEtaNeg;
  

  //  Double_t c2WeightPionPos   =  NumOfPionPosEtaPos*fSumWgtEtaNeg;
  //Double_t c2WeightPionNeg   =  NumOfPionNegEtaPos*fSumWgtEtaNegChNeg;

  Double_t c2WeightPionPos   =  NumOfPionPosEtaPos*fSumWgtEtaNeg;
  Double_t c2WeightPionNeg   =  NumOfPionNegEtaPos*fSumWgtEtaNegChNeg;

  // Double_t c2WeightPionPosPionNeg   =  NumOfPionPosEtaPos*fSumWgtEtaNegChNeg;
  //Double_t c2WeightPionNegPionPos   =  NumOfPionNegEtaPos*fSumWgtEtaNeg;

  Double_t c2WeightPionPosPionNeg   =  NumOfPionPosEtaPos*fSumWgtEtaNegChNeg;
  Double_t c2WeightPionNegPionPos   =  NumOfPionNegEtaPos*fSumWgtEtaNeg;


  Double_t c2WeightKaonPos   =  NumOfKaonPosEtaPos*fSumWgtEtaNeg;
  Double_t c2WeightKaonNeg   =  NumOfKaonNegEtaPos*fSumWgtEtaNegChNeg;

  Double_t c2WeightKaonPosKaonNeg   =  NumOfKaonPosEtaPos*fSumWgtEtaNegChNeg;
  Double_t c2WeightKaonNegKaonPos   =  NumOfKaonNegEtaPos*fSumWgtEtaNeg;

  Double_t c2WeightProtPos   =  NumOfProtPosEtaPos*fSumWgtEtaNeg;
  Double_t c2WeightProtNeg   =  NumOfProtNegEtaPos*fSumWgtEtaNegChNeg;

  Double_t c2WeightProtPosProtNeg   =  NumOfProtPosEtaPos*fSumWgtEtaNegChNeg;
  Double_t c2WeightProtNegProtPos   =  NumOfProtNegEtaPos*fSumWgtEtaNeg;
  
  
  if((NumOfChrgPosEtaPos*fSumWgtEtaNeg)!=0.0){
    ///Chrg+:  
    //Double_t c2WeightChrgPos   =  NumOfChrgPosEtaPos*fSumWgtEtaNeg;
    Double_t c2cumulantChrgPos =  (sumQ2xChrgPosEtaPos*fSumTPCQn2xNeg + sumQ2yChrgPosEtaPos*fSumTPCQn2yNeg)/c2WeightChrgPos;
    fHistv2AchChrgPos[1][iCent]->Fill(fAchrgNet, c2cumulantChrgPos, c2WeightChrgPos*eventwgtcharge);   /// for denominator
  }
  
  if((NumOfChrgNegEtaPos*fSumWgtEtaNegChNeg)!=0.0){  
    ///Chrg-:  
    //Double_t c2WeightChrgNeg   =  NumOfChrgNegEtaPos*fSumWgtEtaNegChNeg;
    Double_t c2cumulantChrgNeg =  (sumQ2xChrgNegEtaPos*fSumTPCQn2xNegChNeg + sumQ2yChrgNegEtaPos*fSumTPCQn2yNegChNeg)/c2WeightChrgNeg;
    fHistv2AchChrgNeg[1][iCent]->Fill(fAchrgNet, c2cumulantChrgNeg, c2WeightChrgNeg*eventwgtcharge);   /// for denominator
  }
  


  if((c2WeightChrgPosChrgNeg)!=0.0){
    //Chrg+: opp correlation
Double_t c2cumulantChrgPosChrgNeg =  (sumQ2xChrgPosEtaPos*fSumTPCQn2xNegChNeg + sumQ2yChrgPosEtaPos*fSumTPCQn2yNegChNeg)/c2WeightChrgPosChrgNeg;
 fHistv2AchChrgPosChrgNeg[1][iCent]->Fill(fAchrgNet, c2cumulantChrgPosChrgNeg, c2WeightChrgPosChrgNeg*eventwgtcharge);   /// for denominator
  }


  if((c2WeightChrgNegChrgPos)!=0.0){  
    ///Chrg-:  
    Double_t c2cumulantChrgNegChrgPos =  (sumQ2xChrgNegEtaPos*fSumTPCQn2xNeg + sumQ2yChrgNegEtaPos*fSumTPCQn2yNeg)/c2WeightChrgNegChrgPos;
    fHistv2AchChrgNegChrgPos[1][iCent]->Fill(fAchrgNet, c2cumulantChrgNegChrgPos, c2WeightChrgNegChrgPos*eventwgtcharge);   /// for denominator
  }
    



  
  if(fParticle==1){
    if((NumOfPionPosEtaPos*fSumWgtEtaNeg)!=0.0){
      ///Pion+:  
      //Double_t c2WeightPionPos   =  NumOfPionPosEtaPos*fSumWgtEtaNeg;
      Double_t c2cumulantPionPos =  (sumQ2xPionPosEtaPos*fSumTPCQn2xNeg + sumQ2yPionPosEtaPos*fSumTPCQn2yNeg)/c2WeightPionPos;
      fHistv2AchPionPos[1][iCent]->Fill(fAchrgNet, c2cumulantPionPos, c2WeightPionPos*eventwgtcharge);   /// for denominator
    }
    if((NumOfPionNegEtaPos*fSumWgtEtaNegChNeg)!=0.0){  
      ///Pion-:  
      //Double_t c2WeightPionNeg   =  NumOfPionNegEtaPos*fSumWgtEtaNegChNeg;
      Double_t c2cumulantPionNeg =  (sumQ2xPionNegEtaPos*fSumTPCQn2xNegChNeg + sumQ2yPionNegEtaPos*fSumTPCQn2yNegChNeg)/c2WeightPionNeg;
      fHistv2AchPionNeg[1][iCent]->Fill(fAchrgNet, c2cumulantPionNeg, c2WeightPionNeg*eventwgtcharge);   /// for denominator
    }


    if((c2WeightPionPosPionNeg)!=0.0){
      ///Pion+:  
      Double_t c2cumulantPionPosPionNeg =  (sumQ2xPionPosEtaPos*fSumTPCQn2xNegChNeg + sumQ2yPionPosEtaPos*fSumTPCQn2yNegChNeg)/c2WeightPionPosPionNeg;
      fHistv2AchPionPosPionNeg[1][iCent]->Fill(fAchrgNet, c2cumulantPionPosPionNeg, c2WeightPionPosPionNeg*eventwgtcharge);   /// for denominator
    }


     if((c2WeightPionNegPionPos)!=0.0){
       //Pion-
    Double_t c2cumulantPionNegPionPos =  (sumQ2xPionNegEtaPos*fSumTPCQn2xNeg + sumQ2yPionNegEtaPos*fSumTPCQn2yNeg)/c2WeightPionNegPionPos;
    fHistv2AchPionNegPionPos[1][iCent]->Fill(fAchrgNet, c2cumulantPionNegPionPos, c2WeightPionNegPionPos*eventwgtcharge);   /// for denominator
    }
  }

  

  if(fParticle==2){
    if((NumOfKaonPosEtaPos*fSumWgtEtaNeg)!=0.0){
      ///Kaon+:  
      Double_t c2WeightKaonPos   =  NumOfKaonPosEtaPos*fSumWgtEtaNeg;
      Double_t c2cumulantKaonPos =  (sumQ2xKaonPosEtaPos*fSumTPCQn2xNeg + sumQ2yKaonPosEtaPos*fSumTPCQn2yNeg)/c2WeightKaonPos;
      fHistv2AchKaonPos[1][iCent]->Fill(fAchrgNet, c2cumulantKaonPos, c2WeightKaonPos);   /// for denominator
    }
    if((NumOfKaonNegEtaPos*fSumWgtEtaNegChNeg)!=0.0){
      ///Kaon-:  
      Double_t c2WeightKaonNeg   =  NumOfKaonNegEtaPos*fSumWgtEtaNegChNeg;
      Double_t c2cumulantKaonNeg =  (sumQ2xKaonNegEtaPos*fSumTPCQn2xNegChNeg + sumQ2yKaonNegEtaPos*fSumTPCQn2yNegChNeg)/c2WeightKaonNeg;
      fHistv2AchKaonNeg[1][iCent]->Fill(fAchrgNet, c2cumulantKaonNeg, c2WeightKaonNeg);   /// for denominator
    }


    if((c2WeightKaonNegKaonPos)!=0.0){
      ///Kaon:  
      //Double_t c2WeightKaonNeg   =  NumOfKaonNegEtaPos*fSumWgtEtaNegChNeg;
      Double_t c2cumulantKaonNegKaonPos =  (sumQ2xKaonNegEtaPos*fSumTPCQn2xNeg + sumQ2yKaonNegEtaPos*fSumTPCQn2yNeg)/c2WeightKaonNegKaonPos;
      fHistv2AchKaonNegKaonPos[1][iCent]->Fill(fAchrgNet, c2cumulantKaonNegKaonPos, c2WeightKaonNegKaonPos);   /// for denominator
    }

    if((c2WeightKaonPosKaonNeg)!=0.0){
      ///Kaon:  
      Double_t c2cumulantKaonPosKaonNeg =  (sumQ2xKaonPosEtaPos*fSumTPCQn2xNegChNeg + sumQ2yKaonPosEtaPos*fSumTPCQn2yNegChNeg)/c2WeightKaonPosKaonNeg;
      fHistv2AchKaonPosKaonNeg[1][iCent]->Fill(fAchrgNet, c2cumulantKaonPosKaonNeg, c2WeightKaonPosKaonNeg);   /// for denominator
    }


    
  }

  if(fParticle==3){
    if((NumOfProtPosEtaPos*fSumWgtEtaNeg)!=0.0){
      ///Prot+:  
      Double_t c2WeightProtPos   =  NumOfProtPosEtaPos*fSumWgtEtaNeg;
      Double_t c2cumulantProtPos =  (sumQ2xProtPosEtaPos*fSumTPCQn2xNeg + sumQ2yProtPosEtaPos*fSumTPCQn2yNeg)/c2WeightProtPos;
      fHistv2AchProtPos[1][iCent]->Fill(fAchrgNet, c2cumulantProtPos, c2WeightProtPos);   /// for denominator
    }

    if((NumOfProtNegEtaPos*fSumWgtEtaNegChNeg)!=0.0){
      ///Prot-:  
      Double_t c2WeightProtNeg   =  NumOfProtNegEtaPos*fSumWgtEtaNegChNeg;
      Double_t c2cumulantProtNeg =  (sumQ2xProtNegEtaPos*fSumTPCQn2xNegChNeg + sumQ2yProtNegEtaPos*fSumTPCQn2yNegChNeg)/c2WeightProtNeg;
      fHistv2AchProtNeg[1][iCent]->Fill(fAchrgNet, c2cumulantProtNeg, c2WeightProtNeg);   /// for denominator
    }



    if((c2WeightProtNegProtPos)!=0.0){
      ///Prot:  
      Double_t c2cumulantProtNegProtPos =  (sumQ2xProtNegEtaPos*fSumTPCQn2xNeg + sumQ2yProtNegEtaPos*fSumTPCQn2yNeg)/c2WeightProtNegProtPos;
      fHistv2AchProtNegProtPos[1][iCent]->Fill(fAchrgNet, c2cumulantProtNegProtPos, c2WeightProtNegProtPos);   /// for denominator
    }

    if((c2WeightProtPosProtNeg)!=0.0){
      ///Prot:  
      Double_t c2cumulantProtPosProtNeg =  (sumQ2xProtPosEtaPos*fSumTPCQn2xNegChNeg + sumQ2yProtPosEtaPos*fSumTPCQn2yNegChNeg)/c2WeightProtPosProtNeg;
      fHistv2AchProtPosProtNeg[1][iCent]->Fill(fAchrgNet, c2cumulantProtPosProtNeg, c2WeightProtPosProtNeg);   /// for denominator
    }
    
    
  }
  

  
  //Last lines in Event loop
  fCentDistAfterCut->Fill(centrality);
  fHistEventCount->Fill(14.5); //final Event count.
  PostData(1,fListHist);

  // std::cout<<" Info:UserExec()  Call Finished ..!!!\n";
}//---------------- UserExec ----------------------








Bool_t AliAnalysisTaskCMWPU2018::CheckEventIsPileUp2018(AliAODEvent *faod) {



  TF1 *fSPDCutPU = new TF1("fSPDCutPU", "400. + 4.*x", 0, 10000);
    

  Double_t parV0[8] = {43.8011, 0.822574, 8.49794e-02, 1.34217e+02, 7.09023e+00, 4.99720e-02, -4.99051e-04, 1.55864e-06};
  TF1 *fV0CutPU = new TF1("fV0CutPU", "[0]+[1]*x - 6.*[2]*([3] + [4]*sqrt(x) + [5]*x + [6]*x*sqrt(x) + [7]*x*x)", 0, 100000);
  fV0CutPU->SetParameters(parV0);
    

  Double_t parV0CL0[6] = {0.320462, 0.961793, 1.02278, 0.0330054, -0.000719631, 6.90312e-06};
  TF1 *fCenCutLowPU = new TF1("fCenCutLowPU", "[0]+[1]*x - 6.5*([2]+[3]*x+[4]*x*x+[5]*x*x*x)", 0, 100);
  fCenCutLowPU->SetParameters(parV0CL0);
  TF1 *fCenCutHighPU = new TF1("fCenCutHighPU", "[0]+[1]*x + 5.5*([2]+[3]*x+[4]*x*x+[5]*x*x*x)", 0, 100);
  fCenCutHighPU->SetParameters(parV0CL0);
    

  Double_t parFB32[8] = {2093.36, -66.425, 0.728932, -0.0027611, 1.01801e+02, -5.23083e+00, -1.03792e+00, 5.70399e-03};
  TF1 *fMultCutPU = new TF1("fMultCutPU", "[0]+[1]*x+[2]*x*x+[3]*x*x*x - 6.*([4]+[5]*sqrt(x)+[6]*x+[7]*x*x)", 0, 90);
  fMultCutPU->SetParameters(parFB32);




  Bool_t BisPileup=kFALSE;


  Double_t centrV0M=300;
  Double_t centrCL1=300;
  Double_t centrCL0=300;

  fMultSelection = (AliMultSelection*) InputEvent()->FindListObject("MultSelection");
 
  if(!fMultSelection) {
    printf("\n\n **WARNING** ::UserExec() AliMultSelection object not found.\n\n");
    exit(1);
  }


  centrV0M = fMultSelection->GetMultiplicityPercentile("V0M");
  centrCL1 = fMultSelection->GetMultiplicityPercentile("CL1");
  centrCL0 = fMultSelection->GetMultiplicityPercentile("CL0");



  Int_t nITSClsLy0 = faod->GetNumberOfITSClusters(0);
  Int_t nITSClsLy1 = faod->GetNumberOfITSClusters(1);
  Int_t nITSCls = nITSClsLy0 + nITSClsLy1;
    

  AliAODTracklets* aodTrkl = (AliAODTracklets*)faod->GetTracklets();
  Int_t nITSTrkls = aodTrkl->GetNumberOfTracklets();


  const Int_t nTracks = faod->GetNumberOfTracks();
    

  Int_t multTrk = 0;
    

  for (Int_t it = 0; it < nTracks; it++) {
        

    AliAODTrack* aodTrk = (AliAODTrack*)faod->GetTrack(it);
        

    if (!aodTrk){
      delete aodTrk;
      continue;
    }


        

    if (aodTrk->TestFilterBit(32)){
      if ((TMath::Abs(aodTrk->Eta()) < 0.8) && (aodTrk->GetTPCNcls() >= 70) && (aodTrk->Pt() >= 0.2))
	multTrk++;
            
    }     

  }




    

  AliAODVZERO* aodV0 = faod->GetVZEROData();
  Float_t multV0a = aodV0->GetMTotV0A();
  Float_t multV0c = aodV0->GetMTotV0C();
  Float_t multV0Tot = multV0a + multV0c;
  UShort_t multV0aOn = aodV0->GetTriggerChargeA();
  UShort_t multV0cOn = aodV0->GetTriggerChargeC();
  UShort_t multV0On = multV0aOn + multV0cOn;

  Int_t tpcClsTot = faod->GetNumberOfTPCClusters();
  Float_t nclsDif = Float_t(tpcClsTot) - (60932.9 + 69.2897*multV0Tot - 0.000217837*multV0Tot*multV0Tot);


  if (centrCL0 < fCenCutLowPU->Eval(centrV0M))
    BisPileup=kTRUE;

  if (centrCL0 > fCenCutHighPU->Eval(centrV0M))
    BisPileup=kTRUE;        


  if (Float_t(nITSCls) > fSPDCutPU->Eval(nITSTrkls))
    BisPileup=kTRUE;
        

  if (multV0On < fV0CutPU->Eval(multV0Tot))
    BisPileup=kTRUE;


  if (Float_t(multTrk) < fMultCutPU->Eval(centrV0M))
    BisPileup=kTRUE;


        

  if (((AliAODHeader*)faod->GetHeader())->GetRefMultiplicityComb08() < 0)
    BisPileup=kTRUE;


  if (faod->IsIncompleteDAQ())
    BisPileup=kTRUE;
        
  //if (nclsDif > 200000)//can be increased to 200000
  // BisPileup=kTRUE;



  fHistTPConlyVsV0MBefore->Fill(centrV0M,multTrk);
  fHistCL0VsV0MBefore->Fill(centrV0M,centrCL0);


  if (!BisPileup)
    {
      fHistTPConlyVsV0MAfter->Fill(centrV0M,multTrk);
      fHistCL0VsV0MAfter->Fill(centrV0M,centrCL0);
    }







 



 return BisPileup; 


}














Bool_t AliAnalysisTaskCMWPU2018::CheckEventIsPileUp(AliAODEvent *faod) {




  cout<<"---------------------------hi i am in 2015 dataset-------------------------------------"<<endl;


  Bool_t BisPileup=kFALSE;


  Double_t centrV0M=300;
  Double_t centrCL1=300;
  Double_t centrCL0=300;
  Double_t centrTRK=300;

  fMultSelection = (AliMultSelection*) InputEvent()->FindListObject("MultSelection");
 
  if(!fMultSelection) {
    printf("\n\n **WARNING** ::UserExec() AliMultSelection object not found.\n\n");
    exit(1);
  }


  centrV0M = fMultSelection->GetMultiplicityPercentile("V0M");
  centrCL1 = fMultSelection->GetMultiplicityPercentile("CL1");
  centrCL0 = fMultSelection->GetMultiplicityPercentile("CL0");
  centrTRK = fMultSelection->GetMultiplicityPercentile("TRK");


  //-- pile-up a la Dobrin for LHC15o -----
  if(PileUpMultiVertex(faod)) {
    fHistPileUpCount->Fill(0.5);    
    BisPileup=kTRUE;
  }
  Int_t isPileup = faod->IsPileupFromSPD(3);
  if(isPileup != 0) {
    fHistPileUpCount->Fill(1.5);
    BisPileup=kTRUE;          
  }
  if(((AliAODHeader*)faod->GetHeader())->GetRefMultiplicityComb08() < 0) {
    fHistPileUpCount->Fill(2.5);
    BisPileup=kTRUE;
  }
  if(faod->IsIncompleteDAQ())  {
    fHistPileUpCount->Fill(3.5);
    BisPileup=kTRUE;
  }
  if(fabs(centrV0M-centrCL1)> 5.0)  {//default: 7.5
    fHistPileUpCount->Fill(4.5);
    BisPileup=kTRUE;
  }



  // check vertex consistency
  const AliAODVertex* vtTrc = faod->GetPrimaryVertex();
  const AliAODVertex* vtSPD = faod->GetPrimaryVertexSPD();

  if(vtTrc->GetNContributors() < 2 || vtSPD->GetNContributors()<1) {
    fHistPileUpCount->Fill(5.5);
    BisPileup=kTRUE;
  }

  double covTrc[6], covSPD[6];
  vtTrc->GetCovarianceMatrix(covTrc);
  vtSPD->GetCovarianceMatrix(covSPD);

  double dz = vtTrc->GetZ() - vtSPD->GetZ();

  double errTot = TMath::Sqrt(covTrc[5]+covSPD[5]);
  double errTrc = TMath::Sqrt(covTrc[5]);
  double nsigTot = dz/errTot;
  double nsigTrc = dz/errTrc;

  if(TMath::Abs(dz)>0.2 || TMath::Abs(nsigTot)>10 || TMath::Abs(nsigTrc)>20)  {
    fHistPileUpCount->Fill(6.5);
    BisPileup=kTRUE;
  }




  Int_t multTPC = 0;
  Int_t multTPCAll = 0;
  Int_t multITSfb96 = 0;
  Int_t multITSfb32 = 0;

  Int_t multTPCFE = 0;
  Int_t multGlobal = 0;
  Int_t multTPCuncut = 0;

  Int_t multEsd = ((AliAODHeader*)faod->GetHeader())->GetNumberOfESDTracks();

  const Int_t nTracks = faod->GetNumberOfTracks();

  for(Int_t iTracks = 0; iTracks < nTracks; iTracks++) {
    //AliNanoAODTrack* track = dynamic_cast<AliNanoAODTrack*>(faod->GetTrack(iTracks));
    AliAODTrack* track = (AliAODTrack*)faod->GetTrack(iTracks);
    if(!track)  continue;
  
    //---------- old method -----------
    if(track->TestFilterBit(128))
      multTPCAll++;
    //if(track->TestFilterBit(96))
    //multITSfb96++;


    //----------------------------------
    if(track->TestFilterBit(1))  multTPCuncut++;
    if(track->TestFilterBit(32)) multITSfb32++;


    if(track->Pt()<0.2 || track->Pt()>10.0 || TMath::Abs(track->Eta())>0.8 || track->GetTPCNcls()<fTPCclustMin || track->GetTPCsignal()<10.0)
      continue;
    if(track->GetDetPid() && track->Chi2perNDF() > 0.2) multTPC++;
    if(track->TestFilterBit(1) && track->Chi2perNDF()>0.2)  multTPCFE++;
    if(!track->TestFilterBit(16) || track->Chi2perNDF()<0.1)   continue;
                
    Double_t b[2]    = {-99., -99.};
    Double_t bCov[3] = {-99., -99., -99.};
                
    AliAODTrack copy(*track);
    Double_t magField = faod->GetMagneticField();
                
    if(magField!=0){     
      if(track->PropagateToDCA(faod->GetPrimaryVertex(), magField, 100., b, bCov) && TMath::Abs(b[0]) < 0.3 && TMath::Abs(b[1]) < 0.3) 
	multGlobal++;    
    }
  }




  //fixed for test:
  // Double_t fPileUpSlopeParm = 3.43;
  //Double_t fPileUpConstParm = 43;

  Double_t multESDTPCDif  = multEsd  - fPileUpSlopeParm*multTPCAll;
  

  Bool_t  bIsOutLier=kFALSE;
  if(multTPC < (-20.0+1.15*multGlobal) || multTPC > (200.+1.45*multGlobal)) { bIsOutLier = kTRUE;}

  //  fHistEventCount->Fill(stepCount); //4
  //stepCount++;





  fTPCvsGlobalTrkBefore->Fill(multGlobal,multTPC);
  fHistTPCVsESDTrkBefore->Fill(multTPCAll,multEsd);   //A. Dobrin
  fHistTPConlyVsCL1Before->Fill(centrCL1,multTPCAll);
  fHistTPConlyVsV0MBefore->Fill(centrV0M,multTPCAll);
  fHistGlobalVsV0MBefore->Fill(centrV0M, multGlobal);



  if(multESDTPCDif > fPileUpConstParm) { 
    fHistPileUpCount->Fill(7.5);
    BisPileup=kTRUE;
  }
  if(BisPileup==kFALSE) {
    if(!fMultSelection->GetThisEventIsNotPileup())  BisPileup=kTRUE;
    if(!fMultSelection->GetThisEventIsNotPileupMV()) BisPileup=kTRUE;
    if(!fMultSelection->GetThisEventIsNotPileupInMultBins()) BisPileup=kTRUE;
    if(!fMultSelection->GetThisEventHasNoInconsistentVertices()) BisPileup=kTRUE;
    if(!fMultSelection->GetThisEventPassesTrackletVsCluster()) BisPileup=kTRUE;
    if(!fMultSelection->GetThisEventIsNotIncompleteDAQ()) BisPileup=kTRUE;
    if(!fMultSelection->GetThisEventHasGoodVertex2016()) BisPileup=kTRUE;
    if(BisPileup)     fHistPileUpCount->Fill(9.5);
  }  


  if (!bIsOutLier){
    fTPCvsGlobalTrkAfter->Fill(multGlobal,multTPC);

    //fHistEventCount->Fill(stepCount); //5
    //stepCount++;

  }
    if(!BisPileup){
    fHistTPCVsESDTrkAfter->Fill(multTPCAll,multEsd);  
    fHistTPConlyVsCL1After->Fill(centrCL1,multTPCAll);
    fHistTPConlyVsV0MAfter->Fill(centrV0M,multTPCAll);
    fHistGlobalVsV0MAfter->Fill(centrV0M, multGlobal);
  
    //fHistEventCount->Fill(stepCount); //6
    //stepCount++
}


  return BisPileup; 
} //-------pile up function ------









Bool_t AliAnalysisTaskCMWPU2018::PileUpMultiVertex(const AliAODEvent* faod)
{  // check for multi-vertexer pile-up
  const int    kMinPlpContrib = 5;
  const double kMaxPlpChi2    = 5.0;
  const double kMinWDist      = 15;

  const AliVVertex* vtPrm = 0;
  const AliVVertex* vtPlp = 0;

  int nPlp = 0;

  if(!(nPlp=faod->GetNumberOfPileupVerticesTracks()))
    return kFALSE;

  vtPrm = faod->GetPrimaryVertex();
  if(vtPrm == faod->GetPrimaryVertexSPD())
    return kTRUE;  // there are pile-up vertices but no primary

  //int bcPrim = vtPrm->GetBC();

  for(int ipl=0;ipl<nPlp;ipl++) {
    vtPlp = (const AliVVertex*)faod->GetPileupVertexTracks(ipl);
    if (vtPlp->GetNContributors() < kMinPlpContrib) continue;
    if (vtPlp->GetChi2perNDF()    > kMaxPlpChi2)    continue;
    //int bcPlp = vtPlp->GetBC();
    //if (bcPlp!=AliVTrack::kTOFBCNA && TMath::Abs(bcPlp-bcPrim)>2)
    // return kTRUE; // pile-up from other BC

    double wDst = GetWDist(vtPrm,vtPlp);
    if (wDst<kMinWDist)        continue;

    return kTRUE; // pile-up: well separated vertices
  }
  return kFALSE;
}




double AliAnalysisTaskCMWPU2018::GetWDist(const AliVVertex* v0, const AliVVertex* v1)
{
  // calculate sqrt of weighted distance to other vertex
  if (!v0 || !v1) {
    AliDebug(2,"\n\n ::GetWDist => One of vertices is not valid\n\n");
    return 0;
  }
  static TMatrixDSym vVb(3);
  double dist = -1;
  double dx = v0->GetX()-v1->GetX();
  double dy = v0->GetY()-v1->GetY();
  double dz = v0->GetZ()-v1->GetZ();
  double cov0[6],cov1[6];
  v0->GetCovarianceMatrix(cov0);
  v1->GetCovarianceMatrix(cov1);
  vVb(0,0) = cov0[0]+cov1[0];
  vVb(1,1) = cov0[2]+cov1[2];
  vVb(2,2) = cov0[5]+cov1[5];
  vVb(1,0) = vVb(0,1) = cov0[1]+cov1[1];
  vVb(0,2) = vVb(1,2) = vVb(2,0) = vVb(2,1) = 0.;
  vVb.InvertFast();
  if (!vVb.IsValid()) {
    AliDebug(2,"Singular Matrix\n");
    return dist;
  }
  dist = vVb(0,0)*dx*dx + vVb(1,1)*dy*dy + vVb(2,2)*dz*dz
    +    2*vVb(0,1)*dx*dy + 2*vVb(0,2)*dx*dz + 2*vVb(1,2)*dy*dz;
  return dist>0 ? TMath::Sqrt(dist) : -1;

}











void AliAnalysisTaskCMWPU2018::SetupEventAndTaskConfigInfo(){




  fHistPileUpCount = new TH1F("fHistPileUpCount", "fHistPileUpCount", 15, 0., 15.);
  fHistPileUpCount->GetXaxis()->SetBinLabel(1,"plpMV");
  fHistPileUpCount->GetXaxis()->SetBinLabel(2,"fromSPD");
  fHistPileUpCount->GetXaxis()->SetBinLabel(3,"RefMultComb08");
  fHistPileUpCount->GetXaxis()->SetBinLabel(4,"IncompleteDAQ");
  fHistPileUpCount->GetXaxis()->SetBinLabel(5,"abs(V0M-CL1)>5.0");
  fHistPileUpCount->GetXaxis()->SetBinLabel(6,"missingVtx");
  fHistPileUpCount->GetXaxis()->SetBinLabel(7,"inconsistentVtx");
  Int_t puConst = fPileUpConstParm;
  //Int_t puConst = 3.0;
  fHistPileUpCount->GetXaxis()->SetBinLabel(8,Form("multESDTPCDif>%d",puConst));
  fHistPileUpCount->GetXaxis()->SetBinLabel(9,Form("multGlobTPCDif>%d",puConst));
  fHistPileUpCount->GetXaxis()->SetBinLabel(10,"PileUpMultSelTask");
  fListHist->Add(fHistPileUpCount);







  fHistEventCount = new TH1F("fHistEventCount","Event counts",15,0,15);
  fHistEventCount->GetXaxis()->SetBinLabel(1,"Called UserExec()");
  fHistEventCount->GetXaxis()->SetBinLabel(2,"Called Exec()");
  //fHistEventCount->GetXaxis()->SetBinLabel(3,"AOD Exist");
  fHistEventCount->GetXaxis()->SetBinLabel(3,"Vz < 10");
  fHistEventCount->GetXaxis()->SetBinLabel(4,"TPC vs Global");
  fHistEventCount->GetXaxis()->SetBinLabel(5,"TPC128 vs ESD");
  fHistEventCount->GetXaxis()->SetBinLabel(6,"Cent vs TPC");
  //fHistEventCount->GetXaxis()->SetBinLabel(8,Form("%2.0f<Cent<%2.0f",fCentralityMin,fCentralityMax));
  fHistEventCount->GetXaxis()->SetBinLabel(7,"noAODtrack > 2 ");  
  fHistEventCount->GetXaxis()->SetBinLabel(8,"cent < 90");  
  fHistEventCount->GetXaxis()->SetBinLabel(9,"mult eta+/- > 2");
  fHistEventCount->GetXaxis()->SetBinLabel(15,"Survived Events");
  fListHist->Add(fHistEventCount);
  //fHistEventCount->Fill(1);
}//----------SetupEventAndTaskConfigInfo-----------




Int_t AliAnalysisTaskCMWPU2018::GetCentralityScaled0to10(Double_t fCent){

 Int_t cIndex = 0;

 if(fCent<5.0) {
   cIndex  = 0; 
 }
 else if(fCent>=5.0 && fCent<10){
   cIndex  = 1;
 }
 else if(fCent>=10.0) {
   cIndex = abs(fCent/10.0)+1;
 }
 return cIndex;
 
}//------------GetCentralityScaled0to10------------

void AliAnalysisTaskCMWPU2018::GetNUACorrectionHist(Int_t run, Int_t kParticleID)
{

  if(fListNUACorr){

    if(kParticleID==0){ //charge
      for(int i=0;i<5;i++){
	fHCorrectNUAposChrg[i] = (TH3F *) fListNUACorr->FindObject(Form("fHist_NUA_VzPhiEta_Charge_Pos_Cent%d_Run%d",i,run)); 
	fHCorrectNUAnegChrg[i] = (TH3F *) fListNUACorr->FindObject(Form("fHist_NUA_VzPhiEta_Charge_Neg_Cent%d_Run%d",i,run));
      }
    }
    else if(kParticleID==1){ //Pion
      for(int i=0;i<5;i++){
	fHCorrectNUAposPion[i] = (TH3F *) fListNUACorr->FindObject(Form("fHist_NUA_VzPhiEta_Pion_Pos_Cent%d_Run%d",i,run)); 
	fHCorrectNUAnegPion[i] = (TH3F *) fListNUACorr->FindObject(Form("fHist_NUA_VzPhiEta_Pion_Neg_Cent%d_Run%d",i,run));
      }
    }
    else if(kParticleID==2){ //Kaon
      for(int i=0;i<5;i++){
	fHCorrectNUAposKaon[i] = (TH3F *) fListNUACorr->FindObject(Form("fHist_NUA_VzPhiEta_Kaon_Pos_Cent%d_Run%d",i,run)); 
	fHCorrectNUAnegKaon[i] = (TH3F *) fListNUACorr->FindObject(Form("fHist_NUA_VzPhiEta_Kaon_Neg_Cent%d_Run%d",i,run));
      }
    }
    else if(kParticleID==3){ //Proton
      for(int i=0;i<5;i++){
	fHCorrectNUAposProt[i] = (TH3F *) fListNUACorr->FindObject(Form("fHist_NUA_VzPhiEta_Proton_Pos_Cent%d_Run%d",i,run)); 
	fHCorrectNUAnegProt[i] = (TH3F *) fListNUACorr->FindObject(Form("fHist_NUA_VzPhiEta_Proton_Neg_Cent%d_Run%d",i,run));
      }
    }
    else{
      for(int i=0;i<5;i++){
	fHCorrectNUAposChrg[i]=NULL;
	fHCorrectNUAnegChrg[i]=NULL;
	fHCorrectNUAposPion[i]=NULL;
	fHCorrectNUAnegPion[i]=NULL;
	fHCorrectNUAposKaon[i]=NULL;
	fHCorrectNUAnegKaon[i]=NULL;
	fHCorrectNUAposProt[i]=NULL;
	fHCorrectNUAnegProt[i]=NULL;
      }
    }
    
  }//------> if list Exist

  // else {
  //   printf("\n ******** Warning: No NUA Correction File/List...!! \n Run= %d, Use NUA Wgt = 1.0 ********* \n",run);
  // }
}







void AliAnalysisTaskCMWPU2018::GetV0MCorrectionHist(Int_t run, Int_t kParticleID)
{

  if(fListV0MCorr){
    //cout<<"*****************AT LAST I GOT INTO EVNTWGT CORRECTION FUNCTION******************************"<<endl;
    if(kParticleID==0){ //charge
    fHCorrectEVNTWGTChrg = (TH1F *) fListV0MCorr->FindObject(Form("hwgtCharge_Run%d",run)); 	
    

    //cout<<"Run no:"<<run<<endl;

}
  }
    else{
      // cout<<"*****************AT LAST I GOT INTO EVNTWGT CORRECTION FUNCTION BUT LIST NOT PRESENT******************************"<<endl;
      fHCorrectEVNTWGTChrg=NULL;
    }
}
    
 



////---------- SetUp Tracking Efficiency Correction Map ---------------

void AliAnalysisTaskCMWPU2018::GetMCCorrectionHist(Int_t run,Float_t centr){

  if(fListTRKCorr) {
    //cout<<"\n =========> Info: Found TList with MC Tracking Corr Histograms <=========== "<<endl;
    /// Default: Centrality Independent MC efficiency:
    
    fHCorrectMCposChrg =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyChrgPos");
    fHCorrectMCposPion =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyPionPos");
    fHCorrectMCposKaon =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyKaonPos");
    fHCorrectMCposProt =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyProtPos");

    fHCorrectMCnegChrg =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyChrgNeg");
    fHCorrectMCnegPion =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyPionNeg");
    fHCorrectMCnegKaon =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyKaonNeg");
    fHCorrectMCnegProt =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyProtNeg");
    
    
    /// Centrality dependent MC efficiency: (Temporary)
    /*if(centr>5.0){
      fHCorrectMCposChrg =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyChrgPos");
      fHCorrectMCposPion =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyPionPos");
      fHCorrectMCposKaon =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyKaonPos");
      fHCorrectMCposProt =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyProtPos");

      fHCorrectMCnegChrg =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyChrgNeg");
      fHCorrectMCnegPion =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyPionNeg");
      fHCorrectMCnegKaon =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyKaonNeg");
      fHCorrectMCnegProt =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyProtNeg");
    }
    else{
      fHCorrectMCposChrg =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyChrgPosCent0");
      fHCorrectMCposPion =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyPionPosCent0");
      fHCorrectMCposKaon =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyKaonPosCent0");
      fHCorrectMCposProt =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyProtPosCent0");

      fHCorrectMCnegChrg =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyChrgNegCent0");
      fHCorrectMCnegPion =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyPionNegCent0");
      fHCorrectMCnegKaon =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyKaonNegCent0");
      fHCorrectMCnegProt =  (TH1D *) fListTRKCorr->FindObject("trkEfficiencyProtNegCent0");
    }
    */
    
    //for(int i=0;i<10;i++) {
    //fFB_Efficiency_Cent[i] = (TH1D *) fListFBHijing->FindObject(Form("eff_unbiased_%d",i));
    //}
  }
  // else if(!fListTRKCorr){
  //   std::cout<<"\n\n !!!!**** Warning : No FB Efficiency Correction, run = "<<run<<"..!!!**** \n using MC TrkWgt = 1.0 \n"<<std::endl;
  // }
}



















