#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define OOM_EXIT_CODE 93
#define NAMEOF(x) #x

void IntMallocDemo()
{
    printf("Let's try to allocate just one byte for integers.\n");

    // This isn't safe for a pretty obvious reason: we try to write
    // 4 bytes into a 1 byte space, which is basically just asking
    // for segmentation faults. But will we lose data?
    // It's more than possible. We'd have to be pretty lucky to get
    // these next to each other in memory though. So we'll force that
    // by simulating it in another test.
    int* p1 = (int*) malloc(1);

    // Make sure we could allocate that one byte
    if (p1 == NULL)
    {
        fprintf(stderr, "Out of memory.\n");
        exit(OOM_EXIT_CODE);
    }

    int* p2 = (int*) malloc(1);

    // And check one more time
    if (p2 == NULL)
    {
        fprintf(stderr, "Out of memory.\n");
        exit(OOM_EXIT_CODE);
    }

    printf("Address of " NAMEOF(p1) ": %p\n", p1);
    printf("Address of " NAMEOF(p2) ": %p\n", p2);

    // Because C pointers are "smart" (in the sense that they increment
    // by a multiple of the referenced type), we'll have to cast around
    // to test if the pointers magically landed next to each other.
    if (((char*) p1) + 1 == (char*) p2)
    {
        printf(NAMEOF(p1) " and " NAMEOF(p2) " magically landed next to each other!\n");
    }
    else
    {
        printf(NAMEOF(p1) " and " NAMEOF(p2) " are not immediately next to each other.\n");
    }

    free(p1);
    p1 = NULL;

    free(p2);
    p2 = NULL;
}

struct Object
{
    // We'll use unsigned integers so we can spell fun words with
    // hex literals without signed/unsigned conversion.
    uint32_t Field1;
    uint32_t Field2;
};

void ObjectMallocDemo()
{
    printf("Let's try allocating the wrong size when using\n"
           "a struct, guaranteeing that the objects are next\n"
           "to each other in memory.\n\n");

    printf("Let's \"fake it 'till we make it\". We'll first allocate an array\n"
           "with enough space to store an entire struct Object (and a half). This is\n"
           "completely legal. But then we'll split this in half to simulate\n"
           "what it'd be like if we only allocated half the space necessary\n"
           "for the entire struct.\n");

    printf("We'll allocate %lu bytes for the array.\n", sizeof(struct Object));
    char* memForObject = (char*) malloc(sizeof(struct Object));

    // Make sure we were able to allocate.
    if (memForObject == NULL)
    {
        fprintf(stderr, "Out of memory.\n");
        exit(OOM_EXIT_CODE);
    }

    printf("Now that we have enough space for an entire object, let's break it in half.\n");
    struct Object* p1 = (struct Object*) memForObject;
    const size_t halfway = sizeof(struct Object) / 2;
    struct Object* p2 = (struct Object*) (memForObject + halfway);

    printf("Address of " NAMEOF(p1) ": %p\n", p1);
    printf("Address of " NAMEOF(p2) ": %p\n", p2);

    printf("Initialize fields on " NAMEOF(p1) ": \n"
           " > " NAMEOF(p1->Field1) ": 0x12341234\n"
           " > " NAMEOF(p1->Field2) ": 0x56785678\n");

    p1->Field1 = 0x12341234;
    p1->Field2 = 0x56785678;

    printf("\n");
    printf("Inspect values of " NAMEOF(p1) ":\n");
    printf(" > " NAMEOF(p1->Field1) ": 0x%x\n", p1->Field1);
    printf(" > " NAMEOF(p1->Field2) ": 0x%x\n", p1->Field2);

    printf("Now for the dangerous part: we'll poke the fields on " NAMEOF(p2) ".\n"
           "Initialize fields on " NAMEOF(p2) ": \n"
           " > " NAMEOF(p2->Field1) ": 0xdeadbeef\n"
           " > " NAMEOF(p2->Field2) ": 0x8badf00d\n");

    p2->Field1 = 0xDEADBEEF;
    p2->Field2 = 0x8BADF00D;

    printf("\nInspect values of " NAMEOF(p2) ":\n");
    printf(" > " NAMEOF(p2->Field1) ": 0x%x\n", p2->Field1);
    printf(" > " NAMEOF(p2->Field2) ": 0x%x\n", p2->Field2);

    printf("\nGreat, looks like everything on " NAMEOF(p2) " is set properly.\n"
           "Let's double-check everything to make sure everything's in order.\n");
    printf(" > " NAMEOF(p1->Field1) ": 0x%x\n", p1->Field1);
    printf(" > " NAMEOF(p1->Field2) ": 0x%x\n", p1->Field2);
    printf(" > " NAMEOF(p2->Field1) ": 0x%x\n", p2->Field1);
    printf(" > " NAMEOF(p2->Field2) ": 0x%x\n", p2->Field2);

    printf("\nWell that can't be right...\n"
           "Let's poke " NAMEOF(p1) " again...\n"
           " > Set " NAMEOF(p1->Field2) " to 0xfeedc0de\n");

    p1->Field2 = 0xfeedc0de;

    printf("\nNow let's look again...\n");
    printf(" > " NAMEOF(p1->Field1) ": 0x%x\n", p1->Field1);
    printf(" > " NAMEOF(p1->Field2) ": 0x%x\n", p1->Field2);
    printf(" > " NAMEOF(p2->Field1) ": 0x%x\n", p2->Field1);
    printf(" > " NAMEOF(p2->Field2) ": 0x%x\n", p2->Field2);

    printf("\nLet's look at a memory layout (using dummy addresses).\n");
    printf(
            "/-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----\\\n"
            "| 0x1 | 0x2 | 0x3 | 0x4 | 0x5 | 0x6 | 0x7 | 0x8 | 0x9 | 0xA | 0xB | 0xC |\n"
            "|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|\n"
            "|                  <--  p1  -->                 |                       |\n"
            "|                       |                  <--  p2  -->                 |\n"
            "\\-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----/\n"
    );
    printf("\nIn this diagram, p1 and p2 are laid on top of each other.\n");
    printf("Let's look at where each field is.\n");
    printf(
            "/-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----\\\n"
            "| 0x1 | 0x2 | 0x3 | 0x4 | 0x5 | 0x6 | 0x7 | 0x8 | 0x9 | 0xA | 0xB | 0xC |\n"
            "|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|\n"
            "|      p1->Field1       |      p1->Field2       |                       |\n"
            "|                       |      p2->Field1       |      p2->Field2       |\n"
            "\\-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----/\n"
    );
    printf("\np1->Field2 and p2->Field1 share the same memory.\n");
    printf("Any changes to p1->Field2 are reflected in p2->Field1 and vice versa.\n");

    // Danger: p1 and p2 can't be trusted as pointers to
    // the actual memory we allocated with malloc. We have
    // to free the pointer given to us by malloc to be as
    // safe as possible.
    free(memForObject);
    memForObject = NULL;
    p1 = NULL;
    p2 = NULL;
}

int main(int argc, char** argv)
{
    // This demo with integers could work, but it's quite unreliable.
    // It trusts that they land right next to each other in order to
    // have a visible effect. But it does have the obvious issue of
    // trying to write 4 bytes into a 1 byte space - a classic buffer
    // overflow type problem.
    //
    // IntMallocDemo();
    // printf("\n====================================================\n\n");

    ObjectMallocDemo();

    return 0;
}
