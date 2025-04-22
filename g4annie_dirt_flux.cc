// g4annie_dirt_flux
// code optimized for tracking particles to the surface of the tank water

#include <cassert>

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4GDMLParser.hh"
#include "G4VPhysicalVolume.hh"

#include "Randomize.hh"

#include "DetectorConstruction.hh"
#include "G4PhysListFactoryAlt.hh"   // alternative version in "alt" namespace

#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "SteppingVerbose.hh"

#include "MyEventRecord.hh"

// RWH  don't try visualization yet
#undef G4VIS_USE

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include <cstdlib>  // abort()

#include "TString.h"

extern "C" {
#include <getopt.h>
}
#ifndef GETOPTDONE // Some getopt.h's have this, some don't...
#define GETOPTDONE (-1)
#endif

// Globals so that PrintUsage can see them
static int verbosity  = 0;
static int batch_mode = 0;
Long64_t nevents      = -1;
Long64_t nrepeat      = 1;
G4String gdmlname     = "../../annie.gdml";
G4String plname       = "FTFP_BERT_HP"; // want those neutrons!!!
G4String infname      = "nuisance.nuwro.test.flat.root";
G4String outfname     = "annie_flux.root";


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void PrintUsage() {
  G4cout 
    << "g4annie_dirt_flux:  a simplified stand-alone Geant4 application " 
    << G4endl
    << "  g4annie_dirt_flux [options]  " << G4endl
    << "  -h --help           this output" << G4endl
    << "  -b --batch          run in batch mode " << G4endl
    << "  -v --verbose        increase verbosity [" << verbosity << "]" << G4endl
    << "  -n --nevents        limit # of events [" << nevents << "]" << G4endl
    << "  -r --repeat         repeat each GENIE event in G4 [" << nrepeat << "]" << G4endl
    << "  -g --gdml=GFILE     input geometry file [" << gdmlname << "]" << G4endl
    << "  -p --physics=PNAME  PhysicsList name [" << plname << "]" << G4endl
    << "                         (unknown names print the list)" << G4endl
    << "  -i --input=GNTP     GENIE file [" << infname << "]" << G4endl
    << "  -o --output=OFILE   output file [" << outfname << "]" << G4endl
    << " " << G4endl;
}


