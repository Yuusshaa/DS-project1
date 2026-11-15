#include <cstdio>
#include <ctime>

#include "dynamicarray.h"
#include "eventlog.h"
#include "bst.h"
#include "avltree.h"
#include "maxheap.h"

struct SortStats {
    long comparisons;
    long moves;

    SortStats() {
        comparisons = 0;
        moves = 0;
    }
};

unsigned int nextRandom(unsigned int& seed) {
    seed = seed * 1103515245u + 12345u;
    return (seed >> 16) & 0x7fff;
}

void makeValues(DynArray<int>& values, int count, int kind, unsigned int seed) {
    values.clear();
    for (int i = 0; i < count; i++) {
        if (kind == 0) {
            values.push(i + 1);
        } else if (kind == 1) {
            values.push(i + 1);
        } else if (kind == 2) {
            values.push(count - i);
        } else {
            values.push(i % 10);
        }
    }

    if (kind == 0) {
        for (int i = count - 1; i > 0; i--) {
            int j = (int)(nextRandom(seed) % (unsigned int)(i + 1));
            int temp = values[i];
            values[i] = values[j];
            values[j] = temp;
        }
    }
}

const char* inputName(int kind) {
    if (kind == 0) {
        return "random";
    }
    if (kind == 1) {
        return "ascending";
    }
    if (kind == 2) {
        return "reverse";
    }
    return "duplicates";
}

double elapsedSeconds(clock_t start, clock_t end) {
    return (double)(end - start) / (double)CLOCKS_PER_SEC;
}

