/* AAPL CORE Revision: 2.1.0 */

/* Copyright 2014 Avago Technologies. All rights reserved.                   */
/*                                                                           */
/* This file is part of the AAPL CORE library.                               */
/*                                                                           */
/* AAPL CORE is free software: you can redistribute it and/or modify it      */
/* under the terms of the GNU Lesser General Public License as published by  */
/* the Free Software Foundation, either version 3 of the License, or         */
/* (at your option) any later version.                                       */
/*                                                                           */
/* AAPL CORE is distributed in the hope that it will be useful,              */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/* GNU Lesser General Public License for more details.                       */
/*                                                                           */
/* You should have received a copy of the GNU Lesser General Public License  */
/* along with AAPL CORE.  If not, see http://www.gnu.org/licenses.           */

/* Core SerDes types and functions. */
/* */

/** Doxygen File Header */
/** @file */
/** @brief Core SerDes types and functions. */

#ifndef AAPL_SERDES_CORE_H_
#define AAPL_SERDES_CORE_H_

EXT int avago_serdes_get_lsb_rev(Aapl_t *aapl, uint sbus_addr);

/*////////////////////////////////////////////////////////////////////// */
/* Memory access functions: */

/** @brief   Selects memory access type. */
/** @details AVAGO_LSB_DIRECT uses SBus transactions to read/write data, */
/**          which avoids interrupting the processor, and also works if the */
/**          SPICO processor is not running. */
/** @details AVAGO_ESB_DIRECT also works if the processor is not running, */
/**          though it is much slower than AVAGO_ESB. */
/** @details The *_PREHALTED versions assume the caller invokes */
/**          avago_spico_halt() and avago_spico_resume() */
/**          appropriately, and are used to avoid the overhead of halting */
/**          and restarting the processor for multiple successive reads. */
typedef enum
{
    AVAGO_ESB,            /**< Electrical Sub-Block access. */
    AVAGO_ESB_DIRECT,     /**< ESB access using SBus instead of an interrupt. */
    AVAGO_LSB,            /**< Logical Sub-Block access. */
    AVAGO_LSB_DIRECT,     /**< LSB access using SBus instead of an interrupt. */
#ifndef MV_HWS_REDUCED_BUILD
    AVAGO_DMEM,           /**< Data memory access. */
    AVAGO_DMEM_PREHALTED, /**< Data memory access, processor already halted. */
    AVAGO_IMEM,           /**< Instruction memory access. */
    AVAGO_IMEM_PREHALTED  /**< Instruction memory access, processor already halted. */
#endif /* MV_HWS_REDUCED_BUILD */
} Avago_serdes_mem_type_t;

EXT int  avago_serdes_mem_rd(Aapl_t *aapl, uint sbus_addr,
                             Avago_serdes_mem_type_t type, uint addr);
EXT void avago_serdes_mem_wr(Aapl_t *aapl, uint sbus_addr,
                             Avago_serdes_mem_type_t type, uint addr, uint data);
EXT int avago_serdes_mem_rmw(Aapl_t *aapl, uint sbus_addr,
                             Avago_serdes_mem_type_t type, uint addr, uint data, uint mask);


# ifdef AAPL_ENABLE_INTERNAL_FUNCTIONS
    EXT int avago_serdes_width_to_bits(int width);
    EXT int avago_serdes_bits_to_width(uint bits);
# endif

EXT int avago_serdes_get_tx_user_data(Aapl_t *aapl, uint sbus_addr, long data[4]);
EXT int avago_serdes_set_tx_user_data(Aapl_t *aapl, uint sbus_addr, long data[4]);

/**< @brief The TX input data selections. */
typedef enum
{
    AVAGO_SERDES_TX_DATA_SEL_PRBS7    = 0, /**< PRBS7 (x^7+x^6+1) generator. */
    AVAGO_SERDES_TX_DATA_SEL_PRBS9    = 1, /**< PRBS9 (x^9+x^7+1). */
    AVAGO_SERDES_TX_DATA_SEL_PRBS11   = 2, /**< PRBS11 (x^11+x^9+1). */
    AVAGO_SERDES_TX_DATA_SEL_PRBS15   = 3, /**< PRBS15 (x^15+x^14+1). */
    AVAGO_SERDES_TX_DATA_SEL_PRBS23   = 4, /**< PRBS23 (x^23+x^18+1). */
    AVAGO_SERDES_TX_DATA_SEL_PRBS31   = 5, /**< PRBS31 (x^31+x^28+1). */
    AVAGO_SERDES_TX_DATA_SEL_USER     = 7, /**< User pattern generator. */
    AVAGO_SERDES_TX_DATA_SEL_CORE     = 8, /**< External data. */
    AVAGO_SERDES_TX_DATA_SEL_LOOPBACK = 9  /**< Parallel loopback from receiver. */
} Avago_serdes_tx_data_sel_t;

