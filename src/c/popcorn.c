#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

char* generalRegisters[] = {"none", "none", "none", "none", "none", "none"};
// eax, ebx, ecx, edx, edi, esi
char* floatRegisters[] = {"none", "none", "none", "none", "none", "none", "none", "none"};
bool vecRegisters[] = {false, false, false, false, false, false, false, false};
// xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7

void append(char **dest, const char *src) { // helper function for strings
    *dest = realloc(*dest, strlen(*dest) + strlen(src) + 1);
    strcat(*dest, src);
}

void appendChar(char **str, char c) {
    size_t len = strlen(*str);
    *str = realloc(*str, len + 2); 
    (*str)[len] = c;
    (*str)[len + 1] = '\0';
}

typedef struct {
    char **data;
    size_t size;
    size_t capacity;
} stringList;

stringList* createStringList(size_t initialCapacity) {
    stringList* list = malloc(sizeof(stringList));

    list->size = 0;
    list->capacity = initialCapacity;


    if (initialCapacity > 0){
        list->data = malloc(initialCapacity * sizeof(char*));}
    else {
        list->data = NULL; }// for 0

    return list;
}

void appendString(stringList* list, const char* str) {
    if (list->size >= list->capacity) {
        size_t newCapacity = (list->capacity == 0) ? 1 : list->capacity * 2;


        list->data = realloc(list->data, newCapacity * sizeof(char*));
        list->capacity = newCapacity;
    }
    list->data[list->size++] = strdup(str);
}

void removeString(stringList* list, size_t index) {
    if (index >= list->size) {
        fprintf(stderr, "out of bounds: %zu\n", index);
        return;
    }


    free(list->data[index]);

    for (size_t i = index; i < list->size - 1; i++) {
        list->data[i] = list->data[i + 1];
    }

    list->size--; 
}

void insertStringAfter(stringList* list, size_t index, const char* str) {
    if (index >= list->size) {
        fprintf(stderr, "out of bounds: %zu\n", index);
        return;
    }
    if (list->size >= list->capacity) {
        size_t newCapacity = (list->capacity == 0) ? 1 : list->capacity * 2;
        list->data = realloc(list->data, newCapacity * sizeof(char*));
        list->capacity = newCapacity;
    }

    for (size_t i = list->size; i > index + 1; i--) {
        list->data[i] = list->data[i - 1];
    }
    list->data[index + 1] = strdup(str);
    list->size++;
}


void freeStringList(stringList* list) { // FREE IT WHEN DONE

    for (size_t i = 0; i < list->size; i++) {
        free(list->data[i]);
    }

    free(list->data);
    free(list);
}


// prototypes
stringList* stackNameList;
stringList* intNameList;
stringList* floatNameList;
stringList* intArrayNameList;
stringList* floatArrayNameList;
stringList* randomList;
stringList* bundleList;


char* parse(char* body, char splitter);

typedef enum {
    MODE,
    INT,
    FLOAT,
    PRIME,
    ADD,
    SUB,
    MUL,
    DIV,
    ADDF,
    SUBF,
    MULF,
    DIVF,
    FTI,
    ITF,
    VEC,
    VTA,
    UVEC,
    VADD,
    VSUB,
    VMUL,
    VDIV,
    VADDF,
    VSUBF,
    VMULF,
    VDIVF,
    ARRAY,
    EDIT,
    PACK,
    CALL,
    IF,
    WHILE,
    RECT,
    PAUSE,
    ASMPACK,
    ASMCALL,
    INF,
    RANDOM,
    INJECT,
    ATV,
    STRING,
    CHAR,
    ARRAYF,
    EXPEL,
    BUNDLE,
    CMD_NOT_RECOGNIZED
} command;

command getEnum(char *cmd) {
    if (strcmp(cmd, "mode") == 0) return MODE;
    if (strcmp(cmd, "int") == 0) return INT;
    if (strcmp(cmd, "float") == 0) return FLOAT;
    if (strcmp(cmd, "prime") == 0) return PRIME;
    if (strcmp(cmd, "add") == 0) return ADD;
    if (strcmp(cmd, "sub") == 0) return SUB;
    if (strcmp(cmd, "mul") == 0) return MUL;
    if (strcmp(cmd, "div") == 0) return DIV;
    if (strcmp(cmd, "addf") == 0) return ADDF;
    if (strcmp(cmd, "subf") == 0) return SUBF;
    if (strcmp(cmd, "mulf") == 0) return MULF;
    if (strcmp(cmd, "divf") == 0) return DIVF;
    if (strcmp(cmd, "f->i") == 0) return FTI;
    if (strcmp(cmd, "i->f") == 0) return ITF;
    if (strcmp(cmd, "vec") == 0) return VEC;
    if (strcmp(cmd, "v->a") == 0) return VTA;
    if (strcmp(cmd, "uvec") == 0) return UVEC;
    if (strcmp(cmd, "vadd") == 0) return VADD;
    if (strcmp(cmd, "vsub") == 0) return VSUB;
    if (strcmp(cmd, "vmul") == 0) return VMUL;
    if (strcmp(cmd, "vdiv") == 0) return VDIV;
    if (strcmp(cmd, "vaddf") == 0) return VADDF;
    if (strcmp(cmd, "vsubf") == 0) return VSUBF;
    if (strcmp(cmd, "vmulf") == 0) return VMULF;
    if (strcmp(cmd, "vdivf") == 0) return VDIVF;
    if (strcmp(cmd, "array") == 0) return ARRAY;
    if (strcmp(cmd, "edit") == 0) return EDIT;
    if (strcmp(cmd, "pack") == 0) return PACK;
    if (strcmp(cmd, "call") == 0) return CALL;
    if (strcmp(cmd, "if") == 0) return IF;
    if (strcmp(cmd, "while") == 0) return WHILE;
    if (strcmp(cmd, "rect") == 0) return RECT;
    if (strcmp(cmd, "pause") == 0) return PAUSE;
    if (strcmp(cmd, "asmpack") == 0) return ASMPACK;
    if (strcmp(cmd, "asmcall") == 0) return ASMCALL;
    if (strcmp(cmd, "inf") == 0) return INF;
    if (strcmp(cmd, "rand") == 0) return RANDOM;
    if (strcmp(cmd, "inject") == 0) return INJECT;
    if (strcmp(cmd, "a->v") == 0) return ATV;
    if (strcmp(cmd, "string") == 0) return STRING;
    if (strcmp(cmd, "char") == 0) return CHAR;
    if (strcmp(cmd, "arrayf") == 0) return ARRAYF;
    if (strcmp(cmd, "expel") == 0) return EXPEL;
    if (strcmp(cmd, "bundle") == 0) return BUNDLE;
    return CMD_NOT_RECOGNIZED;
}

