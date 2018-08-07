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

#include <sstream>
#include <string>

#include <catch/catch.hpp>

#include "sirius/image.h"

#include "sirius/translation/frequency_translator.h"

#include "sirius/frequency_translater.h"
#include "sirius/image_decomposition/periodic_smooth_policy.h"
#include "sirius/image_decomposition/regular_policy.h"

#include "sirius/frequency_translation_factory.h"

#include "sirius/gdal/exception.h"
#include "sirius/gdal/wrapper.h"

#include "sirius/utils/log.h"

#include "utils.h"

TEST_CASE("frequency translater - periodic smooth", "[sirius]") {
    LOG_SET_LEVEL(trace);

    auto freq_trans_r = sirius::FrequencyTranslationFactory::Create(
          sirius::ImageDecompositionPolicies::kRegular, 50.0, 50.0);

    auto freq_trans_ps = sirius::FrequencyTranslationFactory::Create(
          sirius::ImageDecompositionPolicies::kPeriodicSmooth, 50.0, 50.0);

    // test input
    auto lena_image = sirius::gdal::LoadImage("./input/lena.jpg");

    // output
    sirius::Image output;

    SECTION("lena - positive translation on both axis") {
        REQUIRE_NOTHROW(output = freq_trans_r->Compute(lena_image));
        REQUIRE(output.data.size() > 0);
        REQUIRE(output.size.row == lena_image.size.row - 50.0);
        REQUIRE(output.size.col == lena_image.size.col - 50.0);

        LOG("tests", debug, "output size: {}, {}", output.size.row,
            output.size.col);
    }

    SECTION("lena - positive translation on both axis") {
        REQUIRE_NOTHROW(output = freq_trans_ps->Compute(lena_image));
        REQUIRE(output.data.size() > 0);
        REQUIRE(output.size.row == lena_image.size.row - 50.0);
        REQUIRE(output.size.col == lena_image.size.col - 50.0);

        LOG("tests", debug, "output size: {}, {}", output.size.row,
            output.size.col);
    }

    auto freq_trans = sirius::FrequencyTranslationFactory::Create(
          sirius::ImageDecompositionPolicies::kPeriodicSmooth, 0.0, 50.0);
    SECTION("lena - positive translation on x axis") {
        REQUIRE_NOTHROW(output = freq_trans->Compute(lena_image));
        REQUIRE(output.data.size() > 0);
        REQUIRE(output.size.row == lena_image.size.row);
        REQUIRE(output.size.col == lena_image.size.col - 50.0);
        REQUIRE(std::round(output.data[50]) == std::round(lena_image.data[50]));

        LOG("tests", debug, "output size: {}, {}", output.size.row,
            output.size.col);
    }

    freq_trans = sirius::FrequencyTranslationFactory::Create(
          sirius::ImageDecompositionPolicies::kPeriodicSmooth, 50.0, 0.0);
    SECTION("lena - positive translation on y axis") {
        REQUIRE_NOTHROW(output = freq_trans->Compute(lena_image));
        REQUIRE(output.data.size() > 0);
        REQUIRE(output.size.row == lena_image.size.row - 50.0);
        REQUIRE(output.size.col == lena_image.size.col);
        REQUIRE(std::round(output.data[50]) == std::round(lena_image.data[50]));

        LOG("tests", debug, "output size: {}, {}", output.size.row,
            output.size.col);
    }

    freq_trans = sirius::FrequencyTranslationFactory::Create(
          sirius::ImageDecompositionPolicies::kPeriodicSmooth, 0.0, -50.0);
    SECTION("lena - negative translation on x axis") {
        REQUIRE_NOTHROW(output = freq_trans->Compute(lena_image));
        REQUIRE(output.data.size() > 0);
        REQUIRE(output.size.row == lena_image.size.row);
        REQUIRE(output.size.col == lena_image.size.col - 50.0);
        REQUIRE(std::round(output.data[50]) ==
                std::round(lena_image.data[100]));

        LOG("tests", debug, "output size: {}, {}", output.size.row,
            output.size.col);
    }

    freq_trans = sirius::FrequencyTranslationFactory::Create(
          sirius::ImageDecompositionPolicies::kPeriodicSmooth, -50.0, 0.0);
    SECTION("lena - negative translation on y axis") {
        REQUIRE_NOTHROW(output = freq_trans->Compute(lena_image));
        REQUIRE(output.data.size() > 0);
        REQUIRE(output.size.row == lena_image.size.row - 50.0);
        REQUIRE(output.size.col == lena_image.size.col);
        REQUIRE(std::round(output.data[50]) ==
                std::round(lena_image.data[50 * lena_image.size.row + 50]));

        LOG("tests", debug, "output size: {}, {}", output.size.row,
            output.size.col);
    }

    freq_trans = sirius::FrequencyTranslationFactory::Create(
          sirius::ImageDecompositionPolicies::kPeriodicSmooth, -50.72, 50.72);
    SECTION("lena - sub pixel translation") {
        REQUIRE_NOTHROW(output = freq_trans->Compute(lena_image));
        REQUIRE(output.data.size() > 0);
        REQUIRE(output.size.row == lena_image.size.row - 51);
        REQUIRE(output.size.col == lena_image.size.col - 51);

        LOG("tests", debug, "output size: {}, {}", output.size.row,
            output.size.col);
    }

    freq_trans = sirius::FrequencyTranslationFactory::Create(
          sirius::ImageDecompositionPolicies::kPeriodicSmooth, 0.0, 0.0);
    SECTION("lena - no translation") {
        REQUIRE_NOTHROW(output = freq_trans->Compute(lena_image));
        REQUIRE(output.data.size() > 0);
        REQUIRE(output.size.row == lena_image.size.row);
        REQUIRE(output.size.col == lena_image.size.col);
        REQUIRE(std::round(output.data[500]) ==
                std::round(lena_image.data[500]));

        LOG("tests", debug, "output size: {}, {}", output.size.row,
            output.size.col);
    }
}