library(ggplot2)
data <- ggplot(CourseworkTest, aes(x=as.numeric(as.character(Individual.Block.Times)), y = Index)) + geom_point() + scale_x_continuous(limits = c(0.0, 0.2)) + labs(title="Block Mining Time", x="Time Taken", y="Block Index")
data2 <- ggplot(CourseworkTest2, aes(x = Index, y=as.numeric(as.character(Individual.Block.Times)))) + geom_line() + scale_y_continuous(limits = c(0.0, 40.0)) + labs(title="Block Mining Time", x="Block Index", y="Time Taken")
print(data)

testVarX <- CourseworkTest$Difficulty
testVarY <- as.numeric(as.character(CourseworkTest$Individual.Block.Times))   # Old

testVarX2 <- CourseworkTest2$Difficulty
testVarY2 <- as.numeric(as.character(CourseworkTest2$Individual.Block.Times)) # New

testFrame <- data.frame(
  testVarX,
  testVarY
)

testFrame2 <- data.frame(
  testVarX2,
  testVarY2
)

cols = c("Difficulty", "Individual.Block.Times")
colnames(testFrame) = cols
colnames(testFrame2) = cols

p = ggplot() + 
  geom_line(data = testFrame, aes(x = Difficulty, y = Individual.Block.Times, color = "red")) + # Original Scripts
  geom_line(data = testFrame2, aes(x = Difficulty, y = Individual.Block.Times, color = "blue")) + # New Scripts
  xlab('Block Difficulty') +
  xlim(1, 4) +
  ylab('Individual Block Times') +
  scale_y_continuous(trans='log2')

print(p)

# For linear regression and modeling #
index <- as.numeric(as.character(1:99)) # time
blockTimes <- as.numeric(as.character(CourseworkTest2$Individual.Block.Times)) # temp

# Generate first order linear model
lin.mod <- lm(blockTimes~index)

# Generate second order linear model
lin.mod2 <- lm(blockTimes~I(index^2)+index)

# Calculate local regression
ls <- loess(blockTimes~index)

# Predict the data (passing only the model runs the prediction 
# on the data points used to generate the model itself)
pr.lm <- predict(lin.mod)
pr.lm2 <- predict(lin.mod2)
pr.loess <- predict(ls)

par(mfrow=c(2,2))
plot(index, blockTimes, "l", las=1, xlab="Block Index", ylab="Individual Block Time") # Linear Regression
lines(pr.lm~index, col="blue", lwd=2)
legend(1, 1, legend=c("Line 1", "Line 2"),
       col=c("red", "blue"), lty=1:2, cex=0.8)

plot(index, blockTimes, "l", las=1, xlab="Block Index", ylab="Individual Block Time") # Linear Regression 2nd Pass
lines(pr.lm2~index, col="green", lwd=2)

plot(index, blockTimes, "l", las=1, xlab="Block Index", ylab="Individual Block Time") # LOESS
lines(pr.loess~index, col="red", lwd=2)

library(zoo)
mov.avg <- rollmean(blockTimes, 5, fill=NA)                                           # Running Average
plot(index, blockTimes, "l")
lines(index, mov.avg, col="orange", lwd=2)