char* bootloader(){
    return "\n\
org 0x7C00\n\
bits 16\n\
start:\n\
cli\n\
xor ax, ax\n\
mov ds, ax\n\
mov es, ax\n\
mov ss, ax\n\
mov sp, 0x7C00\n\
mov [dap_num_sectors], word 127\n\
mov [dap_offset], word 0x7E00\n\
mov [dap_segment], word 0x0000\n\
mov dword [dap_lba], 1\n\
mov dword [dap_lba+4], 0\n\
mov si, dap\n\
mov ah, 0x42\n\
mov dl, 0x80\n\
int 0x13\n\
jc disk_error\n\
idx_53479354:\n\
dw 4\n\
loop_2054387:\n\
add dword [dap_lba], 127\n\
mov cx, 127\n\
advance_offset1:\n\
add word [dap_offset], 512\n\
loop advance_offset1\n\
mov word [dap_num_sectors], 127\n\
int 0x13\n\
jc disk_error\n\
sub word [idx_53479354], 1\n\
cmp word [idx_53479354], 0\n\
jne loop_2054387\n\
jmp 0x0000:0x7E00\n\
disk_error:\n\
hlt\n\
dap:\n\
dap_size:       db 0x10\n\
dap_reserved:   db 0\n\
dap_num_sectors: dw 0\n\
dap_offset:     dw 0\n\
dap_segment:    dw 0\n\
dap_lba:        dq 0\n\
times 510-($-$$) db 0\n\
dw 0xAA55";
}

char* getNextEmptyGeneralRegister(){
    for (int i = 0; i < 6; i++){
        if (strcmp(generalRegisters[i], "none") == 0){
            switch (i){
                case 0:
                    return "eax";
                case 1:
                    return "ebx";
                case 2:
                    return "ecx";
                case 3:
                    return "edx";
                case 4:
                    return "edi";
                case 5:
                    return "esi";                
            }
        }
    }

    return "none";
}


int getGenRegCorrelation(char* varName){
    if (strcmp(varName, "eax") == 0){
        return 0;
    }
    if (strcmp(varName, "ebx") == 0){
        return 1;
    }
    if (strcmp(varName, "ecx") == 0){
        return 2;
    }
    if (strcmp(varName, "edx") == 0){
        return 3;
    }
    if (strcmp(varName, "edi") == 0){
        return 4;
    }
    if (strcmp(varName, "esi") == 0){
        return 5;
    }
    return -1;
}


int getNextEmptyGeneralRegisterNum(){
    for (int i = 0; i < 6; i++){
        if (strcmp(generalRegisters[i], "none") == 0){
            return i;
        }
    }

    return -1;
}


char* getNextEmptyFloatRegister(){
    for (int i = 0; i < 8; i++){
        if (strcmp(floatRegisters[i], "none") == 0 && !vecRegisters[i]){
            switch (i){
                case 0:
                    return "xmm0";
                case 1:
                    return "xmm1";
                case 2:
                    return "xmm2";
                case 3:
                    return "xmm3";
                case 4:
                    return "xmm4";
                case 5:
                    return "xmm5";
                case 6:
                    return "xmm6";
                case 7:
                    return "xmm7";
            }
        }
    }
    return "none";
}

int getNextEmptyFloatRegisterNum(){
    for (int i = 0; i < 8; i++){
        if (strcmp(floatRegisters[i], "none") == 0 && !vecRegisters[i]){
            return i;
        }
    }
    return -1;
}

