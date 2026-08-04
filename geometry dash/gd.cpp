// =========================================================
//  GEOMETRY DASH CLONE - Console C++ Edition (IMPROVED)
//  Kontrol : SPASI / UP untuk lompat, P untuk pause, Q untuk keluar
// =========================================================
 
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <algorithm>
 
#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <fcntl.h>
#endif
 
// ---------------- KONFIGURASI ----------------
const int WIDTH          = 60;   // lebar area permainan
const int HEIGHT         = 16;   // tinggi area permainan
const int GROUND_Y        = HEIGHT - 2;
const int PLAYER_X        = 8;
const double GRAVITY      = 0.9;
const double JUMP_FORCE   = -3.9;
const int FRAME_DELAY_MS  = 40;  // kecepatan game (ms per frame)
const char* HS_FILE       = "highscore.txt";
 
// ---------------- WARNA ANSI ----------------
namespace col {
    const char* RESET  = "\x1b[0m";
    const char* PLAYER = "\x1b[92m";  // hijau terang
    const char* SPIKE  = "\x1b[91m";  // merah terang
    const char* GROUND = "\x1b[33m";  // kuning/coklat
    const char* BG     = "\x1b[36m";  // cyan (dekorasi)
    const char* TITLE  = "\x1b[95m";  // magenta
    const char* SCORE  = "\x1b[93m";  // kuning terang
    const char* DEAD   = "\x1b[91m";
}
 
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
 
// ---------------- HIGH SCORE ----------------
long loadHighScore() {
    std::ifstream f(HS_FILE);
    long hs = 0;
    if (f.is_open()) f >> hs;
    return hs;
}
 
void saveHighScore(long hs) {
    std::ofstream f(HS_FILE);
    if (f.is_open()) f << hs;
}
 
// ---------------- OBSTACLE ----------------
enum class ObType { SPIKE, PIT, BLOCK };
 
struct Obstacle {
    double x;
    int height;   // untuk SPIKE/BLOCK: tinggi dari tanah. untuk PIT: lebar lubang
    ObType type;
    bool passed = false; // sudah dilewati player (buat cek skor kombo, opsional)
};
 
// ---------------- GAME STATE ----------------
struct Player {
    double y = GROUND_Y;
    double velocity = 0;
    bool onGround = true;
};
 
// Cek apakah posisi x ada di dalam lubang (pit) -> kalau ya dan player di tanah, mati
bool isPit(double x, const std::deque<Obstacle> &obstacles) {
    for (const auto &ob : obstacles) {
        if (ob.type != ObType::PIT) continue;
        if (x >= ob.x && x < ob.x + ob.height) return true;
    }
    return false;
}
 
bool checkCollision(const Player &p, const std::deque<Obstacle> &obstacles) {
    int px = PLAYER_X;
    int py = (int)(p.y + 0.5); // posisi bulat player
 
    // Jatuh ke lubang saat di tanah
    if (p.onGround && isPit((double)px, obstacles)) return true;
 
    for (const auto &ob : obstacles) {
        if (ob.type == ObType::PIT) continue;
        int ox = (int)(ob.x + 0.5);
        if (ox != px) continue;
 
        if (ob.type == ObType::SPIKE) {
            int spikeTop = GROUND_Y - ob.height + 1;
            if (py >= spikeTop) return true;
        } else if (ob.type == ObType::BLOCK) {
            // block melayang di udara, harus dilompati pas ketinggian pas
            int blockY = GROUND_Y - ob.height;
            if (py == blockY) return true;
        }
    }
    return false;
}
 
