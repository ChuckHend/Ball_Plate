# Adam Hendel
# University of Wisconsin - River Falls 2017
# R Script processes raw output from  test scripts
# Gets the mean and SD of all of system response
# for all iterations in test file
# indata must be formatted 0-2000ms for each iteration

# directory where raw data saved
setwd("c:/Users/Adam/OneDrive/Academics/Ball and Plate/Data Analysis")

# string for filename (outfile name included infile name)
infile <- "GolfBall_NN Testing.csv"

# read the raw data  
d <- read.csv(infile, header = T)

# set up a dataframe to fill
d.1 <- data.frame(Time = unique(d$Time.step),
                  Err = 1:length(unique(d$Time.step)),
                  sd = 1:length(unique(d$Time.step)),
                  xPos = 1:length(unique(d$Time.step)),
                  yPos = 1:length(unique(d$Time.step)),
                  xSD = 1:length(unique(d$Time.step)),
                  ySD = 1:length(unique(d$Time.step)))

# write the mean err and sd to respective columns of new data frame
for (i in 1:length(d.1$Time)){
  # find rows for i'th time interval
  boolVec <- d$Time.step == d.1$Time[i]
  # extract those rows from our data set
  d.2 <- d[boolVec,]
  # find mean and sd of all error position for that time interval
  d.1$Err[i] <- mean(d.2$Err, na.rm = T)
  d.1$sd[i] <- sd(d.2$Err, na.rm = T)
  d.1$xPos[i] <- mean(d.2$POS.X, na.rm = T)
  d.1$yPos[i] <- mean(d.2$POS.Y, na.rm = T)
  d.1$xSD[i] <- sd(d.2$POS.X, na.rm = T)
  d.1$ySD[i] <- sd(d.2$POS.Y, na.rm = T)
}

# include date/time in filenam
fname <- paste("PlotData",infile,Sys.time(),".csv")
# remove the illegal characters from file name
fname <- gsub(":",";", fname)
# save data to file
write.csv(d.1, file = fname, row.names = F)

# plot error vs time, with error bars
time <- ggplot(d.1, aes(x=d.1$Time, y=d.1$Err))
time + geom_line() + geom_errorbar(ymin = d.1$Err-d.1$sd, ymax = d.1$Err+d.1$sd)

# plot single axis
ggplot(d.1, aes(x = d.1$Time, y = d.1$xPos)) + # base plot
  geom_line(col = "black") +                   # x axis data
  geom_line(aes(y = d.1$yPos), col = "red") +  # y axis data
  geom_hline(yintercept = 500) 
  
# plot overhead view
overHead <- ggplot(d.1, aes(x = d.1$xPos, y = d.1$yPos))
overHead + geom_line()
  geom_line(d.1, aes(x = d.1$xPos, y = d.1$yPos + d.1$ySD))


