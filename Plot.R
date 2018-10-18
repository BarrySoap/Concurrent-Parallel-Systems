library(ggplot2)
data <- ggplot(CourseworkTest, aes(x=as.numeric(as.character(Individual.Block.Times)), y = Index)) + geom_point() + scale_x_continuous(limits = c(0.0, 0.2)) + labs(title="Block Mining Time", x="Time Taken", y="Block Index")
data2 <- ggplot(CourseworkTest2, aes(x = Index, y=as.numeric(as.character(Individual.Block.Times)))) + geom_line() + scale_y_continuous(limits = c(0.0, 40.0)) + labs(title="Block Mining Time", x="Block Index", y="Time Taken")
print(data)
print(data + data2)

# gg <- ggplot(diamonds, aes(x=carat, y=price, color=cut)) + geom_point() + labs(title="Scatterplot", x="Carat", y="Price")
# 
# gg1 <- gg + theme(plot.title=element_text(size=30, face="bold"), 
#                   axis.text.x=element_text(size=15), 
#                   axis.text.y=element_text(size=15),
#                   axis.title.x=element_text(size=25),
#                   axis.title.y=element_text(size=25)) + 
#   scale_color_discrete(name="Cut of diamonds")  # add title and axis text, change legend title.
# 
# gg1 + facet_wrap( ~ cut, ncol=3)

testVarX <- CourseworkTest$Index
testVarY <- as.numeric(as.character(CourseworkTest$Individual.Block.Times))

testVarX2 <- CourseworkTest2$Index
testVarY2 <- as.numeric(as.character(CourseworkTest2$Individual.Block.Times))

testFrame <- data.frame(
  testVarX,
  testVarY
)

testFrame2 <- data.frame(
  testVarX2,
  testVarY2
)

cols = c("Index", "Individual.Block.Times")
colnames(testFrame) = cols
colnames(testFrame2) = cols

p = ggplot() + 
  geom_line(data = testFrame, aes(x = Index, y = Individual.Block.Times), color = "blue") +
  geom_line(data = testFrame2, aes(x = Index, y = Individual.Block.Times), color = "red") +
  xlab('Block Index') +
  ylab('Individual Block Times')

print(p)

# For linear regression and modeling #
index <- as.numeric(as.character(1:999)) # time
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

plot(index, blockTimes, "l", las=1, xlab="Block Index", ylab="Individual Block Time") # Linear Regression 2nd Pass
lines(pr.lm2~index, col="green", lwd=2)

plot(index, blockTimes, "l", las=1, xlab="Block Index", ylab="Individual Block Time") # LOESS
lines(pr.loess~index, col="red", lwd=2)

library(zoo)
mov.avg <- rollmean(blockTimes, 5, fill=NA)                                           # Running Average
plot(index, blockTimes, "l")
lines(index, mov.avg, col="orange", lwd=2)
