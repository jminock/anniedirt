///----------------------------------------------------------------------------
/**
 * \class MyEventRecord
 * \file  MyEventRecord.hh
 *
 * \brief What to write out for g4annie_dirt_flux
 *
 * \author (last to touch it) $Author: rhatcher $
 *
 * \version $Revision: 1.1 $
 *
 * \date $Date: 2015-08-27 04:47:28 $
 *
 * Contact: rhatcher@fnal.gov
 *
 * $Id: MyEventRecord.hh,v 1.1 2012-11-21 04:47:28 rhatcher Exp $
 *
 */
///----------------------------------------------------------------------------

#ifndef MYEVENTRECORD_H
#define MYEVENTRECORD_H

#include <string>
#include <ostream>

class G4PrimaryVertex;
class G4PrimaryParticle;
class G4Track;
class G4StepPoint;

// need a ROOT class so we have Long64_t defined
#include "TTree.h"


class MyEventRecord
{
public:
  MyEventRecord();
  virtual ~MyEventRecord();

  void clear();

  void AppendG4PrimaryParticle(G4PrimaryVertex* g4vert, 
                               G4PrimaryParticle* g4part);

  void AppendG4Step(G4Track* g4trk, G4StepPoint* g4steppt);

  void EraseNeutrino();

public:

  int         run;        ///< run number
  int         entry;      ///< entry in NUISANCE flat output tree
  int         iter;       ///< n-th interation of the above (0...niter-1)
  int         niter;      ///< max iterations

  int         nupdg;      ///< neutrino PDG code
  double      nuvtxx;     ///< neutrino interaction vertex x (cm)
  double      nuvtxy;     ///< neutrino interaction vertex y (cm)
  double      nuvtxz;     ///< neutrino interaction vertex z (cm)
  double      nuvtxt;     ///< neutrino interaction vertex t (nanosec)

  int         intank;     ///< interaction was in tank TWATER_PV
  int         inhall;     ///< interaction was in hall EXP_HALL_PV
  //std::string vtxvol;     ///< name of vtx volume
  //std::string vtxmat;     ///< material of vtx volume
  char        vtxvol[4096];
  char        vtxmat[4096];

  static const int mxtank = 4096;

  int         ntank;            ///< number of particle entering tank
  int         pdgtank[mxtank];  ///< pdg of particle entering tank
  int         primary[mxtank];  ///< particle is primary from GENIE
  double      vx[mxtank];       ///< point of entry to tank
  double      vy[mxtank];
  double      vz[mxtank];
  double      vt[mxtank];
  double      px[mxtank];       ///< momentum component upon entry
  double      py[mxtank];
  double      pz[mxtank];
  double      E[mxtank];        ///< total energy
  double      kE[mxtank];       ///< kinetic energy

  /////////// meta data not to be written per event

  std::string inputFileName;    ///<!  inputFileName
  Long64_t    inputNEntries;    ///<!  input file # of entries
  double      inputTotalPOTs;   ///<!  equiv exposure of input file
  double      fScaleFactor;     ///<!  Scale Factor

  Long64_t    nG4Repeat;        ///<!  # of passes over NUISANCE file
  Long64_t    nG4BeamOn;        ///<!  # passed to /run/beamOn
                                ///<!     (first N NUISANCE entries)
  Long64_t    nG4Events;        ///<!  total # of G4/NUISANCE events processed

  double      POTsPerPass;      ///<!  POTs per pass
  double      POTs;             ///<!  POTs this (total) file represents

private:
  // nothing

}; // end-of-class MyEventRecord

std::ostream& operator<<(std::ostream& os, const MyEventRecord& mer);


#endif // MYEVENTRECORD_H
