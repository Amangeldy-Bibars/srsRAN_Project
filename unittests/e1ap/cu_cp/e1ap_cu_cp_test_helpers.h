/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../common/e1ap_cu_cp_test_messages.h"
#include "../common/test_helpers.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/e1/common/e1_common.h"
#include "srsgnb/e1/cu_cp/e1_cu_cp.h"
#include "srsgnb/e1/cu_cp/e1_cu_cp_factory.h"
#include <gtest/gtest.h>

namespace srsgnb {
namespace srs_cu_cp {

/// Fixture class for E1AP
class e1ap_cu_cp_test : public ::testing::Test
{
protected:
  struct test_ue {
    cu_cp_ue_id_t                    cu_cp_ue_id;
    optional<gnb_cu_cp_ue_e1ap_id_t> cu_cp_ue_e1ap_id;
    optional<gnb_cu_up_ue_e1ap_id_t> cu_up_ue_e1ap_id;
  };

  e1ap_cu_cp_test();
  ~e1ap_cu_cp_test() override;

  /// \brief Helper method to run E1AP CU-CP Bearer Context Setup procedure.
  void run_bearer_context_setup(cu_cp_ue_id_t cu_cp_ue_id, gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id);

  srslog::basic_logger& e1ap_logger = srslog::fetch_basic_logger("E1AP");
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  slotted_id_table<cu_cp_ue_id_t, test_ue, MAX_NOF_CU_UES> test_ues;

  timer_manager                     timers;
  dummy_e1_pdu_notifier             e1_pdu_notifier;
  dummy_e1_cu_up_processor_notifier cu_up_processor_notifier;
  std::unique_ptr<e1_interface>     e1ap;
};

} // namespace srs_cu_cp
} // namespace srsgnb