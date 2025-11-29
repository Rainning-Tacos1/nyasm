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
	nuint memory = 0;

	struct argparse_option options[] = {
		OPT_HELP(),
		OPT_GROUP("Assembler parameters:"),
		OPT_NUINTEGER('m', "memory", &memory, "Assembler's memory size in Kb", NULL, 0, 0),
		OPT_END(),
	};

	struct argparse argparse;
	argparse_init(&argparse, options, usages, 0);
	
	argparse_describe(&argparse, NULL, "\nNyasm v"TOSTR(VERSION)" "TOSTR(ARCH) " bit");
	argc = argparse_parse(&argparse, argc, argv);

	printf("Memory: %llu\n", (uint64_t)(memory));

	for(nint i=0; i<argc; ++i) {
		printf("Argv[%lld]: %s\n", (long long int)i, argv[i]);
	}

	return 0;
}