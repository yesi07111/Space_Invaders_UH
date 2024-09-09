#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

#include "p_memory.c"
#include "p_threading.c"
#include "p_mlfq.c"
#include "p_game_basics.c"
#include "p_enemies.c"

// Prototipos de funciones
void showMenu();
void showRecords();
void updateRecords();
void initGame();
void *updateTime(void *arg);
void *updateEnemies(void *arg);
void *updateBullets(void *arg);
void *checkCollisions(void *arg);
void updateScreen();
void handleInput();
void addBullet();
void playGame();

int main()
{
    // srand(time(NULL)); // Mejorar el random
    showMenu(); // Mostrar el menu de inicio
    return 0;
}

void showMenu()
{
    p_memory_init(); // Inicializar la memoria virtual

    printf("1. Start\n2. Records\n3. Exit\n");
    int choice;
    scanf("%d", &choice);
    switch (choice)
    {
    case 1:
        playGame();
        break;
    case 2:
        showRecords();
        showMenu();
        break;
    case 3:
        exit(0);
    default:
        showMenu();
    }
}

void playGame()
{
    initGame();

    pthread_t time_thread, enemies_thread, bullets_thread, collisions_thread;
    pthread_create(&time_thread, NULL, updateTime, NULL);
    pthread_create(&enemies_thread, NULL, updateEnemies, NULL);
    pthread_create(&bullets_thread, NULL, updateBullets, NULL);
    pthread_create(&collisions_thread, NULL, checkCollisions, NULL);

    if (SHOULD_RENDER)
        enableRawMode();

    while (game.lives > 0)
    {

        if (SHOULD_RENDER)
        {
            updateScreen();
            handleInput();
        }
        else
        {
            printf("Stage: %d", stageNumber);
            print_memory_state();
        }

        usleep(TIME_STEPS); // 50ms delay
    }

    if (SHOULD_RENDER)
        disableRawMode();

    // Cleanup threads
    pthread_cancel(time_thread);
    pthread_cancel(enemies_thread);
    pthread_cancel(bullets_thread);
    pthread_cancel(collisions_thread);

    updateRecords();

    pthread_mutex_lock(&game_mutex);
    printf("Game Over! Final Score: %d\n", game.score);
    pthread_mutex_unlock(&game_mutex);

    // showMenu();
}

void initGame()
{
    pthread_mutex_lock(&game_mutex);

    // Inicializar el juego, la nave del jugador, etc.
    player = (Ship){COLS / 2, ROWS - 1, "<=>"};
    game = (GameState){0, 0, 3, 1};
    bullet_count = 0;
    enemy_count = 0;

    for (int i = 0; i < MAX_ENEMIES; i++)
        enemies[i] = NULL;

    pthread_mutex_unlock(&game_mutex);
}

void *updateTime(void *arg)
{
    while (1)
    {
        sleep(1);

        pthread_mutex_lock(&game_mutex);
        game.time++;
        pthread_mutex_unlock(&game_mutex);
    }
}

void addBullet(int x, int y)
{
    pthread_mutex_lock(&game_mutex);
    if (bullet_count < MAX_BULLETS)
    {
        bullets[bullet_count].x = x;
        bullets[bullet_count].y = y;
        bullet_count++;
    }
    pthread_mutex_unlock(&game_mutex);
}

void removeBullet(int index)
{
    if (index >= 0 && index < bullet_count)
    {
        for (int i = index; i < bullet_count - 1; i++)
        {
            bullets[i] = bullets[i + 1];
        }
        bullet_count--;
    }
}

void *updateBullets(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&game_mutex);
        for (int i = 0; i < bullet_count; i++)
        {
            bullets[i].y--;
            if (bullets[i].y < 0)
            {
                removeBullet(i);
                i--;
            }
        }
        pthread_mutex_unlock(&game_mutex);
        usleep(TIME_STEPS); // 50ms delay
    }
}

