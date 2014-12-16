#include "Analysis.hpp"
#include <vector>
#include <cmath>
#include <memory>
#include <iomanip>
#include "mt2_bisect.h"
#include "TLorentzVector.h"

/* The ATLAS 2 lepton EW analysis (20fb^-1)

   based on: arXiv: 1403.5294

   Code by Martin White

   Known features: Signal leptons in the paper have certain isolation plus ID cuts (these are ignored here by default)

   a) The trigger efficiencies are important! We need to multiply by these to get the correct yields (after which the cutflows agree rather well). 

*/

namespace Gambit {
  namespace ColliderBit {

    using namespace std;

    double Phi_mpi_pi_2lep(double x){
      double pi=3.14159265359;
      double two_pi=2*3.14159265359;
      while (x >= pi) x -= two_pi;
      while (x < -pi) x += two_pi;
    }


    bool sortByPT_2lep(Particle* lep1, Particle* lep2) { return (lep1->pT() > lep2->pT()); }

    class Analysis_ATLAS_2LEPEW_20invfb : public Analysis {
    private:

      // Numbers passing cuts (floats because we will use the trigger eff)
      float _num_MT2_90_SF;
      float _num_MT2_90_DF;
      float _num_MT2_120_SF;
      float _num_MT2_120_DF;
      float _num_MT2_150_SF;
      float _num_MT2_150_DF;
      float _num_WWa_SF;
      float _num_WWa_DF;
      float _num_WWb_SF;
      float _num_WWb_DF;
      float _num_WWc_SF;
      float _num_WWc_DF;
      float _num_Zjets;

      vector<int> cutFlowVector_alt;
      vector<float> cutFlowVector;
      vector<float> cutFlowIncrements;
      vector<string> cutFlowVector_str;
      const static int NCUTS=90;

      // Debug histos

    public:

      Analysis_ATLAS_2LEPEW_20invfb() {
      _num_MT2_90_SF=0;
      _num_MT2_90_DF=0;
      _num_MT2_120_SF=0;
      _num_MT2_120_DF=0;
      _num_MT2_150_SF=0;
      _num_MT2_150_DF=0;
      _num_WWa_SF=0;
      _num_WWa_DF=0;
      _num_WWb_SF=0;
      _num_WWb_DF=0;
      _num_WWc_SF=0;
      _num_WWc_DF=0;
      _num_Zjets=0;
      
      for(int i=0;i<NCUTS;i++){
          cutFlowVector.push_back(0);
          cutFlowVector_str.push_back("");
          cutFlowVector_alt.push_back(0);
	  cutFlowIncrements.push_back(0.);
        }

      }

      void EleEleOverlapRemoval(vector<Particle*> &vec1, vector<Particle*> &vec2, double DeltaRMax) {
        //Routine to do electron-electron overlap check
        //Discard lowest energy electron if two are found overlapping
        vector<Particle*> Survivors;

        for(unsigned int it1 = 0; it1 < vec1.size(); it1++) {
          bool overlap = false;
          P4 lep1mom=vec1.at(it1)->mom();
          for(unsigned int it2 = 0; it2 < vec2.size(); it2++) {
            if(it1==it2)continue;
            P4 lep2mom=vec2.at(it2)->mom();
            float dR;

            dR=lep1mom.deltaR_eta(lep2mom);

            if(fabs(dR) <= DeltaRMax && lep1mom.E()<lep2mom.E()) overlap=true;
          }
          if(overlap) continue;
          Survivors.push_back(vec1.at(it1));
        }
        vec1=Survivors;

        return;
      }

      void LepLepOverlapRemoval(vector<Particle*> &vec1, vector<Particle*> &vec2, double DeltaRMax) {
        //Routine to do lepton-lepton overlap check
        //Discard first lepton if overlap is found
        vector<Particle*> Survivors;

        for(unsigned int it1 = 0; it1 < vec1.size(); it1++) {
          bool overlap = false;
          P4 lep1mom=vec1.at(it1)->mom();
          for(unsigned int it2 = 0; it2 < vec2.size(); it2++) {
            if(it1==it2)continue;
            P4 lep2mom=vec2.at(it2)->mom();
            float dR;

            dR=lep1mom.deltaR_eta(lep2mom);

            if(fabs(dR) <= DeltaRMax)overlap=true;
          }
          if(overlap) continue;
          Survivors.push_back(vec1.at(it1));
        }
        vec1=Survivors;

        return;
      }

      void JetLeptonOverlapRemoval(vector<Jet*> &jetvec, vector<Particle*> &lepvec, double DeltaRMax) {
        //Routine to do jet-lepton check
        //Discards jets if they are within DeltaRMax of a lepton

        vector<Jet*> Survivors;

        for(unsigned int itjet = 0; itjet < jetvec.size(); itjet++) {
          bool overlap = false;
          P4 jetmom=jetvec.at(itjet)->mom();
          for(unsigned int itlep = 0; itlep < lepvec.size(); itlep++) {
            P4 lepmom=lepvec.at(itlep)->mom();
            float dR=jetmom.deltaR_eta(lepmom);

            if(fabs(dR) <= DeltaRMax) overlap=true;
          }
          if(overlap) continue;
          Survivors.push_back(jetvec.at(itjet));
        }
        jetvec=Survivors;

        return;
      }

      void LeptonJetOverlapRemoval(vector<Particle*> &lepvec, vector<Jet*> &jetvec, double DeltaRMax) {
        //Routine to do lepton-jet check
        //Discards leptons if they are within DeltaRMax of a jet

        vector<Particle*> Survivors;

        for(unsigned int itlep = 0; itlep < lepvec.size(); itlep++) {
          bool overlap = false;
          P4 lepmom=lepvec.at(itlep)->mom();
          for(unsigned int itjet= 0; itjet < jetvec.size(); itjet++) {
            P4 jetmom=jetvec.at(itjet)->mom();
            float dR;

            dR=jetmom.deltaR_eta(lepmom);

            if(fabs(dR) <= DeltaRMax) overlap=true;
          }
          if(overlap) continue;
          Survivors.push_back(lepvec.at(itlep));
        }
        lepvec=Survivors;

        return;
      }
      
