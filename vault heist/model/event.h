// Event types and the data recorded for one model operation.
#pragma once

// Tree operation events.
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
// Heap operation events.
#define EV_HEAP_BUILD         10
#define EV_HEAP_SWAP          11
#define EV_HEAPIFY_START      12
#define EV_HEAPIFY_DONE       13
#define EV_EXTRACT_MAX        14
#define EV_MARK_SORTED        15
#define EV_HEAP_SIZE_CHANGE   16
#define EV_HEAP_SORT_COMPLETE 17
// Marks the end of one operation's event batch.
#define EV_DONE               18

// Child side values.
#define SIDE_NONE   0
#define SIDE_LEFT   1
#define SIDE_RIGHT  2

// Comparison result values.
#define CMP_LESS    -1
#define CMP_EQUAL   0
#define CMP_GREATER 1

// AVL rotation cases.
#define ROT_NONE 0
#define ROT_LL   1
#define ROT_RR   2
#define ROT_LR   3
#define ROT_RL   4

// Subtree highlight reasons.
#define HL_NONE    0
#define HL_VISITED 1
#define HL_SKIPPED 2
#define HL_RESULT  3

struct Event {
    int type;       // one of the EV_ values
    int nodeId;     // main node id 
    int otherId;    // Related node or array index, depending on the event.
    int value;      // key or value involved
    int oldValue;   // Previous value, used when describing or undoing changes.
    int newValue;
    int code;       // a SIDE_, CMP_, ROT_ or HL_ value
    char note[64];  // Short description shown in the event log.

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
    void setNote(const char* text) {
        int i = 0;
        while (text[i] != '\0' && i < 63) {
            note[i] = text[i];
            i++;
        }
        note[i] = '\0';
    }
};