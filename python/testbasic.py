import cppjieqi
from collections import defaultdict
import random

g = cppjieqi.God()
g.Initialize()
t = g.PrintPos()
meta = g.GetMeta()
d = meta["random_map_red"]
tmp = defaultdict(int)
for k in d:
    tmp[d[k]] += 1
print(tmp)
moves = g.GenMoves()
state_red, di1, di0 = meta["state_red"], meta["di1"], meta["di0"]
one = random.sample(moves, 1)
g.Move(*one[0])
g.PrintPos()
one = random.sample(g.GenMoves(), 1)
t = g.Move(*one[0])
print(t["infodict"]["islegal"])
g.PrintPos()
g.UndoMove(2)
assert g.GetMeta()["state_red"] == state_red and g.GetMeta()["di1"] == di1 and g.GetMeta()["di0"] == di0
print(di0)