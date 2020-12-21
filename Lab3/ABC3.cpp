#include <iostream>
#include <cstdlib>
#include <omp.h>
#include <time.h>
#include <vector>

using namespace std;

int main() {
    cout << "Enter size" << "\n";
    int n;
    cin >> n;

	int *array1 = new int[n];
	int *array2 = new int[n];
    int *array3 = new int[n];
    for (int i = 0; i < n; i++)
    {
        array1[i] = rand() % 10 + 1;
        array2[i] = rand() % 10 + 1;
        array3[i] = 0;
    }
    cout << "array1\t" << "array2\t" << "array3\t" << "\n";
    int i, j;
    #pragma omp parallel for private(i, j, n)
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                if(i + j < n)
                array3[i] += array1[j] * array2[j + i];

            }
            cout << array1[i] << "\t" << array2[i] << "\t" << array3[i] << "\n";
        }

}