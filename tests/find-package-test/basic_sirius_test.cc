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

#include <sirius/utils/log.h>

#include <sirius/frequency_resampler_factory.h>
#include <sirius/frequency_translator_factory.h>
#include <sirius/types.h>
#include "sirius/filter.h"
#include "sirius/image.h"

void Resample(const sirius::Image& image);
void Translate(const sirius::Image& image);

int main(int, char**) {
    LOG_SET_LEVEL(trace);

    auto dummy_image = sirius::Image({1000, 750});

    Resample(dummy_image);
    Translate(dummy_image);
}

void Resample(const sirius::Image& image) {
    LOG("basic_test", info, "resample image");

    auto zoom_ratio_2_1 = sirius::ZoomRatio::Create(2, 1);

    sirius::IFrequencyResampler::UPtr freq_resampler =
          sirius::FrequencyResamplerFactory::Create(
                sirius::image_decomposition::Policies::kRegular,
                sirius::FrequencyUpsamplingStrategies::kZeroPadding);

    sirius::resampling::Parameters resampling_params{zoom_ratio_2_1};
    sirius::Image zoomed_image_2_1 =
          freq_resampler->Compute(image, {}, resampling_params);
}

void Translate(const sirius::Image& image) {
    LOG("basic_test", info, "translate image");

    sirius::translation::Parameters translation_parameters{20.5, 19.5};
    sirius::IFrequencyTranslator::UPtr freq_translator =
          sirius::FrequencyTranslatorFactory::Create(
                sirius::image_decomposition::Policies::kPeriodicSmooth);

    sirius::Image translated_image =
          freq_translator->Compute(image, {}, translation_parameters);
}