void *checkCollisions(void *arg)
{
    while (1)
    {
        for (int i = 0; i < bullet_count; i++)
        {
            for (int j = 0; j < MAX_ENEMIES; j++)
            {
                pthread_mutex_lock(&game_mutex);

                if (enemies[j] == NULL)
                {
                    pthread_mutex_unlock(&game_mutex);
                    continue;
                }

                if (bullets[i].x == enemies[j]->x && bullets[i].y == enemies[j]->y)
                {
                    enemies[j]->hp--;
                    if (enemies[j]->hp <= 0)
                    {
                        game.score += enemies[j]->max_hp;

                        pthread_mutex_unlock(&game_mutex);
                        removeEnemy(j);
                        pthread_mutex_lock(&game_mutex);
                    }

                    pthread_mutex_unlock(&game_mutex);
                    removeBullet(i);
                    pthread_mutex_lock(&game_mutex);

                    i--;

                    pthread_mutex_unlock(&game_mutex);
                    break;
                }

                pthread_mutex_unlock(&game_mutex);
            }
        }

        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            pthread_mutex_lock(&game_mutex);

            if (enemies[i] == NULL)
            {
                pthread_mutex_unlock(&game_mutex);
                continue;
            }

            if (enemies[i]->x >= player.x && enemies[i]->x <= player.x + 2 && enemies[i]->y == player.y)
            {
                game.lives -= LIFE_LOSS;

                pthread_mutex_unlock(&game_mutex);
                removeEnemy(i);
                pthread_mutex_lock(&game_mutex);
            }

            else if (enemies[i]->y >= ROWS - 1)
            {
                pthread_mutex_unlock(&game_mutex);
                removeEnemy(i);
                pthread_mutex_lock(&game_mutex);
            }

            pthread_mutex_unlock(&game_mutex);
        }

        usleep(TIME_STEPS); // 50ms delay
    }
    return NULL;
}

void updateScreen()
{
    pthread_mutex_lock(&game_mutex);

    // Limpiar pantalla
    system("clear");

    // Actualizar matriz de pantalla
    memset(screen, ' ', sizeof(screen));

    // Dibujar nave del jugador
    screen[player.y][player.x] = player.shape[0];
    screen[player.y][player.x + 1] = player.shape[1];
    screen[player.y][player.x + 2] = player.shape[2];

    // Dibujar enemigos
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (enemies[i] == NULL)
        {
            continue;
        }
        if (enemies[i]->y >= 0 && enemies[i]->y < ROWS && enemies[i]->x >= 0 && enemies[i]->x < COLS)
        {
            screen[enemies[i]->y][enemies[i]->x] = enemies[i]->shape;
        }
    }

    // Dibujar balas
    for (int i = 0; i < bullet_count; i++)
    {
        if (bullets[i].y >= 0 && bullets[i].y < ROWS && bullets[i].x >= 0 && bullets[i].x < COLS)
        {
            screen[bullets[i].y][bullets[i].x] = '|';
        }
        else
        {
            printf("{%d, %d}", bullets[i].y, bullets[i].x);
            exit(0);
        }
    }

    // Imprimir pantalla
    memory_state state = get_memory_state();
    printf("[MEMORY: Free=%d, Used=%d] Score: %d | Time: %d | Stage: %d | Lives: ", state.free, state.used, game.score, game.time, stageNumber);
    for (int i = 0; i < game.lives; i++)
        printf("♥");
    printf("\n");

    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            putchar(screen[i][j]);
        }
        putchar('\n');
    }

    pthread_mutex_unlock(&game_mutex);
}

void handleInput()
{
    static int is_firing = 0;
    static int move_direction = 0; // -1 izquierda, 0 quieto, 1 derecha
    char c;

    pthread_mutex_lock(&game_mutex);
    if (read(STDIN_FILENO, &c, 1) == 1)
    {
        if (c == 'a')
            move_direction = -1;
        else if (c == 'd')
            move_direction = 1;
        else if (c == 'w')
            is_firing = !is_firing;
        else if (c == ' ')
        {
            move_direction = 0;
        }
    }

    // Mover nave
    player.x += move_direction;
    if (player.x < 0)
        player.x = 0;
    if (player.x > COLS - 3)
        player.x = COLS - 3;

    // Disparar
    if (is_firing && bullet_count < MAX_BULLETS)
    {
        pthread_mutex_unlock(&game_mutex);
        addBullet(player.x + 1, player.y - 1);
        pthread_mutex_lock(&game_mutex);
    }
    pthread_mutex_unlock(&game_mutex);
}

