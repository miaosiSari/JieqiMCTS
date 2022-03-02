from cppjieqi import God, UCCI

g = God()
g.Initialize(random=False, board=
"d..gk...."
"....a...."
"........."
"........."
"........."
"........."
".CC......"
"........."
"........."
".....K...")
aihint = g.AIHint(di=[0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0])
print(UCCI(aihint[0], aihint[1]))
print(aihint)