char* getCorrelation(char* varName, char** resultantAsm){
    if (varName[0] != '$'){
        return varName;
    }

    char* arrayIndex = malloc(1);
    arrayIndex[0] = '\0';
    char* arrayName = malloc(1);
    arrayName[0] = '\0';
    bool isIndex = false;
    for (int i = 0; i < strlen(varName); i++){
        if (varName[i] == '@' && isIndex){
            appendChar(&arrayIndex, varName[i]);
        }else if (varName[i] == '@'){
            isIndex = true;
        } else if (isIndex){
            appendChar(&arrayIndex, varName[i]);
        } else {
            appendChar(&arrayName, varName[i]);
        }
    }

    char* index;
    char* final = malloc(1);
    final[0] = '\0';
    bool popback = false;

    if (isIndex){
        index = getCorrelation(arrayIndex, resultantAsm);
        if (index[0] == '['){ // means if INDEX in stack
            char* next = getNextEmptyGeneralRegister();
            if (strcmp(next, "none") != 0){
                append(resultantAsm, "\nmov ");
                append(resultantAsm, next);
                append(resultantAsm, ", ");
                if (index[0] == '['){
                    append(resultantAsm, "dword ");
                }
                append(resultantAsm, index);
                // appendString(stackNameList, "__POPCORN_RESERVED__");
                append(&final, next); 
            } else {
                append(resultantAsm, "\npush eax\nmov eax, ");
                if (index[0] == '['){
                    append(resultantAsm, "dword ");
                }
                append(resultantAsm, index);
                // appendString(stackNameList, "__POPCORN_RESERVED__");
                // append(resultantAsm, "\npush eax\nmov eax, [esp + 4]");
                append(&final, "eax");
                popback = true;
            }
        } else{
            append(&final, index);
        }
    }

    // FINAL IS INDEX, ARRAYNAME IS ARRAY

    if (final[0] != '\0'){
        // search for array and THEN return
        for (int i = 0; i < stackNameList->size; i++){
            if (strcmp(stackNameList->data[i], arrayName) == 0){
                int j = i * 4;
                if (final[0] != 'e'){
                    int size = snprintf(NULL, 0, "[ebp - %d - %s*4]", j, final) + 1;
                    char* str = malloc(size);

                    sprintf(str, "[ebp - %d - %s*4]", j, final);

                    free(final);
                    free(arrayName);
                    return str;
                } else {
                    int size = snprintf(NULL, 0, "[ebp - %d - %s*4]", j, final) + 1;
                    char* str = malloc(size);

                    sprintf(str, "[ebp - %d - %s*4]", j, final);
                    append(resultantAsm, "\npush dword ");
                    appendString(stackNameList, "__POPCORN_RESERVED__");
                    append(resultantAsm, str);
                    if (popback) {
                        append(resultantAsm, "\nmov eax, [esp + 4]");
                    }
                    sprintf(str, "[esp]"); // pretty sure this is how this works
                    free(final);
                    free(arrayName);
                    return str;
                }
                
                
            }
        }
    }
    free(final);
    free(arrayName);
    
    for (int i = 0; i < 6; i++){
        if (strcmp(generalRegisters[i], varName) == 0){
            switch (i){
                case 0:
                    return "eax";
                case 1:
                    return "ebx";
                case 2:
                    return "ecx";
                case 3:
                    return "edx";
                case 4:
                    return "edi";
                case 5:
                    return "esi";                
            }
        }
    }

    for (int i = 0; i < 8; i++){
        if (strcmp(floatRegisters[i], varName) == 0){
            
            switch (i){
                case 0:
                    return "xmm0";
                case 1:
                    return "xmm1";
                case 2:
                    return "xmm2";
                case 3:
                    return "xmm3";
                case 4:
                    return "xmm4";
                case 5:
                    return "xmm5";
                case 6:
                    return "xmm6";
                case 7:
                    return "xmm7";
            }
        }
    }

    for (int i = 0; i < stackNameList->size; i++){ // PAY ATTENTION: IF THE RESULT IS [EBP + X] MEANING IN STACK, YOU HAVE TO FREE THE RESULTANT STRING
        if (strcmp(stackNameList->data[i], varName) == 0){
            int j = i * 4;
            int size = snprintf(NULL, 0, "[ebp - %d]", j) + 1;
            char* str = malloc(size);

            sprintf(str, "[ebp - %d]", j);
            return str;
        }
    }
    return "ERROR";
}








// THIS IS THE SAME FOR NOW YOU STILL HAVE TO EDIT IT
char* getSacrificialCorrelation(char* varName, char** resultantAsm){
    if (varName[0] != '$'){
        return varName;
    }

    char* arrayIndex = malloc(1);
    arrayIndex[0] = '\0';
    char* arrayName = malloc(1);
    arrayName[0] = '\0';
    bool isIndex = false;
    for (int i = 0; i < strlen(varName); i++){
        if (varName[i] == '@' && isIndex){
            appendChar(&arrayIndex, varName[i]);
        }else if (varName[i] == '@'){
            isIndex = true;
        } else if (isIndex){
            appendChar(&arrayIndex, varName[i]);
        } else {
            appendChar(&arrayName, varName[i]);
        }
    }

    char* index;
    char* final = malloc(1);
    final[0] = '\0';
    bool popback = false;

    if (isIndex){
        index = getCorrelation(arrayIndex, resultantAsm);
        if (index[0] == '['){ // means if INDEX in stack
            char* next = getNextEmptyGeneralRegister();
            if (strcmp(next, "none") != 0){
                append(resultantAsm, "\nmov ");
                append(resultantAsm, next);
                append(resultantAsm, ", ");
                if (index[0] == '['){
                    append(resultantAsm, "dword ");
                }
                append(resultantAsm, index);
                // appendString(stackNameList, "__POPCORN_RESERVED__");
                append(&final, next); 
            } else {
                append(resultantAsm, "\npush eax\nmov eax, ");
                if (index[0] == '['){
                    append(resultantAsm, "dword ");
                }
                append(resultantAsm, index);
                // appendString(stackNameList, "__POPCORN_RESERVED__");
                // append(resultantAsm, "\npush eax\nmov eax, [esp + 4]");
                append(&final, "eax");
                popback = true;
            }
        } else{
            append(&final, index);
        }
    }

    // FINAL IS INDEX, ARRAYNAME IS ARRAY

    if (final[0] != '\0'){
        // search for array and THEN return
        for (int i = 0; i < stackNameList->size; i++){
            if (strcmp(stackNameList->data[i], arrayName) == 0){
                int j = i * 4;
                int size = snprintf(NULL, 0, "[ebp - %d - %s*4]", j, final) + 1;
                char* str = malloc(size);

                sprintf(str, "[ebp - %d - %s*4]", j, final); // no bracket to make it easier to append stuff

                free(final);
                free(arrayName);
                return str;

                
            }
        }
    }
    free(final);
    free(arrayName);
    
    for (int i = 0; i < 6; i++){
        if (strcmp(generalRegisters[i], varName) == 0){
            switch (i){
                case 0:
                    return "eax";
                case 1:
                    return "ebx";
                case 2:
                    return "ecx";
                case 3:
                    return "edx";
                case 4:
                    return "edi";
                case 5:
                    return "esi";                
            }
        }
    }

    for (int i = 0; i < 8; i++){
        if (strcmp(floatRegisters[i], varName) == 0){
            
            switch (i){
                case 0:
                    return "xmm0";
                case 1:
                    return "xmm1";
                case 2:
                    return "xmm2";
                case 3:
                    return "xmm3";
                case 4:
                    return "xmm4";
                case 5:
                    return "xmm5";
                case 6:
                    return "xmm6";
                case 7:
                    return "xmm7";
            }
        }
    }

    for (int i = 0; i < stackNameList->size; i++){ // PAY ATTENTION: IF THE RESULT IS [EBP + X] MEANING IN STACK, YOU HAVE TO FREE THE RESULTANT STRING
        if (strcmp(stackNameList->data[i], varName) == 0){
            int j = i * 4;
            int size = snprintf(NULL, 0, "[ebp - %d]", j) + 1;
            char* str = malloc(size);

            sprintf(str, "[ebp - %d]", j);
            return str;
        }
    }
    return "ERROR";
}












