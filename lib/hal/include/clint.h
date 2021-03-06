/* Copyright 2018 Canaan Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * @file
 * @brief      The CLINT block holds memory-mapped control and status registers
 *             associated with local interrupts for a Coreplex.
 *
 * @note       CLINT RAM Layout
 *
 * | Address   -| Description                     |
 * |------------|---------------------------------|
 * | 0x02000000 | msip for core 0                 |
 * | 0x02000004 | msip for core 1                 |
 * | ...        | ...                             |
 * | 0x02003FF8 | msip for core 4094              |
 * |            |                                 |
 * | 0x02004000 | mtimecmp for core 0             |
 * | 0x02004008 | mtimecmp for core 1             |
 * | ...        | ...                             |
 * | 0x0200BFF0 | mtimecmp For core 4094          |
 * | 0x0200BFF8 | mtime                           |
 * |            |                                 |
 * | 0x0200C000 | Reserved                        |
 * | ...        | ...                             |
 * | 0x0200EFFC | Reserved                        |
 */

#ifndef _DRIVER_CLINT_H
#define _DRIVER_CLINT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* clang-format off */
/* Register address offsets */
#define CLINT_MSIP          (0x0000)
#define CLINT_MSIP_SIZE     (0x4)
#define CLINT_MTIMECMP      (0x4000)
#define CLINT_MTIMECMP_SIZE (0x8)
#define CLINT_MTIME         (0xBFF8)
#define CLINT_MTIME_SIZE    (0x8)
/* Max number of cores */
#define CLINT_MAX_CORES     (4095)
/* Real number of cores */
#define CLINT_NUM_CORES     (2)
/* Clock frequency division factor */
#define CLINT_CLOCK_DIV     (50)
/* clang-format on */

/**
 * @brief       MSIP Registers
 *
 *              Machine-mode software interrupts are generated by writing to a
 *              per-core memory-mapped control register. The msip registers are
 *              32-bit wide WARL registers, where the LSB is reflected in the
 *              msip bit of the associated core???s mip register. Other bits in
 *              the msip registers are hardwired to zero. The mapping supports
 *              up to 4095 machine-mode cores.
 */
typedef struct _clint_msip
{
    uint32_t msip : 1;  /*!< Bit 0 is msip */
    uint32_t zero : 31; /*!< Bits [32:1] is 0 */
} __attribute__((packed, aligned(4))) clint_msip_t;

/**
 * @brief       Timer compare Registers Machine-mode timer interrupts are
 *              generated by a real-time counter and a per-core comparator. The
 *              mtime register is a 64-bit read-only register that contains the
 *              current value of the real-time counter. Each mtimecmp register
 *              holds its core???s time comparator. A timer interrupt is pending
 *              whenever mtime is greater than or equal to the value in a
 *              core???s mtimecmp register. The timer interrupt is reflected in
 *              the mtip bit of the associated core???s mip register.
 */
typedef uint64_t clint_mtimecmp_t;

/**
 * @brief       Timer Registers
 *
 *              The mtime register has a 64-bit precision on all RV32, RV64,
 *              and RV128 systems. Platforms provide a 64-bit memory-mapped
 *              machine-mode timer compare register (mtimecmp), which causes a
 *              timer interrupt to be posted when the mtime register contains a
 *              value greater than or equal to the value in the mtimecmp
 *              register. The interrupt remains posted until it is cleared by
 *              writing the mtimecmp register. The interrupt will only be taken
 *              if interrupts are enabled and the MTIE bit is set in the mie
 *              register.
 */
typedef uint64_t clint_mtime_t;

/**
 * @brief       CLINT object
 *
 *              Coreplex-Local INTerrupts, which includes software interrupts,
 *              local timer interrupts, and other interrupts routed directly to
 *              a core.
 */
typedef struct _clint
{
    /* 0x0000 to 0x3FF8, MSIP Registers */
    clint_msip_t msip[CLINT_MAX_CORES];
    /* Resverd space, do not use */
    uint32_t resv0;
    /* 0x4000 to 0xBFF0, Timer Compare Registers */
    clint_mtimecmp_t mtimecmp[CLINT_MAX_CORES];
    /* 0xBFF8, Time Register */
    clint_mtime_t mtime;
} __attribute__((packed, aligned(4))) clint_t;

/**
 * @brief       Clint object instanse
 */
extern volatile clint_t* const clint;

/**
 * @brief       Initialize local interprocessor interrupt
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int clint_ipi_init(void);

/**
 * @brief       Enable local interprocessor interrupt
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int clint_ipi_enable(void);

/**
 * @brief       Disable local interprocessor interrupt
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int clint_ipi_disable(void);

/**
 * @brief       Send local interprocessor interrupt to core by core id
 *
 * @param[in]   core_id  The core identifier
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int clint_ipi_send(size_t core_id);

/**
 * @brief       Clear local interprocessor interrupt
 *
 * @param[in]   core_id  The core identifier
 *
 * @return      result
 *     - 1      An IPI was pending
 *     - 0      Non IPI was pending
 *     - -1     Fail
 */
int clint_ipi_clear(size_t core_id);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CLINT_H */

