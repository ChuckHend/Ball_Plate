# Adam Hendel
# University of Wisconsin - River Falls 2017
# R Script using K-Means clustering
# selects Kd-Kp parameters based on input data
# input data is the output from Collection Script Data Processing

library(class)

# read in the data
setwd("C:/Users/AdamHendel/OneDrive/Academics/Ball and Plate/Data Analysis")
d <- read.csv("RMS_ SteelBall_Training_LongLever.csv 2017-03-05 09;13;42 .csv", header = T)

# get rid of the concatenated kd.kp column
d.1 <- d[,-1]

# scale all data using z-score method
d.1 <- scale(d.1)
d.1 <- as.data.frame(d.1)

# record the sd, mean of each columns so we can unscale the data later
zScoreDat <- data.frame(Kp = 0:1, Kd = 0:1, err = 0:1, sd = 0:1)
zScoreDat$Kp <- c(mean(d$Kp),sd(d$Kp))
zScoreDat$err <- c(mean(d$err), mean(d$sd))
zScoreDat$Kd <- c(mean(d$Kd),sd(d$Kd))
zScoreDat$sd <- c(sd(d$err), sd(d$err))
row.names(zScoreDat) <- c("mean", "sd")

# optimize k by testing range of centers
wss = 0
numStart = 1000
numCenters = 20
for (i in 1:numCenters){
  wss[i] <- sum(kmeans(d.1, centers=i, nstart = numStart)$withinss)
}

plot(1:numCenters, wss, type="b", xlab="Num Clusters",
     ylab="WWithin Sum Squares", main = "WCSS Elbow plot")
# pick points on plot which increment in K makes a diminished return to WCSS
# select number of clusters use elbow plot to determine k value

k = 4
km <- kmeans(d.1, centers = k, nstart = 20)

optimal <- km$centers
optimal <- data.frame(optimal)
# unscale the data
optimal$Kp <- optimal$Kp*zScoreDat["sd","Kp"] + zScoreDat["mean","Kp"]
optimal$Kd <- optimal$Kd*zScoreDat["sd","Kd"] + zScoreDat["mean","Kd"]
optimal$err <- optimal$err*zScoreDat["sd","err"] + zScoreDat["mean","err"]
optimal$sd <- optimal$Kd*zScoreDat["sd","sd"] + zScoreDat["mean","sd"]

# return to console parameters of centroid w/ lowest error
optimal[optimal$err == min(optimal$err),][3:4]
# or view all centroids
optimal