int getItemInStackIndex(char* varName){
    for (int i = 0; i < stackNameList->size; i++){ 
        if (strcmp(stackNameList->data[i], varName) == 0){
            return i;
        }
    }
    return -1;
}

int getAsmType(char* varName){
    for (int i = 0; i < 6; i++){
        if (strcmp(generalRegisters[i], varName) == 0){
            return 0;
        }
    }

    for (int i = 0; i < 8; i++){
        if (strcmp(floatRegisters[i], varName) == 0){
            return 1;
        }
    }

    for (int i = 0; i < stackNameList->size; i++){ 
        if (strcmp(stackNameList->data[i], varName) == 0){
            return 2;
        }
    }
    return -1;
}

char* getArrayName(char* varName){ // MUST FREE
    char* arrayName = malloc(1);
    arrayName[0] = '\0';
    for (int i = 0; i < strlen(varName); i++){
        if (varName[i] == '@'){
            break;
        } else {
            appendChar(&arrayName, varName[i]);
        }
    }

    return arrayName;
}

int getType(char* varName){
    for (int i = 0; i < intNameList->size; i++){
        if (strcmp(intNameList->data[i], varName) == 0){
            return 0;
        }
    }

    for (int i = 0; i < floatNameList->size; i++){
        if (strcmp(floatNameList->data[i], varName) == 0){
            return 1;
        }
    }

    char* arrayName = malloc(1);
    arrayName[0] = '\0';
    for (int i = 0; i < strlen(varName); i++){
        if (varName[i] == '@'){
            break;
        } else {
            appendChar(&arrayName, varName[i]);
        }
    }

    for (int i = 0; i < intArrayNameList->size; i++){
        if (strcmp(intArrayNameList->data[i], arrayName) == 0){
            free(arrayName);
            return 2;
        }
    }

    for (int i = 0; i < floatArrayNameList->size; i++){
        if (strcmp(floatArrayNameList->data[i], arrayName) == 0){
            free(arrayName);
            return 3;
        }
    }

    return -1;
}

bool inRandomList(char* randomNum){
    for (int i = 0; i < randomList->size; i++){
        if (strcmp(randomList->data[i], randomNum) == 0){
            return true;
        }
    }
    return false;
}

