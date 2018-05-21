####################################
# module to call structural variation (SV) from bam file
# Authors: Xianjun Dong, PhD
# Email: xdong@rics.bwh.harvard.edu
# Date: 05/08/2018
# Version: 0.1
####################################
#!/bin/bash

## prerequirement
# install delly, lumpy, manta, SURVIVOR
# cd ~/neurogen/pipeline/DNAseq/libs
# Delly:
# =======
# wget https://github.com/dellytools/delly/releases/download/v0.7.8/delly_v0.7.8_linux_x86_64bit
# ln -fs delly_v0.7.8_linux_x86_64bit delly

# # Lumpy
# # =======
# git clone --recursive git@github.com:arq5x/lumpy-sv.git
# cd lumpy-sv
# make
# 
# # Manda
# # ======
# wget https://github.com/Illumina/manta/releases/download/v1.4.0/manta-1.4.0.centos6_x86_64.tar.bz2
# tar -xjf manta-1.4.0.centos6_x86_64.tar.bz2
# mkdir build && cd build
# export CC=/path/to/cc
# export CXX=/path/to/c++
# ../manta-1.4.0.centos6_x86_64/configure --jobs=4 --prefix=$HOME
# make -j4 install


samplename=$1
GENOME_INDEX=$2

# delly
delly call \
-x ../libs/hg19.excl \
-o $samplename.delly.bcf \
-g $GENOME_INDEX.fa \
$samplename.bam

bcftools view $samplename.delly.bcf > $samplename.delly.vcf

# lumpy 
lumpyexpress \
-g $GENOME_INDEX.fa \
-B $samplename.bam \
-o $samplename.lumpy.vcf \
-x ../libs/hg19.excl \
-K ~/bin/lumpyexpress.config

# manta
configManta.py \
--bam $samplename.bam \
--referenceFasta $GENOME_INDEX.fa \
--runDir $samplename.manta

# SURVIVOR
ls *vcf > $samplename.vcflist
SURVIVOR merge $samplename.vcflist 1000 2 1 1 0 30 $samplename.merged.vcf