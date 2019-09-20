#ifndef LIMITS_H
#define LIMITS_H

#define MAX_NAME                0x200
#define MAX_SYMOL_NAME         0x50
#define MAX_SYMBOL_COUNT        0x1000000
#define MAX_LIB_COUNT           0x200
#define MAX_THREADS             0x100
#define MAX_THREAD_NUMBER       0x1000
#define MAX_CALL_LEVELS         0x200
#define GRAPH_START             100
#define MAX_LOOP_ADDRS          0x10
//#define MAX_THREADS 0x1000000
//#define MAX_PROPAGATIONS_OBSERVED 0x4000000 //67 milions
//#define MAX_PROPAGATIONS_OBSERVED 0x4000000 //134 milions
//#define MAX_PROPAGATIONS_OBSERVED 0xa000000 //167 milions
//#define MAX_PROPAGATIONS_OBSERVED 0xc000000 //200 milions
//#define MAX_PROPAGATIONS_OBSERVED 0x20000000 //? milions
#define MAX_PROPAGATIONS_OBSERVED 0x40000000 //? milions
#define MAX_TAINTS_OBSERVED     0x400
#define MAX_EXCEPTIONS_COUNT    0x100
#define MAX_BREAKPOINTS         0x50 //temporarily?
#define MAX_SECURITY_LAYERS     0x1000
#define MAX_BLACKLIST           0x50
#define MAX_WANTED              0x100
#define MAX_LOOP_FENCES         0x10
#define MAX_LOOP_ADDR           0x50
#define MAX_LIST_JXX            0x1000
#define MAX_LAST_INSTRUCTIONS   0x50
#define MAX_COMMANDS            0x50

#define MAX_CAUSES              0x8
#define MAX_EXTENDED_CAUSES_ENTRIES  0x100000
#define MAX_EXTENDED_CAUSES     0x20

#define MAX_RESULTS             0x8
#define MAX_EXTENDED_RESULTS_ENTRIES 0x100000
#define MAX_EXTENDED_RESULTS    0x20

#endif
