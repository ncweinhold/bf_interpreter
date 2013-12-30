/**
  * Brainfuck interpreter written by Nick Weinhold
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DATA_SIZE 30000
#define MAX_TAPE_SIZE 30000
#define MAX_STACK_SIZE 5000

typedef enum {SIMPLE=1, JUMP} instruction_type;

typedef struct {
    instruction_type type;
    union {
        struct {
            char opcode;
        } SimpleInstruction;
        struct {
            char opcode;
            unsigned int jump_location;
        } JumpInstruction;
    } Instruction;
} BrainfuckInstruction;

typedef struct {
    unsigned int size;
    unsigned int pc_values[MAX_STACK_SIZE];
} Stack;

void Stack_push(Stack *stack, unsigned int pc_value);
unsigned int Stack_pop(Stack *stack);
int Stack_empty(Stack *stack);

void Stack_push(Stack *stack, unsigned int pc_value) {
    if (stack->size == MAX_STACK_SIZE) {
        printf("Stack overflow...\n");
        exit(1);
    }
    stack->pc_values[stack->size] = pc_value;
    stack->size++;
}

unsigned int Stack_pop(Stack *stack) {
    if (!stack->size) {
        printf("Trying to pop from an empty stack...\n");
        exit(1);
    }
    stack->size--;
    return stack->pc_values[stack->size];
}

int Stack_empty(Stack *stack) {
    return (stack->size == 0);
}

typedef struct {
    unsigned short data[MAX_DATA_SIZE];
    unsigned int data_ptr;

    BrainfuckInstruction program_tape[MAX_TAPE_SIZE];
    unsigned int pc;
    Stack jump_locations;
} BFInterpreter;

void BFInterpreter_compile(BFInterpreter *interpreter, FILE *input_file);
void BFInterpreter_eval(BFInterpreter *interpreter);

void BFInterpreter_compile(BFInterpreter *interpreter, FILE *input_file) {
    int idx = 0;
    int pc = 0;
    int c = 0;
    char current_char;
    while ((c = fgetc(input_file)) != EOF)  {
        if (pc > MAX_TAPE_SIZE-1) {
            printf("Program is larger than the tape size. Exiting...\n");
            exit(1);
        }
        current_char = (char) c;
#ifdef DEBUG
        printf("Current token is: %c\n", current_char);
#endif

        if ((current_char == '+') || (current_char == '-') || (current_char == '>') 
            || (current_char == '<') || (current_char == '.') || (current_char == ',')) {
            interpreter->program_tape[pc].type = SIMPLE;
            interpreter->program_tape[pc].Instruction.SimpleInstruction.opcode = current_char;
            pc++;
        }
        if (current_char == '[') {
            Stack_push(&interpreter->jump_locations, pc);
            interpreter->program_tape[pc].type = JUMP;
            interpreter->program_tape[pc].Instruction.JumpInstruction.opcode = current_char;
            pc++;
        }
        if (current_char == ']') {
            if (Stack_empty(&interpreter->jump_locations)) {
                printf("Unmatched loop instruction!\n");
                exit(1);
            }
            unsigned int jl = Stack_pop(&interpreter->jump_locations);
            interpreter->program_tape[pc].type = JUMP;
            interpreter->program_tape[pc].Instruction.JumpInstruction.opcode = current_char;
            interpreter->program_tape[pc].Instruction.JumpInstruction.jump_location = jl;
            interpreter->program_tape[jl].Instruction.JumpInstruction.jump_location = pc;
#ifdef DEBUG
            printf("Instruction at program_tape[%d] will jump to location: %d\n", pc, jl);
            printf("Instruction at program_tape[%d] will jump to location: %d\n", jl, pc);
#endif
            pc++;
        }
        idx++;
    }
    if (!Stack_empty(&interpreter->jump_locations)) {
        printf("Unmatched loop instruction!\n");
        exit(1);
    }
}

void BFInterpreter_eval(BFInterpreter *interpreter) {
    while (interpreter->program_tape[interpreter->pc].type != 0) {
        BrainfuckInstruction current_instruction = interpreter->program_tape[interpreter->pc];
        char current_opcode;
        if (current_instruction.type == SIMPLE) {
            current_opcode = current_instruction.Instruction.SimpleInstruction.opcode;
        }
        if (current_instruction.type == JUMP) {
            current_opcode = current_instruction.Instruction.JumpInstruction.opcode;
        }
        if (current_opcode == '+') {
            interpreter->data[interpreter->data_ptr]++;
        }
        if (current_opcode == '-') {
            interpreter->data[interpreter->data_ptr]--;
        }
        if (current_opcode == '>') {
            interpreter->data_ptr++;
        }
        if (current_opcode == '<') {
            interpreter->data_ptr--;
        }
        if (current_opcode == '.') {
            putchar(interpreter->data[interpreter->data_ptr]);
        }
        if (current_opcode == ',') {
            interpreter->data[interpreter->data_ptr] = getchar();
        }
        if (current_opcode == ']') {
            if (interpreter->data[interpreter->data_ptr]) {
                interpreter->pc = current_instruction.Instruction.JumpInstruction.jump_location;
#ifdef DEBUG
                printf("Jumping back to location: %d\n", interpreter->pc);
#endif
            }
        }
        if (current_opcode == '[') {
            if (!interpreter->data[interpreter->data_ptr]) {
                interpreter->pc = current_instruction.Instruction.JumpInstruction.jump_location;
#ifdef DEBUG
                printf("Jumping forward to location: %d\n", interpreter->pc);
#endif
            }
        }
        interpreter->pc++;
    }
}

int main(int argc, char* argv[]) {
    BFInterpreter interpreter;

    FILE *fp;
    fp = fopen(argv[1], "r");
    if ((!fp) || (argc != 2)) {
        printf("Usage: %s source_file\n", argv[0]);
        exit(1);
    }
    BFInterpreter_compile(&interpreter, fp);
    BFInterpreter_eval(&interpreter);

    fclose(fp);

    return 0;
}
