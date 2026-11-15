// model/eventlog.h  -- must NOT include raylib
#pragma once
#include "dynamicarray.h"
#include "event.h"

class EventLog {
public:
    bool enabled;            // true = record events, false = ignore them (benchmarks)
    DynArray<Event> events;  // the stored events, in order

    EventLog() {
        enabled = true;
    }

    void add(Event e) {
        if (enabled == false) {
            return;
        }
        events.push(e);
    }

    int size() {
        return events.size();
    }

    Event& get(int i) {
        return events[i];
    }

    void clear() {
        events.clear();
    }

    // Number of EV_VISIT events.
    int countVisits() {
        int total = 0;
        for (int i = 0; i < events.size(); i++) {
            if (events[i].type == EV_VISIT) {
                total = total + 1;
            }
        }
        return total;
    }

    // Alarm rule: every EV_VISIT and every EV_COMPARE costs 1.
    int countAlarm() {
        int total = 0;
        for (int i = 0; i < events.size(); i++) {
            if (events[i].type == EV_VISIT || events[i].type == EV_COMPARE) {
                total = total + 1;
            }
        }
        return total;
    }
};