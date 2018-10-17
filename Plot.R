library(ggplot2)
data <- ggplot(CourseworkTest, aes(x=Individual.Block.Times, y = Index)) + geom_point()
print(data)
