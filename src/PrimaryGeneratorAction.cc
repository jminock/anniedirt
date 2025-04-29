
#include "PrimaryGeneratorAction.hh"

#include "G4VUserDetectorConstruction.hh"
#include "PrimaryGeneratorMessenger.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4TransportationManager.hh"
#include "G4Navigator.hh"
#include "G4TouchableHistory.hh"

#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"

// GENIE headers
#include "Framework/GHEP/GHepParticle.h"
#include "Framework/Ntuple/NtpMCTreeHeader.h"

// ROOT headers


// this program's headers
#include "MyEventRecord.hh"

#include <cstring>

using namespace CLHEP;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction(MyEventRecord* myevtrec,
                                               G4String infname, G4int verbose)
  : fMyEventRecord(myevtrec)
  , fInputFileName(infname)
  , fInputFile(0)
  , fInputTree(0)
  , fEntry(-1)
  , fVerbosity(verbose)
{
  
  //create a messenger for this class
  genMessenger = new PrimaryGeneratorMessenger(this);

  fInputFile = TFile::Open(fInputFileName.c_str(),"READONLY");
  if ( ! fInputFile ) {
    G4cerr << "%%% could not find input file \"" << fInputFileName << "\"" 
           << G4endl;
    exit(127);
  }

/*  genie::NtpMCTreeHeader* header = 0;
  fInputFile->GetObject("header",header);
  if ( ! header ) {
    G4cerr << "%%% could not find NtpMCTreeHeader in \"" << fInputFileName << "\"" 
           << G4endl;
    exit(127);
  }
  G4cout << "from GENIE header: " << G4endl
         << "  runnu " << header->runnu << G4endl
         << "  cvstag " << header->cvstag.GetName() << G4endl
         << "  format " << genie::NtpMCFormat::AsString(header->format) << G4endl
         << "  datime " << header->datime << G4endl
         << G4endl;
*/
//  NtpMCFormat_t format;  ///< Event Record format (GENIE support multiple formats)
//  TObjString    cvstag;  ///< GENIE CVS Tag (to keep track of GENIE's version)
//  NtpMCDTime    datime;  ///< Date and Time that the event ntuple was generated
//  Long_t        runnu;   ///< MC Job run number

 fInputFile->GetObject("FlatTree_VARS",fInputTree);
  if ( ! fInputTree ) {
    G4cerr << "%%% could not find input tree \"FlatTree_VARS\" in \"" 
           << fInputFileName << "\"" << G4endl;
    exit(127);
  }
  fNEntries = fInputTree->GetEntries();

  fMyEventRecord->run            = 0; //FIX THIS: hard-coded
  fMyEventRecord->inputFileName  = fInputFileName;
  fMyEventRecord->inputNEntries  = fNEntries;
//  fMyEventRecord->inputTotalPOTs = fInputTree->GetWeight();

  G4cout << "%%% input file \"" << fInputFileName << "\" has " 
         << fNEntries << " entries " << G4endl;

//  fGenieRecord = new genie::NtpMCEventRecord;
  fInputTree->SetBranchAddress("PDGnu",&PDGnu);
  fInputTree->SetBranchAddress("vtxx",&nuvtxx);
  fInputTree->SetBranchAddress("vtxy",&nuvtxy);
  fInputTree->SetBranchAddress("vtxz",&nuvtxz);
  fInputTree->SetBranchAddress("vtxt",&nuvtxt);
  fInputTree->SetBranchAddress("nfsp",&nfsp);
  fInputTree->SetBranchAddress("px",&px);
  fInputTree->SetBranchAddress("py",&py);
  fInputTree->SetBranchAddress("pz",&pz);
  fInputTree->SetBranchAddress("E",&E);
  fInputTree->SetBranchAddress("pdg",&pdg);
  fInputTree->SetBranchAddress("fScaleFactor",&fScaleFactor);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  if ( fInputFile ) fInputFile->Close();
  delete genMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  //this function is called at the begining of event
  // 
  ++fEntry;
  if ( fVerbosity > 0 ) {
    G4cout << G4endl << "---> GeneratePrimaries from GENIE file entry "
           << fEntry << G4endl;
  }

  if ( fEntry >= fNEntries ) {
    // no more input
    return;
  }
  fInputTree->GetEntry(fEntry);

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

  G4double x = nuvtxx * mm;  // NuWro uses millimeters
  G4double y = nuvtxy * mm;  // NuWro uses millimeters
  G4double z = nuvtxz * mm;  // NuWro uses millimeters
  G4double t = 0.0; //nuvtxt; NuWro does not have time variable, assume time at 0 seconds
  G4ThreeVector g4pos(x,y,z);
  G4PrimaryVertex* evtVertex = new G4PrimaryVertex(g4pos,t);

  int intank = 0;
  int inhall = -1;  // not determining this right yet

  G4Navigator* navigator = 
    G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();

  G4VPhysicalVolume* pvol = 0;

  pvol = navigator->LocateGlobalPointAndSetup(g4pos,0,false,true);

  std::string vtxvol = ( ( pvol ) ? pvol->GetName() : "<<no-pvol>>" );
  // really should walk up ... for hierarchy recording copy #

  std::string vtxmat = 
    ( ( pvol ) ? pvol->GetLogicalVolume()->GetMaterial()->GetName() : "<<unknown>>" );

  /*
  G4TouchableHistory* touchable = navigator->CreateTouchableHistory();
  //navigator->LocateGlobalPointAndUpdateTouchable(g4pos,touchable,false);
  // touchable not owned by navigator ... need to delete

  pvol = navigator->ResetHierarchyAndLocate(g4pos,G4ThreeVector(),*touchable);
  
  //touchable->GetVolume();  

  G4int ndepth = touchable->GetHistoryDepth();
  G4cout << "ndepth " << ndepth << G4endl;

  for (G4int idepth=0; idepth<ndepth; ++idepth) {
    pvol = touchable->GetVolume(idepth);
    if ( pvol ) {
      G4cout << "[" << idepth << "] " << pvol->GetName() << G4endl;
    } else {
      G4cout <<" [" << idepth << "]  no pvol " << G4endl;
    }
  }

  delete touchable;

  while ( pvol ) {
    G4String pvname = pvol->GetName();
    if ( lvname.find("TWATER")   == 0 ) intank = 1;
    if ( lvname.find("EXP_HALL") == 0 ) inhall = 1;
    //lv = lv->
  }
  */

  G4bool insomethinginteresting = ((vtxvol.find("0x"))!=std::string::npos);
  if ( vtxvol == "TWATER_PV" || insomethinginteresting ) {
    intank = 1;
    G4cout << "=========> particles start in " << vtxvol << " entry " 
           << fEntry << G4endl;
  }
  // other posssibilities ? ... tank wall, base, cone, airspace?


  fMyEventRecord->clear();

  fMyEventRecord->entry  = fEntry;
  fMyEventRecord->fScaleFactor = fScaleFactor;
  fMyEventRecord->nupdg  = PDGnu;
  fMyEventRecord->nuvtxx = nuvtxx / 1000.; //convert to meters
  fMyEventRecord->nuvtxy = nuvtxy / 1000.; //convert to meters
  fMyEventRecord->nuvtxz = nuvtxz / 1000.; //convert to meters
  fMyEventRecord->nuvtxt = 0.0;//nuvtxt;

  fMyEventRecord->intank = intank;
  fMyEventRecord->inhall = inhall;
  //fMyEventRecord->vtxvol = vtxvol;
  //fMyEventRecord->vtxmat = vtxmat;
  //char * strncpy ( char * destination, const char * source, size_t num );
  fMyEventRecord->vtxvol[4095] = '\0';
  fMyEventRecord->vtxmat[4095] = '\0';
  strncpy(fMyEventRecord->vtxvol,vtxvol.c_str(),4094);
  strncpy(fMyEventRecord->vtxmat,vtxmat.c_str(),4094);

  fMyEventRecord->ntank  = 0;

  // in principle particles leaving the nucleus are offset from the
  // actual vertex and should start from
  //    evtVertex (meters) + part->V()*1.0e-15 (meters)
  // for now ... just originate everything at center...
  // and don't worry about femto-meter scale offsets
  // (or slightly bigger for charm / tau decays)

  if ( fVerbosity > 1 ) {
    G4cout << " within " <<  vtxvol << G4endl;
  }

  // loop over NUISANCE particles
  int nparticles = nfsp;
  for (int ipart=0; ipart<nparticles; ++ipart ) {

    G4ParticleDefinition* partDef = particleTable->FindParticle(pdg[ipart]);
    if ( ! partDef ) {
      if ( fVerbosity > 0 ) {
        G4cout << "skipping PDG " << pdg[ipart] << G4endl;
      }
      continue;
    }
//    G4ThreeVector voffset(part->Vx(),part->Vy(),part->Vz());
//    G4double toffset = part->Vt();

    G4PrimaryParticle* g4part = 
      new G4PrimaryParticle( partDef, 
                             px[ipart]*GeV, py[ipart]*GeV, pz[ipart]*GeV,
                             E[ipart]*GeV);

    if ( intank ) {
      fMyEventRecord->AppendG4PrimaryParticle(evtVertex,g4part);
      delete g4part;
    } else {
      evtVertex->SetPrimary( g4part ); // really add to G4 to process
    }

    if ( fVerbosity > 1 ) {
      G4cout 
        << "pdg " << pdg[ipart]
        << " E " << E[ipart]
        << " Pz() " << pz[ipart]
        << " (" << pz[ipart]*GeV << ") " << G4endl;
//        << "voffset " << voffset << " fm, t " << toffset << G4endl;
    }
  }
  
  anEvent->AddPrimaryVertex(evtVertex);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

