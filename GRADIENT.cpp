#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <stack>
#include <map>

using namespace cv;
using namespace std;

//This function is only used for testing.
void Print(const vector<vector<int>> &vect) {
    int x = vect.size(), y = vect[0].size();
    for (int i = 0; i < x; ++i) {
        for (int j = 0; j < y; ++j) {
            cout << vect[i][j] << " ";
        }
        cout << endl;
        cout << endl;
    }
}

//... As well as this one.
void PrintThemAll(const vector<vector<int>> &red, const vector<vector<int>> &green, const vector<vector<int>> blue) {
    for (int i = 0; i < red.size(); ++i) {
        for (int j = 0; j < red[0].size(); ++j) {
            cout << "(" << red[i][j] << ", " << green[i][j] << ", " << blue[i][j] << ") ";
        }
        cout << endl;
    }
}

//Function Process finds Laplacian(A), where A is a given matrix.
//The thing is, Laplacian could be used for pictures' processing as well.
//Indeed, if we assume (r, g, b) parametres to be some kinds of function of
//(i, j), then we could take a look at the first and the second descrete
//darivatives of these three functions. Laplacian f turns out to be expressed
//as f(i+1, j) + f(i-1, j) + f(i, j+1) + f(i, j-1) - 4f(i,j). If the calculated
//value is low at some matrix cell, we tell that it belongs to the gradient filling.
vector<vector<int>> Process(vector<vector<int>> &vect) {
    vector<vector<int>> out;
    out.resize(vect.size() - 2);
    for (int i = 0; i < out.size(); ++i) {
        out[i].resize(vect[0].size() - 2);
    }
    for (int i = 1; i < vect.size() - 1; ++i) {
        for (int j = 1; j < vect[0].size() - 1; ++j) {
            out[i - 1][j - 1] = vect[i][j + 1] + vect[i][j - 1] + vect[i + 1][j] 
              + vect[i - 1][j] - 4 * vect[i][j];
        }
    }
    return out;
}

//Function MakeGradient takes processed red, blue and green and sets 1 at (i, j) position
//iff values at the corresponding position of processed red, blue and green templates are low.
vector<vector<int>> MakeGradient(const vector<vector<int>> &red, const vector<vector<int>> &green, 
                                 const vector<vector<int>> &blue) {
    vector<vector<int>> out = red;
    for (int i = 0; i < out.size(); ++i) {
        for (int j = 0; j < out[0].size(); ++j) {
            if (abs(red[i][j]) < 5 && abs(blue[i][j]) < 5 && abs(green[i][j]) < 5) {
                out[i][j] = 1;
            } else {
                out[i][j] = 0;
            }
        }
    }
    return out;
}

//We need this function to solve the problem of finding the biggest submatrix filled 
//with only 1s in the given one.
int MaxArea(vector<int> &temp) {
    stack<int> result;
    int top;
    int max_area = 0;
    int area = 0;
    int i = 0;
    while(i < temp.size()) {
        if (result.empty() || temp[result.top()] <= temp[i]) {
            result.push(i++);
        } else {
            top = temp[result.top()];
            result.pop();
            area = top * i;
            if (!result.empty()) {
                area = top * (i - result.top() - 1);
            }
            max_area = max(area, max_area);
        }
    }
    while (!result.empty()) {
        top = temp[result.top()];
        result.pop();
        area = top * i;
        if (!result.empty()) {
            area = top * (i - result.top() - 1);
        }
        max_area = max(area, max_area);
    }
    return max_area;
}

//We need this function to find the biggest submatrix filled with 1s in
//the given one.
vector<int> MaxRow(vector<vector<int>> &finaltemp) {
    int out = 0;
    int result = MaxArea(finaltemp[0]);
    for (int i = 1; i < finaltemp.size(); ++i) {
        for (int j = 0; j < finaltemp[0].size(); ++j) {
            if (finaltemp[i][j]) {
                finaltemp[i][j] += finaltemp[i-1][j];
            }
        }
        if (result <= MaxArea(finaltemp[i])) {
            out = i;
        }
        result = max(result, MaxArea(finaltemp[i]));
    }
    vector<int> vect = {out, result};
    return vect;
}

int main()
{
    //First, open the file being researched.
    Mat img = imread("example12.jpg", IMREAD_COLOR);
    unsigned char *input = (unsigned char*)(img.data);
    int x = img.rows, y = img.cols;

    //Then initialize matrices called red, green and blue,
    //which are responsible for the respective parameters of each pixel.
    vector<vector<int>> red;
    vector<vector<int>> green;
    vector<vector<int>> blue;
    red.resize(x);
    green.resize(x);
    blue.resize(x);
    for (int i = 0; i < x; ++i) {
        red[i].resize(y);
        green[i].resize(y);
        blue[i].resize(y);
    }
    for (int i = 0; i < x; ++i) {
        for (int j = 0; j < y; ++j) {
            blue[i][j] = input[img.step * i + j];
            green[i][j] = input[img.step * i + j + 1];
            red[i][j] = input[img.step * i + j + 2];
        }
    }

    //Afterwards we need to process all of them and create the final matrix,
    //containing only 1 and 0s. 1 is for satisfying the criteria of sufficient
    //smoothness at a given point, 0 is for the opposite situation.
    vector<vector<int>> redproc = Process(red);
    vector<vector<int>> greenproc = Process(green);
    vector<vector<int>> blueproc = Process(blue);
    vector<vector<int>> final = MakeGradient(redproc, greenproc, blueproc);
    vector<vector<int>> finaltemp = final;

    //Now what we need is to choose the largest submatrix containing only 1s.
    //We could return all submatrices filled with 1s, but there is no much use.
    //To solve it, we'll use dynamic programming approach which saves information regarding
    //the biggest area available for the current row and will update it if needed.
    //Thankfully, I've already tried to solve this task during my programming classes,
    //so it wouldn't be a problem. Actually, the task was about the greatest area of a reactangle
    //if a given shape consists of columns originating from the zero level. But the solution
    //to this problem is largely borrowed from there.
    vector<int> vect = MaxRow(finaltemp);
    int maxRow = vect[0];
    int maxArea = vect[1];
  
    //As we now for sure the area of the largest submatrix and the Row at which its infimum lays,
    //we can find everything else easily.
    map<int, int> list;
    int length, current = 0;
    //A short algorithm of saving all of the substrings consisting only of 1s, retaining the 
    //number of the first character and the length of the corresponding substring as well.
    for (auto c : final[maxRow]) {
        if (c && length > 0) {
            list[current]++;
            length++;
        } else if (c && length == 0){
            list[current] = 1;
            length = 1;
        } else if (!c && length > 0) {
            current = c;
            length = 0;
        } else {
            current++;
        }
    }
    length = 0;
    for (auto c : list) {
        if (length < c.second) {
            length = c.second;
            current = c.first;
        }
    }
    int index1 = int(maxRow - maxArea / length), index2 = int(current), index3 = int (maxRow), index4 = int (current + length);

    //Now we return the answer to the user in format (i1, j1);(i2, j2), where (i1, j1) - coordinates of the
    //upper left corner of the submatrix and (i2, j2) - coordinates of coordinates of the right lower corner
    //of the found submatrix.
    cout << "(" << index1 << ", " << index2 << ");(" << index3 << ", " << index4 << ")" << endl;
    return 0;
}
