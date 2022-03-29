from cppjieqi import God
import argparse

def humanmove():
    m, legal = "", False
    g.PrintPos()
    while len(m) != 4 or not legal:
        m = input("Your move:").strip()
        info = g.Move(m, True)
        legal = info['infodict']['islegal']
    g.PrintPos(not g.GetTurn(), True, False, True)
    return info['infodict']['win']

def aimove(depth):
    result = g.AIHint(depth, [])
    info = g.Move(result[0], result[1], False)
    g.PrintPos(not g.GetTurn(), True, False, True)
    return info['infodict']['win']

def move(first, depth):
    if first == 2:
        if aimove(depth): return True
        if aimove(depth): return True
    elif first == 1:
        if aimove(depth): return True
        if humanmove(): return True
    else:
        if humanmove(): return True
        if aimove(depth): return True
    return False

depth = 8

if __name__ == '__main__':
    parser = argparse.ArgumentParser() 
    parser.add_argument('--first', type=int, default=0, help='0: User first, 1: AI first, 2: aiplay')
    parser.add_argument('--depth', type=int, default=8, help='ai depth')
    args = parser.parse_args()
    first = args.first
    depth = args.depth
    print(first, depth)
    g = God()
    g.Initialize(random=False)
    while True:
        if move(first, depth): break
