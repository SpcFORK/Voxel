typedef struct voxel_Result {
    voxel_ErrorCode errorCode;
    void* value;
} voxel_Result;

typedef struct voxel_Context {
    char* code;
    voxel_Count codeLength;
    struct voxel_Thing* firstTrackedThing;
    struct voxel_Thing* lastTrackedThing;
} voxel_Context;

typedef enum {
    VOXEL_TYPE_NULL,
    VOXEL_TYPE_BOOLEAN,
    VOXEL_TYPE_BYTE,
    VOXEL_TYPE_FUNCTION,
    VOXEL_TYPE_NUMBER,
    VOXEL_TYPE_BUFFER,
    VOXEL_TYPE_STRING,
    VOXEL_TYPE_OBJECT,
    VOXEL_TYPE_LIST
} voxel_DataType;

typedef struct voxel_Thing {
    voxel_DataType type;
    void* value;
    voxel_Count referenceCount;
    voxel_Bool isLocked;
    struct voxel_Thing* previousTrackedThing;
    struct voxel_Thing* nextTrackedThing;
} voxel_Thing;

typedef enum {
    VOXEL_FUNCTION_TYPE_BUILTIN,
    VOXEL_FUNCTION_TYPE_POS_REF
} voxel_FunctionType;

typedef enum {
    VOXEL_NUMBER_TYPE_INT,
    VOXEL_NUMBER_TYPE_FLOAT
} voxel_NumberType;

typedef struct voxel_Number {
    voxel_NumberType type;
    union {
        voxel_Int asInt;
        voxel_Float asFloat;
    } value;
} voxel_Number;

typedef struct voxel_Buffer {
    voxel_Count size;
    voxel_Byte* value;
} voxel_Buffer;

typedef struct voxel_String {
    voxel_Count size;
    voxel_Byte* value;
} voxel_String;

typedef struct voxel_Object {
    voxel_Count length;
    struct voxel_ObjectItem* firstItem;
    struct voxel_ObjectItem* lastItem;
} voxel_Object;

typedef struct voxel_ObjectItem {
    voxel_Thing* key;
    voxel_Thing* value;
    struct voxel_ObjectItem* nextItem;
} voxel_ObjectItem;

typedef struct voxel_List {
    voxel_Count length;
    struct voxel_ListItem* firstItem;
    struct voxel_ListItem* lastItem;
} voxel_List;

typedef struct voxel_ListItem {
    voxel_Thing* value;
    struct voxel_ListItem* previousItem;
    struct voxel_ListItem* nextItem;
} voxel_ListItem;

typedef enum voxel_TokenType {
    VOXEL_TOKEN_TYPE_NULL = 'n',
    VOXEL_TOKEN_TYPE_BOOLEAN_TRUE = 't',
    VOXEL_TOKEN_TYPE_BOOLEAN_FALSE = 'f',
    VOXEL_TOKEN_TYPE_BYTE = 'b',
    VOXEL_TOKEN_TYPE_NUMBER_INT_8 = '3',
    VOXEL_TOKEN_TYPE_NUMBER_INT_16 = '4',
    VOXEL_TOKEN_TYPE_NUMBER_INT_32 = '5',
    VOXEL_TOKEN_TYPE_NUMBER_FLOAT = '%',
    VOXEL_TOKEN_TYPE_BUFFER = 'B',
    VOXEL_TOKEN_TYPE_BUFFER_EMPTY = 'E',
    VOXEL_TOKEN_TYPE_STRING = '$',
    VOXEL_TOKEN_TYPE_CALL = '!',
    VOXEL_TOKEN_TYPE_RESOLVE_BUILTIN = '/'
} voxel_TokenType;

typedef struct voxel_Token {
    voxel_TokenType type;
    void* data;
} voxel_Token;

void voxel_copy(voxel_Byte* source, voxel_Byte* destination, voxel_Count size);
voxel_Bool voxel_compare(voxel_Byte* a, voxel_Byte* b, voxel_Count aSize, voxel_Count bSize);

const voxel_Byte* voxel_lookupError(voxel_ErrorCode error);

voxel_Float voxel_maths_power(voxel_Float base, voxel_Int power);
voxel_Float voxel_maths_roundToPrecision(voxel_Float number, voxel_Count precision);

voxel_Context* voxel_newContext();

