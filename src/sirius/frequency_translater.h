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

#ifndef SIRIUS_FREQUENCY_TRANSLATER_H_
#define SIRIUS_FREQUENCY_TRANSLATER_H_

#include "sirius/i_frequency_translator.h"
#include "sirius/translation/frequency_translator.h"

namespace sirius {

/**
 * \brief Implementation of IFrequencyTranslator
 */
template <class ImageDecompositionPolicy>
class FrequencyTranslater : public IFrequencyTranslation {
  public:
    FrequencyTranslater(float shift_row, float shift_col)
        : image_decomposition_(
                std::make_unique<translation::FrequencyTranslation>(shift_row,
                                                                    shift_col),
                std::make_unique<translation::TranslationInterpolator2D>(
                      shift_row, shift_col)) {}

    ~FrequencyTranslater() = default;

    // copyable
    FrequencyTranslater(const FrequencyTranslater&) = default;
    FrequencyTranslater& operator=(const FrequencyTranslater&) = default;
    // moveable
    FrequencyTranslater(FrequencyTranslater&&) = default;
    FrequencyTranslater& operator=(FrequencyTranslater&&) = default;

    // IFrequencyResampler interface
    Image Compute(const Image& input) const override {
        return image_decomposition_.DecomposeAndProcess(input);
    }

  private:
    ImageDecompositionPolicy image_decomposition_;
};

}  // namespace sirius

#endif  // SIRIUS_FREQUENCY_TRANSLATER_H_
