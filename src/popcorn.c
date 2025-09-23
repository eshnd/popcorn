#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

char* generalRegisters[] = {"", "", "", "", "", ""};
// eax, ebx, ecx, edx, edi, esi
char* floatRegisters[] = {"", "", "", "", "", "", "", ""};
bool vecRegisters[] = {false, false, false, false, false, false, false, false};
// xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7

void append(char **dest, const char *src) { // helper function for strings
    *dest = realloc(*dest, strlen(*dest) + strlen(src) + 1);
    strcat(*dest, src);
}

void append_char(char **str, char c) {
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
    AEDIT,
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
    if (strcmp(cmd, "aedit") == 0) return AEDIT;
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
    return CMD_NOT_RECOGNIZED;
}


char* getNextEmptyGeneralRegister(){
    for (int i = 0; i < 6; i++){
        if (strcmp(generalRegisters[i], "") == 0){
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

char* getNextEmptyFloatRegister(){
    for (int i = 0; i < 8; i++){
        if (strcmp(floatRegisters[i], "") == 0){
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

char* getCorrelation(char* varName){
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
        if (strcmp(floatRegisters[i], "") == 0){
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

    for (int i = 0; i < stackNameList->size; i++){ // PAY ATTENTION: IF THE RESULT IS [ESP + X] MEANING IN STACK, YOU HAVE TO FREE THE RESULTANT STRING
        if (strcmp(stackNameList->data[i], stackNameList->data[i])){
            int j = i * 4;
            int size = snprintf(NULL, 0, "[esp + %d]", j) + 1;
            char* str = malloc(size);

            sprintf(str, "[esp + %d]", j);
            return str;
        }
    }

    return "ERROR";
}

int getAsmType(char* varName){
    for (int i = 0; i < 6; i++){
        if (strcmp(generalRegisters[i], varName) == 0){
            return 0;
        }
    }

    for (int i = 0; i < 8; i++){
        if (strcmp(floatRegisters[i], "") == 0){
            return 1;
        }
    }

    for (int i = 0; i < stackNameList->size; i++){ 
        if (strcmp(stackNameList->data[i], stackNameList->data[i])){
            return 2;
        }
    }
    return -1;
}

int getType(char* varName){
    for (int i = 0; i < intNameList->size; i++){
        if (strcmp(intNameList->data[i], varName) == 0){
            return 0;
        }
    }

    for (int i = 0; i < floatNameList->size; i++){
        if (strcmp(floatNameList->data[i], varName) == 0){
            return 0;
        }
    }

    return -1;
}

char* asmConvert(char* currentCommand, char* currentArgument, int numArguments, char** packs, int numPacks, int packsIndex){ // PAY ATTENTION: FREE THIS AFTER YOU CALL IT NO MATTER WHAT
    char* resultantAsm = malloc(1);
    resultantAsm[0] = '\0';
    return resultantAsm;

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
            append_char(&arguments[argumentIndex], currentArgument[i]); // combine current argument at this with this new character
        }
    }

    switch (getEnum(currentCommand)){
        case MODE:
            if (strcmp(arguments[0], "32BIT_PROTECTED") == 0){
                append(&resultantAsm, "\n\
                org 0x7E00\n\
                \n\
                start:\n\
                    cli\n\
                    xor ax, ax\n\
                    mov ds, ax\n\
                    mov es, ax\n\
                    mov ss, ax\n\
                    mov sp, 0x7C00\n\
                \n\
                    mov ax, 0x4F02\n\
                    mov bx, 0x11F | 0x4000\n\
                    int 0x10\n\
                \n\
                    mov ax, 0x4F01\n\
                    mov cx, 0x11F\n\
                    mov di, mode_info\n\
                    int 0x10\n\
                    \n\
                    \n\
                \n\
                    mov eax, [mode_info + 0x28]\n\
                    mov [lfb_addr], eax\n\
                \n\
                    movzx eax, word [mode_info + 0x10]\n\
                    mov [lfb_pitch], eax \n\
                \n\
                    lgdt [gdt_desc]\n\
                \n\
                    mov eax, cr0\n\
                    or  eax, 1\n\
                    mov cr0, eax\n\
                \n\
                    jmp CODE_SEL:pm_start\n\
                \n\
                bits 32\n\
                \n\
                pm_start:\n\
                    mov ax, DATA_SEL\n\
                    mov ds, ax\n\
                    mov es, ax\n\
                    mov fs, ax\n\
                    mov gs, ax\n\
                    mov ss, ax\n\
                    mov esp, 0x90000\n\
                \n\
                    mov esi, [lfb_addr]\n\
                \n\
                \n\
                \n\
                    mov eax, cr0\n\
                    and eax, 0xFFFB \n\
                    or  eax, 0x2 \n\
                    mov cr0, eax\n\
                \n\
                    mov eax, cr4\n\
                    or  eax, 0x600    \n\
                    mov cr4, eax\n\
                \n\
                \n\
                \n\
                section .data\n\
                \n\
                align 4\n\
                lfb_addr: dd 0\n\
                lfb_pitch: dd 0\n\
                zero: dd 0\n\
                NULL: dd 0\n\
                KEYS dd 128 dup(0)\n\
                \n\
                mode_info: times 256 db 0\n\
                \n\
                align 8\n\
                gdt:\n\
                    dq 0x0000000000000000\n\
                    dq 0x00CF9A000000FFFF\n\
                    dq 0x00CF92000000FFFF\n\
                \n\
                gdt_desc:\n\
                    dw gdt_end - gdt - 1\n\
                    dd gdt\n\
                gdt_end:\n\
                \n\
                CODE_SEL equ 0x08\n\
                DATA_SEL equ 0x10\n\
                \n\
                \n\
                \n");
                break;
            }
        case INT:
        case FLOAT:
        case PRIME:
        case INJECT: // inject into stack
        case ADD:
        case SUB:
        case MUL:
        case DIV:
        case ADDF:
        case SUBF:
        case MULF:
        case DIVF:
        case FTI:
        case ITF:
        case VEC:
        case VTA:
        case UVEC:
        case VADD:
        case VSUB:
        case VMUL:
        case VDIV:
        case VADDF:
        case VSUBF:
        case VMULF:
        case VDIVF:
        case ARRAY:
        case EDIT:
        case AEDIT:
        case PACK:
        case CALL:
        case IF:
        case WHILE:
        case RECT:
        case PAUSE:
        case ASMPACK:
        case ASMCALL:
        case INF:
        case RANDOM:
        case CMD_NOT_RECOGNIZED:
            return "error";
    }

    for (int i = 0; i < numArguments; i++) {
        free(arguments[i]);
    }

}

// parse function will go here

int main(int argc, char* argv[]){
    srand(time(NULL));
    stackNameList = createStringList(0);
    intNameList = createStringList(0);
    floatNameList = createStringList(0);

    // get file, store in string, and call parser

    freeStringList(stackNameList);
    freeStringList(intNameList);
    freeStringList(floatNameList);
}