EXT Avago_serdes_tx_data_sel_t avago_serdes_get_tx_data_sel(Aapl_t *aapl, uint sbus_addr);
EXT int                        avago_serdes_set_tx_data_sel(Aapl_t *aapl, uint sbus_addr,
                                                            Avago_serdes_tx_data_sel_t input);

EXT int  avago_serdes_get_tx_rx_ready(Aapl_t *aapl, uint sbus_addr, BOOL *tx, BOOL *rx);
EXT int  avago_serdes_set_tx_rx_enable(Aapl_t *aapl, uint sbus_addr, BOOL tx, BOOL rx, BOOL tx_output);

EXT BOOL avago_serdes_get_tx_output_enable(Aapl_t *aapl, uint sbus_addr);
EXT int  avago_serdes_set_tx_output_enable(Aapl_t *aapl, uint sbus_addr, BOOL enable);

EXT BOOL avago_serdes_get_tx_invert(Aapl_t *aapl, uint sbus_addr);
EXT int  avago_serdes_set_tx_invert(Aapl_t *aapl, uint sbus_addr, BOOL invert);

EXT int  avago_serdes_tx_inject_error(Aapl_t *aapl, uint sbus_addr, uint num_bits);
EXT int  avago_serdes_rx_inject_error(Aapl_t *aapl, uint sbus_addr, uint num_bits);

/**< @brief The width mode and line encoding selections */
typedef enum
{
    AVAGO_SERDES_NRZ  = 0,    /**< Transmit using 2 levels of encoding */
    AVAGO_SERDES_PAM2 = 0,    /**< Transmit using 2 levels of encoding */
    AVAGO_SERDES_PAM4 = 1     /**< Transmit using 4 levels of encoding */
} Avago_serdes_encoding_mode_t;

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
EXT int  avago_serdes_get_tx_rx_width(Aapl_t *aapl, uint sbus_addr, int *tx_width, int *rx_width);
EXT int  avago_serdes_set_tx_rx_width(Aapl_t *aapl, uint sbus_addr, int tx_width, int rx_width);
EXT BOOL avago_serdes_get_tx_line_encoding (Aapl_t *aapl, uint addr);
EXT BOOL avago_serdes_get_rx_line_encoding (Aapl_t *aapl, uint addr);
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
EXT int  avago_serdes_set_tx_rx_width_pam(Aapl_t *aapl, uint sbus_addr, int tx_width, int rx_width, Avago_serdes_encoding_mode_t tx_encoding, Avago_serdes_encoding_mode_t rx_encoding);


/**< @brief The TX equalization settings. */
typedef struct
{
    short pre;            /**< Pre-cursor setting */
    short atten;          /**< Attenuator setting */
    short post;           /**< Post-cursor setting */
    short slew;           /**< Slew rate setting [0..3], 0 is fastest */
} Avago_serdes_tx_eq_t;

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
EXT void avago_serdes_tx_eq_init(Avago_serdes_tx_eq_t *ptr);
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
EXT int avago_serdes_get_tx_eq(Aapl_t *aapl, uint sbus_addr, Avago_serdes_tx_eq_t *tx_eq);
EXT int avago_serdes_set_tx_eq(Aapl_t *aapl, uint sbus_addr, Avago_serdes_tx_eq_t *tx_eq);



/** @brief Specify the pattern to use in validating the received data. */
typedef enum
{
    AVAGO_SERDES_RX_CMP_DATA_PRBS7    = 0, /**< PRBS7 (x^7+x^6+1) generator. */
    AVAGO_SERDES_RX_CMP_DATA_PRBS9    = 1, /**< PRBS9 (x^9+x^7+1). */
    AVAGO_SERDES_RX_CMP_DATA_PRBS11   = 2, /**< PRBS11 (x^11+x^9+1). */
    AVAGO_SERDES_RX_CMP_DATA_PRBS15   = 3, /**< PRBS15 (x^15+x^14+1). */
    AVAGO_SERDES_RX_CMP_DATA_PRBS23   = 4, /**< PRBS23 (x^23+x^18+1). */
    AVAGO_SERDES_RX_CMP_DATA_PRBS31   = 5, /**< PRBS31 (x^31+x^28+1). */
    AVAGO_SERDES_RX_CMP_DATA_SELF_SEED= 7, /**< Auto-seed to received 40 bit repeating pattern. */
                                           /**< NOTE: This is USER mode in firmware. */
    AVAGO_SERDES_RX_CMP_DATA_OFF      = 8  /**< Disable cmp data generator */
} Avago_serdes_rx_cmp_data_t;

