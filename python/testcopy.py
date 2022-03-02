from cppjieqi import Searcher, Node, God

g = Searcher()
while True:
   g.Initialize(board=
   "d..gk...."
   "....a...."
   ".CC......"
   "........."
   "........."
   "........."
   "........."
   "........."
   "........."
   "....K...."
   )
   g.copy()
   node = g.get()
   if node:
      nodedi = node.di()
      print(nodedi)
      assert nodedi[5] != 5
   else:
      print(None)
