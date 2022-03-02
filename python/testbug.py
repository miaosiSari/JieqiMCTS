from cppjieqi import Searcher, Node, God

s = Searcher()
print(s.Initialize(random=False, turn=True, board=
"....k.f.d"
"...a....."
".h.....h."
"i.iAi.i.i"
"........R"
"..b......"
"a...I.I.I"
".H.....H."
"...K....."
"D..G.GF.."))
s.pos()
print(s.ai())