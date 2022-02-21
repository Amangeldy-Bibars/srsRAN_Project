
#include "du_manager_impl.h"

namespace srsgnb {

du_manager_impl::du_manager_impl(mac_config_interface&       mac,
                                 du_manager_config_notifier& f1ap_notifier,
                                 rlc_ul_sdu_notifier&        rlc_ul_notifier,
                                 task_executor&              du_mng_exec_) :
  du_mng_exec(du_mng_exec_)
{
  ctxt.mac               = &mac;
  ctxt.f1ap_cfg_notifier = &f1ap_notifier;
  ctxt.rlc_ul_notifier   = &rlc_ul_notifier;
}

void du_manager_impl::ue_create(const du_ue_create_message& msg)
{
  du_mng_exec.execute([this, msg]() {
    // Start UE create procedure
    ue_create_proc = std::make_unique<ue_creation_procedure>(ctxt, msg);
  });
}

void du_manager_impl::handle_mac_ue_create_response(const mac_ue_create_request_response_message& resp)
{
  ue_create_proc->mac_ue_create_response(resp);

  // Procedure complete
  ue_create_proc.reset();
}

std::string du_manager_impl::get_ues()
{
  return fmt::format("{}", ctxt.ue_db.size());
}

} // namespace srsgnb
