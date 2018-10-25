OriginalX <- CourseworkTestOriginal$Difficulty
OriginalY <- as.numeric(as.character(CourseworkTestOriginal$Average.Block.Time))

ThreadPoolX <- ThreadPool$Difficulty
ThreadPoolY <- as.numeric(as.character(ThreadPool$Average.Block.Time))

MultiThreadingX <- MultiThreading$Difficulty
MultiThreadingY <- as.numeric(as.character(MultiThreading$Average.Block.Time))

OpenMPX <- OpenMP$Difficulty
OpenMPY <- as.numeric(as.character(OpenMP$Average.Block.Time))

CPUInstructionsX <- CPUInstructions$Difficulty
CPUInstructionsY <- as.numeric(as.character(CPUInstructions$Average.Block.Time))

originalFrame <- data.frame(
  OriginalX,
  OriginalY
)

threadPoolFrame <- data.frame(
  ThreadPoolX,
  ThreadPoolY
)

multiThreadingFrame <- data.frame(
  MultiThreadingX,
  MultiThreadingY
)

openMPFrame <- data.frame(
  OpenMPX,
  OpenMPY
)

CPUInstructionsFrame <- data.frame(
  CPUInstructionsX,
  CPUInstructionsY
)

cols = c("Difficulty", "Average.Block.Time")
colnames(originalFrame) = cols
colnames(threadPoolFrame) = cols
colnames(multiThreadingFrame) = cols
colnames(openMPFrame) = cols
colnames(CPUInstructionsFrame) = cols

library(ggplot2)

p = ggplot() + 
  geom_line(data = originalFrame, aes(x = Difficulty, y = Average.Block.Time, color = "red")) +
  geom_line(data = threadPoolFrame, aes(x = Difficulty, y = Average.Block.Time, color = "blue")) +
  geom_line(data = multiThreadingFrame, aes(x = Difficulty, y = Average.Block.Time, color = "green")) +
  geom_line(data = openMPFrame, aes(x = Difficulty, y = Average.Block.Time, color = "purple")) +
  geom_line(data = CPUInstructionsFrame, aes(x = Difficulty, y = Average.Block.Time, color = "grey")) +
  scale_color_manual(labels = c("Thread Pool", "Original Scripts", "Multi Threading", "OpenMP", "CPU Instrucions"), values = c("blue", "red", "green", "purple", "grey")) +
  xlab('Block Difficulty') +
  ylab('Average Block Times') +
  scale_y_continuous(trans='log2')

print(p)
