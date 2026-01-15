#include <stdio.h>
#include <stddef.h>

#include "types.h"
#include "asm_lang.h"
#include "argparse/argparse.h"

#define VERSION 0.0.0

#define STRINGIFY(x) #x
#define TOSTR(x) STRINGIFY(x)

static const char *const usages[] = {
	"nyasm [options] [[--] args] input output",
	"nyasm [options]",
	NULL,
};

int main(int argc, const char** argv) {
	unint memory = 0;
	// change to nint
	int lang = 0;
	char* input = NULL;
	char* output = NULL;

	struct argparse_option options[] = {
		OPT_HELP(),
		OPT_GROUP("Assembler parameters:"),
		OPT_BOOLEAN('l', "lang", &lang, "print a list of installed languages", NULL, 0, 0),
		OPT_UNINTEGER('m', "memory", &memory, "Assembler's memory size in Kb", NULL, 0, 0),
		OPT_STRING(0, "input", &input, "input file path", NULL, 0, OPT_POSITIONAL),
		OPT_STRING(0, "output", &output, "output file path", NULL, 0, OPT_POSITIONAL),
		OPT_END(),
	};

	struct argparse argparse;
	argparse_init(&argparse, options, usages, 0);
	
	argparse_describe(&argparse, NULL, "\nnyasm v"TOSTR(VERSION)" - "TOSTR(ARCH) " bit");
	argc = argparse_parse(&argparse, argc, argv);

	printf("Memory: %llu\n", (uint64_t)(unint)(memory));
	printf("Lang: %llu\n", (uint64_t)(unint)(lang));
	printf("input file: %s\n", input);
	printf("output file: %s\n", output);

	if(lang) {
		unint c = langs_count();
		if(!c) printf("No languages found.\n");
		else {

			printf("Found %d language%c:\n", c, c != 1);
			for(nint i=0; i<c; ++i) {
				struct asm_lang_t* l = &asm_langs[i];
				printf("  (%s): %s\n",l->code_name, l->lang_name);
			}
		}

	}

	for(nint i=0; i<argc; ++i) {
		printf("Argv[%lld]: %s\n", (long long int)i, argv[i]);
	}

	return 0;
}