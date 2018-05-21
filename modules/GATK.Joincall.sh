### GATK join call shell script
################################
# Pipeline for SNP join call of multiple sample using GATK
# Authors: Bioinformatics Team @ Scherzer's lab 
# Email: gliu@mgh.harvard.edu
# Version:0.1
################################
#!/bin/bash

USAGEMSG="usage: $(basename $0) [-f input GVCF folder][-g genome reference file][-l Target-region file][-o output tag]

-f input g.vcf folder
-g input genome fasta file
-l interval lists (can be Picard-style .interval_list, GATK-style .list, BED files with extension .bed, and VCF files)

Pre-requirement GATK

"
[ $# -lt 3 ] && echo "$USAGEMSG" >&2 && exit 1

while getopts "f:g:l:o:" opt
do
	case $opt in
	(f) GVCFFOLDER=$OPTARG;;
	(g) GENOME=$OPTARG;;
	(l) TARGET=$OPTARG;;
	(o) OUTPUT=$OPTARG;;
	(*) echo "$0: error - unrecognized option $1" >&2; exit 1;;
	esac
done

##### search input files available or not
if [ -f $GENOME ]
then
echo "Find genome file";
else
echo "no such input genome file";exit 0;
fi

if [ -f $TARGET ]
then
echo "Find target file";
else
echo "no such input genome file";exit 0;
fi
############ GATK #######

############### joint calling with Genotype GVCFs

# Perform joint genotyping on a single-sample GVCF from HaplotypeCaller or a multi-sample GVCF from CombineGVCFs or
# GenomicsDBImport

###
find $GVCFFOLDER -name "*.g.vcf" > input.list

#gatk CombineGVCFs -R $GENOME --variant input.list -L $TARGET -O Combine.g.vcf

gatk CombineGVCFs -R $GENOME --variant input.list -O Combine.g.vcf

#gatk --java-options "-Xmx8G" GenotypeGVCFs -R $GENOME -L $TARGET -V Combine.g.vcf -O $OUTPUT.vcf

gatk GenotypeGVCFs -R $GENOME -L $TARGET -V Combine.g.vcf -O $OUTPUT.vcf

rm input.list Combine.g.vcf*

