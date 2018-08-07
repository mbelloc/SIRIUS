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

#ifndef SIRIUS_I_FREQUENCY_TRANSLATION_H_
#define SIRIUS_I_FREQUENCY_TRANSLATION_H_

#include <memory>
#include <string>
#include <vector>

#include "sirius/filter.h"
#include "sirius/image.h"
#include "sirius/types.h"

namespace sirius {

/**
 * \class IFrequencyTranslation
 * \brief Interface that frequency translation should implement
 */
class IFrequencyTranslation {
  public:
    using UPtr = std::unique_ptr<IFrequencyTranslation>;

  public:
    virtual ~IFrequencyTranslation() = default;

    /**
     * \brief Shift an image
     *
     * \remark This method is thread safe
     *
     * \param input image to shift
     * \return Shifted image
     *
     * \throw SiriusException if a computing issue happens
     */
    virtual Image Compute(const Image& input) const = 0;
};

}  // namespace sirius

#endif  // SIRIUS_I_FREQUENCY_TRANSLATION_H_