      void RemoveLeptonsMllLt12(vector<Particle*> &lepvec){
	
	int removeLep1=-1;
	int removeLep2=-1;
	vector<Particle*> Survivors;
	
	//Function removes SF lepton pairs with m_ll < 12 GeV
	for(unsigned int itlep1 = 0; itlep1 < lepvec.size(); itlep1++) {
	  P4 lepmom1=lepvec.at(itlep1)->mom();
          for(unsigned int itlep2= itlep1; itlep2 < lepvec.size(); itlep2++) {
	    if(itlep1!=itlep2){
	      P4 lepmom2=lepvec.at(itlep2)->mom();
	      double mass=(lepmom1+lepmom2).m();
	      if(mass<12.){
		removeLep1=itlep1;
		removeLep2=itlep2;
	      }
	    }
	  }
	}
	for(unsigned int itlep = 0; itlep < lepvec.size(); itlep++) {
	  if(itlep!=removeLep1 && itlep!=removeLep2)Survivors.push_back(lepvec.at(itlep));
	}
	
	lepvec=Survivors;
      }

      void analyze(const Event* event) {

        // Missing energy
        P4 ptot = event->missingmom();
        double met = event->met();

        // Now define vectors of baseline objects
        vector<Particle*> signalElectrons;
        for (Particle* electron : event->electrons()) {
          if (electron->pT() > 10. && fabs(electron->eta()) < 2.47) signalElectrons.push_back(electron);
        }
        vector<Particle*> signalMuons;
        for (Particle* muon : event->muons()) {
          if (muon->pT() > 10. && fabs(muon->eta()) < 2.4) signalMuons.push_back(muon);
        }

        vector<Jet*> signalJets;
        
        for (Jet* jet : event->jets()) {
          if (jet->pT() > 20. && fabs(jet->eta()) < 4.5) signalJets.push_back(jet);

          //if(jet->isBJet() && fabs(jet->eta()) < 2.5 && jet->pT() > 20.) bJets.push_back(jet);
        }
	
	vector<Particle*> signalTaus;
	
	for (Particle* tau : event->taus()) {
	  if (tau->pT() > 20. && fabs(tau->eta()) < 2.5) signalTaus.push_back(tau);
	}

        // Overlap removal

        //Note that ATLAS use |eta|<10 for removing jets close to electrons
        //Then 2.8 is used for the rest of the overlap process
        //Then the signal cut is applied for signal jets

        EleEleOverlapRemoval(signalElectrons,signalElectrons,0.05);
	JetLeptonOverlapRemoval(signalJets,signalElectrons,0.2);
	LepLepOverlapRemoval(signalTaus,signalElectrons,0.2);
	LepLepOverlapRemoval(signalTaus,signalMuons,0.2);
        LeptonJetOverlapRemoval(signalElectrons,signalJets,0.4);
        LeptonJetOverlapRemoval(signalMuons,signalJets,0.4);
	//Note have not bothered with close-by electron and muon pairs (bremsstrahlung probably not significant in signal MC)

	RemoveLeptonsMllLt12(signalElectrons);
	RemoveLeptonsMllLt12(signalMuons);
	JetLeptonOverlapRemoval(signalJets,signalTaus,0.2);

	ApplyTightIDElectronSelection(signalElectrons);

	int numElectrons=signalElectrons.size();
        int numMuons=signalMuons.size();
	int numTaus=signalTaus.size();

        //Search for at least one SFOS pair
        //m_SFOS must be > 12 GeV


	//Classify jets into various categories
	vector<Jet*> centralBJets;
	vector<Jet*> centralNonBJets;
	vector<Jet*> forwardJets;

        const std::vector<float>  a = {0,10.};      
        const std::vector<float>  b = {0,10000.};      
        const std::vector<double> c = {0.8};      
	BinnedFn2D<double> _eff2d(a,b,c);

	for (Jet* jet : signalJets) {
	  bool hasTag=has_tag(_eff2d, jet->eta(), jet->pT());

	  if(fabs(jet->eta()) < 2.4){
	    if(jet->isBJet() && hasTag){
	      centralBJets.push_back(jet);
	    }
	    else {
	      centralNonBJets.push_back(jet);
	    }
	  }
	  if(fabs(jet->eta()) > 2.4 && jet->pT()>30.)forwardJets.push_back(jet);
	}
      
	//Common cuts for all signal regions
	
	bool leptonPTCut=false;
	vector<Particle*> signalLeptons;
	for (Particle* ele : signalElectrons) {
	  signalLeptons.push_back(ele);
	}

	for (Particle* muo : signalMuons) {
	  signalLeptons.push_back(muo);
	}
	
	std::sort(signalLeptons.begin(), signalLeptons.end(), sortByPT_2lep);


	if(signalLeptons.size()==2 && signalLeptons[0]->pT()>35. && signalLeptons[1]->pT()>20.)leptonPTCut=true;
	
	bool mllCut=false;
	if(signalLeptons.size()==2 && (signalLeptons[0]->mom()+signalLeptons[1]->mom()).m() > 20.)mllCut=true;

	bool isOS=false;
	if(signalLeptons.size()==2 && (signalLeptons[0]->pid()*signalLeptons[1]->pid()<0))isOS=true;

	bool passZVeto=true;
	float mLepLep=0.;
	if(signalLeptons.size()==2)mLepLep=(signalLeptons[0]->mom()+signalLeptons[1]->mom()).m();
	if(mLepLep>81.2 && mLepLep<101.2)passZVeto=false;

	bool cut_SRMT290=false;
	bool cut_SRMT2120=false;
	bool cut_SRMT2150=false;

	bool tauVeto=false;
	if(numTaus==0)tauVeto=true;

	int numCentralNonBJets=centralNonBJets.size();
	int numCentralBJets=centralBJets.size();
	int numForwardJets=forwardJets.size();
	
	//Now do the MT2 signal regions

	if(leptonPTCut && mllCut && isOS && tauVeto && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0){
	  
	  //Calculate MT2
	  double pa[3] = { 0, signalLeptons[0]->mom().px(), signalLeptons[0]->mom().py() };
	  double pb[3] = { 0, signalLeptons[1]->mom().px(), signalLeptons[1]->mom().py() };
	  double pmiss[3] = { 0, ptot.px(), ptot.py() };
	  double mn = 0.;
	  
	  mt2_bisect::mt2 mt2_calc;
	  
	  mt2_calc.set_momenta(pa,pb,pmiss);
	  mt2_calc.set_mn(mn);
	  double mt2 = mt2_calc.get_mt2();
	
	  double mll=(signalLeptons[0]->mom() + signalLeptons[1]->mom()).m();


	  if(mt2>90.)cut_SRMT290=true;
	  if(mt2>120.)cut_SRMT2120=true;
	  if(mt2>150.)cut_SRMT2150=true;

	  //Signal region increments use the trigger efficiencies for ee, emu and mumu triggers
	  if(mt2 > 90. && (numElectrons==1 && numMuons==1))_num_MT2_90_DF=_num_MT2_90_DF+0.89;
	  if(passZVeto && mt2 > 90. && (numElectrons==2 && fabs(mll-91.)>10))_num_MT2_90_SF=_num_MT2_90_SF+0.97;
	  if(passZVeto && mt2 > 90. && (numMuons==2 && fabs(mll-91.)>10))_num_MT2_90_SF=_num_MT2_90_SF+0.75;

	  if(mt2 > 120. && (numElectrons==1 && numMuons==1))_num_MT2_120_DF=_num_MT2_120_DF+0.89;
	  if(passZVeto && mt2 > 120. && (numElectrons==2 && fabs(mll-91.)>10))_num_MT2_120_SF=_num_MT2_120_SF+0.97;
	  if(passZVeto && mt2 > 120. &&  (numMuons==2 && fabs(mll-91.)>10))_num_MT2_120_SF=_num_MT2_120_SF+0.75;
	  
	  if(mt2 > 150. && (numElectrons==1 && numMuons==1))_num_MT2_150_DF=_num_MT2_150_DF+0.89;
	  if(passZVeto && mt2 > 150. && (numElectrons==2 && fabs(mll-91.)>10))_num_MT2_150_SF=_num_MT2_150_SF+0.97;
	  if(passZVeto && mt2 > 150. && (numMuons==2 && fabs(mll-91.)>10))_num_MT2_150_SF=_num_MT2_150_SF+0.75;
	  
	}
	
	//Now do the WW channels
	bool passZVeto_WWa=false;
	bool passPTll_WWa=false;
	bool passMetRel_WWa=false;
	bool passMll_WWa=false;

	bool passMT2_WWb=false;
	bool passMT2_WWc=false;
	bool passMll_WWb=false;

	if(leptonPTCut && mllCut && isOS && tauVeto && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0){
	  
	  //Calculate ETmiss_rel
	  float dPhiMin=9999;
	  for(Jet * jet : centralBJets){
	    float dphi=fabs(jet->mom().deltaPhi(ptot));
	    if(fabs(dphi)<dPhiMin)dPhiMin=dphi;
	  }

	  for(Jet * jet : centralNonBJets){
	    float dphi=fabs(jet->mom().deltaPhi(ptot));
	    if(fabs(dphi)<dPhiMin)dPhiMin=dphi;
	  }

	  for(Particle * lep : signalLeptons){
	    float dphi=fabs(lep->mom().deltaPhi(ptot));
	    if(fabs(dphi)<dPhiMin)dPhiMin=dphi;
	  }

	  double ETmiss_rel=0;
	  if(dPhiMin<(3.14/2)){
	    ETmiss_rel=met*sin(dPhiMin);
	  }
	  else {
	    ETmiss_rel=met;
	  }

	  //Calculate MT2
	  double pa[3] = { 0, signalLeptons[0]->mom().px(), signalLeptons[0]->mom().py() };
	  double pb[3] = { 0, signalLeptons[1]->mom().px(), signalLeptons[1]->mom().py() };
	  double pmiss[3] = { 0, ptot.px(), ptot.py() };
	  double mn = 0.;
	  
	  mt2_bisect::mt2 mt2_calc;
	  
	  mt2_calc.set_momenta(pa,pb,pmiss);
	  mt2_calc.set_mn(mn);
	  double mt2 = mt2_calc.get_mt2();

	  double mll=(signalLeptons[0]->mom() + signalLeptons[1]->mom()).m();

	  //Variables for cutflow debugging
	
	  if(fabs(mll-91.)>10.)passZVeto_WWa=true;
	  if((signalLeptons[0]->mom() + signalLeptons[1]->mom()).pT() > 80.)passPTll_WWa=true;
	  if(ETmiss_rel > 80.)passMetRel_WWa=true;
	  if(mll < 120.) passMll_WWa=true;

	  if(mt2 > 90.)passMT2_WWb=true;
	  if(mll < 170.)passMll_WWb=true;

	  if(mt2 > 100.)passMT2_WWc=true;
	  

	  if((signalLeptons[0]->mom() + signalLeptons[1]->mom()).pT() > 80. &&
	     ETmiss_rel > 80. &&
	     mll < 120. &&
	     (numElectrons==1 && numMuons==1))_num_WWa_DF=_num_WWa_DF+0.89;

	  if((signalLeptons[0]->mom() + signalLeptons[1]->mom()).pT() > 80. &&
	     ETmiss_rel > 80. &&
	     mll < 120. &&
	     (numElectrons==2 && fabs(mll-91.)>10.))_num_WWa_SF=_num_WWa_SF+0.97;

	  if((signalLeptons[0]->mom() + signalLeptons[1]->mom()).pT() > 80. &&
	     ETmiss_rel > 80. &&
	     mll < 120. &&
	     (numMuons==2 && fabs(mll-91.)>10.))_num_WWa_SF=_num_WWa_SF+0.75;

	  if(mt2 > 90. &&
	     mll < 170. &&
	     (numElectrons==1 && numMuons==1))_num_WWb_DF=_num_WWb_DF+0.89;

	  if(mt2 > 90. &&
	     mll < 170. &&
	     (numElectrons==2 && fabs(mll-91.)>10.))_num_WWb_SF=_num_WWb_SF+0.97;

	  
	  if(mt2 > 90. &&
	     mll < 170. &&
	     (numMuons==2 && fabs(mll-91.)>10.))_num_WWb_SF=_num_WWb_SF+0.75;

	  if(mt2 > 100. && (numElectrons==1 && numMuons==1))_num_WWc_DF=_num_WWc_DF+0.89;
	  	  
	  if(mt2 > 100. && (numElectrons==2 && fabs(mll-91.)>10.))_num_WWc_SF=_num_WWc_SF+0.97;

	  if(mt2 > 100. && (numMuons==2 && fabs(mll-91.)>10.))_num_WWc_SF=_num_WWc_SF+0.75;
	  
	}
	
	//Finally, do the Z+jets signal region

	bool passZWindow=true;
	bool passPTll=true;
	bool passETmissRel=true;
	bool passdRll=true;
	bool passMjj=true;
	bool passJetPT=true;

	if(leptonPTCut && mllCut && isOS && tauVeto && numCentralNonBJets>=2 && numCentralBJets==0 && numForwardJets==0){
	
	  double mll=(signalLeptons[0]->mom() + signalLeptons[1]->mom()).m();

	  //Calculate ETmiss_rel
	  float dPhiMin=9999;
	  for(Jet * jet : centralBJets){
	    float dphi=jet->mom().deltaPhi(ptot);
	    if(dphi<dPhiMin)dPhiMin=dphi;
	  }

	  for(Jet * jet : centralNonBJets){
	    float dphi=jet->mom().deltaPhi(ptot);
	    if(dphi<dPhiMin)dPhiMin=dphi;
	  }

	  for(Particle * lep : signalLeptons){
	    float dphi=lep->mom().deltaPhi(ptot);
	    if(dphi<dPhiMin)dPhiMin=dphi;
	  }

	  double ETmiss_rel=0;
	  if(dPhiMin<(3.14/2)){
	    ETmiss_rel=met*sin(dPhiMin);
	  }
	  else {
	    ETmiss_rel=met;
	  }
	  
	  
	  float dRll = signalLeptons[0]->mom().deltaR_eta(signalLeptons[1]->mom());

	  float mjj = (centralNonBJets[0]->mom()+centralNonBJets[1]->mom()).m();


	  //Cuts for cutflow debugging
	  if(ETmiss_rel<=80.)passETmissRel=false;
	  if(fabs(mll-91.)>10.)passZWindow=false;
	  if((signalLeptons[0]->mom()+signalLeptons[1]->mom()).pT()<=80.)passPTll=false;
	  if(!(dRll > 0.3 && dRll < 1.5))passdRll=false;
	  if(!(mjj > 50. && mjj<100.))passMjj=false;
	  if(!(centralNonBJets[0]->pT()>45. && centralNonBJets[1]->pT()>45.))passJetPT=false;

	  if(fabs(mll-91.)<10 && ETmiss_rel>80. && (signalLeptons[0]->mom()+signalLeptons[1]->mom()).pT()>80. && dRll > 0.3 && dRll < 1.5 && mjj > 50. && mjj<100. && passJetPT && (numElectrons==2 && numMuons==0))_num_Zjets=_num_Zjets+0.97;
	  
	  if(fabs(mll-91.)<10 && ETmiss_rel>80. && (signalLeptons[0]->mom()+signalLeptons[1]->mom()).pT()>80. && dRll > 0.3 && dRll < 1.5 && mjj > 50. && mjj<100. && passJetPT && (numElectrons==0 && numMuons==2))_num_Zjets=_num_Zjets+0.75;

	}
      
	cutFlowVector_str[0] = "No cuts ";              
	cutFlowVector_str[1] = "2 electrons ";
	cutFlowVector_str[2] = "Lepton pT cuts (trigger) ";  
	cutFlowVector_str[3] = "mll cuts ";  
	cutFlowVector_str[4] = "OS leptons ";  
	cutFlowVector_str[5] = "tau veto ";  
        cutFlowVector_str[6] = "e+e-: Jet veto ";      
        cutFlowVector_str[7] = "e+e-: Z veto ";
        cutFlowVector_str[8] = "e+e-: SR-MT290 ";
        cutFlowVector_str[9] = "e+e-: SR-MT2120 ";
        cutFlowVector_str[10] = "e+e-: SR-MT2150 ";
	cutFlowVector_str[11] = "mu+mu-: 2 signal leptons ";
        cutFlowVector_str[12] = "mu+mu-: Jet veto ";      
        cutFlowVector_str[13] = "mu+mu-: Z veto ";
        cutFlowVector_str[14] = "mu+mu-: SR-MT290 ";
        cutFlowVector_str[15] = "mu+mu-: SR-MT2120 ";
        cutFlowVector_str[16] = "mu+mu-: SR-MT2150 ";
	cutFlowVector_str[17] = "e+-mu-+: 2 signal leptons ";
        cutFlowVector_str[18] = "e+-mu-+: Jet veto ";      
        cutFlowVector_str[19] = "e+-mu-+: Z veto ";
        cutFlowVector_str[20] = "e+-mu-+: SR-MT290 ";
        cutFlowVector_str[21] = "e+-mu-+: SR-MT2120 ";
        cutFlowVector_str[22] = "e+-mu-+: SR-MT2150 ";
	cutFlowVector_str[23] = "SRZjets e+e-: 2 signal leptons ";
	cutFlowVector_str[24] = "SRZjets e+e-: >=2 light jets ";
	cutFlowVector_str[25] = "SRZjets e+e-: No b and forward jets ";
	cutFlowVector_str[26] = "SRZjets e+e-: Z window ";
	cutFlowVector_str[27] = "SRZjets e+e-: pTll > 80 ";
	cutFlowVector_str[28] = "SRZjets e+e-: ETmissrel ";
	cutFlowVector_str[29] = "SRZjets e+e-: dRll ";
	cutFlowVector_str[30] = "SRZjets e+e-: mjj ";
	cutFlowVector_str[31] = "SRZjets e+e-: jet pT ";
	cutFlowVector_str[32] = "SRZjets mu+mu-: 2 signal leptons ";
	cutFlowVector_str[33] = "SRZjets mu+mu-: >=2 light jets ";
	cutFlowVector_str[34] = "SRZjets mu+mu-: No b and forward jets ";
	cutFlowVector_str[35] = "SRZjets mu+mu-: Z window ";
	cutFlowVector_str[36] = "SRZjets mu+mu-: pTll > 80 ";
	cutFlowVector_str[37] = "SRZjets mu+mu-: ETmissrel ";
	cutFlowVector_str[38] = "SRZjets mu+mu-: dRll ";
	cutFlowVector_str[39] = "SRZjets mu+mu-: mjj ";
	cutFlowVector_str[40] = "SRZjets mu+mu-: jet pT ";
	cutFlowVector_str[41] = "SRWWa e+e-: 2 leptons ";
	cutFlowVector_str[42] = "SRWWa e+e-: Jet veto ";
	cutFlowVector_str[43] = "SRWWa e+e-: Z veto ";
	cutFlowVector_str[44] = "SRWWa e+e-: pTll ";
	cutFlowVector_str[45] = "SRWWa e+e-: ETmissrel ";
	cutFlowVector_str[46] = "SRWWa e+e-: mll ";
	cutFlowVector_str[47] = "SRWWa mu+mu-: 2 leptons ";
	cutFlowVector_str[48] = "SRWWa mu+mu-: Jet veto ";
	cutFlowVector_str[49] = "SRWWa mu+mu-: Z veto ";
	cutFlowVector_str[50] = "SRWWa mu+mu-: pTll ";
	cutFlowVector_str[51] = "SRWWa mu+mu-: ETmissrel ";
	cutFlowVector_str[52] = "SRWWa mu+mu-: mll ";
	cutFlowVector_str[53] = "SRWWa e+mu-: 2 leptons ";
	cutFlowVector_str[54] = "SRWWa e+mu-: Jet veto ";
	cutFlowVector_str[55] = "SRWWa e+mu-: pTll ";
	cutFlowVector_str[56] = "SRWWa e+mu-: ETmissrel ";
	cutFlowVector_str[57] = "SRWWa e+mu-: mll ";
	cutFlowVector_str[58] = "SRWWb e+e-: 2 leptons ";
	cutFlowVector_str[59] = "SRWWb e+e-: Jet veto ";
	cutFlowVector_str[60] = "SRWWb e+e-: Z veto ";
	cutFlowVector_str[61] = "SRWWb e+e-: mT2 > 90 ";
	cutFlowVector_str[62] = "SRWWb e+e-: mll < 170 ";
	cutFlowVector_str[63] = "SRWWb mu+mu-: 2 leptons ";
	cutFlowVector_str[64] = "SRWWb mu+mu-: Jet veto ";
	cutFlowVector_str[65] = "SRWWb mu+mu-: Z veto ";
	cutFlowVector_str[66] = "SRWWb mu+mu-: mT2 > 90 ";
	cutFlowVector_str[67] = "SRWWb mu+mu-: mll < 170 ";
	cutFlowVector_str[68] = "SRWWb e+mu-: 2 leptons ";
	cutFlowVector_str[69] = "SRWWb e+mu-: Jet veto ";
	cutFlowVector_str[70] = "SRWWb e+mu-: mT2 > 90 ";
	cutFlowVector_str[71] = "SRWWb e+mu-: mll < 170 ";
	cutFlowVector_str[72] = "SRWWc e+e-: 2 leptons ";
	cutFlowVector_str[73] = "SRWWc e+e-: Jet veto ";
	cutFlowVector_str[74] = "SRWWc e+e-: Z veto ";
	cutFlowVector_str[75] = "SRWWc e+e-: mT2 > 100 ";
	cutFlowVector_str[76] = "SRWWc mu+mu-: 2 leptons ";
	cutFlowVector_str[77] = "SRWWc mu+mu-: Jet veto ";
	cutFlowVector_str[78] = "SRWWc mu+mu-: Z veto ";
	cutFlowVector_str[79] = "SRWWc mu+mu-: mT2 > 100 ";
	cutFlowVector_str[80] = "SRWWc e+mu-: 2 leptons ";
	cutFlowVector_str[81] = "SRWWc e+mu-: Jet veto ";
	cutFlowVector_str[82] = "SRWWc e+mu-: mT2 > 100 ";

	for(int j=0;j<NCUTS;j++){
	  if(j>=0 && j<=10)cutFlowIncrements[j]=0.97;
	  if(j>=11 && j<=16)cutFlowIncrements[j]=0.75;
	  if(j>=17 && j<=22)cutFlowIncrements[j]=0.89;

	  if(j>=23 && j<=31)cutFlowIncrements[j]=0.97;
	  if(j>=32 && j<=40)cutFlowIncrements[j]=0.75;

	  if(j>=41 && j<=46)cutFlowIncrements[j]=0.97;
	  if(j>=47 && j<=52)cutFlowIncrements[j]=0.75;
	  if(j>=53 && j<=57)cutFlowIncrements[j]=0.89;

	  if(j>=58 && j<=62)cutFlowIncrements[j]=0.97;
	  if(j>=63 && j<=67)cutFlowIncrements[j]=0.75;
	  if(j>=68 && j<=71)cutFlowIncrements[j]=0.89;

	  if(j>=72 && j<=75)cutFlowIncrements[j]=0.97;
	  if(j>=76 && j<=79)cutFlowIncrements[j]=0.75;
	  if(j>=80 && j<=82)cutFlowIncrements[j]=0.89;
	}


       	for(int j=0;j<NCUTS;j++){
          if( (j==0) ||

	      (j==1 && signalElectrons.size()==2) ||

	      (j==2 && signalElectrons.size()==2 && leptonPTCut) ||

	      (j==3 && signalElectrons.size()==2 && leptonPTCut && mllCut) ||
	      
	      (j==4 && signalElectrons.size()==2 && leptonPTCut && mllCut && isOS) ||

	      (j==5 && leptonPTCut && mllCut && isOS && signalElectrons.size()==2 && tauVeto) ||
	      
	      (j==6 && leptonPTCut && mllCut && isOS && signalElectrons.size()==2 && tauVeto && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0) ||
	      
	      (j==7 && leptonPTCut && mllCut && isOS && signalElectrons.size()==2 && tauVeto && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto) ||
	      
	      (j==8 && leptonPTCut && mllCut && isOS && signalElectrons.size()==2 && tauVeto && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto && cut_SRMT290) ||

	      (j==9 && leptonPTCut && mllCut && isOS && signalElectrons.size()==2 && tauVeto && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto && cut_SRMT2120) ||
	      
	      (j==10 && leptonPTCut && mllCut && isOS && signalElectrons.size()==2 && tauVeto && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto && cut_SRMT2150) ||

	      //mumu MT2 regions

	      (j==11 && leptonPTCut && mllCut && isOS && signalMuons.size()==2 && tauVeto) ||
	      
	      (j==12 && leptonPTCut && mllCut && isOS && tauVeto && signalMuons.size()==2 && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0) ||

	      (j==13 && leptonPTCut && mllCut && isOS && tauVeto && signalMuons.size()==2 && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto) ||
	      
	      (j==14 && leptonPTCut && mllCut && isOS && tauVeto && signalMuons.size()==2 && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto && cut_SRMT290) ||

	      (j==15 && leptonPTCut && mllCut && isOS && tauVeto && signalMuons.size()==2 && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto && cut_SRMT2120) ||
	      
	      (j==16 && leptonPTCut && mllCut && isOS && tauVeto && signalMuons.size()==2 && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto && cut_SRMT2150) ||
	      
	      //emu MT2 regions

	      (j==17 && leptonPTCut && mllCut && isOS && tauVeto && signalElectrons.size()==1 && signalMuons.size()==1 && (signalElectrons[0]->pid()*signalMuons[0]->pid())<0 && tauVeto) ||
	      
	      (j==18 && leptonPTCut && mllCut && isOS && tauVeto && signalElectrons.size()==1 && signalMuons.size()==1 && (signalElectrons[0]->pid()*signalMuons[0]->pid())<0 && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0) ||

	      (j==19 && leptonPTCut && mllCut && isOS && tauVeto && signalElectrons.size()==1 && signalMuons.size()==1 && (signalElectrons[0]->pid()*signalMuons[0]->pid())<0 && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto) ||
	      
	      (j==20 && leptonPTCut && mllCut && isOS && tauVeto && signalElectrons.size()==1 && signalMuons.size()==1 && (signalElectrons[0]->pid()*signalMuons[0]->pid())<0 && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto && cut_SRMT290) ||

	      (j==21 && leptonPTCut && mllCut && isOS && tauVeto && signalElectrons.size()==1 && signalMuons.size()==1 && (signalElectrons[0]->pid()*signalMuons[0]->pid())<0 && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto && cut_SRMT2120) ||
	      
	      (j==22 && leptonPTCut && mllCut && isOS && tauVeto && signalElectrons.size()==1 && signalMuons.size()==1 && (signalElectrons[0]->pid()*signalMuons[0]->pid())<0 && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto && cut_SRMT2150) ||

	      //Start SR Z jets e+e-
	      (j==23 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==2 && numMuons==0)) ||

	      (j==24 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==2 && numMuons==0) && numCentralNonBJets>=2) ||

	      (j==25 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==2 && numMuons==0) && numCentralNonBJets>=2 && numCentralBJets==0 && numForwardJets==0) ||

