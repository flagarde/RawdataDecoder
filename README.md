# RawdataDecoder

## Generate ROOT/LCIO files

Rawdata to LCIO for HCAL-ECAL

How to run :

1) export STREAMOUT_OUTPUT_PATH=`somepath`
2)

- To write LCIO (**prefered !**)
  ./RawdataToLCIO --filename **File.dat**
- To write ROOT (**Just for check or old softwares !**)
  ./RawdataToROOT --filename **File.dat** -e **NumberEvents**

3) Files will be written to `somepath`
4) Enjoy

## Comparator code

Plot comparator graphs

If you want more plot, you can add code in Compartor{.h,.cc}

How to run :

1) export STREAMOUT_OUTPUT_PATH=`somepath`
2) ./Comparaison --original **OriginalFile.root** --new **NewFile.root** --pdf_name **MyResult** (no .pdf)
3) Enjoy
