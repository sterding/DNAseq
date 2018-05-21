### GATK SNP call shell script
################################
# Pipeline for SNP call of single sample using GATK
# Authors: Bioinformatics Team @ Scherzer's lab 
# Email: gliu@mgh.harvard.edu
# Version:0.1
################################
#!/bin/bash

USAGEMSG="usage: $(basename $0) [-b input bam file][-g genome reference file][-l Target-region file]

-b input bam file
-g input genome fasta file
-l interval lists (can be Picard-style .interval_list, GATK-style .list, BED files with extension .bed, and VCF files)

Pre-requirement GATK  R package TEQC

"
[ $# -lt 2 ] && echo "$USAGEMSG" >&2 && exit 1

while getopts "b:g:l:" opt
do
	case $opt in
	(b) BAM=$OPTARG;;
	(g) GENOME=$OPTARG;;
	(l) TARGET=$OPTARG;;
	(*) echo "$0: error - unrecognized option $1" >&2; exit 1;;
	esac
done

##### search input files available or not
if [ -f $BAM ]
then
echo "Find input bam file";
else
echo "no such input bam file";exit 0;
fi

if [ -f $GENOME ]
then
echo "Find input genome file";
else
echo "no such input genome file";exit 0;
fi

if [ -f $TARGET ]
then
echo "Find input target bed file";
else
echo "no such target file";exit 0;
fi

######## set up a new folder for each sample analysis

samplename=`basename $BAM .bam`

mkdir $samplename.GATK
cd $samplename.GATK

###############
#  Coverage report for each target region
###############
## mean.coverage  stdDev
# R package TEQC Bioconductor to perform Target Enrichment Quality Control
module load R/3.3.0

Rscript ~/neorogen/pipeline/DNAseq/modules/TEQC.R $TARGET $BAM $samplename

############# GATK #######
# export PATH=$PATH:~/neorogen/pipeline/genotyping/gatk-4.0.4.0/gatk
# https://software.broadinstitute.org/gatk/best-practices/
# https://software.broadinstitute.org/gatk/documentation/article?id=11050
# https://docs.google.com/file/d/0B2dK2q40HDWeYzVTUGs0bjM3M1E/preview

#gatk --java-options "-Xmx8G" HaplotypeCaller -R $GENOME -I $BAM -O $samplename.g.vcf -ERC GVCF --variant_index_type LINEAR

gatk HaplotypeCaller \
    -R $GENOME \
    -I $BAM \
    -O $samplename.g.vcf \
    -ERC GVCF \
    -L $TARGET \
    --QUIET \
    --create-output-variant-index FALSE