	      (j==26 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==2 && numMuons==0) && numCentralNonBJets>=2 && numCentralBJets==0 && numForwardJets==0 && passZWindow) ||

	      (j==27 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==2 && numMuons==0) && numCentralNonBJets>=2 && numCentralBJets==0 && numForwardJets==0 && passZWindow && passPTll) ||

	      (j==28 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==2 && numMuons==0) && numCentralNonBJets>=2 && numCentralBJets==0 && numForwardJets==0 && passZWindow && passPTll && passETmissRel) ||

	      (j==29 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==2 && numMuons==0) && numCentralNonBJets>=2 && numCentralBJets==0 && numForwardJets==0 && passZWindow && passPTll && passETmissRel && passdRll) ||

	      (j==30 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==2 && numMuons==0) && numCentralNonBJets>=2 && numCentralBJets==0 && numForwardJets==0 && passZWindow && passPTll && passETmissRel && passdRll && passMjj) ||

	      (j==31 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==2 && numMuons==0) && numCentralNonBJets>=2 && numCentralBJets==0 && numForwardJets==0 && passZWindow && passPTll && passETmissRel && passdRll && passMjj && passJetPT) ||

	      //Start SR Z jets mu+mu-
	      (j==32 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==0 && numMuons==2)) ||

	      (j==33 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==0 && numMuons==2) && numCentralNonBJets>=2) ||

	      (j==34 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==0 && numMuons==2) && numCentralNonBJets>=2 && numCentralBJets==0 && numForwardJets==0) ||

	      (j==35 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==0 && numMuons==2) && numCentralNonBJets>=2 && numCentralBJets==0 && numForwardJets==0 && passZWindow) ||

	      (j==36 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==0 && numMuons==2) && numCentralNonBJets>=2 && numCentralBJets==0 && numForwardJets==0 && passZWindow && passPTll) ||

	      (j==37 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==0 && numMuons==2) && numCentralNonBJets>=2 && numCentralBJets==0 && numForwardJets==0 && passZWindow && passPTll && passETmissRel) ||

	      (j==38 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==0 && numMuons==2) && numCentralNonBJets>=2 && numCentralBJets==0 && numForwardJets==0 && passZWindow && passPTll && passETmissRel && passdRll) ||

	      (j==39 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==0 && numMuons==2) && numCentralNonBJets>=2 && numCentralBJets==0 && numForwardJets==0 && passZWindow && passPTll && passETmissRel && passdRll && passMjj) ||

	      (j==40 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==0 && numMuons==2) && numCentralNonBJets>=2 && numCentralBJets==0 && numForwardJets==0 && passZWindow && passPTll && passETmissRel && passdRll && passMjj && passJetPT) ||
	      
	      //Now start WWa e+e-
	      (j==41 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==2 && numMuons==0)) || 

	      (j==42 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==2 && numMuons==0) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0) ||
	      
	      (j==43 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==2 && numMuons==0) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto_WWa) ||
	      
	      (j==44 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==2 && numMuons==0) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto_WWa && passPTll_WWa) ||

	      (j==45 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==2 && numMuons==0) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto_WWa && passPTll_WWa && passMetRel_WWa) ||

	      (j==46 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==2 && numMuons==0) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto_WWa && passPTll_WWa && passMetRel_WWa && passMll_WWa) ||

	      //Now start WWa mu+mu-
	      (j==47 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==0 && numMuons==2)) || 

	      (j==48 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==0 && numMuons==2) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0) ||
	      
	      (j==49 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==0 && numMuons==2) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto_WWa) ||

	      (j==50 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==0 && numMuons==2) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto_WWa && passPTll_WWa) ||

	      (j==51 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==0 && numMuons==2) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto_WWa && passPTll_WWa && passMetRel_WWa) ||

	      (j==52 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==0 && numMuons==2) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto_WWa && passPTll_WWa && passMetRel_WWa && passMll_WWa) ||

	      //Now start WWa e+mu-
	      (j==53 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==1 && numMuons==1)) || 

	      (j==54 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==1 && numMuons==1) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0) ||
	      
	      (j==55 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==1 && numMuons==1) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto_WWa && passPTll_WWa) ||

	      (j==56 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==1 && numMuons==1) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto_WWa && passPTll_WWa && passMetRel_WWa) ||

	      (j==57 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==1 && numMuons==1) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto_WWa && passPTll_WWa && passMetRel_WWa && passMll_WWa) ||

	      //WWb e+ e-
	      (j==58 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==2 && numMuons==0)) || 

	      (j==59 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==2 && numMuons==0) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0) ||
	      
	      (j==60 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==2 && numMuons==0) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto_WWa) ||
	      
	      (j==61 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==2 && numMuons==0) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto_WWa && passMT2_WWb) ||

	      (j==62 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==2 && numMuons==0) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto_WWa && passMT2_WWb && passMll_WWb) ||

	      //WWb mu+ mu-
	      (j==63 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==0 && numMuons==2)) || 

	      (j==64 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==0 && numMuons==2) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0) ||
	      
	      (j==65 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==0 && numMuons==2) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto_WWa) ||
	      
	      (j==66 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==0 && numMuons==2) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto_WWa && passMT2_WWb) ||

	      (j==67 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==0 && numMuons==2) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto_WWa && passMT2_WWb && passMll_WWb) ||

	      //WWb e+mu-
	  
	      (j==68 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==1 && numMuons==1)) || 

	      (j==69 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==1 && numMuons==1) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0) ||
	      
	      (j==70 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==1 && numMuons==1) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto_WWa && passMT2_WWb) ||

	      (j==71 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==1 && numMuons==1) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto_WWa && passMT2_WWb && passMll_WWb) ||

	      //WWc e+ e-
	      (j==72 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==2 && numMuons==0)) || 

	      (j==73 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==2 && numMuons==0) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0) ||
	      
	      (j==74 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==2 && numMuons==0) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto_WWa) ||
	      
	      (j==75 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==2 && numMuons==0) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto_WWa && passMT2_WWc) ||

	      //WWc mu+ mu-
	      (j==76 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==0 && numMuons==2)) || 

	      (j==77 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==0 && numMuons==2) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0) ||
	      
	      (j==78 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==0 && numMuons==2) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto_WWa) ||
	      
	      (j==79 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==0 && numMuons==2) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto_WWa && passMT2_WWc) ||

	      //WWc e+mu-
	  
	      (j==80 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==1 && numMuons==1)) || 

	      (j==81 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==1 && numMuons==1) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0) ||
	      
	      (j==82 && tauVeto && leptonPTCut && mllCut && isOS && (numElectrons==1 && numMuons==1) && numCentralNonBJets==0 && numCentralBJets==0 && numForwardJets==0 && passZVeto_WWa && passMT2_WWc) 

	      )cutFlowVector[j]=cutFlowVector[j]+cutFlowIncrements[j];
	      
	}
	
	return;
	
      }
      
      void finalize() {

        using namespace std;

        double scale_to = 20.7*5800./cutFlowVector[0];
        double trigger_cleaning_eff = 1.0;

        cout << "------------------------------------------------------------------------------------------------------------------------------ "<<std::endl;
        cout << "CUT FLOW: ATLAS-CONF-2013-035 - Appendix, Table A "<<std::endl;
        cout << "------------------------------------------------------------------------------------------------------------------------------"<<std::endl;
        cout << "------------------------------------------------------------------------------------------------------------------------------"<<std::endl;


        std::cout<< right << setw(40) << "CUT" << setw(20) << "RAW" << setw(20) << "SCALED" << setw(20) << "%" << setw(20) << "clean adj RAW"<< setw(20) << "clean adj %" << endl;
        for(int j=0; j<NCUTS; j++) {
          std::cout << right << setw(40) << cutFlowVector_str[j].c_str() << setw(20) << cutFlowVector[j] << setw(20) << cutFlowVector[j]*scale_to*trigger_cleaning_eff << setw(20) << 100.*cutFlowVector[j] << "%" << setw(20) << trigger_cleaning_eff*cutFlowVector[j]*scale_to << setw(20) << trigger_cleaning_eff*100.*cutFlowVector[j]<< "%" << endl;
        }
        cout << "------------------------------------------------------------------------------------------------------------------------------ "<<std::endl;

        //cout << "RESULTS 2LEP " << _numSRM90SF << " " << _numSRM100SF << " " << _numSRM110SF << " " << _numSRM120SF << " " << _numSRM90DF << " " << _numSRM100DF << " " << _numSRM110DF << " " << _numSRM120DF << endl;
    
      }
    

      double loglikelihood() {
        /// @todo Implement!
        return 1.0;
      }
      
      
      void collect_results() {

	finalize();

	SignalRegionData results_MT2_90_SF;
	results_MT2_90_SF.set_observation(33.);
	results_MT2_90_SF.set_background(38.2);
	results_MT2_90_SF.set_backgroundsys(5.1);
	results_MT2_90_SF.set_signalsys(0.);
	results_MT2_90_SF.set_signal(_num_MT2_90_SF);
	
	SignalRegionData results_MT2_90_DF;
	results_MT2_90_DF.set_observation(21.);
	results_MT2_90_DF.set_background(23.3);
	results_MT2_90_DF.set_backgroundsys(3.7);
	results_MT2_90_DF.set_signalsys(0.);
	results_MT2_90_DF.set_signal(_num_MT2_90_DF);
	
	SignalRegionData results_MT2_120_SF;
	results_MT2_120_SF.set_observation(5.);
	results_MT2_120_SF.set_background(8.9);
	results_MT2_120_SF.set_backgroundsys(2.1);
	results_MT2_120_SF.set_signalsys(0.);
	results_MT2_120_SF.set_signal(_num_MT2_120_SF);
	
	SignalRegionData results_MT2_120_DF;
	results_MT2_120_DF.set_observation(5.);
	results_MT2_120_DF.set_background(3.6);
	results_MT2_120_DF.set_backgroundsys(1.2);
	results_MT2_120_DF.set_signalsys(0.);
	results_MT2_120_DF.set_signal(_num_MT2_120_DF);
	
	SignalRegionData results_MT2_150_SF;
	results_MT2_150_SF.set_observation(3.);
	results_MT2_150_SF.set_background(3.2);
	results_MT2_150_SF.set_backgroundsys(0.7);
	results_MT2_150_SF.set_signalsys(0.);
	results_MT2_150_SF.set_signal(_num_MT2_150_SF);
	
	SignalRegionData results_MT2_150_DF;
	results_MT2_150_DF.set_observation(2.);
	results_MT2_150_DF.set_background(1.0);
	results_MT2_150_DF.set_backgroundsys(0.5);
	results_MT2_150_DF.set_signalsys(0.);
	results_MT2_150_DF.set_signal(_num_MT2_150_DF);
	
	SignalRegionData results_WWa_SF;
	results_WWa_SF.set_observation(73.);
	results_WWa_SF.set_background(86.5);
	results_WWa_SF.set_backgroundsys(7.4);
	results_WWa_SF.set_signalsys(0.);
	results_WWa_SF.set_signal(_num_WWa_SF);
	
	SignalRegionData results_WWa_DF;
	results_WWa_DF.set_observation(70.);
	results_WWa_DF.set_background(73.6);
	results_WWa_DF.set_backgroundsys(7.9);
	results_WWa_DF.set_signalsys(0.);
	results_WWa_DF.set_signal(_num_WWa_DF);
	
	SignalRegionData results_WWb_SF;
	results_WWb_SF.set_observation(26.);
	results_WWb_SF.set_background(30.2);
	results_WWb_SF.set_backgroundsys(3.5);
	results_WWb_SF.set_signalsys(0.);
	results_WWb_SF.set_signal(_num_WWb_SF);
	
	SignalRegionData results_WWb_DF;
	results_WWb_SF.set_observation(17.);
	results_WWb_SF.set_background(18.1);
	results_WWb_SF.set_backgroundsys(2.6);
	results_WWb_SF.set_signalsys(0.);
	results_WWb_SF.set_signal(_num_WWb_DF);
	
	SignalRegionData results_WWc_SF;
	results_WWc_SF.set_observation(10.);
	results_WWc_SF.set_background(20.3);
	results_WWc_SF.set_backgroundsys(3.5);
	results_WWc_SF.set_signalsys(0.);
	results_WWc_SF.set_signal(_num_WWc_SF);
	
	SignalRegionData results_WWc_DF;
	results_WWc_SF.set_observation(11.);
	results_WWc_SF.set_background(9.0);
	results_WWc_SF.set_backgroundsys(2.2);
	results_WWc_SF.set_signalsys(0.);
	results_WWc_SF.set_signal(_num_WWc_DF);
	
	SignalRegionData results_Zjets;
	results_Zjets.set_observation(1.);
	results_Zjets.set_background(1.4);
	results_Zjets.set_backgroundsys(0.6);
	results_Zjets.set_signalsys(0.);
	results_Zjets.set_signal(_num_Zjets);
	
	add_result(results_MT2_90_SF);
	add_result(results_MT2_90_DF);
	add_result(results_MT2_120_SF);
	add_result(results_MT2_120_DF);
	add_result(results_MT2_150_SF);
	add_result(results_MT2_150_DF);
	add_result(results_WWa_SF);
	add_result(results_WWa_DF);
	add_result(results_WWb_SF);
	add_result(results_WWb_DF);
	add_result(results_WWc_SF);
	add_result(results_WWc_DF);
	add_result(results_Zjets);
	
	return;
      }
      
    };

    DEFINE_ANALYSIS_FACTORY(ATLAS_2LEPEW_20invfb)
  }
}