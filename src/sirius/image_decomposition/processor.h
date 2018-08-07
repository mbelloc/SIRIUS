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

#ifndef SIRIUS_IMAGE_DECOMPOSITION_TYPES_H_
#define SIRIUS_IMAGE_DECOMPOSITION_TYPES_H_

namespace sirius {
namespace image_decomposition {

class IInterpolator2D {
  public:
    using UPtr = std::unique_ptr<IInterpolator2D>;

  public:
    virtual Image Interpolate2D(const Image& image) = 0;
};

class IImageProcessor {
  public:
    using UPtr = std::unique_ptr<IImageProcessor>;

  public:
    virtual Image Process(const Image& image) = 0;
};

}  // namespace image_decomposition
}  // namespace sirius

#endif  // SIRIUS_IMAGE_DECOMPOSITION_TYPES_H_