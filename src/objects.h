voxel_Thing* voxel_newObject(voxel_Context* context) {
    voxel_Object* object = (voxel_Object*)VOXEL_MALLOC(sizeof(voxel_Object)); VOXEL_TAG_MALLOC(voxel_Object);

    object->length = 0;
    object->firstItem = VOXEL_NULL;
    object->lastItem = VOXEL_NULL;
    object->prototypes = VOXEL_NULL;

    voxel_Thing* thing = voxel_newThing(context); VOXEL_TAG_NEW_THING(VOXEL_TYPE_OBJECT);

    thing->type = VOXEL_TYPE_OBJECT;
    thing->value = object;

    return thing;
}

VOXEL_ERRORABLE voxel_destroyObject(voxel_Context* context, voxel_Thing* thing) {
    VOXEL_TAG_DESTROY_THING(VOXEL_TYPE_OBJECT);

    voxel_Object* object = (voxel_Object*)thing->value;
    voxel_ObjectItem* currentItem = object->firstItem;
    voxel_ObjectItem* nextItem;

    while (currentItem) {
        voxel_ObjectItemDescriptor* descriptor = currentItem->descriptor;

        VOXEL_MUST(voxel_unreferenceThing(context, currentItem->key));
        VOXEL_MUST(voxel_unreferenceThing(context, currentItem->value));

        if (descriptor) {
            VOXEL_FREE(descriptor); VOXEL_TAG_FREE(voxel_ObjectItemDescriptor);
        }

        nextItem = currentItem->nextItem;

        VOXEL_FREE(currentItem); VOXEL_TAG_FREE(voxel_ObjectItem);

        currentItem = nextItem;
    }

    if (object->prototypes) {
        VOXEL_MUST(voxel_unreferenceThing(context, object->prototypes));
    }

    VOXEL_FREE(object); VOXEL_TAG_FREE(voxel_Object);
    VOXEL_FREE(thing); VOXEL_TAG_FREE(voxel_Thing);

    return VOXEL_OK;
}

voxel_Bool voxel_compareObjects(voxel_Thing* a, voxel_Thing* b) {
    voxel_Object* aObject = (voxel_Object*)a->value;
    voxel_Object* bObject = (voxel_Object*)b->value;

    // First check that `b` contains all values that `a` has

    voxel_ObjectItem* currentItem = aObject->firstItem;

    while (currentItem) {
        voxel_ObjectItem* objectItem = voxel_getObjectItem(b, currentItem->key);
        voxel_Bool bothAreImplicitlyNull = currentItem->value->type == VOXEL_TYPE_NULL && !objectItem;

        if (!bothAreImplicitlyNull) {
            if (!voxel_compareThings(currentItem->value, objectItem->value)) {
                return VOXEL_FALSE;
            }
        }

        currentItem = currentItem->nextItem;
    }

    // Now check in opposite direction to ensure that `b` doesn't have extra items that `a` doesn't have

    currentItem = bObject->firstItem;

    while (currentItem) {
        voxel_ObjectItem* objectItem = voxel_getObjectItem(a, currentItem->key);
        voxel_Bool bothAreImplicitlyNull = currentItem->value->type == VOXEL_TYPE_NULL && !objectItem;

        if (!bothAreImplicitlyNull) {
            if (!voxel_compareThings(currentItem->value, objectItem->value)) {
                return VOXEL_FALSE;
            }
        }

        currentItem = currentItem->nextItem;
    }

    // TODO: Compare prototypes

    return VOXEL_TRUE;
}

void voxel_lockObject(voxel_Thing* thing) {
    voxel_Object* object = (voxel_Object*)thing->value;
    voxel_ObjectItem* currentItem = object->firstItem;

    while (currentItem) {
        voxel_lockThing(currentItem->value);

        currentItem = currentItem->nextItem;
    }

    if (object->prototypes) {
        voxel_lockThing(object->prototypes);
    }
}

voxel_Thing* voxel_copyObject(voxel_Context* context, voxel_Thing* thing) {
    // Shallow copy only; deep copying will be handled in the Voxel standard library

    voxel_Object* object = (voxel_Object*)thing->value;
    voxel_Thing* newObject = voxel_newObject(context);
    voxel_ObjectItem* currentItem = object->firstItem;

    while (currentItem) {
        voxel_setObjectItem(context, newObject, currentItem->key, currentItem->value);

        currentItem = currentItem->nextItem;
    }

    return newObject;
}

