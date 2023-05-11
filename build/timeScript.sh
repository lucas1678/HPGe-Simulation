#!/bin/sh
halfLife=5100
numEvents=80000
numIons=100000
logTwo=$(echo 'l(2)' | bc -l)
lambdaFactor='expr $logTwo + 2'
echo "Half Life: ${halfLife} Number of Events: ${numEvents} Number of Ions: ${numIons} LogTwo: $lambdaFactor"
