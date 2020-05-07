//
// Created by cuom1999 on 4/28/20.
//

#include "BigInt.h"
#include "FastFourierTransform.h"
#include <algorithm>
#include <cassert>
#include <iomanip>
using namespace std;


BigInt::BigInt(long long a) {
    *this = a;
}

BigInt& BigInt::operator= (long long num) {
    sign = 1;
    if (num < 0) {
        sign = -1;
        num = -num;
    }
    digits.clear();
    while (num > 0) {
        digits.push_back(num % BASE);
        num /= BASE;
    }
    return *this;
}

BigInt::BigInt(const BigInt& other) {
    sign = other.sign;
    digits = other.digits;
}

void swap(BigInt& a, BigInt& b) {
    std::swap(a.sign, b.sign); // just to make it clear
    std::swap(a.digits, b.digits);
}

BigInt& BigInt::operator=(BigInt other) {
    swap(*this, other);
    return *this;
}

BigInt::BigInt(BigInt&& other) : BigInt() {
    swap(*this, other);
}

BigInt::BigInt(const std::string &s) {
    assert(s.size() > 0);
    sign = 1;
    digits.clear();

    int pos = 0;

    // handle +, - sign in front
    // ++5 = 5, ++-4 = -4
    while (pos < (int) s.size() && (s[pos] == '-' || s[pos] == '+')) {
        if (s[pos] == '-') {
            sign = -sign;
        }
        pos++;
    }

    // iterate from back to front, every iteration gets BASE_DIGITS
    // consecutive digits
    for (int i = (int) s.size() - 1; i >= pos; i -= BASE_DIGITS) {
        int curDigit = 0; // base 10**9

        // calculate curDigit
        for (int j = max(pos, i - BASE_DIGITS + 1); j <= i; j++) {
            assert(s[j] >= '0' && s[j] <= '9');
            curDigit = curDigit * 10 + (s[j] - '0');
        }

        digits.push_back(curDigit);
    }
    trim();
}

istream& operator>> (istream &inp, BigInt& a) {
    string str;
    inp >> str;
    a = BigInt(str);
    return inp;
}

ostream& operator<< (ostream &out, const BigInt& num) {
    if (num.sign == -1 && !num.isZero()) {
        out << '-';
    }

    // print leftmost digits
    if (num.digits.size() == 0) {
        out << '0';
    }
    else {
        out << num.digits.back();
    }

    // print the rest
    for (int i = (int) num.digits.size() - 2; i >= 0; i--) {
        // make sure if digits[i] = 123, it prints 000000123
        out << setw(num.BASE_DIGITS) << setfill('0')
            << num.digits[i];
    }
    return out;
}

// remove trailing zeros
void BigInt::trim() {
    while (!digits.empty() && !digits.back()) {
        digits.pop_back();
    }

    if (digits.empty()) { // = 0
        sign = 1;
    }
}

bool BigInt::isZero() const {
    return digits.empty() || (digits.size() == 1 && !digits[0]);
}

bool BigInt::operator < (const BigInt& num) const {
    if (sign != num.sign) {
        return sign < num.sign;
    }
    if (digits.size() != num.digits.size()) {
        return digits.size() * sign < num.digits.size() * num.sign;
    }
    // the last case: iterate from high orders to small
    for (int i = (int) digits.size() - 1; i >= 0; i--) {
        if (digits[i] != num.digits[i]) {
            return digits[i] * sign < num.digits[i] * num.sign;
        }
    }

    return false; // equal
}

bool BigInt::operator > (const BigInt& num) const {
    return num < *this;
}

bool BigInt::operator <= (const BigInt& num) const {
    return !(num < *this);
}

bool BigInt::operator >= (const BigInt &num) const {
    return !(*this < num);
}

bool BigInt::operator == (const BigInt &num) const {
    return !(*this < num) && !(num < *this);
}

bool BigInt::operator != (const BigInt& num) const {
    return (*this < num) || (num < *this);
}

BigInt BigInt::operator -() const {
    BigInt res = *this;
    if (isZero()) return res;
    res.sign = -sign;
    return res;
}

// must have same sign
// use primary school rule: carry and other stuff
void BigInt::_internalAdd(const BigInt &num) {
    // fill more zeros if necessary
    if (digits.size() < num.digits.size()) {
        digits.resize(num.digits.size(), 0);
    }
    int carry = 0;
    int numDigits = max(digits.size(), num.digits.size());

    // iterate until carry = 0 and i >= numDigits
    for (int i = 0; i < (int) numDigits || carry > 0; i++) {
        if (i == (int) digits.size()) { // have one more digit than numDigits
            digits.push_back(0);
        }
        if (i < (int) num.digits.size()) digits[i] += num.digits[i];

        digits[i] += carry;

        carry = (digits[i] >= BASE); // 0 or 1
        if (carry) {
            digits[i] -= BASE;
        }
    }
}

