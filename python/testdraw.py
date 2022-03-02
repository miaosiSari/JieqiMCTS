from cppjieqi import Searcher, Node, God

s = Searcher()
s.Initialize()
s.expand({"x": 1, "y": 2})
s.walk(["x"])
s.expand({"w": 3, "z": 4})
s.reset()
s.walk(["y"])
s.expand({"a": 5, "b": 6})
s.reset()
s.draw()
s.pos()
print(s.ai())