VOXEL_ERRORABLE voxel_objectToVxon(voxel_Context* context, voxel_Thing* thing) {
    voxel_Object* object = (voxel_Object*)thing->value;
    voxel_ObjectItem* currentItem = object->firstItem;
    voxel_Thing* string = voxel_newStringTerminated(context, "{");

    while (currentItem) {
        // TODO: Prevent circular references from hanging
        VOXEL_ERRORABLE keyString = voxel_thingToVxon(context, currentItem->key); VOXEL_MUST(keyString);
        VOXEL_ERRORABLE valueString = voxel_thingToVxon(context, currentItem->value); VOXEL_MUST(valueString);

        VOXEL_MUST(voxel_appendToString(context, string, (voxel_Thing*)keyString.value));
        VOXEL_MUST(voxel_appendByteToString(context, string, ':'));
        VOXEL_MUST(voxel_appendToString(context, string, (voxel_Thing*)valueString.value));

        currentItem = currentItem->nextItem;

        if (currentItem) {
            VOXEL_MUST(voxel_appendByteToString(context, string, ','));
        }

        VOXEL_MUST(voxel_unreferenceThing(context, (voxel_Thing*)keyString.value));
        VOXEL_MUST(voxel_unreferenceThing(context, (voxel_Thing*)valueString.value));
    }

    VOXEL_MUST(voxel_appendByteToString(context, string, '}'));

    return VOXEL_OK_RET(string);
}

voxel_Bool voxel_objectIsTruthy(voxel_Thing* thing) {
    voxel_Object* object = (voxel_Object*)thing->value;

    return voxel_getObjectLength(thing) != 0;
}

voxel_ObjectItem* voxel_getPrototypedObjectItem(voxel_Thing* thing, voxel_Thing* key, voxel_Count traverseDepth, voxel_Count* actualTraverseDepth, voxel_Thing** actualParentObject) {
    voxel_Object* object = (voxel_Object*)thing->value;
    voxel_ObjectItem* currentItem = object->firstItem;

    while (VOXEL_TRUE) {
        if (!currentItem) {
            if (traverseDepth == 0) {
                return VOXEL_NULL;
            }

            voxel_Thing* prototypesThing = object->prototypes;

            if (!prototypesThing) {
                return VOXEL_NULL;
            }

            voxel_List* prototypesList = (voxel_List*)prototypesThing->value;
            voxel_ListItem* currentPrototypeListItem = prototypesList->lastItem;

            while (currentPrototypeListItem) {
                voxel_Thing* currentPrototype = currentPrototypeListItem->value;
                voxel_ObjectItem* prototypeObjectItem = voxel_getPrototypedObjectItem(currentPrototype, key, traverseDepth - 1, actualTraverseDepth, actualParentObject);

                if (prototypeObjectItem) {
                    if (actualTraverseDepth) {
                        (*actualTraverseDepth)++;
                    }

                    if (actualParentObject) {
                        *actualParentObject = currentPrototype;
                    }

                    return prototypeObjectItem;
                }

                currentPrototypeListItem = currentPrototypeListItem->previousItem;
            }

            return VOXEL_NULL;
        }

        if (voxel_compareThings(currentItem->key, key)) {
            if (actualParentObject) {
                *actualParentObject = thing;
            }

            return currentItem;
        }

        currentItem = currentItem->nextItem;
    }
}

voxel_ObjectItem* voxel_getObjectItem(voxel_Thing* thing, voxel_Thing* key) {
    return voxel_getPrototypedObjectItem(thing, key, VOXEL_MAX_PROTOTYPE_TRAVERSE_DEPTH, VOXEL_NULL, VOXEL_NULL);
}

VOXEL_ERRORABLE voxel_setObjectItem(voxel_Context* context, voxel_Thing* thing, voxel_Thing* key, voxel_Thing* value) {
    VOXEL_ASSERT(!thing->isLocked, VOXEL_ERROR_THING_LOCKED);

    voxel_Count actualTraverseDepth = 0;
    voxel_Thing* actualParentObject = VOXEL_NULL;
    voxel_Object* object = (voxel_Object*)thing->value;
    voxel_ObjectItem* objectItem = voxel_getPrototypedObjectItem(thing, key, VOXEL_MAX_PROTOTYPE_TRAVERSE_DEPTH, &actualTraverseDepth, &actualParentObject);

    if (objectItem) {
        if (actualTraverseDepth == 0) {
            VOXEL_MUST(voxel_unreferenceThing(context, objectItem->value));

            objectItem->value = value;
            value->referenceCount++;

            return VOXEL_OK_RET(objectItem);
        }

        if (actualParentObject->isLocked) {
            VOXEL_THROW(VOXEL_ERROR_THING_LOCKED);
        }

        key = objectItem->key;
    } else {
        voxel_lockThing(key);
    }

    objectItem = (voxel_ObjectItem*)VOXEL_MALLOC(sizeof(voxel_ObjectItem)); VOXEL_TAG_MALLOC(voxel_ObjectItem);

    objectItem->key = key;
    key->referenceCount++;

    objectItem->value = value;
    value->referenceCount++;

    objectItem->descriptor = VOXEL_NULL;
    objectItem->nextItem = VOXEL_NULL;

    if (!object->firstItem) {
        object->firstItem = objectItem;
    }

    if (object->lastItem) {
        object->lastItem->nextItem = objectItem;
    }

    object->length++;
    object->lastItem = objectItem;

    return VOXEL_OK_RET(objectItem);
}

