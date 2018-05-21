library(TEQC)

args<-commandArgs(TRUE)
Targetfile=args[1]
Readfile=args[2]
resultoutput=args[3]


#####
reads.input <- get.reads(Readfile, filetype = "bam")

targets.input <- get.targets(Targetfile, chrcol = 1, startcol = 2, endcol = 3, skip = 0)


TEQCreport(sampleName = resultoutput,
           targetsName = "Human PD targets",
           referenceName = "Human Genome hg19",
           destDir = "TEQC.report",
           reads = reads.input,
           targets = targets.input,
           genome = "hg19")



#readpairs <- reads2pairs(reads)

Coverage <- coverage.target(reads.input, targets.input, perTarget=T, perBase=F)

targets.cov<-Coverage$targetCoverages

OutputCoverage<-paste(resultoutput,".coverage.csv", sep="")

#write.table(as.data.frame(targets.cov), file=OutputCoverage, sep=",", row.names=F, quote=F)

write.csv(as.data.frame(targets.cov), file = OutputCoverage, row.names=FALSE)


