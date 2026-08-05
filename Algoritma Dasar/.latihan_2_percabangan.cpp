#include <iostream>
using namespace std;

int main()
{
    int umur = 17;
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

    char status = 'y';

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
    cout << endl;
    char AKTIF = 'A';
    int DENDA = 0;

    if (DENDA >= 0 && AKTIF == 'A')
    {
        cout << "kamu boleh meminjam buku";
    }
    else if (DENDA >= 0 && AKTIF == 'A')
    {
        cout << "kamu tidak boleh meminjam karena belum terdaftar";
    }
    else
    {
        cout << "Kamu tidak boleh meminjam buku karena ada denda";
    }
    cout << endl;

    int teori = 80;
    int praktik = 75;

    if (teori >= 80 && praktik >= 75)
    {
        cout << "Kamu lulus ujian.";
    }
    else if (teori >= 80 && praktik < 75)
    {
        cout << "Kamu lulus teori, tetapi tidak lulus praktik.";
    }
    else if (teori < 80 && praktik >= 75)
    {
        cout << "Kamu lulus praktik, tetapi tidak lulus teori.";
    }

    else
    {
        cout << "Kamu tidak lulus ujian.";
    }

int angkah;
    cout << "Masukkan angkah: ";
    cin >> angkah;
    
    if (angka % 2 == 0) {
        cout << angkah << " adalah bilangan genap." << endl;
    } else {
        cout << angkah << " adalah bilangan ganjil." << endl;

int main() {
    int nilai;
    cout << "Masukkan nilai ujian: ";
    cin >> nilai;
    
    if (nilai >= 75) {
        cout << "Status: Lulus" << endl;
    } else {
        cout << "Status: Tidak Lulus" << endl;


}