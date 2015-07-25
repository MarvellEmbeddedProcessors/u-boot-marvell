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

/* Support for AAPL (ASIC and ASSP Programming Layer) generic handling of */
/* SerDes (Serializer/Deserializer) slices on ASIC SBus rings. */

#define AAPL_ENABLE_INTERNAL_FUNCTIONS
#include "aapl.h"

/** Doxygen File Header */
/** @file */
/** @brief Functions for PMD training. */

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
/*============================================================================= */
/* SERDES PMD CONFIG CONSTRUCTOR */
/* */
/** @brief   Creates an Avago_serdes_pmd_config_t struct */
/** @details mallocs and initializes the memory for a Avago_serdes_pmd_config_t */
/**          and returns a pointer to that memory location. */
/** @return  The initialized structure, or 0 on allocation error. */
Avago_serdes_pmd_config_t *avago_serdes_pmd_config_construct(
    Aapl_t *aapl)   /**< [in] Pointer to Aapl_t structure. */
{
    Avago_serdes_pmd_config_t * pmd_config;
    int bytes = sizeof(Avago_serdes_pmd_config_t);

    if (! (pmd_config = (Avago_serdes_pmd_config_t *) aapl_malloc(aapl, bytes, "Avago_serdes_pmd_config_t struct"))) return(NULL);
    memset(pmd_config, 0, sizeof(*pmd_config));

    pmd_config->train_mode = AVAGO_PMD_TRAIN;
    pmd_config->clause     = AVAGO_PMD_CL72;

    return(pmd_config);
}
/*============================================================================= */
/* SERDES PMD CONFIG DESTRUCTOR */
/* */
/** @brief   Destroys the provided Avago_serdes_pmd_config_t struct. */
/** @details Frees resources associated with the pmd configuration structure. */
/** @return  void */
void avago_serdes_pmd_config_destruct(
    Aapl_t *aapl,                           /**< [in] Pointer to Aapl_t structure. */
    Avago_serdes_pmd_config_t *pmd_config)  /**< [in] Pointer to structure to free */
{
    aapl_free(aapl, pmd_config, __func__);
}
#endif /* MV_HWS_REDUCED_BUILD */

static void serdes_pmd_cl91_lane_config(
    Aapl_t *aapl,
    uint sbus_addr,
    Avago_serdes_pmd_config_t *mode)
{
    uint seed = mode->prbs_seed & 0x3FF;
    uint lane = mode->lane;

    if (lane > 3) {
        aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "PMD lane %d is beyond valid range of 0-3 for SBus %s.  Configuring as PMD lane 0.\n",lane,aapl_addr_to_str(sbus_addr));
        lane = 0;
    }

    if (seed == 0) {
        switch(lane) {
        case 3 : {seed = 0x7b6 /*b111_1011_0110*/; break;}
        case 2 : {seed = 0x72d /*b111_0010_1101*/; break;}
        case 1 : {seed = 0x645 /*b110_0100_0101*/; break;}
        case 0 :
        default: {seed = 0x57e /*b101_0111_1110*/; break;}
        }
    }
    avago_spico_int(aapl,sbus_addr,0x3D,0x3000|lane);
    avago_spico_int(aapl,sbus_addr,0x3D,0x4000|seed);
    avago_spico_int(aapl,sbus_addr,0x3D,0x2000|0x01);
}

static void serdes_pmd_cl84_lane_config(
    Aapl_t *aapl,
    uint sbus_addr,
    Avago_serdes_pmd_config_t *mode)
{
    uint seed = mode->prbs_seed & 0x3FF;
    uint lane = mode->lane;

    if (lane > 3) {
        aapl_log_printf(aapl, AVAGO_WARNING, __func__, __LINE__, "PMD lane %d is beyond valid range of 0-3 for SBus %s.  Configuring as PMD lane 0.\n",lane,aapl_addr_to_str(sbus_addr));

        lane = 0;
    }

    if (seed == 0)
        seed = lane + 2;

    avago_spico_int(aapl,sbus_addr,0x3D,0x3000|0x04);
    avago_spico_int(aapl,sbus_addr,0x3D,0x4000|seed);
    avago_spico_int(aapl,sbus_addr,0x3D,0x2000|0x03);
}

