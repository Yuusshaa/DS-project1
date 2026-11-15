// model/maxheap.h  -- must NOT include raylib
// Zero-based array heap. No node objects.
// parent = (i - 1) / 2, left = 2i + 1, right = 2i + 2.
#pragma once
#include "dynamicarray.h"
#include "eventlog.h"

// Reasons stored in the "code" field of EV_HEAP_SWAP
#define HSW_SIFT_UP    1
#define HSW_SIFT_DOWN  2
#define HSW_EXTRACT    3
#define HSW_PLAYER     4

class MaxHeap {
public:
    DynArray<int> data;   // whole array: active heap first, sorted region after it
    int heapSize;         // how many items at the front are still the active heap
    int comparisons;      // key comparisons made (for benchmarks)
    int swaps;            // swaps made (for benchmarks)

    MaxHeap() {
        heapSize = 0;
        comparisons = 0;
        swaps = 0;
    }

    int parentOf(int i) {
        return (i - 1) / 2;
    }
    int leftOf(int i) {
        return 2 * i + 1;
    }
    int rightOf(int i) {
        return 2 * i + 2;
    }

    void record(EventLog& log, int type, int nodeId, int otherId,
                int value, int code, const char* text) {
        Event e;
        e.type = type;
        e.nodeId = nodeId;
        e.otherId = otherId;
        e.value = value;
        e.code = code;
        e.setNote(text);
        log.add(e);
    }

    void resetCounters() {
        comparisons = 0;
        swaps = 0;
    }

    // Copy an array in. It is NOT a heap yet; call buildMaxHeap next.
    void loadArray(DynArray<int>& source) {
        data.clear();
        for (int i = 0; i < source.size(); i++) {
            data.push(source[i]);
        }
        heapSize = data.size();
    }

    int size() {
        return data.size();
    }

    // EV_HEAP_SWAP: N = index A, O = index B,
    //               Old = value that WAS at A, New = value that WAS at B (now at A),
    //               Code = HSW_ reason.
    void swapAt(int i, int j, int reason, EventLog& log) {
        Event e;
        e.type = EV_HEAP_SWAP;
        e.nodeId = i;
        e.otherId = j;
        e.oldValue = data[i];
        e.newValue = data[j];
        e.code = reason;
        e.setNote("Swap array cells");
        log.add(e);

        int temp = data[i];
        data[i] = data[j];
        data[j] = temp;
        swaps = swaps + 1;
    }

    // Moves the item at index i down until both children are smaller or equal.
    // Only looks at indexes below 'size'.
    // EV_HEAPIFY_START / DONE: N = start index, O = active heap size.
    void siftDown(int i, int size, EventLog& log) {
        int start = i;
        record(log, EV_HEAPIFY_START, start, size, 0, 0, "Heapify start");
        while (true) {
            int left = leftOf(i);
            int right = rightOf(i);
            int largest = i;

            if (left < size) {
                comparisons = comparisons + 1;
                if (data[left] > data[largest]) {
                    largest = left;
                }
            }
            if (right < size) {
                comparisons = comparisons + 1;
                if (data[right] > data[largest]) {
                    largest = right;
                }
            }
            if (largest == i) {
                break;
            }
            swapAt(i, largest, HSW_SIFT_DOWN, log);
            i = largest;
        }
        record(log, EV_HEAPIFY_DONE, start, size, 0, 0, "Heapify done");
    }

    // Moves the item at index i up while it is bigger than its parent.
    void siftUp(int i, EventLog& log) {
        while (i > 0) {
            int parent = parentOf(i);
            comparisons = comparisons + 1;
            if (data[i] > data[parent]) {
                swapAt(i, parent, HSW_SIFT_UP, log);
                i = parent;
            } else {
                break;
            }
        }
    }

