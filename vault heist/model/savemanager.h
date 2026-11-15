// model/SaveManager.h  -- must NOT include raylib
// Turns trees and heaps into text (and back). Text lives in a DynArray<char>
// that ends with '\0'. Writing it to a file is the GUI's job.
#pragma once
#include "binarytree.h"
#include "maxheap.h"

class SaveManager {
public:
    // ---------- small text helpers ----------

    void appendText(DynArray<char>& out, const char* text) {
        int i = 0;
        while (text[i] != '\0') {
            out.push(text[i]);
            i++;
        }
    }

    void appendInt(DynArray<char>& out, int value) {
        if (value == 0) {
            out.push('0');
            return;
        }
        if (value < 0) {
            out.push('-');
            value = -value;
        }
        char digits[12];
        int n = 0;
        while (value > 0) {
            digits[n] = (char)('0' + value % 10);
            n++;
            value = value / 10;
        }
        for (int i = n - 1; i >= 0; i--) {
            out.push(digits[i]);
        }
    }

    // Removes a trailing space (if any) and adds the '\0' end marker.
    void finish(DynArray<char>& out) {
        if (out.size() > 0 && out[out.size() - 1] == ' ') {
            out.pop();
        }
        out.push('\0');
    }

    void skipSpaces(DynArray<char>& text, int& pos) {
        while (pos < text.size() && (text[pos] == ' ' || text[pos] == '\n' || text[pos] == '\r' || text[pos] == '\t')) {
            pos++;
        }
    }

    bool atEnd(DynArray<char>& text, int pos) {
        return pos >= text.size() || text[pos] == '\0';
    }

    // Reads one whole number (with optional minus). Returns false if there isn't one.
    bool readInt(DynArray<char>& text, int& pos, int& value) {
        skipSpaces(text, pos);
        if (atEnd(text, pos)) {
            return false;
        }
        bool negative = false;
        if (text[pos] == '-') {
            negative = true;
            pos++;
        }
        if (atEnd(text, pos) || text[pos] < '0' || text[pos] > '9') {
            return false;
        }
        int result = 0;
        while (atEnd(text, pos) == false && text[pos] >= '0' && text[pos] <= '9') {
            result = result * 10 + (text[pos] - '0');
            pos++;
        }
        if (negative) {
            result = -result;
        }
        value = result;
        return true;
    }

    // ---------- trees ----------

    void saveNode(TreeNode* node, DynArray<char>& out) {
        if (node == 0) {
            appendText(out, "# ");
            return;
        }
        appendInt(out, node->value);
        out.push(' ');
        saveNode(node->left, out);
        saveNode(node->right, out);
    }

    // Preorder with '#' for empty spots, e.g. "50 30 # # 70 60 # # #".
    void saveTree(BinaryTree& t, DynArray<char>& out) {
        out.clear();
        saveNode(t.root, out);
        finish(out);
    }

    TreeNode* parseNode(BinaryTree& t, DynArray<char>& text, int& pos, bool& ok) {
        if (ok == false) {
            return 0;
        }
        skipSpaces(text, pos);
        if (atEnd(text, pos)) {
            ok = false;
            return 0;
        }
        if (text[pos] == '#') {
            pos++;
            return 0;
        }
        int value = 0;
        if (readInt(text, pos, value) == false) {
            ok = false;
            return 0;
        }
        TreeNode* node = t.createNode(value);
        TreeNode* leftPart = parseNode(t, text, pos, ok);
        TreeNode* rightPart = parseNode(t, text, pos, ok);
        t.setLeft(node, leftPart);
        t.setRight(node, rightPart);
        return node;
    }

    // Replaces the tree with the one in 'text'. Returns false (and leaves the tree
    // empty) if the text is damaged. Heights/sizes are NOT restored:
    // for an AVLTree call recomputeAll(t.root) afterwards.
    bool loadTree(BinaryTree& t, DynArray<char>& text) {
        t.clear();
        int pos = 0;
        bool ok = true;
        t.root = parseNode(t, text, pos, ok);
        skipSpaces(text, pos);
        if (ok && atEnd(text, pos) == false) {
            ok = false;                    // leftover text after a complete tree
        }
        if (ok == false) {
            t.clear();
        }
        return ok;
    }

    // ---------- heaps ----------

    // Format: "heapSize arrayLength v1 v2 v3 ..."
    void saveHeap(MaxHeap& h, DynArray<char>& out) {
        out.clear();
        appendInt(out, h.heapSize);
        out.push(' ');
        appendInt(out, h.data.size());
        for (int i = 0; i < h.data.size(); i++) {
            out.push(' ');
            appendInt(out, h.data[i]);
        }
        finish(out);
    }

    bool loadHeap(MaxHeap& h, DynArray<char>& text) {
        int pos = 0;
        int activeSize = 0;
        int length = 0;
        if (readInt(text, pos, activeSize) == false) {
            return false;
        }
        if (readInt(text, pos, length) == false) {
            return false;
        }
        if (length < 0 || activeSize < 0 || activeSize > length) {
            return false;
        }
        DynArray<int> values;
        for (int i = 0; i < length; i++) {
            int v = 0;
            if (readInt(text, pos, v) == false) {
                return false;
            }
            values.push(v);
        }
        skipSpaces(text, pos);
        if (atEnd(text, pos) == false) {
            return false;
        }
        h.loadArray(values);
        h.heapSize = activeSize;
        return true;
    }
};