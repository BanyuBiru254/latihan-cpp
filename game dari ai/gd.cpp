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
#endif
 
void sleepMs(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}
 
void clearScreen() {
    // ANSI escape code, jalan di Windows Terminal / CMD baru / Linux / Mac
    std::cout << "\x1b[H\x1b[2J\x1b[3J";
}
 
// ---------------- OBSTACLE ----------------
struct Obstacle {
    double x;
    int height; // 1 = duri kecil, 2 = duri tinggi
};
 
// ---------------- GAME STATE ----------------
struct Player {
    double y = GROUND_Y;
    double velocity = 0;
    bool onGround = true;
};
 
bool checkCollision(const Player &p, const std::deque<Obstacle> &obstacles) {
    for (const auto &ob : obstacles) {
        int ox = (int)ob.x;
        if (ox == PLAYER_X) {
            int playerTop = (int)(p.y) - 1; // tinggi player = 1 kotak (dari y ke y-1)
            int obstacleTop = GROUND_Y - ob.height;
            if ((int)p.y >= obstacleTop) {
                return true;
            }
        }
    }
    return false;
}
 
void render(const Player &p, const std::deque<Obstacle> &obstacles, long score, bool gameOver) {
    std::vector<std::string> grid(HEIGHT, std::string(WIDTH, ' '));
 
    // Lantai
    for (int x = 0; x < WIDTH; x++) grid[GROUND_Y + 1][x] = '=';
 
    // Obstacles (duri)
    for (const auto &ob : obstacles) {
        int ox = (int)ob.x;
        if (ox < 0 || ox >= WIDTH) continue;
        for (int h = 0; h < ob.height; h++) {
            int gy = GROUND_Y - h;
            if (gy >= 0 && gy < HEIGHT) grid[gy][ox] = '^';
        }
    }
 
    // Player (kotak)
    int py = (int)p.y;
    if (py >= 0 && py < HEIGHT && PLAYER_X < WIDTH) {
        grid[py][PLAYER_X] = gameOver ? 'X' : '#';
    }
 
    clearScreen();
    std::cout << "=========== GEOMETRY DASH (C++ Console) ===========\n";
    std::cout << "Skor: " << score << "\n";
    for (auto &row : grid) std::cout << row << "\n";
    std::cout << "=====================================================\n";
    if (gameOver) {
        std::cout << "GAME OVER! Skor akhir: " << score << "\n";
        std::cout << "Tekan R untuk main lagi, Q untuk keluar.\n";
    } else {
        std::cout << "[SPASI/UP] Lompat   [Q] Keluar\n";
    }
}
 
int main() {
    srand((unsigned)time(nullptr));
 
#ifdef _WIN32
    // Aktifkan dukungan ANSI escape code di console Windows (biar layar bisa di-clear)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
 
#ifndef _WIN32
    TermiosRaw raw; // aktifkan mode input non-blocking selama program jalan
#endif
 
    bool running = true;
 
    while (running) {
        Player player;
        std::deque<Obstacle> obstacles;
        long score = 0;
        double gameSpeed = 0.55;
        double spawnTimer = 0;
        bool gameOver = false;
        bool jumpQueued = false;
 
        while (true) {
            // ---------- INPUT ----------
#ifdef _WIN32
            if (_kbhit()) {
                int c = _getch();
                if (c == 'q' || c == 'Q') { running = false; break; }
                if (c == ' ' || c == 72) jumpQueued = true;   // spasi atau panah atas
                if (gameOver && (c == 'r' || c == 'R')) break; // restart
            }
#else
            char c;
            while (keyPressed(c)) {
                if (c == 'q' || c == 'Q') { running = false; }
                if (c == ' ') jumpQueued = true;
                if (gameOver && (c == 'r' || c == 'R')) goto restart;
            }
            if (!running) break;
#endif
 
            if (!gameOver) {
                // ---------- LOGIKA LOMPAT ----------
                if (jumpQueued && player.onGround) {
                    player.velocity = JUMP_FORCE;
                    player.onGround = false;
                }
                jumpQueued = false;
 
                // ---------- FISIKA ----------
                player.velocity += GRAVITY * 0.35;
                player.y += player.velocity * 0.5;
                if (player.y >= GROUND_Y) {
                    player.y = GROUND_Y;
                    player.velocity = 0;
                    player.onGround = true;
                }
 
                // ---------- SPAWN OBSTACLE ----------
                spawnTimer -= gameSpeed;
                if (spawnTimer <= 0) {
                    Obstacle ob;
                    ob.x = WIDTH - 1;
                    ob.height = (rand() % 3 == 0) ? 2 : 1;
                    obstacles.push_back(ob);
                    spawnTimer = 14 + rand() % 10;
                }
 
                // ---------- GERAK OBSTACLE ----------
                for (auto &ob : obstacles) ob.x -= gameSpeed;
                while (!obstacles.empty() && obstacles.front().x < 0)
                    obstacles.pop_front();
 
                // ---------- SKOR & KECEPATAN ----------
                score++;
                if (score % 300 == 0) gameSpeed += 0.08;
 
                // ---------- COLLISION ----------
                if (checkCollision(player, obstacles)) gameOver = true;
            }
 
            render(player, obstacles, score / 10, gameOver);
            sleepMs(FRAME_DELAY_MS);
 
            if (!running) break;
        }
