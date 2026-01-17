#include <stdio.h>

#define Directions(Direction_Definition) \
    Direction_Definition(South) \
    Direction_Definition(North) \
    Direction_Definition(East) \
    Direction_Definition(West)

#define Direction_Enum(suffix) Direction_##suffix,

#define Direction_String(suffix) [Direction_##suffix] = #suffix,

typedef enum {
    Directions(Direction_Enum)
} Direction;

const char *direction_names[] = {
    Directions(Direction_String)
};

int main(int argc, char* argv[]) {
    Direction dir = Direction_North;

    printf("Direction: %s\n", direction_names[dir]);

    return 0;
}
