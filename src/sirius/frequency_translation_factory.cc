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

#include "sirius/frequency_translation_factory.h"

#include "sirius/utils/log.h"

#include "sirius/frequency_translater.h"
#include "sirius/image_decomposition/periodic_smooth_policy.h"
#include "sirius/image_decomposition/regular_policy.h"

namespace sirius {

IFrequencyTranslation::UPtr FrequencyTranslationFactory::Create(
      ImageDecompositionPolicies image_decomposition, float shift_row,
      float shift_col) {
    using FrequencyTranslaterRegular =
          FrequencyTranslater<image_decomposition::RegularPolicy>;

    using FrequencyTranslaterPeriodicSmooth =
          FrequencyTranslater<image_decomposition::PeriodicSmoothPolicy>;

    switch (image_decomposition) {
        case ImageDecompositionPolicies::kRegular:
            return std::make_unique<FrequencyTranslaterRegular>(shift_row,
                                                                shift_col);
        case ImageDecompositionPolicies::kPeriodicSmooth:
            return std::make_unique<FrequencyTranslaterPeriodicSmooth>(
                  shift_row, shift_col);
            break;
        default:
            break;
    }

    LOG("frequency_translation_factory", warn, "image decomposition");
    return nullptr;
}

}  // namespace sirius
