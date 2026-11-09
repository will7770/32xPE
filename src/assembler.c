#include <windows.h>
#include <string.h>
#include <stdio.h>
#include "cpu.h"
#include "isa.h"
#include "files.h"



typedef enum {
    ST_FREE,
    ST_INSTR_FETCH,
    ST_ERROR,
    ST_FINISH
} PARSE_STAGE;


typedef enum {
    TK_INSTRUCTION,
    TK_REGISTER,
    TK_IMMEDIATE,
    TK_NEWLINE,
    TK_END,
    TK_BULLSHIT,
} TOKENTYPE;


struct instr_metadata {
    char name[10];
    TOKENTYPE template[4];
    int template_len;
};

static const struct instr_metadata instr_md_table[INSTR_END] = {
    [ADDI] = { "addi", { TK_REGISTER, TK_IMMEDIATE, TK_NEWLINE }, 3 },
    [ADD] = { "add", { TK_REGISTER, TK_REGISTER, TK_NEWLINE }, 3 }
};


typedef struct {
    PARSE_STAGE stage;

    INSTR target_instr;
    int instr_tpl_step;

    int current_line;
} ParserContext;


typedef struct {
    TOKENTYPE type;
    union {
        int reg_number;
        int imm;
        INSTR instr_type;
    };
} Token;



Token get_token(char **cursor) {
    char *work_mem = *cursor;
    Token token;
    token.type = TK_BULLSHIT;

    for (;;work_mem++) {
        if (*work_mem != ' ' && *work_mem != ',') {
            break;
        }
    }

    char symbol = *work_mem;

    if (symbol == '\0') {
        token.type = TK_END;
        return token;
    }

    char potential_reg_number = *(work_mem+1);

    if (symbol == '\r' && potential_reg_number == '\n') {
        token.type = TK_NEWLINE;
        work_mem += 2;
    }
    // regs
    else if ((symbol == 'x' || symbol == 'X') && (potential_reg_number >= '0' && potential_reg_number <= '8')) {
        token.type = TK_REGISTER;
        token.reg_number = potential_reg_number - '0';
        work_mem += 2;
    }
    // immediate values 
    else if (symbol >= '0' && symbol <= '9') {
        char *bad_addr;
        int tmp = (int)strtol(work_mem, &bad_addr, 0);
        if (bad_addr != work_mem || isspace(*bad_addr)) {
            token.type = TK_IMMEDIATE;
            token.imm = tmp;
            work_mem += (bad_addr - work_mem);
        }
    }
    // probably an instruction then
    else {
        for (int i = 0; i < INSTR_END; i++) {
            int len = strlen(instr_md_table[i].name);
            if (strncmp(work_mem, instr_md_table[i].name, len) == 0) {
                token.type = TK_INSTRUCTION;
                token.instr_type = i;
                work_mem += len;
                break;
            }
        }
    }

    *cursor = work_mem;
    return token;
}


void stage_free(ParserContext *ctx, Token *tk);
void stage_instr_fetch(ParserContext *ctx, Token *tk);
void stage_finish(ParserContext *ctx, Token *tk);

void assemble(const wchar_t *filename) {
    FileMap source;
    get_file_map(filename, &source, true);

    char *cursor = source.map;
    ParserContext ctx = {0};
    ctx.current_line = 1;

    while (ctx.stage != ST_FINISH && ctx.stage != ST_ERROR) {
        Token tk = get_token(&cursor);

        if (tk.type == TK_END) {
            ctx.stage = ST_FINISH;
            break;
        }

        switch (ctx.stage) {
            case ST_FREE:
            stage_free(&ctx, &tk);
            break;

            case ST_INSTR_FETCH:
            stage_instr_fetch(&ctx, &tk);
            break;

            // case ST_FINISH:
            // stage_finish(&ctx, &tk);
            // break;
        }

    }
    if (ctx.stage == ST_FINISH) {
        printf("File assembled successfully. Lines read: %d\n", ctx.current_line);
    }
    else {
        printf("File assembly failed due to the error above.\n");
    }

    close_file_map(&source);
}


void stage_free(ParserContext *ctx, Token *tk) {
    if (tk->type == TK_INSTRUCTION) {
        ctx->stage = ST_INSTR_FETCH;
        ctx->target_instr = tk->instr_type;
        ctx->instr_tpl_step = 0;
    }
    else if (tk->type == TK_NEWLINE) {
        ctx->current_line++;
    }
    else {
        ctx->stage = ST_ERROR;
        printf("Unidentified symbol at line %d\n", ctx->current_line);
    }
}


void stage_instr_fetch(ParserContext *ctx, Token *tk) {
    const struct instr_metadata *md = &instr_md_table[ctx->target_instr];

    tk->type = tk->type == TK_END? TK_NEWLINE : tk->type;

    if (tk->type == md->template[ctx->instr_tpl_step]) {
        ctx->instr_tpl_step++;
        if (ctx->instr_tpl_step == md->template_len) {
            ctx->current_line++;
            ctx->stage = ST_FREE;
            // TODO: introduce a 32 bit instruction buffer and write data to it
        }
        return;
    }
    printf("Instruction syntax error at line %d\n", ctx->current_line);
    ctx->stage = ST_ERROR;
}


void stage_finish(ParserContext *ctx, Token *tk) {

}