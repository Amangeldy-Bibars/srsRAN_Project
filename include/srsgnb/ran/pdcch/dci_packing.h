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

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/pdcch/pdcch_constants.h"
#include "srsgnb/adt/optional.h"

namespace srsgnb {

/// DCI payload data type.
using dci_payload = bounded_bitset<pdcch_constants::MAX_DCI_PAYLOAD_SIZE>;

/// DCI payload sizes.
struct dci_sizes {
  // TODO (joaquim): document these fields.
  unsigned format0_0_common_size;
  unsigned format1_0_common_size;
  unsigned format0_0_ue_specific_size;
  unsigned format1_0_ue_specific_size;
};

struct dci_config {
  // TODO (joaquim): document these fields.
  unsigned N_rb_dl_bwp_initial;

  /// UE DL dedicated search space.
  unsigned N_rb_dl_bwp_active;

  unsigned N_rb_ul_bwp_initial;
  unsigned N_rb_ul_bwp_active;

  unsigned coreset0_bw;

  bool enable_sul;


};

// TODO (joaquim): document these fields.
dci_payload get_dci_sizes(dci_config config);

/// \brief Describes the necessary parameters for packing a DCI format 0_0 scrambled by C-RNTI, CS-RNTI or MCS-C-RNTI.
/// \remark Defined in TS38.212 Section 7.3.1.1.1.
struct dci_0_0_c_rnti_configuration {
  /// \brief DCI format 0_0 payload size.
  ///
  /// the DCI payload size is determined by the DCI size alignment procedure, specified in TS38.212 Section 7.3.1.0 and
  /// implemented by \c get_dci_sizes.
  unsigned payload_size;
  /// \brief Identifier for DCI formats – 1 bit.
  /// \remark The value of this field is always set to 0, indicating an UL DCI format.
  unsigned dci_format_id;
  /// \brief Parameter \f$N_{UL_hop}\f$, as per TS38.212 Section 7.3.1.1.1.
  ///
  /// \f$N_{UL_hop}\f$ is the number of bits used in the DCI payload to pack the frequency hopping offset, represented
  /// by \c hopping_offset.
  ///
  /// If frequency hopping is enabled via the \c frequency_hopping_flag, it must be set to:
  ///   - 1, if the higher layer parameter frequencyHoppingOffsetLists has 2 possible offsets.
  ///   - 2, if the higher layer parameter frequencyHoppingOffsetLists has 4 possible offsets.
  unsigned N_ul_hop;
  /// \brief Frequency hopping offset, selected from the higher layer parameter \c frequencyHoppingOffsetLists.
  ///
  /// \c frequencyHoppingOffsetLists provides 2 or 4 selectable frequency offsets, as per TS38.214 Section 6.3. the
  /// \c hopping_offset field acts as an index for \c frequencyHoppingOffsetLists, determining which of the possible
  /// offsets is used.
  ///
  /// If frequency hopping is enabled via the \c frequency_hopping flag, it must be set to one of the following values:
  ///   - (0, 1), if the higher layer parameter frequencyHoppingOffsetLists has 2 possible offsets.
  ///   - (0, 1, 2, 3), if the higher layer parameter frequencyHoppingOffsetLists has 4 possible offsets.
  unsigned hopping_offset;
  /// Parameter \f$N_{RB}^{UL,BWP}\f$. It must be set according to TS38.212 Section 7.3.1.0.
  unsigned N_rb_ul_bwp;
  /// \brief Frequency domain resource assignment - \f$\Bigl \lceil log_2(N_{RB}^{UL,BWP}(N_{RB}^{UL,BWP}+1)/2) \Bigr
  /// \rceil\f$ bits as per TS38.214 Section 6.1.2.2.2.
  unsigned frequency_resource;
  /// Time domain resource assignment - 4 bit as per TS38.214 Section 6.1.2.1.
  unsigned time_resource;
  /// Frequency hopping flag. 1 bit as per TS38.214 Section 6.3 and Table 7.3.1.1.1-3.
  unsigned frequency_hopping_flag;
  /// Modulation and coding scheme - 5 bits as per TS38.214 Section 6.1.4.1.
  unsigned modulation_coding_scheme;
  /// New data indicator - 1 bit.
  unsigned new_data_indicator;
  /// Redundancy version - 2 bits as per TS38.212 Table 7.3.1.1.1-2.
  unsigned redundancy_version;
  /// HARQ process number - 4 bits.
  unsigned harq_process_number;
  /// TPC command for scheduled PUSCH – 2 bits as per TS38.213 Section 7.1.1.
  unsigned tpc_command;
  /// UL/SUL indicator - 1 bit if present, as per TS38.212 Section 7.3.1.1.1 and Table 7.3.1.1.1-1.
  optional<bool> ul_sul_indicator;
};

/// Packs a DCI format 0_0 scrambled by C-RNTI, CS-RNTI or MCS-C-RNTI.
dci_payload dci_0_0_c_rnti_pack(const dci_0_0_c_rnti_configuration& config);

/// \brief Describes the necessary parameters for packing a DCI format 0_0 scrambled by TC-RNTI.
/// \remark Defined in TS38.212 Section 7.3.1.1.1.
struct dci_0_0_tc_rnti_configuration {
  /// \brief DCI format 0_0 payload size.
  ///
  /// the DCI payload size is determined by the DCI size alignment procedure, specified in TS38.212 Section 7.3.1.0 and
  /// implemented by \c get_dci_sizes.
  unsigned payload_size;
  /// \brief Identifier for DCI formats – 1 bit.
  /// \remark The value of this field is always set to 0, indicating an UL DCI format.
  unsigned dci_format_id;
  /// \brief Parameter \f$N_{UL_hop}\f$, as per TS38.212 Section 7.3.1.1.1.
  ///
  /// \f$N_{UL_hop}\f$ is the number of bits used in the DCI payload to pack the frequency hopping offset, represented
  /// by \c hopping_offset.
  ///
  /// If frequency hopping is enabled via the \c frequency_hopping_flag, it must be set to:
  ///   - 1, if \f$N_{RB}^{UL,BWP}\f$ is smaller than 50.
  ///   - 2 otherwise.
  unsigned N_ul_hop;
  /// \brief Frequency hopping offset, as per TS 38.213 Section 8.3 and Table 8.3-1.
  ///
  /// If frequency hopping is enabled via the \c frequency_hopping flag, it must be set to one of the following values:
  ///   - (0, 1), if \c N_ul_hop is set to 1.
  ///   - (0, 1, 2, 3), if \c N_ul_hop is set to 2.
  unsigned hopping_offset;
  /// Parameter \f$N_{RB}^{UL,BWP}\f$. It must be set to the size of the initial UL BWP.
  unsigned N_rb_ul_bwp;
  /// \brief Frequency domain resource assignment - \f$\Bigl \lceil log_2(N_{RB}^{UL,BWP}(N_{RB}^{UL,BWP}+1)/2) \Bigr
  /// \rceil\f$ bits as per TS38.214 Section 6.1.2.2.2.
  unsigned frequency_resource;
  /// Time domain resource assignment - 4 bit as per TS38.214 Section 6.1.2.1.
  unsigned time_resource;
  /// Frequency hopping flag. 1 bit as per TS38.214 Section 6.3 and Table 7.3.1.1.1-3.
  unsigned frequency_hopping_flag;
  /// Modulation and coding scheme - 5 bits as per TS38.214 Section 6.1.4.1.
  unsigned modulation_coding_scheme;
  /// Redundancy version - 2 bits as per TS38.212 Table 7.3.1.1.1-2.
  unsigned redundancy_version;
  /// TPC command for scheduled PUSCH – 2 bits as per TS38.213 Section 7.1.1.
  unsigned tpc_command;
};

/// Packs a DCI format 0_0 scrambled by TC-RNTI.
dci_payload dci_0_0_tc_rnti_pack(const dci_0_0_tc_rnti_configuration& config);

/// \brief Describes the necessary parameters for packing a DCI format 1_0 scrambled by C-RNTI, CS-RNTI or MCS-C-RNTI.
/// \remark Defined in TS38.212 Section 7.3.1.2.1.
/// \remark The case where the random access procedure is initiated by a PDCCH order is handled separately.
struct dci_1_0_c_rnti_configuration {
  /// \brief DCI format 1_0 payload size.
  ///
  /// the DCI payload size is determined by the DCI size alignment procedure, specified in TS38.212 Section 7.3.1.0 and
  /// implemented by \c get_dci_sizes.
  unsigned payload_size;
  /// \brief Identifier for DCI formats – 1 bit.
  /// \remark The value of this field is always set to 1, indicating a DL DCI format.
  unsigned dci_format_id;
  /// Parameter \f$N_{RB}^{DL,BWP}\f$. It must be set according to TS38.212 Section 7.3.1.0.
  unsigned N_rb_dl_bwp;
  /// \brief Frequency domain resource assignment - \f$\Bigl \lceil log_2(N_{RB}^{DL,BWP}(N_{RB}^{DL,BWP}+1)/2) \Bigr
  /// \rceil\f$ bits as per TS38.214 Section 5.1.2.2.2.
  unsigned frequency_resource;
  /// Time domain resource assignment - 4 bit as per TS38.214 Section 5.1.2.1.
  unsigned time_resource;
  /// VRB-to-PRB mapping - 1 bit as per TS38.212 Table 7.3.1.2.2-5.
  unsigned vrb_to_prb_mapping;
  /// Modulation and coding scheme - 5 bits as per TS38.214 Section 5.1.3.
  unsigned modulation_coding_scheme;
  /// New data indicator - 1 bit.
  unsigned new_data_indicator;
  /// Redundancy version - 2 bits as per TS38.212 Table 7.3.1.1.1-2.
  unsigned redundancy_version;
  /// HARQ process number - 4 bits.
  unsigned harq_process_number;
  /// Downlink assignment index - 2 bits as per TS38.213 Section 9.1.3.
  unsigned dl_assignment_index;
  /// TPC command for scheduled PUCCH – 2 bits as per TS38.213 Section 7.2.1.
  unsigned tpc_command;
  /// PUCCH resource indicator – 3 bits as per TS38.213 Section 9.2.3.
  unsigned pucch_resource_indicator;
  /// PDSCH to HARQ feedback timing indicator – 3 bits as per TS38.213 Section 9.2.3.
  unsigned pdsch_harq_fb_timing_indicator;
};

/// Packs a DCI format 1_0 scrambled by C-RNTI, CS-RNTI or MCS-C-RNTI.
dci_payload dci_1_0_c_rnti_pack(const dci_1_0_c_rnti_configuration& config);

/// \brief Describes the necessary parameters for packing a DCI format 1_0 scrambled by P-RNTI as per TS38.212
/// Section 7.3.1.2.1.
struct dci_1_0_p_rnti_configuration {
  /// \brief DCI format 1_0 payload size.
  ///
  /// the DCI payload size is determined by the DCI size alignment procedure, specified in TS38.212 Section 7.3.1.0 and
  /// implemented by \c get_dci_sizes.
  unsigned payload_size;
  /// Indicates the content of the DCI format 1_0 payload when scrambled by P-RNTI.
  enum class payload_info {
    /// Indicates that only scheduling information for paging is present in the DCI.
    scheduling_information = 1,
    /// Indicates that only short message is present in the DCI.
    short_messages = 2,
    /// Indicates that both scheduling information for paging and short message are present in the DCI.
    both = 3
  };
  /// Parameter \f$N_{RB}^{DL,BWP}\f$. It must be set to CORESET0 size.
  unsigned N_rb_dl_bwp;
  /// Short Messages Indicator - 2 bits as per TS38.212 Section 7.3.1.2 and Table 7.3.1.2.1-1.
  payload_info short_messages_indicator;
  /// \brief Short Messages - 8 bits as per TS38.331 Section 6.5 and Table 6.5-1.
  /// \remark If only the scheduling information for paging is carried, this bit field is reserved.
  unsigned short_messages;
  /// \brief Frequency domain resource assignment - \f$\Bigl \lceil log_2(N_{RB}^{DL,BWP}(N_{RB}^{DL,BWP}+1)/2) \Bigr
  /// \rceil\f$ bits as per TS38.214 Section 5.1.2.2.2.
  /// \remark If only the short message is carried, this bit field is reserved.
  unsigned frequency_resource;
  /// \brief Time domain resource assignment - 4 bit as per TS38.214 Section 5.1.2.1.
  /// \remark If only the short message is carried, this bit field is reserved.
  unsigned time_resource;
  /// \brief VRB-to-PRB mapping - 1 bit as per to TS38.212 Table 7.3.1.2.2-5.
  /// \remark If only the short message is carried, this bit field is reserved.
  unsigned vrb_to_prb_mapping;
  /// \brief Modulation and coding scheme - 5 bits as per TS38.214 Section 5.1.3 and Table 5.1.3.1-1.
  /// \remark If only the short message is carried, this bit field is reserved.
  unsigned modulation_coding_scheme;
  /// \brief Transport Block scaling - 2 bits as per TS38.214 Section 5.1.3 and Table 5.1.3.2-2.
  /// \remark The value \c 0b11 is reserved.
  /// \remark If only the short message is carried, this bit field is reserved.
  unsigned tb_scaling;
  // Reserved bits - 6 bits.
};

/// Packs a DCI format 1_0 scrambled by P-RNTI.
dci_payload dci_1_0_p_rnti_pack(const dci_1_0_p_rnti_configuration& config);

/// \brief Describes the necessary parameters for packing a DCI format 1_0 scrambled by SI-RNTI as per TS38.212
/// Section 7.3.1.2.1.
struct dci_1_0_si_rnti_configuration {
  /// \brief DCI format 1_0 payload size.
  ///
  /// the DCI payload size is determined by the DCI size alignment procedure, specified in TS38.212 Section 7.3.1.0 and
  /// implemented by \c get_dci_sizes.
  unsigned payload_size;
  /// Parameter \f$N_{RB}^{DL,BWP}\f$. It must be set to CORESET0 size.
  unsigned N_rb_dl_bwp;
  /// \brief Frequency domain resource assignment - \f$\Bigl \lceil log_2(N_{RB}^{DL,BWP}(N_{RB}^{DL,BWP}+1)/2) \Bigr
  /// \rceil\f$ bits as per TS38.214 Section 5.1.2.2.2.
  unsigned frequency_resource;
  /// Time domain resource assignment - 4 bit as per TS38.214 Section 5.1.2.1.
  unsigned time_resource;
  /// VRB-to-PRB mapping - 1 bit as per TS38.212 Table 7.3.1.2.2-5.
  unsigned vrb_to_prb_mapping;
  /// Modulation coding scheme - 5 bits as per TS38.214 Section 5.1.3 and Table 5.1.3.1-1.
  unsigned modulation_coding_scheme;
  /// Redundancy version - 2 bits as per TS38.212 Table 7.3.1.1.1-2.
  unsigned redundancy_version;
  /// System information indicator - 1 bit as per TS38.212 Table 7.3.1.2.1-2.
  unsigned system_information_indicator;
  // Reserved bits - 15 bits.
};

/// Packs a DCI format 1_0 scrambled by SI-RNTI.
dci_payload dci_1_0_si_rnti_pack(const dci_1_0_si_rnti_configuration& config);

/// \brief Describes the necessary parameters for packing a DCI format 1_0 scrambled by RA-RNTI as per TS38.212
/// Section 7.3.1.2.1.
struct dci_1_0_ra_rnti_configuration {
  /// \brief DCI format 1_0 payload size.
  ///
  /// the DCI payload size is determined by the DCI size alignment procedure, specified in TS38.212 Section 7.3.1.0 and
  /// implemented by \c get_dci_sizes.
  unsigned payload_size;
  /// \brief Parameter \f$N_{RB}^{DL,BWP}\f$.
  ///
  /// It must be set to:
  ///   - The CORESET0 size, if the CORESET0 is configured.
  ///   - Otherwise, the initial DL BWP size.
  unsigned N_rb_dl_bwp;
  /// \brief Frequency domain resource assignment - \f$\Bigl \lceil log_2(N_{RB}^{DL,BWP}(N_{RB}^{DL,BWP}+1)/2) \Bigr
  /// \rceil\f$ bits as per TS38.214 Section 5.1.2.2.2.
  unsigned frequency_resource;
  /// Time domain resource assignment - 4 bits as per TS38.214 Section 5.1.2.1.
  unsigned time_resource;
  /// VRB-to-PRB mapping - 1 bit as per to TS38.212 Table 7.3.1.2.2-5.
  unsigned vrb_to_prb_mapping;
  /// Modulation and coding scheme - 5 bits as per TS38.214 Section 5.1.3 and Table 5.1.3.1-1.
  unsigned modulation_coding_scheme;
  /// \brief Transport Block scaling - 2 bits as per TS38.214 Section 5.1.3 and Table 5.1.3.2-2.
  /// \remark The value \c 0b11 is reserved.
  unsigned tb_scaling;
  // Reserved bits - 16 bits.
};

/// Packs a DCI format 1_0 scrambled by RA-RNTI.
dci_payload dci_1_0_ra_rnti_pack(const dci_1_0_ra_rnti_configuration& config);

/// \brief Describes the necessary parameters for packing a DCI format 1_0 scrambled by TC-RNTI, as per TS38.212
/// Section 7.3.1.2.1.
struct dci_1_0_tc_rnti_configuration {
  /// \brief DCI format 1_0 payload size.
  ///
  /// the DCI payload size is determined by the DCI size alignment procedure, specified in TS38.212 Section 7.3.1.0 and
  /// implemented by \c get_dci_sizes.
  unsigned payload_size;
  /// \brief Identifier for DCI formats – 1 bit.
  /// \remark The value of this field is always set to 1, indicating a DL DCI format.
  unsigned dci_format_id;
  /// Parameter \f$N_{RB}^{DL,BWP}\f$. It must be set to the CORESET0 size.
  unsigned N_rb_dl_bwp;
  /// \brief Frequency domain resource assignment - \f$\Bigl \lceil log_2(N_{RB}^{DL,BWP}(N_{RB}^{DL,BWP}+1)/2) \Bigr
  /// \rceil\f$ bits as per TS38.214 Section 5.1.2.2.2.
  unsigned frequency_resource;
  /// Time domain resource assignment - 4 bit as per TS38.214 Section 5.1.2.1.
  unsigned time_resource;
  /// VRB-to-PRB mapping - 1 bit as per TS38.212 Table 7.3.1.2.2-5.
  unsigned vrb_to_prb_mapping;
  /// Modulation and coding scheme - 5 bits as per TS38.214 Table 5.1.3.1-1.
  unsigned modulation_coding_scheme;
  /// New data indicator - 1 bit.
  unsigned new_data_indicator;
  /// Redundancy version - 2 bits as per TS38.212 Table 7.3.1.1.1-2.
  unsigned redundancy_version;
  /// HARQ process number - 4 bits.
  unsigned harq_process_number;
  /// TPC command for scheduled PUCCH – 2 bits as per TS38.213 Section 7.2.1.
  unsigned tpc_command;
  /// PUCCH resource indicator – 3 bits as per TS38.213 Section 9.2.3.
  unsigned pucch_resource_indicator;
  /// PDSCH to HARQ feedback timing indicator – 3 bits as per TS38.213 Section 9.2.3.
  unsigned pdsch_harq_fb_timing_indicator;
};

/// Packs a DCI format 1_0 scrambled by TC-RNTI.
dci_payload dci_1_0_tc_rnti_pack(const dci_1_0_tc_rnti_configuration& config);

/// Collects the Random Access Response grant content fields described by TS38.213 Table 8.2-1.
struct dci_rar_configuration {
  /// Frequency hopping flag - 1 bit.
  unsigned frequency_hopping_flag;
  /// PUSCH frequency resource allocation - 14 bits.
  unsigned frequency_resource;
  /// PUSCH time resource allocation - 4 bits.
  unsigned time_resource;
  /// Modulation and coding scheme - 4 bits.
  unsigned modulation_coding_scheme;
  /// Transmission power control for PUSCH - 3 bits.
  unsigned tpc;
  /// CSI request - 1 bit.
  unsigned csi_request;
};

/// Packs a DCI transmitted in a Random Access Response.
dci_payload dci_rar_pack(const dci_rar_configuration& config);

} // namespace srsgnb
