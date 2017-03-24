library(ggplot2)


dset <- "infile.csv"

d <- read.csv(dset, header = T)

trial <- "Long Lever"

# # # PLOT Y AXIS DATA # # #
d$man.qPos <- d$Man.yPos + d$Man.ySD/sqrt(20)
d$man.qNeg <- d$Man.yPos - d$Man.ySD/sqrt(20)
d$NN.qPos <- d$NN.yPos + d$NN.ySD/sqrt(20) 
d$NN.qNeg <- d$NN.yPos - d$NN.ySD/sqrt(20)
d$OrigMan.qPos <- d$OrigMan.yPos + d$OrigMan.ySD/sqrt(20) 
d$OrigMan.qNeg <- d$OrigMan.yPos - d$OrigMan.ySD/sqrt(20) 


ggplot(d, aes(x = Time)) +   geom_hline(yintercept = 500, size = 1.2) +
  geom_ribbon(aes(ymin = man.qNeg, ymax = man.qPos, col = "Manual (S2)", alpha = 0.5, fill = "Manual (S2)")) +
  geom_line(size = 1.2, aes(y=Man.yPos, col = "Manual (S2)")) +  
  
  geom_ribbon(aes(ymin = NN.qNeg, ymax = NN.qPos, col = "Neural Net", alpha = 0.5, fill = "Neural Net")) +
  geom_line(size = 1.2, aes(y=NN.yPos, col = "Neural Net")) +  
  
  geom_ribbon(aes(ymin = OrigMan.qNeg, ymax = OrigMan.qPos, col = "Manual (S1)", alpha = 0.5, fill = "Manual (S1)")) +
  geom_line(size = 1.2, aes(y=OrigMan.yPos, col = "Manual (S1)")) +  
  
  scale_fill_discrete(name = "Method", breaks = c("Manual (S2)","Neural Net", "Manual (S1)")) +
  scale_colour_discrete(guide=F) +
  scale_alpha_continuous(guide=F) +
  scale_y_continuous(name = "Ball Position", breaks = seq(0,700, 100)) +
  scale_x_continuous(name = "Time (milliseconds)", breaks = seq(0,2000,250)) +
  labs(title = paste("2 Second Response Test\nSingle Axis (Y)\n", trial))
  
# # # PLOT X AXIS DATA  # # #

d$man.qPos <- d$Man.xPos + d$Man.xSD/sqrt(20)
d$man.qNeg <- d$Man.xPos - d$Man.xSD/sqrt(20)
d$NN.qPos <- d$NN.xPos + d$NN.xSD/sqrt(20) 
d$NN.qNeg <- d$NN.xPos - d$NN.xSD/sqrt(20)
d$OrigMan.qPos <- d$OrigMan.xPos + d$OrigMan.xSD/sqrt(20) 
d$OrigMan.qNeg <- d$OrigMan.xPos - d$OrigMan.xSD/sqrt(20) 




ggplot(d, aes(x = Time)) +   geom_hline(yintercept = 500, size = 1.2) +
  geom_ribbon(aes(ymin = man.qNeg, ymax = man.qPos, col = "Manual (S2)", alpha = 0.5, fill = "Manual (S2)")) +
  geom_line(size = 1.2, aes(y=Man.xPos, col = "Manual (S2)")) +  
  
  geom_ribbon(aes(ymin = NN.qNeg, ymax = NN.qPos, col = "Neural Net", alpha = 0.5, fill = "Neural Net")) +
  geom_line(size = 1.2, aes(y=NN.xPos, col = "Neural Net")) +  
  
  geom_ribbon(aes(ymin = OrigMan.qNeg, ymax = OrigMan.qPos, col = "Manual (S1)", alpha = 0.5, fill = "Manual (S1)")) +
  geom_line(size = 1.2, aes(y=OrigMan.xPos, col = "Manual (S1)")) +  
  
  scale_fill_discrete(name = "Parameter\n Method", breaks = c("Manual (S2)","Neural Net", "Manual (S1)")) +
  scale_colour_discrete(guide=F) +
  scale_alpha_continuous(guide=F) +
  scale_y_continuous(name = "Ball Position", breaks = seq(0,700, 100)) +
  scale_x_continuous(name = "Time (milliseconds)", breaks = seq(0,2000,250)) +
  labs(title = paste("2 Second Response Test\nSingle Axis (X)\n", trial))

# # # XY Positional
manCI <- sqrt(d$Man.xSD^2 + d$Man.ySD^2)
NNCI <- sqrt(d$NN.xSD^2 + d$NN.ySD^2)

ggplot(d,aes(x=Man.xPos, y=Man.yPos)) +
  # Show setpoint
  geom_hline(yintercept = 500, size = 1.2) +
  geom_vline(xintercept = 500, size = 1.2) +
  # add layer for manual (s1)
  geom_point(col = "red") + 
  geom_ribbon(aes(ymin = Man.yPos-manCI, ymax = Man.yPos+manCI, 
                  col = "Manual (S1)", 
                  alpha = 0.5, 
                  fill = "Manual (S1)"),
              col = "red",
              fill = "red") +
  # add neural net layer
  # for some reason the ribbon here is not adding to the data points
  geom_ribbon(aes(x = d$NN.xPos, ymin = d$NN.yPos-NNCI, ymax = d$NN.yPos+NNCI, 
                  col = "Neural Net", 
                  alpha = 0.5, 
                  fill = "Neural Net"),
              col = "blue", 
              fill = "blue") +
  geom_point(aes(x = d$NN.xPos, y = d$NN.yPos), col = "blue")  

