####################################
# module for DNA sequening data alignment
# Authors: Xianjun Dong, PhD
# Email: xdong@rics.bwh.harvard.edu
# Date: 05/04/2018
# Version: 0.1
####################################
#!/bin/bash

samplename=$1 
GENOME_INDEX=$2
n_threads=$3

# load required application/modules
module load bwa/0.7.16a
module load samtools/1.4.1
module load picard/2.7.1

# For debug test:
# samplename="rawfiles/H1402-1-Plate37-H12_S32"
# GENOME_INDEX=/data/neurogen/referenceGenome/Homo_sapiens/UCSC/hg19/Sequence/BWAIndex/genome

#Create index (if not exist) from FASTA input using bwa index -p prefix reference.fa
#bwa index ...

#Align the input FASTQ files against reference genome using BWA-MEM
bwa mem -M -t $n_threads -T 30 -o $samplename.sam $GENOME_INDEX $samplename.R1.fastq.gz $samplename.R2.fastq.gz

# filter unmapped read pair (both R1 and R2 unmapped)
samtools view -F 12 $samplename.sam | samtools sort - -O bam -@ $n_threads -o $samplename.sorted.bam

##Convert SAM files to BAM files and fix read pairing information and flags using 
# samtools fixmate -O bam $samplename.sam | samtools sort - -O bam -@ $n_threads -o $samplename.sorted.bam  # sort by read name (-n)??

# Mark duplicates (if selected) 
java -Xmx16g -jar picard.jar MarkDuplicates ASSUME_SORTED=true INPUT=$samplename.sorted.bam OUTPUT=$samplename.picard.bam METRICS_FILE=$samplename.metrics.out

# Using picard output bam from Picard as bam for downstream
ln -fs $samplename.picard.bam $samplename.bam

#Index BAM file using 
samtools index -b $samplename.bam

#Calculate alignment statistics (if selected) using 
samtools flagstat $samplename.bam > $samplename.stats.out