# Adam Hendel
# University of Wisconsin - River Falls 2017
# R Script to train and artificial neural network
# selects Kd-Kp parameters based on input data
# input data is the output from Collection Script Data Processing

library(neuralnet)

# read in the data
setwd("C:/Dir")
d <- read.csv("RMS_ SteelBall_Training_LongLever.csv 2017-03-05 09;13;42 .csv", header = T)

# get rid of an uneeded column
# remove concatenated data column
d.1 <- d[,-1]

# record the sd, mean of each columns so we can unscale the after recall
zScoreDat <- data.frame(Kp = 0:1, Kd = 0:1, err = 0:1, sd = 0:1)
zScoreDat$Kp <- c(mean(d$Kp),sd(d$Kp))
zScoreDat$err <- c(mean(d$err), mean(d$sd))
zScoreDat$Kd <- c(mean(d$Kd),sd(d$Kd))
zScoreDat$sd <- c(sd(d$err), sd(d$err))
row.names(zScoreDat) <- c("mean", "sd")

# scale all the data using z-score
d.1 <- scale(d.1)

#d.1[,2:3] <- scale(d.1[,2:3])
d.1 <-as.data.frame(d.1)

# train the neural net
model <- neuralnet(as.formula(Kp + Kd ~ err + sd),
                   data = d.1,
                   hidden = 3,
                   act.fct = "logistic"
                   )

# find the parameters that would give 0 err, 0 sd (ideal case)
# err and sd must be in terms of z score of the model
err <- (0-zScoreDat["mean","err"]) / zScoreDat["sd", "err"]
sd <- (0-zScoreDat["mean","sd"]) / zScoreDat["sd", "sd"]

# compute the model, and format
optimal <- compute(model, data.frame(err = err, sd = sd))
optimal <- optimal$net.result
optimal <- as.data.frame(optimal)
names(optimal) <- c("Kp", "Kd")
# now unscale the data
optimal$Kp <- optimal$Kp*zScoreDat["sd","Kp"] + zScoreDat["mean","Kp"]
optimal$Kd <- optimal$Kd*zScoreDat["sd","Kd"] + zScoreDat["mean","Kd"]

# return the optimal parameters to console
optimal$Kp
optimal$Kd

