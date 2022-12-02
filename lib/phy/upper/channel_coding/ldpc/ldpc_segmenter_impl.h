/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief LDPC codeblock segmentation declaration.

#pragma once

#include "ldpc_graph_impl.h"
#include "srsgnb/phy/upper/channel_coding/crc_calculator.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc_segmenter_rx.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc_segmenter_tx.h"

namespace srsgnb {

/// Maximum accepted transport block size.
static constexpr unsigned MAX_TBS = 1277992;

/// \brief Generic implementation of LDPC segmentation.
///
/// Implements both ldpc_segmenter_tx and ldpc_segmenter_rx. For this reason, the constructor has been hidden behind the
/// static wrapper methods create_ldpc_segmenter_impl_tx() and create_ldpc_segmenter_impl_rx().
class ldpc_segmenter_impl : public ldpc_segmenter_tx, public ldpc_segmenter_rx
{
public:
  /// CRC calculators used in shared channels.
  struct sch_crc {
    /// For short TB checksums.
    std::unique_ptr<crc_calculator> crc16;
    /// For long TB checksums.
    std::unique_ptr<crc_calculator> crc24A;
    /// For segment-specific checksums.
    std::unique_ptr<crc_calculator> crc24B;
  };

  /// \brief Wraps the constructor of the Tx version of the LDPC segmenter.
  static std::unique_ptr<ldpc_segmenter_tx> create_ldpc_segmenter_impl_tx(sch_crc&);

  /// \brief Wraps the constructor of the Rx version of the LDPC segmenter.
  /// \remark The receive-chain version of the segmenter does not need CRC calculators.
  static std::unique_ptr<ldpc_segmenter_rx> create_ldpc_segmenter_impl_rx();

  // Tx-chain segmentation.
  // See interface for the documentation.
  void segment(static_vector<described_segment, MAX_NOF_SEGMENTS>& described_segments,
               span<const uint8_t>                                 transport_block,
               const segmenter_config&                             cfg) override;

  // Rx-chain segmentation.
  // See interface for the documentation.
  void segment(static_vector<described_rx_codeblock, MAX_NOF_SEGMENTS>& described_codeblocks,
               span<const log_likelihood_ratio>                         codeword_llrs,
               unsigned                                                 tbs,
               const segmenter_config&                                  cfg) override;

private:
  /// Default constructor.
  ldpc_segmenter_impl() : crc_set({nullptr, nullptr, nullptr}){};

  /// \brief Creates an LDPC segmentation object that aggregates a crc_calculator.
  ///
  /// \param[in] c The CRC calculator to aggregate. The generation polynomial must be CRC24B.
  explicit ldpc_segmenter_impl(sch_crc c) : crc_set({std::move(c.crc16), std::move(c.crc24A), std::move(c.crc24B)}){};

  /// Computes the length of the rate-matched codeblock corresponding to each segment, as per TS38.212
  /// Section 5.4.2.1.
  unsigned compute_rm_length(unsigned i_seg, modulation_scheme mod, unsigned nof_layers) const;

  /// Internally computed segment metadata.
  struct segment_internal {
    /// Segment index.
    unsigned i_segment;
    /// Total codeword length.
    unsigned cw_length;
    /// Codeblock starting index within the codeword.
    unsigned cw_offset;
    /// Number of filler bits.
    unsigned nof_filler_bits;
    /// Number of segment-specific CRC bits.
    unsigned nof_crc_bits;
    /// Number of TB-specific CRC bits.
    unsigned nof_tb_crc_bits;
  };

  /// Generates a codeblock metadata structure for the current segment configuration.
  codeblock_metadata generate_cb_metadata(const segment_internal& seg_extra, const segmenter_config& cfg) const;

  /// Base graph used for encoding/decoding the current transport block.
  ldpc_base_graph_type base_graph = ldpc_base_graph_type::BG1;
  /// Lifting size used for encoding/decoding the current transport block.
  unsigned lifting_size = 0;

  /// \name Attributes relative to TS38.212 Section 5.2.2.
  ///@{

  /// Final length of a segment (corresponds to \f$K\f$).
  unsigned segment_length = 0;
  /// Number of bits in the transport block (corresponds to \f$B\f$).
  unsigned nof_tb_bits_in = 0;
  /// Augmented number of bits in the transport block, including new CRCs (corresponds to \f$B'\f$).
  unsigned nof_tb_bits_out = 0;
  /// Number of segments resulting from the transport block (corresponds to \f$C\f$).
  unsigned nof_segments = 0;
  ///@}

  /// \name Attributes relative to TS38.212 Section 5.4.2.1.
  ///@{

  /// Number of symbols per transmission layer (corresponds to \f$G / (N_L Q_m)\f$).
  unsigned nof_symbols_per_layer = 0;
  /// \brief Number of segments of short rate-matched length (corresponds to \f$C - \bigr(\bigl(G / (N_L Q_m)\bigr)
  /// \bmod C\bigr)\f$).
  unsigned nof_short_segments = 0;
  ///@}

  /// CRC calculators for transport-block and segment-specific checksums.
  sch_crc crc_set;
};

} // namespace srsgnb