int searchComparisons(TreeNode* root, int key) {
    int count = 0;
    TreeNode* current = root;
    while (current != 0) {
        count = count + 1;
        if (key == current->value) {
            return count;
        }
        if (key < current->value) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return count;
}

void printTreeHeights() {
    std::printf("\n1. HEIGHT COMPARISON\n");
    std::printf("Sorted BST input is capped at 5000 nodes to avoid excessive recursion.\n");
    std::printf("input,n,BST_height,AVL_height\n");

    int sizes[] = {100, 1000, 10000};
    for (int s = 0; s < 3; s++) {
        for (int kind = 0; kind <= 1; kind++) {
            int count = sizes[s];
            if (kind == 1 && count == 10000) {
                count = 5000;
            }

            DynArray<int> values;
            makeValues(values, count, kind, 20261115u);
            EventLog events;
            events.enabled = false;
            BST bst;
            AVLTree avl;
            for (int i = 0; i < count; i++) {
                bst.insert(values[i], events);
                avl.insert(values[i], events);
            }

            const char* name = kind == 0 ? "random" : "sorted";
            std::printf("%s,%d,%d,%d\n", name, count,
                        bst.computeHeight(bst.root), avl.computeHeight(avl.root));
        }
    }
}

void printSearchCosts() {
    std::printf("\n2. SEARCH COST (10000 searches per tree)\n");
    std::printf("input,n,BST_avg_comparisons,BST_seconds,AVL_avg_comparisons,AVL_seconds\n");

    int sizes[] = {100, 1000, 10000};
    const int queryCount = 10000;
    for (int s = 0; s < 3; s++) {
        for (int kind = 0; kind <= 1; kind++) {
            int count = sizes[s];
            if (kind == 1 && count == 10000) {
                count = 5000;
            }

            DynArray<int> values;
            makeValues(values, count, kind, 20261115u);
            EventLog events;
            events.enabled = false;
            BST bst;
            AVLTree avl;
            for (int i = 0; i < count; i++) {
                bst.insert(values[i], events);
                avl.insert(values[i], events);
            }

            DynArray<int> queries;
            unsigned int seed = 891011u;
            long long bstTotal = 0;
            long long avlTotal = 0;
            for (int i = 0; i < queryCount; i++) {
                int key = 1 + (int)(nextRandom(seed) % (unsigned int)count);
                queries.push(key);
                bstTotal += searchComparisons(bst.root, key);
                avlTotal += searchComparisons(avl.root, key);
            }

            clock_t start = clock();
            for (int i = 0; i < queryCount; i++) {
                bst.search(queries[i], events);
            }
            clock_t end = clock();
            double bstTime = elapsedSeconds(start, end);

            start = clock();
            for (int i = 0; i < queryCount; i++) {
                avl.search(queries[i], events);
            }
            end = clock();
            double avlTime = elapsedSeconds(start, end);

            std::printf("%s,%d,%.3f,%.6f,%.3f,%.6f\n",
                        kind == 0 ? "random" : "sorted", count,
                        (double)bstTotal / queryCount, bstTime,
                        (double)avlTotal / queryCount, avlTime);
        }
    }
}

void printRotationCounts() {
    std::printf("\n3. AVL ROTATION COUNTS (n=10000)\n");
    std::printf("input,insert_avg_rotations,delete_avg_rotations,max_delete_rotations\n");

    const int count = 10000;
    for (int kind = 0; kind <= 1; kind++) {
        DynArray<int> values;
        makeValues(values, count, kind, 20261115u);
        EventLog events;
        events.enabled = false;
        AVLTree tree;

        for (int i = 0; i < count; i++) {
            tree.insert(values[i], events);
        }
        int insertionRotations = tree.totalRotations;
        int rotationsBeforeDelete = tree.totalRotations;
        int maximumDeleteRotations = 0;

        for (int i = 0; i < count; i++) {
            int key = kind == 0 ? values[count - i - 1] : i + 1;
            tree.deleteKey(key, events);
            if (tree.lastRotations > maximumDeleteRotations) {
                maximumDeleteRotations = tree.lastRotations;
            }
        }

        int deletionRotations = tree.totalRotations - rotationsBeforeDelete;
        std::printf("%s,%.6f,%.6f,%d\n",
                    kind == 0 ? "random" : "sorted",
                    (double)insertionRotations / count,
                    (double)deletionRotations / count,
                    maximumDeleteRotations);
    }
}

void printOrderStatistics() {
    std::printf("\n4. ORDER STATISTICS (1000 queries on n=10000)\n");
    std::printf("structure,queries,seconds,checksum\n");

    const int count = 10000;
    const int queryCount = 1000;
    DynArray<int> values;
    makeValues(values, count, 0, 20261115u);

    EventLog events;
    events.enabled = false;
    BST bst;
    AVLTree avl;
    for (int i = 0; i < count; i++) {
        bst.insert(values[i], events);
        avl.insert(values[i], events);
    }

    int bstChecksum = 0;
    clock_t start = clock();
    for (int i = 0; i < queryCount; i++) {
        int k = 1 + (i * 7919) % count;
        TreeNode* node = bst.kthSmallest(k);
        if (node != 0) {
            bstChecksum += node->value;
        }
    }
    clock_t end = clock();
    std::printf("BST_inorder_scan,%d,%.6f,%d\n", queryCount,
                elapsedSeconds(start, end), bstChecksum);

    int avlChecksum = 0;
    start = clock();
    for (int i = 0; i < queryCount; i++) {
        int k = 1 + (i * 7919) % count;
        TreeNode* node = avl.kth(k, events);
        if (node != 0) {
            avlChecksum += node->value;
        }
    }
    end = clock();
    std::printf("AVL_subtree_sizes,%d,%.6f,%d\n", queryCount,
                elapsedSeconds(start, end), avlChecksum);
}

void printHeapBuild() {
    std::printf("\n5. HEAP CONSTRUCTION\n");
    std::printf("input,n,seconds,comparisons\n");

    int sizes[] = {100, 1000, 10000};
    for (int s = 0; s < 3; s++) {
        for (int kind = 0; kind <= 1; kind++) {
            DynArray<int> values;
            makeValues(values, sizes[s], kind, 1234567u);
            MaxHeap heap;
            heap.loadArray(values);
            heap.resetCounters();
            EventLog events;
            events.enabled = false;

            clock_t start = clock();
            heap.buildMaxHeap(events);
            clock_t end = clock();
            std::printf("%s,%d,%.6f,%d\n",
                        kind == 0 ? "random" : "ascending", sizes[s],
                        elapsedSeconds(start, end), heap.comparisons);
        }
    }
}

void printHeapSort() {
    std::printf("\n6. HEAP SORT\n");
    std::printf("input,n,seconds,comparisons,swaps,sorted\n");

    const int sizes[] = {1000, 10000};
    for (int s = 0; s < 2; s++) {
        for (int kind = 0; kind < 4; kind++) {
            DynArray<int> values;
            makeValues(values, sizes[s], kind, 7654321u);
            MaxHeap heap;
            heap.loadArray(values);
            heap.resetCounters();
            EventLog events;
            events.enabled = false;

            clock_t start = clock();
            heap.heapSort(events);
            clock_t end = clock();
            std::printf("%s,%d,%.6f,%d,%d,%s\n",
                        inputName(kind), sizes[s], elapsedSeconds(start, end),
                        heap.comparisons, heap.swaps,
                        heap.isSortedAscending() ? "yes" : "no");
        }
    }
}

void insertionSort(DynArray<int>& values, SortStats& stats) {
    for (int i = 1; i < values.size(); i++) {
        int key = values[i];
        int j = i - 1;
        while (j >= 0) {
            stats.comparisons++;
            if (values[j] > key) {
                values[j + 1] = values[j];
                stats.moves++;
                j--;
            } else {
                break;
            }
        }
        values[j + 1] = key;
        stats.moves++;
    }
}

void mergeSortRange(DynArray<int>& values, DynArray<int>& temporary,
                    int first, int last, SortStats& stats) {
    if (first >= last) {
        return;
    }

    int middle = first + (last - first) / 2;
    mergeSortRange(values, temporary, first, middle, stats);
    mergeSortRange(values, temporary, middle + 1, last, stats);

    int left = first;
    int right = middle + 1;
    int output = first;
    while (left <= middle && right <= last) {
        stats.comparisons++;
        if (values[left] <= values[right]) {
            temporary[output] = values[left];
            left++;
        } else {
            temporary[output] = values[right];
            right++;
        }
        output++;
        stats.moves++;
    }
    while (left <= middle) {
        temporary[output] = values[left];
        left++;
        output++;
        stats.moves++;
    }
    while (right <= last) {
        temporary[output] = values[right];
        right++;
        output++;
        stats.moves++;
    }
    for (int i = first; i <= last; i++) {
        values[i] = temporary[i];
        stats.moves++;
    }
}

void mergeSort(DynArray<int>& values, SortStats& stats) {
    DynArray<int> temporary;
    for (int i = 0; i < values.size(); i++) {
        temporary.push(0);
    }
    mergeSortRange(values, temporary, 0, values.size() - 1, stats);
}

void quickSortRange(DynArray<int>& values, int first, int last,
                    SortStats& stats) {
    int left = first;
    int right = last;
    int pivot = values[first + (last - first) / 2];

    while (left <= right) {
        while (left <= last) {
            stats.comparisons++;
            if (values[left] < pivot) {
                left++;
            } else {
                break;
            }
        }
        while (right >= first) {
            stats.comparisons++;
            if (values[right] > pivot) {
                right--;
            } else {
                break;
            }
        }
        if (left <= right) {
            if (left < right) {
                int temp = values[left];
                values[left] = values[right];
                values[right] = temp;
                stats.moves++;
            }
            left++;
            right--;
        }
    }

    if (first < right) {
        quickSortRange(values, first, right, stats);
    }
    if (left < last) {
        quickSortRange(values, left, last, stats);
    }
}

bool isSorted(DynArray<int>& values) {
    for (int i = 1; i < values.size(); i++) {
        if (values[i - 1] > values[i]) {
            return false;
        }
    }
    return true;
}

void printSortingComparison() {
    std::printf("\n7. SORTING COMPARISON (random input)\n");
    std::printf("algorithm,n,seconds,comparisons,moves_or_swaps,auxiliary_ints,sorted\n");

    int sizes[] = {1000, 10000};
    for (int s = 0; s < 2; s++) {
        DynArray<int> original;
        makeValues(original, sizes[s], 0, 998877u);

        DynArray<int> values;
        for (int i = 0; i < original.size(); i++) {
            values.push(original[i]);
        }
        SortStats stats;
        clock_t start = clock();
        insertionSort(values, stats);
        clock_t end = clock();
        std::printf("insertion,%d,%.6f,%ld,%ld,0,%s\n", sizes[s],
                    elapsedSeconds(start, end), stats.comparisons, stats.moves,
                    isSorted(values) ? "yes" : "no");

        values.clear();
        for (int i = 0; i < original.size(); i++) {
            values.push(original[i]);
        }
        stats = SortStats();
        start = clock();
        mergeSort(values, stats);
        end = clock();
        std::printf("merge,%d,%.6f,%ld,%ld,%d,%s\n", sizes[s],
                    elapsedSeconds(start, end), stats.comparisons, stats.moves,
                    sizes[s], isSorted(values) ? "yes" : "no");

        values.clear();
        for (int i = 0; i < original.size(); i++) {
            values.push(original[i]);
        }
        stats = SortStats();
        start = clock();
        quickSortRange(values, 0, values.size() - 1, stats);
        end = clock();
        std::printf("quick,%d,%.6f,%ld,%ld,0,%s\n", sizes[s],
                    elapsedSeconds(start, end), stats.comparisons, stats.moves,
                    isSorted(values) ? "yes" : "no");

        MaxHeap heap;
        heap.loadArray(original);
        heap.resetCounters();
        EventLog events;
        events.enabled = false;
        start = clock();
        heap.heapSort(events);
        end = clock();
        std::printf("heap,%d,%.6f,%d,%d,0,%s\n", sizes[s],
                    elapsedSeconds(start, end), heap.comparisons, heap.swaps,
                    heap.isSortedAscending() ? "yes" : "no");
    }
}

int main() {
    std::printf("Vault Heist Section 10 Benchmarks\n");
    printTreeHeights();
    printSearchCosts();
    printRotationCounts();
    printOrderStatistics();
    printHeapBuild();
    printHeapSort();
    printSortingComparison();
    return 0;
}
