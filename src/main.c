#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "types.h"

#include "langs/asm_lang.h"
#include "argparse/argparse.h"

// Implementation API
#include "memory/memory.h"

#define VERSION "v0.0.0"

#define STRINGIFY(x) #x
#define TOSTR(x) STRINGIFY(x)

static const char *const usages[] = {
	"nyasm [options] [[--] args] input output",
	"nyasm [options]",
	NULL,
};

int main(int argc, const char** argv) {
	// Argument variabler
	unint memory = 1048576; // Default memory size
	nint lang = 0;
	nint version = 0;

	char* input = NULL;
	char* output = NULL;

	struct argparse_option options[] = {
		OPT_HELP(),
		OPT_GROUP("Assembler parameters:"),
		OPT_NBOOLEAN('v', "version", &version, "print the version number", NULL, 0, 0),
		OPT_NBOOLEAN('l', "langs", &lang, "print a list of installed languages", NULL, 0, 0),
		OPT_UNINTEGER('m', "memory", &memory, "assembler's memory size in Kb", NULL, 0, 0),
		OPT_STRING(0, "input", &input, "input file path", NULL, 0, OPT_POSITIONAL),
		OPT_STRING(0, "output", &output, "output file path", NULL, 0, OPT_POSITIONAL),
		OPT_END(),
	};

	struct argparse argparse;
	argparse_init(&argparse, options, usages, 0);
	
	argparse_describe(&argparse, NULL, "\nnyasm "VERSION" - "TOSTR(ARCH)" bit - built on: "__DATE__" "__TIME__);
	argc = argparse_parse(&argparse, argc, argv);

	// Print the version number?
	if(version) {
		printf("nyasm "VERSION"\n");
		exit(EXIT_SUCCESS);
	}

	// List languages option?
	if(lang) {
		unint c = langs_count();
		if(!c) printf("No languages found.\n");
		else {

			printf("Found %u language%s:\n", c, c != 1 ? "s" : "");
			for(nint i=0; i<c; ++i) {
				struct asm_lang_t* l = &asm_langs[i];
				printf("  (%s): %s\n",l->code_name, l->lang_name);
			}
		}
		exit(EXIT_SUCCESS);
	}

	// Ensure memory is not 0
	if(!memory) { printf("Memory size cannot be 0\n"); exit(EXIT_FAILURE); }

	// Ensure required parameters
	if(!input) { printf("Required argument: `input` is missing\n"); exit(EXIT_FAILURE); }
	if(!output) { printf("Required argument: `output` is missing\n"); exit(EXIT_FAILURE); }


	// Reject extra args
	if(argc) {
		printf("Unknown parameters: ");
		for(nint i=0; i<argc; ++i) printf("`%s` ", argv[i]);
		printf("\n");
		exit(EXIT_FAILURE);
	}

	/*
	Removed for now
	if(!nyasm_memory.init(memory)) {
		printf("Failed to allocate %u bytes of memory\n", memory);
		exit(EXIT_FAILURE);
	}
	*/

	return 0;
}