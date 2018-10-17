library(ggplot2)
data <- ggplot(CourseworkTest, aes(x=as.numeric(as.character(Individual.Block.Times)), y = Index)) + geom_point() + scale_x_continuous(limits = c(0.0, 0.2)) + labs(title="Block Mining Time", x="Time Taken", y="Block Index")
data2 <- ggplot(CourseworkTest2, aes(x = Index, y=as.numeric(as.character(Individual.Block.Times)))) + geom_line() + scale_y_continuous(limits = c(0.0, 0.08)) + labs(title="Block Mining Time", x="Block Index", y="Time Taken")
print(data)
print(data2)

gg <- ggplot(diamonds, aes(x=carat, y=price, color=cut)) + geom_point() + labs(title="Scatterplot", x="Carat", y="Price")

gg1 <- gg + theme(plot.title=element_text(size=30, face="bold"), 
                  axis.text.x=element_text(size=15), 
                  axis.text.y=element_text(size=15),
                  axis.title.x=element_text(size=25),
                  axis.title.y=element_text(size=25)) + 
  scale_color_discrete(name="Cut of diamonds")  # add title and axis text, change legend title.

gg1 + facet_wrap( ~ cut, ncol=3)
