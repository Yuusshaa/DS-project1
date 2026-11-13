/*The EventLog

Stores events in order, using your DynArray.
Records them through log(...) and logChange(...).
Counts alarm steps from VISIT and COMPARE events.
Switches off for benchmarks through enabled = false, so nothing is stored.

*/

#pragma once
#include "dynamicarray.h"
#include "event.h"

class EventLog {
public:
    bool enabled;            // true = record events, false = ignore them for benchmarks
    DynArray<Event> events;  // the stored events, in order

    EventLog() {
        enabled = true;
    }

    // Store one event at the end of the log.
    void add(Event e) {
        if (enabled == false) {
            return;          // recording is off, so do nothing
        }
        events.push(e);
    }

    // How many events are stored.
    int size() {
        return events.size();
    }

    // Get the event at position i.
    Event& get(int i) {
        return events[i];
    }

    // Throw away all stored events.
    void clear() {
        events.clear();
    }

    // A visit or a comparison consumes one alarm step.
    int countAlarmSteps() {
        int total = 0;
        for (int i = 0; i < events.size(); i++) {
            if (events[i].type == EV_VISIT || events[i].type == EV_COMPARE) {
                total = total + 1;
            }
        }
        return total;
    }
};
