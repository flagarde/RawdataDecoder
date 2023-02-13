#!/bin/bash
Particle="pi+"  #You need to change particle type here    #TODO 1
DAT_NAME="AHCAL_Run264_20221029_210505.dat"	#You need to change the dat file name here		#TODO 2


#####################
##################### The following controls the programme. 
##################### You don't need to change it.

DAT_DIR="/mnt2/BeamData/HCAL"
#ls $DAT_DIR/$Particle/$DAT_NAME > list_dat.txt
source setup.sh
hbuana -x
sed -i "s:DECODESWITCH:True:g" config.yaml  
sed -i "s:OUTPUTPATHHERE:/mnt2/USTC/Yukun/hbuana/result/$Particle:g" config.yaml  
hbuana -c config.yaml > log 2>&1
