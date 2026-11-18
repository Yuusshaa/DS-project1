// Comprehensive MaxHeap and Heap Sort tests.
#include <cstdio>
#include "testkit.h"
#include "../model/maxheap.h"

int eventCount(EventLog& log, int type) {
    int count = 0;
    for (int i = 0; i < log.size(); i++) if (log.get(i).type == type) count++;
    return count;
}

void insertionSort(int* values, int count) {
    for (int i = 1; i < count; i++) {
        int value = values[i];
        int j = i - 1;
        while (j >= 0 && values[j] > value) { values[j + 1] = values[j]; j--; }
        values[j + 1] = value;
    }
}

void checkActiveHeap(MaxHeap& heap, Tally& T) {
    CHECK(heap.isMaxHeap());
    CHECK(heap.countViolations() == 0);
    CHECK(heap.heapSize >= 0 && heap.heapSize <= heap.size());
    CHECK(heap.isSortedRegionOk());
    CHECK(heap.isValidState());
}

void testIndexHelpersAndEmpty(Tally& T) {
    std::printf("Heap index helpers and empty cases\n");
    MaxHeap heap;
    CHECK(heap.parentOf(1) == 0);
    CHECK(heap.parentOf(2) == 0);
    CHECK(heap.parentOf(5) == 2);
    CHECK(heap.leftOf(0) == 1);
    CHECK(heap.rightOf(0) == 2);
    CHECK(heap.leftOf(3) == 7);
    CHECK(heap.rightOf(3) == 8);
    CHECK(heap.size() == 0 && heap.heapSize == 0);
    CHECK(heap.isMaxHeap() && heap.isSortedAscending() && heap.isValidState());
    CHECK(heap.countViolations() == 0);

    DynArray<int> badValues;
    badValues.push(1); badValues.push(3); badValues.push(2);
    MaxHeap notBuilt;
    notBuilt.loadArray(badValues);
    CHECK(notBuilt.isMaxHeap() == false);
    CHECK(notBuilt.countViolations() == 1);
    CHECK(notBuilt.isValidState() == false);
    CHECK(notBuilt.isSortedAscending() == false);

    EventLog log;
    heap.buildMaxHeap(log);
    CHECK(heap.heapSize == 0);
    CHECK(eventCount(log, EV_HEAP_BUILD) == 1);
    CHECK(log.get(log.size() - 1).type == EV_DONE);
    int result = 77;
    CHECK(heap.extractMax(result, log) == false);
    CHECK(heap.size() == 0 && heap.heapSize == 0);
    CHECK(log.get(log.size() - 1).type == EV_DONE);
}

int sameMultiset(MaxHeap& heap, int* values, int count) {
    int actual[128];
    int expected[128];
    if (heap.size() != count) return 0;
    for (int i = 0; i < count; i++) { actual[i] = heap.data[i]; expected[i] = values[i]; }
    insertionSort(actual, count);
    insertionSort(expected, count);
    for (int i = 0; i < count; i++) if (actual[i] != expected[i]) return 0;
    return 1;
}

void testBuildCases(Tally& T) {
    std::printf("Heap build arrays: singleton, duplicates, heap, ordered, negative\n");
    int inputs[8][10] = {
        {5},
        {4,4,4,4,4},
        {9,7,8,1,2,3,4},
        {1,2,3,4,5,6,7,8,9,10},
        {10,9,8,7,6,5,4,3,2,1},
        {-8,-1,-9,-3},
        {5,-2,0,5,11,-7,3},
        {0}
    };
    int lengths[] = {1,5,7,10,10,4,7,0};
    for (int c = 0; c < 8; c++) {
        DynArray<int> source;
        for (int i = 0; i < lengths[c]; i++) source.push(inputs[c][i]);
        MaxHeap heap;
        heap.loadArray(source);
        CHECK(heap.size() == lengths[c] && heap.heapSize == lengths[c]);
        CHECK(sameMultiset(heap, inputs[c], lengths[c]));
        EventLog log;
        heap.buildMaxHeap(log);
        checkActiveHeap(heap, T);
        CHECK(sameMultiset(heap, inputs[c], lengths[c]));
        CHECK(log.get(0).type == EV_HEAP_BUILD);
        CHECK(log.get(0).otherId == lengths[c]);
        CHECK(log.get(log.size() - 1).type == EV_DONE);
    }
}

