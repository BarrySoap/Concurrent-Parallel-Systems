library(ggplot2)
data <- ggplot(CourseworkTest, aes(x=as.numeric(as.character(Individual.Block.Times)), y = Index)) + geom_point() + scale_x_continuous(limits = c(0.0, 0.2)) + labs(title="Block Mining Time", x="Time Taken", y="Block Index")
print(data)