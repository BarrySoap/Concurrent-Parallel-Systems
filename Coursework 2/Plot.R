OriginalX <- Original$Pixels
OriginalY <- as.numeric(as.character(Original$Execution.Time))

MultiThreadingX <- Threaded$Pixels
MultiThreadingY <- as.numeric(as.character(Threaded$Execution.Time))

OpenMPX <- OpenMP$Pixels
OpenMPY <- as.numeric(as.character(OpenMP$Execution.Time))

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

cols = c("Pixels", "Execution.Time")
colnames(originalFrame) = cols
colnames(multiThreadingFrame) = cols
colnames(openMPFrame) = cols

library(ggplot2)

p = ggplot() + 
  geom_line(data = originalFrame, aes(x = Pixels, y = Execution.Time, color = "red")) +
  geom_line(data = multiThreadingFrame, aes(x = Pixels, y = Execution.Time, color = "green")) +
  geom_line(data = openMPFrame, aes(x = Pixels, y = Execution.Time, color = "purple")) +
  scale_color_manual(labels = c("Multi Threading", "OpenMP", "Original Scripts"), values = c("red", "green", "purple")) +
  xlab('Pixels') +
  ylab('Execution Time (seconds)') +
  scale_x_continuous(name="Pixels", labels = comma) +
  scale_y_continuous(name="Execution Time", labels = comma)

print(p)
