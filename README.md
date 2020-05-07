# CS3353-Final
# Phuoc Dinh Le
# ID: 47601532
This is implementation of a big integer class called "BigInt". It supports all basic operators (+, -, *, /, %, etc) for both positive and negative integers. The goal is to fill the lack of big integer calculations in C++.

The focus of this implementation is the multiplication operator. I implemented 2 different methods: brute force one, and fast one using Fast Fourier Transform. You can see the benchmask result by running "main.cpp". The testcases are contained in "input.txt". Running program will return two files: "output.txt" containing results of multiplications and "time.csv" containing time elapses.

To use the BigInt class, just simply use it like `int`. For example, if you want to add two numbers, you can do:

``` c++
BigInt a, b;
cin >> a >> b;
cout << a + b << endl;
```

Now you don't have to worry about overflow anymore :)
