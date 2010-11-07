import threading

from test_general import *
from time_profiler import timeprofile

tp = timeprofile()

read_channel('test', 0)

@run_in_thread
def do_writes(tid, count):
    #1000 writes
    tp.mark()
    for i in range(0, count):
        write_channel('test', 'Hello world', True)
        if i % 100 == 0:
            print '%s: %s' % (tid, i)
    print 'Elapsed for %s: %s writes %s' % (tid, count, tp.elapsed())

for i in range(0, 10):
    do_writes(i, 1000)