// ---------------- RENDER (pakai buffer, anti-flicker) ----------------
void render(const Player &p, const std::deque<Obstacle> &obstacles,
            long score, long highScore, bool gameOver, bool paused, int frameTick) {
    std::vector<std::string> grid(HEIGHT, std::string(WIDTH, ' '));
    std::vector<std::string> color(HEIGHT, std::string(WIDTH, ' '));
 
    auto setCell = [&](int y, int x, char ch, const char* c) {
        if (y >= 0 && y < HEIGHT && x >= 0 && x < WIDTH) {
            grid[y][x] = ch;
            color[y][x] = c[0]; // marker sederhana, dipetakan lagi saat print
        }
    };
 
    // Dekorasi background (bintang bergerak pelan)
    for (int x = 0; x < WIDTH; x += 7) {
        int bx = (x + frameTick / 3) % WIDTH;
        if (grid[2][bx] == ' ') setCell(2, bx, '.', col::BG);
    }
 
    // Lantai + lubang
    for (int x = 0; x < WIDTH; x++) {
        if (!isPit((double)x, obstacles)) {
            setCell(GROUND_Y + 1, x, '=', col::GROUND);
        }
    }
 
    // Obstacles
    for (const auto &ob : obstacles) {
        if (ob.type == ObType::SPIKE) {
            int ox = (int)(ob.x + 0.5);
            for (int h = 0; h < ob.height; h++) {
                setCell(GROUND_Y - h, ox, (h == ob.height - 1) ? '^' : '#', col::SPIKE);
            }
        } else if (ob.type == ObType::BLOCK) {
            int ox = (int)(ob.x + 0.5);
            setCell(GROUND_Y - ob.height, ox, '#', col::SPIKE);
        }
        // PIT tidak digambar sebagai objek, cuma menghilangkan lantai
    }
 
    // Player
    int py = (int)(p.y + 0.5);
    setCell(py, PLAYER_X, gameOver ? 'X' : '@', col::PLAYER);
 
    // ---------- Build output string sekali jalan ----------
    std::ostringstream out;
    out << "\x1b[H"; // pindah kursor ke atas (gak clear penuh -> kurangi flicker)
    out << col::TITLE << "=========== GEOMETRY DASH (C++ Console) ===========" << col::RESET << "\n";
    out << col::SCORE << "Skor: " << score << "   Terbaik: " << highScore << col::RESET << "        \n";
 
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            char ch = grid[y][x];
            char c = color[y][x];
            if (ch == ' ') { out << ' '; continue; }
            switch (c) {
                case 'P': out << col::PLAYER << ch << col::RESET; break; // unused marker fallback
                default:
                    if (c == col::PLAYER[0]) out << col::PLAYER << ch << col::RESET;
                    else if (c == col::SPIKE[0]) out << col::SPIKE << ch << col::RESET;
                    else if (c == col::GROUND[0]) out << col::GROUND << ch << col::RESET;
                    else if (c == col::BG[0]) out << col::BG << ch << col::RESET;
                    else out << ch;
            }
        }
        out << "\n";
    }
 
    out << col::TITLE << "=====================================================" << col::RESET << "\n";
    if (gameOver) {
        out << col::DEAD << "GAME OVER! Skor akhir: " << score << col::RESET << "                    \n";
        out << "Tekan R untuk main lagi, Q untuk keluar.                        \n";
    } else if (paused) {
        out << "-- PAUSE -- Tekan P untuk lanjut.                                \n";
        out << "[SPASI/UP] Lompat   [P] Pause   [Q] Keluar                       \n";
    } else {
        out << "[SPASI/UP] Lompat   [P] Pause   [Q] Keluar                       \n";
    }
 
    std::cout << out.str() << std::flush;
}
 
int main() {
    srand((unsigned)time(nullptr));
 
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
 
#ifndef _WIN32
    TermiosRaw raw;
#endif
 
    std::cout << "\x1b[2J"; // clear penuh sekali di awal
    long highScore = loadHighScore();
    bool running = true;
 
    while (running) {
        Player player;
        std::deque<Obstacle> obstacles;
        long score = 0;
        double gameSpeed = 0.55;
        double spawnTimer = 20;
        bool gameOver = false;
        bool paused = false;
        bool jumpQueued = false;
        int frameTick = 0;
        bool scoreSaved = false;
 
        while (true) {
            // ---------- INPUT ----------
#ifdef _WIN32
            if (_kbhit()) {
                int c = _getch();
                if (c == 'q' || c == 'Q') { running = false; break; }
                if (c == ' ' || c == 72) jumpQueued = true;
                if (c == 'p' || c == 'P') paused = !paused;
                if (gameOver && (c == 'r' || c == 'R')) break;
            }
#else
            char c;
            while (keyPressed(c)) {
                if (c == 'q' || c == 'Q') { running = false; }
                if (c == ' ') jumpQueued = true;
                if (c == 'p' || c == 'P') paused = !paused;
                if (gameOver && (c == 'r' || c == 'R')) goto restart;
            }
            if (!running) break;
#endif
 
            if (!gameOver && !paused) {
                frameTick++;
 
                // ---------- LOMPAT ----------
                if (jumpQueued && player.onGround) {
                    player.velocity = JUMP_FORCE;
                    player.onGround = false;
                }
                jumpQueued = false;
 
                // ---------- FISIKA ----------
                player.velocity += GRAVITY * 0.35;
                player.y += player.velocity * 0.5;
                if (player.y >= GROUND_Y && !isPit((double)PLAYER_X, obstacles)) {
                    player.y = GROUND_Y;
                    player.velocity = 0;
                    player.onGround = true;
                } else if (player.y < GROUND_Y) {
                    player.onGround = false;
                }
                if (player.y > HEIGHT + 2) gameOver = true; // jatuh ke lubang terlalu dalam
 
                // ---------- SPAWN OBSTACLE ----------
                spawnTimer -= gameSpeed;
                if (spawnTimer <= 0) {
                    Obstacle ob;
                    ob.x = WIDTH - 1;
                    int roll = rand() % 10;
                    if (roll < 5) {
                        ob.type = ObType::SPIKE;
                        ob.height = (rand() % 3 == 0) ? 2 : 1;
                    } else if (roll < 8) {
                        ob.type = ObType::PIT;
                        ob.height = 2 + rand() % 2; // lebar lubang
                    } else {
                        ob.type = ObType::BLOCK;
                        ob.height = 3; // harus dilompati di ketinggian pas
                    }
                    obstacles.push_back(ob);
                    spawnTimer = 16 + rand() % 10;
                }
 
                // ---------- GERAK OBSTACLE ----------
                for (auto &ob : obstacles) ob.x -= gameSpeed;
                while (!obstacles.empty() &&
                       obstacles.front().x + std::max(obstacles.front().height, 1) < 0)
                    obstacles.pop_front();
 
                // ---------- SKOR & KECEPATAN ----------
                score++;
                if (score % 400 == 0) gameSpeed = std::min(gameSpeed + 0.07, 1.6);
 
                // ---------- COLLISION ----------
                if (checkCollision(player, obstacles)) gameOver = true;
 
