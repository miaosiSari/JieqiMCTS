from inspect import trace
from cppjieqi import God, Searcher
import traceback

s = Searcher()
print(s.Initialize(random=False, turn=False, board=
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
    s.Initialize(random=False, turn=False, board=
    "d..gk...."
    "....a...."
    ".CC......"
    "........."
    "........."
    "........."
    "........."
    "........."
    "........."
    "......K..")
except:
    print(traceback.format_exc())
try:
    s.Initialize(random=False, turn=False, board=
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
    s.Initialize(random=False, turn=True, board=
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
    print(s.Initialize(random=False, turn=True, board=
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
    print(s.god.GetMeta())
    print(s.god.Hint())
    print(s.god)
except:
    print(traceback.format_exc())