    // Bottom-up build, O(n). EV_HEAP_BUILD: O = active heap size.
    void buildMaxHeap(EventLog& log) {
        int lastParent = heapSize / 2 - 1;
        record(log, EV_HEAP_BUILD, -1, heapSize, 0, 0, "Build max heap");
        for (int i = lastParent; i >= 0; i--) {
            siftDown(i, heapSize, log);
        }
        record(log, EV_DONE, -1, -1, 0, 0, "Heap built");
    }

    // Adds at the end, then sifts up. Only allowed while no sorted region exists.
    bool insert(int value, EventLog& log) {
        if (heapSize != data.size()) {
            record(log, EV_DONE, -1, -1, 0, 0, "Cannot insert: sorted region exists");
            return false;
        }
        int oldSize = heapSize;
        data.push(value);
        heapSize = heapSize + 1;
        Event e;
        e.type = EV_HEAP_SIZE_CHANGE;
        e.oldValue = oldSize;
        e.newValue = heapSize;
        e.nodeId = heapSize - 1;
        e.setNote("Heap size grew");
        log.add(e);
        siftUp(heapSize - 1, log);
        record(log, EV_DONE, -1, -1, 0, 0, "Inserted");
        return true;
    }

    // Takes the root out of the active heap and puts it in the sorted region.
    bool extractMax(int& result, EventLog& log) {
        if (heapSize == 0) {
            record(log, EV_DONE, -1, -1, 0, 0, "Heap is empty");
            return false;
        }
        result = data[0];
        int last = heapSize - 1;
        record(log, EV_EXTRACT_MAX, 0, last, result, 0, "Extract the maximum");

        if (last > 0) {
            swapAt(0, last, HSW_EXTRACT, log);
        }

        int oldSize = heapSize;
        heapSize = heapSize - 1;
        Event e;
        e.type = EV_HEAP_SIZE_CHANGE;
        e.oldValue = oldSize;
        e.newValue = heapSize;
        e.nodeId = last;
        e.setNote("Heap size shrank");
        log.add(e);

        record(log, EV_MARK_SORTED, last, -1, result, 0, "Moved to sorted region");

        if (heapSize > 1) {
            siftDown(0, heapSize, log);
        }
        record(log, EV_DONE, -1, -1, 0, 0, "Extracted");
        return true;
    }

    // Build, then extract until empty. Afterwards the array is sorted ascending.
    void heapSort(EventLog& log) {
        buildMaxHeap(log);
        int ignored = 0;
        while (heapSize > 0) {
            extractMax(ignored, log);
        }
        record(log, EV_HEAP_SORT_COMPLETE, -1, -1, data.size(), 0, "Heap sort complete");
        record(log, EV_DONE, -1, -1, 0, 0, "Sorted");
    }

    void sortArray(DynArray<int>& source, EventLog& log) {
        loadArray(source);
        heapSort(log);
    }

    // Is every parent in the ACTIVE heap >= its children?
    bool isMaxHeap() {
        for (int i = 1; i < heapSize; i++) {
            if (data[i] > data[parentOf(i)]) {
                return false;
            }
        }
        return true;
    }

    int countViolations() {
        int count = 0;
        for (int i = 1; i < heapSize; i++) {
            if (data[i] > data[parentOf(i)]) {
                count = count + 1;
            }
        }
        return count;
    }

    bool isSortedRegionOk() {
        for (int i = heapSize + 1; i < data.size(); i++) {
            if (data[i - 1] > data[i]) {
                return false;
            }
        }
        if (heapSize > 0 && heapSize < data.size()) {
            if (data[heapSize] < data[0]) {
                return false;
            }
        }
        return true;
    }

    bool isSortedAscending() {
        for (int i = 1; i < data.size(); i++) {
            if (data[i - 1] > data[i]) {
                return false;
            }
        }
        return true;
    }

    bool isValidState() {
        if (heapSize < 0 || heapSize > data.size()) {
            return false;
        }
        return isMaxHeap() && isSortedRegionOk();
    }
};