#include <iostream>
#include <cstdlib>
#include <omp.h>
#include <vector>
#include <thread>

using namespace std;



void task(int* array1, int* array2, int* array3, int n, int i, int j) {
    auto start = chrono::high_resolution_clock::now();
    cout << "array1\t" << "array2\t" << "array3\t" << "\n";
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (i + j < n)
                array3[i] += array1[j] * array2[j + i];

        }
        cout << array1[i] << "\t" << array2[i] << "\t" << array3[i] << "\n";
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<float> duration = end - start;
    cout << "Duration " << duration.count() << "\n\n";
}

int main() {
    cout << "Enter size" << "\n";
    int n;
    cin >> n;

    int* array1 = new int[n];
    int* array2 = new int[n];
    int* array3 = new int[n];
    for (int i = 0; i < n; i++)
    {
        array1[i] = rand() % 10 + 1;
        array2[i] = rand() % 10 + 1;
        array3[i] = 0;
    }
    int i = 0, j = 0;

    cout << "No omp\n";
    task(array1, array2, array3, n, i, j);

    cout << "Omp\n";
#pragma omp parallel for private(i, j, n)
    task(array1, array2, array3, n, i, j);



}