# gcollect

C reference garbage collector.

This rather pithy garbage collector is designed to manage any reference that can be allocated from the Heap
by the Process Memory Manager (the Host OS).

At any time, you can see how many allocations there are, you can free them, check if they are freed, and so on.

You have the option to specify, upon allocation of a memory buffer, how long the buffer is, and/or what type is was registered to hold.

Have fun examining my 'gc.c' SOURCE and 'gc.h' HEADER file. Comments/suggestions/complaints/insults are all welcome, ya Nazi f*ck.

Written by Dave, 2024. Earth 66.

# EOF
