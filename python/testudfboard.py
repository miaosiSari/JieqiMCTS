from inspect import trace
from cppjieqi import God
import traceback

g = God()
print(g.Initialize(random=False, turn=False, board=
"d..gk...."
"....a...."
".CC......"
"........."
"........."
"........."
"........."
"........."
"........."
"....K...."))
try:
    g.Initialize(random=False, turn=False, board=
    "d..gk...."
    "....a...."
    ".CC......"
    "........."
    "........."
    "........."
    "........."
    "........."
    "........."
    ".....K...")
    print("AIHINT", g.AIHint())
except:
    print(traceback.format_exc())
try:
    g.Initialize(random=False, turn=False, board=
    "d..gk...."
    "....a...."
    ".CC......"
    "........."
    "........."
    "........."
    "........."
    "........."
    "........."
    "....KK...")
except:
    print(traceback.format_exc())
try:
    g.Initialize(random=False, turn=True, board=
    "d..g....."
    "....a...."
    ".CC......"
    "....k...."
    "........."
    "........."
    "........."
    "........."
    "........."
    "....KK...")
except:
    print(traceback.format_exc())
try:
    print(g.Initialize(random=False, turn=True, board=
    "d..g.k..."
    "....a...."
    ".CC......"
    "........."
    "........."
    "........."
    "I........"
    "........."
    "........."
    "....K...."))
    print(g.GetMeta())
    print(g.Hint())
except:
    print(traceback.format_exc())
