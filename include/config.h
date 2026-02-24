#ifndef CORE_CONFIG_H
#define CORE_CONFIG_H

// Undefine to compile without memory traces
#define DEBUG

#define MEM_ALIGN 8

// Size of the Grapheme codepoint buffer.
// Stores at most X codepoints of a grapheme
#define MAX_GRAPHEME_SIZE 64

#endif