char* asmConvert(char* currentCommand, char* currentArgument, int numArguments, char** packs, int numPacks, int packsIndex){ // PAY ATTENTION: FREE THIS AFTER YOU CALL IT NO MATTER WHAT
    
    char* resultantAsm = malloc(1);
    resultantAsm[0] = '\0';

    char* arguments[numArguments];
    for (int i = 0; i < numArguments; i++) {
        arguments[i] = malloc(1);
        arguments[i][0] = '\0';
    }

    int argumentIndex = 0;
    for (int i = 0; i < strlen(currentArgument); i++){
        if (currentArgument[i] == ','){
            ++argumentIndex;
        } else {
            appendChar(&arguments[argumentIndex], currentArgument[i]); // combine current argument at this with this new character
        }
    }

    switch (getEnum(currentCommand)){
        case MODE: {
            if (strcmp(arguments[0], "32BIT_PROTECTED") == 0){
                append(&resultantAsm, "org 0x7E00\n\
start:\n\
cli\n\
xor ax, ax\n\
mov ds, ax\n\
mov es, ax\n\
mov ss, ax\n\
mov sp, 0x7C00\n\
mov ax, 0x4F02\n\
mov bx, 0x11F | 0x4000\n\
int 0x10\n\
mov ax, 0x4F01\n\
mov cx, 0x11F\n\
mov di, mode_info\n\
int 0x10\n\
mov eax, [mode_info + 0x28]\n\
mov [lfb_addr], eax\n\
movzx eax, word [mode_info + 0x10]\n\
mov [lfb_pitch], eax \n\
lgdt [gdt_desc]\n\
mov eax, cr0\n\
or  eax, 1\n\
mov cr0, eax\n\
jmp CODE_SEL:pm_start\n\
bits 32\n\
pm_start:\n\
mov ax, DATA_SEL\n\
mov ds, ax\n\
mov es, ax\n\
mov fs, ax\n\
mov gs, ax\n\
mov ss, ax\n\
mov esp, 0x90000\n\
mov esi, [lfb_addr]\n\
mov eax, cr0\n\
and eax, 0xFFFB \n\
or  eax, 0x2 \n\
mov cr0, eax\n\
mov eax, cr4\n\
or  eax, 0x600    \n\
mov cr4, eax\n\
section .data\n\
align 4\n\
lfb_addr: dd 0\n\
lfb_pitch: dd 0\n\
zero: dd 0\n\
NULL: dd 0\n\
KEYS dd 128 dup(0)\n\
mode_info: times 256 db 0\n\
align 8\n\
gdt:\n\
dq 0x0000000000000000\n\
dq 0x00CF9A000000FFFF\n\
dq 0x00CF92000000FFFF\n\
gdt_desc:\n\
dw gdt_end - gdt - 1\n\
dd gdt\n\
gdt_end:\n\
CODE_SEL equ 0x08\n\
DATA_SEL equ 0x10\n\
section .text\n\
mov ebp, esp");
                break;
            }
        }
        case INT: {// IS DWORD DECLARATION NECESSARY?????????????????????? (it is)`
            
            appendString(intNameList, arguments[0]);
            char* next = getNextEmptyGeneralRegister();
            
            char* newValue = getCorrelation(arguments[1], &resultantAsm);
            if (next[0] != 'n'){ // if there is a gen reg avaliable
                int nextNum = getNextEmptyGeneralRegisterNum();
                append(&resultantAsm, "\nmov ");
                append(&resultantAsm, next);
                append(&resultantAsm, ", ");
                if (newValue[0] == '['){
                    append(&resultantAsm, "dword ");
                }
                append(&resultantAsm, newValue);
                generalRegisters[nextNum] = arguments[0];
            } else {
                char* next = getNextEmptyFloatRegister();
                if (next[0] != 'n'){
                    int nextNum = getNextEmptyFloatRegisterNum();
                    floatRegisters[nextNum] = arguments[0];
                    if (arguments[1][0] == '$'){
                        append(&resultantAsm, "\nmovd ");
                        append(&resultantAsm, next);
                        append(&resultantAsm, ", ");
                        if (newValue[0] == '['){
                            append(&resultantAsm, "dword ");
                        }
                        append(&resultantAsm, newValue);
                    } else {
                        append(&resultantAsm, "\npush eax\nmov eax, ");
                        if (newValue[0] == '['){
                            append(&resultantAsm, "dword ");
                        }
                        append(&resultantAsm, newValue);
                        append(&resultantAsm, "\nmovd ");
                        append(&resultantAsm, next);
                        append(&resultantAsm, ", eax\npop eax");
                    }
                } else {
                    // add to stack
                    append(&resultantAsm, "\npush ");
                    if (newValue[0] == '['){
                        append(&resultantAsm, "dword ");
                    }
                    append(&resultantAsm, newValue);
                    appendString(stackNameList, arguments[0]);
                }
            }


            if (newValue[0] == '['){
                free(newValue);
            }

            break;
        }
        case FLOAT: {
            appendString(floatNameList, arguments[0]);
            char* next = getNextEmptyFloatRegister();
            char* newValue = getCorrelation(arguments[1], &resultantAsm);

            if (next[0] != 'n'){
                int nextNum = getNextEmptyFloatRegisterNum();
                floatRegisters[nextNum] = arguments[0];
                if (arguments[1][0] == '$'){
                    if (newValue[0] == 'e'){
                        append(&resultantAsm, "\nmovd ");
                        append(&resultantAsm, next);
                        append(&resultantAsm, ", ");
                        append(&resultantAsm, newValue);
                    }  else if (newValue[0] == 'x'){
                        append(&resultantAsm, "\nmovss ");
                        append(&resultantAsm, next);
                        append(&resultantAsm, ", ");
                        append(&resultantAsm, newValue);
                    } else if (newValue[0] == '['){
                        append(&resultantAsm, "\nmovss ");
                        append(&resultantAsm, next);
                        append(&resultantAsm, ", dword ");
                        append(&resultantAsm, newValue);
                    }
                } else {
                    append(&resultantAsm, "\npush eax\nmov eax, __float32__(");
                    append(&resultantAsm, newValue);
                    append(&resultantAsm, ")\nmovd ");
                    append(&resultantAsm, next);
                    append(&resultantAsm, ", eax\npop eax");
                }
            } else {
                char* next = getNextEmptyGeneralRegister();
                if (next[0] != 'n'){
                    int nextNum = getNextEmptyGeneralRegisterNum();
                    append(&resultantAsm, "\nmov ");
                    append(&resultantAsm, next);
                    append(&resultantAsm, ", ");
                    if (newValue[0] == '['){
                        append(&resultantAsm, "dword ");
                    }
                    if (arguments[1][0] == '$'){
                        append(&resultantAsm, newValue);
                    } else {
                        append(&resultantAsm, "__float32__(");
                        append(&resultantAsm, newValue);
                        append(&resultantAsm, ")");
                    }
                    generalRegisters[nextNum] = arguments[0];
                } else {
                    append(&resultantAsm, "\npush ");
                    if (newValue[0] == '['){
                        append(&resultantAsm, "dword ");
                    }
                    if (arguments[1][0] == '$'){
                        append(&resultantAsm, newValue);
                    } else {
                        append(&resultantAsm, "__float32__(");
                        append(&resultantAsm, newValue);
                        append(&resultantAsm, ")");
                    }
                    appendString(stackNameList, arguments[0]);
                }
            }

            if (newValue[0] == '['){
                free(newValue);
            }

            break;
        }
        case ARRAY: {
            for (int i = 1; i < sizeof(arguments) / sizeof(arguments[0]); i++){
                char* newValue = getCorrelation(arguments[i], &resultantAsm);
                
                append(&resultantAsm, "\npush ");
                if (newValue[0] == '['){
                    append(&resultantAsm, "dword ");
                }
                append(&resultantAsm, newValue);
                appendString(stackNameList, arguments[0]);
                if (newValue[0] == '['){
                    free(newValue);
                }
            }
            appendString(intArrayNameList, arguments[0]);
            break;
        }
        case ARRAYF: {
            for (int i = 1; i < sizeof(arguments) / sizeof(arguments[0]); i++){
                char* newValue = getCorrelation(arguments[i], &resultantAsm);
                append(&resultantAsm, "\npush ");
                if (newValue[0] == '['){
                    append(&resultantAsm, "dword ");
                }
                if (arguments[1][0] == '$'){
                    append(&resultantAsm, newValue);
                } else {
                    append(&resultantAsm, "__float32__(");
                    append(&resultantAsm, newValue);
                    append(&resultantAsm, ")");
                }
                appendString(stackNameList, arguments[0]);
                if (newValue[0] == '['){
                    free(newValue);
                }
                
            }
            appendString(floatArrayNameList, arguments[0]);
            break;
        }
        case PRIME: {// move integers to general registers, move floats to float registers, FOR VALUES WITH @ SIGN IN THEM, JUST COPY THEM ONTO A REGISTER????
            
            
            int currentGeneralRegister = 0;
            int currentFloatRegister = 0;
            
            for (int i = 0; i < sizeof(arguments) / sizeof(arguments[0]); i++){
                
                char* value = getCorrelation(arguments[i], &resultantAsm);
                
                int type = getType(arguments[i]); // 0 for int, 1 for float, 2 for a value in an intArray, 3 for a value in a floatArray
                if ((type % 2 == 0 && value[0] == 'e') || (type % 2 != 0 && value[0] == 'x')){
                    continue;
                }
                

                if (type % 2 == 0){
                    char* next = getNextEmptyGeneralRegister();
                    if (next[0] != 'n' && (type == 0 || type == 2)){
                        int nextNum = getNextEmptyGeneralRegisterNum();
                        generalRegisters[nextNum] = arguments[i];
                        append(&resultantAsm, "\nmov ");
                        append(&resultantAsm, next);
                        append(&resultantAsm, ", ");
                        char* arrayName = getArrayName(arguments[i]);
                        if (value[0] == '['){
                            append(&resultantAsm, "dword ");
                            removeString(stackNameList, getItemInStackIndex(arrayName)); // <- WRONG, GET FLOAT REGISTERS TOO

                        } else if (value[0] == 'x'){
                            floatRegisters[value[3] - '0'] = "none";
                            // floatRegisters[atoi(value)] = "none"; // THIS DOESNT WORK
                            // check x in float registers (DO THIS FOR OTHER REGISTERS AS WELL)
                        }
                        append(&resultantAsm, value);
                        free(arrayName);
                        if (value[0] == '['){
                            free(value);
                        }
                        if (nextNum != 5){
                            currentGeneralRegister = nextNum + 1;
                        } else {
                            currentGeneralRegister = 0;
                        }
                        
                        continue;
                    } else {
                        if (currentGeneralRegister > 5){
                            currentGeneralRegister = 0;
                        }
                        appendString(stackNameList, generalRegisters[currentGeneralRegister]);
                        generalRegisters[currentGeneralRegister] = arguments[i];
                        char* next;
                        switch (currentGeneralRegister){
                            case 0:
                                next = "eax";
                                break;
                            case 1:
                                next = "ebx";
                                break;
                            case 2:
                                next = "ecx";
                                break;
                            case 3:
                                next = "edx";
                                break;
                            case 4:
                                next = "edi";
                                break;
                            case 5:
                                next = "esi";
                                break;                
                        }
                        append(&resultantAsm, "\npush ");
                        append(&resultantAsm, next);
                        append(&resultantAsm, "\nmov ");
                        append(&resultantAsm, next);
                        append(&resultantAsm, ", ");
                        char* arrayName = getArrayName(arguments[i]);

                        if (value[0] == '['){
                            append(&resultantAsm, "dword ");
                            removeString(stackNameList, getItemInStackIndex(arrayName));

                            
                        } else if (value[0] == 'x'){
                            floatRegisters[value[3] - '0'] = "none";
                            // floatRegisters[atoi(value)] = "none"; // THIS DOESNT WORK
                            // check x in float registers (DO THIS FOR OTHER REGISTERS AS WELL)
                        }
                        append(&resultantAsm, value);
                        free(arrayName);
                        if (value[0] == '['){
                            free(value);
                        }
                        if (currentGeneralRegister != 5){
                            currentGeneralRegister = currentGeneralRegister + 1;
                        } else {
                            currentGeneralRegister = 0;
                        }
                        continue;

                    }
                }
                char* next = getNextEmptyFloatRegister();
                if (next[0] != 'n' && (type == 1 || type == 3)){
                    int nextNum = getNextEmptyFloatRegisterNum();
                    floatRegisters[nextNum] = arguments[i];
                    char* arrayName = getArrayName(arguments[i]);
                    
                    if (value[0] == 'e'){
                        append(&resultantAsm, "\nmovd ");
                        append(&resultantAsm, next);
                        append(&resultantAsm, ", ");
                        append(&resultantAsm, value);
                        generalRegisters[getGenRegCorrelation(value)] = "none";
                    }  else if (value[0] == 'x'){
                        append(&resultantAsm, "\nmovss ");
                        append(&resultantAsm, next);
                        append(&resultantAsm, ", ");
                        append(&resultantAsm, value);
                    } else if (value[0] == '['){
                        append(&resultantAsm, "\nmovss ");
                        append(&resultantAsm, next);
                        append(&resultantAsm, ", dword ");
                        append(&resultantAsm, value);
                        removeString(stackNameList, getItemInStackIndex(arrayName));
                    
                    }
                    if (value[0] == '['){
                        free(value);
                    }
                    free(arrayName);

                    if (nextNum != 7){
                        currentGeneralRegister = nextNum + 1;
                    } else {
                        currentGeneralRegister = 0;
                    }

                    continue;
                }else {
                    if (currentFloatRegister > 7){
                        currentFloatRegister = 0;
                    }

                    bool impossible = false;

                    while (vecRegisters[currentFloatRegister]){
                        currentFloatRegister = currentFloatRegister + 1;
                        if (currentFloatRegister > 7){
                            impossible = true;
                            break;
                        }
                    }

                    if (impossible){
                        continue;
                    }

                    appendString(stackNameList, floatRegisters[currentFloatRegister]);
                    floatRegisters[currentFloatRegister] = arguments[i];
                    char* next;
                    switch (currentFloatRegister){
                        case 0:
                            next = "xmm0";
                            break;
                        case 1:
                            next = "xmm1";
                            break;
                        case 2:
                            next = "xmm2";
                            break;
                        case 3:
                            next = "xmm3";
                            break;
                        case 4:
                            next = "xmm4";
                            break;
                        case 5:
                            next = "xmm5";
                            break;
                        case 6:
                            next = "xmm6";
                            break;  
                        case 7:
                            next = "xmm7";
                            break;                  
                    }
                    append(&resultantAsm, "\npush ");
                    append(&resultantAsm, next);
                    char* arrayName = getArrayName(arguments[i]);
                    
                    if (value[0] == 'e'){
                        append(&resultantAsm, "\nmovd ");
                        append(&resultantAsm, next);
                        append(&resultantAsm, ", ");
                        append(&resultantAsm, value);
                        generalRegisters[getGenRegCorrelation(value)] = "none";
                    }  else if (value[0] == 'x'){
                        append(&resultantAsm, "\nmovss ");
                        append(&resultantAsm, next);
                        append(&resultantAsm, ", ");
                        append(&resultantAsm, value);
                    } else if (value[0] == '['){
                        append(&resultantAsm, "\nmovss ");
                        append(&resultantAsm, next);
                        append(&resultantAsm, ", dword ");
                        append(&resultantAsm, value);
                        removeString(stackNameList, getItemInStackIndex(arrayName));
                    
                    }
                    free(arrayName);
                    if (value[0] == '['){
                        free(value);
                    }
                    if (currentGeneralRegister != 7){
                        currentGeneralRegister = currentGeneralRegister + 1;
                    } else {
                        currentGeneralRegister = 0;
                    }
                    continue;

                }
            }

            break;
        }
        case INJECT: { // INJECT ONE VALUE INTO STACK
            appendString(stackNameList, generalRegisters[0]);
            appendString(stackNameList, generalRegisters[1]);
            appendString(stackNameList, generalRegisters[2]);
            append(&resultantAsm, "\npush eax\npush ebx\npush ecx\nmov ebx, esp\nsub esp, 4");
            char str[300];
            append(&resultantAsm, str);
            generalRegisters[0] = "none";
            generalRegisters[1] = "none";
            generalRegisters[2] = "none";
            char* leftVar = getSacrificialCorrelation(arguments[0], &resultantAsm);
            int r1 = rand(); // add rand to list of rands !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            str[0] = '\0';
            sprintf(str, "%d", r1);
            while(inRandomList(str)){
                r1 = rand();
                str[0] = '\0';
                sprintf(str, "%d", r1); // i havent looked at the assembly for this so if this blows up im gonna be sad lol
            }

            str[0] = '\0';
            sprintf(str, "\nlea eax, %s\nsub eax, 4\ninjection_label_%d:\nmov ecx, [ebx + 4]\nmov [ebx], ecx\nadd ebx, 4\ncmp eax, ebx\njne injection_label_%d", leftVar, r1, r1); 
            append(&resultantAsm, str);
            str[0] = '\0';
            insertStringAfter(stackNameList, getItemInStackIndex(getArrayName(arguments[0])), getArrayName(arguments[0]));
            char* actualVar = getCorrelation(arguments[1], &resultantAsm);
            sprintf(str, "\nmov dword [eax], %s", actualVar); 
            append(&resultantAsm, str);
            free(leftVar);
            free(actualVar); // idk if these frees are valid but we'll see ig
            break;
        }
        case BUNDLE: { // BUNDLE REQUIRES YOU TO INSTALL YOUR POPB BUNDLES WITH MAIZE, A PACKAGE MANAGER THAT CAN ALSO WORK USING REPOS ON INTERNET
            for (int i = 0; i < sizeof(arguments) / sizeof(arguments[0]); i++){
                appendString(bundleList, arguments[i]);
            }
        }
        case EXPEL: {
            break;
        } // delete ONE values from stack (EDIT THIS LATER FOR STRING EDITING TOO -- OR JUST MAKE A STRING "BUNDLE")
        case ADD: {break;}
        case SUB: {break;}
        case MUL: {break;}
        case DIV: {break;}
        case ADDF: {break;}
        case SUBF: {break;}
        case MULF: {break;}
        case DIVF: {break;}
        case FTI: {break;}
        case ITF: {break;}
        case VEC: {break;} // max length 32 
        case VTA: {break;}
        case ATV: {break;}
        case UVEC: {break;}
        case VADD: {break;}
        case VSUB: {break;}
        case VMUL: {break;}
        case VDIV: {break;}
        case VADDF: {break;}
        case VSUBF: {break;}
        case VMULF: {break;}
        case VDIVF: {break;}
        case CHAR: {break;} // treat like integer, literally just a byte
        case STRING: {break;}
        case EDIT: {break;}
        case PACK: {break;}
        case CALL: {break;}
        case IF: {break;}
        case WHILE: {break;}
        case RECT: {break;}
        case PAUSE: {break;}
        case ASMPACK: {break;}
        case ASMCALL: {break;}
        case INF: {break;}
        case RANDOM: {break;}
        case CMD_NOT_RECOGNIZED: {break;}
            return "error";
    }

    return resultantAsm;
}

