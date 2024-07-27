#define _VOXEL_HELPER_POP_VALUE(name, type, popCall, getValueCall, defaultValue) type name(voxel_Executor* executor) { \
        voxel_Thing* thing = popCall(executor); \
\
        if (!thing) { \
            return (defaultValue); \
        } \
\
        type result = getValueCall(thing); \
\
        voxel_unreferenceThing(executor->context, thing); \
\
        return result; \
    }

void voxel_push(voxel_Executor* executor, voxel_Thing* thing) {
    if (thing == VOXEL_NULL) {
        thing = voxel_newNull(executor->context);
    }

    voxel_pushOntoList(executor->context, executor->valueStack, thing);
}

void voxel_pushNull(voxel_Executor* executor) {
    voxel_push(executor, voxel_newNull(executor->context));
}

voxel_Thing* voxel_pop(voxel_Executor* executor) {
    VOXEL_ERRORABLE result = voxel_popFromList(executor->context, executor->valueStack);

    if (VOXEL_IS_ERROR(result)) {
        return VOXEL_NULL;
    }

    return (voxel_Thing*)result.value;
}

void voxel_popVoid(voxel_Executor* executor) {
    voxel_unreferenceThing(executor->context, voxel_pop(executor));
}

voxel_Bool voxel_popBoolean(voxel_Executor* executor) {
    voxel_Thing* poppedThing = voxel_pop(executor);

    if (!poppedThing) {
        return VOXEL_NULL;
    }

    voxel_Bool isTruthy = voxel_thingIsTruthy(poppedThing);

    voxel_unreferenceThing(executor->context, poppedThing);

    return isTruthy;
}

voxel_Thing* voxel_popByte(voxel_Executor* executor) {
    voxel_Thing* poppedThing = voxel_pop(executor);

    if (!poppedThing) {
        return VOXEL_NULL;
    }

    if (poppedThing->type == VOXEL_TYPE_BYTE) {
        return poppedThing; // Saves us from having to copy the thing
    }

    VOXEL_ERRORABLE result = voxel_thingToByte(executor->context, poppedThing);

    voxel_unreferenceThing(executor->context, poppedThing);

    if (VOXEL_IS_ERROR(result)) {
        return VOXEL_NULL;
    }

    return (voxel_Thing*)result.value;
}

_VOXEL_HELPER_POP_VALUE(voxel_popByteValue, voxel_Byte, voxel_popByte, voxel_getByte, 0);

voxel_Thing* voxel_popNumber(voxel_Executor* executor) {
    voxel_Thing* poppedThing = voxel_pop(executor);

    if (!poppedThing) {
        return VOXEL_NULL;
    }

    if (poppedThing->type == VOXEL_TYPE_NUMBER) {
        return poppedThing; // Saves us from having to copy the thing
    }

    VOXEL_ERRORABLE result = voxel_thingToNumber(executor->context, poppedThing);

    voxel_unreferenceThing(executor->context, poppedThing);

    if (VOXEL_IS_ERROR(result)) {
        return VOXEL_NULL;
    }

    return (voxel_Thing*)result.value;
}

_VOXEL_HELPER_POP_VALUE(voxel_popNumberInt, voxel_Int, voxel_popNumber, voxel_getNumberInt, 0);
_VOXEL_HELPER_POP_VALUE(voxel_popNumberFloat, voxel_Float, voxel_popNumber, voxel_getNumberFloat, 0);

voxel_Thing* voxel_popString(voxel_Executor* executor) {
    voxel_Thing* poppedThing = voxel_pop(executor);

    if (!poppedThing) {
        return VOXEL_NULL;
    }

    if (poppedThing->type == VOXEL_TYPE_STRING) {
        return poppedThing; // Saves us from having to copy the thing
    }

    VOXEL_ERRORABLE result = voxel_thingToString(executor->context, poppedThing);

    voxel_unreferenceThing(executor->context, poppedThing);

    if (VOXEL_IS_ERROR(result)) {
        return VOXEL_NULL;
    }

    return (voxel_Thing*)result.value;
}

voxel_Thing* voxel_peek(voxel_Executor* executor, voxel_Int index) {
    voxel_Thing* stack = executor->valueStack;

    index = voxel_getListLength(stack) - index - 1;

    if (index < 0) {
        return VOXEL_NULL;
    }

    VOXEL_ERRORABLE listItemResult = voxel_getListItem(executor->context, stack, index);

    if (VOXEL_IS_ERROR(listItemResult)) {
        return VOXEL_NULL;
    }

    voxel_ListItem* listItem = (voxel_ListItem*)listItemResult.value;

    return listItem->value;
}