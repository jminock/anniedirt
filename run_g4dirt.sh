#! /usr/bin/env bash

main() {
export INFILE=""
export OUTDIR=""
export VERBOSE=2
export RWHNODE=0
export RUNNUM=""  # starting run number
export BATCH=""
source /Geant4_install/bin/geant4.sh
source /setup_Dockerfile.sh

for i in "$@"; do
  case $i in
    -r=*        ) export RUNNUM="${i#*=}"               shift    ;;
    -i=*        ) export INFILE="${i#*=}"                shift    ;;
    -n=*        ) export NEVENTS="${i#*=}"              shift    ;;
    -g=*        ) export GEOMETRY="${i#*=}"             shift    ;;
    -o=*        ) export OUTDIR="${i#*=}"               shift    ;;
    -h*|--help* ) usage;                                return 1 ;;
    -*          ) echo "Unknown option \`$i\`."; usage; return 1 ;;
  esac
done

if [ -z "${RUNNUM}" ]; then
  echo "Use \`-r=\` to set the run number (or numbers using \`*\`. Ex: \`-r='4*'\`)."
  return 1
fi

if [ -z "${INFILE}" ]; then
  echo "Use \`-i=\` to set the input directory (ex: -p=/pnfs/annie/persistent/users/...)."
  return 2
fi

if [ -z "${NEVENTS}" ]; then
  echo "Use \`-n=\` to set the number of events per input file to propigate (-n=-1 for all)."
  return 3
fi

if [ -z "${GEOMETRY}" ]; then
  echo "Use \`-g=\` to set the annie geometry (ex: -g=/pnfs/annie/persistent/users/.../name.gdml)"
  return 4
fi

if [ -z "${OUTDIR}" ]; then
  echo "Use \`-o=\` to set the output directory (will create a new file if needed)."
  return 5
fi

export USEPHYLIST="FTFP_BERT_HP"
mkdir ${OUTDIR}
#cd ${INDIR}

#for INFILE in ${INDIR}/gntp.${RUNNUM}.ghep.root; do 
if [ -f "${INFILE}" ]; then
  export CURRUNNUM=$(basename ${INFILE})
  export OUTFILE=annie_tank_flux.0.root
  export OUTFILELOG=annie_tank_flux.${CURRUNNUM}.log

  cat <<EOF > ${OUTFILELOG}
#============================================#
#=============== RUN SETTINGS ===============#
#============================================#
NUISANCE file (in dir): ${INFILE}
 g4dirt file (out dir): ${OUTFILE}
       g4dirt file log: annie_tank_flux.${CURRUNNUM}.log
              commmand: $B/g4annie_dirt_flux --batch -n ${NEVENTS} -g ${GEOMETRY} --physics=${USEPHYLIST} -i ${INFILE} -o ${OUTFILE}


#======================================#
#=============== OUTPUT ===============#
#======================================#
EOF

# I know the `-i ./$basename ${INFILE})` looks really dumb, but its necessary... I learned the hard way
  ../anniedirt_install/bin/g4annie_dirt_flux --batch -n ${NEVENTS} -g ${GEOMETRY} --physics=${USEPHYLIST} -i ${INFILE} -o ${OUTFILE} 2>&1 | tee -a ${OUTFILELOG}

  mv -f annie_tank_flux.* ${OUTDIR}
  rm -f currentEvent.rndm
  rm -f currentRun.rndm
fi
#done

#cd -
}

usage() {
cat >&2 <<EOF
run_g4dirt.sh -r=<run number (or numbers using \`*\`. Ex: \`-r='4*'\`)>
              -i=</path/to/input/NUISANCE/files/dir>
              -n=<number of events per NUISANCE file to propigate>
              -g=</path/to/geometry/file.gdml>
              -o=</path/to/output/dir>
              -h|--help
EOF
}

main "$@"
