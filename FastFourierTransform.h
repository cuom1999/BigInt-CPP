//
// Created by cuom1999 on 4/29/20.
//

#ifndef CS3353_FINAL_FASTFOURIERTRANSFORM_H
#define CS3353_FINAL_FASTFOURIERTRANSFORM_H

#include <vector>
#include <complex>

namespace FFT {
    // only works with n = power of 2
    // if invert = false, FFT(a) -> a
    // else IFFT(a) -> a
    // implement pseudocode in the book "Introduction to Algorithm"
    // this implement is faster then the recursive one, although it has the same complexity
    // because it does not call recursions, which may be very slow
    void fft(std::vector<std::complex<double>> &a, bool invert);

    // after calling this, res = polyA * polyB
    // to avoid float precision, only use it with integers <= ~10000
    std::vector<long long> polyMul(const std::vector<int>& polyA, const std::vector<int>& polyB);
};


#endif //CS3353_FINAL_FASTFOURIERTRANSFORM_H
