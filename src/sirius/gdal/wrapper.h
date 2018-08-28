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

#ifndef SIRIUS_GDAL_WRAPPER_H_
#define SIRIUS_GDAL_WRAPPER_H_

#include <string>

#include "sirius/gdal/types.h"
#include "sirius/image.h"

namespace sirius {
namespace gdal {

/**
 * \brief Data class that represents GDAL geo reference information
 */
struct GeoReference {
    GeoReference();
    GeoReference(const std::vector<double>& geo_trans,
                 const std::string& proj_ref);

    ~GeoReference() = default;
    GeoReference(const GeoReference&) = default;
    GeoReference& operator=(const GeoReference&) = default;
    GeoReference(GeoReference&&) = default;
    GeoReference& operator=(GeoReference&&) = default;

    std::vector<double> geo_transform;
    std::string projection_ref;
    bool is_initialized{false};
};

Image LoadImage(const std::string& filepath);

void SaveImage(const Image& image, const std::string& output_filepath,
               const GeoReference& geoRef = {});

/**
 * \brief Write data to the given coordinates
 * \param dataset dataset to be modified
 * \param row_idx y offset from top border
 * \param col_idx x offset from left border
 * \param height height of the block to be copied
 * \param width width of the block to be copied
 * \param data data to be copied
 */
void WriteToDataset(GDALDataset* dataset, int row_idx, int col_idx, int height,
                    int width, const double* data);

DatasetUPtr LoadDataset(const std::string& filepath);

DatasetUPtr CreateDataset(const std::string& filepath, int w, int h,
                          int n_bands, const GeoReference& geo_ref = {});

GeoReference GetGeoReference(GDALDataset* dataset);

/**
 * \brief Compute resampled georeference information
 * \param input_path input image path
 * \param zoom_ratio zoom ratio
 */
GeoReference ComputeResampledGeoReference(const std::string& input_path,
                                          const ZoomRatio& zoom_ratio);

/**
 * \brief Compute output image origin and pixel size
 * \param dataset input dataset
 * \param zoom_ratio zoom ratio to be applied
 * \return new geo transform
 */
std::vector<double> ComputeResampledGeoTransform(GDALDataset* dataset,
                                                 const ZoomRatio& zoom_ratio);

/**
 * \brief Compute new origin according to translation paramaters
 * \param row_shift shift in pixels on x axis
 * \param col_shift shift in pixels on y axis
 * \return new geo transform
 */
std::vector<double> ComputeShiftedGeoTransform(GDALDataset* dataset,
                                               float row_shift,
                                               float col_shift);

GeoReference ComputeShiftedGeoReference(const std::string& input_path,
                                        float row_shift, float col_shift);

}  // namespace gdal
}  // namespace sirius

#endif  // SIRIUS_GDAL_WRAPPER_H_