void testSiftOperations(Tally& T) {
    std::printf("Sift up/down and swap events\n");
    DynArray<int> source;
    source.push(10); source.push(5); source.push(8); source.push(1);
    MaxHeap heap;
    heap.loadArray(source);
    EventLog build;
    heap.buildMaxHeap(build);
    heap.resetCounters();
    EventLog insert;
    CHECK(heap.insert(20, insert));
    CHECK(heap.data[0] == 20);
    CHECK(heap.swaps > 0 && heap.comparisons > 0);
    CHECK(eventCount(insert, EV_HEAP_SWAP) > 0);
    checkActiveHeap(heap, T);

    // Force a valid parent/child violation, then sift the root down.
    heap.data[0] = -100;
    EventLog down;
    heap.siftDown(0, heap.heapSize, down);
    CHECK(heap.isMaxHeap());
    CHECK(eventCount(down, EV_HEAPIFY_START) == 1);
    CHECK(eventCount(down, EV_HEAPIFY_DONE) == 1);

    // Direct swap also reports the original and moved values.
    EventLog swapLog;
    int oldFirst = heap.data[0];
    int oldSecond = heap.data[1];
    heap.swapAt(0, 1, HSW_PLAYER, swapLog);
    CHECK(heap.data[0] == oldSecond && heap.data[1] == oldFirst);
    CHECK(swapLog.size() == 1 && swapLog.get(0).type == EV_HEAP_SWAP);
    CHECK(swapLog.get(0).nodeId == 0 && swapLog.get(0).otherId == 1);
    CHECK(swapLog.get(0).oldValue == oldFirst && swapLog.get(0).newValue == oldSecond);
    CHECK(swapLog.get(0).code == HSW_PLAYER);
}

void testInsertExtractAndSortedRegion(Tally& T) {
    std::printf("Heap insert, extract, and sorted suffix\n");
    MaxHeap heap;
    DynArray<int> empty;
    heap.loadArray(empty);
    EventLog firstInsert;
    CHECK(heap.insert(-5, firstInsert));
    CHECK(heap.data[0] == -5 && heap.heapSize == 1);
    EventLog secondInsert;
    CHECK(heap.insert(12, secondInsert));
    CHECK(heap.data[0] == 12 && heap.heapSize == 2);
    checkActiveHeap(heap, T);

    int expectedDescending[] = {12, -5};
    for (int i = 0; i < 2; i++) {
        int max = 0;
        EventLog extraction;
        CHECK(heap.extractMax(max, extraction));
        CHECK(max == expectedDescending[i]);
        CHECK(heap.isValidState());
        CHECK(extraction.get(0).type == EV_EXTRACT_MAX);
        CHECK(extraction.get(0).value == max);
        CHECK(eventCount(extraction, EV_HEAP_SIZE_CHANGE) == 1);
        CHECK(eventCount(extraction, EV_MARK_SORTED) == 1);
        CHECK(extraction.get(extraction.size() - 1).type == EV_DONE);
    }
    CHECK(heap.heapSize == 0 && heap.isSortedAscending());
    int priorSize = heap.size();
    int ignored = 0;
    CHECK(heap.extractMax(ignored, firstInsert) == false);
    CHECK(heap.size() == priorSize);

    // A heap with an existing sorted suffix cannot accept another item.
    DynArray<int> values;
    values.push(10); values.push(7); values.push(3);
    MaxHeap partial;
    partial.loadArray(values);
    EventLog make;
    partial.buildMaxHeap(make);
    int removed = 0;
    CHECK(partial.extractMax(removed, make));
    int beforeSize = partial.size();
    int beforeActive = partial.heapSize;
    int before[8];
    for (int i = 0; i < beforeSize; i++) before[i] = partial.data[i];
    EventLog rejected;
    CHECK(partial.insert(100, rejected) == false);
    CHECK(partial.size() == beforeSize && partial.heapSize == beforeActive);
    for (int i = 0; i < beforeSize; i++) CHECK(partial.data[i] == before[i]);
    CHECK(rejected.get(rejected.size() - 1).type == EV_DONE);
}

