from cppjieqi import Searcher, God, UCCI
import random

s = Searcher()
s.Initialize()
print(s.c_puct)
s.c_puct = 2
print(s.c_puct)
s.print()
print(s.rough())
print(s.move("a0c2", True))
g = s.god
print(g.Move("a0c2", True))
move = random.sample(g.GenMoves(), 1)[0]
print(UCCI(*move))
print(s.move(*move, True))