#ifndef P_MLFQ_C
#define P_MLFQ_C

#include <stdbool.h>
#include <stdlib.h>

#include "p_game_basics.c"

typedef struct _Node
{
    Enemy *value;
    struct _Node *next;
} Node;

typedef struct
{
    Node *head, *tail;
    int len;
} Queue;

typedef struct
{
    Queue *priorHigh;
    Queue *priorMid;
    Queue *priorLow;

    int updateTime;
    int timeSlice;

    int updateCount;
} MLFQ;

void start_queue(Queue *q)
{
    q->head = q->tail = NULL;
    q->len = 0;
}

void push_item(Queue *q, Enemy *value)
{
    if (q->tail != NULL)
    {
        q->tail->next = (Node *)p_malloc(sizeof(Node));
        q->tail->next->value = value;
        q->tail = q->tail->next;
    }
    else
    {
        q->tail = q->head = (Node *)p_malloc(sizeof(Node));
        q->head->value = value;
    }

    q->len++;
}

Enemy *pop_item(Queue *q)
{
    if (q->len == 0 || q->head == NULL)
    {
        perror("Empty queue!");
        return NULL;
    }

    Enemy *ans = q->head->value;
    Node *tmp = q->head;

    q->head = q->head->next;
    q->len--;

    p_free(tmp);

    if (q->len == 0)
        q->head = q->tail = NULL;

    return ans;
}

static void delete_dead_enemies(Queue *q)
{
    for (int i = 0; i < q->len; i++)
    {
        Enemy *enemy = pop_item(q);

        if (enemy->hp > 0)
            push_item(q, enemy);
        else
            p_free(enemy);
    }
}

static void concat_queues(Queue *to, Queue **from, int count)
{
    for (int j = 0; j < count; j++)
    {
        Queue *_from = from[j];

        while (_from->len)
        {
            Enemy *enemy = pop_item(_from);
            enemy->time_moving = 0;

            push_item(to, enemy);
        }

        start_queue(_from);
    }
}

Enemy *get_movable_enemy(Queue *_enemies)
{
    if (_enemies->len == 0)
    {
        return NULL;
    }

    for (int i = 0; i < _enemies->len; i++)
    {
        Enemy *enemy = pop_item(_enemies);
        push_item(_enemies, enemy);

        if (enemy->y != -1 || enemy_count < MAX_ENEMIES)
            return enemy;
    }
    return NULL;
}

void initMLFQ(MLFQ *mlfq)
{
    static bool _free = false;

    if (_free)
    {
        p_free(mlfq->priorHigh);
        p_free(mlfq->priorMid);
        p_free(mlfq->priorLow);
    }

    _free = true;

    mlfq->priorHigh = (Queue *)p_malloc(sizeof(Queue));
    mlfq->priorMid = (Queue *)p_malloc(sizeof(Queue));
    mlfq->priorLow = (Queue *)p_malloc(sizeof(Queue));

    start_queue(mlfq->priorHigh);
    start_queue(mlfq->priorMid);
    start_queue(mlfq->priorLow);

    mlfq->updateTime = 20;
    mlfq->updateCount = 0;
    mlfq->timeSlice = 8;
}

void putEnemyInMLFQ(MLFQ *mlfq, Enemy *enemy)
{
    push_item(mlfq->priorHigh, enemy);
}

Enemy *getEnemyFromMLFQ(MLFQ *mlfq)
{
    delete_dead_enemies(mlfq->priorHigh);
    delete_dead_enemies(mlfq->priorMid);
    delete_dead_enemies(mlfq->priorLow);

    if (mlfq->updateCount == mlfq->updateTime)
    {
        mlfq->updateCount = 0;
        Queue *qq[] = {mlfq->priorMid, mlfq->priorLow};
        concat_queues(mlfq->priorHigh, qq, 2);
    }
    mlfq->updateCount++;

    int n = mlfq->priorHigh->len;
    for (int i = 0; i < n; i++)
    {
        if (mlfq->priorMid->len && mlfq->priorMid->head->value->time_moving >= mlfq->timeSlice)
        {
            Enemy *tmp = pop_item(mlfq->priorMid);
            tmp->time_moving = 0;

            push_item(mlfq->priorLow, tmp);
        }
    }

    n = mlfq->priorMid->len;
    for (int i = 0; i < n; i++)
    {
        if (mlfq->priorHigh->len && mlfq->priorHigh->head->value->time_moving == mlfq->timeSlice)
        {
            Enemy *tmp = pop_item(mlfq->priorHigh);
            tmp->time_moving = 0;

            push_item(mlfq->priorMid, tmp);
        }
    }

    Enemy *next = get_movable_enemy(mlfq->priorHigh);
    if (next == NULL)
        next = get_movable_enemy(mlfq->priorMid);
    if (next == NULL)
        next = get_movable_enemy(mlfq->priorLow);

    if (next != NULL)
    {
        next->time_moving += next->movement_speed;
    }

    return next;
}

int getMLFQSize(MLFQ *mlfq)
{
    return mlfq->priorHigh->len + mlfq->priorMid->len + mlfq->priorLow->len;
}

#endif