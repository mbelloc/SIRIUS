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

#ifndef SIRIUS_IMAGE_DECOMPOSITION_PERIODIC_SMOOTH_POLICY_H_
#define SIRIUS_IMAGE_DECOMPOSITION_PERIODIC_SMOOTH_POLICY_H_

#include "sirius/processor.h"

namespace sirius {
namespace image_decomposition {

/**
 * \brief Implementation of <a
 * href="https://hal.archives-ouvertes.fr/file/index/docid/388020/filename/2009-11.pdf">Periodic
 * plus Smooth image decomposition</a>
 */
class PeriodicSmoothPolicy {
  public:
    PeriodicSmoothPolicy(IImageProcessor::UPtr image_processor,
                         IInterpolator2D::UPtr interpolator);

    Image DecomposeAndProcess(const Image& image) const;

  private:
    IImageProcessor::UPtr image_processor_;
    IInterpolator2D::UPtr interpolator_;
};

}  // namespace image_decomposition
}  // namespace sirius

#endif  // SIRIUS_IMAGE_DECOMPOSITION_PERIODIC_SMOOTH_POLICY_H_
