# RawdataDecoder

Rawdata to LCIO for HCAL-ECAL

How to use :

1) export STREAMOUT_OUTPUT_PATH=`somepath`

2)
  - To write LCIO (**prefered !**)
    ./RawdataToLCIO --filename ~/Bureau/AHCAL_Run143_20221024_072149.dat
  - To write LCIO (**Just for check or old softwares !**)
    ./bin/RawdataToROOT --filename ~/Bureau/AHCAL_Run143_20221024_072149.dat -e 1000
3) Files will be written to `somepath`
