#include <iostream>
using namespace std;

int main()
{
    int umur ;
    cin >> umur ;
    int b = 4;

    // cout << "y = " << y;
    //  cout << endl;
    //  cout << "b = " << b;
    //  cout << endl;
    //  cout << "y x b?? = " << y * b << endl;

    //  if (y > 5)
    //  {
    //       cout << "y lebih dari 5" << endl;
    //   }
    //  else
    //   {
    //       cout << "rtuyyf" << endl;
    //   }

    // if (b < 4)
    // {
    //     cout << "b kurang dari 4";
    // }
    // else
    // {
    //     cout << "b itu lebih dari 4 woi";
    // }

    //   if (b < 4)
    //   {
    //       cout << "b kurang dari 4";
    //{
    //   cout << "b lebih dari 4";
    //   }
    //   else
    //   {
    //       cout << "b sama dengan 4";
    // }

    char status ;
    cin >> status ;

    if (umur >= 17 && status == 'y')
    {
        cout << "Warga boleh memilih.";
    }
    else if (umur >= 17)
    {
        cout << "Warga cukup umur, tetapi belum terdaftar.";
    }
    else
    {
        cout << "Warga di bawah umur, akses ditolak.";
    }
}