EXT Avago_serdes_rx_cmp_data_t avago_serdes_get_rx_cmp_data(Aapl_t *aapl, uint sbus_addr);
EXT int                        avago_serdes_set_rx_cmp_data(Aapl_t *aapl, uint sbus_addr,
                                                            Avago_serdes_rx_cmp_data_t cmp_data);

EXT BOOL avago_serdes_get_rx_input_loopback(Aapl_t *aapl, uint sbus_addr);
EXT int  avago_serdes_set_rx_input_loopback(Aapl_t *aapl, uint sbus_addr, BOOL select_internal);

EXT BOOL avago_serdes_get_rx_invert(Aapl_t *aapl, uint sbus_addr);
EXT int  avago_serdes_set_rx_invert(Aapl_t *aapl, uint sbus_addr, BOOL invert);


/** @brief Receiver data comparison mode. */
typedef enum
{
    AVAGO_SERDES_RX_CMP_MODE_OFF         = 0x0000, /**< Turn Off */
    AVAGO_SERDES_RX_CMP_MODE_XOR         = 0x0100, /**< Test^Mission */
    AVAGO_SERDES_RX_CMP_MODE_TEST_PATGEN = 0x0120, /**< Test^PatGen. */
    AVAGO_SERDES_RX_CMP_MODE_MAIN_PATGEN = 0x0200  /**< Mission^PatGen */
} Avago_serdes_rx_cmp_mode_t;

EXT Avago_serdes_rx_cmp_mode_t avago_serdes_get_rx_cmp_mode(Aapl_t *aapl, uint sbus);
EXT int                        avago_serdes_set_rx_cmp_mode(Aapl_t *aapl, uint sbus,
                                                            Avago_serdes_rx_cmp_mode_t mode);

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
/** @brief Specify the RX termination value. */
typedef enum
{
    AVAGO_SERDES_RX_TERM_AGND = 0,    /**< Resistor to ground */
    AVAGO_SERDES_RX_TERM_AVDD = 1,    /**< Resistor to AVDD */
    AVAGO_SERDES_RX_TERM_FLOAT = 2    /**< No resistor. */
} Avago_serdes_rx_term_t;

EXT Avago_serdes_rx_term_t avago_serdes_get_rx_term(Aapl_t *aapl, uint sbus_addr);
EXT int                    avago_serdes_set_rx_term(Aapl_t *aapl, uint sbus_addr,
                                                    Avago_serdes_rx_term_t term);


/** @brief Select the Tx PLL clock source. */
typedef enum
{
    AVAGO_SERDES_TX_PLL_REFCLK             = 0x00,    /**< Use REF clock; default. */
    AVAGO_SERDES_TX_PLL_RX_DIVX            = 0x10,    /**< Use the recovered clock. */
    AVAGO_SERDES_TX_PLL_OFF                = 0x30,    /**< Use no clock. */
    AVAGO_SERDES_TX_PLL_PCIE_CORE_CLK      = 0x70,    /**< Use core clock. */
    AVAGO_SERDES_TX_PLL_PCIE_CORE_CLK_DIV2 = 0xf0     /**< Use core clock / 2. */
} Avago_serdes_tx_pll_clk_t;

EXT Avago_serdes_tx_pll_clk_t avago_serdes_get_tx_pll_clk_src(Aapl_t *aapl, uint sbus_addr);
EXT int                       avago_serdes_set_tx_pll_clk_src(Aapl_t *aapl, uint sbus_addr,
                                                              Avago_serdes_tx_pll_clk_t src);

EXT BOOL avago_serdes_get_electrical_idle(Aapl_t *aapl, uint sbus_addr);
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
EXT BOOL avago_serdes_get_signal_ok_enable(Aapl_t *aapl, uint sbus_addr);
EXT int  avago_serdes_get_signal_ok_threshold(Aapl_t *aapl, uint sbus_addr);

