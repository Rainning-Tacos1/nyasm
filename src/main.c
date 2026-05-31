#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "types.h"
#include "asm_lang.h"

#include "argparse/argparse.h"

// Public API
#include "api/memory.h"
#include "api/debug.h"
#include "api/log.h"
#include "api/file.h"
#include "api/core.h"

#define VERSION "v0.0.0"
#define _1KB 1000

#define STRINGIFY(x) #x
#define TOSTR(x) STRINGIFY(x)

#define EXIT_FAIL() \
    do { exit(EXIT_FAILURE); } while(0)
#define FATAL(msg, ...) \
    do { \
        ERROR("Error: "msg, ##__VA_ARGS__); \
        EXIT_FAIL(); \
    } while(0)

static const char *const usages[] = {
    "nyasm [options] [[--] args] input output",
    "nyasm [options]",
    NULL,
};

int main(int argc, const char** argv) {
    // Argument variables
    unint memory_size = 1048; // Default memory size
    nint lang = 0;
    nint version = 0;

    char* input = NULL;
    char* output = NULL;

    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("Assembler parameters:"),
        OPT_NBOOLEAN('v', "version", &version, "print the version number", NULL, 0, 0),
        OPT_NBOOLEAN('l', "langs", &lang, "print a list of installed languages", NULL, 0, 0),
        OPT_UNINTEGER('m', "memory", &memory_size, "assembler's memory size in Kb", NULL, 0, 0),
        OPT_STRING(0, "input", &input, "input file path", NULL, 0, OPT_POSITIONAL),
        OPT_STRING(0, "output", &output, "output file path", NULL, 0, OPT_POSITIONAL),
        OPT_END(),
    };

      //////////////////////
     // Argument parsing //
    //////////////////////

    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);

    argparse_describe(&argparse, NULL, "\nnyasm "VERSION" - "TOSTR(ARCH)" bit - built on: "__DATE__" "__TIME__);
    argc = argparse_parse(&argparse, argc, argv);


      /////////////////////////
     // Handle flag options //
    /////////////////////////

    // -v | --version ?
    if(version) {
        printf("nyasm "VERSION"\n");
        exit(EXIT_SUCCESS);
    }

    // -l | --langs?
    if(lang) {
        unint c = langs_count();
        if(!c) printf("No languages found.\n");
        else {
            printf("Found %"_UN" language%s:\n", c, c != 1 ? "s" : "");
            for(nint i=0; i<c; ++i) {
                struct asm_lang_t* l = &asm_langs[i];
                printf("  (%s): %s\n",l->code_name, l->lang_name);
            }
        }
        exit(EXIT_SUCCESS);
    }


      /////////////////////////
     // Handle flags & args //
    /////////////////////////

    // Ensure memory is not 0
    memory_size *= _1KB;
    if(!memory_size) FATAL("Memory size cannot be 0\n");

    // Ensure required parameters
    if(!input) FATAL("Required argument: `input` is missing\n");
    if(!output) FATAL("Required argument: `output` is missing\n");

    // Reject extra args
    if(argc) {
        ERROR("Unknown parameters: ");
        for(nint i=0; i<argc; ++i) ERROR("`%s` ", argv[i]);
        ERROR("\n");
        EXIT_FAIL();
    }


      /////////////////
     // File checks //
    /////////////////

    _stat sinput; // soutput;

    if(STAT(input, &sinput) != 0) FATAL("Could not open input file: [errno %"_N"] %s\n", (nint)errno, STAT_ERR_MSG(errno));
    if(!__S_ISREG(sinput.st_mode)) FATAL("Input is not a file\n");

    // if(STAT(output, &soutput) != 0) FATAL("Could not open output file: [errno %"_N"] %s\n", (nint)errno, STAT_ERR_MSG(errno));
    // if(!__S_ISREG(soutput.st_mode)) FATAL("Output is not a file\n");

    // if((sinput.st_dev == soutput.st_dev) && (sinput.st_ino == soutput.st_ino)) 
    //     FATAL("The input and output paths point to the same file\n");

      ///////////////////////////
     // Memory initialization //
    ///////////////////////////
    char* memory = MEM_INIT(memory_size);
    if(!memory) FATAL("Failed to allocate %"_UN" bytes of memory\n", memory_size);

      /////////////
     // Testing //
    /////////////

    ASSEMBLE(input, output);

    MEM_DBG();
    DBG(1, "-------------------------\n");
    MEM_STATS();

    printf("Mem size: %"_UN"\n", MEM_SIZE());
    printf("Mem free: %"_UN"\n", MEM_FREE());
    printf("Mem used: %"_UN"\n", MEM_SIZE() - MEM_FREE());

    MEM_DEINIT();
    return 0;
}