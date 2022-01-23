from cppjieqi import God
from collections import defaultdict
import random

g = God()
g.Initialize(False)
g.PrintPos()
result = g.AIHint()
move = g.UCCI(result[0], result[1])
print(move)
g.Move(move)
g.PrintPos()
