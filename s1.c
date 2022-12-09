typedef enum
{
    KIND_SIMPLE = 0,
    KIND_REDIRECT = 1,
    KIND_PIPELINE = 2,
    KIND_SEQ = 3
} Kind;

typedef enum
{
    RD_INPUT = 10,
    RD_OUTPUT = 11,
    RD_APPEND = 12
} Redirection;

typedef enum
{ 
    OP_SEQ = 20,
    OP_DISJUNCT = 21,
    OP_CONJUNCT = 22, 
    OP_BACKGROUND = 23
} Operation;

typedef struct Command 
{
    int kind;
    union
    {
        // KIND_SIMPLE
        struct 
        {
            int argc;
            char **argv;
        }; 
        // KIND_REDIRECT
        struct
        {
            int rd_mode;
            char *rd_path;
            struct Command *rd_command;
        };  
        // KIND_PIPELINE
        struct
        {
            int pipeline_size;
            struct Command *pipeline_commands;
        };
        // KIND_SEQ
        struct
        {
            int seq_size;
            struct Command *seq_commands;
            int *seq_operations;
        };     
    };  
} Command;

int
main(void)
{
    return 0;
}