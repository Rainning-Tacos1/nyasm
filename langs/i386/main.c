#include <stdio.h>


#include "asm_lang.h"
#include "../../src/core/helper.h"
#include "../../src/core/token.h"
#include "../../src/core/variables.h"
#include "types.h"

#include "api/debug.h"
#include "api/file.h"

#define ERROR_TYPE_I386 "i386Error"


#define MOV_INSTRUCTION ((int32_t[]){'m', 'o', 'v', -1})
#define LEA_INSTRUCTION ((int32_t[]){'l', 'e', 'a', -1})
#define LDS_INSTRUCTION ((int32_t[]){'l', 'd', 's', -1})
#define LES_INSTRUCTION ((int32_t[]){'l', 'e', 's', -1})
#define LFS_INSTRUCTION ((int32_t[]){'l', 'f', 's', -1})
#define LGS_INSTRUCTION ((int32_t[]){'l', 'g', 's', -1})
#define LSS_INSTRUCTION ((int32_t[]){'l', 's', 's', -1})
#define MOVSX_INSTRUCTION ((int32_t[]){'m', 'o', 'v', 's', 'x', -1})
#define MOVZX_INSTRUCTION ((int32_t[]){'m', 'o', 'v', 'z', 'x', -1})
#define BOUND_INSTRUCTION ((int32_t[]){'b', 'o', 'u', 'n', 'd', -1})
#define LAR_INSTRUCTION ((int32_t[]){'l', 'a', 'r', -1})
#define LSL_INSTRUCTION ((int32_t[]){'l', 's', 'l', -1})
#define LGDT_INSTRUCTION ((int32_t[]){'l', 'g', 'd', 't', -1})
#define LIDT_INSTRUCTION ((int32_t[]){'l', 'i', 'd', 't', -1})
#define SGDT_INSTRUCTION ((int32_t[]){'s', 'g', 'd', 't', -1})
#define SIDT_INSTRUCTION ((int32_t[]){'s', 'i', 'd', 't', -1})
#define LLDT_INSTRUCTION ((int32_t[]){'l', 'l', 'd', 't', -1})
#define SLDT_INSTRUCTION ((int32_t[]){'s', 'l', 'd', 't', -1})
#define LTR_INSTRUCTION ((int32_t[]){'l', 't', 'r', -1})
#define STR_INSTRUCTION ((int32_t[]){'s', 't', 'r', -1})
#define LMSW_INSTRUCTION ((int32_t[]){'l', 'm', 's', 'w', -1})
#define SMSW_INSTRUCTION ((int32_t[]){'s', 'm', 's', 'w', -1})
#define VERR_INSTRUCTION ((int32_t[]){'v', 'e', 'r', 'r', -1})
#define VERW_INSTRUCTION ((int32_t[]){'v', 'e', 'r', 'w', -1})
#define JMP_INSTRUCTION ((int32_t[]){'j', 'm', 'p', -1})
#define CALL_INSTRUCTION ((int32_t[]){'c', 'a', 'l', 'l', -1})
#define RET_INSTRUCTION ((int32_t[]){'r', 'e', 't', -1})
#define RETN_INSTRUCTION ((int32_t[]){'r', 'e', 't', 'n', -1})
#define RETF_INSTRUCTION ((int32_t[]){'r', 'e', 't', 'f', -1})
#define ENTER_INSTRUCTION ((int32_t[]){'e', 'n', 't', 'e', 'r', -1})
#define LEAVE_INSTRUCTION ((int32_t[]){'l', 'e', 'a', 'v', 'e', -1})
#define XCHG_INSTRUCTION ((int32_t[]){'x', 'c', 'h', 'g', -1})
#define PUSH_INSTRUCTION ((int32_t[]){'p', 'u', 's', 'h', -1})
#define POP_INSTRUCTION ((int32_t[]){'p', 'o', 'p', -1})
#define PUSHA_INSTRUCTION ((int32_t[]){'p', 'u', 's', 'h', 'a', -1})
#define PUSHAW_INSTRUCTION ((int32_t[]){'p', 'u', 's', 'h', 'a', 'w', -1})
#define PUSHAD_INSTRUCTION ((int32_t[]){'p', 'u', 's', 'h', 'a', 'd', -1})
#define POPA_INSTRUCTION ((int32_t[]){'p', 'o', 'p', 'a', -1})
#define POPAW_INSTRUCTION ((int32_t[]){'p', 'o', 'p', 'a', 'w', -1})
#define POPAD_INSTRUCTION ((int32_t[]){'p', 'o', 'p', 'a', 'd', -1})
#define PUSHF_INSTRUCTION ((int32_t[]){'p', 'u', 's', 'h', 'f', -1})
#define PUSHFD_INSTRUCTION ((int32_t[]){'p', 'u', 's', 'h', 'f', 'd', -1})
#define POPF_INSTRUCTION ((int32_t[]){'p', 'o', 'p', 'f', -1})
#define POPFD_INSTRUCTION ((int32_t[]){'p', 'o', 'p', 'f', 'd', -1})
#define LAHF_INSTRUCTION ((int32_t[]){'l', 'a', 'h', 'f', -1})
#define SAHF_INSTRUCTION ((int32_t[]){'s', 'a', 'h', 'f', -1})
#define XLAT_INSTRUCTION ((int32_t[]){'x', 'l', 'a', 't', -1})
#define XLATB_INSTRUCTION ((int32_t[]){'x', 'l', 'a', 't', 'b', -1})
#define MOVS_INSTRUCTION ((int32_t[]){'m', 'o', 'v', 's', -1})
#define MOVSB_INSTRUCTION ((int32_t[]){'m', 'o', 'v', 's', 'b', -1})
#define MOVSW_INSTRUCTION ((int32_t[]){'m', 'o', 'v', 's', 'w', -1})
#define MOVSD_INSTRUCTION ((int32_t[]){'m', 'o', 'v', 's', 'd', -1})
#define CMPS_INSTRUCTION ((int32_t[]){'c', 'm', 'p', 's', -1})
#define CMPSB_INSTRUCTION ((int32_t[]){'c', 'm', 'p', 's', 'b', -1})
#define CMPSW_INSTRUCTION ((int32_t[]){'c', 'm', 'p', 's', 'w', -1})
#define CMPSD_INSTRUCTION ((int32_t[]){'c', 'm', 'p', 's', 'd', -1})
#define SCAS_INSTRUCTION ((int32_t[]){'s', 'c', 'a', 's', -1})
#define SCASB_INSTRUCTION ((int32_t[]){'s', 'c', 'a', 's', 'b', -1})
#define SCASW_INSTRUCTION ((int32_t[]){'s', 'c', 'a', 's', 'w', -1})
#define SCASD_INSTRUCTION ((int32_t[]){'s', 'c', 'a', 's', 'd', -1})
#define LODS_INSTRUCTION ((int32_t[]){'l', 'o', 'd', 's', -1})
#define LODSB_INSTRUCTION ((int32_t[]){'l', 'o', 'd', 's', 'b', -1})
#define LODSW_INSTRUCTION ((int32_t[]){'l', 'o', 'd', 's', 'w', -1})
#define LODSD_INSTRUCTION ((int32_t[]){'l', 'o', 'd', 's', 'd', -1})
#define STOS_INSTRUCTION ((int32_t[]){'s', 't', 'o', 's', -1})
#define STOSB_INSTRUCTION ((int32_t[]){'s', 't', 'o', 's', 'b', -1})
#define STOSW_INSTRUCTION ((int32_t[]){'s', 't', 'o', 's', 'w', -1})
#define STOSD_INSTRUCTION ((int32_t[]){'s', 't', 'o', 's', 'd', -1})
#define INS_INSTRUCTION ((int32_t[]){'i', 'n', 's', -1})
#define INSB_INSTRUCTION ((int32_t[]){'i', 'n', 's', 'b', -1})
#define INSW_INSTRUCTION ((int32_t[]){'i', 'n', 's', 'w', -1})
#define INSD_INSTRUCTION ((int32_t[]){'i', 'n', 's', 'd', -1})
#define OUTS_INSTRUCTION ((int32_t[]){'o', 'u', 't', 's', -1})
#define OUTSB_INSTRUCTION ((int32_t[]){'o', 'u', 't', 's', 'b', -1})
#define OUTSW_INSTRUCTION ((int32_t[]){'o', 'u', 't', 's', 'w', -1})
#define OUTSD_INSTRUCTION ((int32_t[]){'o', 'u', 't', 's', 'd', -1})
#define LOCK_INSTRUCTION ((int32_t[]){'l', 'o', 'c', 'k', -1})
#define REP_INSTRUCTION ((int32_t[]){'r', 'e', 'p', -1})
#define REPE_INSTRUCTION ((int32_t[]){'r', 'e', 'p', 'e', -1})
#define REPZ_INSTRUCTION ((int32_t[]){'r', 'e', 'p', 'z', -1})
#define REPNE_INSTRUCTION ((int32_t[]){'r', 'e', 'p', 'n', 'e', -1})
#define REPNZ_INSTRUCTION ((int32_t[]){'r', 'e', 'p', 'n', 'z', -1})
#define O16_INSTRUCTION ((int32_t[]){'o', '1', '6', -1})
#define O32_INSTRUCTION ((int32_t[]){'o', '3', '2', -1})
#define A16_INSTRUCTION ((int32_t[]){'a', '1', '6', -1})
#define A32_INSTRUCTION ((int32_t[]){'a', '3', '2', -1})
#define IN_INSTRUCTION ((int32_t[]){'i', 'n', -1})
#define OUT_INSTRUCTION ((int32_t[]){'o', 'u', 't', -1})
#define INT_INSTRUCTION ((int32_t[]){'i', 'n', 't', -1})
#define INTO_INSTRUCTION ((int32_t[]){'i', 'n', 't', 'o', -1})
#define IRET_INSTRUCTION ((int32_t[]){'i', 'r', 'e', 't', -1})
#define IRETD_INSTRUCTION ((int32_t[]){'i', 'r', 'e', 't', 'd', -1})
#define LOOP_INSTRUCTION ((int32_t[]){'l', 'o', 'o', 'p', -1})
#define LOOPE_INSTRUCTION ((int32_t[]){'l', 'o', 'o', 'p', 'e', -1})
#define LOOPZ_INSTRUCTION ((int32_t[]){'l', 'o', 'o', 'p', 'z', -1})
#define LOOPNE_INSTRUCTION ((int32_t[]){'l', 'o', 'o', 'p', 'n', 'e', -1})
#define LOOPNZ_INSTRUCTION ((int32_t[]){'l', 'o', 'o', 'p', 'n', 'z', -1})
#define JCXZ_INSTRUCTION ((int32_t[]){'j', 'c', 'x', 'z', -1})
#define JECXZ_INSTRUCTION ((int32_t[]){'j', 'e', 'c', 'x', 'z', -1})
#define JO_INSTRUCTION ((int32_t[]){'j', 'o', -1})
#define JNO_INSTRUCTION ((int32_t[]){'j', 'n', 'o', -1})
#define JB_INSTRUCTION ((int32_t[]){'j', 'b', -1})
#define JC_INSTRUCTION ((int32_t[]){'j', 'c', -1})
#define JNAE_INSTRUCTION ((int32_t[]){'j', 'n', 'a', 'e', -1})
#define JNB_INSTRUCTION ((int32_t[]){'j', 'n', 'b', -1})
#define JAE_INSTRUCTION ((int32_t[]){'j', 'a', 'e', -1})
#define JNC_INSTRUCTION ((int32_t[]){'j', 'n', 'c', -1})
#define JE_INSTRUCTION ((int32_t[]){'j', 'e', -1})
#define JZ_INSTRUCTION ((int32_t[]){'j', 'z', -1})
#define JNE_INSTRUCTION ((int32_t[]){'j', 'n', 'e', -1})
#define JNZ_INSTRUCTION ((int32_t[]){'j', 'n', 'z', -1})
#define JBE_INSTRUCTION ((int32_t[]){'j', 'b', 'e', -1})
#define JNA_INSTRUCTION ((int32_t[]){'j', 'n', 'a', -1})
#define JA_INSTRUCTION ((int32_t[]){'j', 'a', -1})
#define JNBE_INSTRUCTION ((int32_t[]){'j', 'n', 'b', 'e', -1})
#define JS_INSTRUCTION ((int32_t[]){'j', 's', -1})
#define JNS_INSTRUCTION ((int32_t[]){'j', 'n', 's', -1})
#define JP_INSTRUCTION ((int32_t[]){'j', 'p', -1})
#define JPE_INSTRUCTION ((int32_t[]){'j', 'p', 'e', -1})
#define JNP_INSTRUCTION ((int32_t[]){'j', 'n', 'p', -1})
#define JPO_INSTRUCTION ((int32_t[]){'j', 'p', 'o', -1})
#define JL_INSTRUCTION ((int32_t[]){'j', 'l', -1})
#define JNGE_INSTRUCTION ((int32_t[]){'j', 'n', 'g', 'e', -1})
#define JGE_INSTRUCTION ((int32_t[]){'j', 'g', 'e', -1})
#define JNL_INSTRUCTION ((int32_t[]){'j', 'n', 'l', -1})
#define JLE_INSTRUCTION ((int32_t[]){'j', 'l', 'e', -1})
#define JNG_INSTRUCTION ((int32_t[]){'j', 'n', 'g', -1})
#define JG_INSTRUCTION ((int32_t[]){'j', 'g', -1})
#define JNLE_INSTRUCTION ((int32_t[]){'j', 'n', 'l', 'e', -1})
#define SETA_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'a', -1})
#define SETAE_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'a', 'e', -1})
#define SETB_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'b', -1})
#define SETBE_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'b', 'e', -1})
#define SETC_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'c', -1})
#define SETE_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'e', -1})
#define SETG_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'g', -1})
#define SETGE_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'g', 'e', -1})
#define SETL_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'l', -1})
#define SETLE_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'l', 'e', -1})
#define SETNA_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'n', 'a', -1})
#define SETNAE_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'n', 'a', 'e', -1})
#define SETNB_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'n', 'b', -1})
#define SETNBE_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'n', 'b', 'e', -1})
#define SETNC_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'n', 'c', -1})
#define SETNE_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'n', 'e', -1})
#define SETNG_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'n', 'g', -1})
#define SETNGE_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'n', 'g', 'e', -1})
#define SETNL_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'n', 'l', -1})
#define SETNLE_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'n', 'l', 'e', -1})
#define SETNO_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'n', 'o', -1})
#define SETNP_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'n', 'p', -1})
#define SETNS_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'n', 's', -1})
#define SETNZ_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'n', 'z', -1})
#define SETO_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'o', -1})
#define SETP_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'p', -1})
#define SETPE_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'p', 'e', -1})
#define SETPO_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'p', 'o', -1})
#define SETS_INSTRUCTION ((int32_t[]){'s', 'e', 't', 's', -1})
#define SETZ_INSTRUCTION ((int32_t[]){'s', 'e', 't', 'z', -1})
#define ARPL_INSTRUCTION ((int32_t[]){'a', 'r', 'p', 'l', -1})
#define ADD_INSTRUCTION ((int32_t[]){'a', 'd', 'd', -1})
#define ADC_INSTRUCTION ((int32_t[]){'a', 'd', 'c', -1})
#define SUB_INSTRUCTION ((int32_t[]){'s', 'u', 'b', -1})
#define SBB_INSTRUCTION ((int32_t[]){'s', 'b', 'b', -1})
#define CMP_INSTRUCTION ((int32_t[]){'c', 'm', 'p', -1})
#define INC_INSTRUCTION ((int32_t[]){'i', 'n', 'c', -1})
#define DEC_INSTRUCTION ((int32_t[]){'d', 'e', 'c', -1})
#define NEG_INSTRUCTION ((int32_t[]){'n', 'e', 'g', -1})
#define MUL_INSTRUCTION ((int32_t[]){'m', 'u', 'l', -1})
#define IMUL_INSTRUCTION ((int32_t[]){'i', 'm', 'u', 'l', -1})
#define DIV_INSTRUCTION ((int32_t[]){'d', 'i', 'v', -1})
#define IDIV_INSTRUCTION ((int32_t[]){'i', 'd', 'i', 'v', -1})
#define CBW_INSTRUCTION ((int32_t[]){'c', 'b', 'w', -1})
#define CWD_INSTRUCTION ((int32_t[]){'c', 'w', 'd', -1})
#define CWDE_INSTRUCTION ((int32_t[]){'c', 'w', 'd', 'e', -1})
#define CDQ_INSTRUCTION ((int32_t[]){'c', 'd', 'q', -1})
#define AAA_INSTRUCTION ((int32_t[]){'a', 'a', 'a', -1})
#define AAD_INSTRUCTION ((int32_t[]){'a', 'a', 'd', -1})
#define AAM_INSTRUCTION ((int32_t[]){'a', 'a', 'm', -1})
#define AAS_INSTRUCTION ((int32_t[]){'a', 'a', 's', -1})
#define DAA_INSTRUCTION ((int32_t[]){'d', 'a', 'a', -1})
#define DAS_INSTRUCTION ((int32_t[]){'d', 'a', 's', -1})
#define AND_INSTRUCTION ((int32_t[]){'a', 'n', 'd', -1})
#define OR_INSTRUCTION ((int32_t[]){'o', 'r', -1})
#define XOR_INSTRUCTION ((int32_t[]){'x', 'o', 'r', -1})
#define TEST_INSTRUCTION ((int32_t[]){'t', 'e', 's', 't', -1})
#define NOT_INSTRUCTION ((int32_t[]){'n', 'o', 't', -1})
#define BSF_INSTRUCTION ((int32_t[]){'b', 's', 'f', -1})
#define BSR_INSTRUCTION ((int32_t[]){'b', 's', 'r', -1})
#define BT_INSTRUCTION ((int32_t[]){'b', 't', -1})
#define BTC_INSTRUCTION ((int32_t[]){'b', 't', 'c', -1})
#define BTR_INSTRUCTION ((int32_t[]){'b', 't', 'r', -1})
#define BTS_INSTRUCTION ((int32_t[]){'b', 't', 's', -1})
#define CLC_INSTRUCTION ((int32_t[]){'c', 'l', 'c', -1})
#define CLTS_INSTRUCTION ((int32_t[]){'c', 'l', 't', 's', -1})
#define STC_INSTRUCTION ((int32_t[]){'s', 't', 'c', -1})
#define CMC_INSTRUCTION ((int32_t[]){'c', 'm', 'c', -1})
#define CLD_INSTRUCTION ((int32_t[]){'c', 'l', 'd', -1})
#define STD_INSTRUCTION ((int32_t[]){'s', 't', 'd', -1})
#define CLI_INSTRUCTION ((int32_t[]){'c', 'l', 'i', -1})
#define STI_INSTRUCTION ((int32_t[]){'s', 't', 'i', -1})
#define HLT_INSTRUCTION ((int32_t[]){'h', 'l', 't', -1})
#define WAIT_INSTRUCTION ((int32_t[]){'w', 'a', 'i', 't', -1})
#define FWAIT_INSTRUCTION ((int32_t[]){'f', 'w', 'a', 'i', 't', -1})
#define ESC_INSTRUCTION ((int32_t[]){'e', 's', 'c', -1})
#define NOP_INSTRUCTION ((int32_t[]){'n', 'o', 'p', -1})
#define SHL_INSTRUCTION ((int32_t[]){'s', 'h', 'l', -1})
#define SAL_INSTRUCTION ((int32_t[]){'s', 'a', 'l', -1})
#define SHR_INSTRUCTION ((int32_t[]){'s', 'h', 'r', -1})
#define SAR_INSTRUCTION ((int32_t[]){'s', 'a', 'r', -1})
#define SHLD_INSTRUCTION ((int32_t[]){'s', 'h', 'l', 'd', -1})
#define SHRD_INSTRUCTION ((int32_t[]){'s', 'h', 'r', 'd', -1})
#define ROL_INSTRUCTION ((int32_t[]){'r', 'o', 'l', -1})
#define ROR_INSTRUCTION ((int32_t[]){'r', 'o', 'r', -1})
#define RCL_INSTRUCTION ((int32_t[]){'r', 'c', 'l', -1})
#define RCR_INSTRUCTION ((int32_t[]){'r', 'c', 'r', -1})

nint _86_exec(struct Parser* p, struct AstInstruction* inst);

#ifdef i386
#undef i386
#endif

ASM_LANG i386 = {
    .lang_name = "i386 (16 bit) assembly v1.0.0",
    .code_name = "i386",
    .max_addr = 0Xffff,
    .size_of_ptr = 2,
    .le = 1,

    .exec = _86_exec,
};

static nint fail_unresolved_label(struct Parser *p, struct token *_token) {
    (void)unresolved_label(p, _token);
    return INSTRUCTION_FAILED;
}

static int32_t *registers[] = {
    (int32_t[]){'a', 'x', -1}, (int32_t[]){'b', 'x', -1},
    (int32_t[]){'c', 'x', -1}, (int32_t[]){'d', 'x', -1},
    (int32_t[]){'s', 'p', -1}, (int32_t[]){'b', 'p', -1},
    (int32_t[]){'s', 'i', -1}, (int32_t[]){'d', 'i', -1},
    (int32_t[]){'i', 'p', -1}, (int32_t[]){'c', 's', -1},
    (int32_t[]){'d', 's', -1}, (int32_t[]){'e', 's', -1},
    (int32_t[]){'s', 's', -1}, (int32_t[]){'a', 'l', -1},
    (int32_t[]){'a', 'h', -1}, (int32_t[]){'b', 'l', -1},
    (int32_t[]){'b', 'h', -1}, (int32_t[]){'c', 'l', -1},
    (int32_t[]){'c', 'h', -1}, (int32_t[]){'d', 'l', -1},
    (int32_t[]){'d', 'h', -1}, NULL};

static int32_t *reg8_names[] = {
    (int32_t[]){'a', 'l', -1}, (int32_t[]){'c', 'l', -1},
    (int32_t[]){'d', 'l', -1}, (int32_t[]){'b', 'l', -1},
    (int32_t[]){'a', 'h', -1}, (int32_t[]){'c', 'h', -1},
    (int32_t[]){'d', 'h', -1}, (int32_t[]){'b', 'h', -1}, NULL};

static int32_t *reg16_names[] = {
    (int32_t[]){'a', 'x', -1}, (int32_t[]){'c', 'x', -1},
    (int32_t[]){'d', 'x', -1}, (int32_t[]){'b', 'x', -1},
    (int32_t[]){'s', 'p', -1}, (int32_t[]){'b', 'p', -1},
    (int32_t[]){'s', 'i', -1}, (int32_t[]){'d', 'i', -1}, NULL};

static int32_t *reg32_names[] = {
    (int32_t[]){'e', 'a', 'x', -1}, (int32_t[]){'e', 'c', 'x', -1},
    (int32_t[]){'e', 'd', 'x', -1}, (int32_t[]){'e', 'b', 'x', -1},
    (int32_t[]){'e', 's', 'p', -1}, (int32_t[]){'e', 'b', 'p', -1},
    (int32_t[]){'e', 's', 'i', -1}, (int32_t[]){'e', 'd', 'i', -1}, NULL};

static int32_t *seg_names[] = {
    (int32_t[]){'e', 's', -1}, (int32_t[]){'c', 's', -1},
    (int32_t[]){'s', 's', -1}, (int32_t[]){'d', 's', -1},
    (int32_t[]){'f', 's', -1}, (int32_t[]){'g', 's', -1}, NULL};

static int32_t *cr_names[] = {
    (int32_t[]){'c', 'r', '0', -1}, (int32_t[]){'c', 'r', '1', -1},
    (int32_t[]){'c', 'r', '2', -1}, (int32_t[]){'c', 'r', '3', -1},
    (int32_t[]){'c', 'r', '4', -1}, (int32_t[]){'c', 'r', '5', -1},
    (int32_t[]){'c', 'r', '6', -1}, (int32_t[]){'c', 'r', '7', -1}, NULL};

static int32_t *dr_names[] = {
    (int32_t[]){'d', 'r', '0', -1}, (int32_t[]){'d', 'r', '1', -1},
    (int32_t[]){'d', 'r', '2', -1}, (int32_t[]){'d', 'r', '3', -1},
    (int32_t[]){'d', 'r', '4', -1}, (int32_t[]){'d', 'r', '5', -1},
    (int32_t[]){'d', 'r', '6', -1}, (int32_t[]){'d', 'r', '7', -1}, NULL};

static int32_t *tr_names[] = {
    (int32_t[]){'t', 'r', '0', -1}, (int32_t[]){'t', 'r', '1', -1},
    (int32_t[]){'t', 'r', '2', -1}, (int32_t[]){'t', 'r', '3', -1},
    (int32_t[]){'t', 'r', '4', -1}, (int32_t[]){'t', 'r', '5', -1},
    (int32_t[]){'t', 'r', '6', -1}, (int32_t[]){'t', 'r', '7', -1}, NULL};

#define BYTE_TYPE ((int32_t[]){'b', 'y', 't', 'e', -1})
#define WORD_TYPE ((int32_t[]){'w', 'o', 'r', 'd', -1})
#define DWORD_TYPE ((int32_t[]){'d', 'w', 'o', 'r', 'd', -1})
#define SHORT_TYPE ((int32_t[]){'s', 'h', 'o', 'r', 't', -1})
#define NEAR_TYPE ((int32_t[]){'n', 'e', 'a', 'r', -1})
#define FAR_TYPE ((int32_t[]){'f', 'a', 'r', -1})
#define PTR_NAME ((int32_t[]){'p', 't', 'r', -1})

enum OperandKind {
    OPERAND_INVALID,
    OPERAND_REG8,
    OPERAND_REG16,
    OPERAND_REG32,
    OPERAND_SEG,
    OPERAND_CR,
    OPERAND_DR,
    OPERAND_TR,
    OPERAND_IMM,
    OPERAND_MEM
};

struct MemoryOperand {
    nint addr_size;
    nint rm;
    unint direct;
    nint disp;
    unint has_disp;
    nint seg_override;
    nint base;
    nint index;
    nint scale;
};

struct Operand {
    enum OperandKind kind;
    nint size;
    nint reg;
    nint imm;
    struct MemoryOperand mem;
};

static unint tks_at_end(struct TokenStream *tks) {
    struct token *tok = tks_peek(tks);
    tks_reset_peek(tks);
    return tok == NULL;
}

static void record_error_token(struct token **error_token, struct token *tok) {
    if(error_token && tok && (!*error_token || tok > *error_token)) {
        *error_token = tok;
    }
}

static unint expect_tks_end(struct TokenStream *tks, struct token **error_token) {
    struct token *tok = tks_peek(tks);
    tks_reset_peek(tks);
    if(!tok) return 1;
    record_error_token(error_token, tok);
    return 0;
}

static unint read_token_type(struct Parser *p, struct TokenStream *tks, unint type,
                             struct token **error_token) {
    struct token *tok = tks_read(tks);
    if(!expected_token(p, tok, type)) {
        record_error_token(error_token, tok);
        return 0;
    }
    record_error_token(error_token, tok);
    return 1;
}

static unint token_matches(struct token *tok, int32_t **names, unint *idx) {
    if(!tok || tok->type != NAME) return 0;
    for(unint i = 0; names[i]; i++) {
        if(compare_identifiers_cp_array(tok, names[i]) == SUCCESS) {
            *idx = i;
            return 1;
        }
    }
    return 0;
}

static unint parse_register_exact(struct InstructionArg *arg, int32_t **names,
                                  unint *idx, struct token **error_token) {
    struct TokenStream tks;
    tks_init(&tks, arg->_s, arg->_e);
    if(parse_potential_register(&tks, names, idx) != SUCCESS) return 0;
    record_error_token(error_token, arg->_s);
    return expect_tks_end(&tks, error_token);
}

static unint read_integer(struct Parser *p, struct TokenStream *tks, nint *out,
                          struct token **err_token) {
    unint is_neg = 0;
    struct token *tok = tks_read(tks);
    if(tok && tok->type == MINUS) {
        is_neg = 1;
        record_error_token(err_token, tok);
        tok = tks_read(tks);
    }

    if(!tok || tok->type != NUMBER) {
        return 0;
    }

    record_error_token(err_token, tok);
    struct Value *v = new_number(p, tok, is_neg);
    if(!v) return 0;
    if(v->type != VALUE_INT) {
        _error_from_token(p, tok, ERROR_TYPE_TYPE, "invalid type");
        return 0;
    }

    *out = v->val.number;
    return 1;
}

static unint parse_immediate(struct Parser *p, struct InstructionArg *arg,
                             struct Operand *op, struct token **error_token) {
    struct TokenStream tks;
    nint value;

    tks_init(&tks, arg->_s, arg->_e);
    if(!read_integer(p, &tks, &value, error_token)) return 0;
    if(!expect_tks_end(&tks, error_token)) return 0;

    op->kind = OPERAND_IMM;
    op->imm = value;
    return 1;
}

static nint mem_reg_class(unint reg) {
    switch(reg) {
        case 3: return 1; /* bx */
        case 5: return 1; /* bp */
        case 6: return 2; /* si */
        case 7: return 2; /* di */
        default: return 0;
    }
}

static unint rm_code_from_regs(unint first, nint second, nint *rm) {
    if(second < 0) {
        switch(first) {
            case 6: *rm = 4; return 1; /* si */
            case 7: *rm = 5; return 1; /* di */
            case 5: *rm = 6; return 1; /* bp */
            case 3: *rm = 7; return 1; /* bx */
            default: return 0;
        }
    }

    if(first == 3 && second == 6) { *rm = 0; return 1; }
    if(first == 3 && second == 7) { *rm = 1; return 1; }
    if(first == 5 && second == 6) { *rm = 2; return 1; }
    if(first == 5 && second == 7) { *rm = 3; return 1; }
    return 0;
}

static unint read_memory_displacement(struct Parser *p, struct TokenStream *tks,
                                      nint sign, nint *disp,
                                      struct token **error_token) {
    nint value;

    if(!read_integer(p, tks, &value, error_token)) return 0;
    *disp = sign < 0 ? -value : value;
    return 1;
}

static nint explicit_type_size(struct token *tok) {
    if(compare_identifiers_cp_array(tok, BYTE_TYPE) == SUCCESS) return 1;
    if(compare_identifiers_cp_array(tok, WORD_TYPE) == SUCCESS) return 2;
    if(compare_identifiers_cp_array(tok, DWORD_TYPE) == SUCCESS) return 4;
    return 0;
}

static void set_direct_memory(struct Operand *op, nint addr, nint seg_override) {
    op->kind = OPERAND_MEM;
    op->mem.addr_size = 2;
    op->mem.rm = 6;
    op->mem.direct = 1;
    op->mem.disp = addr;
    op->mem.has_disp = 1;
    op->mem.seg_override = seg_override;
    op->mem.base = -1;
    op->mem.index = -1;
    op->mem.scale = 0;
}

static nint scale_to_bits(nint scale) {
    if(scale == 1) return 0;
    if(scale == 2) return 1;
    if(scale == 4) return 2;
    if(scale == 8) return 3;
    return -1;
}

static unint read_scale(struct Parser *p, struct TokenStream *tks, nint *scale,
                        struct token **error_token) {
    nint value;
    if(!read_integer(p, tks, &value, error_token)) return 0;
    if(scale_to_bits(value) < 0) return 0;
    *scale = value;
    return 1;
}

static void set_memory32(struct Operand *op, nint base, nint index, nint scale,
                         nint disp, unint has_disp, nint seg_override) {
    op->kind = OPERAND_MEM;
    op->mem.addr_size = 4;
    op->mem.rm = base >= 0 ? base : 5;
    op->mem.direct = 0;
    op->mem.disp = disp;
    op->mem.has_disp = has_disp;
    op->mem.seg_override = seg_override;
    op->mem.base = base;
    op->mem.index = index;
    op->mem.scale = scale_to_bits(scale);
}

static nint parse_bracketed_variable(struct Parser *p, struct TokenStream *tks,
                                     struct token *start, struct Operand *op,
                                     nint seg_override,
                                     struct token **error_token) {
    struct TokenStream var_tks;
    nint addr = 0;
    unint status;

    tks_init(&var_tks, start, start);
    status = parse_potential_variable(p, &var_tks, &addr, 0);

    if(status == VP_FAIL) return INSTRUCTION_FAILED;
    if(status == VP_UNRESOLVED_LABEL) {
        record_error_token(error_token, start);
        if(p->last_pass) return fail_unresolved_label(p, start);
        if(!read_token_type(p, tks, RSQB, error_token) ||
           !expect_tks_end(tks, error_token)) return INSTRUCTION_FAILED;
        set_direct_memory(op, 0, seg_override);
        return 1;
    }
    if(status != VP_SUCCESS) return 0;
    record_error_token(error_token, start);
    if(!read_token_type(p, tks, RSQB, error_token) ||
       !expect_tks_end(tks, error_token)) return INSTRUCTION_FAILED;

    set_direct_memory(op, addr, seg_override);
    return 1;
}

static nint parse_memory32_register_term(struct Parser *p, struct TokenStream *tks,
                                         unint reg, nint *base, nint *index,
                                         nint *scale,
                                         struct token **error_token) {
    struct token *tok = tks_read(tks);
    nint term_scale = 1;
    (void)p;

    if(tok && tok->type == STAR) {
        record_error_token(error_token, tok);
        if(!read_scale(p, tks, &term_scale, error_token)) return 0;
    } else if(tok) {
        tks->read = tok;
        tks_reset_peek(tks);
    }

    if(term_scale != 1) {
        if(reg == 4) return 0;
        if(*index >= 0) return 0;
        *index = (nint)reg;
        *scale = term_scale;
        return 1;
    }

    if(*base < 0) {
        *base = (nint)reg;
        return 1;
    }

    if(reg == 4) return 0;
    if(*index >= 0) return 0;
    *index = (nint)reg;
    *scale = 1;
    return 1;
}

static nint finish_memory32_operand(struct Parser *p, struct TokenStream *tks,
                                    unint first, struct Operand *op,
                                    nint seg_override,
                                    struct token **error_token) {
    nint base = -1;
    nint index = -1;
    nint scale = 1;
    nint disp = 0;
    unint has_disp = 0;
    struct token *tok;

    if(!parse_memory32_register_term(p, tks, first, &base, &index, &scale,
                                     error_token)) return 0;

    tok = tks_read(tks);
    while(tok && (tok->type == PLUS || tok->type == MINUS)) {
        nint sign = tok->type == MINUS ? -1 : 1;
        record_error_token(error_token, tok);
        tok = tks_read(tks);
        if(!tok) return 0;

        if(tok->type == NAME) {
            unint reg;
            if(sign < 0) {
                record_error_token(error_token, tok);
                return 0;
            }
            if(!token_matches(tok, reg32_names, &reg)) {
                record_error_token(error_token, tok);
                return 0;
            }
            record_error_token(error_token, tok);
            if(!parse_memory32_register_term(p, tks, reg, &base, &index, &scale,
                                             error_token)) return 0;
        } else if(tok->type == NUMBER) {
            nint value;
            tks->read = tok;
            tks_reset_peek(tks);
            if(!read_integer(p, tks, &value, error_token)) return 0;
            disp += sign < 0 ? -value : value;
            has_disp = 1;
        } else {
            record_error_token(error_token, tok);
            return 0;
        }

        tok = tks_read(tks);
    }

    if(!tok || tok->type != RSQB) {
        record_error_token(error_token, tok);
        return 0;
    }
    record_error_token(error_token, tok);
    if(!expect_tks_end(tks, error_token)) return 0;

    set_memory32(op, base, index, scale, disp, has_disp, seg_override);
    return 1;
}

static nint finish_memory_operand(struct Parser *p, struct TokenStream *tks,
                                  struct Operand *op, nint seg_override,
                                  struct token **error_token) {
    struct token *tok = tks_read(tks);
    unint first;
    unint first32;
    nint second = -1;
    nint rm;
    nint disp = 0;
    unint has_disp = 0;

    if(!tok) return 0;

    if(tok->type == NUMBER || tok->type == MINUS) {
        tks->read = tok;
        if(!read_integer(p, tks, &disp, error_token)) return 0;
        if(!read_token_type(p, tks, RSQB, error_token) ||
           !expect_tks_end(tks, error_token)) return 0;

        set_direct_memory(op, disp, seg_override);
        return 1;
    }

    if(token_matches(tok, reg32_names, &first32)) {
        record_error_token(error_token, tok);
        return finish_memory32_operand(p, tks, first32, op, seg_override,
                                       error_token);
    }

    if(!token_matches(tok, reg16_names, &first) || !mem_reg_class(first)) {
        if(tok->type == NAME) {
            return parse_bracketed_variable(p, tks, tok, op, seg_override,
                                            error_token);
        }
        record_error_token(error_token, tok);
        return 0;
    }
    record_error_token(error_token, tok);

    tok = tks_read(tks);
    if(!tok) return 0;

    if(tok->type == PLUS || tok->type == MINUS) {
        nint sign = tok->type == MINUS ? -1 : 1;
        record_error_token(error_token, tok);
        tok = tks_read(tks);
        if(!tok) return 0;

        if(tok->type == NAME) {
            unint reg;
            if(sign < 0) {
                record_error_token(error_token, tok);
                return 0;
            }
            if(!token_matches(tok, reg16_names, &reg) || !mem_reg_class(reg)) {
                record_error_token(error_token, tok);
                return 0;
            }
            if(mem_reg_class(first) != 1 || mem_reg_class(reg) != 2) return 0;
            record_error_token(error_token, tok);
            second = (nint)reg;

            tok = tks_read(tks);
            if(tok && (tok->type == PLUS || tok->type == MINUS)) {
                sign = tok->type == MINUS ? -1 : 1;
                record_error_token(error_token, tok);
                if(!read_memory_displacement(p, tks, sign, &disp, error_token)) return 0;
                has_disp = 1;
                tok = tks_read(tks);
            }
        } else if(tok->type == NUMBER) {
            tks->read = tok;
            if(!read_memory_displacement(p, tks, sign, &disp, error_token)) return 0;
            has_disp = 1;
            tok = tks_read(tks);
        } else {
            record_error_token(error_token, tok);
            return 0;
        }
    }

    if(!tok || tok->type != RSQB) {
        record_error_token(error_token, tok);
        return 0;
    }
    record_error_token(error_token, tok);
    if(!expect_tks_end(tks, error_token)) return 0;
    if(!rm_code_from_regs(first, second, &rm)) return 0;

    op->kind = OPERAND_MEM;
    op->mem.addr_size = 2;
    op->mem.rm = rm;
    op->mem.direct = 0;
    op->mem.disp = disp;
    op->mem.has_disp = has_disp;
    op->mem.seg_override = seg_override;
    op->mem.base = -1;
    op->mem.index = -1;
    op->mem.scale = 0;
    return 1;
}

static nint parse_memory(struct Parser *p, struct InstructionArg *arg,
                         struct Operand *op, struct token **error_token) {
    struct TokenStream tks;
    struct token *tok;
    nint seg_override = -1;
    nint explicit_size = 0;

    tks_init(&tks, arg->_s, arg->_e);
    tok = tks_read(&tks);
    if(!tok) return 0;

    if(tok->type == NAME) {
        explicit_size = explicit_type_size(tok);
        if(explicit_size) {
            struct token *ptr_tok;

            record_error_token(error_token, tok);
            ptr_tok = tks_read(&tks);
            if(!expected_token(p, ptr_tok, NAME)) {
                record_error_token(error_token, ptr_tok);
                return INSTRUCTION_FAILED;
            }
            record_error_token(error_token, ptr_tok);
            if(compare_identifiers_cp_array(ptr_tok, PTR_NAME) != SUCCESS) {
                _error_from_token(p, ptr_tok, ERROR_TYPE_I386, "expected ptr");
                return INSTRUCTION_FAILED;
            }
            tok = tks_read(&tks);
        }
    }

    if(tok->type == NAME) {
        unint seg;
        struct token *colon;

        if(!token_matches(tok, seg_names, &seg)) return 0;
        record_error_token(error_token, tok);
        colon = tks_read(&tks);
        if(!colon || colon->type != COLON) return 0;
        record_error_token(error_token, colon);
        seg_override = (nint)seg;
        tok = tks_read(&tks);
    }

    if(!tok || tok->type != LSQB) return 0;
    record_error_token(error_token, tok);
    nint status = finish_memory_operand(p, &tks, op, seg_override, error_token);
    if(status > 0) op->size = explicit_size;
    return status;
}

static nint parse_variable_as_immediate(struct Parser *p, struct InstructionArg *arg,
                                        struct Operand *op,
                                        struct token **error_token) {
    struct TokenStream tks;
    nint addr = 0;
    unint status;

    tks_init(&tks, arg->_s, arg->_e);
    status = parse_potential_variable(p, &tks, &addr, 0);

    if(status == VP_FAIL) return -1;
    if(status == VP_UNRESOLVED_LABEL) {
        record_error_token(error_token, arg->_s);
        if(p->last_pass) return fail_unresolved_label(p, arg->_s);
        op->kind = OPERAND_IMM;
        op->imm = 0;
        return 1;
    }
    if(status != VP_SUCCESS) return 0;
    if(!tks_at_end(&tks)) return -1;
    record_error_token(error_token, arg->_e);

    op->kind = OPERAND_IMM;
    op->imm = addr;
    return 1;
}

static nint parse_operand(struct Parser *p, struct InstructionArg *arg,
                          struct Operand *op, struct token **error_token) {
    unint idx;
    nint memory_status;
    nint variable_status;

    op->kind = OPERAND_INVALID;
    op->size = 0;
    op->reg = 0;
    op->imm = 0;
    op->mem.addr_size = 2;
    op->mem.rm = 0;
    op->mem.direct = 0;
    op->mem.disp = 0;
    op->mem.has_disp = 0;
    op->mem.seg_override = -1;
    op->mem.base = -1;
    op->mem.index = -1;
    op->mem.scale = 0;
    record_error_token(error_token, arg->_s);

    if(parse_register_exact(arg, reg8_names, &idx, error_token)) {
        op->kind = OPERAND_REG8;
        op->size = 1;
        op->reg = (nint)idx;
        return 1;
    }
    if(parse_register_exact(arg, reg16_names, &idx, error_token)) {
        op->kind = OPERAND_REG16;
        op->size = 2;
        op->reg = (nint)idx;
        return 1;
    }
    if(parse_register_exact(arg, reg32_names, &idx, error_token)) {
        op->kind = OPERAND_REG32;
        op->size = 4;
        op->reg = (nint)idx;
        return 1;
    }
    if(parse_register_exact(arg, cr_names, &idx, error_token)) {
        op->kind = OPERAND_CR;
        op->size = 4;
        op->reg = (nint)idx;
        return 1;
    }
    if(parse_register_exact(arg, dr_names, &idx, error_token)) {
        op->kind = OPERAND_DR;
        op->size = 4;
        op->reg = (nint)idx;
        return 1;
    }
    if(parse_register_exact(arg, tr_names, &idx, error_token)) {
        op->kind = OPERAND_TR;
        op->size = 4;
        op->reg = (nint)idx;
        return 1;
    }
    if(parse_register_exact(arg, seg_names, &idx, error_token)) {
        op->kind = OPERAND_SEG;
        op->size = 2;
        op->reg = (nint)idx;
        return 1;
    }

    memory_status = parse_memory(p, arg, op, error_token);
    if(memory_status < 0) return memory_status;
    if(memory_status > 0) return 1;

    variable_status = parse_variable_as_immediate(p, arg, op, error_token);
    if(variable_status < 0) return variable_status;
    if(variable_status > 0) return 1;

    if(parse_immediate(p, arg, op, error_token)) return 1;

    return 0;
}

static unint fits_u8(nint value) {
    return value >= 0 && value <= 0xff;
}

static unint fits_u16(nint value) {
    return value >= -32768 && value <= 0xffff;
}

static unint fits_u32(nint value) {
    return value >= -2147483647 - 1 && value <= (nint)0xffffffff;
}

static unint fits_i8(nint value) {
    return value >= -128 && value <= 127;
}

static unint fits_byte_immediate(nint value) {
    return fits_i8(value) || fits_u8(value);
}

static unint fits_i16(nint value) {
    return value >= -32768 && value <= 32767;
}

static unint fits_i32(nint value) {
    return value >= -2147483647 - 1 && value <= 2147483647;
}

static unint fits_push_imm8(nint value) {
    nint low;
    nint word;

    if(fits_i8(value)) return 1;
    if(value < 0 || value > 0xffff) return 0;

    low = value & 0xff;
    word = value & 0xffff;
    if(low & 0x80) return word == (0xff00 | low);
    return word == low;
}

static unint fits_word_sign_extended_i8(nint value) {
    return fits_push_imm8(value);
}

static unint fits_dword_sign_extended_i8(nint value) {
    nint low;
    nint dword;

    if(fits_i8(value)) return 1;
    if(value < 0 || value > (nint)0xffffffff) return 0;

    low = value & 0xff;
    dword = value & (nint)0xffffffff;
    if(low & 0x80) return dword == ((nint)0xffffff00 | low);
    return dword == low;
}

static nint memory32_mod(struct MemoryOperand *mem);
static unint memory32_needs_sib(struct MemoryOperand *mem);
static nint memory32_disp_len(struct MemoryOperand *mem, nint mod);

static nint memory_tail_len(struct MemoryOperand *mem) {
    if(mem->addr_size == 4) {
        nint mod = memory32_mod(mem);
        return (memory32_needs_sib(mem) ? 1 : 0) + memory32_disp_len(mem, mod);
    }
    if(mem->direct) return 2;
    if(mem->rm == 6 && !mem->has_disp) return 1;
    if(!mem->has_disp) return 0;
    return fits_i8(mem->disp) ? 1 : 2;
}

static void emit_byte(struct Parser *p, unsigned char byte) {
    if(p->last_pass) OUT_FILE_WRITE_BYTE(byte);
}

static void emit_word(struct Parser *p, nint value) {
    if(p->last_pass) {
        OUT_FILE_WRITE_BYTE((unsigned char)(value & 0xff));
        OUT_FILE_WRITE_BYTE((unsigned char)((value >> 8) & 0xff));
    }
}

static void emit_dword(struct Parser *p, nint value) {
    if(p->last_pass) {
        OUT_FILE_WRITE_BYTE((unsigned char)(value & 0xff));
        OUT_FILE_WRITE_BYTE((unsigned char)((value >> 8) & 0xff));
        OUT_FILE_WRITE_BYTE((unsigned char)((value >> 16) & 0xff));
        OUT_FILE_WRITE_BYTE((unsigned char)((value >> 24) & 0xff));
    }
}

static void emit_segment_prefix(struct Parser *p, struct MemoryOperand *mem) {
    static const unsigned char prefixes[] = {0x26, 0x2e, 0x36, 0x3e, 0x64, 0x65};
    if(mem->seg_override >= 0) emit_byte(p, prefixes[mem->seg_override]);
}

static nint segment_prefix_len(struct MemoryOperand *mem) {
    return mem->seg_override >= 0 ? 1 : 0;
}

static void emit_address_size_prefix(struct Parser *p, struct MemoryOperand *mem) {
    if(mem->addr_size == 4) emit_byte(p, 0x67);
}

static nint address_size_prefix_len(struct MemoryOperand *mem) {
    return mem->addr_size == 4 ? 1 : 0;
}

static void emit_operand_size_prefix(struct Parser *p, nint operand_size) {
    if(operand_size == 4) emit_byte(p, 0x66);
}

static nint operand_size_prefix_len(nint operand_size) {
    return operand_size == 4 ? 1 : 0;
}

static void emit_memory_prefixes(struct Parser *p, struct MemoryOperand *mem) {
    emit_segment_prefix(p, mem);
    emit_address_size_prefix(p, mem);
}

static nint memory_prefix_len(struct MemoryOperand *mem) {
    return segment_prefix_len(mem) + address_size_prefix_len(mem);
}

static void emit_memory_operand_prefixes(struct Parser *p, struct MemoryOperand *mem,
                                         nint operand_size) {
    emit_segment_prefix(p, mem);
    emit_operand_size_prefix(p, operand_size);
    emit_address_size_prefix(p, mem);
}

static nint memory_operand_prefix_len(struct MemoryOperand *mem, nint operand_size) {
    return segment_prefix_len(mem) + operand_size_prefix_len(operand_size) +
           address_size_prefix_len(mem);
}

static void emit_operand_prefixes(struct Parser *p, struct Operand *op, nint size) {
    if(op->kind == OPERAND_MEM) emit_memory_operand_prefixes(p, &op->mem, size);
    else emit_operand_size_prefix(p, size);
}

static nint operand_prefix_len(struct Operand *op, nint size) {
    if(op->kind == OPERAND_MEM) return memory_operand_prefix_len(&op->mem, size);
    return operand_size_prefix_len(size);
}

static void emit_modrm_reg(struct Parser *p, nint reg, nint rm) {
    emit_byte(p, (unsigned char)(0xc0 | ((reg & 7) << 3) | (rm & 7)));
}

static nint memory32_mod(struct MemoryOperand *mem) {
    if(mem->direct) return 0;
    if(mem->base < 0) return 0;
    if(!mem->has_disp && mem->base != 5) return 0;
    if(!mem->has_disp && mem->base == 5) return 1;
    return fits_i8(mem->disp) ? 1 : 2;
}

static unint memory32_needs_sib(struct MemoryOperand *mem) {
    if(mem->direct) return 0;
    return mem->base < 0 || mem->base == 4 || mem->index >= 0;
}

static nint memory32_disp_len(struct MemoryOperand *mem, nint mod) {
    if(mem->direct) return 4;
    if(mem->base < 0) return 4;
    if(mod == 1) return 1;
    if(mod == 2) return 4;
    return 0;
}

static void emit_modrm_mem16(struct Parser *p, nint reg, struct MemoryOperand *mem) {
    nint mod;

    if(mem->direct) mod = 0;
    else if(mem->rm == 6 && !mem->has_disp) mod = 1;
    else if(!mem->has_disp) mod = 0;
    else mod = fits_i8(mem->disp) ? 1 : 2;

    emit_byte(p, (unsigned char)((mod << 6) | ((reg & 7) << 3) | (mem->rm & 7)));
    if(mem->direct) emit_word(p, mem->disp);
    else if(mod == 1) emit_byte(p, (unsigned char)(mem->disp & 0xff));
    else if(mod == 2) emit_word(p, mem->disp);
}

static void emit_modrm_mem32(struct Parser *p, nint reg, struct MemoryOperand *mem) {
    nint mod = memory32_mod(mem);
    unint needs_sib = memory32_needs_sib(mem);
    nint rm = mem->direct ? 5 : (needs_sib ? 4 : mem->base);

    emit_byte(p, (unsigned char)((mod << 6) | ((reg & 7) << 3) | (rm & 7)));

    if(needs_sib) {
        nint index = mem->index >= 0 ? mem->index : 4;
        nint base = mem->base >= 0 ? mem->base : 5;
        emit_byte(p, (unsigned char)(((mem->scale & 3) << 6) |
                                     ((index & 7) << 3) | (base & 7)));
    }

    if(mem->direct) emit_dword(p, mem->disp);
    else if(mem->base < 0) emit_dword(p, mem->has_disp ? mem->disp : 0);
    else if(mod == 1) emit_byte(p, (unsigned char)(mem->disp & 0xff));
    else if(mod == 2) emit_dword(p, mem->disp);
}

static void emit_modrm_mem(struct Parser *p, nint reg, struct MemoryOperand *mem) {
    if(mem->addr_size == 4) emit_modrm_mem32(p, reg, mem);
    else emit_modrm_mem16(p, reg, mem);
}

static unint is_al(struct Operand *op) {
    return op->kind == OPERAND_REG8 && op->reg == 0;
}

static unint is_ax(struct Operand *op) {
    return op->kind == OPERAND_REG16 && op->reg == 0;
}

static unint is_eax(struct Operand *op) {
    return op->kind == OPERAND_REG32 && op->reg == 0;
}

static unint is_dx(struct Operand *op) {
    return op->kind == OPERAND_REG16 && op->reg == 2;
}

static unint is_cl(struct Operand *op) {
    return op->kind == OPERAND_REG8 && op->reg == 1;
}

static unint memory_size_matches(struct Operand *op, nint size) {
    return op->kind == OPERAND_MEM && (op->size == 0 || op->size == size);
}

static nint encode_mov(struct Parser *p, struct AstInstruction *inst,
                       struct Operand *dst, struct Operand *src) {
    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_IMM) {
        if(!fits_byte_immediate(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 8 bits");
            return INSTRUCTION_FAILED;
        }
        emit_byte(p, (unsigned char)(0xb0 + dst->reg));
        emit_byte(p, (unsigned char)src->imm);
        return 2;
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_IMM) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_byte(p, (unsigned char)(0xb8 + dst->reg));
        emit_word(p, src->imm);
        return 3;
    }

    if(dst->kind == OPERAND_REG32 && src->kind == OPERAND_IMM) {
        if(!fits_u32(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 32 bits");
            return INSTRUCTION_FAILED;
        }
        emit_operand_size_prefix(p, 4);
        emit_byte(p, (unsigned char)(0xb8 + dst->reg));
        emit_dword(p, src->imm);
        return operand_size_prefix_len(4) + 5;
    }

    if(is_al(dst) && src->kind == OPERAND_MEM && src->mem.direct) {
        if(!memory_size_matches(src, 1)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid mov operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &src->mem);
        emit_byte(p, 0xa0);
        emit_word(p, src->mem.disp);
        return memory_prefix_len(&src->mem) + 3;
    }
    if(is_ax(dst) && src->kind == OPERAND_MEM && src->mem.direct) {
        if(!memory_size_matches(src, 2)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid mov operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &src->mem);
        emit_byte(p, 0xa1);
        emit_word(p, src->mem.disp);
        return memory_prefix_len(&src->mem) + 3;
    }
    if(is_eax(dst) && src->kind == OPERAND_MEM && src->mem.direct) {
        if(!memory_size_matches(src, 4)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid mov operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &src->mem, 4);
        emit_byte(p, 0xa1);
        emit_word(p, src->mem.disp);
        return memory_operand_prefix_len(&src->mem, 4) + 3;
    }
    if(dst->kind == OPERAND_MEM && dst->mem.direct && is_al(src)) {
        if(!memory_size_matches(dst, 1)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid mov operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        emit_byte(p, 0xa2);
        emit_word(p, dst->mem.disp);
        return memory_prefix_len(&dst->mem) + 3;
    }
    if(dst->kind == OPERAND_MEM && dst->mem.direct && is_ax(src)) {
        if(!memory_size_matches(dst, 2)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid mov operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        emit_byte(p, 0xa3);
        emit_word(p, dst->mem.disp);
        return memory_prefix_len(&dst->mem) + 3;
    }
    if(dst->kind == OPERAND_MEM && dst->mem.direct && is_eax(src)) {
        if(!memory_size_matches(dst, 4)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid mov operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &dst->mem, 4);
        emit_byte(p, 0xa3);
        emit_word(p, dst->mem.disp);
        return memory_operand_prefix_len(&dst->mem, 4) + 3;
    }

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_REG8) {
        emit_byte(p, 0x88);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }
    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_REG16) {
        emit_byte(p, 0x89);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }
    if(dst->kind == OPERAND_REG32 && src->kind == OPERAND_REG32) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x89);
        emit_modrm_reg(p, src->reg, dst->reg);
        return operand_size_prefix_len(4) + 2;
    }

    if(dst->kind == OPERAND_REG32 && src->kind == OPERAND_CR) {
        emit_byte(p, 0x0f);
        emit_byte(p, 0x20);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 3;
    }
    if(dst->kind == OPERAND_CR && src->kind == OPERAND_REG32) {
        emit_byte(p, 0x0f);
        emit_byte(p, 0x22);
        emit_modrm_reg(p, dst->reg, src->reg);
        return 3;
    }
    if(dst->kind == OPERAND_REG32 && src->kind == OPERAND_DR) {
        emit_byte(p, 0x0f);
        emit_byte(p, 0x21);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 3;
    }
    if(dst->kind == OPERAND_DR && src->kind == OPERAND_REG32) {
        emit_byte(p, 0x0f);
        emit_byte(p, 0x23);
        emit_modrm_reg(p, dst->reg, src->reg);
        return 3;
    }
    if(dst->kind == OPERAND_REG32 && src->kind == OPERAND_TR) {
        emit_byte(p, 0x0f);
        emit_byte(p, 0x24);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 3;
    }
    if(dst->kind == OPERAND_TR && src->kind == OPERAND_REG32) {
        emit_byte(p, 0x0f);
        emit_byte(p, 0x26);
        emit_modrm_reg(p, dst->reg, src->reg);
        return 3;
    }

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_MEM) {
        if(!memory_size_matches(src, 1)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid mov operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &src->mem);
        emit_byte(p, 0x8a);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }
    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_MEM) {
        if(!memory_size_matches(src, 2)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid mov operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &src->mem);
        emit_byte(p, 0x8b);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }
    if(dst->kind == OPERAND_REG32 && src->kind == OPERAND_MEM) {
        if(!memory_size_matches(src, 4)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid mov operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &src->mem, 4);
        emit_byte(p, 0x8b);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_operand_prefix_len(&src->mem, 4) + 2 +
               memory_tail_len(&src->mem);
    }
    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG8) {
        if(!memory_size_matches(dst, 1)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid mov operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        emit_byte(p, 0x88);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }
    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG16) {
        if(!memory_size_matches(dst, 2)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid mov operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        emit_byte(p, 0x89);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }
    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG32) {
        if(!memory_size_matches(dst, 4)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid mov operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &dst->mem, 4);
        emit_byte(p, 0x89);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return memory_operand_prefix_len(&dst->mem, 4) + 2 +
               memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_SEG) {
        emit_byte(p, 0x8c);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }
    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_SEG) {
        if(!memory_size_matches(dst, 2)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid mov operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        emit_byte(p, 0x8c);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }
    if(dst->kind == OPERAND_SEG && src->kind == OPERAND_REG16) {
        emit_byte(p, 0x8e);
        emit_modrm_reg(p, dst->reg, src->reg);
        return 2;
    }
    if(dst->kind == OPERAND_SEG && src->kind == OPERAND_MEM) {
        if(!memory_size_matches(src, 2)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid mov operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &src->mem);
        emit_byte(p, 0x8e);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_IMM) {
        if(dst->size == 1) {
            if(!fits_byte_immediate(src->imm)) {
                _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                                  "immediate does not fit 8 bits");
                return INSTRUCTION_FAILED;
            }
            emit_memory_prefixes(p, &dst->mem);
            emit_byte(p, 0xc6);
            emit_modrm_mem(p, 0, &dst->mem);
            emit_byte(p, (unsigned char)src->imm);
            return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 1;
        }
        if(dst->size == 2) {
            if(!fits_u16(src->imm)) {
                _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                                  "immediate does not fit 16 bits");
                return INSTRUCTION_FAILED;
            }
            emit_memory_prefixes(p, &dst->mem);
            emit_byte(p, 0xc7);
            emit_modrm_mem(p, 0, &dst->mem);
            emit_word(p, src->imm);
            return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 2;
        }
        if(dst->size == 4) {
            if(!fits_u32(src->imm)) {
                _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                                  "immediate does not fit 32 bits");
                return INSTRUCTION_FAILED;
            }
            emit_memory_operand_prefixes(p, &dst->mem, 4);
            emit_byte(p, 0xc7);
            emit_modrm_mem(p, 0, &dst->mem);
            emit_dword(p, src->imm);
            return memory_operand_prefix_len(&dst->mem, 4) + 2 +
                   memory_tail_len(&dst->mem) + 4;
        }
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "ambiguous memory immediate size");
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported mov form");
    return INSTRUCTION_FAILED;
}

static nint encode_lea(struct Parser *p, struct AstInstruction *inst,
                       struct Operand *dst, struct Operand *src) {
    struct Operand direct_src;

    if(src->kind == OPERAND_IMM) {
        direct_src = *src;
        set_direct_memory(&direct_src, src->imm, -1);
        src = &direct_src;
    }

    if((dst->kind != OPERAND_REG16 && dst->kind != OPERAND_REG32) ||
       src->kind != OPERAND_MEM) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported lea form");
        return INSTRUCTION_FAILED;
    }

    emit_memory_operand_prefixes(p, &src->mem, dst->size);
    emit_byte(p, 0x8d);
    emit_modrm_mem(p, dst->reg, &src->mem);
    return memory_operand_prefix_len(&src->mem, dst->size) + 2 +
           memory_tail_len(&src->mem);
}

static nint encode_far_load(struct Parser *p, struct AstInstruction *inst,
                            struct Operand *dst, struct Operand *src,
                            unsigned char opcode) {
    if((dst->kind != OPERAND_REG16 && dst->kind != OPERAND_REG32) ||
       src->kind != OPERAND_MEM) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported far load form");
        return INSTRUCTION_FAILED;
    }

    if(src->size != 0) {
        _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                          "invalid far load operand size");
        return INSTRUCTION_FAILED;
    }

    if(dst->kind == OPERAND_REG32) {
        emit_memory_operand_prefixes(p, &src->mem, 4);
    } else {
        emit_memory_prefixes(p, &src->mem);
    }
    emit_byte(p, opcode);
    emit_modrm_mem(p, dst->reg, &src->mem);
    if(dst->kind == OPERAND_REG32) {
        return memory_operand_prefix_len(&src->mem, 4) + 2 +
               memory_tail_len(&src->mem);
    }
    return memory_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
}

static nint encode_far_load_0f(struct Parser *p, struct AstInstruction *inst,
                               struct Operand *dst, struct Operand *src,
                               unsigned char opcode) {
    if((dst->kind != OPERAND_REG16 && dst->kind != OPERAND_REG32) ||
       src->kind != OPERAND_MEM) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported far load form");
        return INSTRUCTION_FAILED;
    }

    if(src->size != 0) {
        _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                          "invalid far load operand size");
        return INSTRUCTION_FAILED;
    }

    if(dst->kind == OPERAND_REG32) {
        emit_memory_operand_prefixes(p, &src->mem, 4);
    } else {
        emit_memory_prefixes(p, &src->mem);
    }
    emit_byte(p, 0x0f);
    emit_byte(p, opcode);
    emit_modrm_mem(p, dst->reg, &src->mem);
    if(dst->kind == OPERAND_REG32) {
        return memory_operand_prefix_len(&src->mem, 4) + 3 +
               memory_tail_len(&src->mem);
    }
    return memory_prefix_len(&src->mem) + 3 + memory_tail_len(&src->mem);
}

static unint operand_is_mem16(struct Operand *op);
static unint operand_is_mem32(struct Operand *op);
static void force_direct_memory_a32(struct Operand *op);
static void emit_forced_prefix_pair(struct Parser *p, unint operand_prefix,
                                    unint address_prefix);
static nint forced_prefix_pair_len(unint operand_prefix,
                                   unint address_prefix);
static void emit_forced_o32_memory_prefixes(struct Parser *p,
                                            struct MemoryOperand *mem);
static nint forced_o32_memory_prefix_len(struct MemoryOperand *mem);
static unint token_is(struct token *tok, int32_t *name);

static nint encode_lea_prefixed_operands(struct Parser *p,
                                         struct AstInstruction *inst,
                                         struct Operand *dst,
                                         struct Operand *src,
                                         unint force_o32,
                                         unint force_a16,
                                         unint force_a32,
                                         unint force_o16) {
    struct Operand direct_src;
    nint op_size;

    if(src->kind == OPERAND_IMM) {
        direct_src = *src;
        set_direct_memory(&direct_src, src->imm, -1);
        src = &direct_src;
    }

    if((dst->kind != OPERAND_REG16 && dst->kind != OPERAND_REG32) ||
       src->kind != OPERAND_MEM) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported lea form");
        return INSTRUCTION_FAILED;
    }

    if(force_a32) force_direct_memory_a32(src);
    if(force_a16 && operand_is_mem32(src)) {
        _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                          "invalid a16 address form");
        return INSTRUCTION_FAILED;
    }
    if(force_a32 && operand_is_mem16(src)) {
        _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                          "invalid a32 address form");
        return INSTRUCTION_FAILED;
    }
    if(!force_a32 && operand_is_mem32(src) && src->mem.direct) {
        src->mem.addr_size = 2;
    }

    op_size = force_o32 ? 4 : (force_o16 ? 2 : dst->size);
    emit_memory_operand_prefixes(p, &src->mem, op_size);
    emit_byte(p, 0x8d);
    emit_modrm_mem(p, dst->reg, &src->mem);
    return memory_operand_prefix_len(&src->mem, op_size) + 2 +
           memory_tail_len(&src->mem);
}

static nint encode_lea_prefixed(struct Parser *p, struct AstInstruction *inner,
                                unint force_o32, unint force_a16,
                                unint force_a32, unint force_o16) {
    struct Operand dst;
    struct Operand src;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }
    error_token = NULL;
    status = parse_operand(p, inner->args_tail, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    return encode_lea_prefixed_operands(p, inner, &dst, &src, force_o32,
                                        force_a16, force_a32, force_o16);
}

static nint encode_far_load_prefixed_operands(struct Parser *p,
                                              struct AstInstruction *inst,
                                              struct Operand *dst,
                                              struct Operand *src,
                                              unsigned char opcode,
                                              unint force_o32,
                                              unint force_a16,
                                              unint force_a32,
                                              unint force_o16) {
    nint op_size;

    if((dst->kind != OPERAND_REG16 && dst->kind != OPERAND_REG32) ||
       src->kind != OPERAND_MEM) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386,
                          "unsupported far load form");
        return INSTRUCTION_FAILED;
    }

    if(src->size != 0) {
        _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                          "invalid far load operand size");
        return INSTRUCTION_FAILED;
    }

    if(force_a32) force_direct_memory_a32(src);
    if(force_a16 && operand_is_mem32(src)) {
        _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                          "invalid a16 address form");
        return INSTRUCTION_FAILED;
    }
    if(force_a32 && operand_is_mem16(src)) {
        _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                          "invalid a32 address form");
        return INSTRUCTION_FAILED;
    }
    if(!force_a32 && operand_is_mem32(src) && src->mem.direct) {
        src->mem.addr_size = 2;
    }

    op_size = force_o32 ? 4 : (force_o16 ? 2 : dst->size);
    emit_memory_operand_prefixes(p, &src->mem, op_size);
    emit_byte(p, opcode);
    emit_modrm_mem(p, dst->reg, &src->mem);
    return memory_operand_prefix_len(&src->mem, op_size) + 2 +
           memory_tail_len(&src->mem);
}

static nint encode_far_load_prefixed(struct Parser *p,
                                     struct AstInstruction *inner,
                                     unsigned char opcode,
                                     unint force_o32,
                                     unint force_a16,
                                     unint force_a32,
                                     unint force_o16) {
    struct Operand dst;
    struct Operand src;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }
    error_token = NULL;
    status = parse_operand(p, inner->args_tail, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    return encode_far_load_prefixed_operands(p, inner, &dst, &src, opcode,
                                             force_o32, force_a16, force_a32,
                                             force_o16);
}

static nint encode_far_load_0f_prefixed_operands(struct Parser *p,
                                                 struct AstInstruction *inst,
                                                 struct Operand *dst,
                                                 struct Operand *src,
                                                 unsigned char opcode,
                                                 unint force_o32,
                                                 unint force_a16,
                                                 unint force_a32,
                                                 unint force_o16) {
    nint op_size;

    if((dst->kind != OPERAND_REG16 && dst->kind != OPERAND_REG32) ||
       src->kind != OPERAND_MEM) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386,
                          "unsupported far load form");
        return INSTRUCTION_FAILED;
    }

    if(src->size != 0) {
        _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                          "invalid far load operand size");
        return INSTRUCTION_FAILED;
    }

    if(force_a32) force_direct_memory_a32(src);
    if(force_a16 && operand_is_mem32(src)) {
        _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                          "invalid a16 address form");
        return INSTRUCTION_FAILED;
    }
    if(force_a32 && operand_is_mem16(src)) {
        _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                          "invalid a32 address form");
        return INSTRUCTION_FAILED;
    }
    if(!force_a32 && operand_is_mem32(src) && src->mem.direct) {
        src->mem.addr_size = 2;
    }

    op_size = force_o32 ? 4 : (force_o16 ? 2 : dst->size);
    emit_memory_operand_prefixes(p, &src->mem, op_size);
    emit_byte(p, 0x0f);
    emit_byte(p, opcode);
    emit_modrm_mem(p, dst->reg, &src->mem);
    return memory_operand_prefix_len(&src->mem, op_size) + 3 +
           memory_tail_len(&src->mem);
}

static nint encode_far_load_0f_prefixed(struct Parser *p,
                                        struct AstInstruction *inner,
                                        unsigned char opcode,
                                        unint force_o32,
                                        unint force_a16,
                                        unint force_a32,
                                        unint force_o16) {
    struct Operand dst;
    struct Operand src;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }
    error_token = NULL;
    status = parse_operand(p, inner->args_tail, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    return encode_far_load_0f_prefixed_operands(p, inner, &dst, &src, opcode,
                                                force_o32, force_a16,
                                                force_a32, force_o16);
}

static nint encode_mov_extend(struct Parser *p, struct AstInstruction *inst,
                              struct Operand *dst, struct Operand *src,
                              unsigned char byte_opcode,
                              unsigned char word_opcode,
                              const char *name) {
    unsigned char opcode;

    if(dst->kind != OPERAND_REG16 && dst->kind != OPERAND_REG32) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "invalid extend destination");
        return INSTRUCTION_FAILED;
    }

    if(src->kind == OPERAND_REG8) {
        opcode = byte_opcode;
    } else if(src->kind == OPERAND_REG16 && dst->kind == OPERAND_REG32) {
        opcode = word_opcode;
    } else if(src->kind == OPERAND_MEM) {
        if(dst->kind == OPERAND_REG16) {
            if(src->size != 0 && src->size != 1) {
                _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                                  "invalid extend source size");
                return INSTRUCTION_FAILED;
            }
            opcode = byte_opcode;
        } else {
            if(src->size == 0) {
                _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                                  "ambiguous extend source size");
                return INSTRUCTION_FAILED;
            }
            if(src->size == 1) opcode = byte_opcode;
            else if(src->size == 2) opcode = word_opcode;
            else {
                _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                                  "invalid extend source size");
                return INSTRUCTION_FAILED;
            }
        }
    } else {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, name);
        return INSTRUCTION_FAILED;
    }

    if(src->kind == OPERAND_MEM) {
        emit_memory_operand_prefixes(p, &src->mem, dst->size);
        emit_byte(p, 0x0f);
        emit_byte(p, opcode);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_operand_prefix_len(&src->mem, dst->size) + 3 +
               memory_tail_len(&src->mem);
    }

    emit_operand_size_prefix(p, dst->size);
    emit_byte(p, 0x0f);
    emit_byte(p, opcode);
    emit_modrm_reg(p, dst->reg, src->reg);
    return operand_size_prefix_len(dst->size) + 3;
}

static nint encode_bit_scan(struct Parser *p, struct AstInstruction *inst,
                            struct Operand *dst, struct Operand *src,
                            unsigned char opcode, const char *name) {
    if(dst->kind != OPERAND_REG16 && dst->kind != OPERAND_REG32) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "invalid bit scan destination");
        return INSTRUCTION_FAILED;
    }

    if(src->kind == OPERAND_REG16 || src->kind == OPERAND_REG32) {
        if(src->size != dst->size) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid bit scan source size");
            return INSTRUCTION_FAILED;
        }
        emit_operand_size_prefix(p, dst->size);
        emit_byte(p, 0x0f);
        emit_byte(p, opcode);
        emit_modrm_reg(p, dst->reg, src->reg);
        return operand_size_prefix_len(dst->size) + 3;
    }

    if(src->kind == OPERAND_MEM) {
        if(src->size != 0 && src->size != dst->size) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid bit scan source size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &src->mem, dst->size);
        emit_byte(p, 0x0f);
        emit_byte(p, opcode);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_operand_prefix_len(&src->mem, dst->size) + 3 +
               memory_tail_len(&src->mem);
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, name);
    return INSTRUCTION_FAILED;
}

static nint encode_lar_operands(struct Parser *p, struct AstInstruction *inst,
                                struct Operand *dst, struct Operand *src,
                                unint force_o32, unint force_a16,
                                unint force_a32, unint force_o16) {
    nint op_size;

    if(dst->kind != OPERAND_REG16 && dst->kind != OPERAND_REG32) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "invalid lar destination");
        return INSTRUCTION_FAILED;
    }

    op_size = force_o32 ? 4 : (force_o16 ? 2 : dst->size);

    if(src->kind == OPERAND_REG16 || src->kind == OPERAND_REG32) {
        emit_forced_prefix_pair(p, op_size == 4, force_a32);
        emit_byte(p, 0x0f);
        emit_byte(p, 0x02);
        emit_modrm_reg(p, dst->reg, src->reg);
        return forced_prefix_pair_len(op_size == 4, force_a32) + 3;
    }

    if(src->kind == OPERAND_MEM) {
        if(src->size != 0 && src->size != 2) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid lar source size");
            return INSTRUCTION_FAILED;
        }

        if(force_a32) force_direct_memory_a32(src);
        if(force_a16 && operand_is_mem32(src)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid a16 address form");
            return INSTRUCTION_FAILED;
        }
        if(force_a32 && operand_is_mem16(src)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid a32 address form");
            return INSTRUCTION_FAILED;
        }
        if(!force_a32 && operand_is_mem32(src) && src->mem.direct) {
            src->mem.addr_size = 2;
        }

        emit_memory_operand_prefixes(p, &src->mem, op_size);
        emit_byte(p, 0x0f);
        emit_byte(p, 0x02);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_operand_prefix_len(&src->mem, op_size) + 3 +
               memory_tail_len(&src->mem);
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported lar form");
    return INSTRUCTION_FAILED;
}

static nint encode_lsl_operands(struct Parser *p, struct AstInstruction *inst,
                                struct Operand *dst, struct Operand *src,
                                unint force_o32, unint force_a16,
                                unint force_a32, unint force_o16) {
    nint op_size;

    if(dst->kind != OPERAND_REG16 && dst->kind != OPERAND_REG32) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "invalid lsl destination");
        return INSTRUCTION_FAILED;
    }

    op_size = force_o32 ? 4 : (force_o16 ? 2 : dst->size);

    if(src->kind == OPERAND_REG16 || src->kind == OPERAND_REG32) {
        emit_forced_prefix_pair(p, op_size == 4, force_a32);
        emit_byte(p, 0x0f);
        emit_byte(p, 0x03);
        emit_modrm_reg(p, dst->reg, src->reg);
        return forced_prefix_pair_len(op_size == 4, force_a32) + 3;
    }

    if(src->kind == OPERAND_MEM) {
        if(src->size != 0 && src->size != 2) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid lsl source size");
            return INSTRUCTION_FAILED;
        }

        if(force_a32) force_direct_memory_a32(src);
        if(force_a16 && operand_is_mem32(src)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid a16 address form");
            return INSTRUCTION_FAILED;
        }
        if(force_a32 && operand_is_mem16(src)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid a32 address form");
            return INSTRUCTION_FAILED;
        }
        if(!force_a32 && operand_is_mem32(src) && src->mem.direct) {
            src->mem.addr_size = 2;
        }

        emit_memory_operand_prefixes(p, &src->mem, op_size);
        emit_byte(p, 0x0f);
        emit_byte(p, 0x03);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_operand_prefix_len(&src->mem, op_size) + 3 +
               memory_tail_len(&src->mem);
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported lsl form");
    return INSTRUCTION_FAILED;
}

static void emit_system_memory_prefixes(struct Parser *p,
                                        struct MemoryOperand *mem,
                                        unint force_o32) {
    emit_segment_prefix(p, mem);
    if(force_o32) emit_operand_size_prefix(p, 4);
    emit_address_size_prefix(p, mem);
}

static nint system_memory_prefix_len(struct MemoryOperand *mem,
                                     unint force_o32) {
    return segment_prefix_len(mem) + (force_o32 ? 1 : 0) +
           address_size_prefix_len(mem);
}

static nint encode_descriptor_table(struct Parser *p,
                                    struct AstInstruction *inst,
                                    struct Operand *src,
                                    nint group,
                                    const char *name,
                                    unint force_o32,
                                    unint force_a16,
                                    unint force_a32) {
    if(src->kind != OPERAND_MEM) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "invalid %s operand", name);
        return INSTRUCTION_FAILED;
    }

    if(src->size != 0) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "invalid %s operand size", name);
        return INSTRUCTION_FAILED;
    }

    if(force_a32) force_direct_memory_a32(src);
    if(force_a16 && operand_is_mem32(src)) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "invalid a16 address form");
        return INSTRUCTION_FAILED;
    }
    if(force_a32 && operand_is_mem16(src)) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "invalid a32 address form");
        return INSTRUCTION_FAILED;
    }
    if(!force_a32 && operand_is_mem32(src) && src->mem.direct) {
        src->mem.addr_size = 2;
    }

    emit_system_memory_prefixes(p, &src->mem, force_o32);
    emit_byte(p, 0x0f);
    emit_byte(p, 0x01);
    emit_modrm_mem(p, group, &src->mem);
    return system_memory_prefix_len(&src->mem, force_o32) + 3 +
           memory_tail_len(&src->mem);
}

static nint encode_system_rm16_load(struct Parser *p,
                                    struct AstInstruction *inst,
                                    struct Operand *src,
                                    nint group,
                                    const char *name,
                                    unint force_o32,
                                    unint force_a16,
                                    unint force_a32) {
    if(src->kind == OPERAND_REG16) {
        emit_forced_prefix_pair(p, force_o32, force_a32);
        emit_byte(p, 0x0f);
        emit_byte(p, group == 6 ? 0x01 : 0x00);
        emit_modrm_reg(p, group, src->reg);
        return forced_prefix_pair_len(force_o32, force_a32) + 3;
    }

    if(src->kind == OPERAND_MEM) {
        if(src->size != 0 && src->size != 2) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid %s operand size", name);
            return INSTRUCTION_FAILED;
        }

        if(force_a32) force_direct_memory_a32(src);
        if(force_a16 && operand_is_mem32(src)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid a16 address form");
            return INSTRUCTION_FAILED;
        }
        if(force_a32 && operand_is_mem16(src)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid a32 address form");
            return INSTRUCTION_FAILED;
        }
        if(!force_a32 && operand_is_mem32(src) && src->mem.direct) {
            src->mem.addr_size = 2;
        }

        emit_memory_operand_prefixes(p, &src->mem, force_o32 ? 4 : 2);
        emit_byte(p, 0x0f);
        emit_byte(p, group == 6 ? 0x01 : 0x00);
        emit_modrm_mem(p, group, &src->mem);
        return memory_operand_prefix_len(&src->mem, force_o32 ? 4 : 2) + 3 +
               memory_tail_len(&src->mem);
    }

    _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                      "invalid %s operand", name);
    return INSTRUCTION_FAILED;
}

static nint encode_system_rm16_store(struct Parser *p,
                                     struct AstInstruction *inst,
                                     struct Operand *dst,
                                     nint group,
                                     unsigned char opcode,
                                     const char *name,
                                     unint force_o32,
                                     unint force_a16,
                                     unint force_a32,
                                     unint force_o16) {
    if(dst->kind == OPERAND_REG16 || dst->kind == OPERAND_REG32) {
        unint op_prefix = force_o32 || (dst->kind == OPERAND_REG32 && !force_o16);
        emit_forced_prefix_pair(p, op_prefix, force_a32);
        emit_byte(p, 0x0f);
        emit_byte(p, opcode);
        emit_modrm_reg(p, group, dst->reg);
        return forced_prefix_pair_len(op_prefix, force_a32) + 3;
    }

    if(dst->kind == OPERAND_MEM) {
        if(dst->size != 0 && dst->size != 2) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid %s operand size", name);
            return INSTRUCTION_FAILED;
        }

        if(force_a32) force_direct_memory_a32(dst);
        if(force_a16 && operand_is_mem32(dst)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid a16 address form");
            return INSTRUCTION_FAILED;
        }
        if(force_a32 && operand_is_mem16(dst)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid a32 address form");
            return INSTRUCTION_FAILED;
        }
        if(!force_a32 && operand_is_mem32(dst) && dst->mem.direct) {
            dst->mem.addr_size = 2;
        }

        emit_system_memory_prefixes(p, &dst->mem, force_o32);
        emit_byte(p, 0x0f);
        emit_byte(p, opcode);
        emit_modrm_mem(p, group, &dst->mem);
        return system_memory_prefix_len(&dst->mem, force_o32) + 3 +
               memory_tail_len(&dst->mem);
    }

    _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                      "invalid %s operand", name);
    return INSTRUCTION_FAILED;
}

static nint encode_system_verify_selector(struct Parser *p,
                                          struct AstInstruction *inst,
                                          struct Operand *src,
                                          nint group,
                                          const char *name,
                                          unint force_o32,
                                          unint force_a16,
                                          unint force_a32) {
    if(src->kind == OPERAND_REG16) {
        emit_forced_prefix_pair(p, force_o32, force_a32);
        emit_byte(p, 0x0f);
        emit_byte(p, 0x00);
        emit_modrm_reg(p, group, src->reg);
        return forced_prefix_pair_len(force_o32, force_a32) + 3;
    }

    if(src->kind == OPERAND_MEM) {
        if(src->size == 1 || src->size == 4) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid %s operand size", name);
            return INSTRUCTION_FAILED;
        }

        if(force_a32) force_direct_memory_a32(src);
        if(force_a16 && operand_is_mem32(src)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid a16 address form");
            return INSTRUCTION_FAILED;
        }
        if(force_a32 && operand_is_mem16(src)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid a32 address form");
            return INSTRUCTION_FAILED;
        }
        if(!force_a32 && operand_is_mem32(src) && src->mem.direct) {
            src->mem.addr_size = 2;
        }

        emit_system_memory_prefixes(p, &src->mem, force_o32);
        emit_byte(p, 0x0f);
        emit_byte(p, 0x00);
        emit_modrm_mem(p, group, &src->mem);
        return system_memory_prefix_len(&src->mem, force_o32) + 3 +
               memory_tail_len(&src->mem);
    }

    _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                      "invalid %s operand", name);
    return INSTRUCTION_FAILED;
}

static nint encode_system_one_operand_prefixed(struct Parser *p,
                                               struct AstInstruction *inner,
                                               unint force_o32,
                                               unint force_a16,
                                               unint force_a32,
                                               unint force_o16) {
    struct Operand src;
    struct token *error_token = NULL;
    nint status;

    if(!token_is(inner->name, LGDT_INSTRUCTION) &&
       !token_is(inner->name, LIDT_INSTRUCTION) &&
       !token_is(inner->name, SGDT_INSTRUCTION) &&
       !token_is(inner->name, SIDT_INSTRUCTION) &&
       !token_is(inner->name, LLDT_INSTRUCTION) &&
       !token_is(inner->name, SLDT_INSTRUCTION) &&
       !token_is(inner->name, LTR_INSTRUCTION) &&
       !token_is(inner->name, STR_INSTRUCTION) &&
       !token_is(inner->name, LMSW_INSTRUCTION) &&
       !token_is(inner->name, SMSW_INSTRUCTION) &&
       !token_is(inner->name, VERR_INSTRUCTION) &&
       !token_is(inner->name, VERW_INSTRUCTION)) {
        return 0;
    }

    if(inner->arg_count != 1) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }

    if(token_is(inner->name, LGDT_INSTRUCTION)) {
        return encode_descriptor_table(p, inner, &src, 2, "lgdt", force_o32,
                                       force_a16, force_a32);
    }
    if(token_is(inner->name, LIDT_INSTRUCTION)) {
        return encode_descriptor_table(p, inner, &src, 3, "lidt", force_o32,
                                       force_a16, force_a32);
    }
    if(token_is(inner->name, SGDT_INSTRUCTION)) {
        return encode_descriptor_table(p, inner, &src, 0, "sgdt", force_o32,
                                       force_a16, force_a32);
    }
    if(token_is(inner->name, SIDT_INSTRUCTION)) {
        return encode_descriptor_table(p, inner, &src, 1, "sidt", force_o32,
                                       force_a16, force_a32);
    }
    if(token_is(inner->name, LLDT_INSTRUCTION)) {
        return encode_system_rm16_load(p, inner, &src, 2, "lldt", force_o32,
                                       force_a16, force_a32);
    }
    if(token_is(inner->name, SLDT_INSTRUCTION)) {
        return encode_system_rm16_store(p, inner, &src, 0, 0x00, "sldt",
                                        force_o32, force_a16, force_a32,
                                        force_o16);
    }
    if(token_is(inner->name, LTR_INSTRUCTION)) {
        return encode_system_rm16_load(p, inner, &src, 3, "ltr", force_o32,
                                       force_a16, force_a32);
    }
    if(token_is(inner->name, STR_INSTRUCTION)) {
        return encode_system_rm16_store(p, inner, &src, 1, 0x00, "str",
                                        force_o32, force_a16, force_a32,
                                        force_o16);
    }
    if(token_is(inner->name, LMSW_INSTRUCTION)) {
        return encode_system_rm16_load(p, inner, &src, 6, "lmsw", force_o32,
                                       force_a16, force_a32);
    }
    if(token_is(inner->name, SMSW_INSTRUCTION)) {
        return encode_system_rm16_store(p, inner, &src, 4, 0x01, "smsw",
                                        force_o32, force_a16, force_a32,
                                        force_o16);
    }
    if(token_is(inner->name, VERR_INSTRUCTION)) {
        return encode_system_verify_selector(p, inner, &src, 4, "verr",
                                             force_o32, force_a16, force_a32);
    }
    if(token_is(inner->name, VERW_INSTRUCTION)) {
        return encode_system_verify_selector(p, inner, &src, 5, "verw",
                                             force_o32, force_a16, force_a32);
    }

    return 0;
}

static nint encode_bound(struct Parser *p, struct AstInstruction *inst,
                         struct Operand *dst, struct Operand *src) {
    if((dst->kind != OPERAND_REG16 && dst->kind != OPERAND_REG32) ||
       src->kind != OPERAND_MEM) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported bound form");
        return INSTRUCTION_FAILED;
    }

    if(src->size != 0) {
        _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                          "invalid bound source size");
        return INSTRUCTION_FAILED;
    }

    emit_memory_operand_prefixes(p, &src->mem, dst->size);
    emit_byte(p, 0x62);
    emit_modrm_mem(p, dst->reg, &src->mem);
    return memory_operand_prefix_len(&src->mem, dst->size) + 2 +
           memory_tail_len(&src->mem);
}

#define JMP_CACHE_MAX 512

static nint jmp_cache_addr[JMP_CACHE_MAX];
static nint jmp_cache_size[JMP_CACHE_MAX];
static unint jmp_cache_used[JMP_CACHE_MAX];

static void remember_jmp_size(nint addr, nint size) {
    for(unint i = 0; i < JMP_CACHE_MAX; i++) {
        if(jmp_cache_used[i] && jmp_cache_addr[i] == addr) {
            jmp_cache_size[i] = size;
            return;
        }
    }
    for(unint i = 0; i < JMP_CACHE_MAX; i++) {
        if(!jmp_cache_used[i]) {
            jmp_cache_used[i] = 1;
            jmp_cache_addr[i] = addr;
            jmp_cache_size[i] = size;
            return;
        }
    }
}

static nint remembered_jmp_size(nint addr) {
    for(unint i = 0; i < JMP_CACHE_MAX; i++) {
        if(jmp_cache_used[i] && jmp_cache_addr[i] == addr) return jmp_cache_size[i];
    }
    return 0;
}

static unint token_is(struct token *tok, int32_t *name) {
    return tok && tok->type == NAME &&
           compare_identifiers_cp_array(tok, name) == SUCCESS;
}

static nint read_jump_scalar(struct Parser *p, struct TokenStream *tks,
                             nint *value, unint *unresolved,
                             unint *is_variable,
                             struct token **error_token) {
    struct token *tok = tks_peek(tks);
    unint status;
    tks_reset_peek(tks);
    if(!tok) return 0;

    if(tok->type == NAME) {
        status = parse_potential_variable(p, tks, value, 1);
        if(status == VP_SUCCESS) {
            record_error_token(error_token, tok);
            *is_variable = 1;
            return 1;
        }
        if(status == VP_UNRESOLVED_LABEL) {
            record_error_token(error_token, tok);
            *value = 0;
            *unresolved = 1;
            *is_variable = 1;
            if(p->last_pass) return fail_unresolved_label(p, tok);
            return 1;
        }
        if(status == VP_FAIL) return INSTRUCTION_FAILED;
        return 0;
    }

    if(tok->type == NUMBER || tok->type == MINUS) {
        if(!read_integer(p, tks, value, error_token)) return 0;
        *is_variable = 0;
        return 1;
    }

    return 0;
}

static nint emit_jmp_relative(struct Parser *p, nint target, nint size,
                              struct token *tok) {
    nint disp = target - (p->addr + size);
    if(size == 2) {
        if(!fits_i8(disp)) {
            _error_from_token(p, tok, ERROR_TYPE_OVERFLOW,
                              "short jump target out of range");
            return INSTRUCTION_FAILED;
        }
        emit_byte(p, 0xeb);
        emit_byte(p, (unsigned char)(disp & 0xff));
        return 2;
    }
    if(!fits_i16(disp)) {
        _error_from_token(p, tok, ERROR_TYPE_OVERFLOW,
                          "near jump target out of range");
        return INSTRUCTION_FAILED;
    }
    emit_byte(p, 0xe9);
    emit_word(p, disp);
    return 3;
}

static nint parse_jump_memory_from(struct Parser *p, struct token *start,
                                   struct token *end, struct Operand *mem,
                                   struct token **error_token) {
    struct InstructionArg arg;
    nint status;

    arg._s = start;
    arg._e = end;
    arg.next = NULL;
    status = parse_memory(p, &arg, mem, error_token);
    if(status <= 0) return status;
    if(mem->size != 0 && mem->size != 2 && mem->size != 4) {
        _error_from_token(p, start, ERROR_TYPE_I386,
                          "invalid jump pointer size");
        return INSTRUCTION_FAILED;
    }
    return 1;
}

static nint encode_jmp_ptr_mem(struct Parser *p, struct token *start,
                               struct token *end, nint far_ptr) {
    struct Operand mem;
    struct token *error_token = NULL;
    nint status = parse_jump_memory_from(p, start, end, &mem, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token ? error_token : start, ERROR_TYPE_I386,
                          "invalid jump pointer");
        return INSTRUCTION_FAILED;
    }

    emit_memory_operand_prefixes(p, &mem.mem, mem.size == 4 ? 4 : 2);
    emit_byte(p, 0xff);
    emit_modrm_mem(p, far_ptr ? 5 : 4, &mem.mem);
    return memory_operand_prefix_len(&mem.mem, mem.size == 4 ? 4 : 2) + 2 +
           memory_tail_len(&mem.mem);
}

static nint encode_jmp_far_immediate(struct Parser *p, struct token *start,
                                     struct token *end, nint offset_size) {
    struct TokenStream tks;
    struct token *colon;
    struct token *error_token = NULL;
    nint seg = 0;
    nint off = 0;
    unint unresolved = 0;
    unint is_variable = 0;
    nint status;

    if(offset_size != 2 && offset_size != 4) {
        _error_from_token(p, start, ERROR_TYPE_I386,
                          "invalid far jump pointer size");
        return INSTRUCTION_FAILED;
    }

    tks_init(&tks, start, end);
    status = read_jump_scalar(p, &tks, &seg, &unresolved, &is_variable, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token ? error_token : start, ERROR_TYPE_I386,
                          "invalid far jump segment");
        return INSTRUCTION_FAILED;
    }

    colon = tks_read(&tks);
    if(!colon || colon->type != COLON) {
        _error_from_token(p, colon ? colon : start, ERROR_TYPE_I386,
                          "expected far jump separator");
        return INSTRUCTION_FAILED;
    }
    record_error_token(&error_token, colon);

    status = read_jump_scalar(p, &tks, &off, &unresolved, &is_variable, &error_token);
    if(status <= 0 || !expect_tks_end(&tks, &error_token)) {
        _error_from_token(p, error_token ? error_token : start, ERROR_TYPE_I386,
                          "invalid far jump offset");
        return INSTRUCTION_FAILED;
    }

    if(unresolved && !p->last_pass) return offset_size == 4 ? 7 : 5;
    if(!fits_u16(seg) || (offset_size == 2 && !fits_u16(off)) ||
       (offset_size == 4 && !fits_u32(off))) {
        _error_from_token(p, start, ERROR_TYPE_OVERFLOW,
                          "far jump pointer does not fit");
        return INSTRUCTION_FAILED;
    }

    if(offset_size == 4) emit_byte(p, 0x66);
    emit_byte(p, 0xea);
    if(offset_size == 4) emit_dword(p, off);
    else emit_word(p, off);
    emit_word(p, seg);
    return offset_size == 4 ? 7 : 5;
}

static nint encode_jmp_direct(struct Parser *p, struct token *start,
                              struct token *end, nint explicit_size) {
    struct TokenStream tks;
    struct token *error_token = NULL;
    nint target = 0;
    nint size = explicit_size;
    nint cached;
    unint unresolved = 0;
    unint is_variable = 0;
    nint status;

    tks_init(&tks, start, end);
    status = read_jump_scalar(p, &tks, &target, &unresolved, &is_variable, &error_token);
    if(status <= 0 || !expect_tks_end(&tks, &error_token)) {
        _error_from_token(p, error_token ? error_token : start, ERROR_TYPE_I386,
                          "invalid jump target");
        return INSTRUCTION_FAILED;
    }

    if(explicit_size) {
        if(unresolved && !p->last_pass) return explicit_size;
        return emit_jmp_relative(p, target, explicit_size, start);
    }

    if(!p->last_pass) {
        if(unresolved) {
            remember_jmp_size(p->addr, 3);
            return 3;
        }
        size = fits_i8(target - (p->addr + 2)) ? 2 : 3;
        remember_jmp_size(p->addr, size);
        return size;
    }

    cached = remembered_jmp_size(p->addr);
    if(cached) size = cached;
    else if(is_variable) size = 3;
    else size = fits_i8(target - (p->addr + 2)) ? 2 : 3;

    return emit_jmp_relative(p, target, size, start);
}

static nint encode_jmp(struct Parser *p, struct AstInstruction *inst) {
    struct TokenStream tks;
    struct token *tok;
    struct token *ptr_tok;
    struct token *target_start;
    struct Operand dst;
    nint is_far;
    nint mem_status;
    nint status;
    struct Operand mem;
    struct token *error_token = NULL;

    if(inst->arg_count != 1) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    tks_init(&tks, inst->args_head->_s, inst->args_head->_e);
    tok = tks_read(&tks);
    if(!tok) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "invalid jump target");
        return INSTRUCTION_FAILED;
    }

    if(token_is(tok, SHORT_TYPE)) {
        target_start = tks_read(&tks);
        if(!target_start) {
            _error_from_token(p, tok, ERROR_TYPE_I386, "invalid jump target");
            return INSTRUCTION_FAILED;
        }
        return encode_jmp_direct(p, target_start, inst->args_head->_e, 2);
    }

    if(token_is(tok, NEAR_TYPE) || token_is(tok, FAR_TYPE)) {
        is_far = token_is(tok, FAR_TYPE);
        ptr_tok = tks_read(&tks);
        if(ptr_tok && explicit_type_size(ptr_tok)) {
            struct token *size_start = ptr_tok;
            nint ptr_size = explicit_type_size(ptr_tok);
            struct token *type_ptr_tok = tks_read(&tks);
            if(!type_ptr_tok || !token_is(type_ptr_tok, PTR_NAME)) {
                _error_from_token(p, ptr_tok, ERROR_TYPE_I386, "expected ptr");
                return INSTRUCTION_FAILED;
            }
            target_start = tks_read(&tks);
            if(!target_start) {
                _error_from_token(p, type_ptr_tok, ERROR_TYPE_I386,
                                  "invalid jump pointer");
                return INSTRUCTION_FAILED;
            }
            mem_status = parse_jump_memory_from(p, size_start,
                                                inst->args_head->_e, &mem,
                                                &error_token);
            if(mem_status > 0) {
                return encode_jmp_ptr_mem(p, size_start, inst->args_head->_e,
                                          is_far);
            }
            if(mem_status < 0) return INSTRUCTION_FAILED;
            if(is_far) {
                return encode_jmp_far_immediate(p, target_start,
                                                inst->args_head->_e, ptr_size);
            }
            return encode_jmp_ptr_mem(p, size_start, inst->args_head->_e,
                                      is_far);
        }
        if(ptr_tok && token_is(ptr_tok, PTR_NAME)) {
            target_start = tks_read(&tks);
            if(!target_start) {
                _error_from_token(p, ptr_tok, ERROR_TYPE_I386,
                                  "invalid jump pointer");
                return INSTRUCTION_FAILED;
            }
            if(!is_far) {
                return encode_jmp_ptr_mem(p, target_start, inst->args_head->_e, 0);
            }

            mem_status = parse_jump_memory_from(p, target_start,
                                                inst->args_head->_e, &mem,
                                                &error_token);
            if(mem_status > 0) {
                return encode_jmp_ptr_mem(p, target_start,
                                          inst->args_head->_e, 1);
            }
            return encode_jmp_far_immediate(p, target_start, inst->args_head->_e, 2);
        }
        if(is_far) {
            _error_from_token(p, tok, ERROR_TYPE_I386, "expected ptr");
            return INSTRUCTION_FAILED;
        }
        if(ptr_tok) return encode_jmp_direct(p, ptr_tok, inst->args_head->_e, 3);
        _error_from_token(p, tok, ERROR_TYPE_I386, "invalid jump target");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inst->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid jump target");
        return INSTRUCTION_FAILED;
    }

    if(dst.kind == OPERAND_REG16) {
        emit_byte(p, 0xff);
        emit_modrm_reg(p, 4, dst.reg);
        return 2;
    }

    if(dst.kind == OPERAND_REG32) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0xff);
        emit_modrm_reg(p, 4, dst.reg);
        return 3;
    }

    if(dst.kind == OPERAND_MEM) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "ambiguous jump pointer");
        return INSTRUCTION_FAILED;
    }

    if(dst.kind == OPERAND_IMM) {
        return encode_jmp_direct(p, inst->args_head->_s, inst->args_head->_e, 0);
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported jmp form");
    return INSTRUCTION_FAILED;
}

static nint encode_rel8_control(struct Parser *p, struct AstInstruction *inst,
                                unsigned char opcode) {
    struct TokenStream tks;
    struct token *error_token = NULL;
    nint target = 0;
    nint disp;
    unint unresolved = 0;
    unint is_variable = 0;
    nint status;

    if(inst->arg_count != 1) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    tks_init(&tks, inst->args_head->_s, inst->args_head->_e);
    status = read_jump_scalar(p, &tks, &target, &unresolved,
                              &is_variable, &error_token);
    if(status <= 0 || !expect_tks_end(&tks, &error_token)) {
        _error_from_token(p, error_token ? error_token : inst->args_head->_s,
                          ERROR_TYPE_I386, "invalid branch target");
        return INSTRUCTION_FAILED;
    }

    if(unresolved && !p->last_pass) return 2;
    disp = target - (p->addr + 2);
    if(!fits_i8(disp)) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_OVERFLOW,
                          "branch target out of range");
        return INSTRUCTION_FAILED;
    }

    emit_byte(p, opcode);
    emit_byte(p, (unsigned char)(disp & 0xff));
    return 2;
}

static nint encode_jecxz(struct Parser *p, struct AstInstruction *inst) {
    struct TokenStream tks;
    struct token *error_token = NULL;
    nint target = 0;
    nint disp;
    unint unresolved = 0;
    unint is_variable = 0;
    nint status;

    if(inst->arg_count != 1) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    tks_init(&tks, inst->args_head->_s, inst->args_head->_e);
    status = read_jump_scalar(p, &tks, &target, &unresolved,
                              &is_variable, &error_token);
    if(status <= 0 || !expect_tks_end(&tks, &error_token)) {
        _error_from_token(p, error_token ? error_token : inst->args_head->_s,
                          ERROR_TYPE_I386, "invalid branch target");
        return INSTRUCTION_FAILED;
    }

    if(unresolved && !p->last_pass) return 3;
    disp = target - (p->addr + 3);
    if(!fits_i8(disp)) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_OVERFLOW,
                          "branch target out of range");
        return INSTRUCTION_FAILED;
    }

    emit_byte(p, 0x67);
    emit_byte(p, 0xe3);
    emit_byte(p, (unsigned char)(disp & 0xff));
    return 3;
}

static nint emit_cond_relative(struct Parser *p, nint target, nint size,
                               unsigned char short_opcode,
                               unsigned char near_opcode,
                               struct token *tok) {
    nint disp = target - (p->addr + size);
    if(size == 2) {
        if(!fits_i8(disp)) {
            _error_from_token(p, tok, ERROR_TYPE_OVERFLOW,
                              "branch target out of range");
            return INSTRUCTION_FAILED;
        }
        emit_byte(p, short_opcode);
        emit_byte(p, (unsigned char)(disp & 0xff));
        return 2;
    }
    if(!fits_i16(disp)) {
        _error_from_token(p, tok, ERROR_TYPE_OVERFLOW,
                          "near branch target out of range");
        return INSTRUCTION_FAILED;
    }
    emit_byte(p, 0x0f);
    emit_byte(p, near_opcode);
    emit_word(p, disp);
    return 4;
}

static nint encode_conditional_jump(struct Parser *p, struct AstInstruction *inst,
                                    unsigned char short_opcode,
                                    unsigned char near_opcode) {
    struct TokenStream tks;
    struct token *tok;
    struct token *target_start;
    struct token *error_token = NULL;
    nint target = 0;
    nint size = 2;
    unint unresolved = 0;
    unint is_variable = 0;
    nint status;

    if(inst->arg_count != 1) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    tks_init(&tks, inst->args_head->_s, inst->args_head->_e);
    tok = tks_read(&tks);
    if(!tok) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "invalid branch target");
        return INSTRUCTION_FAILED;
    }

    target_start = tok;
    if(token_is(tok, SHORT_TYPE)) {
        target_start = tks_read(&tks);
        if(!target_start) {
            _error_from_token(p, tok, ERROR_TYPE_I386, "invalid branch target");
            return INSTRUCTION_FAILED;
        }
        size = 2;
    } else if(token_is(tok, NEAR_TYPE)) {
        target_start = tks_read(&tks);
        if(!target_start) {
            _error_from_token(p, tok, ERROR_TYPE_I386, "invalid branch target");
            return INSTRUCTION_FAILED;
        }
        size = 4;
    }

    tks_init(&tks, target_start, inst->args_head->_e);
    status = read_jump_scalar(p, &tks, &target, &unresolved,
                              &is_variable, &error_token);
    if(status <= 0 || !expect_tks_end(&tks, &error_token)) {
        _error_from_token(p, error_token ? error_token : target_start,
                          ERROR_TYPE_I386, "invalid branch target");
        return INSTRUCTION_FAILED;
    }

    if(unresolved && !p->last_pass) return size;
    (void)is_variable;
    return emit_cond_relative(p, target, size, short_opcode, near_opcode,
                              target_start);
}

static nint emit_call_relative(struct Parser *p, nint target,
                               struct token *tok) {
    nint disp = target - (p->addr + 3);
    if(!fits_i16(disp)) {
        _error_from_token(p, tok, ERROR_TYPE_OVERFLOW,
                          "near call target out of range");
        return INSTRUCTION_FAILED;
    }
    emit_byte(p, 0xe8);
    emit_word(p, disp);
    return 3;
}

static nint parse_call_memory_from(struct Parser *p, struct token *start,
                                   struct token *end, struct Operand *mem,
                                   struct token **error_token) {
    struct InstructionArg arg;
    nint status;

    arg._s = start;
    arg._e = end;
    arg.next = NULL;
    status = parse_memory(p, &arg, mem, error_token);
    if(status <= 0) return status;
    if(mem->size != 0 && mem->size != 2 && mem->size != 4) {
        _error_from_token(p, start, ERROR_TYPE_I386,
                          "invalid call pointer size");
        return INSTRUCTION_FAILED;
    }
    return 1;
}

static nint encode_call_ptr_mem(struct Parser *p, struct token *start,
                                struct token *end, nint far_ptr) {
    struct Operand mem;
    struct token *error_token = NULL;
    nint status = parse_call_memory_from(p, start, end, &mem, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token ? error_token : start, ERROR_TYPE_I386,
                          "invalid call pointer");
        return INSTRUCTION_FAILED;
    }

    emit_memory_operand_prefixes(p, &mem.mem, mem.size == 4 ? 4 : 2);
    emit_byte(p, 0xff);
    emit_modrm_mem(p, far_ptr ? 3 : 2, &mem.mem);
    return memory_operand_prefix_len(&mem.mem, mem.size == 4 ? 4 : 2) + 2 +
           memory_tail_len(&mem.mem);
}

static nint encode_call_far_immediate(struct Parser *p, struct token *start,
                                      struct token *end, nint offset_size) {
    struct TokenStream tks;
    struct token *colon;
    struct token *error_token = NULL;
    nint seg = 0;
    nint off = 0;
    unint unresolved = 0;
    unint is_variable = 0;
    nint status;

    if(offset_size != 2 && offset_size != 4) {
        _error_from_token(p, start, ERROR_TYPE_I386,
                          "invalid far call pointer size");
        return INSTRUCTION_FAILED;
    }

    tks_init(&tks, start, end);
    status = read_jump_scalar(p, &tks, &seg, &unresolved, &is_variable, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token ? error_token : start, ERROR_TYPE_I386,
                          "invalid far call segment");
        return INSTRUCTION_FAILED;
    }

    colon = tks_read(&tks);
    if(!colon || colon->type != COLON) {
        _error_from_token(p, colon ? colon : start, ERROR_TYPE_I386,
                          "expected far call separator");
        return INSTRUCTION_FAILED;
    }
    record_error_token(&error_token, colon);

    status = read_jump_scalar(p, &tks, &off, &unresolved, &is_variable, &error_token);
    if(status <= 0 || !expect_tks_end(&tks, &error_token)) {
        _error_from_token(p, error_token ? error_token : start, ERROR_TYPE_I386,
                          "invalid far call offset");
        return INSTRUCTION_FAILED;
    }

    if(unresolved && !p->last_pass) return offset_size == 4 ? 7 : 5;
    if(!fits_u16(seg) || (offset_size == 2 && !fits_u16(off)) ||
       (offset_size == 4 && !fits_u32(off))) {
        _error_from_token(p, start, ERROR_TYPE_OVERFLOW,
                          "far call pointer does not fit");
        return INSTRUCTION_FAILED;
    }

    if(offset_size == 4) emit_byte(p, 0x66);
    emit_byte(p, 0x9a);
    if(offset_size == 4) emit_dword(p, off);
    else emit_word(p, off);
    emit_word(p, seg);
    return offset_size == 4 ? 7 : 5;
}

static nint encode_call_direct(struct Parser *p, struct token *start,
                               struct token *end) {
    struct TokenStream tks;
    struct token *error_token = NULL;
    nint target = 0;
    unint unresolved = 0;
    unint is_variable = 0;
    nint status;

    tks_init(&tks, start, end);
    status = read_jump_scalar(p, &tks, &target, &unresolved, &is_variable, &error_token);
    if(status <= 0 || !expect_tks_end(&tks, &error_token)) {
        _error_from_token(p, error_token ? error_token : start, ERROR_TYPE_I386,
                          "invalid call target");
        return INSTRUCTION_FAILED;
    }

    if(unresolved && !p->last_pass) return 3;
    return emit_call_relative(p, target, start);
}

static nint encode_call(struct Parser *p, struct AstInstruction *inst) {
    struct TokenStream tks;
    struct token *tok;
    struct token *ptr_tok;
    struct token *target_start;
    struct Operand dst;
    struct Operand mem;
    struct token *error_token = NULL;
    nint is_far;
    nint mem_status;
    nint status;

    if(inst->arg_count != 1) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    tks_init(&tks, inst->args_head->_s, inst->args_head->_e);
    tok = tks_read(&tks);
    if(!tok) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "invalid call target");
        return INSTRUCTION_FAILED;
    }

    if(token_is(tok, SHORT_TYPE)) {
        _error_from_token(p, tok, ERROR_TYPE_I386, "invalid call target size");
        return INSTRUCTION_FAILED;
    }

    if(token_is(tok, NEAR_TYPE) || token_is(tok, FAR_TYPE)) {
        is_far = token_is(tok, FAR_TYPE);
        ptr_tok = tks_read(&tks);
        if(ptr_tok && explicit_type_size(ptr_tok)) {
            struct token *size_start = ptr_tok;
            nint ptr_size = explicit_type_size(ptr_tok);
            struct token *type_ptr_tok = tks_read(&tks);
            if(!type_ptr_tok || !token_is(type_ptr_tok, PTR_NAME)) {
                _error_from_token(p, ptr_tok, ERROR_TYPE_I386, "expected ptr");
                return INSTRUCTION_FAILED;
            }
            target_start = tks_read(&tks);
            if(!target_start) {
                _error_from_token(p, type_ptr_tok, ERROR_TYPE_I386,
                                  "invalid call pointer");
                return INSTRUCTION_FAILED;
            }
            mem_status = parse_call_memory_from(p, size_start,
                                                inst->args_head->_e, &mem,
                                                &error_token);
            if(mem_status > 0) {
                return encode_call_ptr_mem(p, size_start, inst->args_head->_e,
                                           is_far);
            }
            if(mem_status < 0) return INSTRUCTION_FAILED;
            if(is_far) {
                return encode_call_far_immediate(p, target_start,
                                                 inst->args_head->_e, ptr_size);
            }
            return encode_call_ptr_mem(p, size_start, inst->args_head->_e,
                                       is_far);
        }
        if(ptr_tok && token_is(ptr_tok, PTR_NAME)) {
            target_start = tks_read(&tks);
            if(!target_start) {
                _error_from_token(p, ptr_tok, ERROR_TYPE_I386,
                                  "invalid call pointer");
                return INSTRUCTION_FAILED;
            }
            if(!is_far) {
                return encode_call_ptr_mem(p, target_start, inst->args_head->_e, 0);
            }

            mem_status = parse_call_memory_from(p, target_start,
                                                inst->args_head->_e, &mem,
                                                &error_token);
            if(mem_status > 0) {
                return encode_call_ptr_mem(p, target_start,
                                           inst->args_head->_e, 1);
            }
            return encode_call_far_immediate(p, target_start, inst->args_head->_e, 2);
        }
        if(is_far) {
            _error_from_token(p, tok, ERROR_TYPE_I386, "expected ptr");
            return INSTRUCTION_FAILED;
        }
        if(ptr_tok) return encode_call_direct(p, ptr_tok, inst->args_head->_e);
        _error_from_token(p, tok, ERROR_TYPE_I386, "invalid call target");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inst->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid call target");
        return INSTRUCTION_FAILED;
    }

    if(dst.kind == OPERAND_REG16) {
        emit_byte(p, 0xff);
        emit_modrm_reg(p, 2, dst.reg);
        return 2;
    }

    if(dst.kind == OPERAND_REG32) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0xff);
        emit_modrm_reg(p, 2, dst.reg);
        return 3;
    }

    if(dst.kind == OPERAND_MEM) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "ambiguous call pointer");
        return INSTRUCTION_FAILED;
    }

    if(dst.kind == OPERAND_IMM) {
        return encode_call_direct(p, inst->args_head->_s, inst->args_head->_e);
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported call form");
    return INSTRUCTION_FAILED;
}

static nint encode_ret(struct Parser *p, struct AstInstruction *inst,
                       unsigned char plain_opcode,
                       unsigned char imm_opcode) {
    struct Operand imm;
    struct token *error_token = NULL;
    nint status;

    if(inst->arg_count == 0) {
        emit_byte(p, plain_opcode);
        return 1;
    }

    if(inst->arg_count != 1) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inst->args_head, &imm, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid return pop count");
        return INSTRUCTION_FAILED;
    }

    if(imm.kind != OPERAND_IMM) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "invalid return pop count");
        return INSTRUCTION_FAILED;
    }

    if(!fits_u16(imm.imm)) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_OVERFLOW,
                          "return pop count does not fit 16 bits");
        return INSTRUCTION_FAILED;
    }

    emit_byte(p, imm_opcode);
    emit_word(p, imm.imm);
    return 3;
}

static nint encode_prefixed_ret(struct Parser *p,
                                struct AstInstruction *inner,
                                unsigned char plain_opcode,
                                unsigned char imm_opcode,
                                unint operand_prefix,
                                unint address_prefix) {
    nint status;

    if(operand_prefix) emit_operand_size_prefix(p, 4);
    if(address_prefix) emit_byte(p, 0x67);
    status = encode_ret(p, inner, plain_opcode, imm_opcode);
    return status < 0 ? status : status + (operand_prefix ? 1 : 0) +
           (address_prefix ? 1 : 0);
}

static nint encode_enter(struct Parser *p, struct AstInstruction *inst) {
    struct Operand frame_size;
    struct Operand nesting_level;
    struct token *error_token = NULL;
    nint status;

    if(inst->arg_count != 2) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inst->args_head, &frame_size, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid frame size");
        return INSTRUCTION_FAILED;
    }

    error_token = NULL;
    status = parse_operand(p, inst->args_tail, &nesting_level, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid nesting level");
        return INSTRUCTION_FAILED;
    }

    if(frame_size.kind != OPERAND_IMM) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "invalid frame size");
        return INSTRUCTION_FAILED;
    }

    if(nesting_level.kind != OPERAND_IMM) {
        _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                          "invalid nesting level");
        return INSTRUCTION_FAILED;
    }

    emit_byte(p, 0xc8);
    emit_word(p, frame_size.imm);
    emit_byte(p, (unsigned char)(nesting_level.imm & 0xff));
    return 4;
}

static nint encode_prefixed_enter(struct Parser *p,
                                  struct AstInstruction *inner,
                                  unint operand_prefix,
                                  unint address_prefix) {
    nint status;
    if(operand_prefix) emit_operand_size_prefix(p, 4);
    if(address_prefix) emit_byte(p, 0x67);
    status = encode_enter(p, inner);
    if(status < 0) return status;
    return status + (operand_prefix ? 1 : 0) + (address_prefix ? 1 : 0);
}

static nint encode_xchg(struct Parser *p, struct AstInstruction *inst,
                        struct Operand *dst, struct Operand *src) {
    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_REG16) {
        if(dst->reg == 0) {
            emit_byte(p, (unsigned char)(0x90 + src->reg));
            return 1;
        }
        if(src->reg == 0) {
            emit_byte(p, (unsigned char)(0x90 + dst->reg));
            return 1;
        }
        emit_byte(p, 0x87);
        emit_modrm_reg(p, dst->reg, src->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_REG8) {
        emit_byte(p, 0x86);
        emit_modrm_reg(p, dst->reg, src->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG32 && src->kind == OPERAND_REG32) {
        emit_operand_size_prefix(p, 4);
        if(dst->reg == 0) {
            emit_byte(p, (unsigned char)(0x90 + src->reg));
            return 2;
        }
        if(src->reg == 0) {
            emit_byte(p, (unsigned char)(0x90 + dst->reg));
            return 2;
        }
        emit_byte(p, 0x87);
        emit_modrm_reg(p, dst->reg, src->reg);
        return 3;
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG8) {
        if(!memory_size_matches(dst, 1)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid xchg operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        emit_byte(p, 0x86);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }
    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG16) {
        if(!memory_size_matches(dst, 2)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid xchg operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        emit_byte(p, 0x87);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG32) {
        if(!memory_size_matches(dst, 4)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid xchg operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &dst->mem, 4);
        emit_byte(p, 0x87);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return memory_operand_prefix_len(&dst->mem, 4) + 2 +
               memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_MEM) {
        if(!memory_size_matches(src, 1)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid xchg operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &src->mem);
        emit_byte(p, 0x86);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_MEM) {
        if(!memory_size_matches(src, 2)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid xchg operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &src->mem);
        emit_byte(p, 0x87);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(dst->kind == OPERAND_REG32 && src->kind == OPERAND_MEM) {
        if(!memory_size_matches(src, 4)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid xchg operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &src->mem, 4);
        emit_byte(p, 0x87);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_operand_prefix_len(&src->mem, 4) + 2 +
               memory_tail_len(&src->mem);
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported xchg form");
    return INSTRUCTION_FAILED;
}

static nint emit_xchg_memory_reg(struct Parser *p,
                                 struct AstInstruction *inst,
                                 struct Operand *mem,
                                 struct Operand *reg,
                                 unint mem_first,
                                 unint force_o32) {
    if(reg->kind == OPERAND_REG8) {
        if(!memory_size_matches(mem, 1)) {
            _error_from_token(p, mem_first ? inst->args_head->_s :
                                           inst->args_tail->_s,
                              ERROR_TYPE_I386, "invalid xchg operand size");
            return INSTRUCTION_FAILED;
        }
        if(force_o32) emit_forced_o32_memory_prefixes(p, &mem->mem);
        else emit_memory_prefixes(p, &mem->mem);
        emit_byte(p, 0x86);
        emit_modrm_mem(p, reg->reg, &mem->mem);
        return (force_o32 ? forced_o32_memory_prefix_len(&mem->mem) :
                            memory_prefix_len(&mem->mem)) +
               2 + memory_tail_len(&mem->mem);
    }

    if(reg->kind == OPERAND_REG16) {
        if(!memory_size_matches(mem, 2)) {
            _error_from_token(p, mem_first ? inst->args_head->_s :
                                           inst->args_tail->_s,
                              ERROR_TYPE_I386, "invalid xchg operand size");
            return INSTRUCTION_FAILED;
        }
        if(force_o32) emit_forced_o32_memory_prefixes(p, &mem->mem);
        else emit_memory_prefixes(p, &mem->mem);
        emit_byte(p, 0x87);
        emit_modrm_mem(p, reg->reg, &mem->mem);
        return (force_o32 ? forced_o32_memory_prefix_len(&mem->mem) :
                            memory_prefix_len(&mem->mem)) +
               2 + memory_tail_len(&mem->mem);
    }

    if(reg->kind == OPERAND_REG32) {
        if(!memory_size_matches(mem, 4)) {
            _error_from_token(p, mem_first ? inst->args_head->_s :
                                           inst->args_tail->_s,
                              ERROR_TYPE_I386, "invalid xchg operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &mem->mem, 4);
        emit_byte(p, 0x87);
        emit_modrm_mem(p, reg->reg, &mem->mem);
        return memory_operand_prefix_len(&mem->mem, 4) + 2 +
               memory_tail_len(&mem->mem);
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported xchg form");
    return INSTRUCTION_FAILED;
}

static nint encode_prefixed_xchg(struct Parser *p,
                                 struct AstInstruction *inner,
                                 unint force_o32,
                                 unint force_a16,
                                 unint force_a32) {
    struct Operand dst;
    struct Operand src;
    struct token *error_token = NULL;
    nint status;
    unint op_prefix = force_o32;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }
    error_token = NULL;
    status = parse_operand(p, inner->args_tail, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    if(force_a32) {
        force_direct_memory_a32(&dst);
        force_direct_memory_a32(&src);
    }

    if(force_a16 && (operand_is_mem32(&dst) || operand_is_mem32(&src))) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid a16 address form");
        return INSTRUCTION_FAILED;
    }
    if(force_a32 && (operand_is_mem16(&dst) || operand_is_mem16(&src))) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid a32 address form");
        return INSTRUCTION_FAILED;
    }

    if(dst.kind == OPERAND_REG8 && src.kind == OPERAND_REG8) {
        emit_forced_prefix_pair(p, op_prefix, force_a32);
        emit_byte(p, 0x86);
        emit_modrm_reg(p, dst.reg, src.reg);
        return forced_prefix_pair_len(op_prefix, force_a32) + 2;
    }

    if(dst.kind == OPERAND_REG16 && src.kind == OPERAND_REG16) {
        emit_forced_prefix_pair(p, op_prefix, force_a32);
        if(dst.reg == 0) {
            emit_byte(p, (unsigned char)(0x90 + src.reg));
            return forced_prefix_pair_len(op_prefix, force_a32) + 1;
        }
        if(src.reg == 0) {
            emit_byte(p, (unsigned char)(0x90 + dst.reg));
            return forced_prefix_pair_len(op_prefix, force_a32) + 1;
        }
        emit_byte(p, 0x87);
        emit_modrm_reg(p, dst.reg, src.reg);
        return forced_prefix_pair_len(op_prefix, force_a32) + 2;
    }

    if(dst.kind == OPERAND_REG32 && src.kind == OPERAND_REG32) {
        op_prefix = 1;
        emit_forced_prefix_pair(p, op_prefix, force_a32);
        if(dst.reg == 0) {
            emit_byte(p, (unsigned char)(0x90 + src.reg));
            return forced_prefix_pair_len(op_prefix, force_a32) + 1;
        }
        if(src.reg == 0) {
            emit_byte(p, (unsigned char)(0x90 + dst.reg));
            return forced_prefix_pair_len(op_prefix, force_a32) + 1;
        }
        emit_byte(p, 0x87);
        emit_modrm_reg(p, dst.reg, src.reg);
        return forced_prefix_pair_len(op_prefix, force_a32) + 2;
    }

    if(dst.kind == OPERAND_MEM) {
        return emit_xchg_memory_reg(p, inner, &dst, &src, 1, force_o32);
    }
    if(src.kind == OPERAND_MEM) {
        return emit_xchg_memory_reg(p, inner, &src, &dst, 0, force_o32);
    }

    _error_from_token(p, inner->name, ERROR_TYPE_I386, "unsupported xchg form");
    return INSTRUCTION_FAILED;
}

static nint emit_and_reg_imm(struct Parser *p, struct AstInstruction *inst,
                             struct Operand *dst, struct Operand *src) {
    if(dst->kind == OPERAND_REG8) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        if(is_al(dst)) {
            emit_byte(p, 0x24);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 2;
        }
        emit_byte(p, 0x80);
        emit_modrm_reg(p, 4, dst->reg);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return 3;
    }

    if(dst->kind == OPERAND_REG16) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        if(fits_word_sign_extended_i8(src->imm)) {
            emit_byte(p, 0x83);
            emit_modrm_reg(p, 4, dst->reg);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 3;
        }
        if(is_ax(dst)) {
            emit_byte(p, 0x25);
            emit_word(p, src->imm);
            return 3;
        }
        emit_byte(p, 0x81);
        emit_modrm_reg(p, 4, dst->reg);
        emit_word(p, src->imm);
        return 4;
    }

    if(dst->kind == OPERAND_REG32) {
        if(!fits_u32(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 32 bits");
            return INSTRUCTION_FAILED;
        }
        emit_operand_size_prefix(p, 4);
        if(fits_dword_sign_extended_i8(src->imm)) {
            emit_byte(p, 0x83);
            emit_modrm_reg(p, 4, dst->reg);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 4;
        }
        if(is_eax(dst)) {
            emit_byte(p, 0x25);
            emit_dword(p, src->imm);
            return 6;
        }
        emit_byte(p, 0x81);
        emit_modrm_reg(p, 4, dst->reg);
        emit_dword(p, src->imm);
        return 7;
    }

    return 0;
}

static nint emit_and_mem_imm(struct Parser *p, struct AstInstruction *inst,
                             struct Operand *dst, struct Operand *src) {
    if(dst->size == 1) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        emit_byte(p, 0x80);
        emit_modrm_mem(p, 4, &dst->mem);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 1;
    }

    if(dst->size == 2) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        if(fits_word_sign_extended_i8(src->imm)) {
            emit_byte(p, 0x83);
            emit_modrm_mem(p, 4, &dst->mem);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 1;
        }
        emit_byte(p, 0x81);
        emit_modrm_mem(p, 4, &dst->mem);
        emit_word(p, src->imm);
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 2;
    }

    if(dst->size == 4) {
        if(!fits_u32(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 32 bits");
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &dst->mem, 4);
        if(fits_dword_sign_extended_i8(src->imm)) {
            emit_byte(p, 0x83);
            emit_modrm_mem(p, 4, &dst->mem);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return memory_operand_prefix_len(&dst->mem, 4) + 2 +
                   memory_tail_len(&dst->mem) + 1;
        }
        emit_byte(p, 0x81);
        emit_modrm_mem(p, 4, &dst->mem);
        emit_dword(p, src->imm);
        return memory_operand_prefix_len(&dst->mem, 4) + 2 +
               memory_tail_len(&dst->mem) + 4;
    }

    _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                      "ambiguous memory immediate size");
    return INSTRUCTION_FAILED;
}

static nint encode_and(struct Parser *p, struct AstInstruction *inst,
                       struct Operand *dst, struct Operand *src) {
    nint status;

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_REG8) {
        emit_byte(p, 0x20);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_REG16) {
        emit_byte(p, 0x21);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG32 && src->kind == OPERAND_REG32) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x21);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 3;
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG8) {
        if(!memory_size_matches(dst, 1)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid and operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        emit_byte(p, 0x20);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG16) {
        if(!memory_size_matches(dst, 2)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid and operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        emit_byte(p, 0x21);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG32) {
        if(!memory_size_matches(dst, 4)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid and operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &dst->mem, 4);
        emit_byte(p, 0x21);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return memory_operand_prefix_len(&dst->mem, 4) + 2 +
               memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_MEM) {
        if(!memory_size_matches(src, 1)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid and operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &src->mem);
        emit_byte(p, 0x22);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_MEM) {
        if(!memory_size_matches(src, 2)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid and operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &src->mem);
        emit_byte(p, 0x23);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(dst->kind == OPERAND_REG32 && src->kind == OPERAND_MEM) {
        if(!memory_size_matches(src, 4)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid and operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &src->mem, 4);
        emit_byte(p, 0x23);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_operand_prefix_len(&src->mem, 4) + 2 +
               memory_tail_len(&src->mem);
    }

    if(src->kind == OPERAND_IMM) {
        status = emit_and_reg_imm(p, inst, dst, src);
        if(status) return status;
        if(dst->kind == OPERAND_MEM) return emit_and_mem_imm(p, inst, dst, src);
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported and form");
    return INSTRUCTION_FAILED;
}

static nint emit_or_reg_imm(struct Parser *p, struct AstInstruction *inst,
                            struct Operand *dst, struct Operand *src) {
    if(dst->kind == OPERAND_REG8) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        if(is_al(dst)) {
            emit_byte(p, 0x0c);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 2;
        }
        emit_byte(p, 0x80);
        emit_modrm_reg(p, 1, dst->reg);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return 3;
    }

    if(dst->kind == OPERAND_REG16) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        if(fits_word_sign_extended_i8(src->imm)) {
            emit_byte(p, 0x83);
            emit_modrm_reg(p, 1, dst->reg);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 3;
        }
        if(is_ax(dst)) {
            emit_byte(p, 0x0d);
            emit_word(p, src->imm);
            return 3;
        }
        emit_byte(p, 0x81);
        emit_modrm_reg(p, 1, dst->reg);
        emit_word(p, src->imm);
        return 4;
    }

    if(dst->kind == OPERAND_REG32) {
        if(!fits_u32(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 32 bits");
            return INSTRUCTION_FAILED;
        }
        emit_operand_size_prefix(p, 4);
        if(fits_dword_sign_extended_i8(src->imm)) {
            emit_byte(p, 0x83);
            emit_modrm_reg(p, 1, dst->reg);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 4;
        }
        if(is_eax(dst)) {
            emit_byte(p, 0x0d);
            emit_dword(p, src->imm);
            return 6;
        }
        emit_byte(p, 0x81);
        emit_modrm_reg(p, 1, dst->reg);
        emit_dword(p, src->imm);
        return 7;
    }

    return 0;
}

static nint emit_or_mem_imm(struct Parser *p, struct AstInstruction *inst,
                            struct Operand *dst, struct Operand *src) {
    if(dst->size == 1) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        emit_byte(p, 0x80);
        emit_modrm_mem(p, 1, &dst->mem);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 1;
    }

    if(dst->size == 2) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        if(fits_word_sign_extended_i8(src->imm)) {
            emit_byte(p, 0x83);
            emit_modrm_mem(p, 1, &dst->mem);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 1;
        }
        emit_byte(p, 0x81);
        emit_modrm_mem(p, 1, &dst->mem);
        emit_word(p, src->imm);
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 2;
    }

    if(dst->size == 4) {
        if(!fits_u32(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 32 bits");
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &dst->mem, 4);
        if(fits_dword_sign_extended_i8(src->imm)) {
            emit_byte(p, 0x83);
            emit_modrm_mem(p, 1, &dst->mem);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return memory_operand_prefix_len(&dst->mem, 4) + 2 +
                   memory_tail_len(&dst->mem) + 1;
        }
        emit_byte(p, 0x81);
        emit_modrm_mem(p, 1, &dst->mem);
        emit_dword(p, src->imm);
        return memory_operand_prefix_len(&dst->mem, 4) + 2 +
               memory_tail_len(&dst->mem) + 4;
    }

    _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                      "ambiguous memory immediate size");
    return INSTRUCTION_FAILED;
}

static nint encode_or(struct Parser *p, struct AstInstruction *inst,
                      struct Operand *dst, struct Operand *src) {
    nint status;

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_REG8) {
        emit_byte(p, 0x08);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_REG16) {
        emit_byte(p, 0x09);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG32 && src->kind == OPERAND_REG32) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x09);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 3;
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG8) {
        if(!memory_size_matches(dst, 1)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid or operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        emit_byte(p, 0x08);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG16) {
        if(!memory_size_matches(dst, 2)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid or operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        emit_byte(p, 0x09);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG32) {
        if(!memory_size_matches(dst, 4)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid or operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &dst->mem, 4);
        emit_byte(p, 0x09);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return memory_operand_prefix_len(&dst->mem, 4) + 2 +
               memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_MEM) {
        if(!memory_size_matches(src, 1)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid or operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &src->mem);
        emit_byte(p, 0x0a);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_MEM) {
        if(!memory_size_matches(src, 2)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid or operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &src->mem);
        emit_byte(p, 0x0b);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(dst->kind == OPERAND_REG32 && src->kind == OPERAND_MEM) {
        if(!memory_size_matches(src, 4)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid or operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &src->mem, 4);
        emit_byte(p, 0x0b);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_operand_prefix_len(&src->mem, 4) + 2 +
               memory_tail_len(&src->mem);
    }

    if(src->kind == OPERAND_IMM) {
        status = emit_or_reg_imm(p, inst, dst, src);
        if(status) return status;
        if(dst->kind == OPERAND_MEM) return emit_or_mem_imm(p, inst, dst, src);
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported or form");
    return INSTRUCTION_FAILED;
}

static nint emit_xor_reg_imm(struct Parser *p, struct AstInstruction *inst,
                             struct Operand *dst, struct Operand *src) {
    if(dst->kind == OPERAND_REG8) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        if(is_al(dst)) {
            emit_byte(p, 0x34);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 2;
        }
        emit_byte(p, 0x80);
        emit_modrm_reg(p, 6, dst->reg);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return 3;
    }

    if(dst->kind == OPERAND_REG16) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        if(fits_word_sign_extended_i8(src->imm)) {
            emit_byte(p, 0x83);
            emit_modrm_reg(p, 6, dst->reg);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 3;
        }
        if(is_ax(dst)) {
            emit_byte(p, 0x35);
            emit_word(p, src->imm);
            return 3;
        }
        emit_byte(p, 0x81);
        emit_modrm_reg(p, 6, dst->reg);
        emit_word(p, src->imm);
        return 4;
    }

    if(dst->kind == OPERAND_REG32) {
        if(!fits_u32(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 32 bits");
            return INSTRUCTION_FAILED;
        }
        emit_operand_size_prefix(p, 4);
        if(fits_dword_sign_extended_i8(src->imm)) {
            emit_byte(p, 0x83);
            emit_modrm_reg(p, 6, dst->reg);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 4;
        }
        if(is_eax(dst)) {
            emit_byte(p, 0x35);
            emit_dword(p, src->imm);
            return 6;
        }
        emit_byte(p, 0x81);
        emit_modrm_reg(p, 6, dst->reg);
        emit_dword(p, src->imm);
        return 7;
    }

    return 0;
}

static nint emit_xor_mem_imm(struct Parser *p, struct AstInstruction *inst,
                             struct Operand *dst, struct Operand *src) {
    if(dst->size == 1) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        emit_byte(p, 0x80);
        emit_modrm_mem(p, 6, &dst->mem);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 1;
    }

    if(dst->size == 2) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        if(fits_word_sign_extended_i8(src->imm)) {
            emit_byte(p, 0x83);
            emit_modrm_mem(p, 6, &dst->mem);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 1;
        }
        emit_byte(p, 0x81);
        emit_modrm_mem(p, 6, &dst->mem);
        emit_word(p, src->imm);
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 2;
    }

    if(dst->size == 4) {
        if(!fits_u32(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 32 bits");
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &dst->mem, 4);
        if(fits_dword_sign_extended_i8(src->imm)) {
            emit_byte(p, 0x83);
            emit_modrm_mem(p, 6, &dst->mem);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return memory_operand_prefix_len(&dst->mem, 4) + 2 +
                   memory_tail_len(&dst->mem) + 1;
        }
        emit_byte(p, 0x81);
        emit_modrm_mem(p, 6, &dst->mem);
        emit_dword(p, src->imm);
        return memory_operand_prefix_len(&dst->mem, 4) + 2 +
               memory_tail_len(&dst->mem) + 4;
    }

    _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                      "ambiguous memory immediate size");
    return INSTRUCTION_FAILED;
}

static nint encode_xor(struct Parser *p, struct AstInstruction *inst,
                       struct Operand *dst, struct Operand *src) {
    nint status;

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_REG8) {
        emit_byte(p, 0x30);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_REG16) {
        emit_byte(p, 0x31);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG32 && src->kind == OPERAND_REG32) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x31);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 3;
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG8) {
        if(!memory_size_matches(dst, 1)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid xor operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        emit_byte(p, 0x30);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG16) {
        if(!memory_size_matches(dst, 2)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid xor operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        emit_byte(p, 0x31);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG32) {
        if(!memory_size_matches(dst, 4)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid xor operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &dst->mem, 4);
        emit_byte(p, 0x31);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return memory_operand_prefix_len(&dst->mem, 4) + 2 +
               memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_MEM) {
        if(!memory_size_matches(src, 1)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid xor operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &src->mem);
        emit_byte(p, 0x32);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_MEM) {
        if(!memory_size_matches(src, 2)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid xor operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &src->mem);
        emit_byte(p, 0x33);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(dst->kind == OPERAND_REG32 && src->kind == OPERAND_MEM) {
        if(!memory_size_matches(src, 4)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid xor operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &src->mem, 4);
        emit_byte(p, 0x33);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_operand_prefix_len(&src->mem, 4) + 2 +
               memory_tail_len(&src->mem);
    }

    if(src->kind == OPERAND_IMM) {
        status = emit_xor_reg_imm(p, inst, dst, src);
        if(status) return status;
        if(dst->kind == OPERAND_MEM) return emit_xor_mem_imm(p, inst, dst, src);
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported xor form");
    return INSTRUCTION_FAILED;
}

static nint emit_test_reg_imm(struct Parser *p, struct AstInstruction *inst,
                              struct Operand *dst, struct Operand *src) {
    if(dst->kind == OPERAND_REG8) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        if(is_al(dst)) {
            emit_byte(p, 0xa8);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 2;
        }
        emit_byte(p, 0xf6);
        emit_modrm_reg(p, 0, dst->reg);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return 3;
    }

    if(dst->kind == OPERAND_REG16) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        if(is_ax(dst)) {
            emit_byte(p, 0xa9);
            emit_word(p, src->imm);
            return 3;
        }
        emit_byte(p, 0xf7);
        emit_modrm_reg(p, 0, dst->reg);
        emit_word(p, src->imm);
        return 4;
    }

    if(dst->kind == OPERAND_REG32) {
        if(!fits_u32(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 32 bits");
            return INSTRUCTION_FAILED;
        }
        emit_operand_size_prefix(p, 4);
        if(is_eax(dst)) {
            emit_byte(p, 0xa9);
            emit_dword(p, src->imm);
            return 6;
        }
        emit_byte(p, 0xf7);
        emit_modrm_reg(p, 0, dst->reg);
        emit_dword(p, src->imm);
        return 7;
    }

    return 0;
}

static nint emit_test_mem_imm(struct Parser *p, struct AstInstruction *inst,
                              struct Operand *dst, struct Operand *src) {
    if(dst->size == 1) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        emit_byte(p, 0xf6);
        emit_modrm_mem(p, 0, &dst->mem);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 1;
    }

    if(dst->size == 2) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        emit_byte(p, 0xf7);
        emit_modrm_mem(p, 0, &dst->mem);
        emit_word(p, src->imm);
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 2;
    }

    if(dst->size == 4) {
        if(!fits_u32(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 32 bits");
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &dst->mem, 4);
        emit_byte(p, 0xf7);
        emit_modrm_mem(p, 0, &dst->mem);
        emit_dword(p, src->imm);
        return memory_operand_prefix_len(&dst->mem, 4) + 2 +
               memory_tail_len(&dst->mem) + 4;
    }

    _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                      "ambiguous memory immediate size");
    return INSTRUCTION_FAILED;
}

static nint encode_test(struct Parser *p, struct AstInstruction *inst,
                        struct Operand *dst, struct Operand *src) {
    nint status;

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_REG8) {
        emit_byte(p, 0x84);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_REG16) {
        emit_byte(p, 0x85);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG32 && src->kind == OPERAND_REG32) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x85);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 3;
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG8) {
        if(!memory_size_matches(dst, 1)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid test operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        emit_byte(p, 0x84);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG16) {
        if(!memory_size_matches(dst, 2)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid test operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        emit_byte(p, 0x85);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG32) {
        if(!memory_size_matches(dst, 4)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid test operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &dst->mem, 4);
        emit_byte(p, 0x85);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return memory_operand_prefix_len(&dst->mem, 4) + 2 +
               memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_MEM) {
        if(!memory_size_matches(src, 1)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid test operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &src->mem);
        emit_byte(p, 0x84);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_MEM) {
        if(!memory_size_matches(src, 2)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid test operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &src->mem);
        emit_byte(p, 0x85);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(dst->kind == OPERAND_REG32 && src->kind == OPERAND_MEM) {
        if(!memory_size_matches(src, 4)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid test operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &src->mem, 4);
        emit_byte(p, 0x85);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_operand_prefix_len(&src->mem, 4) + 2 +
               memory_tail_len(&src->mem);
    }

    if(src->kind == OPERAND_IMM) {
        status = emit_test_reg_imm(p, inst, dst, src);
        if(status) return status;
        if(dst->kind == OPERAND_MEM) return emit_test_mem_imm(p, inst, dst, src);
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported test form");
    return INSTRUCTION_FAILED;
}

static nint shift_target_size(struct Parser *p, struct AstInstruction *inst,
                              struct Operand *dst, nint *size) {
    if(dst->kind == OPERAND_REG8) {
        *size = 1;
        return 1;
    }
    if(dst->kind == OPERAND_REG16) {
        *size = 2;
        return 1;
    }
    if(dst->kind == OPERAND_REG32) {
        *size = 4;
        return 1;
    }
    if(dst->kind == OPERAND_MEM) {
        if(dst->size == 1 || dst->size == 2 || dst->size == 4) {
            *size = dst->size;
            return 1;
        }
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "ambiguous shift operand size");
        return INSTRUCTION_FAILED;
    }
    return 0;
}

static nint emit_shift_modrm(struct Parser *p, struct Operand *dst, nint group) {
    if(dst->kind == OPERAND_MEM) {
        emit_modrm_mem(p, group, &dst->mem);
        return memory_tail_len(&dst->mem);
    }
    emit_modrm_reg(p, group, dst->reg);
    return 0;
}

static nint encode_shift_group(struct Parser *p, struct AstInstruction *inst,
                               struct Operand *dst, struct Operand *src,
                               nint group) {
    nint size;
    nint tail;
    unsigned char opcode;

    nint status = shift_target_size(p, inst, dst, &size);
    if(status <= 0) {
        if(status == INSTRUCTION_FAILED) return INSTRUCTION_FAILED;
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported shift form");
        return INSTRUCTION_FAILED;
    }

    if(src->kind == OPERAND_IMM) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "shift count does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }

        if(src->imm == 1) {
            opcode = size == 1 ? 0xd0 : 0xd1;
            emit_operand_prefixes(p, dst, size);
            emit_byte(p, opcode);
            tail = emit_shift_modrm(p, dst, group);
            return operand_prefix_len(dst, size) + 2 + tail;
        }

        opcode = size == 1 ? 0xc0 : 0xc1;
        emit_operand_prefixes(p, dst, size);
        emit_byte(p, opcode);
        tail = emit_shift_modrm(p, dst, group);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return operand_prefix_len(dst, size) + 2 + tail + 1;
    }

    if(is_cl(src)) {
        opcode = size == 1 ? 0xd2 : 0xd3;
        emit_operand_prefixes(p, dst, size);
        emit_byte(p, opcode);
        tail = emit_shift_modrm(p, dst, group);
        return operand_prefix_len(dst, size) + 2 + tail;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported shift form");
    return INSTRUCTION_FAILED;
}

static void emit_forced_prefix_pair(struct Parser *p, unint operand_prefix,
                                    unint address_prefix);
static nint forced_prefix_pair_len(unint operand_prefix,
                                   unint address_prefix);

static nint encode_in(struct Parser *p, struct AstInstruction *inst,
                      struct Operand *dst, struct Operand *src) {
    if(is_al(dst) && src->kind == OPERAND_IMM) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "port does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_byte(p, 0xe4);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return 2;
    }

    if(is_ax(dst) && src->kind == OPERAND_IMM) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "port does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_byte(p, 0xe5);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return 2;
    }

    if(is_eax(dst) && src->kind == OPERAND_IMM) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "port does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0xe5);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return 3;
    }

    if(is_al(dst) && is_dx(src)) {
        emit_byte(p, 0xec);
        return 1;
    }

    if(is_ax(dst) && is_dx(src)) {
        emit_byte(p, 0xed);
        return 1;
    }

    if(is_eax(dst) && is_dx(src)) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0xed);
        return 2;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported in form");
    return INSTRUCTION_FAILED;
}

static nint encode_in_prefixed_operands(struct Parser *p,
                                        struct AstInstruction *inst,
                                        struct Operand *dst,
                                        struct Operand *src,
                                        unint force_o32,
                                        unint force_a32,
                                        unint force_o16) {
    unint op_prefix = force_o32;
    nint prefix_len;

    if(is_eax(dst) && !force_o16) op_prefix = 1;

    if(is_al(dst) && src->kind == OPERAND_IMM) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "port does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_forced_prefix_pair(p, op_prefix, force_a32);
        emit_byte(p, 0xe4);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return forced_prefix_pair_len(op_prefix, force_a32) + 2;
    }

    if((is_ax(dst) || is_eax(dst)) && src->kind == OPERAND_IMM) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "port does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_forced_prefix_pair(p, op_prefix, force_a32);
        emit_byte(p, 0xe5);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return forced_prefix_pair_len(op_prefix, force_a32) + 2;
    }

    if(is_al(dst) && is_dx(src)) {
        emit_forced_prefix_pair(p, op_prefix, force_a32);
        emit_byte(p, 0xec);
        return forced_prefix_pair_len(op_prefix, force_a32) + 1;
    }

    if((is_ax(dst) || is_eax(dst)) && is_dx(src)) {
        prefix_len = forced_prefix_pair_len(op_prefix, force_a32);
        emit_forced_prefix_pair(p, op_prefix, force_a32);
        emit_byte(p, 0xed);
        return prefix_len + 1;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported in form");
    return INSTRUCTION_FAILED;
}

static nint encode_prefixed_in(struct Parser *p,
                               struct AstInstruction *inner,
                               unint force_o32,
                               unint force_a32,
                               unint force_o16) {
    struct Operand dst;
    struct Operand src;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }

    error_token = NULL;
    status = parse_operand(p, inner->args_tail, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    return encode_in_prefixed_operands(p, inner, &dst, &src,
                                       force_o32, force_a32, force_o16);
}

static nint encode_out(struct Parser *p, struct AstInstruction *inst,
                       struct Operand *dst, struct Operand *src) {
    if(dst->kind == OPERAND_IMM && is_al(src)) {
        if(!fits_u16(dst->imm)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_OVERFLOW,
                              "port does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_byte(p, 0xe6);
        emit_byte(p, (unsigned char)(dst->imm & 0xff));
        return 2;
    }

    if(dst->kind == OPERAND_IMM && is_ax(src)) {
        if(!fits_u16(dst->imm)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_OVERFLOW,
                              "port does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_byte(p, 0xe7);
        emit_byte(p, (unsigned char)(dst->imm & 0xff));
        return 2;
    }

    if(dst->kind == OPERAND_IMM && is_eax(src)) {
        if(!fits_u16(dst->imm)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_OVERFLOW,
                              "port does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0xe7);
        emit_byte(p, (unsigned char)(dst->imm & 0xff));
        return 3;
    }

    if(is_dx(dst) && is_al(src)) {
        emit_byte(p, 0xee);
        return 1;
    }

    if(is_dx(dst) && is_ax(src)) {
        emit_byte(p, 0xef);
        return 1;
    }

    if(is_dx(dst) && is_eax(src)) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0xef);
        return 2;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported out form");
    return INSTRUCTION_FAILED;
}

static nint encode_out_prefixed_operands(struct Parser *p,
                                         struct AstInstruction *inst,
                                         struct Operand *dst,
                                         struct Operand *src,
                                         unint force_o32,
                                         unint force_a32,
                                         unint force_o16) {
    unint op_prefix = force_o32;
    nint prefix_len;

    if(is_eax(src) && !force_o16) op_prefix = 1;

    if(dst->kind == OPERAND_IMM && is_al(src)) {
        if(!fits_u16(dst->imm)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_OVERFLOW,
                              "port does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_forced_prefix_pair(p, op_prefix, force_a32);
        emit_byte(p, 0xe6);
        emit_byte(p, (unsigned char)(dst->imm & 0xff));
        return forced_prefix_pair_len(op_prefix, force_a32) + 2;
    }

    if(dst->kind == OPERAND_IMM && (is_ax(src) || is_eax(src))) {
        if(!fits_u16(dst->imm)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_OVERFLOW,
                              "port does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_forced_prefix_pair(p, op_prefix, force_a32);
        emit_byte(p, 0xe7);
        emit_byte(p, (unsigned char)(dst->imm & 0xff));
        return forced_prefix_pair_len(op_prefix, force_a32) + 2;
    }

    if(is_dx(dst) && is_al(src)) {
        emit_forced_prefix_pair(p, op_prefix, force_a32);
        emit_byte(p, 0xee);
        return forced_prefix_pair_len(op_prefix, force_a32) + 1;
    }

    if(is_dx(dst) && (is_ax(src) || is_eax(src))) {
        prefix_len = forced_prefix_pair_len(op_prefix, force_a32);
        emit_forced_prefix_pair(p, op_prefix, force_a32);
        emit_byte(p, 0xef);
        return prefix_len + 1;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported out form");
    return INSTRUCTION_FAILED;
}

static nint encode_prefixed_out(struct Parser *p,
                                struct AstInstruction *inner,
                                unint force_o32,
                                unint force_a32,
                                unint force_o16) {
    struct Operand dst;
    struct Operand src;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }

    error_token = NULL;
    status = parse_operand(p, inner->args_tail, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    return encode_out_prefixed_operands(p, inner, &dst, &src,
                                        force_o32, force_a32, force_o16);
}

static nint encode_intr(struct Parser *p, struct AstInstruction *inst,
                        struct Operand *dst) {
    if(dst->kind != OPERAND_IMM) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "invalid interrupt number");
        return INSTRUCTION_FAILED;
    }

    if(!fits_u8(dst->imm)) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_OVERFLOW,
                          "interrupt number does not fit 8 bits");
        return INSTRUCTION_FAILED;
    }

    emit_byte(p, 0xcd);
    emit_byte(p, (unsigned char)(dst->imm & 0xff));
    return 2;
}

static nint emit_arith_reg_imm(struct Parser *p, struct AstInstruction *inst,
                               struct Operand *dst, struct Operand *src,
                               nint group, unsigned char acc8,
                               unsigned char acc16) {
    if(dst->kind == OPERAND_REG8) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        if(is_al(dst)) {
            emit_byte(p, acc8);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 2;
        }
        emit_byte(p, 0x80);
        emit_modrm_reg(p, group, dst->reg);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return 3;
    }

    if(dst->kind == OPERAND_REG16) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        if(fits_word_sign_extended_i8(src->imm)) {
            emit_byte(p, 0x83);
            emit_modrm_reg(p, group, dst->reg);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 3;
        }
        if(is_ax(dst)) {
            emit_byte(p, acc16);
            emit_word(p, src->imm);
            return 3;
        }
        emit_byte(p, 0x81);
        emit_modrm_reg(p, group, dst->reg);
        emit_word(p, src->imm);
        return 4;
    }

    if(dst->kind == OPERAND_REG32) {
        if(!fits_u32(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 32 bits");
            return INSTRUCTION_FAILED;
        }
        emit_operand_size_prefix(p, 4);
        if(fits_dword_sign_extended_i8(src->imm)) {
            emit_byte(p, 0x83);
            emit_modrm_reg(p, group, dst->reg);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 4;
        }
        if(is_eax(dst)) {
            emit_byte(p, acc16);
            emit_dword(p, src->imm);
            return 6;
        }
        emit_byte(p, 0x81);
        emit_modrm_reg(p, group, dst->reg);
        emit_dword(p, src->imm);
        return 7;
    }

    return 0;
}

static nint emit_arith_mem_imm(struct Parser *p, struct AstInstruction *inst,
                               struct Operand *dst, struct Operand *src,
                               nint group) {
    if(dst->size == 1) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        emit_byte(p, 0x80);
        emit_modrm_mem(p, group, &dst->mem);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 1;
    }

    if(dst->size == 2) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        if(fits_word_sign_extended_i8(src->imm)) {
            emit_byte(p, 0x83);
            emit_modrm_mem(p, group, &dst->mem);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 1;
        }
        emit_byte(p, 0x81);
        emit_modrm_mem(p, group, &dst->mem);
        emit_word(p, src->imm);
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem) + 2;
    }

    if(dst->size == 4) {
        if(!fits_u32(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 32 bits");
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &dst->mem, 4);
        if(fits_dword_sign_extended_i8(src->imm)) {
            emit_byte(p, 0x83);
            emit_modrm_mem(p, group, &dst->mem);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return memory_operand_prefix_len(&dst->mem, 4) + 2 +
                   memory_tail_len(&dst->mem) + 1;
        }
        emit_byte(p, 0x81);
        emit_modrm_mem(p, group, &dst->mem);
        emit_dword(p, src->imm);
        return memory_operand_prefix_len(&dst->mem, 4) + 2 +
               memory_tail_len(&dst->mem) + 4;
    }

    _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                      "ambiguous memory immediate size");
    return INSTRUCTION_FAILED;
}

static nint encode_arith(struct Parser *p, struct AstInstruction *inst,
                         struct Operand *dst, struct Operand *src,
                         unsigned char base, unsigned char acc8,
                         unsigned char acc16, nint group,
                         const char *name) {
    nint status;

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_REG8) {
        emit_byte(p, base);
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_REG16) {
        emit_byte(p, (unsigned char)(base + 1));
        emit_modrm_reg(p, src->reg, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG32 && src->kind == OPERAND_REG32) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, (unsigned char)(base + 1));
        emit_modrm_reg(p, src->reg, dst->reg);
        return 3;
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG8) {
        if(!memory_size_matches(dst, 1)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid %s operand size", name);
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        emit_byte(p, base);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG16) {
        if(!memory_size_matches(dst, 2)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid %s operand size", name);
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &dst->mem);
        emit_byte(p, (unsigned char)(base + 1));
        emit_modrm_mem(p, src->reg, &dst->mem);
        return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_MEM && src->kind == OPERAND_REG32) {
        if(!memory_size_matches(dst, 4)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid %s operand size", name);
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &dst->mem, 4);
        emit_byte(p, (unsigned char)(base + 1));
        emit_modrm_mem(p, src->reg, &dst->mem);
        return memory_operand_prefix_len(&dst->mem, 4) + 2 +
               memory_tail_len(&dst->mem);
    }

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_MEM) {
        if(!memory_size_matches(src, 1)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid %s operand size", name);
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &src->mem);
        emit_byte(p, (unsigned char)(base + 2));
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_MEM) {
        if(!memory_size_matches(src, 2)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid %s operand size", name);
            return INSTRUCTION_FAILED;
        }
        emit_memory_prefixes(p, &src->mem);
        emit_byte(p, (unsigned char)(base + 3));
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_prefix_len(&src->mem) + 2 + memory_tail_len(&src->mem);
    }

    if(dst->kind == OPERAND_REG32 && src->kind == OPERAND_MEM) {
        if(!memory_size_matches(src, 4)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid %s operand size", name);
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &src->mem, 4);
        emit_byte(p, (unsigned char)(base + 3));
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_operand_prefix_len(&src->mem, 4) + 2 +
               memory_tail_len(&src->mem);
    }

    if(src->kind == OPERAND_IMM) {
        status = emit_arith_reg_imm(p, inst, dst, src, group, acc8, acc16);
        if(status) return status;
        if(dst->kind == OPERAND_MEM) {
            return emit_arith_mem_imm(p, inst, dst, src, group);
        }
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported %s form", name);
    return INSTRUCTION_FAILED;
}

static nint encode_not(struct Parser *p, struct AstInstruction *inst,
                       struct Operand *dst) {
    if(dst->kind == OPERAND_REG8) {
        emit_byte(p, 0xf6);
        emit_modrm_reg(p, 2, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG16) {
        emit_byte(p, 0xf7);
        emit_modrm_reg(p, 2, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG32) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0xf7);
        emit_modrm_reg(p, 2, dst->reg);
        return 3;
    }

    if(dst->kind == OPERAND_MEM) {
        if(dst->size == 1) {
            emit_memory_prefixes(p, &dst->mem);
            emit_byte(p, 0xf6);
            emit_modrm_mem(p, 2, &dst->mem);
            return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
        }
        if(dst->size == 2) {
            emit_memory_prefixes(p, &dst->mem);
            emit_byte(p, 0xf7);
            emit_modrm_mem(p, 2, &dst->mem);
            return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
        }
        if(dst->size == 4) {
            emit_memory_operand_prefixes(p, &dst->mem, 4);
            emit_byte(p, 0xf7);
            emit_modrm_mem(p, 2, &dst->mem);
            return memory_operand_prefix_len(&dst->mem, 4) + 2 +
                   memory_tail_len(&dst->mem);
        }
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "ambiguous not operand size");
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported not form");
    return INSTRUCTION_FAILED;
}

static nint encode_unary_group(struct Parser *p, struct AstInstruction *inst,
                               struct Operand *dst, nint group,
                               const char *name) {
    if(dst->kind == OPERAND_REG8) {
        emit_byte(p, 0xf6);
        emit_modrm_reg(p, group, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG16) {
        emit_byte(p, 0xf7);
        emit_modrm_reg(p, group, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG32) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0xf7);
        emit_modrm_reg(p, group, dst->reg);
        return 3;
    }

    if(dst->kind == OPERAND_MEM) {
        if(dst->size == 1) {
            emit_memory_prefixes(p, &dst->mem);
            emit_byte(p, 0xf6);
            emit_modrm_mem(p, group, &dst->mem);
            return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
        }
        if(dst->size == 2) {
            emit_memory_prefixes(p, &dst->mem);
            emit_byte(p, 0xf7);
            emit_modrm_mem(p, group, &dst->mem);
            return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
        }
        if(dst->size == 4) {
            emit_memory_operand_prefixes(p, &dst->mem, 4);
            emit_byte(p, 0xf7);
            emit_modrm_mem(p, group, &dst->mem);
            return memory_operand_prefix_len(&dst->mem, 4) + 2 +
                   memory_tail_len(&dst->mem);
        }
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "ambiguous %s operand size", name);
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported %s form", name);
    return INSTRUCTION_FAILED;
}

static unint operand_is_mem16(struct Operand *op);
static unint operand_is_mem32(struct Operand *op);
static void force_direct_memory_a32(struct Operand *op);
static void emit_forced_o32_memory_prefixes(struct Parser *p,
                                            struct MemoryOperand *mem);
static nint forced_o32_memory_prefix_len(struct MemoryOperand *mem);

static nint encode_prefixed_unary_group(struct Parser *p,
                                        struct AstInstruction *inner,
                                        nint group,
                                        const char *name,
                                        unint force_o32,
                                        unint force_a16,
                                        unint force_a32) {
    struct Operand dst;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count != 1) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid operand");
        return INSTRUCTION_FAILED;
    }

    if(force_a32) force_direct_memory_a32(&dst);
    if(force_a16 && operand_is_mem32(&dst)) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "invalid a16 address form");
        return INSTRUCTION_FAILED;
    }
    if(force_a32 && operand_is_mem16(&dst)) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "invalid a32 address form");
        return INSTRUCTION_FAILED;
    }

    if(force_a32 && dst.kind != OPERAND_MEM) {
        if(force_o32 || dst.kind == OPERAND_REG32) emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x67);
        if(dst.kind == OPERAND_REG8) {
            emit_byte(p, 0xf6);
            emit_modrm_reg(p, group, dst.reg);
            return 3 + (force_o32 ? 1 : 0);
        }
        if(dst.kind == OPERAND_REG16 || dst.kind == OPERAND_REG32) {
            emit_byte(p, 0xf7);
            emit_modrm_reg(p, group, dst.reg);
            return 3 + ((force_o32 || dst.kind == OPERAND_REG32) ? 1 : 0);
        }
    }

    if(!force_o32) return encode_unary_group(p, inner, &dst, group, name);

    if(dst.kind == OPERAND_REG32 ||
       (dst.kind == OPERAND_MEM && dst.size == 4)) {
        return encode_unary_group(p, inner, &dst, group, name);
    }

    if(dst.kind == OPERAND_REG8) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0xf6);
        emit_modrm_reg(p, group, dst.reg);
        return 3;
    }

    if(dst.kind == OPERAND_REG16) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0xf7);
        emit_modrm_reg(p, group, dst.reg);
        return 3;
    }

    if(dst.kind == OPERAND_MEM) {
        if(dst.size == 1) {
            emit_forced_o32_memory_prefixes(p, &dst.mem);
            emit_byte(p, 0xf6);
            emit_modrm_mem(p, group, &dst.mem);
            return forced_o32_memory_prefix_len(&dst.mem) + 2 +
                   memory_tail_len(&dst.mem);
        }
        if(dst.size == 2) {
            emit_forced_o32_memory_prefixes(p, &dst.mem);
            emit_byte(p, 0xf7);
            emit_modrm_mem(p, group, &dst.mem);
            return forced_o32_memory_prefix_len(&dst.mem) + 2 +
                   memory_tail_len(&dst.mem);
        }
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "ambiguous %s operand size", name);
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inner->name, ERROR_TYPE_I386,
                      "unsupported %s form", name);
    return INSTRUCTION_FAILED;
}

static nint emit_imul_reg_rm(struct Parser *p, struct AstInstruction *inst,
                             struct Operand *dst, struct Operand *src) {
    if(dst->kind == OPERAND_REG16) {
        if(src->kind == OPERAND_REG16) {
            emit_byte(p, 0x0f);
            emit_byte(p, 0xaf);
            emit_modrm_reg(p, dst->reg, src->reg);
            return 3;
        }
        if(src->kind == OPERAND_MEM) {
            if(!memory_size_matches(src, 2)) {
                _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                                  "invalid imul operand size");
                return INSTRUCTION_FAILED;
            }
            emit_memory_prefixes(p, &src->mem);
            emit_byte(p, 0x0f);
            emit_byte(p, 0xaf);
            emit_modrm_mem(p, dst->reg, &src->mem);
            return memory_prefix_len(&src->mem) + 3 +
                   memory_tail_len(&src->mem);
        }
    }

    if(dst->kind == OPERAND_REG32) {
        if(src->kind == OPERAND_REG32) {
            emit_operand_size_prefix(p, 4);
            emit_byte(p, 0x0f);
            emit_byte(p, 0xaf);
            emit_modrm_reg(p, dst->reg, src->reg);
            return 4;
        }
        if(src->kind == OPERAND_MEM) {
            if(!memory_size_matches(src, 4)) {
                _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                                  "invalid imul operand size");
                return INSTRUCTION_FAILED;
            }
            emit_memory_operand_prefixes(p, &src->mem, 4);
            emit_byte(p, 0x0f);
            emit_byte(p, 0xaf);
            emit_modrm_mem(p, dst->reg, &src->mem);
            return memory_operand_prefix_len(&src->mem, 4) + 3 +
                   memory_tail_len(&src->mem);
        }
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported imul form");
    return INSTRUCTION_FAILED;
}

static nint emit_imul_imm_tail(struct Parser *p, struct AstInstruction *inst,
                               struct Operand *dst, struct Operand *src,
                               struct Operand *imm) {
    if(imm->kind != OPERAND_IMM) {
        _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                          "invalid imul immediate");
        return INSTRUCTION_FAILED;
    }

    if(dst->kind == OPERAND_REG16) {
        if(src->kind != OPERAND_REG16 && src->kind != OPERAND_MEM) {
            _error_from_token(p, inst->args_head->next->_s, ERROR_TYPE_I386,
                              "invalid imul source");
            return INSTRUCTION_FAILED;
        }
        if(src->kind == OPERAND_MEM && !memory_size_matches(src, 2)) {
            _error_from_token(p, inst->args_head->next->_s, ERROR_TYPE_I386,
                              "invalid imul operand size");
            return INSTRUCTION_FAILED;
        }
        if(!fits_u16(imm->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        if(src->kind == OPERAND_MEM) emit_memory_prefixes(p, &src->mem);
        if(fits_word_sign_extended_i8(imm->imm)) {
            emit_byte(p, 0x6b);
            if(src->kind == OPERAND_MEM) emit_modrm_mem(p, dst->reg, &src->mem);
            else emit_modrm_reg(p, dst->reg, src->reg);
            emit_byte(p, (unsigned char)(imm->imm & 0xff));
            return (src->kind == OPERAND_MEM ? memory_prefix_len(&src->mem) +
                   memory_tail_len(&src->mem) : 0) + 3;
        }
        emit_byte(p, 0x69);
        if(src->kind == OPERAND_MEM) emit_modrm_mem(p, dst->reg, &src->mem);
        else emit_modrm_reg(p, dst->reg, src->reg);
        emit_word(p, imm->imm);
        return (src->kind == OPERAND_MEM ? memory_prefix_len(&src->mem) +
               memory_tail_len(&src->mem) : 0) + 4;
    }

    if(dst->kind == OPERAND_REG32) {
        if(src->kind != OPERAND_REG32 && src->kind != OPERAND_MEM) {
            _error_from_token(p, inst->args_head->next->_s, ERROR_TYPE_I386,
                              "invalid imul source");
            return INSTRUCTION_FAILED;
        }
        if(src->kind == OPERAND_MEM && !memory_size_matches(src, 4)) {
            _error_from_token(p, inst->args_head->next->_s, ERROR_TYPE_I386,
                              "invalid imul operand size");
            return INSTRUCTION_FAILED;
        }
        if(!fits_u32(imm->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 32 bits");
            return INSTRUCTION_FAILED;
        }
        if(src->kind == OPERAND_MEM) emit_memory_operand_prefixes(p, &src->mem, 4);
        else emit_operand_size_prefix(p, 4);
        if(fits_dword_sign_extended_i8(imm->imm)) {
            emit_byte(p, 0x6b);
            if(src->kind == OPERAND_MEM) emit_modrm_mem(p, dst->reg, &src->mem);
            else emit_modrm_reg(p, dst->reg, src->reg);
            emit_byte(p, (unsigned char)(imm->imm & 0xff));
            return (src->kind == OPERAND_MEM ? memory_operand_prefix_len(&src->mem, 4) +
                   memory_tail_len(&src->mem) : 1) + 3;
        }
        emit_byte(p, 0x69);
        if(src->kind == OPERAND_MEM) emit_modrm_mem(p, dst->reg, &src->mem);
        else emit_modrm_reg(p, dst->reg, src->reg);
        emit_dword(p, imm->imm);
        return (src->kind == OPERAND_MEM ? memory_operand_prefix_len(&src->mem, 4) +
               memory_tail_len(&src->mem) : 1) + 6;
    }

    _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                      "invalid imul destination");
    return INSTRUCTION_FAILED;
}

static nint encode_imul_multi(struct Parser *p, struct AstInstruction *inst) {
    struct Operand dst;
    struct Operand src;
    struct Operand imm;
    struct token *error_token = NULL;
    nint status;

    if(inst->arg_count == 1) {
        status = parse_operand(p, inst->args_head, &dst, &error_token);
        if(status <= 0) {
            _error_from_token(p, error_token, ERROR_TYPE_I386,
                              "invalid first operand");
            return INSTRUCTION_FAILED;
        }
        return encode_unary_group(p, inst, &dst, 5, "imul");
    }

    status = parse_operand(p, inst->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }

    error_token = NULL;
    status = parse_operand(p, inst->args_head->next, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    if(inst->arg_count == 2) {
        if(src.kind == OPERAND_IMM) return emit_imul_imm_tail(p, inst, &dst, &dst, &src);
        return emit_imul_reg_rm(p, inst, &dst, &src);
    }

    error_token = NULL;
    status = parse_operand(p, inst->args_tail, &imm, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid third operand");
        return INSTRUCTION_FAILED;
    }

    return emit_imul_imm_tail(p, inst, &dst, &src, &imm);
}

static nint encode_o32_unary_group(struct Parser *p, struct AstInstruction *inner,
                                   nint group, const char *name);
static void emit_forced_o32_memory_prefixes(struct Parser *p,
                                            struct MemoryOperand *mem);
static nint forced_o32_memory_prefix_len(struct MemoryOperand *mem);

static nint emit_o32_imul_reg_rm(struct Parser *p, struct AstInstruction *inst,
                                 struct Operand *dst, struct Operand *src) {
    if(dst->kind == OPERAND_REG32 ||
       (src->kind == OPERAND_MEM && src->size == 4)) {
        return emit_imul_reg_rm(p, inst, dst, src);
    }

    if(dst->kind == OPERAND_REG16) {
        if(src->kind == OPERAND_REG16) {
            emit_operand_size_prefix(p, 4);
            emit_byte(p, 0x0f);
            emit_byte(p, 0xaf);
            emit_modrm_reg(p, dst->reg, src->reg);
            return 4;
        }
        if(src->kind == OPERAND_MEM) {
            if(!memory_size_matches(src, 2)) {
                _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                                  "invalid imul operand size");
                return INSTRUCTION_FAILED;
            }
            emit_forced_o32_memory_prefixes(p, &src->mem);
            emit_byte(p, 0x0f);
            emit_byte(p, 0xaf);
            emit_modrm_mem(p, dst->reg, &src->mem);
            return forced_o32_memory_prefix_len(&src->mem) + 3 +
                   memory_tail_len(&src->mem);
        }
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported imul form");
    return INSTRUCTION_FAILED;
}

static nint emit_o32_imul_imm_tail(struct Parser *p, struct AstInstruction *inst,
                                   struct Operand *dst, struct Operand *src,
                                   struct Operand *imm) {
    if(dst->kind == OPERAND_REG32 ||
       (src->kind == OPERAND_MEM && src->size == 4)) {
        return emit_imul_imm_tail(p, inst, dst, src, imm);
    }

    if(imm->kind != OPERAND_IMM) {
        _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                          "invalid imul immediate");
        return INSTRUCTION_FAILED;
    }

    if(dst->kind != OPERAND_REG16 ||
       (src->kind != OPERAND_REG16 && src->kind != OPERAND_MEM)) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported imul form");
        return INSTRUCTION_FAILED;
    }

    if(src->kind == OPERAND_MEM && !memory_size_matches(src, 2)) {
        _error_from_token(p, inst->args_head->next->_s, ERROR_TYPE_I386,
                          "invalid imul operand size");
        return INSTRUCTION_FAILED;
    }

    if(!fits_u16(imm->imm)) {
        _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                          "immediate does not fit 16 bits");
        return INSTRUCTION_FAILED;
    }

    if(src->kind == OPERAND_MEM) emit_forced_o32_memory_prefixes(p, &src->mem);
    else emit_operand_size_prefix(p, 4);

    if(fits_word_sign_extended_i8(imm->imm)) {
        emit_byte(p, 0x6b);
        if(src->kind == OPERAND_MEM) emit_modrm_mem(p, dst->reg, &src->mem);
        else emit_modrm_reg(p, dst->reg, src->reg);
        emit_byte(p, (unsigned char)(imm->imm & 0xff));
        return (src->kind == OPERAND_MEM ? forced_o32_memory_prefix_len(&src->mem) +
               memory_tail_len(&src->mem) : 1) + 3;
    }

    emit_byte(p, 0x69);
    if(src->kind == OPERAND_MEM) emit_modrm_mem(p, dst->reg, &src->mem);
    else emit_modrm_reg(p, dst->reg, src->reg);
    emit_word(p, imm->imm);
    return (src->kind == OPERAND_MEM ? forced_o32_memory_prefix_len(&src->mem) +
           memory_tail_len(&src->mem) : 1) + 4;
}

static nint encode_o32_imul(struct Parser *p, struct AstInstruction *inner) {
    struct Operand dst;
    struct Operand src;
    struct Operand imm;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count < 1 || inner->arg_count > 3) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if(inner->arg_count == 1) return encode_o32_unary_group(p, inner, 5, "imul");

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }
    error_token = NULL;
    status = parse_operand(p, inner->args_head->next, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    if(inner->arg_count == 2) {
        if(src.kind == OPERAND_IMM) return emit_o32_imul_imm_tail(p, inner, &dst, &dst, &src);
        return emit_o32_imul_reg_rm(p, inner, &dst, &src);
    }

    error_token = NULL;
    status = parse_operand(p, inner->args_tail, &imm, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid third operand");
        return INSTRUCTION_FAILED;
    }

    return emit_o32_imul_imm_tail(p, inner, &dst, &src, &imm);
}

static nint encode_prefixed_imul(struct Parser *p,
                                 struct AstInstruction *inner,
                                 unint force_o32,
                                 unint force_a16,
                                 unint force_a32) {
    struct Operand dst;
    struct Operand src;
    struct Operand imm;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count < 1 || inner->arg_count > 3) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if(inner->arg_count == 1) {
        status = parse_operand(p, inner->args_head, &dst, &error_token);
        if(status <= 0) {
            _error_from_token(p, error_token, ERROR_TYPE_I386,
                              "invalid first operand");
            return INSTRUCTION_FAILED;
        }
        if(force_a32) force_direct_memory_a32(&dst);
        if(force_a16 && operand_is_mem32(&dst)) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "invalid a16 address form");
            return INSTRUCTION_FAILED;
        }
        if(force_a32 && operand_is_mem16(&dst)) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "invalid a32 address form");
            return INSTRUCTION_FAILED;
        }
        if(force_o32 && force_a32 && dst.kind != OPERAND_MEM) {
            emit_operand_size_prefix(p, 4);
            emit_byte(p, 0x67);
            if(dst.kind == OPERAND_REG8) {
                emit_byte(p, 0xf6);
                emit_modrm_reg(p, 5, dst.reg);
                return 4;
            }
            if(dst.kind == OPERAND_REG16 || dst.kind == OPERAND_REG32) {
                emit_byte(p, 0xf7);
                emit_modrm_reg(p, 5, dst.reg);
                return 4;
            }
        }
        if(force_a32 && dst.kind != OPERAND_MEM) {
            emit_byte(p, 0x67);
            status = encode_prefixed_unary_group(p, inner, 5, "imul",
                                                 force_o32, force_a16, 0);
            return status < 0 ? status : status + 1;
        }
        return encode_prefixed_unary_group(p, inner, 5, "imul",
                                           force_o32, force_a16, force_a32);
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }

    error_token = NULL;
    status = parse_operand(p, inner->args_head->next, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    if(force_a32) force_direct_memory_a32(&src);
    if(force_a16 && operand_is_mem32(&src)) {
        _error_from_token(p, inner->args_head->next->_s, ERROR_TYPE_I386,
                          "invalid a16 address form");
        return INSTRUCTION_FAILED;
    }
    if(force_a32 && operand_is_mem16(&src)) {
        _error_from_token(p, inner->args_head->next->_s, ERROR_TYPE_I386,
                          "invalid a32 address form");
        return INSTRUCTION_FAILED;
    }

    if(inner->arg_count == 2) {
        if(force_o32 && force_a32 && src.kind != OPERAND_MEM) {
            emit_operand_size_prefix(p, 4);
            emit_byte(p, 0x67);
            if(src.kind == OPERAND_IMM) {
                status = emit_imul_imm_tail(p, inner, &dst, &dst, &src);
                return status < 0 ? status : status + 2;
            }
            if(dst.kind == OPERAND_REG16 && src.kind == OPERAND_REG16) {
                emit_byte(p, 0x0f);
                emit_byte(p, 0xaf);
                emit_modrm_reg(p, dst.reg, src.reg);
                return 5;
            }
            if(dst.kind == OPERAND_REG32 && src.kind == OPERAND_REG32) {
                emit_byte(p, 0x0f);
                emit_byte(p, 0xaf);
                emit_modrm_reg(p, dst.reg, src.reg);
                return 5;
            }
        }
        if(force_a32 && src.kind != OPERAND_MEM) {
            emit_byte(p, 0x67);
            status = src.kind == OPERAND_IMM ?
                (force_o32 ? emit_o32_imul_imm_tail(p, inner, &dst, &dst, &src) :
                             emit_imul_imm_tail(p, inner, &dst, &dst, &src)) :
                (force_o32 ? emit_o32_imul_reg_rm(p, inner, &dst, &src) :
                             emit_imul_reg_rm(p, inner, &dst, &src));
            return status < 0 ? status : status + 1;
        }
        if(src.kind == OPERAND_IMM) {
            return force_o32 ?
                emit_o32_imul_imm_tail(p, inner, &dst, &dst, &src) :
                emit_imul_imm_tail(p, inner, &dst, &dst, &src);
        }
        return force_o32 ?
            emit_o32_imul_reg_rm(p, inner, &dst, &src) :
            emit_imul_reg_rm(p, inner, &dst, &src);
    }

    error_token = NULL;
    status = parse_operand(p, inner->args_tail, &imm, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid third operand");
        return INSTRUCTION_FAILED;
    }

    if(force_o32 && force_a32 && src.kind != OPERAND_MEM) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x67);
        status = emit_imul_imm_tail(p, inner, &dst, &src, &imm);
        return status < 0 ? status : status + 2;
    }

    if(force_a32 && src.kind != OPERAND_MEM) {
        emit_byte(p, 0x67);
        status = force_o32 ?
            emit_o32_imul_imm_tail(p, inner, &dst, &src, &imm) :
            emit_imul_imm_tail(p, inner, &dst, &src, &imm);
        return status < 0 ? status : status + 1;
    }

    return force_o32 ?
        emit_o32_imul_imm_tail(p, inner, &dst, &src, &imm) :
        emit_imul_imm_tail(p, inner, &dst, &src, &imm);
}

static nint encode_aad(struct Parser *p, struct AstInstruction *inst,
                       struct Operand *src) {
    if(src->kind == OPERAND_IMM) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_byte(p, 0xd5);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return 2;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported aad form");
    return INSTRUCTION_FAILED;
}

static nint encode_aam(struct Parser *p, struct AstInstruction *inst,
                       struct Operand *src) {
    if(src->kind == OPERAND_IMM) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_byte(p, 0xd4);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return 2;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported aam form");
    return INSTRUCTION_FAILED;
}

static nint encode_inc_dec(struct Parser *p, struct AstInstruction *inst,
                           struct Operand *dst, nint group,
                           unsigned char reg16_base, const char *name) {
    if(dst->kind == OPERAND_REG8) {
        emit_byte(p, 0xfe);
        emit_modrm_reg(p, group, dst->reg);
        return 2;
    }

    if(dst->kind == OPERAND_REG16) {
        emit_byte(p, (unsigned char)(reg16_base + dst->reg));
        return 1;
    }

    if(dst->kind == OPERAND_REG32) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, (unsigned char)(reg16_base + dst->reg));
        return 2;
    }

    if(dst->kind == OPERAND_MEM) {
        if(dst->size == 1) {
            emit_memory_prefixes(p, &dst->mem);
            emit_byte(p, 0xfe);
            emit_modrm_mem(p, group, &dst->mem);
            return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
        }
        if(dst->size == 2) {
            emit_memory_prefixes(p, &dst->mem);
            emit_byte(p, 0xff);
            emit_modrm_mem(p, group, &dst->mem);
            return memory_prefix_len(&dst->mem) + 2 + memory_tail_len(&dst->mem);
        }
        if(dst->size == 4) {
            emit_memory_operand_prefixes(p, &dst->mem, 4);
            emit_byte(p, 0xff);
            emit_modrm_mem(p, group, &dst->mem);
            return memory_operand_prefix_len(&dst->mem, 4) + 2 +
                   memory_tail_len(&dst->mem);
        }
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "ambiguous %s operand size", name);
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported %s form", name);
    return INSTRUCTION_FAILED;
}

static nint emit_prefixed_zero_op_byte(struct Parser *p,
                                       struct AstInstruction *inner,
                                       unsigned char opcode,
                                       unint operand_prefix,
                                       unint address_prefix);
static nint emit_prefixed_movs(struct Parser *p,
                               struct AstInstruction *inner,
                               unint operand_prefix,
                               unint address_prefix,
                               unint force_o16);
static nint emit_prefixed_lods(struct Parser *p,
                               struct AstInstruction *inner,
                               unint operand_prefix,
                               unint address_prefix,
                               unint force_o16);
static nint emit_prefixed_stos(struct Parser *p,
                               struct AstInstruction *inner,
                               unint operand_prefix,
                               unint address_prefix,
                               unint force_o16);
static nint emit_prefixed_scas(struct Parser *p,
                               struct AstInstruction *inner,
                               unint operand_prefix,
                               unint address_prefix,
                               unint force_o16);
static nint encode_bit_scan_prefixed(struct Parser *p,
                                     struct AstInstruction *inner,
                                     unsigned char opcode,
                                     const char *name,
                                     unint force_o32,
                                     unint force_a16,
                                     unint force_a32);
static nint encode_lar_prefixed(struct Parser *p,
                                struct AstInstruction *inner,
                                unint force_o32,
                                unint force_a16,
                                unint force_a32,
                                unint force_o16);
static nint encode_lsl_prefixed(struct Parser *p,
                                struct AstInstruction *inner,
                                unint force_o32,
                                unint force_a16,
                                unint force_a32,
                                unint force_o16);
static unint operand_is_mem16(struct Operand *op);
static unint operand_is_mem32(struct Operand *op);
static void force_direct_memory_a32(struct Operand *op);

static nint emit_prefixed_convert(struct Parser *p, struct AstInstruction *inner,
                                  unint operand_prefix, unint address_prefix,
                                  unint force_o16) {
    unint emit_operand_prefix = operand_prefix;
    unsigned char opcode;

    if(!token_is(inner->name, CWD_INSTRUCTION) &&
       !token_is(inner->name, CWDE_INSTRUCTION)) {
        return 0;
    }

    if(inner->arg_count != 0) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if(token_is(inner->name, CWDE_INSTRUCTION)) {
        opcode = 0x98;
        if(!force_o16) emit_operand_prefix = 1;
    } else {
        opcode = 0x99;
    }

    if(emit_operand_prefix) emit_operand_size_prefix(p, 4);
    if(address_prefix) emit_byte(p, 0x67);
    emit_byte(p, opcode);
    return 1 + (emit_operand_prefix ? 1 : 0) + (address_prefix ? 1 : 0);
}

static nint emit_prefixed_decimal_adjust(struct Parser *p,
                                         struct AstInstruction *inner,
                                         unint operand_prefix,
                                         unint address_prefix) {
    if(token_is(inner->name, DAA_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0x27,
                                          operand_prefix, address_prefix);
    }
    if(token_is(inner->name, DAS_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0x2f,
                                          operand_prefix, address_prefix);
    }
    return 0;
}

static nint emit_prefixed_flag_control(struct Parser *p,
                                       struct AstInstruction *inner,
                                       unint operand_prefix,
                                       unint address_prefix);
static nint emit_prefixed_ins(struct Parser *p,
                              struct AstInstruction *inner,
                              unint operand_prefix,
                              unint address_prefix,
                              unint force_o16);
static nint emit_prefixed_outs(struct Parser *p,
                               struct AstInstruction *inner,
                               unint operand_prefix,
                               unint address_prefix,
                               unint force_o16);
static nint emit_prefixed_interrupt(struct Parser *p,
                                    struct AstInstruction *inner,
                                    unint operand_prefix,
                                    unint address_prefix,
                                    unint force_o16);
static unint jcc_opcodes_from_name(struct token *name,
                                   unsigned char *short_opcode,
                                   unsigned char *near_opcode);
static nint encode_prefixed_jcc(struct Parser *p,
                                struct AstInstruction *inner,
                                unsigned char short_opcode,
                                unsigned char near_opcode,
                                unint force_o32,
                                unint force_a16,
                                unint force_a32);
static nint encode_prefixed_rel8_control(struct Parser *p,
                                         struct AstInstruction *inner,
                                         unsigned char opcode,
                                         unint operand_prefix,
                                         unint address_prefix);
static nint encode_prefixed_jmp(struct Parser *p,
                                struct AstInstruction *inner,
                                unint force_o32,
                                unint force_a16,
                                unint force_a32,
                                unint force_o16);
static nint encode_prefixed_esc(struct Parser *p,
                                struct AstInstruction *inner,
                                unint force_o32,
                                unint force_a16,
                                unint force_a32);
static nint encode_prefixed_pop(struct Parser *p,
                                struct AstInstruction *inner,
                                unint force_o32,
                                unint force_a16,
                                unint force_a32,
                                unint force_o16);
static nint encode_prefixed_push(struct Parser *p,
                                 struct AstInstruction *inner,
                                 unint force_o32,
                                 unint force_a16,
                                 unint force_a32,
                                 unint force_o16);
static nint encode_prefixed_shift_group(struct Parser *p,
                                        struct AstInstruction *inner,
                                        nint group,
                                        unint force_o32,
                                        unint force_a16,
                                        unint force_a32,
                                        unint force_o16);
static nint encode_prefixed_double_shift(struct Parser *p,
                                         struct AstInstruction *inner,
                                         unsigned char imm_opcode,
                                         unsigned char cl_opcode,
                                         const char *name,
                                         unint force_o32,
                                         unint force_a16,
                                         unint force_a32,
                                         unint force_o16);
static nint encode_prefixed_setcc(struct Parser *p,
                                  struct AstInstruction *inner,
                                  unsigned char opcode,
                                  const char *name,
                                  unint force_o32,
                                  unint force_a16,
                                  unint force_a32);
static unint setcc_opcode_from_name(struct token *name,
                                    unsigned char *opcode,
                                    const char **mnemonic);

static nint encode_prefixed_inc_dec(struct Parser *p,
                                    struct AstInstruction *inner,
                                    nint group,
                                    unsigned char reg16_base,
                                    const char *name,
                                    unint force_o32,
                                    unint force_a16,
                                    unint force_a32) {
    struct Operand dst;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count != 1) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid operand");
        return INSTRUCTION_FAILED;
    }

    if(force_a32) force_direct_memory_a32(&dst);
    if(force_a16 && operand_is_mem32(&dst)) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "invalid a16 address form");
        return INSTRUCTION_FAILED;
    }
    if(force_a32 && operand_is_mem16(&dst)) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "invalid a32 address form");
        return INSTRUCTION_FAILED;
    }

    if(force_a32 && dst.kind != OPERAND_MEM) {
        unint op_prefix = force_o32 || dst.kind == OPERAND_REG32;
        if(op_prefix) emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x67);
        if(dst.kind == OPERAND_REG8) {
            emit_byte(p, 0xfe);
            emit_modrm_reg(p, group, dst.reg);
            return 3 + (op_prefix ? 1 : 0);
        }
        if(dst.kind == OPERAND_REG16 || dst.kind == OPERAND_REG32) {
            emit_byte(p, (unsigned char)(reg16_base + dst.reg));
            return 2 + (op_prefix ? 1 : 0);
        }
    }

    if(!force_o32) return encode_inc_dec(p, inner, &dst, group, reg16_base, name);

    if(dst.kind == OPERAND_REG32 ||
       (dst.kind == OPERAND_MEM && dst.size == 4)) {
        return encode_inc_dec(p, inner, &dst, group, reg16_base, name);
    }

    if(dst.kind == OPERAND_REG8) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0xfe);
        emit_modrm_reg(p, group, dst.reg);
        return 3;
    }

    if(dst.kind == OPERAND_REG16) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, (unsigned char)(reg16_base + dst.reg));
        return 2;
    }

    if(dst.kind == OPERAND_MEM) {
        if(dst.size == 1) {
            emit_forced_o32_memory_prefixes(p, &dst.mem);
            emit_byte(p, 0xfe);
            emit_modrm_mem(p, group, &dst.mem);
            return forced_o32_memory_prefix_len(&dst.mem) + 2 +
                   memory_tail_len(&dst.mem);
        }
        if(dst.size == 2) {
            emit_forced_o32_memory_prefixes(p, &dst.mem);
            emit_byte(p, 0xff);
            emit_modrm_mem(p, group, &dst.mem);
            return forced_o32_memory_prefix_len(&dst.mem) + 2 +
                   memory_tail_len(&dst.mem);
        }
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "ambiguous %s operand size", name);
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inner->name, ERROR_TYPE_I386,
                      "unsupported %s form", name);
    return INSTRUCTION_FAILED;
}

static nint encode_o16_inc_dec(struct Parser *p,
                               struct AstInstruction *inner,
                               nint group,
                               unsigned char reg16_base,
                               const char *name,
                               unint force_a16,
                               unint force_a32) {
    struct Operand dst;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count != 1) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid operand");
        return INSTRUCTION_FAILED;
    }

    if(force_a32) force_direct_memory_a32(&dst);
    if(force_a16 && operand_is_mem32(&dst)) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "invalid a16 address form");
        return INSTRUCTION_FAILED;
    }
    if(force_a32 && operand_is_mem16(&dst)) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "invalid a32 address form");
        return INSTRUCTION_FAILED;
    }

    if(force_a32 && dst.kind != OPERAND_MEM) {
        emit_byte(p, 0x67);
        status = encode_o16_inc_dec(p, inner, group, reg16_base, name,
                                    force_a16, 0);
        return status < 0 ? status : status + 1;
    }

    if(dst.kind == OPERAND_REG8) {
        emit_byte(p, 0xfe);
        emit_modrm_reg(p, group, dst.reg);
        return 2;
    }

    if(dst.kind == OPERAND_REG16 || dst.kind == OPERAND_REG32) {
        emit_byte(p, (unsigned char)(reg16_base + dst.reg));
        return 1;
    }

    if(dst.kind == OPERAND_MEM) {
        if(dst.size == 1) {
            emit_memory_prefixes(p, &dst.mem);
            emit_byte(p, 0xfe);
            emit_modrm_mem(p, group, &dst.mem);
            return memory_prefix_len(&dst.mem) + 2 + memory_tail_len(&dst.mem);
        }
        if(dst.size == 2 || dst.size == 4) {
            emit_memory_prefixes(p, &dst.mem);
            emit_byte(p, 0xff);
            emit_modrm_mem(p, group, &dst.mem);
            return memory_prefix_len(&dst.mem) + 2 + memory_tail_len(&dst.mem);
        }
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "ambiguous %s operand size", name);
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inner->name, ERROR_TYPE_I386,
                      "unsupported %s form", name);
    return INSTRUCTION_FAILED;
}

static nint encode_misc_prefixed_instruction(struct Parser *p,
                                             struct AstInstruction *inner,
                                             unint force_o32,
                                             unint force_a16,
                                             unint force_a32,
                                             unint force_o16) {
    nint status;

    status = emit_prefixed_convert(p, inner, force_o32, force_a32, force_o16);
    if(status != 0) return status;
    status = emit_prefixed_decimal_adjust(p, inner, force_o32, force_a32);
    if(status != 0) return status;
    status = emit_prefixed_flag_control(p, inner, force_o32, force_a32);
    if(status != 0) return status;
    status = emit_prefixed_scas(p, inner, force_o32, force_a32, force_o16);
    if(status != 0) return status;
    status = emit_prefixed_movs(p, inner, force_o32, force_a32, force_o16);
    if(status != 0) return status;
    status = emit_prefixed_lods(p, inner, force_o32, force_a32, force_o16);
    if(status != 0) return status;
    status = emit_prefixed_stos(p, inner, force_o32, force_a32, force_o16);
    if(status != 0) return status;
    status = emit_prefixed_ins(p, inner, force_o32, force_a32, force_o16);
    if(status != 0) return status;
    status = emit_prefixed_outs(p, inner, force_o32, force_a32, force_o16);
    if(status != 0) return status;
    status = emit_prefixed_interrupt(p, inner, force_o32, force_a32,
                                     force_o16);
    if(status != 0) return status;
    {
        unsigned char short_opcode = 0;
        unsigned char near_opcode = 0;
    if(jcc_opcodes_from_name(inner->name, &short_opcode, &near_opcode)) {
            return encode_prefixed_jcc(p, inner, short_opcode, near_opcode,
                                       force_o32, force_a16, force_a32);
        }
    }
    if(token_is(inner->name, LOOP_INSTRUCTION)) {
        return encode_prefixed_rel8_control(p, inner, 0xe2,
                                            force_o32, force_a32);
    }
    if(token_is(inner->name, LOOPE_INSTRUCTION) ||
       token_is(inner->name, LOOPZ_INSTRUCTION)) {
        return encode_prefixed_rel8_control(p, inner, 0xe1,
                                            force_o32, force_a32);
    }
    if(token_is(inner->name, LOOPNE_INSTRUCTION) ||
       token_is(inner->name, LOOPNZ_INSTRUCTION)) {
        return encode_prefixed_rel8_control(p, inner, 0xe0,
                                            force_o32, force_a32);
    }
    if(token_is(inner->name, JCXZ_INSTRUCTION)) {
        return encode_prefixed_rel8_control(p, inner, 0xe3,
                                            force_o32, force_a32);
    }
    if(token_is(inner->name, JECXZ_INSTRUCTION)) {
        return encode_prefixed_rel8_control(p, inner, 0xe3,
                                            force_o32, 1);
    }
    if(token_is(inner->name, JMP_INSTRUCTION)) {
        return encode_prefixed_jmp(p, inner, force_o32, force_a16, force_a32,
                                   force_o16);
    }
    if(token_is(inner->name, LEA_INSTRUCTION)) {
        return encode_lea_prefixed(p, inner, force_o32, force_a16, force_a32,
                                   force_o16);
    }
    if(token_is(inner->name, LDS_INSTRUCTION)) {
        return encode_far_load_prefixed(p, inner, 0xc5, force_o32, force_a16,
                                        force_a32, force_o16);
    }
    if(token_is(inner->name, LES_INSTRUCTION)) {
        return encode_far_load_prefixed(p, inner, 0xc4, force_o32, force_a16,
                                        force_a32, force_o16);
    }
    if(token_is(inner->name, LFS_INSTRUCTION)) {
        return encode_far_load_0f_prefixed(p, inner, 0xb4, force_o32,
                                           force_a16, force_a32, force_o16);
    }
    if(token_is(inner->name, LGS_INSTRUCTION)) {
        return encode_far_load_0f_prefixed(p, inner, 0xb5, force_o32,
                                           force_a16, force_a32, force_o16);
    }
    if(token_is(inner->name, LSS_INSTRUCTION)) {
        return encode_far_load_0f_prefixed(p, inner, 0xb2, force_o32,
                                           force_a16, force_a32, force_o16);
    }
    status = encode_system_one_operand_prefixed(p, inner, force_o32,
                                                force_a16, force_a32,
                                                force_o16);
    if(status != 0) return status;

    if(token_is(inner->name, ENTER_INSTRUCTION)) {
        return encode_prefixed_enter(p, inner, force_o32, force_a32);
    }
    if(token_is(inner->name, LEAVE_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0xc9,
                                          force_o32, force_a32);
    }
    if(token_is(inner->name, RET_INSTRUCTION) ||
       token_is(inner->name, RETN_INSTRUCTION)) {
        return encode_prefixed_ret(p, inner, 0xc3, 0xc2,
                                   force_o32, force_a32);
    }
    if(token_is(inner->name, RETF_INSTRUCTION)) {
        return encode_prefixed_ret(p, inner, 0xcb, 0xca,
                                   force_o32, force_a32);
    }
    if(token_is(inner->name, INC_INSTRUCTION)) {
        if(force_o16) {
            return encode_o16_inc_dec(p, inner, 0, 0x40, "inc",
                                      force_a16, force_a32);
        }
        return encode_prefixed_inc_dec(p, inner, 0, 0x40, "inc",
                                       force_o32, force_a16, force_a32);
    }
    if(token_is(inner->name, DEC_INSTRUCTION)) {
        if(force_o16) {
            return encode_o16_inc_dec(p, inner, 1, 0x48, "dec",
                                      force_a16, force_a32);
        }
        return encode_prefixed_inc_dec(p, inner, 1, 0x48, "dec",
                                       force_o32, force_a16, force_a32);
    }
    if(token_is(inner->name, PUSH_INSTRUCTION)) {
        return encode_prefixed_push(p, inner, force_o32, force_a16,
                                    force_a32, force_o16);
    }
    if(token_is(inner->name, POP_INSTRUCTION)) {
        return encode_prefixed_pop(p, inner, force_o32, force_a16, force_a32,
                                   force_o16);
    }
    if(token_is(inner->name, PUSHA_INSTRUCTION) ||
       token_is(inner->name, PUSHAW_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0x60,
                                          force_o32, force_a32);
    }
    if(token_is(inner->name, PUSHAD_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0x60,
                                          force_o16 ? 0 : 1, force_a32);
    }
    if(token_is(inner->name, POPA_INSTRUCTION) ||
       token_is(inner->name, POPAW_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0x61,
                                          force_o32, force_a32);
    }
    if(token_is(inner->name, POPAD_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0x61,
                                          force_o16 ? 0 : 1, force_a32);
    }
    if(token_is(inner->name, PUSHF_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0x9c,
                                          force_o32, force_a32);
    }
    if(token_is(inner->name, PUSHFD_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0x9c,
                                          force_o16 ? 0 : 1, force_a32);
    }
    if(token_is(inner->name, POPF_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0x9d,
                                          force_o32, force_a32);
    }
    if(token_is(inner->name, POPFD_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0x9d,
                                          force_o16 ? 0 : 1, force_a32);
    }
    if(token_is(inner->name, XLAT_INSTRUCTION) ||
       token_is(inner->name, XLATB_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0xd7,
                                          force_o32, force_a32);
    }
    if(token_is(inner->name, XCHG_INSTRUCTION)) {
        return encode_prefixed_xchg(p, inner, force_o32, force_a16,
                                    force_a32);
    }
    if(token_is(inner->name, SHLD_INSTRUCTION)) {
        return encode_prefixed_double_shift(p, inner, 0xa4, 0xa5, "shld",
                                            force_o32, force_a16, force_a32,
                                            force_o16);
    }
    if(token_is(inner->name, SHRD_INSTRUCTION)) {
        return encode_prefixed_double_shift(p, inner, 0xac, 0xad, "shrd",
                                            force_o32, force_a16, force_a32,
                                            force_o16);
    }
    if(token_is(inner->name, SHL_INSTRUCTION) ||
       token_is(inner->name, SAL_INSTRUCTION)) {
        return encode_prefixed_shift_group(p, inner, 4, force_o32,
                                           force_a16, force_a32, force_o16);
    }
    if(token_is(inner->name, SHR_INSTRUCTION)) {
        return encode_prefixed_shift_group(p, inner, 5, force_o32,
                                           force_a16, force_a32, force_o16);
    }
    if(token_is(inner->name, SAR_INSTRUCTION)) {
        return encode_prefixed_shift_group(p, inner, 7, force_o32,
                                           force_a16, force_a32, force_o16);
    }
    if(token_is(inner->name, ROL_INSTRUCTION)) {
        return encode_prefixed_shift_group(p, inner, 0, force_o32,
                                           force_a16, force_a32, force_o16);
    }
    if(token_is(inner->name, ROR_INSTRUCTION)) {
        return encode_prefixed_shift_group(p, inner, 1, force_o32,
                                           force_a16, force_a32, force_o16);
    }
    if(token_is(inner->name, RCL_INSTRUCTION)) {
        return encode_prefixed_shift_group(p, inner, 2, force_o32,
                                           force_a16, force_a32, force_o16);
    }
    if(token_is(inner->name, RCR_INSTRUCTION)) {
        return encode_prefixed_shift_group(p, inner, 3, force_o32,
                                           force_a16, force_a32, force_o16);
    }
    {
        unsigned char setcc_opcode = 0;
        const char *setcc_name = NULL;
        if(setcc_opcode_from_name(inner->name, &setcc_opcode, &setcc_name)) {
            return encode_prefixed_setcc(p, inner, setcc_opcode, setcc_name,
                                         force_o32, force_a16, force_a32);
        }
    }
    if(token_is(inner->name, NEG_INSTRUCTION)) {
        return encode_prefixed_unary_group(p, inner, 3, "neg",
                                           force_o32, force_a16, force_a32);
    }
    if(token_is(inner->name, MUL_INSTRUCTION)) {
        return encode_prefixed_unary_group(p, inner, 4, "mul",
                                           force_o32, force_a16, force_a32);
    }
    if(token_is(inner->name, NOT_INSTRUCTION)) {
        return encode_prefixed_unary_group(p, inner, 2, "not",
                                           force_o32, force_a16, force_a32);
    }
    if(token_is(inner->name, DIV_INSTRUCTION)) {
        return encode_prefixed_unary_group(p, inner, 6, "div",
                                           force_o32, force_a16, force_a32);
    }
    if(token_is(inner->name, IDIV_INSTRUCTION)) {
        return encode_prefixed_unary_group(p, inner, 7, "idiv",
                                           force_o32, force_a16, force_a32);
    }
    if(token_is(inner->name, IMUL_INSTRUCTION)) {
        return encode_prefixed_imul(p, inner, force_o32, force_a16, force_a32);
    }
    if(token_is(inner->name, IN_INSTRUCTION)) {
        return encode_prefixed_in(p, inner, force_o32, force_a32, force_o16);
    }
    if(token_is(inner->name, OUT_INSTRUCTION)) {
        return encode_prefixed_out(p, inner, force_o32, force_a32, force_o16);
    }
    if(token_is(inner->name, ESC_INSTRUCTION)) {
        return encode_prefixed_esc(p, inner, force_o32, force_a16, force_a32);
    }
    if(token_is(inner->name, LAR_INSTRUCTION)) {
        return encode_lar_prefixed(p, inner, force_o32, force_a16, force_a32,
                                   force_o16);
    }
    if(token_is(inner->name, LSL_INSTRUCTION)) {
        return encode_lsl_prefixed(p, inner, force_o32, force_a16, force_a32,
                                   force_o16);
    }
    if(token_is(inner->name, NOP_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0x90,
                                          force_o32, force_a32);
    }

    return 0;
}

static nint encode_push(struct Parser *p, struct AstInstruction *inst,
                        struct Operand *src) {
    if(src->kind == OPERAND_REG16) {
        emit_byte(p, (unsigned char)(0x50 + src->reg));
        return 1;
    }

    if(src->kind == OPERAND_REG32) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, (unsigned char)(0x50 + src->reg));
        return 2;
    }

    if(src->kind == OPERAND_SEG) {
        if(src->reg < 4) {
            emit_byte(p, (unsigned char)(0x06 + (src->reg << 3)));
            return 1;
        }
        emit_byte(p, 0x0f);
        emit_byte(p, src->reg == 4 ? 0xa0 : 0xa8);
        return 2;
    }

    if(src->kind == OPERAND_IMM) {
        if(src->size == 1) {
            emit_byte(p, 0x6a);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 2;
        }
        if(src->size == 4) {
            emit_operand_size_prefix(p, 4);
            if(fits_i8(src->imm)) {
                emit_byte(p, 0x6a);
                emit_byte(p, (unsigned char)(src->imm & 0xff));
                return 3;
            }
            if(fits_u32(src->imm)) {
                emit_byte(p, 0x68);
                emit_dword(p, src->imm);
                return 6;
            }
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 32 bits");
            return INSTRUCTION_FAILED;
        }
        if(fits_push_imm8(src->imm)) {
            emit_byte(p, 0x6a);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return 2;
        }
        if(fits_u16(src->imm)) {
            emit_byte(p, 0x68);
            emit_word(p, src->imm);
            return 3;
        }
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_OVERFLOW,
                          "immediate does not fit 16 bits");
        return INSTRUCTION_FAILED;
    }

    if(src->kind == OPERAND_MEM) {
        if(src->size == 1) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid push operand size");
            return INSTRUCTION_FAILED;
        }
        if(src->size != 2 && src->size != 4) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "ambiguous push operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &src->mem, src->size);
        emit_byte(p, 0xff);
        emit_modrm_mem(p, 6, &src->mem);
        return memory_operand_prefix_len(&src->mem, src->size) + 2 +
               memory_tail_len(&src->mem);
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported push form");
    return INSTRUCTION_FAILED;
}

static nint parse_push_sized_immediate(struct Parser *p, struct InstructionArg *arg,
                                       struct Operand *op,
                                       struct token **error_token) {
    struct InstructionArg imm_arg;
    struct token *size_tok;
    struct token *next_tok;
    nint size;
    nint status;

    size_tok = arg->_s;
    if(!size_tok || size_tok->type != NAME) return 0;

    size = explicit_type_size(size_tok);
    if(!size) return 0;

    next_tok = size_tok + 1;
    if(next_tok > arg->_e) {
        record_error_token(error_token, size_tok);
        return INSTRUCTION_FAILED;
    }

    if(token_is(next_tok, PTR_NAME)) return 0;

    imm_arg._s = next_tok;
    imm_arg._e = arg->_e;
    imm_arg.next = NULL;
    status = parse_operand(p, &imm_arg, op, error_token);
    if(status <= 0) return status;

    if(op->kind != OPERAND_IMM) {
        record_error_token(error_token, next_tok);
        return INSTRUCTION_FAILED;
    }

    op->size = size;
    return 1;
}

static nint parse_push_operand(struct Parser *p, struct InstructionArg *arg,
                               struct Operand *op,
                               struct token **error_token) {
    nint status = parse_push_sized_immediate(p, arg, op, error_token);
    if(status) return status;
    return parse_operand(p, arg, op, error_token);
}

static nint encode_pop(struct Parser *p, struct AstInstruction *inst,
                       struct Operand *dst) {
    if(dst->kind == OPERAND_REG16) {
        emit_byte(p, (unsigned char)(0x58 + dst->reg));
        return 1;
    }

    if(dst->kind == OPERAND_REG32) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, (unsigned char)(0x58 + dst->reg));
        return 2;
    }

    if(dst->kind == OPERAND_SEG) {
        if(dst->reg == 1) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "unsupported pop segment register");
            return INSTRUCTION_FAILED;
        }
        if(dst->reg < 4) {
            emit_byte(p, (unsigned char)(0x07 + (dst->reg << 3)));
            return 1;
        }
        emit_byte(p, 0x0f);
        emit_byte(p, dst->reg == 4 ? 0xa1 : 0xa9);
        return 2;
    }

    if(dst->kind == OPERAND_MEM) {
        if(dst->size == 1) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid pop operand size");
            return INSTRUCTION_FAILED;
        }
        if(dst->size != 2 && dst->size != 4) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "ambiguous pop operand size");
            return INSTRUCTION_FAILED;
        }
        emit_memory_operand_prefixes(p, &dst->mem, dst->size);
        emit_byte(p, 0x8f);
        emit_modrm_mem(p, 0, &dst->mem);
        return memory_operand_prefix_len(&dst->mem, dst->size) + 2 +
               memory_tail_len(&dst->mem);
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported pop form");
    return INSTRUCTION_FAILED;
}

static nint encode_o32_push(struct Parser *p, struct AstInstruction *inner) {
    struct Operand src;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count != 1) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_push_operand(p, inner->args_head, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }

    if(src.kind == OPERAND_REG16 || src.kind == OPERAND_REG32) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, (unsigned char)(0x50 + src.reg));
        return 2;
    }

    if(src.kind == OPERAND_SEG) {
        emit_operand_size_prefix(p, 4);
        if(src.reg < 4) {
            emit_byte(p, (unsigned char)(0x06 + (src.reg << 3)));
            return 2;
        }
        emit_byte(p, 0x0f);
        emit_byte(p, src.reg == 4 ? 0xa0 : 0xa8);
        return 3;
    }

    if(src.kind == OPERAND_IMM) {
        emit_operand_size_prefix(p, 4);
        if(src.size == 1 || fits_push_imm8(src.imm)) {
            emit_byte(p, 0x6a);
            emit_byte(p, (unsigned char)(src.imm & 0xff));
            return 3;
        }
        if(src.size == 4) {
            if(!fits_u32(src.imm)) {
                _error_from_token(p, inner->args_head->_s, ERROR_TYPE_OVERFLOW,
                                  "immediate does not fit 32 bits");
                return INSTRUCTION_FAILED;
            }
            emit_byte(p, 0x68);
            emit_dword(p, src.imm);
            return 6;
        }
        if(!fits_u16(src.imm)) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_byte(p, 0x68);
        emit_word(p, src.imm);
        return 4;
    }

    if(src.kind == OPERAND_MEM) {
        if(src.size == 1) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "invalid push operand size");
            return INSTRUCTION_FAILED;
        }
        if(src.size != 2 && src.size != 4) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "ambiguous push operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &src.mem);
        emit_operand_size_prefix(p, 4);
        emit_address_size_prefix(p, &src.mem);
        emit_byte(p, 0xff);
        emit_modrm_mem(p, 6, &src.mem);
        return memory_prefix_len(&src.mem) + 3 + memory_tail_len(&src.mem);
    }

    _error_from_token(p, inner->name, ERROR_TYPE_I386, "unsupported push form");
    return INSTRUCTION_FAILED;
}

static nint encode_prefixed_push(struct Parser *p,
                                 struct AstInstruction *inner,
                                 unint force_o32,
                                 unint force_a16,
                                 unint force_a32,
                                 unint force_o16) {
    struct Operand src;
    struct token *error_token = NULL;
    unint op_prefix = force_o32;
    nint status;

    if(inner->arg_count != 1) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_push_operand(p, inner->args_head, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }

    if(force_a32) force_direct_memory_a32(&src);
    if(force_a16 && operand_is_mem32(&src)) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "invalid a16 address form");
        return INSTRUCTION_FAILED;
    }
    if(force_a32 && operand_is_mem16(&src)) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "invalid a32 address form");
        return INSTRUCTION_FAILED;
    }

    if((src.kind == OPERAND_REG32 || (src.kind == OPERAND_MEM && src.size == 4) ||
        (src.kind == OPERAND_IMM && src.size == 4)) && !force_o16) {
        op_prefix = 1;
    }

    if(src.kind == OPERAND_REG16 || src.kind == OPERAND_REG32) {
        if(op_prefix) emit_operand_size_prefix(p, 4);
        if(force_a32) emit_byte(p, 0x67);
        emit_byte(p, (unsigned char)(0x50 + src.reg));
        return 1 + (op_prefix ? 1 : 0) + (force_a32 ? 1 : 0);
    }

    if(src.kind == OPERAND_SEG) {
        if(op_prefix) emit_operand_size_prefix(p, 4);
        if(force_a32) emit_byte(p, 0x67);
        if(src.reg < 4) {
            emit_byte(p, (unsigned char)(0x06 + (src.reg << 3)));
            return 1 + (op_prefix ? 1 : 0) + (force_a32 ? 1 : 0);
        }
        emit_byte(p, 0x0f);
        emit_byte(p, src.reg == 4 ? 0xa0 : 0xa8);
        return 2 + (op_prefix ? 1 : 0) + (force_a32 ? 1 : 0);
    }

    if(src.kind == OPERAND_IMM) {
        if(op_prefix) emit_operand_size_prefix(p, 4);
        if(force_a32) emit_byte(p, 0x67);
        if(src.size == 1 || fits_push_imm8(src.imm)) {
            emit_byte(p, 0x6a);
            emit_byte(p, (unsigned char)(src.imm & 0xff));
            return 2 + (op_prefix ? 1 : 0) + (force_a32 ? 1 : 0);
        }
        if(src.size == 4 && !force_o16) {
            if(!fits_u32(src.imm)) {
                _error_from_token(p, inner->args_head->_s, ERROR_TYPE_OVERFLOW,
                                  "immediate does not fit 32 bits");
                return INSTRUCTION_FAILED;
            }
            emit_byte(p, 0x68);
            emit_dword(p, src.imm);
            return 5 + (op_prefix ? 1 : 0) + (force_a32 ? 1 : 0);
        }
        if(!fits_u16(src.imm)) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_byte(p, 0x68);
        emit_word(p, src.imm);
        return 3 + (op_prefix ? 1 : 0) + (force_a32 ? 1 : 0);
    }

    if(src.kind == OPERAND_MEM) {
        if(src.size == 1) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "invalid push operand size");
            return INSTRUCTION_FAILED;
        }
        if(src.size != 2 && src.size != 4) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "ambiguous push operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &src.mem);
        if(op_prefix) emit_operand_size_prefix(p, 4);
        emit_address_size_prefix(p, &src.mem);
        emit_byte(p, 0xff);
        emit_modrm_mem(p, 6, &src.mem);
        return memory_prefix_len(&src.mem) + (op_prefix ? 1 : 0) + 2 +
               memory_tail_len(&src.mem);
    }

    _error_from_token(p, inner->name, ERROR_TYPE_I386, "unsupported push form");
    return INSTRUCTION_FAILED;
}

static nint encode_o32_pop(struct Parser *p, struct AstInstruction *inner) {
    struct Operand dst;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count != 1) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }

    if(dst.kind == OPERAND_REG16 || dst.kind == OPERAND_REG32) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, (unsigned char)(0x58 + dst.reg));
        return 2;
    }

    if(dst.kind == OPERAND_SEG) {
        if(dst.reg == 1) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "unsupported pop segment register");
            return INSTRUCTION_FAILED;
        }
        emit_operand_size_prefix(p, 4);
        if(dst.reg < 4) {
            emit_byte(p, (unsigned char)(0x07 + (dst.reg << 3)));
            return 2;
        }
        emit_byte(p, 0x0f);
        emit_byte(p, dst.reg == 4 ? 0xa1 : 0xa9);
        return 3;
    }

    if(dst.kind == OPERAND_MEM) {
        if(dst.size == 1) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "invalid pop operand size");
            return INSTRUCTION_FAILED;
        }
        if(dst.size != 2 && dst.size != 4) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "ambiguous pop operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst.mem);
        emit_operand_size_prefix(p, 4);
        emit_address_size_prefix(p, &dst.mem);
        emit_byte(p, 0x8f);
        emit_modrm_mem(p, 0, &dst.mem);
        return memory_prefix_len(&dst.mem) + 3 + memory_tail_len(&dst.mem);
    }

    _error_from_token(p, inner->name, ERROR_TYPE_I386, "unsupported pop form");
    return INSTRUCTION_FAILED;
}

static nint encode_prefixed_pop(struct Parser *p,
                                struct AstInstruction *inner,
                                unint force_o32,
                                unint force_a16,
                                unint force_a32,
                                unint force_o16) {
    struct Operand dst;
    struct token *error_token = NULL;
    unint op_prefix = force_o32;
    nint status;

    if(inner->arg_count != 1) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }

    if(force_a32) force_direct_memory_a32(&dst);
    if(force_a16 && operand_is_mem32(&dst)) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "invalid a16 address form");
        return INSTRUCTION_FAILED;
    }
    if(force_a32 && operand_is_mem16(&dst)) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "invalid a32 address form");
        return INSTRUCTION_FAILED;
    }

    if((dst.kind == OPERAND_REG32 || (dst.kind == OPERAND_MEM && dst.size == 4)) &&
       !force_o16) {
        op_prefix = 1;
    }

    if(dst.kind == OPERAND_REG16 || dst.kind == OPERAND_REG32) {
        if(op_prefix) emit_operand_size_prefix(p, 4);
        if(force_a32) emit_byte(p, 0x67);
        emit_byte(p, (unsigned char)(0x58 + dst.reg));
        return 1 + (op_prefix ? 1 : 0) + (force_a32 ? 1 : 0);
    }

    if(dst.kind == OPERAND_SEG) {
        if(dst.reg == 1) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "unsupported pop segment register");
            return INSTRUCTION_FAILED;
        }
        if(op_prefix) emit_operand_size_prefix(p, 4);
        if(force_a32) emit_byte(p, 0x67);
        if(dst.reg < 4) {
            emit_byte(p, (unsigned char)(0x07 + (dst.reg << 3)));
            return 1 + (op_prefix ? 1 : 0) + (force_a32 ? 1 : 0);
        }
        emit_byte(p, 0x0f);
        emit_byte(p, dst.reg == 4 ? 0xa1 : 0xa9);
        return 2 + (op_prefix ? 1 : 0) + (force_a32 ? 1 : 0);
    }

    if(dst.kind == OPERAND_MEM) {
        if(dst.size == 1) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "invalid pop operand size");
            return INSTRUCTION_FAILED;
        }
        if(dst.size != 2 && dst.size != 4) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "ambiguous pop operand size");
            return INSTRUCTION_FAILED;
        }
        emit_segment_prefix(p, &dst.mem);
        if(op_prefix) emit_operand_size_prefix(p, 4);
        emit_address_size_prefix(p, &dst.mem);
        emit_byte(p, 0x8f);
        emit_modrm_mem(p, 0, &dst.mem);
        return memory_prefix_len(&dst.mem) + (op_prefix ? 1 : 0) + 2 +
               memory_tail_len(&dst.mem);
    }

    _error_from_token(p, inner->name, ERROR_TYPE_I386, "unsupported pop form");
    return INSTRUCTION_FAILED;
}

static void emit_forced_o32_memory_prefixes(struct Parser *p,
                                            struct MemoryOperand *mem) {
    emit_segment_prefix(p, mem);
    emit_operand_size_prefix(p, 4);
    emit_address_size_prefix(p, mem);
}

static nint forced_o32_memory_prefix_len(struct MemoryOperand *mem) {
    return memory_prefix_len(mem) + 1;
}

static unint arith_operands_naturally_o32(struct Operand *dst,
                                          struct Operand *src) {
    return dst->kind == OPERAND_REG32 || src->kind == OPERAND_REG32 ||
           (dst->kind == OPERAND_MEM && dst->size == 4) ||
           (src->kind == OPERAND_MEM && src->size == 4);
}

static nint encode_o32_mov(struct Parser *p, struct AstInstruction *inner) {
    struct Operand dst;
    struct Operand src;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }
    error_token = NULL;
    status = parse_operand(p, inner->args_tail, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    if(dst.kind == OPERAND_REG32 && src.kind == OPERAND_CR) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x0f);
        emit_byte(p, 0x20);
        emit_modrm_reg(p, src.reg, dst.reg);
        return 4;
    }
    if(dst.kind == OPERAND_CR && src.kind == OPERAND_REG32) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x0f);
        emit_byte(p, 0x22);
        emit_modrm_reg(p, dst.reg, src.reg);
        return 4;
    }
    if(dst.kind == OPERAND_REG32 && src.kind == OPERAND_DR) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x0f);
        emit_byte(p, 0x21);
        emit_modrm_reg(p, src.reg, dst.reg);
        return 4;
    }
    if(dst.kind == OPERAND_DR && src.kind == OPERAND_REG32) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x0f);
        emit_byte(p, 0x23);
        emit_modrm_reg(p, dst.reg, src.reg);
        return 4;
    }
    if(dst.kind == OPERAND_REG32 && src.kind == OPERAND_TR) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x0f);
        emit_byte(p, 0x24);
        emit_modrm_reg(p, src.reg, dst.reg);
        return 4;
    }
    if(dst.kind == OPERAND_TR && src.kind == OPERAND_REG32) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x0f);
        emit_byte(p, 0x26);
        emit_modrm_reg(p, dst.reg, src.reg);
        return 4;
    }

    if(arith_operands_naturally_o32(&dst, &src)) {
        return encode_mov(p, inner, &dst, &src);
    }

    if(dst.kind == OPERAND_REG8 && src.kind == OPERAND_IMM) {
        if(!fits_byte_immediate(src.imm)) {
            _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 8 bits");
            return INSTRUCTION_FAILED;
        }
        emit_operand_size_prefix(p, 4);
        emit_byte(p, (unsigned char)(0xb0 + dst.reg));
        emit_byte(p, (unsigned char)src.imm);
        return 3;
    }
    if(dst.kind == OPERAND_REG16 && src.kind == OPERAND_IMM) {
        if(!fits_u16(src.imm)) {
            _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        emit_operand_size_prefix(p, 4);
        emit_byte(p, (unsigned char)(0xb8 + dst.reg));
        emit_word(p, src.imm);
        return 4;
    }

    if(is_al(&dst) && src.kind == OPERAND_MEM && src.mem.direct) {
        if(!memory_size_matches(&src, 1)) {
            _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_I386,
                              "invalid mov operand size");
            return INSTRUCTION_FAILED;
        }
        emit_forced_o32_memory_prefixes(p, &src.mem);
        emit_byte(p, 0xa0);
        emit_word(p, src.mem.disp);
        return forced_o32_memory_prefix_len(&src.mem) + 3;
    }
    if(is_ax(&dst) && src.kind == OPERAND_MEM && src.mem.direct) {
        if(!memory_size_matches(&src, 2)) {
            _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_I386,
                              "invalid mov operand size");
            return INSTRUCTION_FAILED;
        }
        emit_forced_o32_memory_prefixes(p, &src.mem);
        emit_byte(p, 0xa1);
        emit_word(p, src.mem.disp);
        return forced_o32_memory_prefix_len(&src.mem) + 3;
    }
    if(dst.kind == OPERAND_MEM && dst.mem.direct && is_al(&src)) {
        if(!memory_size_matches(&dst, 1)) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "invalid mov operand size");
            return INSTRUCTION_FAILED;
        }
        emit_forced_o32_memory_prefixes(p, &dst.mem);
        emit_byte(p, 0xa2);
        emit_word(p, dst.mem.disp);
        return forced_o32_memory_prefix_len(&dst.mem) + 3;
    }
    if(dst.kind == OPERAND_MEM && dst.mem.direct && is_ax(&src)) {
        if(!memory_size_matches(&dst, 2)) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "invalid mov operand size");
            return INSTRUCTION_FAILED;
        }
        emit_forced_o32_memory_prefixes(p, &dst.mem);
        emit_byte(p, 0xa3);
        emit_word(p, dst.mem.disp);
        return forced_o32_memory_prefix_len(&dst.mem) + 3;
    }

    if(dst.kind == OPERAND_REG8 && src.kind == OPERAND_REG8) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x88);
        emit_modrm_reg(p, src.reg, dst.reg);
        return 3;
    }
    if(dst.kind == OPERAND_REG16 && src.kind == OPERAND_REG16) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x89);
        emit_modrm_reg(p, src.reg, dst.reg);
        return 3;
    }

    if(dst.kind == OPERAND_REG8 && src.kind == OPERAND_MEM) {
        if(!memory_size_matches(&src, 1)) {
            _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_I386,
                              "invalid mov operand size");
            return INSTRUCTION_FAILED;
        }
        emit_forced_o32_memory_prefixes(p, &src.mem);
        emit_byte(p, 0x8a);
        emit_modrm_mem(p, dst.reg, &src.mem);
        return forced_o32_memory_prefix_len(&src.mem) + 2 +
               memory_tail_len(&src.mem);
    }
    if(dst.kind == OPERAND_REG16 && src.kind == OPERAND_MEM) {
        if(!memory_size_matches(&src, 2)) {
            _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_I386,
                              "invalid mov operand size");
            return INSTRUCTION_FAILED;
        }
        emit_forced_o32_memory_prefixes(p, &src.mem);
        emit_byte(p, 0x8b);
        emit_modrm_mem(p, dst.reg, &src.mem);
        return forced_o32_memory_prefix_len(&src.mem) + 2 +
               memory_tail_len(&src.mem);
    }
    if(dst.kind == OPERAND_MEM && src.kind == OPERAND_REG8) {
        if(!memory_size_matches(&dst, 1)) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "invalid mov operand size");
            return INSTRUCTION_FAILED;
        }
        emit_forced_o32_memory_prefixes(p, &dst.mem);
        emit_byte(p, 0x88);
        emit_modrm_mem(p, src.reg, &dst.mem);
        return forced_o32_memory_prefix_len(&dst.mem) + 2 +
               memory_tail_len(&dst.mem);
    }
    if(dst.kind == OPERAND_MEM && src.kind == OPERAND_REG16) {
        if(!memory_size_matches(&dst, 2)) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "invalid mov operand size");
            return INSTRUCTION_FAILED;
        }
        emit_forced_o32_memory_prefixes(p, &dst.mem);
        emit_byte(p, 0x89);
        emit_modrm_mem(p, src.reg, &dst.mem);
        return forced_o32_memory_prefix_len(&dst.mem) + 2 +
               memory_tail_len(&dst.mem);
    }

    if(dst.kind == OPERAND_REG16 && src.kind == OPERAND_SEG) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x8c);
        emit_modrm_reg(p, src.reg, dst.reg);
        return 3;
    }
    if(dst.kind == OPERAND_MEM && src.kind == OPERAND_SEG) {
        if(!memory_size_matches(&dst, 2)) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "invalid mov operand size");
            return INSTRUCTION_FAILED;
        }
        emit_forced_o32_memory_prefixes(p, &dst.mem);
        emit_byte(p, 0x8c);
        emit_modrm_mem(p, src.reg, &dst.mem);
        return forced_o32_memory_prefix_len(&dst.mem) + 2 +
               memory_tail_len(&dst.mem);
    }
    if(dst.kind == OPERAND_SEG && src.kind == OPERAND_REG16) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x8e);
        emit_modrm_reg(p, dst.reg, src.reg);
        return 3;
    }
    if(dst.kind == OPERAND_SEG && src.kind == OPERAND_MEM) {
        if(!memory_size_matches(&src, 2)) {
            _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_I386,
                              "invalid mov operand size");
            return INSTRUCTION_FAILED;
        }
        emit_forced_o32_memory_prefixes(p, &src.mem);
        emit_byte(p, 0x8e);
        emit_modrm_mem(p, dst.reg, &src.mem);
        return forced_o32_memory_prefix_len(&src.mem) + 2 +
               memory_tail_len(&src.mem);
    }

    if(dst.kind == OPERAND_MEM && src.kind == OPERAND_IMM) {
        if(dst.size == 1) {
            if(!fits_byte_immediate(src.imm)) {
                _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_OVERFLOW,
                                  "immediate does not fit 8 bits");
                return INSTRUCTION_FAILED;
            }
            emit_forced_o32_memory_prefixes(p, &dst.mem);
            emit_byte(p, 0xc6);
            emit_modrm_mem(p, 0, &dst.mem);
            emit_byte(p, (unsigned char)src.imm);
            return forced_o32_memory_prefix_len(&dst.mem) + 3 +
                   memory_tail_len(&dst.mem);
        }
        if(dst.size == 2) {
            if(!fits_u16(src.imm)) {
                _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_OVERFLOW,
                                  "immediate does not fit 16 bits");
                return INSTRUCTION_FAILED;
            }
            emit_forced_o32_memory_prefixes(p, &dst.mem);
            emit_byte(p, 0xc7);
            emit_modrm_mem(p, 0, &dst.mem);
            emit_word(p, src.imm);
            return forced_o32_memory_prefix_len(&dst.mem) + 4 +
                   memory_tail_len(&dst.mem);
        }

        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "ambiguous memory immediate size");
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inner->name, ERROR_TYPE_I386, "unsupported mov form");
    return INSTRUCTION_FAILED;
}

static nint encode_o32_arith(struct Parser *p, struct AstInstruction *inner,
                             unsigned char base, unsigned char acc8,
                             unsigned char acc16, nint group,
                             const char *name) {
    struct Operand dst;
    struct Operand src;
    struct token *error_token = NULL;
    nint len;
    nint status;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }
    error_token = NULL;
    status = parse_operand(p, inner->args_tail, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    if(arith_operands_naturally_o32(&dst, &src)) {
        return encode_arith(p, inner, &dst, &src, base, acc8, acc16,
                            group, name);
    }

    if(dst.kind == OPERAND_REG8 && src.kind == OPERAND_REG8) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, base);
        emit_modrm_reg(p, src.reg, dst.reg);
        return 3;
    }
    if(dst.kind == OPERAND_REG16 && src.kind == OPERAND_REG16) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, (unsigned char)(base + 1));
        emit_modrm_reg(p, src.reg, dst.reg);
        return 3;
    }

    if(dst.kind == OPERAND_MEM && src.kind == OPERAND_REG8) {
        if(!memory_size_matches(&dst, 1)) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "invalid %s operand size", name);
            return INSTRUCTION_FAILED;
        }
        emit_forced_o32_memory_prefixes(p, &dst.mem);
        emit_byte(p, base);
        emit_modrm_mem(p, src.reg, &dst.mem);
        return forced_o32_memory_prefix_len(&dst.mem) + 2 +
               memory_tail_len(&dst.mem);
    }
    if(dst.kind == OPERAND_MEM && src.kind == OPERAND_REG16) {
        if(!memory_size_matches(&dst, 2)) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "invalid %s operand size", name);
            return INSTRUCTION_FAILED;
        }
        emit_forced_o32_memory_prefixes(p, &dst.mem);
        emit_byte(p, (unsigned char)(base + 1));
        emit_modrm_mem(p, src.reg, &dst.mem);
        return forced_o32_memory_prefix_len(&dst.mem) + 2 +
               memory_tail_len(&dst.mem);
    }

    if(dst.kind == OPERAND_REG8 && src.kind == OPERAND_MEM) {
        if(!memory_size_matches(&src, 1)) {
            _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_I386,
                              "invalid %s operand size", name);
            return INSTRUCTION_FAILED;
        }
        emit_forced_o32_memory_prefixes(p, &src.mem);
        emit_byte(p, (unsigned char)(base + 2));
        emit_modrm_mem(p, dst.reg, &src.mem);
        return forced_o32_memory_prefix_len(&src.mem) + 2 +
               memory_tail_len(&src.mem);
    }
    if(dst.kind == OPERAND_REG16 && src.kind == OPERAND_MEM) {
        if(!memory_size_matches(&src, 2)) {
            _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_I386,
                              "invalid %s operand size", name);
            return INSTRUCTION_FAILED;
        }
        emit_forced_o32_memory_prefixes(p, &src.mem);
        emit_byte(p, (unsigned char)(base + 3));
        emit_modrm_mem(p, dst.reg, &src.mem);
        return forced_o32_memory_prefix_len(&src.mem) + 2 +
               memory_tail_len(&src.mem);
    }

    if(src.kind == OPERAND_IMM) {
        if(dst.kind == OPERAND_REG8) {
            if(!fits_u16(src.imm)) {
                _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_OVERFLOW,
                                  "immediate does not fit 16 bits");
                return INSTRUCTION_FAILED;
            }
            emit_operand_size_prefix(p, 4);
            if(is_al(&dst)) {
                emit_byte(p, acc8);
                emit_byte(p, (unsigned char)(src.imm & 0xff));
                return 3;
            }
            emit_byte(p, 0x80);
            emit_modrm_reg(p, group, dst.reg);
            emit_byte(p, (unsigned char)(src.imm & 0xff));
            return 4;
        }
        if(dst.kind == OPERAND_REG16) {
            if(!fits_u16(src.imm)) {
                _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_OVERFLOW,
                                  "immediate does not fit 16 bits");
                return INSTRUCTION_FAILED;
            }
            emit_operand_size_prefix(p, 4);
            if(fits_word_sign_extended_i8(src.imm)) {
                emit_byte(p, 0x83);
                emit_modrm_reg(p, group, dst.reg);
                emit_byte(p, (unsigned char)(src.imm & 0xff));
                return 4;
            }
            if(is_ax(&dst)) {
                emit_byte(p, acc16);
                emit_word(p, src.imm);
                return 4;
            }
            emit_byte(p, 0x81);
            emit_modrm_reg(p, group, dst.reg);
            emit_word(p, src.imm);
            return 5;
        }
        if(dst.kind == OPERAND_MEM) {
            if(dst.size == 1) {
                if(!fits_u16(src.imm)) {
                    _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_OVERFLOW,
                                      "immediate does not fit 16 bits");
                    return INSTRUCTION_FAILED;
                }
                emit_forced_o32_memory_prefixes(p, &dst.mem);
                emit_byte(p, 0x80);
                emit_modrm_mem(p, group, &dst.mem);
                emit_byte(p, (unsigned char)(src.imm & 0xff));
                return forced_o32_memory_prefix_len(&dst.mem) + 3 +
                       memory_tail_len(&dst.mem);
            }
            if(dst.size == 2) {
                if(!fits_u16(src.imm)) {
                    _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_OVERFLOW,
                                      "immediate does not fit 16 bits");
                    return INSTRUCTION_FAILED;
                }
                emit_forced_o32_memory_prefixes(p, &dst.mem);
                if(fits_word_sign_extended_i8(src.imm)) {
                    emit_byte(p, 0x83);
                    emit_modrm_mem(p, group, &dst.mem);
                    emit_byte(p, (unsigned char)(src.imm & 0xff));
                    return forced_o32_memory_prefix_len(&dst.mem) + 3 +
                           memory_tail_len(&dst.mem);
                }
                emit_byte(p, 0x81);
                emit_modrm_mem(p, group, &dst.mem);
                emit_word(p, src.imm);
                return forced_o32_memory_prefix_len(&dst.mem) + 4 +
                       memory_tail_len(&dst.mem);
            }
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "ambiguous memory immediate size");
            return INSTRUCTION_FAILED;
        }
    }

    len = encode_arith(p, inner, &dst, &src, base, acc8, acc16, group, name);
    if(len > 0) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "internal operand-size prefix error");
        return INSTRUCTION_FAILED;
    }
    return len;
}

static nint encode_o32_test(struct Parser *p, struct AstInstruction *inner) {
    struct Operand dst;
    struct Operand src;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }
    error_token = NULL;
    status = parse_operand(p, inner->args_tail, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    if(arith_operands_naturally_o32(&dst, &src)) {
        return encode_test(p, inner, &dst, &src);
    }

    if(dst.kind == OPERAND_REG8 && src.kind == OPERAND_REG8) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x84);
        emit_modrm_reg(p, src.reg, dst.reg);
        return 3;
    }
    if(dst.kind == OPERAND_REG16 && src.kind == OPERAND_REG16) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x85);
        emit_modrm_reg(p, src.reg, dst.reg);
        return 3;
    }

    if(dst.kind == OPERAND_MEM && src.kind == OPERAND_REG8) {
        if(!memory_size_matches(&dst, 1)) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "invalid test operand size");
            return INSTRUCTION_FAILED;
        }
        emit_forced_o32_memory_prefixes(p, &dst.mem);
        emit_byte(p, 0x84);
        emit_modrm_mem(p, src.reg, &dst.mem);
        return forced_o32_memory_prefix_len(&dst.mem) + 2 +
               memory_tail_len(&dst.mem);
    }
    if(dst.kind == OPERAND_MEM && src.kind == OPERAND_REG16) {
        if(!memory_size_matches(&dst, 2)) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "invalid test operand size");
            return INSTRUCTION_FAILED;
        }
        emit_forced_o32_memory_prefixes(p, &dst.mem);
        emit_byte(p, 0x85);
        emit_modrm_mem(p, src.reg, &dst.mem);
        return forced_o32_memory_prefix_len(&dst.mem) + 2 +
               memory_tail_len(&dst.mem);
    }

    if(dst.kind == OPERAND_REG8 && src.kind == OPERAND_MEM) {
        if(!memory_size_matches(&src, 1)) {
            _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_I386,
                              "invalid test operand size");
            return INSTRUCTION_FAILED;
        }
        emit_forced_o32_memory_prefixes(p, &src.mem);
        emit_byte(p, 0x84);
        emit_modrm_mem(p, dst.reg, &src.mem);
        return forced_o32_memory_prefix_len(&src.mem) + 2 +
               memory_tail_len(&src.mem);
    }
    if(dst.kind == OPERAND_REG16 && src.kind == OPERAND_MEM) {
        if(!memory_size_matches(&src, 2)) {
            _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_I386,
                              "invalid test operand size");
            return INSTRUCTION_FAILED;
        }
        emit_forced_o32_memory_prefixes(p, &src.mem);
        emit_byte(p, 0x85);
        emit_modrm_mem(p, dst.reg, &src.mem);
        return forced_o32_memory_prefix_len(&src.mem) + 2 +
               memory_tail_len(&src.mem);
    }

    if(src.kind == OPERAND_IMM) {
        if(dst.kind == OPERAND_REG8) {
            if(!fits_u16(src.imm)) {
                _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_OVERFLOW,
                                  "immediate does not fit 16 bits");
                return INSTRUCTION_FAILED;
            }
            emit_operand_size_prefix(p, 4);
            if(is_al(&dst)) {
                emit_byte(p, 0xa8);
                emit_byte(p, (unsigned char)(src.imm & 0xff));
                return 3;
            }
            emit_byte(p, 0xf6);
            emit_modrm_reg(p, 0, dst.reg);
            emit_byte(p, (unsigned char)(src.imm & 0xff));
            return 4;
        }
        if(dst.kind == OPERAND_REG16) {
            if(!fits_u16(src.imm)) {
                _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_OVERFLOW,
                                  "immediate does not fit 16 bits");
                return INSTRUCTION_FAILED;
            }
            emit_operand_size_prefix(p, 4);
            if(is_ax(&dst)) {
                emit_byte(p, 0xa9);
                emit_word(p, src.imm);
                return 4;
            }
            emit_byte(p, 0xf7);
            emit_modrm_reg(p, 0, dst.reg);
            emit_word(p, src.imm);
            return 5;
        }
        if(dst.kind == OPERAND_MEM) {
            if(dst.size == 1) {
                if(!fits_u16(src.imm)) {
                    _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_OVERFLOW,
                                      "immediate does not fit 16 bits");
                    return INSTRUCTION_FAILED;
                }
                emit_forced_o32_memory_prefixes(p, &dst.mem);
                emit_byte(p, 0xf6);
                emit_modrm_mem(p, 0, &dst.mem);
                emit_byte(p, (unsigned char)(src.imm & 0xff));
                return forced_o32_memory_prefix_len(&dst.mem) + 3 +
                       memory_tail_len(&dst.mem);
            }
            if(dst.size == 2) {
                if(!fits_u16(src.imm)) {
                    _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_OVERFLOW,
                                      "immediate does not fit 16 bits");
                    return INSTRUCTION_FAILED;
                }
                emit_forced_o32_memory_prefixes(p, &dst.mem);
                emit_byte(p, 0xf7);
                emit_modrm_mem(p, 0, &dst.mem);
                emit_word(p, src.imm);
                return forced_o32_memory_prefix_len(&dst.mem) + 4 +
                       memory_tail_len(&dst.mem);
            }
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "ambiguous memory immediate size");
            return INSTRUCTION_FAILED;
        }
    }

    _error_from_token(p, inner->name, ERROR_TYPE_I386, "unsupported test form");
    return INSTRUCTION_FAILED;
}

static nint encode_address_prefixed_test(struct Parser *p,
                                         struct AstInstruction *inner,
                                         unint force_a16,
                                         unint force_a32) {
    struct Operand dst;
    struct Operand src;
    struct token *error_token = NULL;
    nint status;
    unint needs_register_prefix;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }
    error_token = NULL;
    status = parse_operand(p, inner->args_tail, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    if(force_a32) {
        force_direct_memory_a32(&dst);
        force_direct_memory_a32(&src);
    }
    if(force_a16 && (operand_is_mem32(&dst) || operand_is_mem32(&src))) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid a16 address form");
        return INSTRUCTION_FAILED;
    }
    if(force_a32 && (operand_is_mem16(&dst) || operand_is_mem16(&src))) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid a32 address form");
        return INSTRUCTION_FAILED;
    }

    needs_register_prefix = force_a32 &&
                            dst.kind != OPERAND_MEM &&
                            src.kind != OPERAND_MEM;
    if(needs_register_prefix) emit_byte(p, 0x67);
    status = encode_test(p, inner, &dst, &src);
    if(status < 0) return status;
    return status + (needs_register_prefix ? 1 : 0);
}

static nint emit_o32_shift_prefixes(struct Parser *p, struct Operand *dst) {
    if(dst->kind == OPERAND_MEM) {
        emit_forced_o32_memory_prefixes(p, &dst->mem);
        return forced_o32_memory_prefix_len(&dst->mem);
    }
    emit_operand_size_prefix(p, 4);
    return 1;
}

static nint encode_o32_shift_group(struct Parser *p, struct AstInstruction *inner,
                                   nint group) {
    struct Operand dst;
    struct Operand src;
    struct token *error_token = NULL;
    nint size;
    nint prefix_len;
    nint tail;
    nint status;
    unsigned char opcode;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }
    error_token = NULL;
    status = parse_operand(p, inner->args_tail, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    status = shift_target_size(p, inner, &dst, &size);
    if(status <= 0) {
        if(status == INSTRUCTION_FAILED) return INSTRUCTION_FAILED;
        _error_from_token(p, inner->name, ERROR_TYPE_I386, "unsupported shift form");
        return INSTRUCTION_FAILED;
    }

    if(size == 4) return encode_shift_group(p, inner, &dst, &src, group);

    if(src.kind == OPERAND_IMM) {
        if(!fits_u16(src.imm)) {
            _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "shift count does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }

        prefix_len = emit_o32_shift_prefixes(p, &dst);
        if(src.imm == 1) {
            opcode = size == 1 ? 0xd0 : 0xd1;
            emit_byte(p, opcode);
            tail = emit_shift_modrm(p, &dst, group);
            return prefix_len + 2 + tail;
        }

        opcode = size == 1 ? 0xc0 : 0xc1;
        emit_byte(p, opcode);
        tail = emit_shift_modrm(p, &dst, group);
        emit_byte(p, (unsigned char)(src.imm & 0xff));
        return prefix_len + 3 + tail;
    }

    if(is_cl(&src)) {
        prefix_len = emit_o32_shift_prefixes(p, &dst);
        opcode = size == 1 ? 0xd2 : 0xd3;
        emit_byte(p, opcode);
        tail = emit_shift_modrm(p, &dst, group);
        return prefix_len + 2 + tail;
    }

    _error_from_token(p, inner->name, ERROR_TYPE_I386, "unsupported shift form");
    return INSTRUCTION_FAILED;
}

static nint encode_prefixed_shift_group(struct Parser *p,
                                        struct AstInstruction *inner,
                                        nint group,
                                        unint force_o32,
                                        unint force_a16,
                                        unint force_a32,
                                        unint force_o16) {
    struct Operand dst;
    struct Operand src;
    struct token *error_token = NULL;
    nint size;
    nint tail;
    nint status;
    unint op_prefix;
    unsigned char opcode;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }
    error_token = NULL;
    status = parse_operand(p, inner->args_tail, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    if(force_a32) force_direct_memory_a32(&dst);
    if(force_a16 && operand_is_mem32(&dst)) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "invalid a16 address form");
        return INSTRUCTION_FAILED;
    }
    if(force_a32 && operand_is_mem16(&dst)) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "invalid a32 address form");
        return INSTRUCTION_FAILED;
    }

    status = shift_target_size(p, inner, &dst, &size);
    if(status <= 0) {
        if(status == INSTRUCTION_FAILED) return INSTRUCTION_FAILED;
        _error_from_token(p, inner->name, ERROR_TYPE_I386, "unsupported shift form");
        return INSTRUCTION_FAILED;
    }

    op_prefix = force_o32 || (size == 4 && !force_o16);

    if(src.kind == OPERAND_IMM) {
        if(!fits_u16(src.imm)) {
            _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "shift count does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }

        if(dst.kind == OPERAND_MEM) emit_segment_prefix(p, &dst.mem);
        if(op_prefix) emit_operand_size_prefix(p, 4);
        if(dst.kind == OPERAND_MEM) emit_address_size_prefix(p, &dst.mem);
        else if(force_a32) emit_byte(p, 0x67);

        opcode = src.imm == 1 ? (size == 1 ? 0xd0 : 0xd1)
                              : (size == 1 ? 0xc0 : 0xc1);
        emit_byte(p, opcode);
        tail = emit_shift_modrm(p, &dst, group);
        if(src.imm != 1) emit_byte(p, (unsigned char)(src.imm & 0xff));

        return (dst.kind == OPERAND_MEM ? memory_prefix_len(&dst.mem) : 0) +
               (op_prefix ? 1 : 0) +
               (dst.kind != OPERAND_MEM && force_a32 ? 1 : 0) +
               2 + tail + (src.imm == 1 ? 0 : 1);
    }

    if(is_cl(&src)) {
        if(dst.kind == OPERAND_MEM) emit_segment_prefix(p, &dst.mem);
        if(op_prefix) emit_operand_size_prefix(p, 4);
        if(dst.kind == OPERAND_MEM) emit_address_size_prefix(p, &dst.mem);
        else if(force_a32) emit_byte(p, 0x67);

        opcode = size == 1 ? 0xd2 : 0xd3;
        emit_byte(p, opcode);
        tail = emit_shift_modrm(p, &dst, group);
        return (dst.kind == OPERAND_MEM ? memory_prefix_len(&dst.mem) : 0) +
               (op_prefix ? 1 : 0) +
               (dst.kind != OPERAND_MEM && force_a32 ? 1 : 0) +
               2 + tail;
    }

    _error_from_token(p, inner->name, ERROR_TYPE_I386, "unsupported shift form");
    return INSTRUCTION_FAILED;
}

static nint double_shift_target_size(struct Parser *p,
                                     struct AstInstruction *inst,
                                     struct Operand *dst,
                                     nint source_size,
                                     const char *name,
                                     nint *size) {
    if(dst->kind == OPERAND_REG16) {
        if(source_size != 2) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid %s operand size", name);
            return INSTRUCTION_FAILED;
        }
        *size = 2;
        return 1;
    }

    if(dst->kind == OPERAND_REG32) {
        if(source_size != 4) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid %s operand size", name);
            return INSTRUCTION_FAILED;
        }
        *size = 4;
        return 1;
    }

    if(dst->kind == OPERAND_MEM) {
        if(dst->size == 0) dst->size = source_size;
        if(dst->size == source_size && (dst->size == 2 || dst->size == 4)) {
            *size = dst->size;
            return 1;
        }
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "invalid %s operand size", name);
        return INSTRUCTION_FAILED;
    }

    return 0;
}

static nint encode_prefixed_double_shift(struct Parser *p,
                                         struct AstInstruction *inner,
                                         unsigned char imm_opcode,
                                         unsigned char cl_opcode,
                                         const char *name,
                                         unint force_o32,
                                         unint force_a16,
                                         unint force_a32,
                                         unint force_o16) {
    struct Operand dst;
    struct Operand src;
    struct Operand count;
    struct InstructionArg *src_arg;
    struct InstructionArg *count_arg;
    struct token *error_token = NULL;
    nint source_size;
    nint size;
    nint tail;
    nint status;
    nint prefix_len;
    unint op_prefix;
    unsigned char opcode;

    if(inner->arg_count != 3) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    src_arg = inner->args_head->next;
    count_arg = src_arg->next;

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }
    error_token = NULL;
    status = parse_operand(p, src_arg, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }
    error_token = NULL;
    status = parse_operand(p, count_arg, &count, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid third operand");
        return INSTRUCTION_FAILED;
    }

    if(src.kind == OPERAND_REG16) source_size = 2;
    else if(src.kind == OPERAND_REG32) source_size = 4;
    else {
        _error_from_token(p, src_arg->_s, ERROR_TYPE_I386,
                          "invalid %s source operand", name);
        return INSTRUCTION_FAILED;
    }

    if(force_a32) force_direct_memory_a32(&dst);
    if(force_a16 && operand_is_mem32(&dst)) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "invalid a16 address form");
        return INSTRUCTION_FAILED;
    }
    if(force_a32 && operand_is_mem16(&dst)) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "invalid a32 address form");
        return INSTRUCTION_FAILED;
    }

    status = double_shift_target_size(p, inner, &dst, source_size, name, &size);
    if(status <= 0) {
        if(status == INSTRUCTION_FAILED) return INSTRUCTION_FAILED;
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "unsupported %s form", name);
        return INSTRUCTION_FAILED;
    }

    if(count.kind == OPERAND_IMM) {
        if(!fits_u16(count.imm)) {
            _error_from_token(p, count_arg->_s, ERROR_TYPE_OVERFLOW,
                              "shift count does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        opcode = imm_opcode;
    }
    else if(is_cl(&count)) {
        opcode = cl_opcode;
    }
    else {
        _error_from_token(p, count_arg->_s, ERROR_TYPE_I386,
                          "invalid %s count operand", name);
        return INSTRUCTION_FAILED;
    }

    op_prefix = force_o32 || (size == 4 && !force_o16);
    if(dst.kind == OPERAND_MEM) {
        emit_segment_prefix(p, &dst.mem);
        if(op_prefix) emit_operand_size_prefix(p, 4);
        emit_address_size_prefix(p, &dst.mem);
        prefix_len = memory_prefix_len(&dst.mem) + (op_prefix ? 1 : 0);
    }
    else {
        if(op_prefix) emit_operand_size_prefix(p, 4);
        if(force_a32) emit_byte(p, 0x67);
        prefix_len = (op_prefix ? 1 : 0) + (force_a32 ? 1 : 0);
    }

    emit_byte(p, 0x0f);
    emit_byte(p, opcode);
    if(dst.kind == OPERAND_MEM) {
        emit_modrm_mem(p, src.reg, &dst.mem);
        tail = memory_tail_len(&dst.mem);
    }
    else {
        emit_modrm_reg(p, src.reg, dst.reg);
        tail = 0;
    }

    if(count.kind == OPERAND_IMM) {
        emit_byte(p, (unsigned char)(count.imm & 0xff));
        return prefix_len + 3 + tail + 1;
    }

    return prefix_len + 3 + tail;
}

static nint encode_o32_unary_group(struct Parser *p, struct AstInstruction *inner,
                                   nint group, const char *name) {
    struct Operand dst;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count != 1) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid operand");
        return INSTRUCTION_FAILED;
    }

    if(dst.kind == OPERAND_REG32 ||
       (dst.kind == OPERAND_MEM && dst.size == 4)) {
        return encode_unary_group(p, inner, &dst, group, name);
    }

    if(dst.kind == OPERAND_REG8) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0xf6);
        emit_modrm_reg(p, group, dst.reg);
        return 3;
    }

    if(dst.kind == OPERAND_REG16) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0xf7);
        emit_modrm_reg(p, group, dst.reg);
        return 3;
    }

    if(dst.kind == OPERAND_MEM) {
        if(dst.size == 1) {
            emit_forced_o32_memory_prefixes(p, &dst.mem);
            emit_byte(p, 0xf6);
            emit_modrm_mem(p, group, &dst.mem);
            return forced_o32_memory_prefix_len(&dst.mem) + 2 +
                   memory_tail_len(&dst.mem);
        }
        if(dst.size == 2) {
            emit_forced_o32_memory_prefixes(p, &dst.mem);
            emit_byte(p, 0xf7);
            emit_modrm_mem(p, group, &dst.mem);
            return forced_o32_memory_prefix_len(&dst.mem) + 2 +
                   memory_tail_len(&dst.mem);
        }
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "ambiguous %s operand size", name);
        return INSTRUCTION_FAILED;
    }

    _error_from_token(p, inner->name, ERROR_TYPE_I386,
                      "unsupported %s form", name);
    return INSTRUCTION_FAILED;
}

static nint encode_esc(struct Parser *p, struct AstInstruction *inst,
                       struct Operand *op, struct Operand *rm) {
    unsigned char opcode;
    nint group;

    if(op->kind != OPERAND_IMM) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "invalid escape opcode");
        return INSTRUCTION_FAILED;
    }

    if(op->imm < 0 || op->imm > 63) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_OVERFLOW,
                          "escape opcode does not fit 6 bits");
        return INSTRUCTION_FAILED;
    }

    opcode = (unsigned char)(0xd8 + ((op->imm >> 3) & 7));
    group = op->imm & 7;

    if(rm->kind == OPERAND_MEM) {
        emit_memory_prefixes(p, &rm->mem);
        emit_byte(p, opcode);
        emit_modrm_mem(p, group, &rm->mem);
        return memory_prefix_len(&rm->mem) + 2 + memory_tail_len(&rm->mem);
    }

    if(rm->kind == OPERAND_REG16) {
        emit_byte(p, opcode);
        emit_modrm_reg(p, group, rm->reg);
        return 2;
    }

    _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                      "unsupported esc form");
    return INSTRUCTION_FAILED;
}

static nint encode_prefixed_esc(struct Parser *p,
                                struct AstInstruction *inner,
                                unint force_o32,
                                unint force_a16,
                                unint force_a32) {
    struct Operand op;
    struct Operand rm;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &op, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }

    error_token = NULL;
    status = parse_operand(p, inner->args_tail, &rm, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    if(force_a32) force_direct_memory_a32(&rm);
    if(force_a16 && operand_is_mem32(&rm)) {
        _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_I386,
                          "invalid a16 address form");
        return INSTRUCTION_FAILED;
    }
    if(force_a32 && operand_is_mem16(&rm)) {
        _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_I386,
                          "invalid a32 address form");
        return INSTRUCTION_FAILED;
    }

    if(force_o32) emit_operand_size_prefix(p, 4);
    status = encode_esc(p, inner, &op, &rm);
    if(status < 0) return status;
    return status + (force_o32 ? 1 : 0);
}

static nint append_prefix_arg(struct AstInstruction *inner,
                              struct InstructionArg *args,
                              struct token *start, struct token *end) {
    struct InstructionArg *slot;
    if(inner->arg_count >= 3 || !start || !end || start > end) return 0;
    slot = &args[inner->arg_count];
    slot->_s = start;
    slot->_e = end;
    slot->next = NULL;
    if(inner->args_tail) inner->args_tail->next = slot;
    else inner->args_head = slot;
    inner->args_tail = slot;
    inner->arg_count++;
    return 1;
}

static nint build_prefixed_instruction(struct Parser *p,
                                       struct AstInstruction *inst,
                                       struct AstInstruction *inner,
                                       struct InstructionArg *inner_args) {
    struct InstructionArg *first = inst->args_head;
    struct InstructionArg *src;
    struct token *error_token;

    if(inst->arg_count == 0 || !first) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if(!first->_s || !first->_e || first->_s > first->_e) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386,
                          "invalid prefixed instruction");
        return INSTRUCTION_FAILED;
    }

    if(first->_s->type != NAME) {
        _error_from_token(p, first->_s, ERROR_TYPE_I386,
                          "expected instruction after prefix");
        return INSTRUCTION_FAILED;
    }

    inner->name = first->_s;
    inner->arg_count = 0;
    inner->args_head = NULL;
    inner->args_tail = NULL;

    if(first->_s < first->_e) {
        error_token = first->_s + 1;
        if(!append_prefix_arg(inner, inner_args, error_token, first->_e)) {
            _error_from_token(p, error_token, ERROR_TYPE_I386,
                              "invalid prefixed instruction");
            return INSTRUCTION_FAILED;
        }
    }

    src = first->next;
    while(src) {
        error_token = src->_s ? src->_s : inner->name;
        if(!append_prefix_arg(inner, inner_args, src->_s, src->_e)) {
            _error_from_token(p, error_token, ERROR_TYPE_I386,
                              "invalid number of arguments");
            return INSTRUCTION_FAILED;
        }
        src = src->next;
    }

    return 1;
}

static unint rep_allows_instruction(struct token *name) {
    return token_is(name, MOVS_INSTRUCTION) || token_is(name, MOVSB_INSTRUCTION) ||
           token_is(name, MOVSW_INSTRUCTION) || token_is(name, MOVSD_INSTRUCTION) ||
           token_is(name, CMPS_INSTRUCTION) || token_is(name, CMPSB_INSTRUCTION) ||
           token_is(name, CMPSW_INSTRUCTION) || token_is(name, CMPSD_INSTRUCTION) ||
           token_is(name, SCAS_INSTRUCTION) || token_is(name, SCASB_INSTRUCTION) ||
           token_is(name, SCASW_INSTRUCTION) || token_is(name, SCASD_INSTRUCTION) ||
           token_is(name, LODS_INSTRUCTION) || token_is(name, LODSB_INSTRUCTION) ||
           token_is(name, LODSW_INSTRUCTION) || token_is(name, LODSD_INSTRUCTION) ||
           token_is(name, STOS_INSTRUCTION) || token_is(name, STOSB_INSTRUCTION) ||
           token_is(name, STOSW_INSTRUCTION) || token_is(name, STOSD_INSTRUCTION) ||
           token_is(name, INSB_INSTRUCTION) || token_is(name, INSW_INSTRUCTION) ||
           token_is(name, INSD_INSTRUCTION) ||
           token_is(name, OUTSB_INSTRUCTION) || token_is(name, OUTSW_INSTRUCTION) ||
           token_is(name, OUTSD_INSTRUCTION);
}

static unint repe_allows_instruction(struct token *name) {
    return token_is(name, CMPS_INSTRUCTION) || token_is(name, CMPSB_INSTRUCTION) ||
           token_is(name, CMPSW_INSTRUCTION) || token_is(name, CMPSD_INSTRUCTION) ||
           token_is(name, SCAS_INSTRUCTION) ||
           token_is(name, SCASB_INSTRUCTION) || token_is(name, SCASW_INSTRUCTION) ||
           token_is(name, SCASD_INSTRUCTION);
}

static unint repeat_operand_prefix_name(struct token *name) {
    return token_is(name, O16_INSTRUCTION) || token_is(name, O32_INSTRUCTION) ||
           token_is(name, A16_INSTRUCTION) || token_is(name, A32_INSTRUCTION);
}

static nint repeat_target_allowed(struct Parser *p,
                                  struct AstInstruction *inner,
                                  unint (*allowed)(struct token *),
                                  const char *message) {
    struct AstInstruction nested;
    struct InstructionArg nested_args[3];
    nint status;

    if(repeat_operand_prefix_name(inner->name)) {
        status = build_prefixed_instruction(p, inner, &nested, nested_args);
        if(status <= 0) return status;
        return repeat_target_allowed(p, &nested, allowed, message);
    }

    if(!allowed(inner->name)) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386, message);
        return INSTRUCTION_FAILED;
    }

    if(inner->arg_count != 0) {
        _error_from_token(p, inner->args_head ? inner->args_head->_s : inner->name,
                          ERROR_TYPE_I386, "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    return 1;
}

static nint encode_repeat_prefix(struct Parser *p, struct AstInstruction *inst,
                                 unint (*allowed)(struct token *),
                                 const char *message,
                                 unsigned char prefix) {
    struct AstInstruction inner;
    struct InstructionArg inner_args[3];
    nint status = build_prefixed_instruction(p, inst, &inner, inner_args);
    if(status <= 0) return status;

    status = repeat_target_allowed(p, &inner, allowed, message);
    if(status <= 0) return status;

    emit_byte(p, prefix);
    status = _86_exec(p, &inner);
    if(status < 0) return status;
    return status + 1;
}

static nint encode_rep_prefix(struct Parser *p, struct AstInstruction *inst) {
    return encode_repeat_prefix(p, inst, rep_allows_instruction,
                                "unsupported rep instruction", 0xf3);
}

static nint encode_repe_prefix(struct Parser *p, struct AstInstruction *inst) {
    return encode_repeat_prefix(p, inst, repe_allows_instruction,
                                "unsupported repe instruction", 0xf3);
}

static nint encode_repne_prefix(struct Parser *p, struct AstInstruction *inst) {
    return encode_repeat_prefix(p, inst, repe_allows_instruction,
                                "unsupported repne instruction", 0xf2);
}

static nint read_o32_relative_target(struct Parser *p, struct token *start,
                                     struct token *end, nint *target,
                                     unint *unresolved,
                                     struct token **error_token) {
    struct TokenStream tks;
    unint is_variable = 0;
    nint status;

    tks_init(&tks, start, end);
    status = read_jump_scalar(p, &tks, target, unresolved,
                              &is_variable, error_token);
    if(status <= 0 || !expect_tks_end(&tks, error_token)) return 0;
    (void)is_variable;
    return 1;
}

static nint emit_o32_rel8(struct Parser *p, nint target,
                          unsigned char opcode, struct token *tok) {
    nint disp = target - (p->addr + 3);
    if(!fits_i8(disp)) {
        _error_from_token(p, tok, ERROR_TYPE_OVERFLOW,
                          "short target out of range");
        return INSTRUCTION_FAILED;
    }
    emit_operand_size_prefix(p, 4);
    emit_byte(p, opcode);
    emit_byte(p, (unsigned char)(disp & 0xff));
    return 3;
}

static nint emit_o32_rel32(struct Parser *p, nint target,
                           unsigned char opcode, struct token *tok) {
    nint disp = target - (p->addr + 6);
    if(!fits_i32(disp)) {
        _error_from_token(p, tok, ERROR_TYPE_OVERFLOW,
                          "near target out of range");
        return INSTRUCTION_FAILED;
    }
    emit_operand_size_prefix(p, 4);
    emit_byte(p, opcode);
    emit_dword(p, disp);
    return 6;
}

static nint emit_o32_jcc_rel32(struct Parser *p, nint target,
                               unsigned char near_opcode,
                               struct token *tok) {
    nint disp = target - (p->addr + 7);
    if(!fits_i32(disp)) {
        _error_from_token(p, tok, ERROR_TYPE_OVERFLOW,
                          "near branch target out of range");
        return INSTRUCTION_FAILED;
    }
    emit_operand_size_prefix(p, 4);
    emit_byte(p, 0x0f);
    emit_byte(p, near_opcode);
    emit_dword(p, disp);
    return 7;
}

static unint jcc_opcodes_from_name(struct token *name,
                                   unsigned char *short_opcode,
                                   unsigned char *near_opcode) {
    if(token_is(name, JO_INSTRUCTION)) {
        *short_opcode = 0x70; *near_opcode = 0x80; return 1;
    }
    if(token_is(name, JNO_INSTRUCTION)) {
        *short_opcode = 0x71; *near_opcode = 0x81; return 1;
    }
    if(token_is(name, JB_INSTRUCTION) || token_is(name, JC_INSTRUCTION) ||
       token_is(name, JNAE_INSTRUCTION)) {
        *short_opcode = 0x72; *near_opcode = 0x82; return 1;
    }
    if(token_is(name, JNB_INSTRUCTION) || token_is(name, JAE_INSTRUCTION) ||
       token_is(name, JNC_INSTRUCTION)) {
        *short_opcode = 0x73; *near_opcode = 0x83; return 1;
    }
    if(token_is(name, JE_INSTRUCTION) || token_is(name, JZ_INSTRUCTION)) {
        *short_opcode = 0x74; *near_opcode = 0x84; return 1;
    }
    if(token_is(name, JNE_INSTRUCTION) || token_is(name, JNZ_INSTRUCTION)) {
        *short_opcode = 0x75; *near_opcode = 0x85; return 1;
    }
    if(token_is(name, JBE_INSTRUCTION) || token_is(name, JNA_INSTRUCTION)) {
        *short_opcode = 0x76; *near_opcode = 0x86; return 1;
    }
    if(token_is(name, JA_INSTRUCTION) || token_is(name, JNBE_INSTRUCTION)) {
        *short_opcode = 0x77; *near_opcode = 0x87; return 1;
    }
    if(token_is(name, JS_INSTRUCTION)) {
        *short_opcode = 0x78; *near_opcode = 0x88; return 1;
    }
    if(token_is(name, JNS_INSTRUCTION)) {
        *short_opcode = 0x79; *near_opcode = 0x89; return 1;
    }
    if(token_is(name, JP_INSTRUCTION) || token_is(name, JPE_INSTRUCTION)) {
        *short_opcode = 0x7a; *near_opcode = 0x8a; return 1;
    }
    if(token_is(name, JNP_INSTRUCTION) || token_is(name, JPO_INSTRUCTION)) {
        *short_opcode = 0x7b; *near_opcode = 0x8b; return 1;
    }
    if(token_is(name, JL_INSTRUCTION) || token_is(name, JNGE_INSTRUCTION)) {
        *short_opcode = 0x7c; *near_opcode = 0x8c; return 1;
    }
    if(token_is(name, JGE_INSTRUCTION) || token_is(name, JNL_INSTRUCTION)) {
        *short_opcode = 0x7d; *near_opcode = 0x8d; return 1;
    }
    if(token_is(name, JLE_INSTRUCTION) || token_is(name, JNG_INSTRUCTION)) {
        *short_opcode = 0x7e; *near_opcode = 0x8e; return 1;
    }
    if(token_is(name, JG_INSTRUCTION) || token_is(name, JNLE_INSTRUCTION)) {
        *short_opcode = 0x7f; *near_opcode = 0x8f; return 1;
    }
    return 0;
}

static nint encode_o32_jmp(struct Parser *p, struct AstInstruction *inner) {
    struct TokenStream tks;
    struct token *tok;
    struct token *target_start;
    struct token *error_token = NULL;
    nint target = 0;
    unint unresolved = 0;
    nint is_short = 0;

    if(inner->arg_count != 1) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    tks_init(&tks, inner->args_head->_s, inner->args_head->_e);
    tok = tks_read(&tks);
    if(!tok) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386, "invalid jump target");
        return INSTRUCTION_FAILED;
    }

    target_start = tok;
    if(token_is(tok, SHORT_TYPE)) {
        is_short = 1;
        target_start = tks_read(&tks);
        if(!target_start) {
            _error_from_token(p, tok, ERROR_TYPE_I386, "invalid jump target");
            return INSTRUCTION_FAILED;
        }
    } else if(token_is(tok, NEAR_TYPE)) {
        target_start = tks_read(&tks);
        if(!target_start) {
            _error_from_token(p, tok, ERROR_TYPE_I386, "invalid jump target");
            return INSTRUCTION_FAILED;
        }
    } else if(token_is(tok, FAR_TYPE)) {
        _error_from_token(p, tok, ERROR_TYPE_I386, "unsupported o32 jump form");
        return INSTRUCTION_FAILED;
    }

    if(!read_o32_relative_target(p, target_start, inner->args_head->_e,
                                 &target, &unresolved, &error_token)) {
        _error_from_token(p, error_token ? error_token : target_start,
                          ERROR_TYPE_I386, "invalid jump target");
        return INSTRUCTION_FAILED;
    }

    if(unresolved && !p->last_pass) return is_short ? 3 : 6;
    if(is_short) return emit_o32_rel8(p, target, 0xeb, target_start);
    return emit_o32_rel32(p, target, 0xe9, target_start);
}

static nint encode_o32_call(struct Parser *p, struct AstInstruction *inner) {
    struct TokenStream tks;
    struct token *tok;
    struct token *target_start;
    struct token *error_token = NULL;
    nint target = 0;
    unint unresolved = 0;

    if(inner->arg_count != 1) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    tks_init(&tks, inner->args_head->_s, inner->args_head->_e);
    tok = tks_read(&tks);
    if(!tok) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386, "invalid call target");
        return INSTRUCTION_FAILED;
    }

    target_start = tok;
    if(token_is(tok, SHORT_TYPE) || token_is(tok, FAR_TYPE)) {
        _error_from_token(p, tok, ERROR_TYPE_I386, "invalid call target size");
        return INSTRUCTION_FAILED;
    }
    if(token_is(tok, NEAR_TYPE)) {
        target_start = tks_read(&tks);
        if(!target_start) {
            _error_from_token(p, tok, ERROR_TYPE_I386, "invalid call target");
            return INSTRUCTION_FAILED;
        }
    }

    if(!read_o32_relative_target(p, target_start, inner->args_head->_e,
                                 &target, &unresolved, &error_token)) {
        _error_from_token(p, error_token ? error_token : target_start,
                          ERROR_TYPE_I386, "invalid call target");
        return INSTRUCTION_FAILED;
    }

    if(unresolved && !p->last_pass) return 6;
    return emit_o32_rel32(p, target, 0xe8, target_start);
}

static nint encode_o32_jcc(struct Parser *p, struct AstInstruction *inner,
                           unsigned char short_opcode,
                           unsigned char near_opcode) {
    struct TokenStream tks;
    struct token *tok;
    struct token *target_start;
    struct token *error_token = NULL;
    nint target = 0;
    unint unresolved = 0;
    nint is_short = 0;

    if(inner->arg_count != 1) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    tks_init(&tks, inner->args_head->_s, inner->args_head->_e);
    tok = tks_read(&tks);
    if(!tok) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386, "invalid branch target");
        return INSTRUCTION_FAILED;
    }

    target_start = tok;
    if(token_is(tok, SHORT_TYPE)) {
        is_short = 1;
        target_start = tks_read(&tks);
        if(!target_start) {
            _error_from_token(p, tok, ERROR_TYPE_I386, "invalid branch target");
            return INSTRUCTION_FAILED;
        }
    } else if(token_is(tok, NEAR_TYPE)) {
        target_start = tks_read(&tks);
        if(!target_start) {
            _error_from_token(p, tok, ERROR_TYPE_I386, "invalid branch target");
            return INSTRUCTION_FAILED;
        }
    }

    if(!read_o32_relative_target(p, target_start, inner->args_head->_e,
                                 &target, &unresolved, &error_token)) {
        _error_from_token(p, error_token ? error_token : target_start,
                          ERROR_TYPE_I386, "invalid branch target");
        return INSTRUCTION_FAILED;
    }

    if(unresolved && !p->last_pass) return is_short ? 3 : 7;
    if(is_short) return emit_o32_rel8(p, target, short_opcode, target_start);
    return emit_o32_jcc_rel32(p, target, near_opcode, target_start);
}

static nint emit_prefixed_jcc_rel8(struct Parser *p, nint target,
                                   unsigned char opcode,
                                   struct token *tok,
                                   unint force_o32,
                                   unint force_a32) {
    nint len = 2 + (force_o32 ? 1 : 0) + (force_a32 ? 1 : 0);
    nint disp = target - (p->addr + len);
    if(!fits_i8(disp)) {
        _error_from_token(p, tok, ERROR_TYPE_OVERFLOW,
                          "short target out of range");
        return INSTRUCTION_FAILED;
    }
    if(force_o32) emit_operand_size_prefix(p, 4);
    if(force_a32) emit_byte(p, 0x67);
    emit_byte(p, opcode);
    emit_byte(p, (unsigned char)(disp & 0xff));
    return len;
}

static nint emit_prefixed_jcc_near(struct Parser *p, nint target,
                                   unsigned char near_opcode,
                                   struct token *tok,
                                   unint operand_prefix,
                                   unint rel32,
                                   unint force_a32) {
    nint len = (rel32 ? 6 : 4) + (operand_prefix ? 1 : 0) +
               (force_a32 ? 1 : 0);
    nint disp = target - (p->addr + len);

    if(rel32) {
        if(!fits_i32(disp)) {
            _error_from_token(p, tok, ERROR_TYPE_OVERFLOW,
                              "near branch target out of range");
            return INSTRUCTION_FAILED;
        }
        if(operand_prefix) emit_operand_size_prefix(p, 4);
        if(force_a32) emit_byte(p, 0x67);
        emit_byte(p, 0x0f);
        emit_byte(p, near_opcode);
        emit_dword(p, disp);
        return len;
    }

    if(!fits_i16(disp)) {
        _error_from_token(p, tok, ERROR_TYPE_OVERFLOW,
                          "near branch target out of range");
        return INSTRUCTION_FAILED;
    }
    if(operand_prefix) emit_operand_size_prefix(p, 4);
    if(force_a32) emit_byte(p, 0x67);
    emit_byte(p, 0x0f);
    emit_byte(p, near_opcode);
    emit_word(p, disp);
    return len;
}

static nint encode_prefixed_jcc(struct Parser *p,
                                struct AstInstruction *inner,
                                unsigned char short_opcode,
                                unsigned char near_opcode,
                                unint force_o32,
                                unint force_a16,
                                unint force_a32) {
    struct TokenStream tks;
    struct token *tok;
    struct token *target_start;
    struct token *error_token = NULL;
    nint target = 0;
    unint unresolved = 0;
    nint is_short = 0;
    nint is_near = 0;
    nint size;
    unint rel32 = force_o32 && !force_a16 && !force_a32;

    if(inner->arg_count != 1) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    tks_init(&tks, inner->args_head->_s, inner->args_head->_e);
    tok = tks_read(&tks);
    if(!tok) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid branch target");
        return INSTRUCTION_FAILED;
    }

    target_start = tok;
    if(token_is(tok, SHORT_TYPE)) {
        is_short = 1;
        target_start = tks_read(&tks);
        if(!target_start) {
            _error_from_token(p, tok, ERROR_TYPE_I386,
                              "invalid branch target");
            return INSTRUCTION_FAILED;
        }
    } else if(token_is(tok, NEAR_TYPE)) {
        is_near = 1;
        target_start = tks_read(&tks);
        if(!target_start) {
            _error_from_token(p, tok, ERROR_TYPE_I386,
                              "invalid branch target");
            return INSTRUCTION_FAILED;
        }
    }

    if(!read_o32_relative_target(p, target_start, inner->args_head->_e,
                                 &target, &unresolved, &error_token)) {
        _error_from_token(p, error_token ? error_token : target_start,
                          ERROR_TYPE_I386, "invalid branch target");
        return INSTRUCTION_FAILED;
    }

    if(!is_short && !is_near && !force_o32) is_short = 1;
    size = (is_short ? 2 : ((rel32 ? 6 : 4) + (force_o32 ? 1 : 0))) +
           (force_a32 ? 1 : 0);
    if(unresolved && !p->last_pass) return size;
    if(is_short) {
        return emit_prefixed_jcc_rel8(p, target, short_opcode, target_start,
                                      force_o32, force_a32);
    }
    return emit_prefixed_jcc_near(p, target, near_opcode, target_start,
                                  force_o32, rel32, force_a32);
}

static nint encode_prefixed_rel8_control(struct Parser *p,
                                         struct AstInstruction *inner,
                                         unsigned char opcode,
                                         unint operand_prefix,
                                         unint address_prefix) {
    struct TokenStream tks;
    struct token *error_token = NULL;
    nint target = 0;
    nint len = 2 + (operand_prefix ? 1 : 0) + (address_prefix ? 1 : 0);
    nint disp;
    unint unresolved = 0;
    unint is_variable = 0;
    nint status;

    if(inner->arg_count != 1) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    tks_init(&tks, inner->args_head->_s, inner->args_head->_e);
    status = read_jump_scalar(p, &tks, &target, &unresolved,
                              &is_variable, &error_token);
    if(status <= 0 || !expect_tks_end(&tks, &error_token)) {
        _error_from_token(p, error_token ? error_token : inner->args_head->_s,
                          ERROR_TYPE_I386, "invalid branch target");
        return INSTRUCTION_FAILED;
    }

    if(unresolved && !p->last_pass) return len;
    (void)is_variable;
    disp = target - (p->addr + len);
    if(!fits_i8(disp)) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_OVERFLOW,
                          "branch target out of range");
        return INSTRUCTION_FAILED;
    }

    if(operand_prefix) emit_operand_size_prefix(p, 4);
    if(address_prefix) emit_byte(p, 0x67);
    emit_byte(p, opcode);
    emit_byte(p, (unsigned char)(disp & 0xff));
    return len;
}

static nint emit_prefixed_jmp_rel8(struct Parser *p, nint target,
                                   struct token *tok,
                                   unint operand_prefix,
                                   unint address_prefix) {
    nint len = 2 + (operand_prefix ? 1 : 0) + (address_prefix ? 1 : 0);
    nint disp = target - (p->addr + len);
    if(!fits_i8(disp)) {
        _error_from_token(p, tok, ERROR_TYPE_OVERFLOW,
                          "short target out of range");
        return INSTRUCTION_FAILED;
    }
    if(operand_prefix) emit_operand_size_prefix(p, 4);
    if(address_prefix) emit_byte(p, 0x67);
    emit_byte(p, 0xeb);
    emit_byte(p, (unsigned char)(disp & 0xff));
    return len;
}

static nint emit_prefixed_jmp_near(struct Parser *p, nint target,
                                   struct token *tok,
                                   unint operand_prefix,
                                   unint rel32,
                                   unint address_prefix) {
    nint len = (rel32 ? 5 : 3) + (operand_prefix ? 1 : 0) +
               (address_prefix ? 1 : 0);
    nint disp = target - (p->addr + len);

    if(rel32) {
        if(!fits_i32(disp)) {
            _error_from_token(p, tok, ERROR_TYPE_OVERFLOW,
                              "near target out of range");
            return INSTRUCTION_FAILED;
        }
        if(operand_prefix) emit_operand_size_prefix(p, 4);
        if(address_prefix) emit_byte(p, 0x67);
        emit_byte(p, 0xe9);
        emit_dword(p, disp);
        return len;
    }

    if(!fits_i16(disp)) {
        _error_from_token(p, tok, ERROR_TYPE_OVERFLOW,
                          "near target out of range");
        return INSTRUCTION_FAILED;
    }
    if(operand_prefix) emit_operand_size_prefix(p, 4);
    if(address_prefix) emit_byte(p, 0x67);
    emit_byte(p, 0xe9);
    emit_word(p, disp);
    return len;
}

static nint encode_prefixed_jmp_direct(struct Parser *p,
                                       struct AstInstruction *inner,
                                       struct token *start,
                                       struct token *end,
                                       nint explicit_size,
                                       unint force_o32,
                                       unint force_a16,
                                       unint force_a32) {
    struct TokenStream tks;
    struct token *error_token = NULL;
    nint target = 0;
    unint unresolved = 0;
    unint is_variable = 0;
    unint rel32 = force_o32 && !force_a16 && !force_a32;
    nint size = explicit_size;
    nint status;

    (void)inner;

    tks_init(&tks, start, end);
    status = read_jump_scalar(p, &tks, &target, &unresolved,
                              &is_variable, &error_token);
    if(status <= 0 || !expect_tks_end(&tks, &error_token)) {
        _error_from_token(p, error_token ? error_token : start,
                          ERROR_TYPE_I386, "invalid jump target");
        return INSTRUCTION_FAILED;
    }

    if(!size) {
        if(force_o32) size = rel32 ? 6 : 4;
        else size = 2;
    }
    if(size == 3 && rel32) size = 6;

    if(unresolved && !p->last_pass) {
        return size + (force_a32 ? 1 : 0);
    }
    (void)is_variable;

    if(size == 2) {
        return emit_prefixed_jmp_rel8(p, target, start, force_o32, force_a32);
    }
    return emit_prefixed_jmp_near(p, target, start, force_o32, rel32,
                                  force_a32);
}

static nint encode_prefixed_jmp_ptr_mem(struct Parser *p,
                                        struct token *start,
                                        struct token *end,
                                        nint far_ptr,
                                        unint force_o32,
                                        unint force_a16,
                                        unint force_a32) {
    struct Operand mem;
    struct token *error_token = NULL;
    nint status = parse_jump_memory_from(p, start, end, &mem, &error_token);
    unint operand_prefix;

    if(status <= 0) {
        _error_from_token(p, error_token ? error_token : start,
                          ERROR_TYPE_I386, "invalid jump pointer");
        return INSTRUCTION_FAILED;
    }

    if(force_a32) force_direct_memory_a32(&mem);
    if(force_a16 && operand_is_mem32(&mem)) {
        _error_from_token(p, start, ERROR_TYPE_I386, "invalid a16 address form");
        return INSTRUCTION_FAILED;
    }
    if(force_a32 && operand_is_mem16(&mem)) {
        _error_from_token(p, start, ERROR_TYPE_I386, "invalid a32 address form");
        return INSTRUCTION_FAILED;
    }

    operand_prefix = force_o32 || mem.size == 4;
    if(operand_prefix) emit_operand_size_prefix(p, 4);
    emit_memory_prefixes(p, &mem.mem);
    emit_byte(p, 0xff);
    emit_modrm_mem(p, far_ptr ? 5 : 4, &mem.mem);
    return (operand_prefix ? 1 : 0) + memory_prefix_len(&mem.mem) + 2 +
           memory_tail_len(&mem.mem);
}

static nint encode_prefixed_jmp_indirect(struct Parser *p,
                                         struct AstInstruction *inner,
                                         unint force_o32,
                                         unint force_a16,
                                         unint force_a32,
                                         unint force_o16) {
    struct Operand dst;
    struct token *error_token = NULL;
    nint status = parse_operand(p, inner->args_head, &dst, &error_token);

    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid jump target");
        return INSTRUCTION_FAILED;
    }

    if(force_a32) force_direct_memory_a32(&dst);
    if(force_a16 && operand_is_mem32(&dst)) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "invalid a16 address form");
        return INSTRUCTION_FAILED;
    }
    if(force_a32 && operand_is_mem16(&dst)) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "invalid a32 address form");
        return INSTRUCTION_FAILED;
    }

    if(dst.kind == OPERAND_REG16 || dst.kind == OPERAND_REG32) {
        if(!force_o16 && (force_o32 || dst.kind == OPERAND_REG32)) {
            emit_operand_size_prefix(p, 4);
        }
        if(force_a32) emit_byte(p, 0x67);
        emit_byte(p, 0xff);
        emit_modrm_reg(p, 4, dst.reg);
        return 2 + ((!force_o16 && (force_o32 || dst.kind == OPERAND_REG32)) ? 1 : 0) +
               (force_a32 ? 1 : 0);
    }

    if(dst.kind == OPERAND_MEM) {
        if(!force_o16 && (force_o32 || dst.size == 4)) {
            emit_operand_size_prefix(p, 4);
        }
        emit_memory_prefixes(p, &dst.mem);
        emit_byte(p, 0xff);
        emit_modrm_mem(p, 4, &dst.mem);
        return ((!force_o16 && (force_o32 || dst.size == 4)) ? 1 : 0) +
               memory_prefix_len(&dst.mem) + 2 + memory_tail_len(&dst.mem);
    }

    _error_from_token(p, inner->name, ERROR_TYPE_I386, "unsupported jmp form");
    return INSTRUCTION_FAILED;
}

static nint encode_prefixed_jmp(struct Parser *p,
                                struct AstInstruction *inner,
                                unint force_o32,
                                unint force_a16,
                                unint force_a32,
                                unint force_o16) {
    struct TokenStream tks;
    struct token *tok;
    struct token *target_start;

    if(inner->arg_count != 1) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    tks_init(&tks, inner->args_head->_s, inner->args_head->_e);
    tok = tks_read(&tks);
    if(!tok) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386, "invalid jump target");
        return INSTRUCTION_FAILED;
    }

    if(token_is(tok, SHORT_TYPE)) {
        target_start = tks_read(&tks);
        if(!target_start) {
            _error_from_token(p, tok, ERROR_TYPE_I386, "invalid jump target");
            return INSTRUCTION_FAILED;
        }
        return encode_prefixed_jmp_direct(p, inner, target_start,
                                          inner->args_head->_e, 2,
                                          force_o32, force_a16, force_a32);
    }

    if(token_is(tok, NEAR_TYPE)) {
        struct token *next = tks_read(&tks);
        if(!next) {
            _error_from_token(p, tok, ERROR_TYPE_I386, "invalid jump target");
            return INSTRUCTION_FAILED;
        }
        if(token_is(next, PTR_NAME)) {
            target_start = tks_read(&tks);
            if(!target_start) {
                _error_from_token(p, next, ERROR_TYPE_I386, "invalid jump pointer");
                return INSTRUCTION_FAILED;
            }
            return encode_prefixed_jmp_ptr_mem(p, target_start,
                                               inner->args_head->_e, 0,
                                               force_o32, force_a16, force_a32);
        }
        if(explicit_type_size(next)) {
            struct token *ptr_tok = tks_read(&tks);
            if(!ptr_tok || !token_is(ptr_tok, PTR_NAME)) {
                _error_from_token(p, next, ERROR_TYPE_I386, "expected ptr");
                return INSTRUCTION_FAILED;
            }
            target_start = tks_read(&tks);
            if(!target_start) {
                _error_from_token(p, ptr_tok, ERROR_TYPE_I386,
                                  "invalid jump pointer");
                return INSTRUCTION_FAILED;
            }
            return encode_prefixed_jmp_ptr_mem(p, next, inner->args_head->_e, 0,
                                               force_o32, force_a16, force_a32);
        }
        return encode_prefixed_jmp_direct(p, inner, next,
                                          inner->args_head->_e, 3,
                                          force_o32, force_a16, force_a32);
    }

    if(token_is(tok, FAR_TYPE)) return encode_jmp(p, inner);

    {
        struct Operand probe;
        struct token *error_token = NULL;
        nint status = parse_operand(p, inner->args_head, &probe, &error_token);
        if(status > 0 && probe.kind != OPERAND_IMM) {
            return encode_prefixed_jmp_indirect(p, inner, force_o32,
                                                force_a16, force_a32,
                                                force_o16);
        }
    }

    return encode_prefixed_jmp_direct(p, inner, inner->args_head->_s,
                                      inner->args_head->_e, 0,
                                      force_o32, force_a16, force_a32);
}

static nint emit_prefixed_zero_op_byte(struct Parser *p,
                                       struct AstInstruction *inner,
                                       unsigned char opcode,
                                       unint operand_prefix,
                                       unint address_prefix) {
    if(inner->arg_count != 0) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }
    if(operand_prefix) emit_operand_size_prefix(p, 4);
    if(address_prefix) emit_byte(p, 0x67);
    emit_byte(p, opcode);
    return 1 + (operand_prefix ? 1 : 0) + (address_prefix ? 1 : 0);
}

static nint emit_prefixed_zero_op_0f(struct Parser *p,
                                     struct AstInstruction *inner,
                                     unsigned char opcode,
                                     unint operand_prefix,
                                     unint address_prefix) {
    if(inner->arg_count != 0) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }
    if(operand_prefix) emit_operand_size_prefix(p, 4);
    if(address_prefix) emit_byte(p, 0x67);
    emit_byte(p, 0x0f);
    emit_byte(p, opcode);
    return 2 + (operand_prefix ? 1 : 0) + (address_prefix ? 1 : 0);
}

static nint emit_prefixed_flag_control(struct Parser *p,
                                       struct AstInstruction *inner,
                                       unint operand_prefix,
                                       unint address_prefix) {
    if(token_is(inner->name, CLC_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0xf8,
                                          operand_prefix, address_prefix);
    }
    if(token_is(inner->name, STC_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0xf9,
                                          operand_prefix, address_prefix);
    }
    if(token_is(inner->name, CLD_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0xfc,
                                          operand_prefix, address_prefix);
    }
    if(token_is(inner->name, STD_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0xfd,
                                          operand_prefix, address_prefix);
    }
    if(token_is(inner->name, CLI_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0xfa,
                                          operand_prefix, address_prefix);
    }
    if(token_is(inner->name, STI_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0xfb,
                                          operand_prefix, address_prefix);
    }
    if(token_is(inner->name, CMC_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0xf5,
                                          operand_prefix, address_prefix);
    }
    if(token_is(inner->name, LAHF_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0x9f,
                                          operand_prefix, address_prefix);
    }
    if(token_is(inner->name, SAHF_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0x9e,
                                          operand_prefix, address_prefix);
    }
    if(token_is(inner->name, CLTS_INSTRUCTION)) {
        return emit_prefixed_zero_op_0f(p, inner, 0x06,
                                        operand_prefix, address_prefix);
    }
    if(token_is(inner->name, HLT_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0xf4,
                                          operand_prefix, address_prefix);
    }
    return 0;
}

static nint emit_prefixed_cmps(struct Parser *p,
                               struct AstInstruction *inner,
                               unint operand_prefix,
                               unint address_prefix) {
    unint emit_operand_prefix = operand_prefix;
    unsigned char opcode;

    if(!token_is(inner->name, CMPS_INSTRUCTION) &&
       !token_is(inner->name, CMPSB_INSTRUCTION) &&
       !token_is(inner->name, CMPSW_INSTRUCTION) &&
       !token_is(inner->name, CMPSD_INSTRUCTION)) {
        return 0;
    }

    if(inner->arg_count != 0) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if(token_is(inner->name, CMPSB_INSTRUCTION)) {
        opcode = 0xa6;
    } else {
        opcode = 0xa7;
        if(token_is(inner->name, CMPSD_INSTRUCTION)) emit_operand_prefix = 1;
    }

    if(emit_operand_prefix) emit_operand_size_prefix(p, 4);
    if(address_prefix) emit_byte(p, 0x67);
    emit_byte(p, opcode);
    return 1 + (emit_operand_prefix ? 1 : 0) + (address_prefix ? 1 : 0);
}

static nint emit_prefixed_scas(struct Parser *p,
                               struct AstInstruction *inner,
                               unint operand_prefix,
                               unint address_prefix,
                               unint force_o16) {
    unint emit_operand_prefix = operand_prefix;
    unsigned char opcode;

    if(!token_is(inner->name, SCAS_INSTRUCTION) &&
       !token_is(inner->name, SCASB_INSTRUCTION) &&
       !token_is(inner->name, SCASW_INSTRUCTION) &&
       !token_is(inner->name, SCASD_INSTRUCTION)) {
        return 0;
    }

    if(inner->arg_count != 0) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if(token_is(inner->name, SCASB_INSTRUCTION)) {
        opcode = 0xae;
    } else {
        opcode = 0xaf;
        if(token_is(inner->name, SCASD_INSTRUCTION) && !force_o16) {
            emit_operand_prefix = 1;
        }
    }

    if(emit_operand_prefix) emit_operand_size_prefix(p, 4);
    if(address_prefix) emit_byte(p, 0x67);
    emit_byte(p, opcode);
    return 1 + (emit_operand_prefix ? 1 : 0) + (address_prefix ? 1 : 0);
}

static nint emit_prefixed_movs(struct Parser *p,
                               struct AstInstruction *inner,
                               unint operand_prefix,
                               unint address_prefix,
                               unint force_o16) {
    unint emit_operand_prefix = operand_prefix;
    unsigned char opcode;

    if(!token_is(inner->name, MOVS_INSTRUCTION) &&
       !token_is(inner->name, MOVSB_INSTRUCTION) &&
       !token_is(inner->name, MOVSW_INSTRUCTION) &&
       !token_is(inner->name, MOVSD_INSTRUCTION)) {
        return 0;
    }

    if(inner->arg_count != 0) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if(token_is(inner->name, MOVSB_INSTRUCTION)) {
        opcode = 0xa4;
    } else {
        opcode = 0xa5;
        if(token_is(inner->name, MOVSD_INSTRUCTION) && !force_o16) {
            emit_operand_prefix = 1;
        }
    }

    if(emit_operand_prefix) emit_operand_size_prefix(p, 4);
    if(address_prefix) emit_byte(p, 0x67);
    emit_byte(p, opcode);
    return 1 + (emit_operand_prefix ? 1 : 0) + (address_prefix ? 1 : 0);
}

static nint emit_prefixed_lods(struct Parser *p,
                               struct AstInstruction *inner,
                               unint operand_prefix,
                               unint address_prefix,
                               unint force_o16) {
    unint emit_operand_prefix = operand_prefix;
    unsigned char opcode;

    if(!token_is(inner->name, LODS_INSTRUCTION) &&
       !token_is(inner->name, LODSB_INSTRUCTION) &&
       !token_is(inner->name, LODSW_INSTRUCTION) &&
       !token_is(inner->name, LODSD_INSTRUCTION)) {
        return 0;
    }

    if(inner->arg_count != 0) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if(token_is(inner->name, LODSB_INSTRUCTION)) {
        opcode = 0xac;
    } else {
        opcode = 0xad;
        if(token_is(inner->name, LODSD_INSTRUCTION) && !force_o16) {
            emit_operand_prefix = 1;
        }
    }

    if(emit_operand_prefix) emit_operand_size_prefix(p, 4);
    if(address_prefix) emit_byte(p, 0x67);
    emit_byte(p, opcode);
    return 1 + (emit_operand_prefix ? 1 : 0) + (address_prefix ? 1 : 0);
}

static nint emit_prefixed_stos(struct Parser *p,
                               struct AstInstruction *inner,
                               unint operand_prefix,
                               unint address_prefix,
                               unint force_o16) {
    unint emit_operand_prefix = operand_prefix;
    unsigned char opcode;

    if(!token_is(inner->name, STOS_INSTRUCTION) &&
       !token_is(inner->name, STOSB_INSTRUCTION) &&
       !token_is(inner->name, STOSW_INSTRUCTION) &&
       !token_is(inner->name, STOSD_INSTRUCTION)) {
        return 0;
    }

    if(inner->arg_count != 0) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if(token_is(inner->name, STOSB_INSTRUCTION)) {
        opcode = 0xaa;
    } else {
        opcode = 0xab;
        if(token_is(inner->name, STOSD_INSTRUCTION) && !force_o16) {
            emit_operand_prefix = 1;
        }
    }

    if(emit_operand_prefix) emit_operand_size_prefix(p, 4);
    if(address_prefix) emit_byte(p, 0x67);
    emit_byte(p, opcode);
    return 1 + (emit_operand_prefix ? 1 : 0) + (address_prefix ? 1 : 0);
}

static nint emit_prefixed_ins(struct Parser *p,
                              struct AstInstruction *inner,
                              unint operand_prefix,
                              unint address_prefix,
                              unint force_o16) {
    unint emit_operand_prefix = operand_prefix;
    unsigned char opcode;

    if(!token_is(inner->name, INSB_INSTRUCTION) &&
       !token_is(inner->name, INSW_INSTRUCTION) &&
       !token_is(inner->name, INSD_INSTRUCTION)) {
        return 0;
    }

    if(inner->arg_count != 0) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if(token_is(inner->name, INSB_INSTRUCTION)) {
        opcode = 0x6c;
    } else {
        opcode = 0x6d;
        if(token_is(inner->name, INSD_INSTRUCTION) && !force_o16) {
            emit_operand_prefix = 1;
        }
    }

    if(emit_operand_prefix) emit_operand_size_prefix(p, 4);
    if(address_prefix) emit_byte(p, 0x67);
    emit_byte(p, opcode);
    return 1 + (emit_operand_prefix ? 1 : 0) + (address_prefix ? 1 : 0);
}

static nint emit_prefixed_outs(struct Parser *p,
                               struct AstInstruction *inner,
                               unint operand_prefix,
                               unint address_prefix,
                               unint force_o16) {
    unint emit_operand_prefix = operand_prefix;
    unsigned char opcode;

    if(!token_is(inner->name, OUTSB_INSTRUCTION) &&
       !token_is(inner->name, OUTSW_INSTRUCTION) &&
       !token_is(inner->name, OUTSD_INSTRUCTION)) {
        return 0;
    }

    if(inner->arg_count != 0) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if(token_is(inner->name, OUTSB_INSTRUCTION)) {
        opcode = 0x6e;
    } else {
        opcode = 0x6f;
        if(token_is(inner->name, OUTSD_INSTRUCTION) && !force_o16) {
            emit_operand_prefix = 1;
        }
    }

    if(emit_operand_prefix) emit_operand_size_prefix(p, 4);
    if(address_prefix) emit_byte(p, 0x67);
    emit_byte(p, opcode);
    return 1 + (emit_operand_prefix ? 1 : 0) + (address_prefix ? 1 : 0);
}

static nint emit_prefixed_interrupt(struct Parser *p,
                                    struct AstInstruction *inner,
                                    unint operand_prefix,
                                    unint address_prefix,
                                    unint force_o16) {
    struct Operand dst;
    struct token *error_token = NULL;
    nint status;

    if(token_is(inner->name, INTO_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0xce,
                                          operand_prefix, address_prefix);
    }
    if(token_is(inner->name, IRET_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0xcf,
                                          operand_prefix, address_prefix);
    }
    if(token_is(inner->name, IRETD_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, inner, 0xcf,
                                          force_o16 ? 0 : 1, address_prefix);
    }
    if(!token_is(inner->name, INT_INSTRUCTION)) return 0;

    if(inner->arg_count != 1) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }

    if(dst.kind != OPERAND_IMM) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "invalid interrupt number");
        return INSTRUCTION_FAILED;
    }
    if(!fits_u8(dst.imm)) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_OVERFLOW,
                          "interrupt number does not fit 8 bits");
        return INSTRUCTION_FAILED;
    }

    if(operand_prefix) emit_operand_size_prefix(p, 4);
    if(address_prefix) emit_byte(p, 0x67);
    emit_byte(p, 0xcd);
    emit_byte(p, (unsigned char)(dst.imm & 0xff));
    return 2 + (operand_prefix ? 1 : 0) + (address_prefix ? 1 : 0);
}

static nint emit_prefixed_ascii_imm(struct Parser *p, struct AstInstruction *inner,
                                    unsigned char opcode,
                                    unint operand_prefix,
                                    unint address_prefix,
                                    const char *name) {
    nint imm = 0x0a;

    if(inner->arg_count > 1) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if(inner->arg_count == 1) {
        struct Operand src;
        struct token *error_token = NULL;
        nint status = parse_operand(p, inner->args_head, &src, &error_token);
        if(status <= 0) {
            _error_from_token(p, error_token, ERROR_TYPE_I386,
                              "invalid first operand");
            return INSTRUCTION_FAILED;
        }
        if(src.kind != OPERAND_IMM) {
            _error_from_token(p, inner->name, ERROR_TYPE_I386,
                              "unsupported %s form", name);
            return INSTRUCTION_FAILED;
        }
        if(!fits_u16(src.imm)) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        imm = src.imm;
    }

    if(operand_prefix) emit_operand_size_prefix(p, 4);
    if(address_prefix) emit_byte(p, 0x67);
    emit_byte(p, opcode);
    emit_byte(p, (unsigned char)(imm & 0xff));
    return 2 + (operand_prefix ? 1 : 0) + (address_prefix ? 1 : 0);
}

static void emit_forced_prefix_pair(struct Parser *p, unint operand_prefix,
                                    unint address_prefix) {
    if(operand_prefix) emit_operand_size_prefix(p, 4);
    if(address_prefix) emit_byte(p, 0x67);
}

static nint forced_prefix_pair_len(unint operand_prefix,
                                   unint address_prefix) {
    return (operand_prefix ? 1 : 0) + (address_prefix ? 1 : 0);
}

static unint setcc_opcode_from_name(struct token *name,
                                    unsigned char *opcode,
                                    const char **mnemonic) {
    if(token_is(name, SETO_INSTRUCTION)) {
        *opcode = 0x90;
        *mnemonic = "seto";
        return 1;
    }
    if(token_is(name, SETNO_INSTRUCTION)) {
        *opcode = 0x91;
        *mnemonic = "setno";
        return 1;
    }
    if(token_is(name, SETB_INSTRUCTION) || token_is(name, SETC_INSTRUCTION) ||
       token_is(name, SETNAE_INSTRUCTION)) {
        *opcode = 0x92;
        if(token_is(name, SETC_INSTRUCTION)) *mnemonic = "setc";
        else if(token_is(name, SETNAE_INSTRUCTION)) *mnemonic = "setnae";
        else *mnemonic = "setb";
        return 1;
    }
    if(token_is(name, SETAE_INSTRUCTION) || token_is(name, SETNB_INSTRUCTION) ||
       token_is(name, SETNC_INSTRUCTION)) {
        *opcode = 0x93;
        if(token_is(name, SETNB_INSTRUCTION)) *mnemonic = "setnb";
        else if(token_is(name, SETNC_INSTRUCTION)) *mnemonic = "setnc";
        else *mnemonic = "setae";
        return 1;
    }
    if(token_is(name, SETE_INSTRUCTION) || token_is(name, SETZ_INSTRUCTION)) {
        *opcode = 0x94;
        *mnemonic = token_is(name, SETZ_INSTRUCTION) ? "setz" : "sete";
        return 1;
    }
    if(token_is(name, SETNE_INSTRUCTION) || token_is(name, SETNZ_INSTRUCTION)) {
        *opcode = 0x95;
        *mnemonic = token_is(name, SETNZ_INSTRUCTION) ? "setnz" : "setne";
        return 1;
    }
    if(token_is(name, SETBE_INSTRUCTION) || token_is(name, SETNA_INSTRUCTION)) {
        *opcode = 0x96;
        *mnemonic = token_is(name, SETNA_INSTRUCTION) ? "setna" : "setbe";
        return 1;
    }
    if(token_is(name, SETA_INSTRUCTION) || token_is(name, SETNBE_INSTRUCTION)) {
        *opcode = 0x97;
        *mnemonic = token_is(name, SETNBE_INSTRUCTION) ? "setnbe" : "seta";
        return 1;
    }
    if(token_is(name, SETS_INSTRUCTION)) {
        *opcode = 0x98;
        *mnemonic = "sets";
        return 1;
    }
    if(token_is(name, SETNS_INSTRUCTION)) {
        *opcode = 0x99;
        *mnemonic = "setns";
        return 1;
    }
    if(token_is(name, SETP_INSTRUCTION) || token_is(name, SETPE_INSTRUCTION)) {
        *opcode = 0x9a;
        *mnemonic = token_is(name, SETPE_INSTRUCTION) ? "setpe" : "setp";
        return 1;
    }
    if(token_is(name, SETNP_INSTRUCTION) || token_is(name, SETPO_INSTRUCTION)) {
        *opcode = 0x9b;
        *mnemonic = token_is(name, SETPO_INSTRUCTION) ? "setpo" : "setnp";
        return 1;
    }
    if(token_is(name, SETL_INSTRUCTION) || token_is(name, SETNGE_INSTRUCTION)) {
        *opcode = 0x9c;
        *mnemonic = token_is(name, SETNGE_INSTRUCTION) ? "setnge" : "setl";
        return 1;
    }
    if(token_is(name, SETGE_INSTRUCTION) || token_is(name, SETNL_INSTRUCTION)) {
        *opcode = 0x9d;
        *mnemonic = token_is(name, SETNL_INSTRUCTION) ? "setnl" : "setge";
        return 1;
    }
    if(token_is(name, SETLE_INSTRUCTION) || token_is(name, SETNG_INSTRUCTION)) {
        *opcode = 0x9e;
        *mnemonic = token_is(name, SETNG_INSTRUCTION) ? "setng" : "setle";
        return 1;
    }
    if(token_is(name, SETG_INSTRUCTION) || token_is(name, SETNLE_INSTRUCTION)) {
        *opcode = 0x9f;
        *mnemonic = token_is(name, SETNLE_INSTRUCTION) ? "setnle" : "setg";
        return 1;
    }
    return 0;
}

static nint encode_prefixed_setcc(struct Parser *p,
                                  struct AstInstruction *inner,
                                  unsigned char opcode,
                                  const char *name,
                                  unint force_o32,
                                  unint force_a16,
                                  unint force_a32) {
    struct Operand dst;
    struct token *error_token = NULL;
    nint prefix_len;
    nint status;

    if(inner->arg_count != 1) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386, "invalid operand");
        return INSTRUCTION_FAILED;
    }

    if(force_a32) force_direct_memory_a32(&dst);
    if(force_a16 && operand_is_mem32(&dst)) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "invalid a16 address form");
        return INSTRUCTION_FAILED;
    }
    if(force_a32 && operand_is_mem16(&dst)) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "invalid a32 address form");
        return INSTRUCTION_FAILED;
    }

    if(dst.kind == OPERAND_REG8) {
        emit_forced_prefix_pair(p, force_o32, force_a32);
        emit_byte(p, 0x0f);
        emit_byte(p, opcode);
        emit_modrm_reg(p, 0, dst.reg);
        return forced_prefix_pair_len(force_o32, force_a32) + 3;
    }

    if(dst.kind == OPERAND_MEM) {
        if(!memory_size_matches(&dst, 1)) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "invalid %s operand size", name);
            return INSTRUCTION_FAILED;
        }
        prefix_len = memory_prefix_len(&dst.mem) + (force_o32 ? 1 : 0);
        emit_segment_prefix(p, &dst.mem);
        if(force_o32) emit_operand_size_prefix(p, 4);
        emit_address_size_prefix(p, &dst.mem);
        emit_byte(p, 0x0f);
        emit_byte(p, opcode);
        emit_modrm_mem(p, 0, &dst.mem);
        return prefix_len + 3 + memory_tail_len(&dst.mem);
    }

    _error_from_token(p, inner->name, ERROR_TYPE_I386,
                      "unsupported %s form", name);
    return INSTRUCTION_FAILED;
}

static nint encode_arith_no_mem_prefixed(struct Parser *p,
                                         struct AstInstruction *inst,
                                         struct Operand *dst,
                                         struct Operand *src,
                                         unsigned char base,
                                         unsigned char acc8,
                                         unsigned char acc16,
                                         nint group,
                                         const char *name,
                                         unint force_o32,
                                         unint force_a32) {
    unint op_prefix = force_o32;
    nint prefix_len;

    if(dst->kind == OPERAND_REG8 && src->kind == OPERAND_REG8) {
        emit_forced_prefix_pair(p, op_prefix, force_a32);
        emit_byte(p, base);
        emit_modrm_reg(p, src->reg, dst->reg);
        return forced_prefix_pair_len(op_prefix, force_a32) + 2;
    }

    if(dst->kind == OPERAND_REG16 && src->kind == OPERAND_REG16) {
        emit_forced_prefix_pair(p, op_prefix, force_a32);
        emit_byte(p, (unsigned char)(base + 1));
        emit_modrm_reg(p, src->reg, dst->reg);
        return forced_prefix_pair_len(op_prefix, force_a32) + 2;
    }

    if(dst->kind == OPERAND_REG32 && src->kind == OPERAND_REG32) {
        op_prefix = 1;
        emit_forced_prefix_pair(p, op_prefix, force_a32);
        emit_byte(p, (unsigned char)(base + 1));
        emit_modrm_reg(p, src->reg, dst->reg);
        return forced_prefix_pair_len(op_prefix, force_a32) + 2;
    }

    if(src->kind == OPERAND_IMM) {
        if(dst->kind == OPERAND_REG8) {
            if(!fits_u16(src->imm)) {
                _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                                  "immediate does not fit 16 bits");
                return INSTRUCTION_FAILED;
            }
            emit_forced_prefix_pair(p, op_prefix, force_a32);
            prefix_len = forced_prefix_pair_len(op_prefix, force_a32);
            if(is_al(dst)) {
                emit_byte(p, acc8);
                emit_byte(p, (unsigned char)(src->imm & 0xff));
                return prefix_len + 2;
            }
            emit_byte(p, 0x80);
            emit_modrm_reg(p, group, dst->reg);
            emit_byte(p, (unsigned char)(src->imm & 0xff));
            return prefix_len + 3;
        }

        if(dst->kind == OPERAND_REG16) {
            if(!fits_u16(src->imm)) {
                _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                                  "immediate does not fit 16 bits");
                return INSTRUCTION_FAILED;
            }
            emit_forced_prefix_pair(p, op_prefix, force_a32);
            prefix_len = forced_prefix_pair_len(op_prefix, force_a32);
            if(fits_word_sign_extended_i8(src->imm)) {
                emit_byte(p, 0x83);
                emit_modrm_reg(p, group, dst->reg);
                emit_byte(p, (unsigned char)(src->imm & 0xff));
                return prefix_len + 3;
            }
            if(is_ax(dst)) {
                emit_byte(p, acc16);
                emit_word(p, src->imm);
                return prefix_len + 3;
            }
            emit_byte(p, 0x81);
            emit_modrm_reg(p, group, dst->reg);
            emit_word(p, src->imm);
            return prefix_len + 4;
        }

        if(dst->kind == OPERAND_REG32) {
            if(!fits_u32(src->imm)) {
                _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                                  "immediate does not fit 32 bits");
                return INSTRUCTION_FAILED;
            }
            op_prefix = 1;
            emit_forced_prefix_pair(p, op_prefix, force_a32);
            prefix_len = forced_prefix_pair_len(op_prefix, force_a32);
            if(fits_dword_sign_extended_i8(src->imm)) {
                emit_byte(p, 0x83);
                emit_modrm_reg(p, group, dst->reg);
                emit_byte(p, (unsigned char)(src->imm & 0xff));
                return prefix_len + 3;
            }
            if(is_eax(dst)) {
                emit_byte(p, acc16);
                emit_dword(p, src->imm);
                return prefix_len + 5;
            }
            emit_byte(p, 0x81);
            emit_modrm_reg(p, group, dst->reg);
            emit_dword(p, src->imm);
            return prefix_len + 6;
        }
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported %s form", name);
    return INSTRUCTION_FAILED;
}

static unint operand_is_mem16(struct Operand *op) {
    return op->kind == OPERAND_MEM && op->mem.addr_size != 4;
}

static unint operand_is_mem32(struct Operand *op) {
    return op->kind == OPERAND_MEM && op->mem.addr_size == 4;
}

static void force_direct_memory_a32(struct Operand *op) {
    if(op->kind == OPERAND_MEM && op->mem.direct) {
        op->mem.addr_size = 4;
    }
}

static nint encode_a16_arith(struct Parser *p, struct AstInstruction *inner,
                             unsigned char base, unsigned char acc8,
                             unsigned char acc16, nint group,
                             const char *name, unint force_o32) {
    struct Operand dst;
    struct Operand src;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }
    error_token = NULL;
    status = parse_operand(p, inner->args_tail, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    if(operand_is_mem32(&dst)) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "invalid a16 address form");
        return INSTRUCTION_FAILED;
    }
    if(operand_is_mem32(&src)) {
        _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_I386,
                          "invalid a16 address form");
        return INSTRUCTION_FAILED;
    }

    if(force_o32) {
        return encode_o32_arith(p, inner, base, acc8, acc16, group, name);
    }
    return encode_arith(p, inner, &dst, &src, base, acc8, acc16, group, name);
}

static nint encode_a32_arith(struct Parser *p, struct AstInstruction *inner,
                             unsigned char base, unsigned char acc8,
                             unsigned char acc16, nint group,
                             const char *name, unint force_o32) {
    struct Operand dst;
    struct Operand src;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }
    error_token = NULL;
    status = parse_operand(p, inner->args_tail, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    force_direct_memory_a32(&dst);
    force_direct_memory_a32(&src);

    if(operand_is_mem16(&dst)) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "invalid a32 address form");
        return INSTRUCTION_FAILED;
    }
    if(operand_is_mem16(&src)) {
        _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_I386,
                          "invalid a32 address form");
        return INSTRUCTION_FAILED;
    }

    if(dst.kind == OPERAND_MEM || src.kind == OPERAND_MEM) {
        if(force_o32) {
            if(arith_operands_naturally_o32(&dst, &src)) {
                return encode_arith(p, inner, &dst, &src, base, acc8, acc16,
                                    group, name);
            }

            if(dst.kind == OPERAND_MEM && src.kind == OPERAND_REG8) {
                if(!memory_size_matches(&dst, 1)) {
                    _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                                      "invalid %s operand size", name);
                    return INSTRUCTION_FAILED;
                }
                emit_forced_o32_memory_prefixes(p, &dst.mem);
                emit_byte(p, base);
                emit_modrm_mem(p, src.reg, &dst.mem);
                return forced_o32_memory_prefix_len(&dst.mem) + 2 +
                       memory_tail_len(&dst.mem);
            }
            if(dst.kind == OPERAND_MEM && src.kind == OPERAND_REG16) {
                if(!memory_size_matches(&dst, 2)) {
                    _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                                      "invalid %s operand size", name);
                    return INSTRUCTION_FAILED;
                }
                emit_forced_o32_memory_prefixes(p, &dst.mem);
                emit_byte(p, (unsigned char)(base + 1));
                emit_modrm_mem(p, src.reg, &dst.mem);
                return forced_o32_memory_prefix_len(&dst.mem) + 2 +
                       memory_tail_len(&dst.mem);
            }
            if(dst.kind == OPERAND_REG8 && src.kind == OPERAND_MEM) {
                if(!memory_size_matches(&src, 1)) {
                    _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_I386,
                                      "invalid %s operand size", name);
                    return INSTRUCTION_FAILED;
                }
                emit_forced_o32_memory_prefixes(p, &src.mem);
                emit_byte(p, (unsigned char)(base + 2));
                emit_modrm_mem(p, dst.reg, &src.mem);
                return forced_o32_memory_prefix_len(&src.mem) + 2 +
                       memory_tail_len(&src.mem);
            }
            if(dst.kind == OPERAND_REG16 && src.kind == OPERAND_MEM) {
                if(!memory_size_matches(&src, 2)) {
                    _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_I386,
                                      "invalid %s operand size", name);
                    return INSTRUCTION_FAILED;
                }
                emit_forced_o32_memory_prefixes(p, &src.mem);
                emit_byte(p, (unsigned char)(base + 3));
                emit_modrm_mem(p, dst.reg, &src.mem);
                return forced_o32_memory_prefix_len(&src.mem) + 2 +
                       memory_tail_len(&src.mem);
            }
            if(dst.kind == OPERAND_MEM && src.kind == OPERAND_IMM) {
                if(dst.size == 1) {
                    if(!fits_u16(src.imm)) {
                        _error_from_token(p, inner->args_tail->_s,
                                          ERROR_TYPE_OVERFLOW,
                                          "immediate does not fit 16 bits");
                        return INSTRUCTION_FAILED;
                    }
                    emit_forced_o32_memory_prefixes(p, &dst.mem);
                    emit_byte(p, 0x80);
                    emit_modrm_mem(p, group, &dst.mem);
                    emit_byte(p, (unsigned char)(src.imm & 0xff));
                    return forced_o32_memory_prefix_len(&dst.mem) + 3 +
                           memory_tail_len(&dst.mem);
                }
                if(dst.size == 2) {
                    if(!fits_u16(src.imm)) {
                        _error_from_token(p, inner->args_tail->_s,
                                          ERROR_TYPE_OVERFLOW,
                                          "immediate does not fit 16 bits");
                        return INSTRUCTION_FAILED;
                    }
                    emit_forced_o32_memory_prefixes(p, &dst.mem);
                    if(fits_word_sign_extended_i8(src.imm)) {
                        emit_byte(p, 0x83);
                        emit_modrm_mem(p, group, &dst.mem);
                        emit_byte(p, (unsigned char)(src.imm & 0xff));
                        return forced_o32_memory_prefix_len(&dst.mem) + 3 +
                               memory_tail_len(&dst.mem);
                    }
                    emit_byte(p, 0x81);
                    emit_modrm_mem(p, group, &dst.mem);
                    emit_word(p, src.imm);
                    return forced_o32_memory_prefix_len(&dst.mem) + 4 +
                           memory_tail_len(&dst.mem);
                }
                _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                                  "ambiguous memory immediate size");
                return INSTRUCTION_FAILED;
            }

            _error_from_token(p, inner->name, ERROR_TYPE_I386,
                              "unsupported %s form", name);
            return INSTRUCTION_FAILED;
        }
        return encode_arith(p, inner, &dst, &src, base, acc8, acc16,
                            group, name);
    }

    return encode_arith_no_mem_prefixed(p, inner, &dst, &src, base, acc8,
                                        acc16, group, name, force_o32, 1);
}

static nint encode_arpl_operands(struct Parser *p, struct AstInstruction *inst,
                                 struct Operand *dst, struct Operand *src,
                                 unint force_o32, unint force_a32) {
    if(src->kind != OPERAND_REG16) {
        _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                          "invalid arpl source");
        return INSTRUCTION_FAILED;
    }

    if(force_a32) force_direct_memory_a32(dst);
    if(force_a32 && operand_is_mem16(dst)) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "invalid a32 address form");
        return INSTRUCTION_FAILED;
    }

    if(dst->kind == OPERAND_REG16) {
        emit_forced_prefix_pair(p, force_o32, force_a32);
        emit_byte(p, 0x63);
        emit_modrm_reg(p, src->reg, dst->reg);
        return forced_prefix_pair_len(force_o32, force_a32) + 2;
    }

    if(dst->kind == OPERAND_MEM) {
        if(!memory_size_matches(dst, 2)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid arpl operand size");
            return INSTRUCTION_FAILED;
        }
        if(force_o32) emit_memory_operand_prefixes(p, &dst->mem, 4);
        else emit_memory_prefixes(p, &dst->mem);
        emit_byte(p, 0x63);
        emit_modrm_mem(p, src->reg, &dst->mem);
        return (force_o32 ? memory_operand_prefix_len(&dst->mem, 4) :
                            memory_prefix_len(&dst->mem)) +
               2 + memory_tail_len(&dst->mem);
    }

    _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                      "invalid arpl destination");
    return INSTRUCTION_FAILED;
}

static nint encode_arpl_prefix(struct Parser *p, struct AstInstruction *inner,
                               unint force_o32, unint force_a32) {
    struct Operand dst;
    struct Operand src;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }
    error_token = NULL;
    status = parse_operand(p, inner->args_tail, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    return encode_arpl_operands(p, inner, &dst, &src, force_o32, force_a32);
}

static nint encode_a16_arpl(struct Parser *p, struct AstInstruction *inner,
                            unint force_o32) {
    struct Operand dst;
    struct Operand src;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }
    error_token = NULL;
    status = parse_operand(p, inner->args_tail, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    if(operand_is_mem32(&dst)) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "invalid a16 address form");
        return INSTRUCTION_FAILED;
    }

    return encode_arpl_operands(p, inner, &dst, &src, force_o32, 0);
}

static nint encode_bound_prefixed_operands(struct Parser *p,
                                           struct AstInstruction *inst,
                                           struct Operand *dst,
                                           struct Operand *src,
                                           unint force_o32,
                                           unint force_a32) {
    nint op_size;

    if((dst->kind != OPERAND_REG16 && dst->kind != OPERAND_REG32) ||
       src->kind != OPERAND_MEM) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported bound form");
        return INSTRUCTION_FAILED;
    }

    if(src->size != 0) {
        _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                          "invalid bound source size");
        return INSTRUCTION_FAILED;
    }

    if(force_a32) force_direct_memory_a32(src);
    if(force_a32 && operand_is_mem16(src)) {
        _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                          "invalid a32 address form");
        return INSTRUCTION_FAILED;
    }
    if(!force_a32 && operand_is_mem32(src) && src->mem.direct) {
        src->mem.addr_size = 2;
    }

    op_size = (force_o32 || dst->kind == OPERAND_REG32) ? 4 : 2;
    emit_memory_operand_prefixes(p, &src->mem, op_size);
    emit_byte(p, 0x62);
    emit_modrm_mem(p, dst->reg, &src->mem);
    return memory_operand_prefix_len(&src->mem, op_size) + 2 +
           memory_tail_len(&src->mem);
}

static nint encode_bound_prefixed(struct Parser *p, struct AstInstruction *inner,
                                  unint force_o32, unint force_a16,
                                  unint force_a32) {
    struct Operand dst;
    struct Operand src;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }
    error_token = NULL;
    status = parse_operand(p, inner->args_tail, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    if(force_a16 && operand_is_mem32(&src)) {
        _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_I386,
                          "invalid a16 address form");
        return INSTRUCTION_FAILED;
    }

    return encode_bound_prefixed_operands(p, inner, &dst, &src,
                                          force_o32, force_a32);
}

static nint encode_bit_scan_prefixed_operands(struct Parser *p,
                                              struct AstInstruction *inst,
                                              struct Operand *dst,
                                              struct Operand *src,
                                              unsigned char opcode,
                                              const char *name,
                                              unint force_o32,
                                              unint force_a32) {
    nint op_size;

    if(dst->kind != OPERAND_REG16 && dst->kind != OPERAND_REG32) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "invalid bit scan destination");
        return INSTRUCTION_FAILED;
    }

    if(src->kind == OPERAND_REG16 || src->kind == OPERAND_REG32) {
        if(src->size != dst->size) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid bit scan source size");
            return INSTRUCTION_FAILED;
        }
        op_size = (force_o32 || dst->kind == OPERAND_REG32) ? 4 : 2;
        emit_forced_prefix_pair(p, op_size == 4, force_a32);
        emit_byte(p, 0x0f);
        emit_byte(p, opcode);
        emit_modrm_reg(p, dst->reg, src->reg);
        return forced_prefix_pair_len(op_size == 4, force_a32) + 3;
    }

    if(src->kind == OPERAND_MEM) {
        if(src->size != 0 && src->size != dst->size) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid bit scan source size");
            return INSTRUCTION_FAILED;
        }

        if(force_a32) force_direct_memory_a32(src);
        if(force_a32 && operand_is_mem16(src)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid a32 address form");
            return INSTRUCTION_FAILED;
        }
        if(!force_a32 && operand_is_mem32(src) && src->mem.direct) {
            src->mem.addr_size = 2;
        }

        op_size = (force_o32 || dst->kind == OPERAND_REG32) ? 4 : dst->size;
        emit_memory_operand_prefixes(p, &src->mem, op_size);
        emit_byte(p, 0x0f);
        emit_byte(p, opcode);
        emit_modrm_mem(p, dst->reg, &src->mem);
        return memory_operand_prefix_len(&src->mem, op_size) + 3 +
               memory_tail_len(&src->mem);
    }

    _error_from_token(p, inst->name, ERROR_TYPE_I386, name);
    return INSTRUCTION_FAILED;
}

static nint encode_bit_scan_prefixed(struct Parser *p,
                                     struct AstInstruction *inner,
                                     unsigned char opcode,
                                     const char *name,
                                     unint force_o32,
                                     unint force_a16,
                                     unint force_a32) {
    struct Operand dst;
    struct Operand src;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }
    error_token = NULL;
    status = parse_operand(p, inner->args_tail, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    if(force_a16 && operand_is_mem32(&src)) {
        _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_I386,
                          "invalid a16 address form");
        return INSTRUCTION_FAILED;
    }

    return encode_bit_scan_prefixed_operands(p, inner, &dst, &src, opcode,
                                             name, force_o32, force_a32);
}

static nint encode_lar_prefixed(struct Parser *p,
                                struct AstInstruction *inner,
                                unint force_o32,
                                unint force_a16,
                                unint force_a32,
                                unint force_o16) {
    struct Operand dst;
    struct Operand src;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }
    error_token = NULL;
    status = parse_operand(p, inner->args_tail, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    return encode_lar_operands(p, inner, &dst, &src, force_o32, force_a16,
                               force_a32, force_o16);
}

static nint encode_lsl_prefixed(struct Parser *p,
                                struct AstInstruction *inner,
                                unint force_o32,
                                unint force_a16,
                                unint force_a32,
                                unint force_o16) {
    struct Operand dst;
    struct Operand src;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }
    error_token = NULL;
    status = parse_operand(p, inner->args_tail, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    return encode_lsl_operands(p, inner, &dst, &src, force_o32, force_a16,
                               force_a32, force_o16);
}

static nint bit_test_operand_size(struct Parser *p, struct AstInstruction *inst,
                                  struct Operand *dst, struct Operand *src,
                                  unint force_o32, nint *size) {
    if(src->kind == OPERAND_IMM) {
        if(dst->kind == OPERAND_REG16 || dst->kind == OPERAND_REG32) {
            *size = (force_o32 || dst->kind == OPERAND_REG32) ? 4 : 2;
            return 1;
        }
        if(dst->kind == OPERAND_MEM) {
            if(dst->size != 2 && dst->size != 4) {
                _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                                  "ambiguous bit test operand size");
                return INSTRUCTION_FAILED;
            }
            *size = (force_o32 || dst->size == 4) ? 4 : 2;
            return 1;
        }
        return 0;
    }

    if(src->kind != OPERAND_REG16 && src->kind != OPERAND_REG32) return 0;

    if(dst->kind == OPERAND_REG16 || dst->kind == OPERAND_REG32) {
        if(dst->size != src->size) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_I386,
                              "invalid bit test source size");
            return INSTRUCTION_FAILED;
        }
        *size = (force_o32 || dst->kind == OPERAND_REG32) ? 4 : 2;
        return 1;
    }

    if(dst->kind == OPERAND_MEM) {
        if(dst->size != 0 && dst->size != src->size) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid bit test operand size");
            return INSTRUCTION_FAILED;
        }
        *size = (force_o32 || src->kind == OPERAND_REG32) ? 4 : 2;
        return 1;
    }

    return 0;
}

static nint encode_bit_test_prefixed_operands(struct Parser *p,
                                              struct AstInstruction *inst,
                                              struct Operand *dst,
                                              struct Operand *src,
                                              unsigned char reg_opcode,
                                              nint imm_group,
                                              const char *name,
                                              unint force_o32,
                                              unint force_a16,
                                              unint force_a32) {
    nint size;
    nint status;

    if(dst->kind != OPERAND_REG16 && dst->kind != OPERAND_REG32 &&
       dst->kind != OPERAND_MEM) {
        _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                          "invalid bit test destination");
        return INSTRUCTION_FAILED;
    }

    if(dst->kind == OPERAND_MEM) {
        if(force_a32) force_direct_memory_a32(dst);
        if(force_a16 && operand_is_mem32(dst)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid a16 address form");
            return INSTRUCTION_FAILED;
        }
        if(force_a32 && operand_is_mem16(dst)) {
            _error_from_token(p, inst->args_head->_s, ERROR_TYPE_I386,
                              "invalid a32 address form");
            return INSTRUCTION_FAILED;
        }
    }

    status = bit_test_operand_size(p, inst, dst, src, force_o32, &size);
    if(status <= 0) {
        if(status == INSTRUCTION_FAILED) return INSTRUCTION_FAILED;
        _error_from_token(p, inst->name, ERROR_TYPE_I386, name);
        return INSTRUCTION_FAILED;
    }

    if(src->kind == OPERAND_IMM) {
        if(!fits_u16(src->imm)) {
            _error_from_token(p, inst->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "bit offset does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }

        if(dst->kind == OPERAND_MEM) emit_memory_operand_prefixes(p, &dst->mem, size);
        else emit_forced_prefix_pair(p, size == 4, force_a32);
        emit_byte(p, 0x0f);
        emit_byte(p, 0xba);
        if(dst->kind == OPERAND_MEM) emit_modrm_mem(p, imm_group, &dst->mem);
        else emit_modrm_reg(p, imm_group, dst->reg);
        emit_byte(p, (unsigned char)(src->imm & 0xff));
        return (dst->kind == OPERAND_MEM ? memory_operand_prefix_len(&dst->mem, size) :
                                           forced_prefix_pair_len(size == 4, force_a32)) +
               3 + (dst->kind == OPERAND_MEM ? memory_tail_len(&dst->mem) : 0) + 1;
    }

    if(dst->kind == OPERAND_MEM) emit_memory_operand_prefixes(p, &dst->mem, size);
    else emit_forced_prefix_pair(p, size == 4, force_a32);
    emit_byte(p, 0x0f);
    emit_byte(p, reg_opcode);
    if(dst->kind == OPERAND_MEM) emit_modrm_mem(p, src->reg, &dst->mem);
    else emit_modrm_reg(p, src->reg, dst->reg);
    return (dst->kind == OPERAND_MEM ? memory_operand_prefix_len(&dst->mem, size) :
                                       forced_prefix_pair_len(size == 4, force_a32)) +
           3 + (dst->kind == OPERAND_MEM ? memory_tail_len(&dst->mem) : 0);
}

static nint encode_bit_test_prefixed(struct Parser *p,
                                     struct AstInstruction *inner,
                                     unsigned char reg_opcode,
                                     nint imm_group,
                                     const char *name,
                                     unint force_o32,
                                     unint force_a16,
                                     unint force_a32) {
    struct Operand dst;
    struct Operand src;
    struct token *error_token = NULL;
    nint status;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_operand(p, inner->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }
    error_token = NULL;
    status = parse_operand(p, inner->args_tail, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    return encode_bit_test_prefixed_operands(p, inner, &dst, &src,
                                             reg_opcode, imm_group, name,
                                             force_o32, force_a16, force_a32);
}

static nint encode_o16_prefix(struct Parser *p, struct AstInstruction *inst) {
    struct AstInstruction inner;
    struct InstructionArg inner_args[3];
    nint status = build_prefixed_instruction(p, inst, &inner, inner_args);
    if(status <= 0) return status;

    status = encode_misc_prefixed_instruction(p, &inner, 0, 0, 0, 1);
    if(status != 0) return status;

    status = emit_prefixed_convert(p, &inner, 0, 0, 1);
    if(status != 0) return status;
    status = emit_prefixed_decimal_adjust(p, &inner, 0, 0);
    if(status != 0) return status;
    if(token_is(inner.name, DEC_INSTRUCTION)) {
        return encode_prefixed_inc_dec(p, &inner, 1, 0x48, "dec", 0, 0, 0);
    }
    if(token_is(inner.name, OR_INSTRUCTION)) {
        return _86_exec(p, &inner);
    }
    if(token_is(inner.name, SBB_INSTRUCTION)) {
        return _86_exec(p, &inner);
    }
    if(token_is(inner.name, SUB_INSTRUCTION)) {
        return _86_exec(p, &inner);
    }
    if(token_is(inner.name, TEST_INSTRUCTION)) {
        return _86_exec(p, &inner);
    }

    if(token_is(inner.name, A16_INSTRUCTION) || token_is(inner.name, A32_INSTRUCTION) ||
       token_is(inner.name, O32_INSTRUCTION)) {
        struct AstInstruction nested;
        struct InstructionArg nested_args[3];
        unint force_a16 = token_is(inner.name, A16_INSTRUCTION);
        unint force_a32 = token_is(inner.name, A32_INSTRUCTION);
        unint force_o32 = token_is(inner.name, O32_INSTRUCTION);
        status = build_prefixed_instruction(p, &inner, &nested, nested_args);
        if(status <= 0) return status;

        status = encode_misc_prefixed_instruction(p, &nested, force_o32,
                                                  force_a16, force_a32,
                                                  !force_o32);
        if(status != 0) return status;

        status = emit_prefixed_convert(p, &nested, force_o32, force_a32,
                                       !force_o32);
        if(status != 0) return status;
        status = emit_prefixed_decimal_adjust(p, &nested, force_o32, force_a32);
        if(status != 0) return status;
        if(token_is(nested.name, DEC_INSTRUCTION)) {
            return encode_prefixed_inc_dec(p, &nested, 1, 0x48, "dec",
                                           force_o32, force_a16, force_a32);
        }
    }

    _error_from_token(p, inner.name, ERROR_TYPE_I386,
                      "unsupported o16 instruction");
    return INSTRUCTION_FAILED;
}

static nint encode_a16_prefix(struct Parser *p, struct AstInstruction *inst) {
    struct AstInstruction inner;
    struct InstructionArg inner_args[3];
    nint status = build_prefixed_instruction(p, inst, &inner, inner_args);
    if(status <= 0) return status;

    status = encode_misc_prefixed_instruction(p, &inner, 0, 1, 0, 0);
    if(status != 0) return status;

    if(token_is(inner.name, AAA_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, &inner, 0x37, 0, 0);
    }
    if(token_is(inner.name, AAS_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, &inner, 0x3f, 0, 0);
    }
    if(token_is(inner.name, CBW_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, &inner, 0x98, 0, 0);
    }
    if(token_is(inner.name, CDQ_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, &inner, 0x99, 1, 0);
    }
    status = emit_prefixed_convert(p, &inner, 0, 0, 0);
    if(status != 0) return status;
    status = emit_prefixed_decimal_adjust(p, &inner, 0, 0);
    if(status != 0) return status;
    status = emit_prefixed_flag_control(p, &inner, 0, 0);
    if(status != 0) return status;
    status = emit_prefixed_cmps(p, &inner, 0, 0);
    if(status != 0) return status;
    if(token_is(inner.name, AAD_INSTRUCTION)) {
        return emit_prefixed_ascii_imm(p, &inner, 0xd5, 0, 0, "aad");
    }
    if(token_is(inner.name, AAM_INSTRUCTION)) {
        return emit_prefixed_ascii_imm(p, &inner, 0xd4, 0, 0, "aam");
    }
    if(token_is(inner.name, ADD_INSTRUCTION)) {
        return encode_a16_arith(p, &inner, 0x00, 0x04, 0x05, 0, "add", 0);
    }
    if(token_is(inner.name, ADC_INSTRUCTION)) {
        return encode_a16_arith(p, &inner, 0x10, 0x14, 0x15, 2, "adc", 0);
    }
    if(token_is(inner.name, AND_INSTRUCTION)) {
        return encode_a16_arith(p, &inner, 0x20, 0x24, 0x25, 4, "and", 0);
    }
    if(token_is(inner.name, OR_INSTRUCTION)) {
        return encode_a16_arith(p, &inner, 0x08, 0x0c, 0x0d, 1, "or", 0);
    }
    if(token_is(inner.name, XOR_INSTRUCTION)) {
        return encode_a16_arith(p, &inner, 0x30, 0x34, 0x35, 6, "xor", 0);
    }
    if(token_is(inner.name, SBB_INSTRUCTION)) {
        return encode_a16_arith(p, &inner, 0x18, 0x1c, 0x1d, 3, "sbb", 0);
    }
    if(token_is(inner.name, SUB_INSTRUCTION)) {
        return encode_a16_arith(p, &inner, 0x28, 0x2c, 0x2d, 5, "sub", 0);
    }
    if(token_is(inner.name, CMP_INSTRUCTION)) {
        return encode_a16_arith(p, &inner, 0x38, 0x3c, 0x3d, 7, "cmp", 0);
    }
    if(token_is(inner.name, TEST_INSTRUCTION)) {
        return encode_address_prefixed_test(p, &inner, 1, 0);
    }
    if(token_is(inner.name, DEC_INSTRUCTION)) {
        return encode_prefixed_inc_dec(p, &inner, 1, 0x48, "dec", 0, 1, 0);
    }
    if(token_is(inner.name, ARPL_INSTRUCTION)) {
        return encode_a16_arpl(p, &inner, 0);
    }
    if(token_is(inner.name, BOUND_INSTRUCTION)) {
        return encode_bound_prefixed(p, &inner, 0, 1, 0);
    }
    if(token_is(inner.name, BSF_INSTRUCTION)) {
        return encode_bit_scan_prefixed(p, &inner, 0xbc,
                                        "unsupported bsf form", 0, 1, 0);
    }
    if(token_is(inner.name, BSR_INSTRUCTION)) {
        return encode_bit_scan_prefixed(p, &inner, 0xbd,
                                        "unsupported bsr form", 0, 1, 0);
    }
    if(token_is(inner.name, BT_INSTRUCTION)) {
        return encode_bit_test_prefixed(p, &inner, 0xa3, 4,
                                        "unsupported bt form", 0, 1, 0);
    }
    if(token_is(inner.name, BTC_INSTRUCTION)) {
        return encode_bit_test_prefixed(p, &inner, 0xbb, 7,
                                        "unsupported btc form", 0, 1, 0);
    }
    if(token_is(inner.name, BTR_INSTRUCTION)) {
        return encode_bit_test_prefixed(p, &inner, 0xb3, 6,
                                        "unsupported btr form", 0, 1, 0);
    }
    if(token_is(inner.name, BTS_INSTRUCTION)) {
        return encode_bit_test_prefixed(p, &inner, 0xab, 5,
                                        "unsupported bts form", 0, 1, 0);
    }

    if(token_is(inner.name, O16_INSTRUCTION)) {
        struct AstInstruction nested;
        struct InstructionArg nested_args[3];
        status = build_prefixed_instruction(p, &inner, &nested, nested_args);
        if(status <= 0) return status;

        status = encode_misc_prefixed_instruction(p, &nested, 0, 1, 0, 1);
        if(status != 0) return status;

        status = emit_prefixed_convert(p, &nested, 0, 0, 1);
        if(status != 0) return status;
        status = emit_prefixed_decimal_adjust(p, &nested, 0, 0);
        if(status != 0) return status;
        if(token_is(nested.name, DEC_INSTRUCTION)) {
            return encode_prefixed_inc_dec(p, &nested, 1, 0x48, "dec", 0, 1, 0);
        }
    }

    if(token_is(inner.name, O32_INSTRUCTION)) {
        struct AstInstruction nested;
        struct InstructionArg nested_args[3];
        status = build_prefixed_instruction(p, &inner, &nested, nested_args);
        if(status <= 0) return status;

        status = encode_misc_prefixed_instruction(p, &nested, 1, 1, 0, 0);
        if(status != 0) return status;

        if(token_is(nested.name, AAA_INSTRUCTION)) {
            return emit_prefixed_zero_op_byte(p, &nested, 0x37, 1, 0);
        }
        if(token_is(nested.name, AAS_INSTRUCTION)) {
            return emit_prefixed_zero_op_byte(p, &nested, 0x3f, 1, 0);
        }
        if(token_is(nested.name, CBW_INSTRUCTION)) {
            return emit_prefixed_zero_op_byte(p, &nested, 0x98, 1, 0);
        }
        if(token_is(nested.name, CDQ_INSTRUCTION)) {
            return emit_prefixed_zero_op_byte(p, &nested, 0x99, 1, 0);
        }
        status = emit_prefixed_convert(p, &nested, 1, 0, 0);
        if(status != 0) return status;
        status = emit_prefixed_decimal_adjust(p, &nested, 1, 0);
        if(status != 0) return status;
        status = emit_prefixed_flag_control(p, &nested, 1, 0);
        if(status != 0) return status;
        status = emit_prefixed_cmps(p, &nested, 1, 0);
        if(status != 0) return status;
        if(token_is(nested.name, AAD_INSTRUCTION)) {
            return emit_prefixed_ascii_imm(p, &nested, 0xd5, 1, 0, "aad");
        }
        if(token_is(nested.name, AAM_INSTRUCTION)) {
            return emit_prefixed_ascii_imm(p, &nested, 0xd4, 1, 0, "aam");
        }
        if(token_is(nested.name, ADD_INSTRUCTION)) {
            return encode_a16_arith(p, &nested, 0x00, 0x04, 0x05, 0, "add", 1);
        }
        if(token_is(nested.name, ADC_INSTRUCTION)) {
            return encode_a16_arith(p, &nested, 0x10, 0x14, 0x15, 2, "adc", 1);
        }
        if(token_is(nested.name, AND_INSTRUCTION)) {
            return encode_a16_arith(p, &nested, 0x20, 0x24, 0x25, 4, "and", 1);
        }
        if(token_is(nested.name, OR_INSTRUCTION)) {
            return encode_a16_arith(p, &nested, 0x08, 0x0c, 0x0d, 1, "or", 1);
        }
        if(token_is(nested.name, XOR_INSTRUCTION)) {
            return encode_a16_arith(p, &nested, 0x30, 0x34, 0x35, 6, "xor", 1);
        }
        if(token_is(nested.name, SBB_INSTRUCTION)) {
            return encode_a16_arith(p, &nested, 0x18, 0x1c, 0x1d, 3, "sbb", 1);
        }
        if(token_is(nested.name, SUB_INSTRUCTION)) {
            return encode_a16_arith(p, &nested, 0x28, 0x2c, 0x2d, 5, "sub", 1);
        }
        if(token_is(nested.name, CMP_INSTRUCTION)) {
            return encode_a16_arith(p, &nested, 0x38, 0x3c, 0x3d, 7, "cmp", 1);
        }
        if(token_is(nested.name, DEC_INSTRUCTION)) {
            return encode_prefixed_inc_dec(p, &nested, 1, 0x48, "dec", 1, 1, 0);
        }
        if(token_is(nested.name, ARPL_INSTRUCTION)) {
            return encode_a16_arpl(p, &nested, 1);
        }
        if(token_is(nested.name, BOUND_INSTRUCTION)) {
            return encode_bound_prefixed(p, &nested, 1, 1, 0);
        }
        if(token_is(nested.name, BSF_INSTRUCTION)) {
            return encode_bit_scan_prefixed(p, &nested, 0xbc,
                                            "unsupported bsf form", 1, 1, 0);
        }
        if(token_is(nested.name, BSR_INSTRUCTION)) {
            return encode_bit_scan_prefixed(p, &nested, 0xbd,
                                            "unsupported bsr form", 1, 1, 0);
        }
        if(token_is(nested.name, BT_INSTRUCTION)) {
            return encode_bit_test_prefixed(p, &nested, 0xa3, 4,
                                            "unsupported bt form", 1, 1, 0);
        }
        if(token_is(nested.name, BTC_INSTRUCTION)) {
            return encode_bit_test_prefixed(p, &nested, 0xbb, 7,
                                            "unsupported btc form", 1, 1, 0);
        }
        if(token_is(nested.name, BTR_INSTRUCTION)) {
            return encode_bit_test_prefixed(p, &nested, 0xb3, 6,
                                            "unsupported btr form", 1, 1, 0);
        }
        if(token_is(nested.name, BTS_INSTRUCTION)) {
            return encode_bit_test_prefixed(p, &nested, 0xab, 5,
                                            "unsupported bts form", 1, 1, 0);
        }
    }

    _error_from_token(p, inner.name, ERROR_TYPE_I386,
                      "unsupported a16 instruction");
    return INSTRUCTION_FAILED;
}

static nint encode_a32_prefix(struct Parser *p, struct AstInstruction *inst) {
    struct AstInstruction inner;
    struct InstructionArg inner_args[3];
    nint status = build_prefixed_instruction(p, inst, &inner, inner_args);
    if(status <= 0) return status;

    status = encode_misc_prefixed_instruction(p, &inner, 0, 0, 1, 0);
    if(status != 0) return status;

    if(token_is(inner.name, AAA_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, &inner, 0x37, 0, 1);
    }
    if(token_is(inner.name, AAS_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, &inner, 0x3f, 0, 1);
    }
    if(token_is(inner.name, CBW_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, &inner, 0x98, 0, 1);
    }
    if(token_is(inner.name, CDQ_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, &inner, 0x99, 1, 1);
    }
    status = emit_prefixed_convert(p, &inner, 0, 1, 0);
    if(status != 0) return status;
    status = emit_prefixed_decimal_adjust(p, &inner, 0, 1);
    if(status != 0) return status;
    status = emit_prefixed_flag_control(p, &inner, 0, 1);
    if(status != 0) return status;
    status = emit_prefixed_cmps(p, &inner, 0, 1);
    if(status != 0) return status;
    if(token_is(inner.name, AAD_INSTRUCTION)) {
        return emit_prefixed_ascii_imm(p, &inner, 0xd5, 0, 1, "aad");
    }
    if(token_is(inner.name, AAM_INSTRUCTION)) {
        return emit_prefixed_ascii_imm(p, &inner, 0xd4, 0, 1, "aam");
    }
    if(token_is(inner.name, ADC_INSTRUCTION)) {
        return encode_a32_arith(p, &inner, 0x10, 0x14, 0x15, 2, "adc", 0);
    }
    if(token_is(inner.name, ADD_INSTRUCTION)) {
        return encode_a32_arith(p, &inner, 0x00, 0x04, 0x05, 0, "add", 0);
    }
    if(token_is(inner.name, AND_INSTRUCTION)) {
        return encode_a32_arith(p, &inner, 0x20, 0x24, 0x25, 4, "and", 0);
    }
    if(token_is(inner.name, OR_INSTRUCTION)) {
        return encode_a32_arith(p, &inner, 0x08, 0x0c, 0x0d, 1, "or", 0);
    }
    if(token_is(inner.name, XOR_INSTRUCTION)) {
        return encode_a32_arith(p, &inner, 0x30, 0x34, 0x35, 6, "xor", 0);
    }
    if(token_is(inner.name, SBB_INSTRUCTION)) {
        return encode_a32_arith(p, &inner, 0x18, 0x1c, 0x1d, 3, "sbb", 0);
    }
    if(token_is(inner.name, SUB_INSTRUCTION)) {
        return encode_a32_arith(p, &inner, 0x28, 0x2c, 0x2d, 5, "sub", 0);
    }
    if(token_is(inner.name, CMP_INSTRUCTION)) {
        return encode_a32_arith(p, &inner, 0x38, 0x3c, 0x3d, 7, "cmp", 0);
    }
    if(token_is(inner.name, TEST_INSTRUCTION)) {
        return encode_address_prefixed_test(p, &inner, 0, 1);
    }
    if(token_is(inner.name, DEC_INSTRUCTION)) {
        return encode_prefixed_inc_dec(p, &inner, 1, 0x48, "dec", 0, 0, 1);
    }
    if(token_is(inner.name, ARPL_INSTRUCTION)) {
        return encode_arpl_prefix(p, &inner, 0, 1);
    }
    if(token_is(inner.name, BOUND_INSTRUCTION)) {
        return encode_bound_prefixed(p, &inner, 0, 0, 1);
    }
    if(token_is(inner.name, BSF_INSTRUCTION)) {
        return encode_bit_scan_prefixed(p, &inner, 0xbc,
                                        "unsupported bsf form", 0, 0, 1);
    }
    if(token_is(inner.name, BSR_INSTRUCTION)) {
        return encode_bit_scan_prefixed(p, &inner, 0xbd,
                                        "unsupported bsr form", 0, 0, 1);
    }
    if(token_is(inner.name, BT_INSTRUCTION)) {
        return encode_bit_test_prefixed(p, &inner, 0xa3, 4,
                                        "unsupported bt form", 0, 0, 1);
    }
    if(token_is(inner.name, BTC_INSTRUCTION)) {
        return encode_bit_test_prefixed(p, &inner, 0xbb, 7,
                                        "unsupported btc form", 0, 0, 1);
    }
    if(token_is(inner.name, BTR_INSTRUCTION)) {
        return encode_bit_test_prefixed(p, &inner, 0xb3, 6,
                                        "unsupported btr form", 0, 0, 1);
    }
    if(token_is(inner.name, BTS_INSTRUCTION)) {
        return encode_bit_test_prefixed(p, &inner, 0xab, 5,
                                        "unsupported bts form", 0, 0, 1);
    }

    if(token_is(inner.name, O16_INSTRUCTION)) {
        struct AstInstruction nested;
        struct InstructionArg nested_args[3];
        status = build_prefixed_instruction(p, &inner, &nested, nested_args);
        if(status <= 0) return status;

        status = encode_misc_prefixed_instruction(p, &nested, 0, 0, 1, 1);
        if(status != 0) return status;

        status = emit_prefixed_convert(p, &nested, 0, 1, 1);
        if(status != 0) return status;
        status = emit_prefixed_decimal_adjust(p, &nested, 0, 1);
        if(status != 0) return status;
        if(token_is(nested.name, DEC_INSTRUCTION)) {
            return encode_prefixed_inc_dec(p, &nested, 1, 0x48, "dec", 0, 0, 1);
        }
    }

    if(token_is(inner.name, O32_INSTRUCTION)) {
        struct AstInstruction nested;
        struct InstructionArg nested_args[3];
        status = build_prefixed_instruction(p, &inner, &nested, nested_args);
        if(status <= 0) return status;
        status = encode_misc_prefixed_instruction(p, &nested, 1, 0, 1, 0);
        if(status != 0) return status;
        if(token_is(nested.name, AAA_INSTRUCTION)) {
            return emit_prefixed_zero_op_byte(p, &nested, 0x37, 1, 1);
        }
        if(token_is(nested.name, AAS_INSTRUCTION)) {
            return emit_prefixed_zero_op_byte(p, &nested, 0x3f, 1, 1);
        }
        if(token_is(nested.name, CBW_INSTRUCTION)) {
            return emit_prefixed_zero_op_byte(p, &nested, 0x98, 1, 1);
        }
        if(token_is(nested.name, CDQ_INSTRUCTION)) {
            return emit_prefixed_zero_op_byte(p, &nested, 0x99, 1, 1);
        }
        status = emit_prefixed_convert(p, &nested, 1, 1, 0);
        if(status != 0) return status;
        status = emit_prefixed_decimal_adjust(p, &nested, 1, 1);
        if(status != 0) return status;
        status = emit_prefixed_flag_control(p, &nested, 1, 1);
        if(status != 0) return status;
        status = emit_prefixed_cmps(p, &nested, 1, 1);
        if(status != 0) return status;
        if(token_is(nested.name, AAD_INSTRUCTION)) {
            return emit_prefixed_ascii_imm(p, &nested, 0xd5, 1, 1, "aad");
        }
        if(token_is(nested.name, AAM_INSTRUCTION)) {
            return emit_prefixed_ascii_imm(p, &nested, 0xd4, 1, 1, "aam");
        }
        if(token_is(nested.name, ADC_INSTRUCTION)) {
            return encode_a32_arith(p, &nested, 0x10, 0x14, 0x15, 2, "adc", 1);
        }
        if(token_is(nested.name, ADD_INSTRUCTION)) {
            return encode_a32_arith(p, &nested, 0x00, 0x04, 0x05, 0, "add", 1);
        }
        if(token_is(nested.name, AND_INSTRUCTION)) {
            return encode_a32_arith(p, &nested, 0x20, 0x24, 0x25, 4, "and", 1);
        }
        if(token_is(nested.name, OR_INSTRUCTION)) {
            return encode_a32_arith(p, &nested, 0x08, 0x0c, 0x0d, 1, "or", 1);
        }
        if(token_is(nested.name, XOR_INSTRUCTION)) {
            return encode_a32_arith(p, &nested, 0x30, 0x34, 0x35, 6, "xor", 1);
        }
        if(token_is(nested.name, SBB_INSTRUCTION)) {
            return encode_a32_arith(p, &nested, 0x18, 0x1c, 0x1d, 3, "sbb", 1);
        }
        if(token_is(nested.name, SUB_INSTRUCTION)) {
            return encode_a32_arith(p, &nested, 0x28, 0x2c, 0x2d, 5, "sub", 1);
        }
        if(token_is(nested.name, CMP_INSTRUCTION)) {
            return encode_a32_arith(p, &nested, 0x38, 0x3c, 0x3d, 7, "cmp", 1);
        }
        if(token_is(nested.name, DEC_INSTRUCTION)) {
            return encode_prefixed_inc_dec(p, &nested, 1, 0x48, "dec", 1, 0, 1);
        }
        if(token_is(nested.name, ARPL_INSTRUCTION)) {
            return encode_arpl_prefix(p, &nested, 1, 1);
        }
        if(token_is(nested.name, BOUND_INSTRUCTION)) {
            return encode_bound_prefixed(p, &nested, 1, 0, 1);
        }
        if(token_is(nested.name, BSF_INSTRUCTION)) {
            return encode_bit_scan_prefixed(p, &nested, 0xbc,
                                            "unsupported bsf form", 1, 0, 1);
        }
        if(token_is(nested.name, BSR_INSTRUCTION)) {
            return encode_bit_scan_prefixed(p, &nested, 0xbd,
                                            "unsupported bsr form", 1, 0, 1);
        }
        if(token_is(nested.name, BT_INSTRUCTION)) {
            return encode_bit_test_prefixed(p, &nested, 0xa3, 4,
                                            "unsupported bt form", 1, 0, 1);
        }
        if(token_is(nested.name, BTC_INSTRUCTION)) {
            return encode_bit_test_prefixed(p, &nested, 0xbb, 7,
                                            "unsupported btc form", 1, 0, 1);
        }
        if(token_is(nested.name, BTR_INSTRUCTION)) {
            return encode_bit_test_prefixed(p, &nested, 0xb3, 6,
                                            "unsupported btr form", 1, 0, 1);
        }
        if(token_is(nested.name, BTS_INSTRUCTION)) {
            return encode_bit_test_prefixed(p, &nested, 0xab, 5,
                                            "unsupported bts form", 1, 0, 1);
        }
    }

    _error_from_token(p, inner.name, ERROR_TYPE_I386,
                      "unsupported a32 instruction");
    return INSTRUCTION_FAILED;
}

static nint encode_o32_prefix(struct Parser *p, struct AstInstruction *inst) {
    struct AstInstruction inner;
    struct InstructionArg inner_args[3];
    nint status = build_prefixed_instruction(p, inst, &inner, inner_args);
    unsigned char short_opcode = 0;
    unsigned char near_opcode = 0;
    if(status <= 0) return status;

    status = encode_misc_prefixed_instruction(p, &inner, 1, 0, 0, 0);
    if(status != 0) return status;

    if(token_is(inner.name, AAA_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, &inner, 0x37, 1, 0);
    }
    if(token_is(inner.name, AAS_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, &inner, 0x3f, 1, 0);
    }
    if(token_is(inner.name, CBW_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, &inner, 0x98, 1, 0);
    }
    if(token_is(inner.name, CDQ_INSTRUCTION)) {
        return emit_prefixed_zero_op_byte(p, &inner, 0x99, 1, 0);
    }
    status = emit_prefixed_convert(p, &inner, 1, 0, 0);
    if(status != 0) return status;
    status = emit_prefixed_decimal_adjust(p, &inner, 1, 0);
    if(status != 0) return status;
    status = emit_prefixed_flag_control(p, &inner, 1, 0);
    if(status != 0) return status;
    status = emit_prefixed_cmps(p, &inner, 1, 0);
    if(status != 0) return status;
    if(token_is(inner.name, AAD_INSTRUCTION)) {
        return emit_prefixed_ascii_imm(p, &inner, 0xd5, 1, 0, "aad");
    }
    if(token_is(inner.name, AAM_INSTRUCTION)) {
        return emit_prefixed_ascii_imm(p, &inner, 0xd4, 1, 0, "aam");
    }
    if(token_is(inner.name, A32_INSTRUCTION)) {
        struct AstInstruction nested;
        struct InstructionArg nested_args[3];
        status = build_prefixed_instruction(p, &inner, &nested, nested_args);
        if(status <= 0) return status;
        status = encode_misc_prefixed_instruction(p, &nested, 1, 0, 1, 0);
        if(status != 0) return status;
        if(token_is(nested.name, AAA_INSTRUCTION)) {
            return emit_prefixed_zero_op_byte(p, &nested, 0x37, 1, 1);
        }
        if(token_is(nested.name, AAS_INSTRUCTION)) {
            return emit_prefixed_zero_op_byte(p, &nested, 0x3f, 1, 1);
        }
        if(token_is(nested.name, CBW_INSTRUCTION)) {
            return emit_prefixed_zero_op_byte(p, &nested, 0x98, 1, 1);
        }
        if(token_is(nested.name, CDQ_INSTRUCTION)) {
            return emit_prefixed_zero_op_byte(p, &nested, 0x99, 1, 1);
        }
        status = emit_prefixed_convert(p, &nested, 1, 1, 0);
        if(status != 0) return status;
        status = emit_prefixed_decimal_adjust(p, &nested, 1, 1);
        if(status != 0) return status;
        status = emit_prefixed_flag_control(p, &nested, 1, 1);
        if(status != 0) return status;
        status = emit_prefixed_cmps(p, &nested, 1, 1);
        if(status != 0) return status;
        if(token_is(nested.name, AAD_INSTRUCTION)) {
            return emit_prefixed_ascii_imm(p, &nested, 0xd5, 1, 1, "aad");
        }
        if(token_is(nested.name, AAM_INSTRUCTION)) {
            return emit_prefixed_ascii_imm(p, &nested, 0xd4, 1, 1, "aam");
        }
        if(token_is(nested.name, ADC_INSTRUCTION)) {
            return encode_a32_arith(p, &nested, 0x10, 0x14, 0x15, 2, "adc", 1);
        }
        if(token_is(nested.name, ADD_INSTRUCTION)) {
            return encode_a32_arith(p, &nested, 0x00, 0x04, 0x05, 0, "add", 1);
        }
        if(token_is(nested.name, AND_INSTRUCTION)) {
            return encode_a32_arith(p, &nested, 0x20, 0x24, 0x25, 4, "and", 1);
        }
        if(token_is(nested.name, OR_INSTRUCTION)) {
            return encode_a32_arith(p, &nested, 0x08, 0x0c, 0x0d, 1, "or", 1);
        }
        if(token_is(nested.name, XOR_INSTRUCTION)) {
            return encode_a32_arith(p, &nested, 0x30, 0x34, 0x35, 6, "xor", 1);
        }
        if(token_is(nested.name, SBB_INSTRUCTION)) {
            return encode_a32_arith(p, &nested, 0x18, 0x1c, 0x1d, 3, "sbb", 1);
        }
        if(token_is(nested.name, SUB_INSTRUCTION)) {
            return encode_a32_arith(p, &nested, 0x28, 0x2c, 0x2d, 5, "sub", 1);
        }
        if(token_is(nested.name, CMP_INSTRUCTION)) {
            return encode_a32_arith(p, &nested, 0x38, 0x3c, 0x3d, 7, "cmp", 1);
        }
        if(token_is(nested.name, DEC_INSTRUCTION)) {
            return encode_prefixed_inc_dec(p, &nested, 1, 0x48, "dec", 1, 0, 1);
        }
        if(token_is(nested.name, ARPL_INSTRUCTION)) {
            return encode_arpl_prefix(p, &nested, 1, 1);
        }
        if(token_is(nested.name, BOUND_INSTRUCTION)) {
            return encode_bound_prefixed(p, &nested, 1, 0, 1);
        }
        if(token_is(nested.name, BSF_INSTRUCTION)) {
            return encode_bit_scan_prefixed(p, &nested, 0xbc,
                                            "unsupported bsf form", 1, 0, 1);
        }
        if(token_is(nested.name, BSR_INSTRUCTION)) {
            return encode_bit_scan_prefixed(p, &nested, 0xbd,
                                            "unsupported bsr form", 1, 0, 1);
        }
        if(token_is(nested.name, BT_INSTRUCTION)) {
            return encode_bit_test_prefixed(p, &nested, 0xa3, 4,
                                            "unsupported bt form", 1, 0, 1);
        }
        if(token_is(nested.name, BTC_INSTRUCTION)) {
            return encode_bit_test_prefixed(p, &nested, 0xbb, 7,
                                            "unsupported btc form", 1, 0, 1);
        }
        if(token_is(nested.name, BTR_INSTRUCTION)) {
            return encode_bit_test_prefixed(p, &nested, 0xb3, 6,
                                            "unsupported btr form", 1, 0, 1);
        }
        if(token_is(nested.name, BTS_INSTRUCTION)) {
            return encode_bit_test_prefixed(p, &nested, 0xab, 5,
                                            "unsupported bts form", 1, 0, 1);
        }
        _error_from_token(p, nested.name, ERROR_TYPE_I386,
                          "unsupported o32 a32 instruction");
        return INSTRUCTION_FAILED;
    }
    if(token_is(inner.name, A16_INSTRUCTION)) {
        struct AstInstruction nested;
        struct InstructionArg nested_args[3];
        status = build_prefixed_instruction(p, &inner, &nested, nested_args);
        if(status <= 0) return status;

        status = encode_misc_prefixed_instruction(p, &nested, 1, 1, 0, 0);
        if(status != 0) return status;

        if(token_is(nested.name, AAA_INSTRUCTION)) {
            return emit_prefixed_zero_op_byte(p, &nested, 0x37, 1, 0);
        }
        if(token_is(nested.name, AAS_INSTRUCTION)) {
            return emit_prefixed_zero_op_byte(p, &nested, 0x3f, 1, 0);
        }
        if(token_is(nested.name, CBW_INSTRUCTION)) {
            return emit_prefixed_zero_op_byte(p, &nested, 0x98, 1, 0);
        }
        if(token_is(nested.name, CDQ_INSTRUCTION)) {
            return emit_prefixed_zero_op_byte(p, &nested, 0x99, 1, 0);
        }
        status = emit_prefixed_convert(p, &nested, 1, 0, 0);
        if(status != 0) return status;
        status = emit_prefixed_decimal_adjust(p, &nested, 1, 0);
        if(status != 0) return status;
        status = emit_prefixed_flag_control(p, &nested, 1, 0);
        if(status != 0) return status;
        status = emit_prefixed_cmps(p, &nested, 1, 0);
        if(status != 0) return status;
        if(token_is(nested.name, AAD_INSTRUCTION)) {
            return emit_prefixed_ascii_imm(p, &nested, 0xd5, 1, 0, "aad");
        }
        if(token_is(nested.name, AAM_INSTRUCTION)) {
            return emit_prefixed_ascii_imm(p, &nested, 0xd4, 1, 0, "aam");
        }
        if(token_is(nested.name, ADD_INSTRUCTION)) {
            return encode_a16_arith(p, &nested, 0x00, 0x04, 0x05, 0, "add", 1);
        }
        if(token_is(nested.name, ADC_INSTRUCTION)) {
            return encode_a16_arith(p, &nested, 0x10, 0x14, 0x15, 2, "adc", 1);
        }
        if(token_is(nested.name, AND_INSTRUCTION)) {
            return encode_a16_arith(p, &nested, 0x20, 0x24, 0x25, 4, "and", 1);
        }
        if(token_is(nested.name, OR_INSTRUCTION)) {
            return encode_a16_arith(p, &nested, 0x08, 0x0c, 0x0d, 1, "or", 1);
        }
        if(token_is(nested.name, XOR_INSTRUCTION)) {
            return encode_a16_arith(p, &nested, 0x30, 0x34, 0x35, 6, "xor", 1);
        }
        if(token_is(nested.name, SBB_INSTRUCTION)) {
            return encode_a16_arith(p, &nested, 0x18, 0x1c, 0x1d, 3, "sbb", 1);
        }
        if(token_is(nested.name, SUB_INSTRUCTION)) {
            return encode_a16_arith(p, &nested, 0x28, 0x2c, 0x2d, 5, "sub", 1);
        }
        if(token_is(nested.name, CMP_INSTRUCTION)) {
            return encode_a16_arith(p, &nested, 0x38, 0x3c, 0x3d, 7, "cmp", 1);
        }
        if(token_is(nested.name, DEC_INSTRUCTION)) {
            return encode_prefixed_inc_dec(p, &nested, 1, 0x48, "dec", 1, 1, 0);
        }
        if(token_is(nested.name, O16_INSTRUCTION)) {
            struct AstInstruction o16_nested;
            struct InstructionArg o16_args[3];
            status = build_prefixed_instruction(p, &nested, &o16_nested, o16_args);
            if(status <= 0) return status;
            status = emit_prefixed_convert(p, &o16_nested, 0, 0, 1);
            if(status != 0) return status;
            status = emit_prefixed_decimal_adjust(p, &o16_nested, 0, 0);
            if(status != 0) return status;
            if(token_is(o16_nested.name, DEC_INSTRUCTION)) {
                return encode_prefixed_inc_dec(p, &o16_nested, 1, 0x48,
                                               "dec", 0, 1, 0);
            }
        }
        if(token_is(nested.name, ARPL_INSTRUCTION)) {
            return encode_a16_arpl(p, &nested, 1);
        }
        if(token_is(nested.name, BOUND_INSTRUCTION)) {
            return encode_bound_prefixed(p, &nested, 1, 1, 0);
        }
        if(token_is(nested.name, BSF_INSTRUCTION)) {
            return encode_bit_scan_prefixed(p, &nested, 0xbc,
                                            "unsupported bsf form", 1, 1, 0);
        }
        if(token_is(nested.name, BSR_INSTRUCTION)) {
            return encode_bit_scan_prefixed(p, &nested, 0xbd,
                                            "unsupported bsr form", 1, 1, 0);
        }
        if(token_is(nested.name, BT_INSTRUCTION)) {
            return encode_bit_test_prefixed(p, &nested, 0xa3, 4,
                                            "unsupported bt form", 1, 1, 0);
        }
        if(token_is(nested.name, BTC_INSTRUCTION)) {
            return encode_bit_test_prefixed(p, &nested, 0xbb, 7,
                                            "unsupported btc form", 1, 1, 0);
        }
        if(token_is(nested.name, BTR_INSTRUCTION)) {
            return encode_bit_test_prefixed(p, &nested, 0xb3, 6,
                                            "unsupported btr form", 1, 1, 0);
        }
        if(token_is(nested.name, BTS_INSTRUCTION)) {
            return encode_bit_test_prefixed(p, &nested, 0xab, 5,
                                            "unsupported bts form", 1, 1, 0);
        }
        _error_from_token(p, nested.name, ERROR_TYPE_I386,
                          "unsupported o32 a16 instruction");
        return INSTRUCTION_FAILED;
    }
    if(token_is(inner.name, ADC_INSTRUCTION)) {
        return encode_o32_arith(p, &inner, 0x10, 0x14, 0x15, 2, "adc");
    }
    if(token_is(inner.name, ADD_INSTRUCTION)) {
        return encode_o32_arith(p, &inner, 0x00, 0x04, 0x05, 0, "add");
    }
    if(token_is(inner.name, AND_INSTRUCTION)) {
        return encode_o32_arith(p, &inner, 0x20, 0x24, 0x25, 4, "and");
    }
    if(token_is(inner.name, OR_INSTRUCTION)) {
        return encode_o32_arith(p, &inner, 0x08, 0x0c, 0x0d, 1, "or");
    }
    if(token_is(inner.name, SBB_INSTRUCTION)) {
        return encode_o32_arith(p, &inner, 0x18, 0x1c, 0x1d, 3, "sbb");
    }
    if(token_is(inner.name, SUB_INSTRUCTION)) {
        return encode_o32_arith(p, &inner, 0x28, 0x2c, 0x2d, 5, "sub");
    }
    if(token_is(inner.name, ARPL_INSTRUCTION)) {
        return encode_arpl_prefix(p, &inner, 1, 0);
    }
    if(token_is(inner.name, BOUND_INSTRUCTION)) {
        return encode_bound_prefixed(p, &inner, 1, 0, 0);
    }
    if(token_is(inner.name, BSF_INSTRUCTION)) {
        return encode_bit_scan_prefixed(p, &inner, 0xbc,
                                        "unsupported bsf form", 1, 0, 0);
    }
    if(token_is(inner.name, BSR_INSTRUCTION)) {
        return encode_bit_scan_prefixed(p, &inner, 0xbd,
                                        "unsupported bsr form", 1, 0, 0);
    }
    if(token_is(inner.name, BT_INSTRUCTION)) {
        return encode_bit_test_prefixed(p, &inner, 0xa3, 4,
                                        "unsupported bt form", 1, 0, 0);
    }
    if(token_is(inner.name, BTC_INSTRUCTION)) {
        return encode_bit_test_prefixed(p, &inner, 0xbb, 7,
                                        "unsupported btc form", 1, 0, 0);
    }
    if(token_is(inner.name, BTR_INSTRUCTION)) {
        return encode_bit_test_prefixed(p, &inner, 0xb3, 6,
                                        "unsupported btr form", 1, 0, 0);
    }
    if(token_is(inner.name, BTS_INSTRUCTION)) {
        return encode_bit_test_prefixed(p, &inner, 0xab, 5,
                                        "unsupported bts form", 1, 0, 0);
    }
    if(token_is(inner.name, MOV_INSTRUCTION)) return encode_o32_mov(p, &inner);
    if(token_is(inner.name, PUSH_INSTRUCTION)) return encode_o32_push(p, &inner);
    if(token_is(inner.name, POP_INSTRUCTION)) return encode_o32_pop(p, &inner);
    if(token_is(inner.name, CMP_INSTRUCTION)) {
        return encode_o32_arith(p, &inner, 0x38, 0x3c, 0x3d, 7, "cmp");
    }
    if(token_is(inner.name, DEC_INSTRUCTION)) {
        return encode_prefixed_inc_dec(p, &inner, 1, 0x48, "dec", 1, 0, 0);
    }
    if(token_is(inner.name, XOR_INSTRUCTION)) {
        return encode_o32_arith(p, &inner, 0x30, 0x34, 0x35, 6, "xor");
    }
    if(token_is(inner.name, NOT_INSTRUCTION)) {
        return encode_o32_unary_group(p, &inner, 2, "not");
    }
    if(token_is(inner.name, TEST_INSTRUCTION)) return encode_o32_test(p, &inner);
    if(token_is(inner.name, IMUL_INSTRUCTION)) return encode_o32_imul(p, &inner);
    if(token_is(inner.name, SHL_INSTRUCTION) || token_is(inner.name, SAL_INSTRUCTION)) {
        return encode_o32_shift_group(p, &inner, 4);
    }
    if(token_is(inner.name, SHR_INSTRUCTION)) {
        return encode_o32_shift_group(p, &inner, 5);
    }
    if(token_is(inner.name, JMP_INSTRUCTION)) return encode_o32_jmp(p, &inner);
    if(token_is(inner.name, CALL_INSTRUCTION)) return encode_o32_call(p, &inner);
    if(jcc_opcodes_from_name(inner.name, &short_opcode, &near_opcode)) {
        return encode_o32_jcc(p, &inner, short_opcode, near_opcode);
    }

    if(!token_is(inner.name, RET_INSTRUCTION) &&
       !token_is(inner.name, RETN_INSTRUCTION) &&
       !token_is(inner.name, RETF_INSTRUCTION)) {
        _error_from_token(p, inner.name, ERROR_TYPE_I386,
                          "unsupported o32 instruction");
        return INSTRUCTION_FAILED;
    }

    emit_operand_size_prefix(p, 4);
    status = _86_exec(p, &inner);
    if(status < 0) return status;
    return status + 1;
}

static unint lock_allows_arith(struct token *name) {
    return token_is(name, ADD_INSTRUCTION) || token_is(name, ADC_INSTRUCTION) ||
           token_is(name, AND_INSTRUCTION) || token_is(name, OR_INSTRUCTION) ||
           token_is(name, SBB_INSTRUCTION) || token_is(name, SUB_INSTRUCTION) ||
           token_is(name, XOR_INSTRUCTION);
}

static unint lock_allows_unary(struct token *name) {
    return token_is(name, INC_INSTRUCTION) || token_is(name, DEC_INSTRUCTION) ||
           token_is(name, NEG_INSTRUCTION) || token_is(name, NOT_INSTRUCTION);
}

static nint parse_lock_operand(struct Parser *p, struct InstructionArg *arg,
                               struct Operand *op, const char *message) {
    struct token *error_token = NULL;
    nint status = parse_operand(p, arg, op, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386, message);
        return INSTRUCTION_FAILED;
    }
    return 1;
}

static nint validate_lock_arith(struct Parser *p, struct AstInstruction *inner) {
    struct Operand dst;
    struct Operand src;
    nint status;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_lock_operand(p, inner->args_head, &dst, "invalid first operand");
    if(status <= 0) return status;
    status = parse_lock_operand(p, inner->args_tail, &src, "invalid second operand");
    if(status <= 0) return status;

    if(dst.kind != OPERAND_MEM) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "lock requires memory destination");
        return INSTRUCTION_FAILED;
    }

    if(src.kind == OPERAND_REG8) {
        if(!memory_size_matches(&dst, 1)) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "invalid lock operand size");
            return INSTRUCTION_FAILED;
        }
        return 1;
    }

    if(src.kind == OPERAND_REG16) {
        if(!memory_size_matches(&dst, 2)) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "invalid lock operand size");
            return INSTRUCTION_FAILED;
        }
        return 1;
    }

    if(src.kind == OPERAND_IMM) {
        if(dst.size != 1 && dst.size != 2) {
            _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                              "ambiguous lock operand size");
            return INSTRUCTION_FAILED;
        }
        if(!fits_u16(src.imm)) {
            _error_from_token(p, inner->args_tail->_s, ERROR_TYPE_OVERFLOW,
                              "immediate does not fit 16 bits");
            return INSTRUCTION_FAILED;
        }
        return 1;
    }

    _error_from_token(p, inner->name, ERROR_TYPE_I386, "unsupported lock form");
    return INSTRUCTION_FAILED;
}

static nint validate_lock_xchg(struct Parser *p, struct AstInstruction *inner) {
    struct Operand dst;
    struct Operand src;
    struct Operand *mem;
    struct Operand *reg;
    nint status;

    if(inner->arg_count != 2) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_lock_operand(p, inner->args_head, &dst, "invalid first operand");
    if(status <= 0) return status;
    status = parse_lock_operand(p, inner->args_tail, &src, "invalid second operand");
    if(status <= 0) return status;

    if(dst.kind == OPERAND_MEM) {
        mem = &dst;
        reg = &src;
    } else if(src.kind == OPERAND_MEM) {
        mem = &src;
        reg = &dst;
    } else {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "lock requires memory operand");
        return INSTRUCTION_FAILED;
    }

    if(reg->kind == OPERAND_REG8) {
        if(!memory_size_matches(mem, 1)) {
            _error_from_token(p, inner->name, ERROR_TYPE_I386,
                              "invalid lock operand size");
            return INSTRUCTION_FAILED;
        }
        return 1;
    }

    if(reg->kind == OPERAND_REG16) {
        if(!memory_size_matches(mem, 2)) {
            _error_from_token(p, inner->name, ERROR_TYPE_I386,
                              "invalid lock operand size");
            return INSTRUCTION_FAILED;
        }
        return 1;
    }

    if(reg->kind == OPERAND_REG32) {
        if(!memory_size_matches(mem, 4)) {
            _error_from_token(p, inner->name, ERROR_TYPE_I386,
                              "invalid lock operand size");
            return INSTRUCTION_FAILED;
        }
        return 1;
    }

    _error_from_token(p, inner->name, ERROR_TYPE_I386, "unsupported lock form");
    return INSTRUCTION_FAILED;
}

static nint validate_lock_unary(struct Parser *p, struct AstInstruction *inner) {
    struct Operand dst;
    nint status;

    if(inner->arg_count != 1) {
        _error_from_token(p, inner->name, ERROR_TYPE_I386,
                          "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    status = parse_lock_operand(p, inner->args_head, &dst, "invalid operand");
    if(status <= 0) return status;

    if(dst.kind != OPERAND_MEM) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "lock requires memory operand");
        return INSTRUCTION_FAILED;
    }

    if(dst.size != 1 && dst.size != 2) {
        _error_from_token(p, inner->args_head->_s, ERROR_TYPE_I386,
                          "ambiguous lock operand size");
        return INSTRUCTION_FAILED;
    }

    return 1;
}

static nint validate_lock_instruction(struct Parser *p, struct AstInstruction *inner) {
    if(lock_allows_arith(inner->name)) return validate_lock_arith(p, inner);
    if(token_is(inner->name, XCHG_INSTRUCTION)) return validate_lock_xchg(p, inner);
    if(lock_allows_unary(inner->name)) return validate_lock_unary(p, inner);

    _error_from_token(p, inner->name, ERROR_TYPE_I386,
                      "unsupported lock instruction");
    return INSTRUCTION_FAILED;
}

static nint encode_lock_prefix(struct Parser *p, struct AstInstruction *inst) {
    struct AstInstruction inner;
    struct InstructionArg inner_args[3];
    nint status = build_prefixed_instruction(p, inst, &inner, inner_args);
    if(status <= 0) return status;

    status = validate_lock_instruction(p, &inner);
    if(status <= 0) return status;

    emit_byte(p, 0xf0);
    status = _86_exec(p, &inner);
    if(status < 0) return status;
    return status + 1;
}

nint _86_exec(struct Parser *p, struct AstInstruction *inst) {
    struct Operand dst;
    struct Operand src;
    struct token *error_token = NULL;
    nint status;
    unint is_mov = compare_identifiers_cp_array(inst->name, MOV_INSTRUCTION) == SUCCESS;
    unint is_lea = compare_identifiers_cp_array(inst->name, LEA_INSTRUCTION) == SUCCESS;
    unint is_lds = compare_identifiers_cp_array(inst->name, LDS_INSTRUCTION) == SUCCESS;
    unint is_les = compare_identifiers_cp_array(inst->name, LES_INSTRUCTION) == SUCCESS;
    unint is_lfs = compare_identifiers_cp_array(inst->name, LFS_INSTRUCTION) == SUCCESS;
    unint is_lgs = compare_identifiers_cp_array(inst->name, LGS_INSTRUCTION) == SUCCESS;
    unint is_lss = compare_identifiers_cp_array(inst->name, LSS_INSTRUCTION) == SUCCESS;
    unint is_movsx = compare_identifiers_cp_array(inst->name, MOVSX_INSTRUCTION) == SUCCESS;
    unint is_movzx = compare_identifiers_cp_array(inst->name, MOVZX_INSTRUCTION) == SUCCESS;
    unint is_bound = compare_identifiers_cp_array(inst->name, BOUND_INSTRUCTION) == SUCCESS;
    unint is_jmp = compare_identifiers_cp_array(inst->name, JMP_INSTRUCTION) == SUCCESS;
    unint is_call = compare_identifiers_cp_array(inst->name, CALL_INSTRUCTION) == SUCCESS;
    unint is_ret = compare_identifiers_cp_array(inst->name, RET_INSTRUCTION) == SUCCESS;
    unint is_retn = compare_identifiers_cp_array(inst->name, RETN_INSTRUCTION) == SUCCESS;
    unint is_retf = compare_identifiers_cp_array(inst->name, RETF_INSTRUCTION) == SUCCESS;
    unint is_enter = compare_identifiers_cp_array(inst->name, ENTER_INSTRUCTION) == SUCCESS;
    unint is_leave = compare_identifiers_cp_array(inst->name, LEAVE_INSTRUCTION) == SUCCESS;
    unint is_xchg = compare_identifiers_cp_array(inst->name, XCHG_INSTRUCTION) == SUCCESS;
    unint is_push = compare_identifiers_cp_array(inst->name, PUSH_INSTRUCTION) == SUCCESS;
    unint is_pop = compare_identifiers_cp_array(inst->name, POP_INSTRUCTION) == SUCCESS;
    unint is_pusha = compare_identifiers_cp_array(inst->name, PUSHA_INSTRUCTION) == SUCCESS;
    unint is_pushaw = compare_identifiers_cp_array(inst->name, PUSHAW_INSTRUCTION) == SUCCESS;
    unint is_pushad = compare_identifiers_cp_array(inst->name, PUSHAD_INSTRUCTION) == SUCCESS;
    unint is_popa = compare_identifiers_cp_array(inst->name, POPA_INSTRUCTION) == SUCCESS;
    unint is_popaw = compare_identifiers_cp_array(inst->name, POPAW_INSTRUCTION) == SUCCESS;
    unint is_popad = compare_identifiers_cp_array(inst->name, POPAD_INSTRUCTION) == SUCCESS;
    unint is_pushf = compare_identifiers_cp_array(inst->name, PUSHF_INSTRUCTION) == SUCCESS;
    unint is_pushfd = compare_identifiers_cp_array(inst->name, PUSHFD_INSTRUCTION) == SUCCESS;
    unint is_popf = compare_identifiers_cp_array(inst->name, POPF_INSTRUCTION) == SUCCESS;
    unint is_popfd = compare_identifiers_cp_array(inst->name, POPFD_INSTRUCTION) == SUCCESS;
    unint is_lahf = compare_identifiers_cp_array(inst->name, LAHF_INSTRUCTION) == SUCCESS;
    unint is_sahf = compare_identifiers_cp_array(inst->name, SAHF_INSTRUCTION) == SUCCESS;
    unint is_xlat = compare_identifiers_cp_array(inst->name, XLAT_INSTRUCTION) == SUCCESS;
    unint is_xlatb = compare_identifiers_cp_array(inst->name, XLATB_INSTRUCTION) == SUCCESS;
    unint is_movs = compare_identifiers_cp_array(inst->name, MOVS_INSTRUCTION) == SUCCESS;
    unint is_movsb = compare_identifiers_cp_array(inst->name, MOVSB_INSTRUCTION) == SUCCESS;
    unint is_movsw = compare_identifiers_cp_array(inst->name, MOVSW_INSTRUCTION) == SUCCESS;
    unint is_movsd = compare_identifiers_cp_array(inst->name, MOVSD_INSTRUCTION) == SUCCESS;
    unint is_cmps = compare_identifiers_cp_array(inst->name, CMPS_INSTRUCTION) == SUCCESS;
    unint is_cmpsb = compare_identifiers_cp_array(inst->name, CMPSB_INSTRUCTION) == SUCCESS;
    unint is_cmpsw = compare_identifiers_cp_array(inst->name, CMPSW_INSTRUCTION) == SUCCESS;
    unint is_cmpsd = compare_identifiers_cp_array(inst->name, CMPSD_INSTRUCTION) == SUCCESS;
    unint is_scas = compare_identifiers_cp_array(inst->name, SCAS_INSTRUCTION) == SUCCESS;
    unint is_scasb = compare_identifiers_cp_array(inst->name, SCASB_INSTRUCTION) == SUCCESS;
    unint is_scasw = compare_identifiers_cp_array(inst->name, SCASW_INSTRUCTION) == SUCCESS;
    unint is_scasd = compare_identifiers_cp_array(inst->name, SCASD_INSTRUCTION) == SUCCESS;
    unint is_lods = compare_identifiers_cp_array(inst->name, LODS_INSTRUCTION) == SUCCESS;
    unint is_lodsb = compare_identifiers_cp_array(inst->name, LODSB_INSTRUCTION) == SUCCESS;
    unint is_lodsw = compare_identifiers_cp_array(inst->name, LODSW_INSTRUCTION) == SUCCESS;
    unint is_lodsd = compare_identifiers_cp_array(inst->name, LODSD_INSTRUCTION) == SUCCESS;
    unint is_stos = compare_identifiers_cp_array(inst->name, STOS_INSTRUCTION) == SUCCESS;
    unint is_stosb = compare_identifiers_cp_array(inst->name, STOSB_INSTRUCTION) == SUCCESS;
    unint is_stosw = compare_identifiers_cp_array(inst->name, STOSW_INSTRUCTION) == SUCCESS;
    unint is_stosd = compare_identifiers_cp_array(inst->name, STOSD_INSTRUCTION) == SUCCESS;
    unint is_insb = compare_identifiers_cp_array(inst->name, INSB_INSTRUCTION) == SUCCESS;
    unint is_insw = compare_identifiers_cp_array(inst->name, INSW_INSTRUCTION) == SUCCESS;
    unint is_insd = compare_identifiers_cp_array(inst->name, INSD_INSTRUCTION) == SUCCESS;
    unint is_outs = compare_identifiers_cp_array(inst->name, OUTS_INSTRUCTION) == SUCCESS;
    unint is_outsb = compare_identifiers_cp_array(inst->name, OUTSB_INSTRUCTION) == SUCCESS;
    unint is_outsw = compare_identifiers_cp_array(inst->name, OUTSW_INSTRUCTION) == SUCCESS;
    unint is_outsd = compare_identifiers_cp_array(inst->name, OUTSD_INSTRUCTION) == SUCCESS;
    unint is_lock = compare_identifiers_cp_array(inst->name, LOCK_INSTRUCTION) == SUCCESS;
    unint is_rep = compare_identifiers_cp_array(inst->name, REP_INSTRUCTION) == SUCCESS;
    unint is_repe = compare_identifiers_cp_array(inst->name, REPE_INSTRUCTION) == SUCCESS;
    unint is_repz = compare_identifiers_cp_array(inst->name, REPZ_INSTRUCTION) == SUCCESS;
    unint is_repne = compare_identifiers_cp_array(inst->name, REPNE_INSTRUCTION) == SUCCESS;
    unint is_repnz = compare_identifiers_cp_array(inst->name, REPNZ_INSTRUCTION) == SUCCESS;
    unint is_o16 = compare_identifiers_cp_array(inst->name, O16_INSTRUCTION) == SUCCESS;
    unint is_o32 = compare_identifiers_cp_array(inst->name, O32_INSTRUCTION) == SUCCESS;
    unint is_a16 = compare_identifiers_cp_array(inst->name, A16_INSTRUCTION) == SUCCESS;
    unint is_a32 = compare_identifiers_cp_array(inst->name, A32_INSTRUCTION) == SUCCESS;
    unint is_in = compare_identifiers_cp_array(inst->name, IN_INSTRUCTION) == SUCCESS;
    unint is_out = compare_identifiers_cp_array(inst->name, OUT_INSTRUCTION) == SUCCESS;
    unint is_intr = compare_identifiers_cp_array(inst->name, INT_INSTRUCTION) == SUCCESS;
    unint is_into = compare_identifiers_cp_array(inst->name, INTO_INSTRUCTION) == SUCCESS;
    unint is_iret = compare_identifiers_cp_array(inst->name, IRET_INSTRUCTION) == SUCCESS;
    unint is_iretd = compare_identifiers_cp_array(inst->name, IRETD_INSTRUCTION) == SUCCESS;
    unint is_loop = compare_identifiers_cp_array(inst->name, LOOP_INSTRUCTION) == SUCCESS;
    unint is_loope = compare_identifiers_cp_array(inst->name, LOOPE_INSTRUCTION) == SUCCESS;
    unint is_loopz = compare_identifiers_cp_array(inst->name, LOOPZ_INSTRUCTION) == SUCCESS;
    unint is_loopne = compare_identifiers_cp_array(inst->name, LOOPNE_INSTRUCTION) == SUCCESS;
    unint is_loopnz = compare_identifiers_cp_array(inst->name, LOOPNZ_INSTRUCTION) == SUCCESS;
    unint is_jcxz = compare_identifiers_cp_array(inst->name, JCXZ_INSTRUCTION) == SUCCESS;
    unint is_jecxz = compare_identifiers_cp_array(inst->name, JECXZ_INSTRUCTION) == SUCCESS;
    unint is_jo = compare_identifiers_cp_array(inst->name, JO_INSTRUCTION) == SUCCESS;
    unint is_jno = compare_identifiers_cp_array(inst->name, JNO_INSTRUCTION) == SUCCESS;
    unint is_jb = compare_identifiers_cp_array(inst->name, JB_INSTRUCTION) == SUCCESS;
    unint is_jc = compare_identifiers_cp_array(inst->name, JC_INSTRUCTION) == SUCCESS;
    unint is_jnae = compare_identifiers_cp_array(inst->name, JNAE_INSTRUCTION) == SUCCESS;
    unint is_jnb = compare_identifiers_cp_array(inst->name, JNB_INSTRUCTION) == SUCCESS;
    unint is_jae = compare_identifiers_cp_array(inst->name, JAE_INSTRUCTION) == SUCCESS;
    unint is_jnc = compare_identifiers_cp_array(inst->name, JNC_INSTRUCTION) == SUCCESS;
    unint is_je = compare_identifiers_cp_array(inst->name, JE_INSTRUCTION) == SUCCESS;
    unint is_jz = compare_identifiers_cp_array(inst->name, JZ_INSTRUCTION) == SUCCESS;
    unint is_jne = compare_identifiers_cp_array(inst->name, JNE_INSTRUCTION) == SUCCESS;
    unint is_jnz = compare_identifiers_cp_array(inst->name, JNZ_INSTRUCTION) == SUCCESS;
    unint is_jbe = compare_identifiers_cp_array(inst->name, JBE_INSTRUCTION) == SUCCESS;
    unint is_jna = compare_identifiers_cp_array(inst->name, JNA_INSTRUCTION) == SUCCESS;
    unint is_ja = compare_identifiers_cp_array(inst->name, JA_INSTRUCTION) == SUCCESS;
    unint is_jnbe = compare_identifiers_cp_array(inst->name, JNBE_INSTRUCTION) == SUCCESS;
    unint is_js = compare_identifiers_cp_array(inst->name, JS_INSTRUCTION) == SUCCESS;
    unint is_jns = compare_identifiers_cp_array(inst->name, JNS_INSTRUCTION) == SUCCESS;
    unint is_jp = compare_identifiers_cp_array(inst->name, JP_INSTRUCTION) == SUCCESS;
    unint is_jpe = compare_identifiers_cp_array(inst->name, JPE_INSTRUCTION) == SUCCESS;
    unint is_jnp = compare_identifiers_cp_array(inst->name, JNP_INSTRUCTION) == SUCCESS;
    unint is_jpo = compare_identifiers_cp_array(inst->name, JPO_INSTRUCTION) == SUCCESS;
    unint is_jl = compare_identifiers_cp_array(inst->name, JL_INSTRUCTION) == SUCCESS;
    unint is_jnge = compare_identifiers_cp_array(inst->name, JNGE_INSTRUCTION) == SUCCESS;
    unint is_jge = compare_identifiers_cp_array(inst->name, JGE_INSTRUCTION) == SUCCESS;
    unint is_jnl = compare_identifiers_cp_array(inst->name, JNL_INSTRUCTION) == SUCCESS;
    unint is_jle = compare_identifiers_cp_array(inst->name, JLE_INSTRUCTION) == SUCCESS;
    unint is_jng = compare_identifiers_cp_array(inst->name, JNG_INSTRUCTION) == SUCCESS;
    unint is_jg = compare_identifiers_cp_array(inst->name, JG_INSTRUCTION) == SUCCESS;
    unint is_jnle = compare_identifiers_cp_array(inst->name, JNLE_INSTRUCTION) == SUCCESS;
    unsigned char setcc_opcode = 0;
    const char *setcc_name = NULL;
    unint is_setcc = setcc_opcode_from_name(inst->name, &setcc_opcode, &setcc_name);
    unint is_arpl = compare_identifiers_cp_array(inst->name, ARPL_INSTRUCTION) == SUCCESS;
    unint is_add = compare_identifiers_cp_array(inst->name, ADD_INSTRUCTION) == SUCCESS;
    unint is_adc = compare_identifiers_cp_array(inst->name, ADC_INSTRUCTION) == SUCCESS;
    unint is_sub = compare_identifiers_cp_array(inst->name, SUB_INSTRUCTION) == SUCCESS;
    unint is_sbb = compare_identifiers_cp_array(inst->name, SBB_INSTRUCTION) == SUCCESS;
    unint is_cmp = compare_identifiers_cp_array(inst->name, CMP_INSTRUCTION) == SUCCESS;
    unint is_inc = compare_identifiers_cp_array(inst->name, INC_INSTRUCTION) == SUCCESS;
    unint is_dec = compare_identifiers_cp_array(inst->name, DEC_INSTRUCTION) == SUCCESS;
    unint is_neg = compare_identifiers_cp_array(inst->name, NEG_INSTRUCTION) == SUCCESS;
    unint is_mul = compare_identifiers_cp_array(inst->name, MUL_INSTRUCTION) == SUCCESS;
    unint is_imul = compare_identifiers_cp_array(inst->name, IMUL_INSTRUCTION) == SUCCESS;
    unint is_div = compare_identifiers_cp_array(inst->name, DIV_INSTRUCTION) == SUCCESS;
    unint is_idiv = compare_identifiers_cp_array(inst->name, IDIV_INSTRUCTION) == SUCCESS;
    unint is_cbw = compare_identifiers_cp_array(inst->name, CBW_INSTRUCTION) == SUCCESS;
    unint is_cwd = compare_identifiers_cp_array(inst->name, CWD_INSTRUCTION) == SUCCESS;
    unint is_cwde = compare_identifiers_cp_array(inst->name, CWDE_INSTRUCTION) == SUCCESS;
    unint is_cdq = compare_identifiers_cp_array(inst->name, CDQ_INSTRUCTION) == SUCCESS;
    unint is_aaa = compare_identifiers_cp_array(inst->name, AAA_INSTRUCTION) == SUCCESS;
    unint is_aad = compare_identifiers_cp_array(inst->name, AAD_INSTRUCTION) == SUCCESS;
    unint is_aam = compare_identifiers_cp_array(inst->name, AAM_INSTRUCTION) == SUCCESS;
    unint is_aas = compare_identifiers_cp_array(inst->name, AAS_INSTRUCTION) == SUCCESS;
    unint is_daa = compare_identifiers_cp_array(inst->name, DAA_INSTRUCTION) == SUCCESS;
    unint is_das = compare_identifiers_cp_array(inst->name, DAS_INSTRUCTION) == SUCCESS;
    unint is_and = compare_identifiers_cp_array(inst->name, AND_INSTRUCTION) == SUCCESS;
    unint is_or = compare_identifiers_cp_array(inst->name, OR_INSTRUCTION) == SUCCESS;
    unint is_xor = compare_identifiers_cp_array(inst->name, XOR_INSTRUCTION) == SUCCESS;
    unint is_test = compare_identifiers_cp_array(inst->name, TEST_INSTRUCTION) == SUCCESS;
    unint is_not = compare_identifiers_cp_array(inst->name, NOT_INSTRUCTION) == SUCCESS;
    unint is_bsf = compare_identifiers_cp_array(inst->name, BSF_INSTRUCTION) == SUCCESS;
    unint is_bsr = compare_identifiers_cp_array(inst->name, BSR_INSTRUCTION) == SUCCESS;
    unint is_lar = compare_identifiers_cp_array(inst->name, LAR_INSTRUCTION) == SUCCESS;
    unint is_lsl = compare_identifiers_cp_array(inst->name, LSL_INSTRUCTION) == SUCCESS;
    unint is_lgdt = compare_identifiers_cp_array(inst->name, LGDT_INSTRUCTION) == SUCCESS;
    unint is_lidt = compare_identifiers_cp_array(inst->name, LIDT_INSTRUCTION) == SUCCESS;
    unint is_sgdt = compare_identifiers_cp_array(inst->name, SGDT_INSTRUCTION) == SUCCESS;
    unint is_sidt = compare_identifiers_cp_array(inst->name, SIDT_INSTRUCTION) == SUCCESS;
    unint is_lldt = compare_identifiers_cp_array(inst->name, LLDT_INSTRUCTION) == SUCCESS;
    unint is_sldt = compare_identifiers_cp_array(inst->name, SLDT_INSTRUCTION) == SUCCESS;
    unint is_ltr = compare_identifiers_cp_array(inst->name, LTR_INSTRUCTION) == SUCCESS;
    unint is_str = compare_identifiers_cp_array(inst->name, STR_INSTRUCTION) == SUCCESS;
    unint is_lmsw = compare_identifiers_cp_array(inst->name, LMSW_INSTRUCTION) == SUCCESS;
    unint is_smsw = compare_identifiers_cp_array(inst->name, SMSW_INSTRUCTION) == SUCCESS;
    unint is_verr = compare_identifiers_cp_array(inst->name, VERR_INSTRUCTION) == SUCCESS;
    unint is_verw = compare_identifiers_cp_array(inst->name, VERW_INSTRUCTION) == SUCCESS;
    unint is_bt = compare_identifiers_cp_array(inst->name, BT_INSTRUCTION) == SUCCESS;
    unint is_btc = compare_identifiers_cp_array(inst->name, BTC_INSTRUCTION) == SUCCESS;
    unint is_btr = compare_identifiers_cp_array(inst->name, BTR_INSTRUCTION) == SUCCESS;
    unint is_bts = compare_identifiers_cp_array(inst->name, BTS_INSTRUCTION) == SUCCESS;
    unint is_clc = compare_identifiers_cp_array(inst->name, CLC_INSTRUCTION) == SUCCESS;
    unint is_clts = compare_identifiers_cp_array(inst->name, CLTS_INSTRUCTION) == SUCCESS;
    unint is_stc = compare_identifiers_cp_array(inst->name, STC_INSTRUCTION) == SUCCESS;
    unint is_cmc = compare_identifiers_cp_array(inst->name, CMC_INSTRUCTION) == SUCCESS;
    unint is_cld = compare_identifiers_cp_array(inst->name, CLD_INSTRUCTION) == SUCCESS;
    unint is_std = compare_identifiers_cp_array(inst->name, STD_INSTRUCTION) == SUCCESS;
    unint is_cli = compare_identifiers_cp_array(inst->name, CLI_INSTRUCTION) == SUCCESS;
    unint is_sti = compare_identifiers_cp_array(inst->name, STI_INSTRUCTION) == SUCCESS;
    unint is_hlt = compare_identifiers_cp_array(inst->name, HLT_INSTRUCTION) == SUCCESS;
    unint is_wait = compare_identifiers_cp_array(inst->name, WAIT_INSTRUCTION) == SUCCESS;
    unint is_fwait = compare_identifiers_cp_array(inst->name, FWAIT_INSTRUCTION) == SUCCESS;
    unint is_esc = compare_identifiers_cp_array(inst->name, ESC_INSTRUCTION) == SUCCESS;
    unint is_nop = compare_identifiers_cp_array(inst->name, NOP_INSTRUCTION) == SUCCESS;
    unint is_shl = compare_identifiers_cp_array(inst->name, SHL_INSTRUCTION) == SUCCESS;
    unint is_shld = compare_identifiers_cp_array(inst->name, SHLD_INSTRUCTION) == SUCCESS;
    unint is_sal = compare_identifiers_cp_array(inst->name, SAL_INSTRUCTION) == SUCCESS;
    unint is_shr = compare_identifiers_cp_array(inst->name, SHR_INSTRUCTION) == SUCCESS;
    unint is_shrd = compare_identifiers_cp_array(inst->name, SHRD_INSTRUCTION) == SUCCESS;
    unint is_sar = compare_identifiers_cp_array(inst->name, SAR_INSTRUCTION) == SUCCESS;
    unint is_rol = compare_identifiers_cp_array(inst->name, ROL_INSTRUCTION) == SUCCESS;
    unint is_ror = compare_identifiers_cp_array(inst->name, ROR_INSTRUCTION) == SUCCESS;
    unint is_rcl = compare_identifiers_cp_array(inst->name, RCL_INSTRUCTION) == SUCCESS;
    unint is_rcr = compare_identifiers_cp_array(inst->name, RCR_INSTRUCTION) == SUCCESS;

    (void)registers;

    if(is_lock) return encode_lock_prefix(p, inst);
    if(is_repe || is_repz) return encode_repe_prefix(p, inst);
    if(is_repne || is_repnz) return encode_repne_prefix(p, inst);
    if(is_rep) return encode_rep_prefix(p, inst);
    if(is_o16) return encode_o16_prefix(p, inst);
    if(is_o32) return encode_o32_prefix(p, inst);
    if(is_a16) return encode_a16_prefix(p, inst);
    if(is_a32) return encode_a32_prefix(p, inst);

    if(is_outs) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "unsupported outs form");
        return INSTRUCTION_FAILED;
    }

    if(!is_mov && !is_lea && !is_lds && !is_les && !is_lfs && !is_lgs &&
       !is_lss && !is_movsx && !is_movzx && !is_bound && !is_jmp && !is_call &&
       !is_ret && !is_retn && !is_retf && !is_enter && !is_leave && !is_xchg &&
       !is_push && !is_pop && !is_pusha && !is_pushaw && !is_pushad &&
       !is_popa && !is_popaw && !is_popad &&
       !is_pushf && !is_pushfd && !is_popf && !is_popfd && !is_lahf && !is_sahf &&
       !is_xlat && !is_xlatb && !is_movs && !is_movsb && !is_movsw && !is_movsd &&
       !is_cmps && !is_cmpsb && !is_cmpsw && !is_cmpsd &&
       !is_scas && !is_scasb && !is_scasw && !is_scasd &&
       !is_lods && !is_lodsb && !is_lodsw && !is_lodsd &&
       !is_stos && !is_stosb && !is_stosw && !is_stosd &&
       !is_insb && !is_insw && !is_insd &&
       !is_outs && !is_outsb && !is_outsw && !is_outsd &&
       !is_in && !is_out && !is_intr && !is_into && !is_iret && !is_iretd &&
       !is_loop && !is_loope && !is_loopz && !is_loopne && !is_loopnz &&
       !is_jcxz && !is_jecxz && !is_jo && !is_jno && !is_jb && !is_jc && !is_jnae &&
       !is_jnb && !is_jae && !is_jnc && !is_je && !is_jz && !is_jne && !is_jnz &&
       !is_jbe && !is_jna && !is_ja && !is_jnbe &&
       !is_js && !is_jns && !is_jp && !is_jpe && !is_jnp && !is_jpo &&
       !is_jl && !is_jnge && !is_jge && !is_jnl &&
       !is_jle && !is_jng && !is_jg && !is_jnle &&
       !is_setcc && !is_arpl && !is_add && !is_adc && !is_sub && !is_sbb && !is_cmp &&
       !is_inc && !is_dec && !is_neg && !is_mul && !is_imul && !is_div && !is_idiv &&
       !is_cbw && !is_cwd && !is_cwde && !is_cdq &&
       !is_aaa && !is_aad && !is_aam && !is_aas &&
       !is_daa && !is_das && !is_and && !is_or &&
       !is_xor && !is_test && !is_not && !is_bsf && !is_bsr && !is_lar && !is_lsl &&
       !is_lgdt && !is_lidt && !is_sgdt && !is_sidt &&
       !is_lldt && !is_sldt && !is_ltr && !is_str && !is_lmsw && !is_smsw &&
       !is_verr && !is_verw &&
       !is_bt && !is_btc && !is_btr && !is_bts &&
       !is_clc && !is_clts && !is_stc && !is_cmc &&
       !is_cld && !is_std && !is_cli && !is_sti && !is_hlt && !is_wait &&
       !is_fwait && !is_esc && !is_nop && !is_shl && !is_shld && !is_sal &&
       !is_shr && !is_shrd && !is_sar && !is_rol && !is_ror && !is_rcl &&
       !is_rcr) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "invalid i386 instruction");
        return INSTRUCTION_FAILED;
    }

    if((is_pusha || is_pushaw || is_pushad || is_popa || is_popaw || is_popad ||
        is_pushf || is_pushfd || is_popf || is_popfd ||
        is_lahf || is_sahf || is_xlat || is_xlatb ||
        is_movs || is_movsb || is_movsw || is_movsd ||
        is_cmps || is_cmpsb || is_cmpsw || is_cmpsd ||
        is_scas || is_scasb || is_scasw || is_scasd ||
        is_lods || is_lodsb || is_lodsw || is_lodsd ||
        is_stos || is_stosb || is_stosw || is_stosd ||
        is_insb || is_insw || is_insd ||
        is_outsb || is_outsw || is_outsd ||
        is_clc || is_clts || is_stc || is_cmc ||
        is_cld || is_std || is_cli || is_sti || is_hlt || is_wait || is_fwait ||
        is_nop || is_cbw || is_cwd || is_cwde || is_cdq ||
        is_aaa || is_aas || is_daa || is_das || is_into || is_iret || is_iretd ||
        is_leave) &&
       inst->arg_count != 0) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if((is_aad || is_aam) && inst->arg_count > 1) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if((is_push || is_pop || is_setcc || is_not || is_inc || is_dec || is_neg || is_intr ||
        is_mul || is_div || is_idiv || is_lgdt || is_lidt || is_sgdt || is_sidt ||
        is_lldt || is_sldt || is_ltr || is_str || is_lmsw || is_smsw ||
        is_verr || is_verw) &&
       inst->arg_count != 1) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if((is_shld || is_shrd) && inst->arg_count != 3) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if(is_imul && (inst->arg_count < 1 || inst->arg_count > 3)) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if(!is_jmp && !is_call && !is_ret && !is_retn && !is_retf && !is_enter &&
       !is_loop && !is_loope && !is_loopz && !is_loopne && !is_loopnz &&
       !is_jcxz && !is_jecxz && !is_jo && !is_jno && !is_jb && !is_jc && !is_jnae &&
       !is_jnb && !is_jae && !is_jnc && !is_je && !is_jz && !is_jne && !is_jnz &&
       !is_jbe && !is_jna && !is_ja && !is_jnbe &&
       !is_js && !is_jns && !is_jp && !is_jpe && !is_jnp && !is_jpo &&
       !is_jl && !is_jnge && !is_jge && !is_jnl &&
       !is_jle && !is_jng && !is_jg && !is_jnle &&
       !is_setcc &&
       !is_push && !is_pop && !is_pusha && !is_pushaw && !is_pushad &&
       !is_popa && !is_popaw && !is_popad &&
       !is_not && !is_inc && !is_dec && !is_neg &&
       !is_mul && !is_imul && !is_div && !is_idiv &&
       !is_pushf && !is_pushfd && !is_popf && !is_popfd &&
       !is_leave && !is_lahf && !is_sahf && !is_xlat && !is_xlatb && !is_movs &&
       !is_movsb && !is_movsw && !is_movsd &&
       !is_cmps && !is_cmpsb && !is_cmpsw && !is_cmpsd &&
       !is_scas && !is_scasb && !is_scasw && !is_scasd &&
       !is_lods && !is_lodsb && !is_lodsw && !is_lodsd &&
       !is_stos && !is_stosb && !is_stosw && !is_stosd &&
       !is_insb && !is_insw && !is_insd &&
       !is_outs && !is_outsb && !is_outsw && !is_outsd &&
       !is_clc && !is_clts && !is_stc && !is_cmc &&
       !is_cld && !is_std && !is_cli && !is_sti && !is_hlt && !is_wait &&
       !is_fwait && !is_nop && !is_cbw && !is_cwd && !is_cwde && !is_cdq &&
       !is_aaa && !is_aad && !is_aam && !is_aas && !is_daa && !is_das &&
       !is_lgdt && !is_lidt && !is_sgdt && !is_sidt &&
       !is_lldt && !is_sldt && !is_ltr && !is_str && !is_lmsw && !is_smsw &&
       !is_verr && !is_verw &&
       !is_intr && !is_into && !is_iret && !is_iretd &&
       !is_loop && !is_loope && !is_loopz && !is_loopne && !is_loopnz &&
       !is_jcxz && !is_jecxz && !is_jo && !is_jno && !is_jb && !is_jc && !is_jnae &&
       !is_jnb && !is_jae && !is_jnc && !is_je && !is_jz && !is_jne && !is_jnz &&
       !is_jbe && !is_jna && !is_ja && !is_jnbe &&
       !is_js && !is_jns && !is_jp && !is_jpe && !is_jnp && !is_jpo &&
       !is_jl && !is_jnge && !is_jge && !is_jnl &&
       !is_jle && !is_jng && !is_jg && !is_jnle &&
       !is_shld && !is_shrd &&
       inst->arg_count != 2) {
        _error_from_token(p, inst->name, ERROR_TYPE_I386, "invalid number of arguments");
        return INSTRUCTION_FAILED;
    }

    if(is_pusha || is_pushaw) {
        emit_byte(p, 0x60);
        return 1;
    }
    if(is_pushad) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x60);
        return 2;
    }
    if(is_popa || is_popaw) {
        emit_byte(p, 0x61);
        return 1;
    }
    if(is_popad) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x61);
        return 2;
    }
    if(is_pushf) {
        emit_byte(p, 0x9c);
        return 1;
    }
    if(is_pushfd) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x9c);
        return 2;
    }
    if(is_popf) {
        emit_byte(p, 0x9d);
        return 1;
    }
    if(is_popfd) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x9d);
        return 2;
    }
    if(is_lahf) {
        emit_byte(p, 0x9f);
        return 1;
    }
    if(is_sahf) {
        emit_byte(p, 0x9e);
        return 1;
    }
    if(is_xlat || is_xlatb) {
        emit_byte(p, 0xd7);
        return 1;
    }
    if(is_movsb) {
        emit_byte(p, 0xa4);
        return 1;
    }
    if(is_movs || is_movsw) {
        emit_byte(p, 0xa5);
        return 1;
    }
    if(is_movsd) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0xa5);
        return 2;
    }
    if(is_cmpsb) {
        emit_byte(p, 0xa6);
        return 1;
    }
    if(is_cmps || is_cmpsw) {
        emit_byte(p, 0xa7);
        return 1;
    }
    if(is_cmpsd) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0xa7);
        return 2;
    }
    if(is_scasb) {
        emit_byte(p, 0xae);
        return 1;
    }
    if(is_scas || is_scasw) {
        emit_byte(p, 0xaf);
        return 1;
    }
    if(is_scasd) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0xaf);
        return 2;
    }
    if(is_lodsb) {
        emit_byte(p, 0xac);
        return 1;
    }
    if(is_lods || is_lodsw) {
        emit_byte(p, 0xad);
        return 1;
    }
    if(is_lodsd) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0xad);
        return 2;
    }
    if(is_stosb) {
        emit_byte(p, 0xaa);
        return 1;
    }
    if(is_stos || is_stosw) {
        emit_byte(p, 0xab);
        return 1;
    }
    if(is_stosd) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0xab);
        return 2;
    }
    if(is_insb) {
        emit_byte(p, 0x6c);
        return 1;
    }
    if(is_insw) {
        emit_byte(p, 0x6d);
        return 1;
    }
    if(is_insd) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x6d);
        return 2;
    }
    if(is_outsb) {
        emit_byte(p, 0x6e);
        return 1;
    }
    if(is_outsw) {
        emit_byte(p, 0x6f);
        return 1;
    }
    if(is_outsd) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x6f);
        return 2;
    }
    if(is_cbw) {
        emit_byte(p, 0x98);
        return 1;
    }
    if(is_cwd) {
        emit_byte(p, 0x99);
        return 1;
    }
    if(is_cwde) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x98);
        return 2;
    }
    if(is_cdq) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0x99);
        return 2;
    }
    if(is_aaa) {
        emit_byte(p, 0x37);
        return 1;
    }
    if(is_aad && inst->arg_count == 0) {
        emit_byte(p, 0xd5);
        emit_byte(p, 0x0a);
        return 2;
    }
    if(is_aam && inst->arg_count == 0) {
        emit_byte(p, 0xd4);
        emit_byte(p, 0x0a);
        return 2;
    }
    if(is_aas) {
        emit_byte(p, 0x3f);
        return 1;
    }
    if(is_daa) {
        emit_byte(p, 0x27);
        return 1;
    }
    if(is_das) {
        emit_byte(p, 0x2f);
        return 1;
    }
    if(is_clc) {
        emit_byte(p, 0xf8);
        return 1;
    }
    if(is_clts) {
        emit_byte(p, 0x0f);
        emit_byte(p, 0x06);
        return 2;
    }
    if(is_stc) {
        emit_byte(p, 0xf9);
        return 1;
    }
    if(is_cmc) {
        emit_byte(p, 0xf5);
        return 1;
    }
    if(is_cld) {
        emit_byte(p, 0xfc);
        return 1;
    }
    if(is_std) {
        emit_byte(p, 0xfd);
        return 1;
    }
    if(is_cli) {
        emit_byte(p, 0xfa);
        return 1;
    }
    if(is_sti) {
        emit_byte(p, 0xfb);
        return 1;
    }
    if(is_hlt) {
        emit_byte(p, 0xf4);
        return 1;
    }
    if(is_wait || is_fwait) {
        emit_byte(p, 0x9b);
        return 1;
    }
    if(is_nop) {
        emit_byte(p, 0x90);
        return 1;
    }
    if(is_into) {
        emit_byte(p, 0xce);
        return 1;
    }
    if(is_iret) {
        emit_byte(p, 0xcf);
        return 1;
    }
    if(is_iretd) {
        emit_operand_size_prefix(p, 4);
        emit_byte(p, 0xcf);
        return 2;
    }
    if(is_leave) {
        emit_byte(p, 0xc9);
        return 1;
    }

    if(is_jmp) return encode_jmp(p, inst);
    if(is_loop) return encode_rel8_control(p, inst, 0xe2);
    if(is_loope || is_loopz) return encode_rel8_control(p, inst, 0xe1);
    if(is_loopne || is_loopnz) return encode_rel8_control(p, inst, 0xe0);
    if(is_jcxz) return encode_rel8_control(p, inst, 0xe3);
    if(is_jecxz) return encode_jecxz(p, inst);
    if(is_jo) return encode_conditional_jump(p, inst, 0x70, 0x80);
    if(is_jno) return encode_conditional_jump(p, inst, 0x71, 0x81);
    if(is_jb || is_jc || is_jnae) return encode_conditional_jump(p, inst, 0x72, 0x82);
    if(is_jnb || is_jae || is_jnc) return encode_conditional_jump(p, inst, 0x73, 0x83);
    if(is_je || is_jz) return encode_conditional_jump(p, inst, 0x74, 0x84);
    if(is_jne || is_jnz) return encode_conditional_jump(p, inst, 0x75, 0x85);
    if(is_jbe || is_jna) return encode_conditional_jump(p, inst, 0x76, 0x86);
    if(is_ja || is_jnbe) return encode_conditional_jump(p, inst, 0x77, 0x87);
    if(is_js) return encode_conditional_jump(p, inst, 0x78, 0x88);
    if(is_jns) return encode_conditional_jump(p, inst, 0x79, 0x89);
    if(is_jp || is_jpe) return encode_conditional_jump(p, inst, 0x7a, 0x8a);
    if(is_jnp || is_jpo) return encode_conditional_jump(p, inst, 0x7b, 0x8b);
    if(is_jl || is_jnge) return encode_conditional_jump(p, inst, 0x7c, 0x8c);
    if(is_jge || is_jnl) return encode_conditional_jump(p, inst, 0x7d, 0x8d);
    if(is_jle || is_jng) return encode_conditional_jump(p, inst, 0x7e, 0x8e);
    if(is_jg || is_jnle) return encode_conditional_jump(p, inst, 0x7f, 0x8f);
    if(is_call) return encode_call(p, inst);
    if(is_ret || is_retn) return encode_ret(p, inst, 0xc3, 0xc2);
    if(is_retf) return encode_ret(p, inst, 0xcb, 0xca);
    if(is_enter) return encode_enter(p, inst);

    if(is_push) {
        status = parse_push_operand(p, inst->args_head, &dst, &error_token);
        if(status <= 0) {
            _error_from_token(p, error_token, ERROR_TYPE_I386,
                              "invalid first operand");
            return INSTRUCTION_FAILED;
        }
        return encode_push(p, inst, &dst);
    }

    if(is_imul) return encode_imul_multi(p, inst);
    if(is_setcc) return encode_prefixed_setcc(p, inst, setcc_opcode,
                                              setcc_name, 0, 0, 0);
    if(is_shld) {
        return encode_prefixed_double_shift(p, inst, 0xa4, 0xa5, "shld",
                                            0, 0, 0, 0);
    }
    if(is_shrd) {
        return encode_prefixed_double_shift(p, inst, 0xac, 0xad, "shrd",
                                            0, 0, 0, 0);
    }

    status = parse_operand(p, inst->args_head, &dst, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid first operand");
        return INSTRUCTION_FAILED;
    }

    if(is_pop) return encode_pop(p, inst, &dst);
    if(is_intr) return encode_intr(p, inst, &dst);
    if(is_not) return encode_not(p, inst, &dst);
    if(is_inc) return encode_inc_dec(p, inst, &dst, 0, 0x40, "inc");
    if(is_dec) return encode_inc_dec(p, inst, &dst, 1, 0x48, "dec");
    if(is_neg) return encode_unary_group(p, inst, &dst, 3, "neg");
    if(is_mul) return encode_unary_group(p, inst, &dst, 4, "mul");
    if(is_div) return encode_unary_group(p, inst, &dst, 6, "div");
    if(is_idiv) return encode_unary_group(p, inst, &dst, 7, "idiv");
    if(is_aad) return encode_aad(p, inst, &dst);
    if(is_aam) return encode_aam(p, inst, &dst);
    if(is_lgdt) return encode_descriptor_table(p, inst, &dst, 2, "lgdt", 0, 0, 0);
    if(is_lidt) return encode_descriptor_table(p, inst, &dst, 3, "lidt", 0, 0, 0);
    if(is_sgdt) return encode_descriptor_table(p, inst, &dst, 0, "sgdt", 0, 0, 0);
    if(is_sidt) return encode_descriptor_table(p, inst, &dst, 1, "sidt", 0, 0, 0);
    if(is_lldt) return encode_system_rm16_load(p, inst, &dst, 2, "lldt", 0, 0, 0);
    if(is_sldt) {
        return encode_system_rm16_store(p, inst, &dst, 0, 0x00, "sldt",
                                        0, 0, 0, 0);
    }
    if(is_ltr) return encode_system_rm16_load(p, inst, &dst, 3, "ltr", 0, 0, 0);
    if(is_str) {
        return encode_system_rm16_store(p, inst, &dst, 1, 0x00, "str",
                                        0, 0, 0, 0);
    }
    if(is_lmsw) return encode_system_rm16_load(p, inst, &dst, 6, "lmsw", 0, 0, 0);
    if(is_smsw) {
        return encode_system_rm16_store(p, inst, &dst, 4, 0x01, "smsw",
                                        0, 0, 0, 0);
    }
    if(is_verr) return encode_system_verify_selector(p, inst, &dst, 4, "verr", 0, 0, 0);
    if(is_verw) return encode_system_verify_selector(p, inst, &dst, 5, "verw", 0, 0, 0);

    error_token = NULL;
    status = parse_operand(p, inst->args_tail, &src, &error_token);
    if(status <= 0) {
        _error_from_token(p, error_token, ERROR_TYPE_I386,
                          "invalid second operand");
        return INSTRUCTION_FAILED;
    }

    if(is_mov) {
        return encode_mov(p, inst, &dst, &src);
    }
    if(is_lds) {
        return encode_far_load(p, inst, &dst, &src, 0xc5);
    }
    if(is_les) {
        return encode_far_load(p, inst, &dst, &src, 0xc4);
    }
    if(is_lfs) {
        return encode_far_load_0f(p, inst, &dst, &src, 0xb4);
    }
    if(is_lgs) {
        return encode_far_load_0f(p, inst, &dst, &src, 0xb5);
    }
    if(is_lss) {
        return encode_far_load_0f(p, inst, &dst, &src, 0xb2);
    }
    if(is_movsx) {
        return encode_mov_extend(p, inst, &dst, &src, 0xbe, 0xbf,
                                 "unsupported movsx form");
    }
    if(is_movzx) {
        return encode_mov_extend(p, inst, &dst, &src, 0xb6, 0xb7,
                                 "unsupported movzx form");
    }
    if(is_bound) {
        return encode_bound(p, inst, &dst, &src);
    }
    if(is_arpl) {
        return encode_arpl_operands(p, inst, &dst, &src, 0, 0);
    }
    if(is_bsf) {
        return encode_bit_scan(p, inst, &dst, &src, 0xbc,
                               "unsupported bsf form");
    }
    if(is_bsr) {
        return encode_bit_scan(p, inst, &dst, &src, 0xbd,
                               "unsupported bsr form");
    }
    if(is_lar) {
        return encode_lar_operands(p, inst, &dst, &src, 0, 0, 0, 0);
    }
    if(is_lsl) {
        return encode_lsl_operands(p, inst, &dst, &src, 0, 0, 0, 0);
    }
    if(is_bt) {
        return encode_bit_test_prefixed_operands(p, inst, &dst, &src,
                                                 0xa3, 4,
                                                 "unsupported bt form",
                                                 0, 0, 0);
    }
    if(is_btc) {
        return encode_bit_test_prefixed_operands(p, inst, &dst, &src,
                                                 0xbb, 7,
                                                 "unsupported btc form",
                                                 0, 0, 0);
    }
    if(is_btr) {
        return encode_bit_test_prefixed_operands(p, inst, &dst, &src,
                                                 0xb3, 6,
                                                 "unsupported btr form",
                                                 0, 0, 0);
    }
    if(is_bts) {
        return encode_bit_test_prefixed_operands(p, inst, &dst, &src,
                                                 0xab, 5,
                                                 "unsupported bts form",
                                                 0, 0, 0);
    }
    if(is_xchg) {
        return encode_xchg(p, inst, &dst, &src);
    }
    if(is_in) {
        return encode_in(p, inst, &dst, &src);
    }
    if(is_out) {
        return encode_out(p, inst, &dst, &src);
    }
    if(is_esc) {
        return encode_esc(p, inst, &dst, &src);
    }
    if(is_add) {
        return encode_arith(p, inst, &dst, &src, 0x00, 0x04, 0x05, 0, "add");
    }
    if(is_adc) {
        return encode_arith(p, inst, &dst, &src, 0x10, 0x14, 0x15, 2, "adc");
    }
    if(is_sub) {
        return encode_arith(p, inst, &dst, &src, 0x28, 0x2c, 0x2d, 5, "sub");
    }
    if(is_sbb) {
        return encode_arith(p, inst, &dst, &src, 0x18, 0x1c, 0x1d, 3, "sbb");
    }
    if(is_cmp) {
        return encode_arith(p, inst, &dst, &src, 0x38, 0x3c, 0x3d, 7, "cmp");
    }
    if(is_and) {
        return encode_and(p, inst, &dst, &src);
    }
    if(is_or) {
        return encode_or(p, inst, &dst, &src);
    }
    if(is_xor) {
        return encode_xor(p, inst, &dst, &src);
    }
    if(is_test) {
        return encode_test(p, inst, &dst, &src);
    }
    if(is_shl || is_sal) {
        return encode_shift_group(p, inst, &dst, &src, 4);
    }
    if(is_shr) {
        return encode_shift_group(p, inst, &dst, &src, 5);
    }
    if(is_sar) {
        return encode_shift_group(p, inst, &dst, &src, 7);
    }
    if(is_rol) {
        return encode_shift_group(p, inst, &dst, &src, 0);
    }
    if(is_ror) {
        return encode_shift_group(p, inst, &dst, &src, 1);
    }
    if(is_rcl) {
        return encode_shift_group(p, inst, &dst, &src, 2);
    }
    if(is_rcr) {
        return encode_shift_group(p, inst, &dst, &src, 3);
    }
    return encode_lea(p, inst, &dst, &src);
}