// must have same sign and |*this| >= |num|
void BigInt::_internalSub(const BigInt &num) {
    int carry = 0;
    for (int i = 0; i < (int) digits.size() || carry > 0; i++) {
        if (i < (int) num.digits.size()) {
            digits[i] -= num.digits[i];
        }

        digits[i] -= carry;

        carry = (digits[i] < 0); // 0 or 1
        if (carry) digits[i] += BASE;
    }
    trim();
}

// ignore the signs
int _compareAbs(const BigInt& a, const BigInt& b) {
    if (a.digits.size() != b.digits.size()) {
        if (a.digits.size() < b.digits.size()) {
            return -1;
        }
        return 1;
    }

    // compare from high degree to small
    for (int i = (int) a.digits.size() - 1; i >= 0; i--) {
        if (a.digits[i] < b.digits[i]) return -1;
        if (a.digits[i] > b.digits[i]) return 1;
    }
    return 0;
}

// if having same sign, use _internalAdd
// otherwise use _internalSub
BigInt& BigInt::operator += (const BigInt& num) {
    if (sign == num.sign) {
        _internalAdd(num);
    }
    else {
        if (_compareAbs(*this, num) >= 0) { // |*this| >= |num|
            _internalSub(num);
        }
        else { // |*this| < |num|
            BigInt tmp = num;
            swap(*this, tmp);
            _internalSub(tmp);
        }
    }
    return *this;
}

// if having same sign, use _internalSub
// else use _internalAdd
BigInt& BigInt::operator -= (const BigInt &num) {
    if (sign == num.sign) {
        if (_compareAbs(*this, num) >= 0) {
            _internalSub(num);
        }
        else {
            BigInt tmp = num;
            swap(*this, tmp);
            _internalSub(tmp);
            sign = -sign;
        }
    }
    else {
        _internalAdd(num);
    }
    return *this;
}

BigInt BigInt::operator + (const BigInt &a) {
    BigInt res = *this;
    res += a;
    return res;
}

BigInt BigInt::operator - (const BigInt &a) {
    BigInt res = *this;
    res -= a;
    return res;
}

BigInt BigInt::abs() const {
    BigInt res = *this;
    res.sign = 1;
    return res;
}

// return pair (a / b, a % b)
// just do brute force division like what we learned in primary school
// could be done better in O(nlogn) with FFT, but kinda hard to implement
// and I'm lazy ...
pair<BigInt, BigInt> divmod(const BigInt& a, const BigInt& b) {
    assert(b > 0); // only defined when b > 0
    const int BASE = a.BASE;

    long long norm = BASE / (b.digits.back() + 1);

    BigInt divident = a.abs() * norm;
    BigInt divisor = b.abs() * norm;
    BigInt quotient = 0, remainder = 0;
    quotient.digits.resize(divident.digits.size());

    for (int i = (int) divident.digits.size() - 1; i >= 0; i--) {
        remainder *= BASE;
        remainder += divident.digits[i];
        long long lastCoef1 = 0, lastCoef2 = 0;

        int divisorNumDigits = divisor.digits.size();

        if (remainder.digits.size() > divisorNumDigits) {
            lastCoef1 = remainder.digits[divisorNumDigits];
        }
        if (remainder.digits.size() > divisorNumDigits - 1) {
            lastCoef2 = remainder.digits[divisorNumDigits - 1];
        }

        long long curQuotientDigit = ((long long) BASE * lastCoef1 + lastCoef2)
                                    / divisor.digits.back();
        remainder -= divisor * curQuotientDigit ;
        while (remainder < 0) {
            remainder += divisor;
            curQuotientDigit--;
        }
        quotient.digits[i] = curQuotientDigit;
    }

    quotient.sign = divisor.sign * divident.sign;
    remainder.sign = divident.sign;
    quotient.trim();
    remainder.trim();
    return {quotient, remainder / norm};
}

BigInt BigInt::operator/ (const BigInt &v) const {
    return divmod(*this, v).first;
}

BigInt& BigInt::operator /= (const BigInt &a) {
    *this = (*this) / a;
    return *this;
}

