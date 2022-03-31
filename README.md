# 1C_TASK
This is a solution I found for the task given me for admission's selection to "1C" Department. It's code is 212, 

In short, firstly I constructed the Laplacian of the image given to me (the file name is hardcoded, edit the file to change it), and then solved the problem of finding the submatrix of the maximum size belonging to the binary matrix. The proceedings were described in code in greater details.

To run this code, you need to set OpenCV library. It can be compiled with the command "g++ test.cpp -o test `pkg-config --cflags --libs opencv`" in linux system and launched with the command ./test. It's important: you need to have a file you working with at the same directory as where your code lays.