voxel_Thing* voxel_newThing(voxel_Context* context);
VOXEL_ERRORABLE voxel_destroyThing(voxel_Context* context, voxel_Thing* thing);
VOXEL_ERRORABLE voxel_unreferenceThing(voxel_Context* context, voxel_Thing* thing);
VOXEL_ERRORABLE voxel_removeUnusedThings(voxel_Context* context);
voxel_Bool voxel_compareThingTypes(voxel_Thing* a, voxel_Thing* b);
voxel_Bool voxel_compareThings(voxel_Thing* a, voxel_Thing* b);
void voxel_lockThing(voxel_Thing* thing);
voxel_Thing* voxel_copyThing(voxel_Context* context, voxel_Thing* thing);
VOXEL_ERRORABLE voxel_thingToString(voxel_Context* context, voxel_Thing* thing);
VOXEL_ERRORABLE voxel_thingToVxon(voxel_Context* context, voxel_Thing* thing);

voxel_Thing* voxel_newNull(voxel_Context* context);
void voxel_destroyNull(voxel_Thing* thing);
voxel_Bool voxel_compareNulls(voxel_Thing* a, voxel_Thing* b);
voxel_Thing* voxel_copyNull(voxel_Context* context, voxel_Thing* thing);
VOXEL_ERRORABLE voxel_nullToString(voxel_Context* context, voxel_Thing* thing);

voxel_Thing* voxel_newBoolean(voxel_Context* context, voxel_Bool value);
void voxel_destroyBoolean(voxel_Thing* thing);
voxel_Bool voxel_compareBooleans(voxel_Thing* a, voxel_Thing* b);
voxel_Thing* voxel_copyBoolean(voxel_Context* context, voxel_Thing* thing);
VOXEL_ERRORABLE voxel_booleanToString(voxel_Context* context, voxel_Thing* thing);

voxel_Thing* voxel_newByte(voxel_Context* context, voxel_Byte value);
void voxel_destroyByte(voxel_Thing* thing);
voxel_Bool voxel_compareBytes(voxel_Thing* a, voxel_Thing* b);
voxel_Thing* voxel_copyByte(voxel_Context* context, voxel_Thing* thing);
voxel_Thing* voxel_byteToNumber(voxel_Context* context, voxel_Thing* thing);
VOXEL_ERRORABLE voxel_byteToString(voxel_Context* context, voxel_Thing* thing);
VOXEL_ERRORABLE voxel_byteToVxon(voxel_Context* context, voxel_Thing* thing);

voxel_Thing* voxel_newFunctionBuiltin(voxel_Context* context, voxel_Count builtinFunctionIndex);
voxel_Thing* voxel_newFunctionPosRef(voxel_Context* context, voxel_Count positionReference);
void voxel_destroyFunction(voxel_Thing* thing);
voxel_Bool voxel_compareFunctions(voxel_Thing* a, voxel_Thing* b);
voxel_Thing* voxel_copyFunction(voxel_Context* context, voxel_Thing* thing);
VOXEL_ERRORABLE voxel_functionToString(voxel_Context* context, voxel_Thing* thing);
voxel_FunctionType voxel_getFunctionType(voxel_Context* context, voxel_Thing* thing);

voxel_Thing* voxel_newNumberInt(voxel_Context* context, voxel_Int value);
voxel_Thing* voxel_newNumberFloat(voxel_Context* context, voxel_Float value);
voxel_Int voxel_getNumberInt(voxel_Thing* thing);
voxel_Float voxel_getNumberFloat(voxel_Thing* thing);
void voxel_destroyNumber(voxel_Thing* thing);
voxel_Bool voxel_compareNumbers(voxel_Thing* a, voxel_Thing* b);
voxel_Thing* voxel_copyNumber(voxel_Context* context, voxel_Thing* thing);
VOXEL_ERRORABLE voxel_numberToString(voxel_Context* context, voxel_Thing* thing);
VOXEL_ERRORABLE voxel_numberToBaseString(voxel_Context* context, voxel_Thing* thing, voxel_Count base, voxel_Count minSize);

