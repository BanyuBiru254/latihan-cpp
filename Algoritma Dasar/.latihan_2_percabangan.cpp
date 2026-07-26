#include <iostream>
using namespace std;

int main()
{
    int y = 8;
    int b = 4;

    cout << "y = " << y;
    cout << endl;
    cout << "b = " << b;
    cout << endl;
    cout << "y x b?? = " << y * b << endl;

    if (y > 5)
    {
        cout << "y lebih dari 5" << endl;
    }
    else
    {
        cout << "rtuyyf" << endl;
    }

    // if (b < 4)
    // {
    //     cout << "b kurang dari 4";
    // }
    // else
    // {
    //     cout << "b itu lebih dari 4 woi";
    // }
    
    if (b < 4){
        cout << "b kurang dari 4";
    } else if (b > 4){
        cout << "b lebih dari 4";
    } else {
        cout << "b sama dengan 4";
    }
}