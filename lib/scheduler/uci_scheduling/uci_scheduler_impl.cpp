/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "uci_scheduler_impl.h"
#include "../cell/resource_grid.h"
#include "uci_allocator_impl.h"

using namespace srsran;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uci_scheduler_impl::uci_scheduler_impl(const cell_configuration& cell_cfg_,
                                       uci_allocator&            uci_alloc_,
                                       ue_repository&            ues_) :
  cell_cfg(cell_cfg_), uci_alloc(uci_alloc_), ues(ues_), logger(srslog::fetch_basic_logger("SCHED"))
{
}

uci_scheduler_impl::~uci_scheduler_impl() = default;

void uci_scheduler_impl::run_slot(cell_resource_allocator& cell_alloc, slot_point sl_tx)
{
  uci_alloc.slot_indication(sl_tx);

  // Check if the slot is UL enabled.
  if (not cell_cfg.is_fully_ul_enabled(sl_tx)) {
    return;
  }

  // Iterate over the users to check for SR opportunities.
  for (auto& user : ues) {
    // At this point, we assume the config validator ensures there is pCell.
    auto& ue_cell = user->get_pcell();

    // NOTE: Allocating the CSI after the SR helps the PUCCH allocation to verify that the number of UCI bits allocated
    // over a PUCCH F2 grant is within PUCCH capacity.
    // TODO: possibly, this could be delegated to the Scheduler validator.
    if (ue_cell.is_pucch_grid_inited()) {
      // > Schedule SR grants.
      // At this point, we assume the UE has a \c ul_config, a \c pucch_cfg and a \c sr_res_list.
      const auto& sr_resource_cfg_list =
          ue_cell.cfg().cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value().sr_res_list;

      // Only allocate in the farthest slot in the grid, as the previous slot have been already served.
      auto& slot_alloc = cell_alloc[RING_ALLOCATOR_SIZE - 1];

      for (const auto& sr_res : sr_resource_cfg_list) {
        srsran_assert(sr_res.period >= sr_periodicity::sl_1, "Minimum supported SR periodicity is 1 slot.");

        // Check if this slot corresponds to an SR opportunity for the UE.
        if ((slot_alloc.slot - sr_res.offset).to_uint() % sr_periodicity_to_slot(sr_res.period) == 0) {
          // It is up to the UCI allocator to verify whether SR allocation can be skipped due to an existing PUCCH
          // grant.
          uci_alloc.uci_allocate_sr_opportunity(slot_alloc, user->crnti, ue_cell.cfg());
        }
      }

      // Schedule CSI grants.
      if (ue_cell.cfg().cfg_dedicated().csi_meas_cfg.has_value()) {
        // We assume we only use the first CSI report configuration.
        const unsigned csi_report_cfg_idx = 0;
        const auto&    csi_report_cfg =
            ue_cell.cfg().cfg_dedicated().csi_meas_cfg.value().csi_report_cfg_list[csi_report_cfg_idx];

        unsigned csi_offset =
            variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(csi_report_cfg.report_cfg_type)
                .report_slot_offset;
        unsigned csi_period = csi_report_periodicity_to_uint(
            variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(csi_report_cfg.report_cfg_type)
                .report_slot_period);

        // Check if this slot corresponds to a CSI opportunity for the UE.
        if ((slot_alloc.slot - csi_offset).to_uint() % csi_period == 0) {
          uci_alloc.uci_allocate_csi_opportunity(slot_alloc, user->crnti, ue_cell.cfg());
        }
      }
    } else {
      // > Schedule SR grants.
      // At this point, we assume the UE has a \c ul_config, a \c pucch_cfg and a \c sr_res_list.
      const auto& sr_resource_cfg_list =
          ue_cell.cfg().cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value().sr_res_list;

      for (const auto& sr_res : sr_resource_cfg_list) {
        srsran_assert(sr_res.period >= sr_periodicity::sl_1, "Minimum supported SR periodicity is 1 slot.");

        for (unsigned n = 0; n != RING_ALLOCATOR_SIZE; ++n) {
          auto& slot_alloc = cell_alloc[n];
          if ((slot_alloc.slot - sr_res.offset).to_uint() % sr_periodicity_to_slot(sr_res.period) == 0) {
            uci_alloc.uci_allocate_sr_opportunity(slot_alloc, user->crnti, ue_cell.cfg());
          }
        }
      }

      // > Schedule CSI grants.
      if (ue_cell.cfg().cfg_dedicated().csi_meas_cfg.has_value()) {
        // We assume we only use the first CSI report configuration.
        const unsigned csi_report_cfg_idx = 0;
        const auto&    csi_report_cfg =
            ue_cell.cfg().cfg_dedicated().csi_meas_cfg.value().csi_report_cfg_list[csi_report_cfg_idx];

        unsigned csi_offset =
            variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(csi_report_cfg.report_cfg_type)
                .report_slot_offset;
        unsigned csi_period = csi_report_periodicity_to_uint(
            variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(csi_report_cfg.report_cfg_type)
                .report_slot_period);

        for (unsigned n = 0; n != RING_ALLOCATOR_SIZE; ++n) {
          auto& slot_alloc = cell_alloc[n];
          if ((slot_alloc.slot - csi_offset).to_uint() % csi_period == 0) {
            uci_alloc.uci_allocate_csi_opportunity(slot_alloc, user->crnti, ue_cell.cfg());
          }
        }
      }
      ue_cell.set_pucch_grid_inited();
    }
  }
}
