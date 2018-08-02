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

#include "sirius/gdal/shifted_output_stream.h"

#include "sirius/gdal/error_code.h"
#include "sirius/gdal/wrapper.h"

#include "sirius/utils/log.h"

namespace sirius {
namespace gdal {

ShiftedOutputStream::ShiftedOutputStream(const std::string& input_path,
                                         const std::string& output_path,
                                         float row_shift, float col_shift) {
    auto input_dataset = gdal::LoadDataset(input_path);

    int output_h =
          std::ceil(input_dataset->GetRasterYSize() - std::ceil(row_shift));
    int output_w =
          std::ceil(input_dataset->GetRasterXSize() - std::ceil(col_shift));

    auto geo_ref =
          gdal::ComputeShiftedGeoReference(input_path, row_shift, col_shift);
    output_dataset_ =
          gdal::CreateDataset(output_path, output_w, output_h, 1, geo_ref);
    LOG("shifted_output_stream", info, "shifted image '{}' ({}x{})",
        output_path, output_h, output_w);
}

void ShiftedOutputStream::Write(StreamBlock&& block, std::error_code& ec) {
    CPLErr err = output_dataset_->GetRasterBand(1)->RasterIO(
          GF_Write, block.col_idx, block.row_idx, block.buffer.size.col,
          block.buffer.size.row, const_cast<double*>(block.buffer.data.data()),
          block.buffer.size.col, block.buffer.size.row, GDT_Float64, 0, 0,
          NULL);
    if (err) {
        LOG("shifted_output_stream", error,
            "GDAL error: {} - could not write to the given dataset", err);
        ec = make_error_code(err);
        return;
    }
    ec = make_error_code(CPLE_None);
}

}  // namespace gdal
}  // namespace sirus
