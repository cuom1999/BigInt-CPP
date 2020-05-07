//
// Created by cuom1999 on 4/29/20.
//

#include "FastFourierTransform.h"
using namespace std;


void FFT::fft(std::vector<std::complex<double> > &poly, bool invert) {
    const double PI = 3.14159265358979323846;
    int n = (int) poly.size();

    // preprocess to avoid the recursion calls in our divide and conquer approach
    // change the order of coefficients to the order they will be processed in DnC
    for (int i = 1, j = 0; i < n; ++i) {
        int bit = n >> 1;
        for (; j >= bit; bit >>= 1)
            j -= bit;
        j += bit;
        if (i < j)
            swap(poly[i], poly[j]);
    }

    // Divide and Conquer without recursion, according to "Introduction to Algorithm"
    for (int len = 2; len <= n; len *= 2) {
        double ang = 2 * PI / len * (invert ? -1 : 1);
        complex<double> wlen(cos(ang), sin(ang)); // roots of equation: x^n - 1 = 0
        for (int i = 0; i < n; i += len) {
            complex<double> w(1);
            for (int j = 0; j < len / 2; ++j) {
                complex<double> u = poly[i + j];
                complex<double> v = poly[i + j + len / 2] * w;
                poly[i + j] = u + v;
                poly[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
    }

    // for IFFT only
    if (invert) {
        for (int i = 0; i < n; i++) {
            poly[i] /= n;
        }
    }
}

// Let n be a power of 2 such that n > deg(polyA) + deg(polyB)
// Fill more zeros to make polyA and polyB both of degree n
// FFT(res)[x] = FFT(polyA)[x] * FFT(polyB)[x] for all coefficients
// So first transform A, B. Calculate FFT(res(x)) with all roots of x^n=1, then IFFT(res) -> res
vector<long long> FFT::polyMul(const std::vector<int> &polyA, const std::vector<int> &polyB) {
    vector<long long> res;
    vector<complex<double>> fftA(polyA.begin(), polyA.end()); // clone
    vector<complex<double>> fftB(polyB.begin(), polyB.end());

    // calculate n we talked above
    int n = 1;
    while (n < (int) polyA.size() + polyB.size()) {
        n *= 2;
    }
    n *= 2;

    // fill more zeros
    fftA.resize(n);
    fftB.resize(n);

    // transform fftA = FFT(polyA), fftB = FFT(polyB)
    fft(fftA, false);
    fft(fftB, false);

    // calculate FFT(res), we use fftA to save memory
    // after this, fftA = FFT(res)
    for (int i = 0; i < n; i++) {
        fftA[i] *= fftB[i];
    }

    // inverse transform
    fft(fftA, true);

    // calculate result
    res.resize(n);
    for (int i = 0; i < n; i++) {
        res[i] = (long long) (fftA[i].real() + 0.5); // round the the nearest integer
    }

    return res;
}