VOXEL_ERRORABLE voxel_removeObjectItem(voxel_Context* context, voxel_Thing* thing, voxel_Thing* key) {
    VOXEL_ASSERT(!thing->isLocked, VOXEL_ERROR_THING_LOCKED);

    voxel_Object* object = (voxel_Object*)thing->value;
    voxel_ObjectItem* currentItem = object->firstItem;
    voxel_ObjectItem* previousItem = VOXEL_NULL;

    while (VOXEL_TRUE) {
        if (!currentItem) {
            return VOXEL_OK;
        }

        if (voxel_compareThings(currentItem->key, key)) {
            if (currentItem == object->firstItem) {
                object->firstItem = currentItem->nextItem;
            } else {
                previousItem->nextItem = currentItem->nextItem;
            }

            if (currentItem == object->lastItem) {
                object->lastItem = previousItem;
            }

            VOXEL_MUST(voxel_unreferenceThing(context, currentItem->key));
            VOXEL_MUST(voxel_unreferenceThing(context, currentItem->value));

            VOXEL_FREE(currentItem); VOXEL_TAG_FREE(voxel_ObjectItem);

            object->length--;

            return VOXEL_OK;
        }

        previousItem = currentItem;
        currentItem = currentItem->nextItem;
    }
}

voxel_ObjectItemDescriptor* voxel_ensureObjectItemDescriptor(voxel_Context* context, voxel_ObjectItem* objectItem) {
    if (objectItem->descriptor) {
        return objectItem->descriptor;
    }

    voxel_ObjectItemDescriptor* descriptor = (voxel_ObjectItemDescriptor*)VOXEL_MALLOC(sizeof(voxel_ObjectItemDescriptor)); VOXEL_TAG_MALLOC(voxel_ObjectItemDescriptor);

    descriptor->getterFunction = VOXEL_NULL;
    descriptor->setterFunction = VOXEL_NULL;

    objectItem->descriptor = descriptor;

    return descriptor;
}

VOXEL_ERRORABLE voxel_addPrototypedObjectKeys(voxel_Context* context, voxel_Thing* thing, voxel_Thing* list, voxel_Count traverseDepth) {
    voxel_Object* object = (voxel_Object*)thing->value;
    voxel_ObjectItem* currentItem = object->firstItem;

    while (currentItem) {
        voxel_List* listValue = (voxel_List*)list->value;
        voxel_ListItem* currentListItem = listValue->firstItem;
        voxel_Bool shouldPush = VOXEL_TRUE;

        while (currentListItem) {
            if (voxel_compareThings(currentListItem->value, currentItem->key)) {
                shouldPush = VOXEL_FALSE;

                break;
            }

            currentListItem = currentListItem->nextItem;
        }

        if (shouldPush) {
            VOXEL_MUST(voxel_pushOntoList(context, list, currentItem->key));
        }

        currentItem = currentItem->nextItem;
    }

    if (traverseDepth == 0) {
        return VOXEL_OK;
    }

    voxel_Thing* prototypesThing = object->prototypes;

    if (!prototypesThing) {
        return VOXEL_OK;
    }

    voxel_List* prototypesList = (voxel_List*)prototypesThing->value;
    voxel_ListItem* currentPrototypeListItem = prototypesList->lastItem;

    while (currentPrototypeListItem) {
        VOXEL_MUST(voxel_addPrototypedObjectKeys(context, currentPrototypeListItem->value, list, traverseDepth - 1));

        currentPrototypeListItem = currentPrototypeListItem->previousItem;
    }

    return VOXEL_OK;
}

VOXEL_ERRORABLE voxel_getObjectKeys(voxel_Context* context, voxel_Thing* thing, voxel_Count traverseDepth) {
    voxel_Thing* list = voxel_newList(context);

    voxel_addPrototypedObjectKeys(context, thing, list, traverseDepth);

    list->isLocked = VOXEL_TRUE; // Shallow only

    return VOXEL_OK_RET(list);
}

voxel_Count voxel_getObjectLength(voxel_Thing* thing) {
    voxel_Object* object = (voxel_Object*)thing->value;

    return object->length;
}

voxel_Thing* voxel_getObjectPrototypes(voxel_Context* context, voxel_Thing* thing) {
    voxel_Object* object = (voxel_Object*)thing->value;

    if (object->prototypes) {
        return object->prototypes;
    }

    voxel_Thing* prototypes = voxel_newList(context);

    if (thing->isLocked) {
        voxel_lockThing(prototypes);
    }

    object->prototypes = prototypes;

    return prototypes;
}

voxel_Bool voxel_checkWhetherObjectInherits(voxel_Thing* thing, voxel_Thing* target, voxel_Count traverseDepth) {
    voxel_Object* object = (voxel_Object*)thing->value;

    if (!object->prototypes) {
        return VOXEL_FALSE;
    }

    voxel_List* prototypesList = (voxel_List*)object->prototypes->value;
    voxel_ListItem* currentItem = prototypesList->firstItem;

    while (currentItem) {
        voxel_Thing* currentPrototype = currentItem->value;

        if (currentPrototype == target) {
            return VOXEL_TRUE;
        }

        if (traverseDepth > 0) {
            return voxel_checkWhetherObjectInherits(currentPrototype, target, traverseDepth - 1);
        }

        currentItem = currentItem->nextItem;
    }

    return VOXEL_FALSE;
}