#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <random>
#include <vector>
using namespace std;
struct Stats {
    unsigned long long comparisons = 0;
    unsigned long long swaps = 0;
};

void selectionSort(vector<double>& a, Stats& s) {
    size_t n = a.size();
    for (size_t i = 0; i + 1 < n; ++i) {
        size_t minIndex = i;
        for (size_t j = i + 1; j < n; ++j) {
            ++s.comparisons;
            if (a[j] < a[minIndex])
                minIndex = j;
        }
        if (minIndex != i) {
            swap(a[i], a[minIndex]);
            ++s.swaps;
        }
    }
}
void mergeRange(vector<double>& a, vector<double>& temp,
                size_t l, size_t m, size_t r, Stats& s) {
    size_t i = l, j = m + 1, k = l;
    while (i <= m && j <= r) {
        ++s.comparisons;
        if (a[i] <= a[j]) temp[k++] = a[i++];
        else temp[k++] = a[j++];
    }
    while (i <= m) temp[k++] = a[i++];
    while (j <= r) temp[k++] = a[j++];
    for (size_t p = l; p <= r; ++p) a[p] = temp[p];
}
void mergeSortRec(vector<double>& a, vector<double>& temp,
                  size_t l, size_t r, Stats& s) {
    if (l >= r) return;
    size_t m = l + (r - l) / 2;
    mergeSortRec(a, temp, l, m, s);
    mergeSortRec(a, temp, m + 1, r, s);
    mergeRange(a, temp, l, m, r, s);
}
void mergeSort(vector<double>& a, Stats& s) {
    if (a.empty()) return;
    vector<double> temp(a.size());
    mergeSortRec(a, temp, 0, a.size() - 1, s);
}
void insertionSort(vector<double>& a, size_t l, size_t r, Stats& s) {
    for (size_t i = l + 1; i <= r; ++i) {
        double key = a[i];
        size_t j = i;
        while (j > l) {
            ++s.comparisons;
            if (a[j - 1] <= key) break;
            a[j] = a[j - 1];
            --j;
            ++s.swaps; // counted here as data movements
        }
        a[j] = key;
    }
}
void hybridMergeSortRec(vector<double>& a, vector<double>& temp,
                        size_t l, size_t r, Stats& s, size_t threshold = 16) {
    if (r - l + 1 <= threshold) {
        insertionSort(a, l, r, s);
        return;
    }
    size_t m = l + (r - l) / 2;
    hybridMergeSortRec(a, temp, l, m, s, threshold);
    hybridMergeSortRec(a, temp, m + 1, r, s, threshold);
    mergeRange(a, temp, l, m, r, s);
}

void hybridSort(vector<double>& a, Stats& s) {
    if (a.empty()) return;
    vector<double> temp(a.size());
    hybridMergeSortRec(a, temp, 0, a.size() - 1, s, 16);
}
int sequentialSearch(const vector<double>& a, double key, Stats& s) {
    for (size_t i = 0; i < a.size(); ++i) {
        ++s.comparisons;
        if (a[i] == key) return static_cast<int>(i);
    }
    return -1;
}
int binarySearch(const vector<double>& a, double key, Stats& s) {
    int l = 0, r = static_cast<int>(a.size()) - 1;
    while (l <= r) {
        int m = l + (r - l) / 2;
        ++s.comparisons;
        if (a[m] == key) return m;
        ++s.comparisons;
        if (a[m] < key) l = m + 1;
        else r = m - 1;
    }
    return -1;
}
vector<double> makeData(size_t n) {
    mt19937_64 gen(123456);
    uniform_real_distribution<double> dist(0.0, 1000000.0);
    vector<double> a(n);
    for (double& x : a) x = dist(gen);
    return a;
}

int main() {
    vector<size_t> sizes = {1000, 10000, 100000, 1000000};
    for (size_t n : sizes) {
        vector<double> base = makeData(n);
        // Selection Sort: run only when practical; otherwise report infeasible
        if (n <= 100000) {
            auto a = base;
            Stats s;
            auto t1 = chrono::steady_clock::now();
            selectionSort(a, s);
            auto t2 = chrono::steady_clock::now();
            double ms = chrono::duration<double, milli>(t2 - t1).count();
            cout << "Selection," << n << "," << ms << ","
                 << s.comparisons << "," << s.swaps << "\n";
        } else {
            cout << "Selection," << n << ",INFEASIBLE,"
                 << (static_cast<unsigned long long>(n) *
                     (n - 1) / 2) << ",N/A\n";
        }
        auto a1 = base;
        Stats ms;
        auto t1 = chrono::steady_clock::now();
        mergeSort(a1, ms);
        auto t2 = chrono::steady_clock::now();
        cout << "Merge," << n << ","
             << chrono::duration<double, milli>(t2 - t1).count()
             << "," << ms.comparisons << "," << ms.swaps << "\n";
        auto a2 = base;
        Stats hs;
        t1 = chrono::steady_clock::now();
        hybridSort(a2, hs);
        t2 = chrono::steady_clock::now();
        cout << "Hybrid," << n << ","
             << chrono::duration<double, milli>(t2 - t1).count()
             << "," << hs.comparisons << "," << hs.swaps << "\n";
        // Search benchmark: use sorted array
        double key = a1[n / 2];
        Stats ss;
        t1 = chrono::steady_clock::now();
        sequentialSearch(base, key, ss);
        t2 = chrono::steady_clock::now();
        cout << "Sequential," << n << ","
             << chrono::duration<double, milli>(t2 - t1).count()
             << "," << ss.comparisons << "\n";
        Stats bs;
        t1 = chrono::steady_clock::now();
        binarySearch(a1, key, bs);
        t2 = chrono::steady_clock::now();
        cout << "Binary," << n << ","
             << chrono::duration<double, milli>(t2 - t1).count()
             << "," << bs.comparisons << "\n";
    }
}
