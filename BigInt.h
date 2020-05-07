//
// Created by cuom1999 on 4/28/20.
//

#ifndef CS3353_FINAL_BIGINT_H
#define CS3353_FINAL_BIGINT_H

#include <vector>
#include <string>
#include <iostream>

// All calculations are done in base 10**9 instead of base 10
class BigInt {
public:
    // ----------------- Constructors And Assignment -------------
    BigInt(): sign(1){}

    BigInt(long long num);
    BigInt& operator= (long long num);

    friend void swap(BigInt& a, BigInt& b); // Copy and swap idiom
    BigInt(const BigInt&);
    BigInt& operator= (BigInt);
    BigInt(BigInt&&);

    BigInt(const std::string&); // from string

    // ---------------- Input and Output --------------------------
    friend std::istream& operator >> (std::istream &stream, BigInt& a);
    friend std::ostream& operator << (std::ostream &stream, const BigInt& a);

    // ---------------- Comparison --------------------------------
    bool operator < (const BigInt& a) const;
    bool operator > (const BigInt& a) const;
    bool operator <= (const BigInt& a) const;
    bool operator >= (const BigInt& a) const;
    bool operator == (const BigInt& a) const;
    bool operator != (const BigInt& a) const;

    // ---------------- Unary operator - and + --------------------
    BigInt operator -() const;
    BigInt& operator += (const BigInt& a);
    BigInt& operator -= (const BigInt& a);
    BigInt operator + (const BigInt& a);
    BigInt operator - (const BigInt& a);


    // ---------------- operator *, /, % ---------------------------
    BigInt mulBrute(const BigInt &num) const;
    BigInt mulFFT(const BigInt &num) const;
    BigInt operator * (const BigInt& a) const;
    BigInt operator / (const BigInt& a) const;
    BigInt operator % (const BigInt& a) const;
    BigInt& operator *= (const BigInt& a);
    BigInt& operator /= (const BigInt& a);
    BigInt& operator %= (const BigInt& a);

    // -------------------- operations with int ---------------
    BigInt& operator/=(int v);
    BigInt operator/(int v) const;

    // ---------------- others --------------------
    BigInt abs() const;

private:
    static const int BASE_DIGITS = 9;
    static const int BASE = 1000000000;

    int sign;

    // digits contain numbers < BASE from small to big degrees
    // example if a number = 10..0123 (1e17 + 123),
    // digits = {123, 100000000}
    std::vector<int> digits;
    // ---------------- helper functions for calculations -------------------

    // 0 if |a| == |b|, -1 if |a| < |b|, 1 if |a| > |b|
    friend int _compareAbs(const BigInt& a, const BigInt& b);
    void _internalAdd(const BigInt& a); // *this and a has same sign
    void _internalSub(const BigInt& a); // *this and a has same sign, |*this| >= |a|

    // return (a / b, a % b)
    friend std::pair<BigInt, BigInt> divmod(const BigInt& a, const BigInt& b); // return div and mod


    // -------------------- utilities ---------------------------------

    // convert base from 10**oldBase -> 10**newBase, use for FFT due to its float precision error
    std::vector<int> convertBase(const std::vector<int> &digits, int oldBase, int newBase) const;
    void trim();
    bool isZero() const;

};



#endif //CS3353_FINAL_BIGINT_H
