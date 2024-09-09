#ifndef P_GAME_BASICS_C
#define P_GAME_BASICS_C

#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#define ROWS 32
#define COLS 100
#define MAX_ENEMIES 100
#define MAX_BULLETS 100
#define LIFE_LOSS 1
#define TIME_STEPS 50000
#define SHOULD_RENDER 1
#define MAX_RECORDS 10
#define RECORD_FILENAME "game_records.txt"

void enableRawMode()
{
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
}

void disableRawMode()
{
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag |= (ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) & ~O_NONBLOCK);
}

typedef struct
{
    int x;
    int y;
    char shape[3];
} Ship;

typedef struct
{
    int x;
    int y;
    int hp;
    int max_hp;
    char shape;
    int movement_speed; // steps it makes each frame
    int movement_type;  // 0 for down, 1 for zigzag
    int time_moving;    // time it has been moving on screen
} Enemy;

typedef struct
{
    int x;
    int y;
} Bullet;

typedef struct
{
    int score;
    int time;
    int lives;
    int stage;
} GameState;

static char screen[ROWS][COLS];
static Ship player;
static Enemy *enemies[MAX_ENEMIES];
static Bullet bullets[MAX_BULLETS];
static GameState game;
static int enemy_count = 0;
static int bullet_count = 0;
static int stageNumber = 1;

#endif