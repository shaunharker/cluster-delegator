#!/bin/bash
#PBS -l nodes=10:ppn=8
cd $PBS_O_WORKDIR
mpiexec ./my_program
