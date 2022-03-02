from cppjieqi import Searcher
import numpy as np

for cnt in range(10000):
    s = Searcher()
    s.Initialize()
    t = s.meta()[0]
    state = s.state()
    f = {
        'R': 0,
        'N': 1,
        'B': 2,
        'A': 3,
        'K': 4,
        'C': 5, 
        'P': 6,
        'D': 7,
        'E': 8,
        'F': 9,
        'G': 10,
        'H': 11,
        'I': 12,
        'r': 13,
        'n': 14,
        'b': 15,
        'a': 16,
        'k': 17,
        'c': 18,
        'p': 19,
        'd': 20,
        'e': 21,
        'f': 22,
        'g': 23,
        'h': 24,
        'i': 25
    }
    for i in range(9):
        for j in range(10):
            for k in range(26):
                x = 195 - 16*j + i
                index = f.get(state[x])
                if abs(t[k][j][i] - 1.0) < 1e-7: assert(index == k)
                elif abs(t[k][j][i]) < 1e-7: assert(index != k)
                else: assert False