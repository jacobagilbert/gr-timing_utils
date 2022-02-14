/* -*- c++ -*- */
/*
 * Copyright 2018, 2019, 2020 National Technology & Engineering Solutions of
 * Sandia, LLC (NTESS). Under the terms of Contract DE-NA0003525 with NTESS, the U.S.
 * Government retains certain rights in this software.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_TIMING_UTILS_INTERRUPT_EMITTER_H
#define INCLUDED_TIMING_UTILS_INTERRUPT_EMITTER_H

#include <gnuradio/sync_block.h>
#include <timing_utils/api.h>

namespace gr {
namespace timing_utils {

/*!
 * \brief Emit a message at a desired time based on the time of a processed
 * sample
 *
 * \ingroup timing_utils
 *
 * For applications requiring an interrupt like event to wake or trigger
 * functionality, the interrupt_emitter block emits a message based
 * partially on the time of the currently processed sample and the system
 * time.  Using the system time as a reference point, the interrupt
 * emitter can more accurately emit a message without actually processing
 * the sample nearest in time to the requested interrupt.
 *
 * The emitted message is a dictionary with the following elements:
 *    - trigger_time (original request type, either uint64, pair, or tuple)
 *    - trigger_sample (uint64)
 *    - late_delta (double)
 *
 * In the event of a late interrupt being issued, the
 * dictionary element `late_delta` gives an the difference between the
 * requested interrupt time and the actual interrupt time
 *
 * The `rx_time` stream tag is used to adjust the internal times, accounting
 * for things like overflows or discontinous streams of data
 *
 * The \p loop_gain parameter specifies the update rate for the error tracking
 * loop.  The error tracking loop is used to compensate for drift between
 * the system clock and the data source clock.  As the block only knows
 * system time explicitly, based on the timestamp associated with samples in
 * each work function, the system clock is skewed in order to produce an
 * estimate of data source time at the requested time.
 *
 * The loop gain parameter should be set according to:
 *    m = maximum system to data clock rate drift
 *    c = maximal noisy error estimate
 *
 *    gain >= (m / (m + c))
 *
 * The maximal noisy error estimate is the result of system loading causing
 * gnuradio scheduling to process data at varying rates.  The gain is inversely
 * proportional to the noisy estimate, indicating that as the noise increases,
 * the gain should decrease to compensate for the noise.
 *
 * Note: This block has been templatized to maintain backward compatability
 * (Each block is instantiated based on the input/output data type)
 */
template <class T>
class TIMING_UTILS_API interrupt_emitter : virtual public gr::sync_block
{
public:
    typedef boost::shared_ptr<interrupt_emitter<T>> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of
     * timing_utils::uhd_timed_pdu_emitter.
     *
     *
     * \param rate Sample rate (Hz)
     * \param drop_late If true, do not emit a message for interrupt requests
     *    in the past
     * \param loop_gain Clock drift tracking loop gain
     *
     * \return pointer to new instance
     */
    static sptr make(double rate, bool drop_late, double loop_gain = .0001);

    /*! \brief Set the stream rate
     *
     * \param rate Sample rate (Hz)
     */
    virtual void set_rate(double rate) = 0;

    /*! \brief Set debug mode
     *
     * \param value Enable additional debug statements that are printed
     *     directly to stdout
     */
    virtual void set_debug(bool value) = 0;
};

typedef interrupt_emitter<unsigned char> interrupt_emitter_b;
typedef interrupt_emitter<short> interrupt_emitter_s;
typedef interrupt_emitter<int32_t> interrupt_emitter_i;
typedef interrupt_emitter<float> interrupt_emitter_f;
typedef interrupt_emitter<gr_complex> interrupt_emitter_c;
} // namespace timing_utils
} // namespace gr

#endif /* INCLUDED_TIMING_UTILS_INTERRUPT_EMITTER_H */
