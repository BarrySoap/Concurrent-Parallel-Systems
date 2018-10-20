NewScriptsX <- CourseworkTestNew$Difficulty
NewScriptsY <- as.numeric(as.character(CourseworkTestNew$Average.Block.Time))

OriginalScriptsX <- CourseworkTestOriginal$Difficulty
OriginalScriptsY <- as.numeric(as.character(CourseworkTestOriginal$Average.Block.Time))

NewX <- CourseworkTestTotal$Difficulty
NewY <- as.numeric(as.character(CourseworkTestTotal$Average.Block.Time))

newScriptsFrame <- data.frame(
  NewScriptsX,
  NewScriptsY
)

originalScriptsFrame <- data.frame(
  OriginalScriptsX,
  OriginalScriptsY
)

newFrame <- data.frame(
  NewX,
  NewY
)

cols = c("Difficulty", "Average.Block.Time")
colnames(originalScriptsFrame) = cols
colnames(newScriptsFrame) = cols
colnames(newFrame) = cols

p = ggplot() + 
  geom_line(data = newScriptsFrame, aes(x = Difficulty, y = Average.Block.Time, color = "red")) +
  geom_line(data = originalScriptsFrame, aes(x = Difficulty, y = Average.Block.Time, color = "blue")) +
  geom_line(data = newFrame, aes(x = Difficulty, y = Average.Block.Time, color = "green")) +
  scale_color_manual(labels = c("Original Scripts", "New Scripts", "Test"), values = c("blue", "red", "green")) +
  xlab('Block Difficulty') +
  xlim(1, 4) +
  ylab('Average Block Times') +
  scale_y_continuous(trans='log2')

print(p)
