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

#ifndef SIRIUS_GDAL_SHIFTED_OUTPUT_STREAM_H_
#define SIRIUS_GDAL_SHIFTED_OUTPUT_STREAM_H_

#include <system_error>

#include "sirius/types.h"

#include "sirius/gdal/stream_block.h"
#include "sirius/gdal/types.h"

namespace sirius {
namespace gdal {

/**
 * \brief Write a resampled image by block
 */
class ShiftedOutputStream {
  public:
    ShiftedOutputStream(const std::string& input_path,
                        const std::string& output_path, float row_shift,
                        float col_shift);

    ~ShiftedOutputStream() = default;
    ShiftedOutputStream(const ShiftedOutputStream&) = delete;
    ShiftedOutputStream& operator=(const ShiftedOutputStream&) = delete;
    ShiftedOutputStream(ShiftedOutputStream&&) = delete;
    ShiftedOutputStream& operator=(ShiftedOutputStream&&) = delete;

    /**
     * \brief Write a zoomed block in the output file
     * \param block block to write
     * \param ec error code if operation failed
     */
    void Write(StreamBlock&& block, std::error_code& ec);

  private:
    gdal::DatasetUPtr output_dataset_;
};

}  // namespace gdal
}  // namespace sirius

#endif  // SIRIUS_GDAL_SHIFTED_OUTPUT_STREAM_H_