typedef struct
{
    int time;
    int score;
    int stage;
} Record;

    void updateRecords()
{
    int time = game.time;
    int score = game.score;
    int stage = stageNumber;

    FILE *file;
    Record records[MAX_RECORDS + 1];
    int count = 0;

    // Leer registros existentes
    file = fopen(RECORD_FILENAME, "r");
    if (file != NULL)
    {
        while (fscanf(file, "%d %d %d", &records[count].time, &records[count].score, &records[count].stage) == 3)
        {
            count++;
            if (count >= MAX_RECORDS)
                break;
        }
        fclose(file);
    }

    // Añadir nuevo registro
    records[count].time = time;
    records[count].score = score;
    records[count].stage = stage;
    count++;

    // Ordenar registros por puntuación (de mayor a menor)
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = 0; j < count - i - 1; j++)
        {
            if (records[j].score < records[j + 1].score)
            {
                Record temp = records[j];
                records[j] = records[j + 1];
                records[j + 1] = temp;
            }
        }
    }

    // Escribir los mejores registros de vuelta al archivo
    file = fopen(RECORD_FILENAME, "w");
    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    for (int i = 0; i < count && i < MAX_RECORDS; i++)
    {
        fprintf(file, "%d %d %d\n", records[i].time, records[i].score, records[i].stage);
    }

    fclose(file);
}

void showRecords()
{
    FILE *file;
    int time, score, stage;
    int count = 0;

    file = fopen(RECORD_FILENAME, "r");
    if (file == NULL)
    {
        printf("No stored records.\n");
        return;
    }

    printf("Top %d Records:\n", MAX_RECORDS);
    printf("Rank\tTime\tScore\tStage\n");
    while (fscanf(file, "%d %d %d", &time, &score, &stage) == 3 && count < MAX_RECORDS)
    {
        printf("%d\t%d\t%d\t%d\n", count + 1, time, score, stage);
        count++;
    }

    if (count == 0)
    {
        printf("No stored records.\n");
    }

    fclose(file);
}

// void updateRecords()
// {
//     int time = game.time;
//     int score = game.score;

//     FILE *file;
//     Record records[MAX_RECORDS + 1];
//     int count = 0;

//     // Leer registros existentes
//     file = fopen(RECORD_FILENAME, "r");
//     if (file != NULL)
//     {
//         while (fscanf(file, "%d %d", &records[count].time, &records[count].score) == 2)
//         {
//             count++;
//             if (count >= MAX_RECORDS)
//                 break;
//         }
//         fclose(file);
//     }

//     // Añadir nuevo registro
//     records[count].time = time;
//     records[count].score = score;
//     count++;

//     // Ordenar registros por puntuación (de mayor a menor)
//     for (int i = 0; i < count - 1; i++)
//     {
//         for (int j = 0; j < count - i - 1; j++)
//         {
//             if (records[j].score < records[j + 1].score)
//             {
//                 Record temp = records[j];
//                 records[j] = records[j + 1];
//                 records[j + 1] = temp;
//             }
//         }
//     }

//     // Escribir los mejores registros de vuelta al archivo
//     file = fopen(RECORD_FILENAME, "w");
//     if (file == NULL)
//     {
//         printf("Error opening file.\n");
//         return;
//     }

//     for (int i = 0; i < count && i < MAX_RECORDS; i++)
//     {
//         fprintf(file, "%d %d\n", records[i].time, records[i].score);
//     }

//     fclose(file);
// }

// void showRecords()
// {
//     FILE *file;
//     int time, score;
//     int count = 0;

//     file = fopen(RECORD_FILENAME, "r");
//     if (file == NULL)
//     {
//         printf("No stored records.\n");
//         return;
//     }

//     printf("Top %d Records:\n", MAX_RECORDS);
//     printf("Rank\tTime\tScore\n");
//     while (fscanf(file, "%d %d", &time, &score) == 2 && count < MAX_RECORDS)
//     {
//         printf("%d\t%d\t%d\n", count + 1, time, score);
//         count++;
//     }

//     if (count == 0)
//     {
//         printf("No stored records.\n");
//     }

//     fclose(file);
// }
