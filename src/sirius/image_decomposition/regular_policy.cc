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

#include "sirius/image_decomposition/regular_policy.h"

#include "sirius/utils/log.h"

namespace sirius {
namespace image_decomposition {

RegularPolicy::RegularPolicy(IImageProcessor::UPtr image_processor,
                             IInterpolator2D::UPtr)
    : image_processor_(std::move(image_processor)) {}

Image RegularPolicy::DecomposeAndProcess(const Image& image) const {
    LOG("regular_decomposition", trace, "process image");
    return image_processor_->Process(image);
}

}  // namespace image_decomposition
}  // namespace sirius
