
// =========================================================
//  GEOMETRY DASH CLONE - Console C++ Edition
//  Kontrol : SPASI / UP untuk lompat, Q untuk keluar
// =========================================================
 
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <deque>
 
#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/select.h>
#endif
 
// ---------------- KONFIGURASI ----------------
const int WIDTH        = 60;   // lebar area permainan
const int HEIGHT        = 16;   // tinggi area permainan
const int GROUND_Y       = HEIGHT - 2;
const int PLAYER_X       = 8;
const double GRAVITY     = 0.9;
const double JUMP_FORCE  = -3.6;
const int FRAME_DELAY_MS = 45;  // kecepatan game (ms per frame)
 
// ---------------- INPUT NON-BLOCKING ----------------
#ifndef _WIN32
struct TermiosRaw {
    termios oldt{};
    TermiosRaw() {
        termios newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    }
    ~TermiosRaw() {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    }
};
 
bool keyPressed(char &c) {
    char buf;
    ssize_t n = read(STDIN_FILENO, &buf, 1);
    if (n > 0) { c = buf; return true; }
    return false;
}
