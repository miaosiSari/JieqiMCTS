from cppjieqi import God
from collections import defaultdict
import random

g = God()
g.Initialize(random=True)
g.PrintPos()
result = g.AIHint()
print(g.Hint())
move = g.UCCI(result[0], result[1])
print(move)
g.Move(move)
g.PrintPos()
print(g.Rough())
print(g.GetMeta()["random_map_red"])
