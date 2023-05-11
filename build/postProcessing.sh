#!/bin/bash

outputNum=$1
sed -e '1,5d' "outputData${outputNum}_nt_Energy Deposition.csv" > gammaData.txt
root data_analysis.C
