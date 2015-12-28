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

/* AAPL (ASIC and ASSP Programming Layer) generic handling of SerDes */
/* (Serializer/Deserializer) slices on ASIC SBus rings; all functions exported */
/* from this directory. */

/** Doxygen File Header */
/** @file */
/** @brief Declarations for SerDes PMD/KR functions. */

#ifndef AVAGO_SERDES_PMD_H_
#define AVAGO_SERDES_PMD_H_

/*////////////////////////////////////////////////////////////////////////////// */
/* START: KR functions and structs */

/**@brief Controls which Rx DFE settings are updated. */
typedef enum
{
    AVAGO_PMD_RESTART,        /**< Reset the PMD training */
    AVAGO_PMD_BYPASS,         /**< Set Tx Eq to Initialize and start sending core data */
    AVAGO_PMD_TRAIN           /**< Run full PMD training */
} Avago_serdes_pmd_train_mode_t;

/**@brief Controls which Rx DFE settings are updated. */
typedef enum
{
    AVAGO_PMD_FC16G,         /**< PRBS11 - x^11 + x^9 + 1 */
    AVAGO_PMD_CL72,          /**< PRBS11 - x^11 + x^9 + 1 */
    AVAGO_PMD_CL84,          /**< PRBS11 - x^11 + x^9 + 1 */
    AVAGO_PMD_CL92           /**< PRBS11 - sequence based on lane config */
} Avago_serdes_pmd_clause_t;

/**@brief Link Training Configuration struct. */
typedef struct
{
  Avago_serdes_pmd_train_mode_t train_mode;/**< Select pmd link training mode to execute by serdes_pmd_train */

  uint sbus_addr;        /**< SBus Rx Address of SerDes to run training on */

  BOOL disable_timeout;  /**< Disables the timeout timer */

  Avago_serdes_pmd_clause_t clause; /**< CL72,CL84,CL92 */
  uint                        lane; /**< When clause is set to CL92, defines which PMD lane this SerDes is */
  uint                   prbs_seed; /**< When non-zero overrides the default PRBS seed value */

  BOOL  TT_FECreq;      /**< Set to transmit FEC request in TT training frames. */
  BOOL  TT_FECcap;      /**< Set to transmit FEC capability in TT training frames. */
  BOOL  TT_TF;          /**< Set to transmit Fixed TxEq in TT training frames. */

} Avago_serdes_pmd_config_t;

#ifndef MV_HWS_REDUCED_BUILD_EXT_CM3
EXT Avago_serdes_pmd_config_t *avago_serdes_pmd_config_construct(Aapl_t *aapl);
EXT void avago_serdes_pmd_config_destruct( Aapl_t *aapl, Avago_serdes_pmd_config_t *pmd_config);
#endif /* MV_HWS_REDUCED_BUILD_EXT_CM3 */
EXT void avago_serdes_pmd_init(Aapl_t *aapl, Avago_serdes_pmd_config_t *pmd_config);
EXT void avago_serdes_pmd_train(Aapl_t *aapl, Avago_serdes_pmd_config_t *mode_control);
EXT int avago_serdes_pmd_status(Aapl_t *aapl, uint addr);

#endif
