#include <iostream>
#include <fstream>
#include <cassert>
#include <chrono>
#include "BigInt.h"
using namespace std;

void generateTest() {
    ofstream inp("input.txt");
    inp << 20 << endl;
    for (int i = 1; i <= 20; i++) {
        int numDigit = (i - 1) * 10000 + 3;
        for (int j = 1; j <= numDigit; j++) {
            inp << (char) (rand() % 10 + '0');
        }
        inp << " ";
        for (int j = 1; j <= numDigit; j++) {
            inp << (char) (rand() % 10 + '0');
        }
        inp << endl;
    }
}

// get data from input.txt, print to output.txt
// input.txt is as the form:
// the first line contains one number: numTests
// each of next numTests lines contains 2 integers
// for each test, we will print out the product of those integers on 1 line
void test() {
    using namespace std::chrono;

    ifstream inp("input.txt");
    ofstream out("output.txt");
    ofstream timeResult("time.csv");
    if (!inp.is_open()) {
        cout << "Cannot open file" << endl;
        return;
    }

    int numTest;
    inp >> numTest;
    for (int i = 1; i <= numTest; i++) {
        BigInt a, b;
        inp >> a >> b;

        // calculate and measure time
        auto start = high_resolution_clock::now();
        BigInt prodBrute = a.mulBrute(b);
        auto stop = high_resolution_clock::now();
        auto durationBrute = duration_cast<milliseconds>(stop - start);

        start = high_resolution_clock::now();
        BigInt prodFFT = a.mulFFT(b);
        stop = high_resolution_clock::now();
        auto durationFFT = duration_cast<milliseconds>(stop - start);

        assert(prodBrute == prodFFT);

        out << prodBrute << endl;
        timeResult << durationBrute.count() << "," << durationFFT.count() << endl;
    }
}

int main() {
//    generateTest();
    test();
    return 0;
}
