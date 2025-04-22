//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: PrimaryGeneratorAction.hh,v 1.1.1.1 2011-12-06 17:49:39 rhatcher Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

#include "G4ThreeVector.hh"

#include "TFile.h"
#include "TTree.h"
#include "Framework/Ntuple/NtpMCEventRecord.h"

class G4Event;
class G4VUserDetectorConstruction;
class PrimaryGeneratorMessenger;

class MyEventRecord;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction(MyEventRecord* myevtrec, 
                         G4String infname, G4int verbose=0);    
  virtual ~PrimaryGeneratorAction();

  void      GeneratePrimaries(G4Event*);
  Long64_t  GetEntries() const { return fNEntries; }

  void      SetNextEntry(Long64_t i) { fEntry = i-1; }

private:
  PrimaryGeneratorMessenger* genMessenger;   //messenger of this class

  MyEventRecord*             fMyEventRecord; ///< not owned

  G4String                   fInputFileName;
  TFile*                     fInputFile;
  TTree*                     fInputTree;
  Long64_t                   fNEntries;
  Long64_t                   fEntry;
//  genie::NtpMCEventRecord*   fGenieRecord;
  int                        PDGnu;
  float                      nuvtxx;  //mm
  float                      nuvtxy;  //mm
  float                      nuvtxz;  //mm
  float                      nuvtxt;
  int                        nfsp;    //number of final state particles
  float                      px[200]; //GeV/c
  float                      py[200]; //GeV/c
  float                      pz[200]; //GeV/c
  float                      E[200];  //GeV
  int                        pdg[200];
  double                     fScaleFactor;

  G4int                      fVerbosity;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


