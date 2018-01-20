#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""Send input commands to the BTree test app.

ins X
rem X
(etc)

"""

import random

random.seed()

# Simple rotations

def test_000():
    lst = [10, 20, 30]
    for x in lst:
        print('ins %s' % x)
    #
    for x in lst:
        print('rem %s' % x)
    #

def test_001():
    lst = [30, 20, 10]
    for x in lst:
        print('ins %s' % x)
    #
    for x in lst:
        print('rem %s' % x)
    #

# Double rotations

def test_002():
    lst = [10, 20, 15]
    for x in lst:
        print('ins %s' % x)
    #
    for x in lst:
        print('rem %s' % x)
    #

def test_003():
    lst = [20, 10, 15]
    for x in lst:
        print('ins %s' % x)
    #
    for x in lst:
        print('rem %s' % x)
    #

# Simple rotations with weights

def test_004():
    lst = [10, 20, 30, 25, 35, 40]
    for x in lst:
        print('ins %s' % x)
    #
    for x in lst:
        print('rem %s' % x)
    #

def test_005():
    lst = [30, 20, 10, 5, 15, 1]
    for x in lst:
        print('ins %s' % x)
    #
    for x in lst:
        print('rem %s' % x)
    #

# Double rotations with weights

def test_006():
    lst = [10, 20, 15, 19, 17]
    for x in lst:
        print('ins %s' % x)
    #
    for x in lst:
        print('rem %s' % x)
    #

def test_007():
    lst = [20, 10, 15, 12, 13]
    for x in lst:
        print('ins %s' % x)
    #
    for x in lst:
        print('rem %s' % x)
    #

def test_02():
    lst = [281, 114, 159, 346, 121, 457, 345, 973, 656, 820]
    for x in lst:
        print('ins %s' % x)
    #
    for x in lst:
        print('rem %s' % x)
    #

def test_03():
    lst = [910, 362, 921, 784, 917, 294, 257, 598, 558, 528, 601]
    for x in lst:
        print('ins %s' % x)
    #
    for x in lst:
        print('rem %s' % x)
    #

def test_04():
    lst = [439, 310, 236, 282, 691, 928, 225, 965, 938, 931]
    for x in lst:
        print('ins %s' % x)
    #
    for x in lst:
        print('rem %s' % x)
    #

def test_05():
    lst = [820, 423, 834, 19, 116, 650]
    for x in lst:
        print('ins %s' % x)
    #
    for x in lst:
        print('rem %s' % x)
    #

def test_06():
    lst = [160, 103, 512, 458, 829, 211]
    for x in lst:
        print('ins %s' % x)
    #
    for x in lst:
        print('rem %s' % x)
    #

def test_07():
    lst = [899, 942, 284, 307, 341, 309, 532, 55, 257, 651, 708]
    for x in lst:
        print('ins %s' % x)
    #
    for x in lst:
        print('rem %s' % x)
    #

def test_08():
    lst = [665, 900, 446, 169, 134, 92, 451, 385, 659, 971]
    for x in lst:
        print('ins %s' % x)
    #
    for x in lst:
        print('rem %s' % x)
    #

def test_09():
    lst = [700, 308, 52, 817, 666, 488, 474, 444, 379]
    for x in lst:
        print('ins %s' % x)
    #
    for x in lst:
        print('rem %s' % x)
    #

def test_10():
    lst = [72, 623, 115, 174, 702, 227, 313, 729]
    for x in lst:
        print('ins %s' % x)
    #
    for x in lst:
        print('rem %s' % x)
    #

def test_99():
    elem = list(range(1,10001))
    random.shuffle(elem)
    for i in range(10000):
        x = elem.pop()
        print('ins %s' % x)
    elem = list(range(1,10001))
    random.shuffle(elem)
    for i in range(10000):
        x = elem.pop()
        print('rem %s' % x)
    #


def main():
    lst = [test_000, test_001, test_002, test_003, test_004, test_005, test_006, test_007,
        test_02, test_03, test_04, test_05, test_06, test_07, test_08, test_09, test_10]
    for x in lst:
        x()
        print("del")
    #


def main1():
    test_99()

if __name__ == '__main__': main1()

# end.