OriginalX <- CourseworkTestOriginal$Difficulty
OriginalY <- as.numeric(as.character(CourseworkTestOriginal$Average.Block.Time))

MultiThreadingX <- MultiThreading$Difficulty
MultiThreadingY <- as.numeric(as.character(MultiThreading$Average.Block.Time))

OpenMPX <- OpenMP$Difficulty
OpenMPY <- as.numeric(as.character(OpenMP$Average.Block.Time))

originalFrame <- data.frame(
  OriginalX,
  OriginalY
)

multiThreadingFrame <- data.frame(
  MultiThreadingX,
  MultiThreadingY
)

openMPFrame <- data.frame(
  OpenMPX,
  OpenMPY
)

cols = c("Difficulty", "Average.Block.Time")
colnames(originalFrame) = cols
colnames(multiThreadingFrame) = cols
colnames(openMPFrame) = cols

library(ggplot2)

p = ggplot() + 
  geom_line(data = originalFrame, aes(x = Difficulty, y = Average.Block.Time, color = "red")) +
  geom_line(data = multiThreadingFrame, aes(x = Difficulty, y = Average.Block.Time, color = "green")) +
  geom_line(data = openMPFrame, aes(x = Difficulty, y = Average.Block.Time, color = "purple")) +
  scale_color_manual(labels = c("Multi Threading", "OpenMP", "Original Scripts"), values = c("red", "green", "purple")) +
  xlab('Block Difficulty') +
  ylab('Average Block Times (seconds)') +
  scale_y_continuous(trans='log10')

print(p)
