# ANNIEDirt - Docker branch

## About
ANNIEDirt (g4annie_dirt_flux, g4dirt, g4annie, etc.), created by Robert Hatcher, is part of ANNIE's simulation chain. 
It propagates final state particles from [GENIE](https://github.com/ANNIEsoft/GENIE-v3) through `/home/annie_v04.gdml` until they reach `TWATER_PV` (**T**ank **WATER** **P**hysical **V**olume) or another physical volume that contains `0x` (e.g. MRD and FMV components)
This can drastically reduce run time when studing neutrino events outside the ANNIE tank.

## `Compilation` Usage
```
cd anniedirt/
source setup_Dockerfile.sh
cd ../anniedirt_build/
cmake ../anniedirt -DCMAKE_INSTALL_PREFIX=../anniedirt -DCMAKE_CXX_STANDARD=14
make
make install
```

## `/home/g4annie_dirt_flux` Usage
```
g4annie_dirt_flux -h | --help           print usage statment
                  -b | --batch          run in batch mode
                  -v | --verbose        increase verbosity
                  -n | --nevents        limit # of events
                  -r | --repeat         repeat each GENIE event in G4
                  -g | --gdml=GFILE     input geometry file
                  -p | --physics=PNAME  PhysicsList name
                  -i | --input=GNTP     GENIE file
                  -o | --output=OFILE   output file
```

## `/home/run_g4dirt.sh` Usage
```
run_g4dirt.sh -r=<run number (or numbers using `*`. Ex: \`-r='4*'\`)>
              -i=</path/to/input/GENIE/files/dir>
              -n=<number of events per GENIE file to propigate>
              -g=</path/to/geometry/file.gdml>
              -o=</path/to/output/dir>
              -h|--help
```

## `/home/run_g4dirt.sh` Example Usage
`home $ source ./run_g4dirt.sh -r=0 -i=/input -n=10 -g=/home/annie_v04.gdml -o=/output`
- Will use run number 0. Aka, will use `/input/gntp.0.ghep.root`
- Input director is `/input`
- Will run first 10 events in `/input/gntp.0.ghep.root`
- Will propagate final state GENIE particles through `/home/annie_v04.gdml`
- Output files, including `.log` and `.root` files will be saved to `/output`

`home $ source ./run_g4dirt.sh -r='*' -i=/input -n='-1' -g=/home/annie_v04.gdml -o=/output`
- Will use all run numbers. Aka, will use `/input/gntp.*.ghep.root`
- Input director is `/input`
- Will run all events in `/input/gntp.*.ghep.root`
- Will propagate final state GENIE particles through `/home/annie_v04.gdml`
- Output files, including `.log` and `.root` files will be saved to `/output`
