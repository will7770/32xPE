#include <windows.h>
#include <string.h>
#include <stdio.h>
#include "cpu.h"
#include "isa.h"
#include "files.h"



typedef enum {
    INSTRUCTION,
    REGISTER,
    IMMEDIATE,
    NEWLINE,
    END,
    BULLSHIT,
} TOKENTYPE;


struct instr_metadata {
    char name[10];
    TOKENTYPE template[4];
    int template_len;
};

static const struct instr_metadata instr_md_table[INSTR_END] = {
    [ADDI] = { "addi", { REGISTER, IMMEDIATE, NEWLINE }, 3 },
    [ADD] = { "add", { REGISTER, REGISTER, NEWLINE }, 3 }
};


typedef struct {
    TOKENTYPE type;
    union {
        int reg_number;
        int imm;
        INSTR instr_type;
    };
} Token;



Token get_token(const char *mem, int *cursor) {
    const char *work_mem = mem + *cursor;
    Token token;
    token.type = BULLSHIT;

    for (;;work_mem++) {
        if (*work_mem != ' ' && *work_mem != ',') {
            break;
        }
    }


    char symbol = *work_mem;
    char potential_reg_number = *(work_mem+1);

    if (symbol == '\r' && potential_reg_number == '\n') {
        token.type = NEWLINE;
        work_mem += 2;
    }
    // regs
    else if ((symbol == 'x' || symbol == 'X') && (potential_reg_number >= '0' && potential_reg_number <= '8')) {
        token.type = REGISTER;
        token.reg_number = potential_reg_number - '0';
        work_mem += 2;
    }
    // immediate values 
    else if (symbol >= '0' && symbol <= '9') {
        char *bad_addr;
        int tmp = (int)strtol(work_mem, &bad_addr, 0);
        if (bad_addr != work_mem || isspace(*bad_addr)) {
            token.type = IMMEDIATE;
            token.imm = tmp;
            work_mem += (bad_addr - work_mem);
        }
    }
    // probably an instruction then
    else {
        for (int i = 0; i < INSTR_END; i++) {
            int len = strlen(instr_md_table[i].name);
            if (strncmp(work_mem, instr_md_table[i].name, len) == 0) {
                token.type = INSTRUCTION;
                token.instr_type = i;
                work_mem += len;
                break;
            }
        }

    }

    *cursor += work_mem - (mem + *cursor);
    return token;
}


void assemble(const wchar_t *filename) {
    FileMap source;
    get_file_map(filename, &source);

    int cursor = 0;

    while (cursor < source.mapsize) {
        Token tk = get_token(source.map, &cursor);
        if (tk.type != INSTRUCTION) {
            break;
        }
        else if (tk.type == NEWLINE) { continue; }

        const struct instr_metadata *md = &instr_md_table[tk.instr_type];
        int tpl_len = md->template_len;


        for (int i = 0; i < tpl_len; i++) {
            tk = get_token(source.map, &cursor);
            if (tk.type == md->template[i]) {
                printf("%d ", tk.type);
                continue;
            }
            // some error
        }
        printf("\n");
    }

    close_file_map(&source);
}