static void serdes_pmd_16gfc_lane_config(
    Aapl_t *aapl,
    uint sbus_addr,
    Avago_serdes_pmd_config_t * mode)
{
    uint int_data = 0;
    if (mode->TT_FECreq) int_data |= 0x01;
    if (mode->TT_TF    ) int_data |= 0x02;
    if (mode->TT_FECcap) int_data |= 0x04;

    avago_spico_int(aapl,sbus_addr,0x3D,0x1000|int_data);
}

/* ported from aapl 2.2.3 for PMD Training Status  */
/** @brief  Returns PMD training status.
** @return Returns 1 if training completed successfully.
** @return Returns 2 if training is in progress.
** @return Returns 0 if training ran and failed.
** @return Returns -1 if training hasn't been initiated.
**/
int avago_serdes_pmd_status(Aapl_t *aapl, uint addr)
{
#if 0 /* Ported from aapl 2.2.3 */
    int reg_serdes_status = (aapl_get_sdrev(aapl,addr) == AAPL_SDREV_P1) ? 0xef : 0x27;
#else
    int reg_serdes_status = 0x27;
#endif
    int status = avago_serdes_mem_rd(aapl, addr, AVAGO_LSB, reg_serdes_status);
    if( status & (1 << 1) ) return 2;
    if( (status & 7) == 4 ) return 1;
    if( status & (1 << 0) ) return 0;
    return -1;
}

/*============================================================================= */
/* SERDES PMD TUNE */
/*============================================================================= */
/** @brief Initialize struct with default values */
EXT void avago_serdes_pmd_init(Aapl_t *aapl, Avago_serdes_pmd_config_t *pmd_config)
{
    memset(pmd_config, 0, sizeof(Avago_serdes_pmd_config_t));

    pmd_config->train_mode = AVAGO_PMD_TRAIN;
    pmd_config->clause     = AVAGO_PMD_CL72;
}

/** @brief  Launches and halts PMD link training procedure */
/** @return void */
void avago_serdes_pmd_train(
    Aapl_t *aapl,                       /**< [in] Pointer to Aapl_t structure. */
    Avago_serdes_pmd_config_t *mode_control) /**< [in] Pointer to control structure. */
{
    uint sbus_addr = mode_control->sbus_addr;
    uint sbus_tx   = mode_control->sbus_addr;
    uint int_data = 0;

    if (!aapl_check_ip_type(aapl, sbus_addr, __func__, __LINE__, TRUE, 1, AVAGO_SERDES)) return;
    if (!aapl_check_process(aapl, sbus_addr, __func__, __LINE__, TRUE, 2, AVAGO_PROCESS_B, AVAGO_PROCESS_F)) return;

    if (mode_control->disable_timeout)  int_data |= 0x0010;


    switch(mode_control->train_mode)
    {
    case AVAGO_PMD_BYPASS  : {int_data |= 0x01; break;}
    case AVAGO_PMD_TRAIN   : {int_data |= 0x02; break;}
    case AVAGO_PMD_RESTART : {int_data |= 0x00; break;}
    default                : {int_data |= 0x00; break;}
    }

    if (mode_control->train_mode != AVAGO_PMD_RESTART)
    {
        uint i;
        avago_spico_int_check(aapl, __func__, __LINE__, sbus_addr, 0x04, 0);

        i = 0;
        while( (i < 10000) && ((avago_serdes_mem_rd(aapl, sbus_addr, AVAGO_LSB, 0x027) & 0x0010)==0) )
            i++;

        if (i >= 10000)
        {
            aapl_fail(aapl, __func__, __LINE__, "serdes_pmd_train RESTART command sent before initiating training timed out for sbus_addr %04x.  Training aborted.\n", sbus_addr);
            return;
        }


        if (aapl_get_lsb_rev(aapl,sbus_addr) >= 4)
        {
            if (mode_control->clause == AVAGO_PMD_CL92)
                serdes_pmd_cl91_lane_config(aapl,sbus_tx,mode_control);
            if (mode_control->clause == AVAGO_PMD_CL84)
                serdes_pmd_cl84_lane_config(aapl,sbus_tx,mode_control);
        }
        if (mode_control->clause == AVAGO_PMD_FC16G)
            serdes_pmd_16gfc_lane_config(aapl,sbus_addr,mode_control);
    }

    avago_spico_int_check(aapl, __func__, __LINE__, sbus_addr, 0x04, int_data);
}
