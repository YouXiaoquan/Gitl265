




#include "common.h"
#include "cpu.h"



const x265_cpu_name_t x265_cpu_names[] =
{
    {"Altivec",     X265_CPU_ALTIVEC},
//  {"MMX",         X265_CPU_MMX}, // we don't support asm on mmx1 cpus anymore
    {"MMX2",        X265_CPU_MMX|X265_CPU_MMX2},
    {"MMXEXT",      X265_CPU_MMX|X265_CPU_MMX2},
//  {"SSE",         X265_CPU_MMX|X265_CPU_MMX2|X265_CPU_SSE}, // there are no sse1 functions in x265
#define SSE2 X265_CPU_MMX|X265_CPU_MMX2|X265_CPU_SSE|X265_CPU_SSE2
    {"SSE2Slow",    SSE2|X265_CPU_SSE2_IS_SLOW},
    {"SSE2",        SSE2},
    {"SSE2Fast",    SSE2|X265_CPU_SSE2_IS_FAST},
    {"SSE3",        SSE2|X265_CPU_SSE3},
    {"SSSE3",       SSE2|X265_CPU_SSE3|X265_CPU_SSSE3},
    {"FastShuffle", SSE2|X265_CPU_SHUFFLE_IS_FAST},
    {"SSE4.1",      SSE2|X265_CPU_SSE3|X265_CPU_SSSE3|X265_CPU_SSE4},
    {"SSE4",        SSE2|X265_CPU_SSE3|X265_CPU_SSSE3|X265_CPU_SSE4},
    {"SSE4.2",      SSE2|X265_CPU_SSE3|X265_CPU_SSSE3|X265_CPU_SSE4|X265_CPU_SSE42},
#define AVX SSE2|X265_CPU_SSE3|X265_CPU_SSSE3|X265_CPU_SSE4|X265_CPU_SSE42|X265_CPU_AVX
    {"AVX",         AVX},
    {"XOP",         AVX|X265_CPU_XOP},
    {"FMA4",        AVX|X265_CPU_FMA4},
    {"AVX2",        AVX|X265_CPU_AVX2},
    {"FMA3",        AVX|X265_CPU_FMA3},
#undef AVX
#undef SSE2
    {"Cache32",         X265_CPU_CACHELINE_32},
    {"Cache64",         X265_CPU_CACHELINE_64},
    {"SSEMisalign",     X265_CPU_SSE_MISALIGN},
    {"LZCNT",           X265_CPU_LZCNT},
    {"BMI1",            X265_CPU_BMI1},
    {"BMI2",            X265_CPU_BMI1|X265_CPU_BMI2},
    {"TBM",             X265_CPU_TBM},
    {"Slow_mod4_stack", X265_CPU_STACK_MOD4},
    {"ARMv6",           X265_CPU_ARMV6},
    {"NEON",            X265_CPU_NEON},
    {"Fast_NEON_MRC",   X265_CPU_FAST_NEON_MRC},
    {"SlowCTZ",         X265_CPU_SLOW_CTZ},
    {"SlowAtom",        X265_CPU_SLOW_ATOM},
    {"", 0},
};

#if HAVE_MMX
int x265_cpu_cpuid_test( void );
void x265_cpu_cpuid( uint32_t op, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx );
void x265_cpu_xgetbv( uint32_t op, uint32_t *eax, uint32_t *edx );

