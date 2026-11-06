//this is all the events 
#pragma once

// ---- Event types 
// tree events
#define EV_VISIT              0
#define EV_COMPARE            1
#define EV_INSERT             2
#define EV_UNLINK             3
#define EV_SWAP_VALUE         4
#define EV_ROTATE             5
#define EV_UPDATE_HEIGHT      6
#define EV_UPDATE_SIZE        7
#define EV_BALANCE_ALERT      8
#define EV_HIGHLIGHT_SUBTREE  9
// heap events
#define EV_HEAP_BUILD         10
#define EV_HEAP_SWAP          11
#define EV_HEAPIFY_START      12
#define EV_HEAPIFY_DONE       13
#define EV_EXTRACT_MAX        14
#define EV_MARK_SORTED        15
#define EV_HEAP_SIZE_CHANGE   16
#define EV_HEAP_SORT_COMPLETE 17
// end of a batch
#define EV_DONE               18

// ---- Side 
#define SIDE_NONE   0
#define SIDE_LEFT   1
#define SIDE_RIGHT  2

// ---- Compare result 
#define CMP_LESS    -1
#define CMP_EQUAL   0
#define CMP_GREATER 1

// ---- Rotation type 
#define ROT_NONE 0
#define ROT_LL   1
#define ROT_RR   2
#define ROT_LR   3
#define ROT_RL   4

// ---- Highlight mode 
#define HL_NONE    0
#define HL_VISITED 1
#define HL_SKIPPED 2
#define HL_RESULT  3

struct Event {
    int type;       // one of the EV_ values
    int nodeId;     // main node id 
    int otherId;    // parent id
    int value;      // key or value involved
    int oldValue;   // for UPDATE (stored so undo is possible)
    int newValue;
    int code;       // a SIDE_, CMP_, ROT_ or HL_ value
    char note[64];  // human-readable text for the Event Log panel

    Event() {
        type = EV_DONE;
        nodeId = -1;
        otherId = -1;
        value = 0;
        oldValue = 0;
        newValue = 0;
        code = 0;
        note[0] = '\0';
    }
};