int main(int argc,char** argv)
{
  // parse the arguments
  static struct option long_options[] = {
    { "batch",   no_argument,       &batch_mode, 1 },
    { "verbose", no_argument,       0, 'v' },
    { "nevents", required_argument, 0, 'n' },
    { "repeat",  required_argument, 0, 'r' },
    { "gdml",    required_argument, 0, 'g' },
    { "physics", required_argument, 0, 'p' },
    { "input",   required_argument, 0, 'i' },
    { "output",  required_argument, 0, 'o' },
    { "help",    no_argument,       0, 'h' },
    // dont' forget to add short char to getopt_long list below
    { 0, 0, 0, 0 } // signal end
  };
  // getopt_long stored the option index here
#ifndef MACOSX
  optind = 0; // getopt.h: Reset getopt to start of arguments
#else
  optind = 1; // skip 0th argument (executable) for MACOSX
#endif
  int option_index = 0;
  int c;
  while ( true ) {
    c = getopt_long ( argc, argv, "vhn:r:g:p:i:o:bnqz", long_options, &option_index);
    if ( c == -1 ) break; // detect end of options
    switch ( c ) {
    case 0: {
      // if option set a flag, do nothing else
      if ( long_options[option_index].flag != 0 ) break;

      G4String oname = long_options[option_index].name;
      G4cout << "=========== long arg handling" << G4endl;
      if ( verbosity > 0 ) {
        G4cout << " oname=\"" << oname 
               << "\" optarg=\"" << optarg << "\"" << G4endl;
      }
      if      ( oname == "physics" ) plname   = optarg;
      else if ( oname == "gdml"    ) gdmlname = optarg;
      else if ( oname == "input"   ) infname  = optarg;
      else if ( oname == "output"  ) outfname = optarg;
      break;
    }
    case 'v':
      verbosity++;
      break;
    case 'n':
      nevents = atoi(optarg);
      break;
    case 'r':
      nrepeat = atoi(optarg);
      break;
    case 'b':
      batch_mode = 1;
      break;
    case 'q':
      batch_mode = 1;
      break;
    case 'g':
      //G4cout << " -g optarg=\"" << optarg << "\"" << G4endl;
      gdmlname = optarg;
      break;
    case 'p':
      //G4cout << " -p optarg=\"" << optarg << "\"" << G4endl;
      plname = optarg;
      break;
    case 'i':
      //G4cout << " -i optarg=\"" << optarg << "\"" << G4endl;
      infname = optarg;
      break;
    case 'o':
      //G4cout << " -o optarg=\"" << optarg << "\"" << G4endl;
      outfname = optarg;
      break;
    case 'h':
      PrintUsage();
      exit(0);
    case '?':
      // getopt_long already printed an error message
      PrintUsage();
      break;
    default:
      G4cout << "unknown opt=\"" << char(c) << "\""
             <<" optarg=\"" << optarg << "\"" 
             << G4endl;
      PrintUsage();
      exit(127);
    }
  }

  // Choose the Random engine
  //
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  
  // User Verbose output class
  //
  G4VSteppingVerbose::SetInstance(new SteppingVerbose);
     
  // Construct the default run manager
  //
  G4RunManager * runManager = new G4RunManager;

  // Set mandatory initialization classes
  //

  //DetectorConstruction* detector = 0;
  G4VUserDetectorConstruction* detector = 0;
  G4cout << "DetectorConstruction(\"" << gdmlname << "\")" << G4endl;
  detector = new DetectorConstruction(gdmlname);

  G4cout << "DetectorConstruction created" << G4endl;
  assert(detector);
  runManager->SetUserInitialization(detector);
  G4cout << "SetUserInitialization DetectorConstruction done" << G4endl;

  //
  // Run time choice on physics list
  //
  //old:  PhysicsList* physics = new PhysicsList;
  g4alt::G4PhysListFactory factory;

  G4VModularPhysicsList* physics = factory.GetReferencePhysList(plname);
  if ( physics ) {
    runManager->SetUserInitialization(physics);
  } else {
    factory.PrintAvailablePhysLists();
    exit(127);
  }
    

  MyEventRecord* myevtrec = new MyEventRecord;

  // Set user action classes
  PrimaryGeneratorAction* gen_action = 
    new PrimaryGeneratorAction(myevtrec,infname,verbosity);
  Long64_t nentries = gen_action->GetEntries();
  if ( nevents < 0 || nevents > nentries ) nevents = nentries;
  G4cout << "%%% process " << nevents << " of " << nentries << G4endl;

  runManager->SetUserAction(gen_action);
  //
  RunAction* run_action = new RunAction(myevtrec,verbosity);  
  runManager->SetUserAction(run_action);
  //
  EventAction* event_action = new EventAction(myevtrec,run_action,verbosity);
  runManager->SetUserAction(event_action);
  //
  SteppingAction* stepping_action =
    new SteppingAction(myevtrec, detector, event_action, verbosity);
  runManager->SetUserAction(stepping_action);
  
  // Initialize G4 kernel
  //
  runManager->Initialize();
  
#ifdef G4VIS_USE
  // Initialize visualization
  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();
#endif

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // treat remaining arguments as macro file names
  // to be processed in order
  while ( optind < argc ) {
    G4String command = "/control/execute ";
    G4String macrofile = argv[optind++];
    G4cout << command << macrofile << G4endl;
    UImanager->ApplyCommand(command+macrofile);
  }

  if ( ! batch_mode ) {
    // interactive mode : define UI session
#ifdef G4UI_USE
    G4UIExecutive* ui = new G4UIExecutive(argc, argv);
#ifdef G4VIS_USE
    UImanager->ApplyCommand("/control/execute vis.mac"); 
#endif
    if (ui->IsGUI())
      UImanager->ApplyCommand("/control/execute gui.mac");
    ui->SessionStart();
    delete ui;
#endif
  } else {
    myevtrec->nG4Repeat = nrepeat;
    myevtrec->nG4BeamOn = nevents;
    for (Long64_t irepeat = 0; irepeat < nrepeat; ++irepeat ) {

      gen_action->SetNextEntry(0);
      myevtrec->iter  = irepeat;
      myevtrec->niter = nrepeat;

      G4String beamOn = Form("/run/beamOn %lld",nevents);  // TString.h
      G4cerr << "about to do:  " << beamOn << " for repetition "
             << irepeat << G4endl << std::flush;

      UImanager->ApplyCommand(beamOn);
    }
  }

  G4cout << "%%% complete" << G4endl;

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  //                 owned and deleted by the run manager, so they should not
  //                 be deleted in the main() program !
#ifdef G4VIS_USE
  delete visManager;
#endif
  delete runManager;

  delete myevtrec;

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
