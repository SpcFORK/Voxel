#ifndef LIBVOXEL_H_
#define LIBVOXEL_H_

// src/context.h

typedef struct voxel_Context {
    struct voxel_Thing* firstTrackedThing;
    struct voxel_Thing* lastTrackedThing;
} voxel_Context;

voxel_Context* voxel_newContext() {
    voxel_Context* context = VOXEL_MALLOC(sizeof(voxel_Context));

    context->firstTrackedThing = NULL;
    context->lastTrackedThing = NULL;

    return context;
}

// src/things.h

typedef enum {
    VOXEL_TYPE_NULL,
    VOXEL_TYPE_POSITION,
    VOXEL_TYPE_BYTE,
    VOXEL_TYPE_NATIVE_FUNCTION
} voxel_DataType;

typedef struct voxel_Thing {
    voxel_DataType type;
    void* valuePtr;
    VOXEL_COUNT referenceCount;
    struct voxel_Thing* previousTrackedThing;
    struct voxel_Thing* nextTrackedThing;
} voxel_Thing;

void voxel_unreferenceThing(voxel_Context* context, voxel_Thing* thing) {
    if (thing->referenceCount > 0) {
        thing->referenceCount--;

        return;
    }

    if (thing == context->firstTrackedThing) {
        context->firstTrackedThing = thing->nextTrackedThing;
    }

    if (thing == context->lastTrackedThing) {
        context->lastTrackedThing = thing->previousTrackedThing;
    }

    if (thing->previousTrackedThing) {
        thing->previousTrackedThing->nextTrackedThing = thing->nextTrackedThing;
    }

    switch (thing->type) {
        case VOXEL_TYPE_NULL:
            break;

        default:
            VOXEL_FREE(thing->valuePtr);
    }

    VOXEL_FREE(thing);
}

void voxel_removeUnusedThings(voxel_Context* context) {
    voxel_Thing* currentThing = context->firstTrackedThing;

    while (currentThing != NULL) {
        voxel_Thing* nextThing = currentThing->nextTrackedThing;

        if (currentThing->referenceCount == 0) {
            voxel_unreferenceThing(context, currentThing);
        }

        currentThing = nextThing;
    }
}

voxel_Thing* voxel_newThing(voxel_Context* context) {
    voxel_Thing* thing = VOXEL_MALLOC(sizeof(voxel_Thing));

    thing->type = VOXEL_TYPE_NULL;
    thing->valuePtr = NULL;
    thing->referenceCount = 1;
    thing->previousTrackedThing = context->lastTrackedThing;
    thing->nextTrackedThing = NULL;

    if (context->lastTrackedThing) {
        context->lastTrackedThing->nextTrackedThing = thing;
    }

    context->lastTrackedThing = thing;

    if (!context->firstTrackedThing) {
        context->firstTrackedThing = thing;
    }

    return thing;
}

voxel_Thing* voxel_newNull(voxel_Context* context) {
    return voxel_newThing(context);
}

// src/voxel.h

void voxel_test() {
    VOXEL_LOG("Hello from Voxel!\n");
}

#endif
