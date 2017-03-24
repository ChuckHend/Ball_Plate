# Adam Hendel
# University of Wisconsin - River Falls 2017
# Script processes raw output from  collection scripts 
# Converts data to single observation of each Proportional 
# and Derivative combination, along with its corresponding
# mean squared error (using pythag error) and std dev.

library(stringr)

# set working directory
setwd("c:/Users/Adam/OneDrive/Academics/Ball and Plate/Data Analysis")

infile <- "GolfBall_TrainingScript.csv"
# read in the data collected
d <- read.csv(infile, header = T)

# add an error column, as distance away from SP in both x and y direction
d$error <- sqrt(abs(d$POS.X - d$SP.X)^2 + abs(d$POS.Y - d$SP.Y)^2)

# make all Kd values positive (only negative for servo direction)
d$Kd <- abs(d$Kd)

# create identifier for each Kd-Kp combination
d$combination <- paste(d$Kp, d$Kd)

# get vectors of unique values for Kp and Kd
uniqueKp <- unique(d$Kp)
uniqueKd <- unique(d$Kd)
uniqComb <- unique(d$combination)
sumData <- as.data.frame(uniqComb)

for (i in 1:length(uniqComb)){
  # find rows of the unique combination
  boolVec <- d$combination == uniqComb[i]
  # extract those rows from our data set
  d.1 <- d[boolVec,]
  # sum up the errors of those rows
  sumErrs <- mean(d.1$error, na.rm = T)
  sumData$err[i] <- sumErrs
  sumData$sd[i] <- sd(d.1$err, na.rm = T)
}
# split the unique combo to 2 strings
split <- str_split_fixed(sumData$uniqComb, " ", 2)
split <- as.data.frame(split)

# add the 2 columns to the end of the sumdata frame
sumData$Kp <- split$V1
sumData$Kd <- split$V2

# include date/time in filenam
fname <- paste("RMS_",infile,Sys.time(),".csv")
# remove the illegal characters from file name
fname <- gsub(":",";", fname)
# save data to file
write.csv(sumData, file = fname, row.names = F)