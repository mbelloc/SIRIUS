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

#include "sirius/gdal/shifted_input_stream.h"

#include "sirius/exception.h"
#include "sirius/types.h"

#include "sirius/gdal/error_code.h"
#include "sirius/gdal/wrapper.h"

#include "sirius/utils/log.h"

namespace sirius {
namespace gdal {

ShiftedInputStream::ShiftedInputStream(const std::string& image_path,
                                       const sirius::Size& block_size,
                                       float row_shift, float col_shift)
    : input_dataset_(gdal::LoadDataset(image_path)),
      block_size_(block_size),
      row_shift_(row_shift),
      col_shift_(col_shift),
      is_ended_(false),
      row_idx_(0),
      col_idx_(0) {
    if (block_size_.row <= 0 || block_size_.col <= 0) {
        LOG("shifted_input_stream", error, "invalid block size");
        throw SiriusException("invalid block size");
    }

    LOG("shifted_input_stream", info, "input image '{}' ({}x{})", image_path,
        input_dataset_->GetRasterYSize(), input_dataset_->GetRasterXSize());
}

StreamBlock ShiftedInputStream::Read(std::error_code& ec) {
    if (is_ended_) {
        ec = make_error_code(CPLE_ObjectNull);
        return {};
    }

    int w = input_dataset_->GetRasterXSize();
    int h = input_dataset_->GetRasterYSize();
    int w_to_read = block_size_.col;
    int h_to_read = block_size_.row;

    if (w_to_read > w || h_to_read > h) {
        LOG("shifted_input_stream", critical,
            "requested block size ({}x{}) is bigger than source image ({}x{}). "
            "You should use regular processing",
            w_to_read, h_to_read, w, h);
        ec = make_error_code(CPLE_ObjectNull);
        return {};
    }

    // resize block if needed
    if (row_idx_ + h_to_read > h) {
        // assign size that can be read
        h_to_read -= (row_idx_ + h_to_read - h);
    }
    if (col_idx_ + w_to_read > w) {
        w_to_read -= (col_idx_ + w_to_read - w);
    }

    Image output_buffer({h_to_read, w_to_read});

    CPLErr err = input_dataset_->GetRasterBand(1)->RasterIO(
          GF_Read, col_idx_, row_idx_, w_to_read, h_to_read,
          output_buffer.data.data(), w_to_read, h_to_read, GDT_Float64, 0, 0);

    if (err) {
        LOG("shifted_input_stream", error,
            "GDAL error: {} - could not read from the dataset", err);
        ec = make_error_code(err);
        return {};
    }

    int block_row_idx = row_idx_;
    int block_col_idx = col_idx_;

    StreamBlock output_block(std::move(output_buffer), block_row_idx,
                             block_col_idx, Padding(0, 0, 0, 0));

    if (((row_idx_ + block_size_.row) >= h) &&
        ((col_idx_ + block_size_.col) >= w)) {
        is_ended_ = true;
    }

    if (col_idx_ >= w - block_size_.col) {
        col_idx_ = 0;
        row_idx_ += block_size_.row - std::ceil(std::abs(row_shift_));
    } else {
        col_idx_ += block_size_.col - std::ceil(std::abs(col_shift_));
    }

    LOG("shifted_input_stream", debug, "reading block of size {}x{} at ({},{})",
        output_block.buffer.size.row, output_block.buffer.size.col,
        output_block.row_idx, output_block.col_idx);

    ec = make_error_code(CPLE_None);
    return output_block;
}

}  // namespace gdal
}  // namespace sirius
