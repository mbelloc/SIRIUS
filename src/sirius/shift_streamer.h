/**
 * Copyright (C) 2018 CS - Systemes d'Information (CS-SI)
 *
 * This file is part of Sirius
 *
 *     https://github.com/CS-SI/SIRIUS
 *
 * Sirius is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sirius is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Sirius.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SIRIUS_SHIFT_STREAMER_H_
#define SIRIUS_SHIFT_STREAMER_H_

#include "sirius/translation/frequency_translation.h"

#include "sirius/gdal/input_stream.h"
#include "sirius/gdal/shifted_output_stream.h"
#include "sirius/gdal/wrapper.h"

namespace sirius {

/**
 * \brief Image streamer with monothread or multithread strategies
 */
class ShiftStreamer {
  public:
    /**
     * \brief Instanciate an image streamer which will stream input image, apply
     *   a resampling transformation and write the result into the output image
     * \param input_path input image path
     * \param output_path output image path
     * \param block_size stream block size
     * \param row_shift translation on y axis
     * \param col_shift translation on x axis
     * \param max_parallel_workers max parallel workers to compute the zoom on
     *        stream blocks
     */
    ShiftStreamer(const std::string& input_path, const std::string& output_path,
                  const Size& block_size, float row_shift, float col_shift,
                  unsigned int max_parallel_workers);

    /**
     * \brief Stream the input image, compute the resampling and stream
     *   output data
     * \param frequency_shifter requested frequency shifter to apply on
     *   stream block
     */
    void Stream(FrequencyTranslation& frequency_shifter);

  private:
    /**
     * \brief Stream image in monothreading mode
     *
     * Read a block, compute the transmation and write the output in the output
     *   file
     *
     * \param frequency_shifter frequency translation to apply on stream block
     */
    void RunMonothreadStream(FrequencyTranslation& frequency_shifter);

    /**
     * \brief Stream image in multithreading mode
     *
     * One thread will generate input blocks and feed an input queue
     * One thread will consume shifted blocks from an output queue and write
     *   them in the output file
     * max_parallel_tasks threads will consume input blocks from an input queue,
     *   compute the shifted blocks and feed an output queue
     *
     * \param frequency_shifted frequency translation to apply on stream block
     */
    void RunMultithreadStream(FrequencyTranslation& frequency_shifter);

  private:
    unsigned int max_parallel_workers_;
    Size block_size_;
    gdal::InputStream input_stream_;
    gdal::ShiftedOutputStream output_stream_;
};

}  // namespace sirius

#endif  // SIRIUS_SHIFT_STREAMER_H_