uint32_t x265_cpu_detect( void )
{
    uint32_t cpu = 0;
    uint32_t eax, ebx, ecx, edx;
    uint32_t vendor[4] = {0};
    uint32_t max_extended_cap;
    int cache;

#if !ARCH_X86_64
    if( !x265_cpu_cpuid_test() )
        return 0;
#endif

    x265_cpu_cpuid( 0, &eax, vendor+0, vendor+2, vendor+1 );
    if( eax == 0 )
        return 0;

    x265_cpu_cpuid( 1, &eax, &ebx, &ecx, &edx );
    if( edx&0x00800000 )
        cpu |= X265_CPU_MMX;
    else
        return 0;
    if( edx&0x02000000 )
        cpu |= X265_CPU_MMX2|X265_CPU_SSE;
    if( edx&0x04000000 )
        cpu |= X265_CPU_SSE2;
    if( ecx&0x00000001 )
        cpu |= X265_CPU_SSE3;
    if( ecx&0x00000200 )
        cpu |= X265_CPU_SSSE3;
    if( ecx&0x00080000 )
        cpu |= X265_CPU_SSE4;
    if( ecx&0x00100000 )
        cpu |= X265_CPU_SSE42;
    /* Check OXSAVE and AVX bits */
    if( (ecx&0x18000000) == 0x18000000 )
    {
        /* Check for OS support */
        x265_cpu_xgetbv( 0, &eax, &edx );
        if( (eax&0x6) == 0x6 )
        {
            cpu |= X265_CPU_AVX;
            if( ecx&0x00001000 )
                cpu |= X265_CPU_FMA3;
        }
    }

    x265_cpu_cpuid( 7, &eax, &ebx, &ecx, &edx );
    /* AVX2 requires OS support, but BMI1/2 don't. */
    if( (cpu&X265_CPU_AVX) && (ebx&0x00000020) )
        cpu |= X265_CPU_AVX2;
    if( ebx&0x00000008 )
    {
        cpu |= X265_CPU_BMI1;
        if( ebx&0x00000100 )
            cpu |= X265_CPU_BMI2;
    }

    if( cpu & X265_CPU_SSSE3 )
        cpu |= X265_CPU_SSE2_IS_FAST;
    if( cpu & X265_CPU_SSE4 )
        cpu |= X265_CPU_SHUFFLE_IS_FAST;

    x265_cpu_cpuid( 0x80000000, &eax, &ebx, &ecx, &edx );
    max_extended_cap = eax;

    if( !strcmp((char*)vendor, "AuthenticAMD") && max_extended_cap >= 0x80000001 )
    {
        cpu |= X265_CPU_SLOW_CTZ;
        x265_cpu_cpuid( 0x80000001, &eax, &ebx, &ecx, &edx );
        if( edx&0x00400000 )
            cpu |= X265_CPU_MMX2;
        if( cpu & X265_CPU_SSE2 )
        {
            if( ecx&0x00000040 ) /* SSE4a */
            {
                cpu |= X265_CPU_SSE2_IS_FAST;
                cpu |= X265_CPU_LZCNT;
                cpu |= X265_CPU_SHUFFLE_IS_FAST;
                cpu &= ~X265_CPU_SLOW_CTZ;
            }
            else
                cpu |= X265_CPU_SSE2_IS_SLOW;

            if( ecx&0x00000080 ) /* Misalign SSE */
            {
                cpu |= X265_CPU_SSE_MISALIGN;
                x265_cpu_mask_misalign_sse();
            }

            if( cpu & X265_CPU_AVX )
            {
                if( ecx&0x00000800 ) /* XOP */
                    cpu |= X265_CPU_XOP;
                if( ecx&0x00010000 ) /* FMA4 */
                    cpu |= X265_CPU_FMA4;
            }

            if( ecx&0x00200000 )
                cpu |= X265_CPU_TBM;
        }
    }

    if( !strcmp((char*)vendor, "GenuineIntel") )
    {
        x265_cpu_cpuid( 1, &eax, &ebx, &ecx, &edx );
        int family = ((eax>>8)&0xf) + ((eax>>20)&0xff);
        int model  = ((eax>>4)&0xf) + ((eax>>12)&0xf0);
        if( family == 6 )
        {
            /* 6/9 (pentium-m "banias"), 6/13 (pentium-m "dothan"), and 6/14 (core1 "yonah")
             * theoretically support sse2, but it's significantly slower than mmx for
             * almost all of x265's functions, so let's just pretend they don't. */
            if( model == 9 || model == 13 || model == 14 )
            {
                cpu &= ~(X265_CPU_SSE2|X265_CPU_SSE3);
                assert(!(cpu&(X265_CPU_SSSE3|X265_CPU_SSE4)));
            }
            /* Detect Atom CPU */
            else if( model == 28 )
            {
                cpu |= X265_CPU_SLOW_ATOM;
                cpu |= X265_CPU_SLOW_CTZ;
            }
            /* Some Penryns and Nehalems are pointlessly crippled (SSE4 disabled), so
             * detect them here. */
            else if( model >= 23 )
                cpu |= X265_CPU_SHUFFLE_IS_FAST;
        }
    }

    if( (!strcmp((char*)vendor, "GenuineIntel") || !strcmp((char*)vendor, "CyrixInstead")) && !(cpu&X265_CPU_SSE42))
    {
        /* cacheline size is specified in 3 places, any of which may be missing */
        x265_cpu_cpuid( 1, &eax, &ebx, &ecx, &edx );
        cache = (ebx&0xff00)>>5; // cflush size
        if( !cache && max_extended_cap >= 0x80000006 )
        {
            x265_cpu_cpuid( 0x80000006, &eax, &ebx, &ecx, &edx );
            cache = ecx&0xff; // cacheline size
        }
        if( !cache )
        {
            // Cache and TLB Information
            static const char cache32_ids[] = { 0x0a, 0x0c, 0x41, 0x42, 0x43, 0x44, 0x45, 0x82, 0x83, 0x84, 0x85, 0 };
            static const char cache64_ids[] = { 0x22, 0x23, 0x25, 0x29, 0x2c, 0x46, 0x47, 0x49, 0x60, 0x66, 0x67,
                                                0x68, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7c, 0x7f, 0x86, 0x87, 0 };
            uint32_t buf[4];
            int max, i = 0;
            do {
                x265_cpu_cpuid( 2, buf+0, buf+1, buf+2, buf+3 );
                max = buf[0]&0xff;
                buf[0] &= ~0xff;
                for( int j = 0; j < 4; j++ )
                    if( !(buf[j]>>31) )
                        while( buf[j] )
                        {
                            if( strchr( cache32_ids, buf[j]&0xff ) )
                                cache = 32;
                            if( strchr( cache64_ids, buf[j]&0xff ) )
                                cache = 64;
                            buf[j] >>= 8;
                        }
            } while( ++i < max );
        }

        if( cache == 32 )
            cpu |= X265_CPU_CACHELINE_32;
        else if( cache == 64 )
            cpu |= X265_CPU_CACHELINE_64;
        else
            x265_log( NULL, X265_LOG_WARNING, "unable to determine cacheline size\n" );
    }

#if BROKEN_STACK_ALIGNMENT
    cpu |= X265_CPU_STACK_MOD4;
#endif

    return cpu;
}

