#ifndef P_ENEMIES_C
#define P_ENEMIES_C

#include "p_game_basics.c"
#include "p_threading.c"
#include "p_mlfq.c"
#include "p_memory.c"

const Enemy enemy_types[10] = {
    {0, -1, 1, 1, 'X', 1, 0, 0},   // Tipo 1: Débil y lento
    {0, -1, 2, 2, 'Y', 1, 0, 0},   // Tipo 2: Un poco más resistente
    {0, -1, 3, 3, 'Z', 2, 0, 0},   // Tipo 3: Más rápido
    {0, -1, 4, 4, '@', 2, 1, 0},   // Tipo 4: Zigzag y más resistente
    {0, -1, 5, 5, '#', 3, 0, 0},   // Tipo 5: Aún más rápido y resistente
    {0, -1, 6, 6, 'W', 3, 1, 0},   // Tipo 6: Zigzag rápido
    {0, -1, 8, 8, 'T', 4, 0, 0},   // Tipo 7: Muy resistente y rápido
    {0, -1, 10, 10, 'V', 4, 1, 0}, // Tipo 8: Zigzag muy resistente
    {0, -1, 12, 12, 'M', 5, 0, 0}, // Tipo 9: Extremadamente rápido y resistente
    {0, -1, 15, 15, 'U', 5, 1, 0}  // Tipo 10: Jefe final, zigzag muy rápido y resistente
};

void addEnemy(Enemy *enemy)
{
    pthread_mutex_lock(&game_mutex);
    if (enemy_count < MAX_ENEMIES)
    {
        int i = 0;
        for (; i < MAX_ENEMIES && enemies[i] != NULL; i++)
            ;
        enemies[i] = enemy;
        enemy_count++;
    }
    pthread_mutex_unlock(&game_mutex);
}

void removeEnemy(int index)
{
    pthread_mutex_lock(&game_mutex);
    if (index >= 0 && index < MAX_ENEMIES && enemies[index] != NULL)
    {
        enemies[index]->hp = 0; // kill that enemy
        enemies[index] = NULL;
        enemy_count--;
    }
    pthread_mutex_unlock(&game_mutex);
}

Enemy **getStage(int stageSize)
{
    Enemy **stageEnemies = (Enemy **)p_malloc(stageSize * sizeof(Enemy *));

    int i = 0, j = 0;
    while (j < stageSize)
    {
        int mid = (stageSize - j) / 2;

        for (; mid > 0; j++, mid--)
        {
            stageEnemies[j] = p_malloc(sizeof(Enemy));

            pthread_mutex_lock(&game_mutex);
            memcpy(stageEnemies[j], &enemy_types[i], sizeof(Enemy));
            pthread_mutex_unlock(&game_mutex);
        }

        if (j == stageSize - 1)
        {
            stageEnemies[j] = p_malloc(sizeof(Enemy));

            pthread_mutex_lock(&game_mutex);
            memcpy(stageEnemies[j], &enemy_types[i], sizeof(Enemy));
            pthread_mutex_unlock(&game_mutex);

            break;
        }

        i = (i + 1) % 10;
    }

    return stageEnemies;
}

void moveEnemy(Enemy *enemy)
{
    pthread_mutex_lock(&game_mutex);
    if (enemy == NULL)
    {
        perror("Trying to move NULL enemy");
        abort();
    }

    if (enemy->movement_type == 0)
    {
        enemy->y++;
    }
    else
    {
        enemy->y++;
        enemy->x += enemy->movement_type; // Movimiento zigzag

        if (rand() % 5 == 0)enemy->movement_type *= -1;
    }

    if (enemy->x < 0)
        enemy->x = 0;
    if (enemy->x >= COLS)
        enemy->x = COLS - 1;
    if (enemy->y >= ROWS)
        enemy->y = ROWS - 1;
    pthread_mutex_unlock(&game_mutex);
}

void *updateEnemies(void *arg)
{
    MLFQ *enemyManager;
    stageNumber = 1;
    Enemy *current = NULL;
    int currentStepsRemaining = 0;
    Enemy **stage = NULL;

    const int initialStageSize = 1;
    const int stageGrowthRate = 3;
    int x = 0;

    enemyManager = (MLFQ *)p_malloc(sizeof(MLFQ));
    initMLFQ(enemyManager);

    while (1)
    {

        if (getMLFQSize(enemyManager) == 0)
        {
            int stageSize = initialStageSize + (stageNumber - 1) * stageGrowthRate;
            stageNumber++;

            stage = getStage(stageSize);

            for (int i = 0; i < stageSize; i++)
            {
                putEnemyInMLFQ(enemyManager, stage[i]);
            }

            p_free(stage);
        }

        if (currentStepsRemaining == 0)
        {
            current = getEnemyFromMLFQ(enemyManager);
            
            if (current == NULL)
            {
                usleep(TIME_STEPS);
                continue;
            }

            currentStepsRemaining = current->movement_speed;
        }

        if (current->y == -1)
        {
            current->y = 0;
            current->x = rand() % COLS;

            addEnemy(current);
        }

        moveEnemy(current);

        currentStepsRemaining--;
        usleep(TIME_STEPS);
    }
}

#endif