BigInt& BigInt::operator/=(int v) {
    assert(v > 0);  // operator / not well-defined for v <= 0.
    if (llabs(v) >= BASE) {
        *this /= BigInt(v);
        return *this;
    }
    if (v < 0)
        sign = -sign, v = -v;
    for (int i = (int) digits.size() - 1, rem = 0; i >= 0; --i) {
        long long cur = digits[i] + rem * (long long) BASE;
        digits[i] = (int) (cur / v);
        rem = (int) (cur % v);
    }
    trim();
    return *this;
}

BigInt BigInt::operator/(int v) const {
    assert(v > 0);  // operator / not well-defined for v <= 0.
    if (llabs(v) >= BASE) {
        return *this / BigInt(v);
    }
    BigInt res = *this;
    res /= v;
    return res;
}

BigInt BigInt::operator% (const BigInt &v) const {
    return divmod(*this, v).second;
}

BigInt& BigInt::operator %= (const BigInt &v) {
    (*this) = (*this) % v;
    return *this;
}

// O(mn) algorithm
// (a0 + a1 * BASE + a2 * BASE^2 + ...)(b0 + b1 * BASE + ...)
// do polynomial multiplication first, then trim result
BigInt BigInt::mulBrute(const BigInt& num) const {
    BigInt res;
    res.sign = sign * num.sign;
    res.digits.resize(digits.size() + num.digits.size());

    // polynomial multiplication
    // x^k = sum of all a_i * x^(k-i) * b_(k - i) * x^i
    for (int i = 0; i < (int) digits.size(); i++) {
        if (!digits[i]) continue; // ignore 0
        int carry = 0;

        for (int j = 0; j < (int) num.digits.size(); j++) {
            long long cur = res.digits[i + j] + (long long) digits[i] * num.digits[j] + carry;
            carry = (int) (cur / BASE);
            res.digits[i + j] = (int) (cur % BASE);
        }

        if (carry > 0) { // need to add carry to one more digit
            res.digits[(int) num.digits.size() + i] += carry;
        }
    }
    res.trim();
    return res;
}

// first convert base to 1e3 due to possible float precision errors of FFT algorithm
BigInt BigInt::mulFFT(const BigInt &num) const {
    BigInt res;
    res.sign = sign * num.sign;

    vector<long long> product = FFT::polyMul(convertBase(digits, BASE_DIGITS, 3),
                                             convertBase(num.digits, BASE_DIGITS, 3));

    // construct res.digits in base 1000
    // always more than enough space for res.digits
    int CUR_BASE = 1000;
    for (int i = 0; i < product.size(); i++) {
        int carry = product[i] / CUR_BASE;
        if (carry && i == (int) product.size()) {
            product.push_back(0);
        }
        if (i + 1 < product.size()) {
            product[i + 1] += carry; // move carry to the next digit
        }

        res.digits.push_back(product[i] % CUR_BASE);
    }

    // transform back to base 1e9
    res.digits = convertBase(res.digits, 3, BASE_DIGITS);
    res.trim();
    return res;
}

vector<int> BigInt::convertBase(const std::vector<int> &digits, int oldBase, int newBase) const {
    vector<long long> power(18); // power[i] = 10**i
    // calculate power
    power[0] = 1;
    for (int i = 1; i <= 18; i++) {
        power[i] = power[i - 1] * 10;
    }

    // construct result
    vector<int> res;
    long long curDigit = 0;
    int curNumDigits = 0;
    for (int i = 0; i < (int) digits.size(); i++) {
        curDigit += digits[i] * power[curNumDigits];
        curNumDigits += oldBase;

        // if curNumDigits >= newBase, trim curDigit and update curNumDigits
        while (curNumDigits >= newBase) {
            res.push_back((long long) curDigit % power[newBase]);
            curDigit /= power[newBase];
            curNumDigits -= newBase;
        }
    }

    res.push_back((int) curDigit); // add the last
    while (!res.empty() && !res.back()) res.pop_back(); // remove trailing 0s

    return res;
}


// FFT is fast only when both *this and num are big, because its complexity is O(KlogK)
// with K is the smallest power of 2 >= digits.size and num.digits.size
// If one of them is small, better use brute force
BigInt BigInt::operator* (const BigInt& num) const {
    if (min(digits.size(), num.digits.size()) <= 100) {
        return mulBrute(num);
    }
    return mulFFT(num);
}

BigInt& BigInt::operator*= (const BigInt& num) {
    *this = (*this) * num;
    return *this;
}




