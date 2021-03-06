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

#ifndef SIRIUS_TYPES_H_
#define SIRIUS_TYPES_H_

#include <cmath>

#include <array>
#include <vector>

namespace sirius {

using Buffer = std::vector<double>;

/**
 * \brief Data class that represents the size of an image
 */
struct Size {
    Size() = default;

    Size(int row, int col);
    Size(const std::array<int, 2>& size);

    ~Size() = default;
    Size(const Size&) = default;
    Size& operator=(const Size&) = default;
    Size(Size&&) = default;
    Size& operator=(Size&&) = default;

    inline bool operator<(const Size& rhs) const {
        return (row < rhs.row) || ((row == rhs.row) && (col < rhs.col));
    }

    inline bool operator==(const Size& rhs) const {
        return row == rhs.row && col == rhs.col;
    }

    inline Size operator*(int scale) const {
        Size result(*this);
        result.row *= scale;
        result.col *= scale;
        return result;
    }

    inline Size operator*(double scale) const {
        Size result(*this);
        result.row = std::ceil(result.row * scale);
        result.col = std::ceil(result.col * scale);
        return result;
    }

    inline Size& operator*=(int scale) {
        *this = *this * scale;
        return *this;
    }

    inline int CellCount() const { return row * col; }

    int row{0};
    int col{0};
};

/**
 * \brief Data class that represents zoom ratio as
 *        input_resolution/output_resolution
 */
class ZoomRatio {
  public:
    /**
     * \brief Instantiate a zoom ratio 1/1
     */
    ZoomRatio() = default;

    /**
     * \brief Zoom ratio as input_resolution/output_resolution
     *        Reduce the ratio
     * \param input_resolution numerator of the ratio
     * \param output_resolution denominator of the ratio
     *
     * \throw SiriusException if input or output resolution is invalid
     */
    ZoomRatio(int input_resolution, int output_resolution);

    ~ZoomRatio() = default;
    ZoomRatio(const ZoomRatio&) = default;
    ZoomRatio(ZoomRatio&&) = default;
    ZoomRatio& operator=(const ZoomRatio&) = default;
    ZoomRatio& operator=(ZoomRatio&&) = default;

    inline int input_resolution() const { return input_resolution_; }

    inline int output_resolution() const { return output_resolution_; }

    inline double ratio() const {
        return input_resolution_ / static_cast<double>(output_resolution_);
    }

    bool IsRealZoom() const;

  private:
    void ReduceRatio();

  private:
    int input_resolution_{1};
    int output_resolution_{1};
};

}  // namespace sirius

#endif  // SIRIUS_TYPES_H_