EXT BOOL avago_serdes_get_signal_ok(Aapl_t *aapl, uint sbus_addr, BOOL reset);
EXT int avago_serdes_initialize_signal_ok(Aapl_t *aapl, uint sbus_addr, int threshold);
EXT BOOL avago_serdes_get_frequency_lock(Aapl_t *aapl, uint addr);
EXT uint avago_serdes_get_errors(Aapl_t *aapl, uint sbus_addr,
                                 Avago_serdes_mem_type_t type, BOOL reset_count_after_get);

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
EXT BOOL avago_serdes_slip_bits(Aapl_t *aapl, uint sbus_addr, uint bits);
EXT BOOL avago_serdes_slip_rx_phase(Aapl_t *aapl, uint sbus_addr, uint bits, BOOL apply_at_init);
EXT BOOL avago_serdes_slip_tx_phase(Aapl_t *aapl, uint sbus_addr, uint bits, BOOL apply_at_init);
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

/** @brief Initialize a SerDes slice. */
typedef enum
{
    AVAGO_PRBS31_ILB,    /**< Run a PRBS check in ILB; leave SerDes in ILB running PRBS 31 data. */
    AVAGO_PRBS31_ELB,    /**< Run a PRBS check in ILB; leave SerDes in ELB running PRBS 31 data. */
    AVAGO_CORE_DATA_ILB, /**< Run a PRBS check in ILB; leave SerDes in ILB sending data from the ASIC core. */
    AVAGO_CORE_DATA_ELB, /**< Run a PRBS check in ILB; leave SerDes in ELB sending data from the ASIC core. */
    AVAGO_INIT_ONLY      /**< Don't check any errors during init, just init the serdes. */
                         /**< SerDes will be left in ELB mode sending data from the ASIC core. */
} Avago_serdes_init_mode_t;

typedef struct
{
    BOOL sbus_reset;
    BOOL init_tx;
    BOOL init_rx;
    Avago_serdes_init_mode_t init_mode;

    uint tx_divider;
    Avago_serdes_encoding_mode_t tx_encoding;
    BOOL tx_output_en;
    uint tx_width;      /**< TX width mode: 10, 20 or 40 */
    BOOL tx_phase_cal;
    BOOL refclk_sync_master;
    BOOL tx_inversion;  /**< Enable TX polarity inversion if ELB selected */

    uint rx_divider;
    Avago_serdes_encoding_mode_t rx_encoding;
    uint rx_width;      /**< RX width mode: 10, 20 or 40 */
    BOOL signal_ok_en;
    int  signal_ok_threshold;   /**< [0-15] */
    BOOL rx_inversion;  /**< Enable RX polarity inversion if ELB selected */
} Avago_serdes_init_config_t;

#if !defined MV_HWS_REDUCED_BUILD_EXT_CM3 || defined MV_HWS_BIN_HEADER
EXT Avago_serdes_init_config_t *avago_serdes_init_config_construct(Aapl_t *aapl);
EXT void avago_serdes_init_config_destruct(Aapl_t *aapl, Avago_serdes_init_config_t *config);
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
void avago_serdes_init_config(Avago_serdes_init_config_t *config);
EXT int avago_serdes_init(Aapl_t *aapl, uint sbus_addr, Avago_serdes_init_config_t *config);
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
EXT int avago_serdes_init_quick(Aapl_t *aapl, uint sbus_addr, uint divider);
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */

/**@brief Controls what type of tuning to execute. */
typedef enum
{
    AVAGO_DFE_ICAL,          /**< Initial calibration, coarse tuning. (Default) */
    AVAGO_DFE_PCAL,          /**< Periodic calibration, fine tuning, no LF,HF adjustments */
    AVAGO_DFE_START_ADAPTIVE,/**< Launch continous pCAL */
    AVAGO_DFE_STOP_ADAPTIVE, /**< Stop continous pCAL */
    AVAGO_DFE_ENABLE_RR,     /**< Enable SerDes to participate in Round-Robin pCal */
    AVAGO_DFE_DISABLE_RR     /**< Disable SerDes from participation in Round-Robin pCal */
} Avago_serdes_dfe_tune_mode_t;


/** @details If fixed[n] is set then the value[n] setting of the struct */
/**          will be loaded before tuning is executed. */
/** @details NOTE: Not all versions of the firmware support all settings. */
typedef struct
{
    BOOL fixed[4];
    int  value[4];

    uint dfe_disable; /**< When set, skip tuning of dfeTaps. */

    Avago_serdes_dfe_tune_mode_t tune_mode; /**< Select tuning mode to execute. */

} Avago_serdes_dfe_tune_t;
#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
EXT void avago_serdes_tune_init(Aapl_t *aapl, Avago_serdes_dfe_tune_t *control);
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
EXT void avago_serdes_tune(Aapl_t *aapl, uint sbus_addr, Avago_serdes_dfe_tune_t *control);

EXT uint avago_serdes_eye_get_simple_metric(Aapl_t *aapl, uint addr);


#endif
