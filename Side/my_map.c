#include <stdio.h>
#include <cstdlib>

struct Map;

typedef struct
{
    char *key;
    char *value;
    struct Map *next;
} MapElem;

typedef struct Map
{
    MapElem *current;
} Map;

int main()
{
    Map map;
    map.current = (MapElem*)(malloc(sizeof(MapElem)));

    map.current->key = (char*)(malloc(sizeof("Pesho") + 1));
    map.current->key = "Pesho\0";
    map.current->value = (char*)(malloc(sizeof("19 years") + 1));
    map.current->value = "19 years\0";

    map.current->next = (Map*)malloc(sizeof(Map));
    map.current->next->current = (MapElem*)(malloc(sizeof(MapElem)));

    map.current->next->current->key = (char*)(malloc(sizeof("Gosho") + 1));
    map.current->next->current->key = "Gosho\0";
    map.current->next->current->value = (char*)(malloc(sizeof("20 years") + 1));
    map.current->next->current->value = "20 years\0";

    printf("%s %s\n", map.current->key, map.current->value);
    printf("%s %s\n", map.current->next->current->key, map.current->next->current->value);

    free(map.current->next->current->value);
    free(map.current->next->current->key);
    free(map.current->next->current);
    free(map.current->next);
    free(map.current->value);
    free(map.current->key);
    free(map.current);
}