void testHeapifyActivePrefix(Tally& T) {
    std::printf("Heapify only changes the active prefix\n");
    DynArray<int> source;
    int raw[] = {1,9,8,7,6,5,4};
    for (int i = 0; i < 7; i++) source.push(raw[i]);
    MaxHeap heap;
    heap.loadArray(source);
    EventLog build;
    heap.buildMaxHeap(build);
    heap.data.push(1000); // sorted suffix sentinel
    heap.heapSize = 7;
    heap.data[0] = -100;
    EventLog log;
    heap.siftDown(0, heap.heapSize, log);
    CHECK(heap.isMaxHeap());
    CHECK(heap.data[7] == 1000);
    CHECK(heap.isSortedRegionOk());
    int comparisons = heap.comparisons;
    EventLog leaf;
    heap.siftDown(6, heap.heapSize, leaf);
    CHECK(heap.data[7] == 1000);
    CHECK(heap.comparisons == comparisons);
}

void testSortReferenceCases(Tally& T) {
    std::printf("Heap sort: edge cases against independent sorted arrays\n");
    int inputs[8][12] = {
        {0}, {8}, {4,1,4,-2,0}, {-8,-1,-9,-3},
        {-4,-2,0,1,8,11}, {11,8,1,0,-2,-4},
        {6,-1,6,0,12,-20,2,12,3},
        {100,-100,0,100,-100,1,-1,0,1,-1}
    };
    int lengths[] = {0,1,5,4,6,6,9,10};
    for (int c = 0; c < 8; c++) {
        DynArray<int> source;
        int expected[16];
        for (int i = 0; i < lengths[c]; i++) {
            source.push(inputs[c][i]);
            expected[i] = inputs[c][i];
        }
        insertionSort(expected, lengths[c]);
        MaxHeap heap;
        EventLog log;
        heap.sortArray(source, log);
        CHECK(heap.heapSize == 0 && heap.size() == lengths[c]);
        CHECK(heap.isSortedAscending());
        CHECK(eventCount(log, EV_HEAP_SORT_COMPLETE) == 1);
        CHECK(log.get(log.size() - 1).type == EV_DONE);
        for (int i = 0; i < lengths[c]; i++) CHECK(heap.data[i] == expected[i]);
        int original[16];
        for (int i = 0; i < lengths[c]; i++) original[i] = inputs[c][i];
        CHECK(sameMultiset(heap, original, lengths[c]));
    }
}

void testCountersAndDisabledEvents(Tally& T) {
    std::printf("Heap counters and disabled event recording\n");
    DynArray<int> source;
    source.push(1); source.push(7); source.push(3); source.push(9);
    MaxHeap heap;
    heap.loadArray(source);
    EventLog quiet;
    quiet.enabled = false;
    heap.buildMaxHeap(quiet);
    CHECK(quiet.size() == 0);
    CHECK(heap.comparisons > 0);
    CHECK(heap.swaps > 0);
    heap.resetCounters();
    CHECK(heap.comparisons == 0 && heap.swaps == 0);
    CHECK(heap.isMaxHeap());
}

int main() {
    Tally T;
    testIndexHelpersAndEmpty(T);
    testBuildCases(T);
    testSiftOperations(T);
    testInsertExtractAndSortedRegion(T);
    testHeapifyActivePrefix(T);
    testSortReferenceCases(T);
    testCountersAndDisabledEvents(T);
    std::printf("\nPassed: %d  Failed: %d\n", T.passed, T.failed);
    return T.failed == 0 ? 0 : 1;
}