// parse function will go here
char* parse(char* fileString, char splitter){
    
    char* resultantAsm = malloc(1);
    resultantAsm[0] = '\0';
    
    bool commandStatus = true;
    bool packStatus = false;
    bool commentStatus = false;
    
    char* currentCommand = malloc(1);
    currentCommand[0] = '\0';
    char* currentArgument = malloc(1);
    currentArgument[0] = '\0';
    
    int numArguments = 1;
    int numPacks = 0;
    
    char* currentWord = malloc(1);
    currentWord[0] = '\0';
    
    for (int i = 0; i < strlen(fileString); i++){
        
        if (fileString[i] == ' '){
            if (strcmp(currentWord, "pack") == 0){
                ++numPacks;
            }
        }
        
        appendChar(&currentWord, fileString[i]);
    }
    
    free(currentWord);
    
    char* packs[numPacks];

    int packsIndex = 0;
    
    
    for (int i = 0; i < strlen(fileString); i++){
        if (!commentStatus){
            if (fileString[i] == '`'){
                commentStatus = !commentStatus;
            }else if (fileString[i] == ' ' || fileString[i] == '\n' || fileString[i] == '{' ||fileString[i] == '}' ||fileString[i] == '&'){continue;} 
            else if (fileString[i] == ':' && !packStatus){
                commandStatus = false;
                if (strcmp(currentCommand, "pack") == 0){
                    packStatus = true;
                    ++numArguments;
                }
            } else if (fileString[i] == splitter && !packStatus){
                commandStatus = true;
                
                char* j =asmConvert(currentCommand, currentArgument, numArguments, packs, numPacks, packsIndex);
                append(&resultantAsm, j);
                free(j);
                currentCommand[0] = '\0';
                currentArgument[0] = '\0';
                numArguments = 1;
            }else if (fileString[i] == splitter){
               if (i + 1 < strlen(fileString)){
                  if (fileString[i+1] == '&'){
                     commandStatus = true;
                     packStatus = false;
                
                     char* j =asmConvert(currentCommand, currentArgument, numArguments, packs, numPacks, packsIndex);
                     append(&resultantAsm, j);
                     free(j);
                     currentCommand[0] = '\0';
                     currentArgument[0] = '\0';
                     numArguments = 1;
                  }
               }
               
            } else if (commandStatus){
                appendChar(&currentCommand, fileString[i]);
                
            } else if (fileString[i] == ',' && !packStatus){
                appendChar(&currentArgument, fileString[i]);
                ++numArguments;
            } else {
                appendChar(&currentArgument, fileString[i]);
            }
        }
        
    }


    free(currentCommand);
    free(currentArgument);
    append(&resultantAsm, "\nhlt");
    return resultantAsm;
}

char* readFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("file doesnt exist");
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET); 

    char* buffer = malloc(length + 1);
    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);
    return buffer;
}

int writeFile(const char* filename, const char* content) {
    FILE* file = fopen(filename, "w");  
    if (fputs(content, file) == EOF) {
        fclose(file);
        return 0; 
    }
    fclose(file);
    return 1;
}

void createDisk(const char* disk_name, size_t block_size, size_t count) {
    FILE* disk = fopen(disk_name, "wb");
    char* buffer = calloc(block_size, 1); 
    for (size_t i = 0; i < count; i++) {
        if (fwrite(buffer, 1, block_size, disk) != block_size) {
            free(buffer);
            fclose(disk);
            exit(1);
        }
    }
    free(buffer);
    fclose(disk);
}

void writeDisk(const char* disk_name, const char* file_name, size_t block_size, size_t seek_blocks) {
    FILE* disk = fopen(disk_name, "r+b"); 
    FILE* file = fopen(file_name, "rb");
    fseek(disk, block_size * seek_blocks, SEEK_SET);
    char buffer[512];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (fwrite(buffer, 1, n, disk) != n) {
            fclose(file);
            fclose(disk);
            exit(1);
        }
    }
    fclose(file);
    fclose(disk);
}

int main(int argc, char* argv[]){
    srand(time(NULL));
    stackNameList = createStringList(0);
    intNameList = createStringList(0);
    floatNameList = createStringList(0);
    intArrayNameList = createStringList(0);
    floatArrayNameList = createStringList(0);
    randomList = createStringList(0);
    bundleList = createStringList(0);

    // get file, store in string, and call parser
    char* result = parse(readFile(argv[1]), ';');
    writeFile(".__pop__.popc", result);
    writeFile(".__pop_boot__.popc", bootloader());
    // char nasmCall[100];
    // sprintf(nasmCall, "nasm -f bin .__pop__.asm -o .__pop__.bin", argv[2]);
    
    createDisk(argv[2], 512, 200);
    
    system("nasm -f bin .__pop__.popc -o .__pop__.popcb");
    system("nasm -f bin .__pop_boot__.popc -o .__pop_boot__.popcb");

    writeDisk(argv[2], ".__pop_boot__.popcb", 512, 0);
    writeDisk(argv[2], ".__pop__.popcb", 512, 1);

    if (argc == 3){
        remove(".__pop__.popc");
    } else {
        rename(".__pop__.popc", argv[3]);
    }
    remove(".__pop_boot__.popc");
    remove(".__pop_boot__.popcb");
    remove(".__pop__.popcb");
    free(result);

    // result = parse("prime: $a;", ';');
    // printf("%s", result);
    // free(result);
    freeStringList(stackNameList);
    freeStringList(intNameList);
    freeStringList(floatNameList);
    freeStringList(intArrayNameList);
    freeStringList(floatArrayNameList);
    freeStringList(randomList);
    freeStringList(bundleList);
}