#elif ARCH_PPC

#if SYS_MACOSX || SYS_OPENBSD
#include <sys/sysctl.h>
uint32_t x265_cpu_detect( void )
{
    /* Thank you VLC */
    uint32_t cpu = 0;
#if SYS_OPENBSD
    int      selectors[2] = { CTL_MACHDEP, CPU_ALTIVEC };
#else
    int      selectors[2] = { CTL_HW, HW_VECTORUNIT };
#endif
    int      has_altivec = 0;
    size_t   length = sizeof( has_altivec );
    int      error = sysctl( selectors, 2, &has_altivec, &length, NULL, 0 );

    if( error == 0 && has_altivec != 0 )
        cpu |= X265_CPU_ALTIVEC;

    return cpu;
}

#elif SYS_LINUX

uint32_t x265_cpu_detect( void )
{
    static void (*oldsig)( int );

    oldsig = signal( SIGILL, sigill_handler );
    if( sigsetjmp( jmpbuf, 1 ) )
    {
        signal( SIGILL, oldsig );
        return 0;
    }

    canjump = 1;
    asm volatile( "mtspr 256, %0\n\t"
                  "vand 0, 0, 0\n\t"
                  :
                  : "r"(-1) );
    canjump = 0;

    signal( SIGILL, oldsig );

    return X265_CPU_ALTIVEC;
}
#endif

#elif ARCH_ARM

void x265_cpu_neon_test( void );
int x265_cpu_fast_neon_mrc_test( void );

uint32_t x265_cpu_detect( void )
{
    int flags = 0;
#if HAVE_ARMV6
    flags |= X265_CPU_ARMV6;

    // don't do this hack if compiled with -mfpu=neon
#if !HAVE_NEON
    static void (* oldsig)( int );
    oldsig = signal( SIGILL, sigill_handler );
    if( sigsetjmp( jmpbuf, 1 ) )
    {
        signal( SIGILL, oldsig );
        return flags;
    }

    canjump = 1;
    x265_cpu_neon_test();
    canjump = 0;
    signal( SIGILL, oldsig );
#endif

    flags |= X265_CPU_NEON;

    // fast neon -> arm (Cortex-A9) detection relies on user access to the
    // cycle counter; this assumes ARMv7 performance counters.
    // NEON requires at least ARMv7, ARMv8 may require changes here, but
    // hopefully this hacky detection method will have been replaced by then.
    // Note that there is potential for a race condition if another program or
    // x265 instance disables or reinits the counters while x265 is using them,
    // which may result in incorrect detection and the counters stuck enabled.
    flags |= x265_cpu_fast_neon_mrc_test() ? X265_CPU_FAST_NEON_MRC : 0;
    // TODO: write dual issue test? currently it's A8 (dual issue) vs. A9 (fast mrc)
#endif
    return flags;
}

#else

uint32_t x265_cpu_detect( void )
{
    return 0;
}

#endif