voxel_Thing* voxel_newBuffer(voxel_Context* context, voxel_Count size, voxel_Byte* data);
void voxel_destroyBuffer(voxel_Thing* thing);
voxel_Bool voxel_compareBuffers(voxel_Thing* a, voxel_Thing* b);
voxel_Thing* voxel_copyBuffer(voxel_Context* context, voxel_Thing* thing);
VOXEL_ERRORABLE voxel_bufferToString(voxel_Context* context, voxel_Thing* thing);
VOXEL_ERRORABLE voxel_bufferToVxon(voxel_Context* context, voxel_Thing* thing);

voxel_Thing* voxel_newString(voxel_Context* context, voxel_Count size, voxel_Byte* data);
voxel_Thing* voxel_newStringTerminated(voxel_Context* context, voxel_Byte* data);
void voxel_destroyString(voxel_Thing* thing);
voxel_Bool voxel_compareStrings(voxel_Thing* a, voxel_Thing* b);
voxel_Thing* voxel_copyString(voxel_Context* context, voxel_Thing* thing);
VOXEL_ERRORABLE voxel_stringToVxon(voxel_Context* context, voxel_Thing* thing);
voxel_Count voxel_getStringSize(voxel_Thing* thing);
void voxel_logString(voxel_Thing* thing);
voxel_Thing* voxel_concatenateStrings(voxel_Context* context, voxel_Thing* a, voxel_Thing* b);
VOXEL_ERRORABLE voxel_appendToString(voxel_Context* context, voxel_Thing* a, voxel_Thing* b);
VOXEL_ERRORABLE voxel_appendToStringTerminatedBytes(voxel_Context* context, voxel_Thing* a, voxel_Byte* b);
VOXEL_ERRORABLE voxel_appendByteToString(voxel_Context* context, voxel_Thing* thing, voxel_Byte byte);
VOXEL_ERRORABLE voxel_reverseString(voxel_Context* context, voxel_Thing* thing);
VOXEL_ERRORABLE voxel_cutStringStart(voxel_Context* context, voxel_Thing* thing, voxel_Count size);
VOXEL_ERRORABLE voxel_cutStringEnd(voxel_Context* context, voxel_Thing* thing, voxel_Count size);
VOXEL_ERRORABLE voxel_padStringStart(voxel_Context* context, voxel_Thing* thing, voxel_Count minSize, voxel_Byte byte);
VOXEL_ERRORABLE voxel_padStringEnd(voxel_Context* context, voxel_Thing* thing, voxel_Count minSize, voxel_Byte byte);

voxel_Thing* voxel_newObject(voxel_Context* context);
voxel_Thing* voxel_copyObject(voxel_Context* context, voxel_Thing* thing);
voxel_ObjectItem* voxel_getObjectItem(voxel_Thing* thing, voxel_Thing* key);
VOXEL_ERRORABLE voxel_setObjectItem(voxel_Context* context, voxel_Thing* thing, voxel_Thing* key, voxel_Thing* value);
VOXEL_ERRORABLE removeObjectItem(voxel_Context* context, voxel_Thing* thing, voxel_Thing* key);
void voxel_destroyObject(voxel_Context* context, voxel_Thing* thing);
voxel_Bool voxel_compareObjects(voxel_Thing* a, voxel_Thing* b);
void voxel_lockObject(voxel_Thing* thing);
VOXEL_ERRORABLE voxel_objectToVxon(voxel_Context* context, voxel_Thing* thing);

voxel_Thing* voxel_newList(voxel_Context* context);
VOXEL_ERRORABLE voxel_getListItem(voxel_Context* context, voxel_Thing* thing, voxel_Count index);
VOXEL_ERRORABLE voxel_setListItem(voxel_Context* context, voxel_Thing* thing, voxel_Count index, voxel_Thing* value);
VOXEL_ERRORABLE voxel_removeListItem(voxel_Context* context, voxel_Thing* thing, voxel_Count index);
VOXEL_ERRORABLE voxel_pushOntoList(voxel_Context* context, voxel_Thing* thing, voxel_Thing* value);
VOXEL_ERRORABLE voxel_popFromList(voxel_Context* context, voxel_Thing* thing);
VOXEL_ERRORABLE voxel_insertIntoList(voxel_Context* context, voxel_Thing* thing, voxel_Count index, voxel_Thing* value);

VOXEL_ERRORABLE voxel_safeToRead(voxel_Context* context, voxel_Count* position, voxel_Count bytesToRead);
VOXEL_ERRORABLE voxel_nextToken(voxel_Context* context, voxel_Count* position);

void voxel_test();