#pragma once

#include <stdint-gcc.h>
#include <stddef.h>

typedef struct {
    void *value;
    size_t length;
    uint16_t references;
} SmartPointer;

#define USE(type, name, scope) \
do { \
    type *name = (type*) _##name->value; \
    _##name->references++; \
    do scope while(0); \
    _##name->references--; \
    } while (0);

#define MALLOC(type, name, value_length, scope) \
do { \
    SmartPointer *_##name = malloc(sizeof(SmartPointer) + value_length); \
    _##name->value = _##name + sizeof(SmartPointer); \
    _##name->length = value_length; \
    _##name->references = 0; \
    USE(type, name, scope) \
    SMART_FREE(name)\
} while (0);

#define ASSIGN(to, name) \
_##name->references++; \
_##to = _##name;

#define ABANDON(from) \
SMART_FREE(from) \
_##from = NULL;

#define SMART_FREE(name) \
if (--_##name->references == 0) free(_##name);

#define PASS(name) _##name

#define SMART_POINTER(name) SmartPointer *_##name


// EXAMPLES

SMART_POINTER(some_global);

void test2(SMART_POINTER(ptr)) {
    USE(int, ptr, {
        *ptr = 8;

        ASSIGN(some_global, ptr)
    })
}

void test() {
    MALLOC(int, pointer, 10, {
        *pointer = 10;
        test2(PASS(pointer));
    })

    ABANDON(some_global)
}