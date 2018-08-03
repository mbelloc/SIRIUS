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

#include "sirius/translation/frequency_translation.h"

#include <cstring>

#include "sirius/exception.h"

#include "sirius/utils/gsl.h"
#include "sirius/utils/log.h"
#include "sirius/utils/numeric.h"

#include "sirius/fftw/wrapper.h"

namespace sirius {

FrequencyTranslation::FrequencyTranslation(float row_shift, float col_shift)
    : row_shift_(row_shift), col_shift_(col_shift) {}

Image FrequencyTranslation::Shift(const Image& image) {
    if (row_shift_ >= image.size.row || col_shift_ >= image.size.col ||
        row_shift_ <= -image.size.row || col_shift_ <= -image.size.col) {
        LOG("sirius", warn,
            "Desired shift x:{}, y:{} is greater than image size, this shift "
            "will be applied instead x:{}, y:{}",
            col_shift_, row_shift_,
            col_shift_ / static_cast<float>(image.size.col),
            row_shift_ / static_cast<float>(image.size.row));
        row_shift_ /= static_cast<float>(image.size.row);
        col_shift_ /= static_cast<float>(image.size.col);
    }

    LOG("sirius", trace, "Translation x:{}, y:{}", col_shift_, row_shift_);

    float reduced_x_shift = col_shift_ - static_cast<int>(col_shift_);
    float reduced_y_shift = row_shift_ - static_cast<int>(row_shift_);

    Image output_image(image.size);

    // sub_pixel translation required
    if (reduced_x_shift != 0.0 || reduced_y_shift != 0.0) {
        Image shifted_image(image.size);

        utils::IFFTShift2D(image.data.data(), image.size,
                           shifted_image.data.data());

        auto fft_image =
              fftw::FFT(shifted_image.data.data(), shifted_image.size);
        int fft_row_count = shifted_image.size.row;
        int fft_col_count = shifted_image.size.col / 2 + 1;

        // get frequencies for which fft was calculated
        std::vector<double> freq_y =
              utils::ComputeFFTFreq(shifted_image.size.col);
        std::vector<double> freq_x =
              utils::ComputeFFTFreq(shifted_image.size.row, false);

        auto exp_cplx_y = fftw::CreateComplex({1, fft_col_count});
        auto exp_cplx_x = fftw::CreateComplex({fft_row_count, 1});

        // calculate 1D shift vectors
        for (int j = 0; j < fft_col_count; ++j) {
            exp_cplx_y.get()[j][0] =
                  cos(-2 * M_PI * reduced_x_shift * freq_y[j]);
            exp_cplx_y.get()[j][1] =
                  sin(-2 * M_PI * reduced_x_shift * freq_y[j]);
        }

        for (int i = 0; i < fft_row_count; ++i) {
            exp_cplx_x.get()[i][0] =
                  cos(-2 * M_PI * reduced_y_shift * freq_x[i]);
            exp_cplx_x.get()[i][1] =
                  sin(-2 * M_PI * reduced_y_shift * freq_x[i]);
        }

        auto fft_image_span =
              utils::MakeSmartPtrArraySpan(fft_image, image.size);
        // apply 2D shift on real and imaginary parts
        for (int i = 0; i < fft_row_count; ++i) {
            for (int j = 0; j < fft_col_count; ++j) {
                int idx = i * fft_col_count + j;
                auto tmp_real = fft_image_span[idx][0];
                auto tmp_im = fft_image_span[idx][1];
                fft_image_span[idx][0] =
                      fft_image_span[idx][0] * exp_cplx_x[i][0] *
                            exp_cplx_y[j][0] -
                      fft_image_span[idx][1] * exp_cplx_x[i][1] *
                            exp_cplx_y[j][0] -
                      fft_image_span[idx][1] * exp_cplx_x[i][0] *
                            exp_cplx_y[j][1] -
                      fft_image_span[idx][0] * exp_cplx_x[i][1] *
                            exp_cplx_y[j][1];

                fft_image_span[idx][1] =
                      tmp_real * exp_cplx_x[i][0] * exp_cplx_y[j][1] -
                      tmp_im * exp_cplx_x[i][1] * exp_cplx_y[j][1] +
                      tmp_im * exp_cplx_x[i][0] * exp_cplx_y[j][0] +
                      tmp_real * exp_cplx_x[i][1] * exp_cplx_y[j][0];
            }
        }

        shifted_image = fftw::IFFT(shifted_image.size, std::move(fft_image));

        utils::FFTShift2D(shifted_image.data.data(), output_image.size,
                          output_image.data.data());

        // normalize output image
        int pixel_count = output_image.CellCount();
        std::for_each(output_image.data.begin(), output_image.data.end(),
                      [pixel_count](double& pixel) { pixel /= pixel_count; });
    } else {
        output_image = image;
    }

    // truncate borders on the opposite side of where it should have been
    // duplicated if we had applied the complete shift
    output_image = RemoveBorders(output_image, static_cast<int>(-row_shift_),
                                 static_cast<int>(-col_shift_));

    if (reduced_x_shift < 0) {
        reduced_x_shift = std::floor(reduced_x_shift);
    } else {
        reduced_x_shift = std::ceil(reduced_x_shift);
    }

    if (reduced_y_shift < 0) {
        reduced_y_shift = std::floor(reduced_y_shift);
    } else {
        reduced_y_shift = std::ceil(reduced_y_shift);
    }
    // remove, at max, 1 row and col (sub pixel shift)
    return RemoveBorders(output_image, reduced_y_shift, reduced_x_shift);
}

Image FrequencyTranslation::RemoveBorders(const Image& image, int row_shift,
                                          int col_shift) {
    LOG("FrequencyTranslation", trace, "Remove borders x:{}, y:{}", col_shift,
        row_shift);

    if (col_shift == 0 && row_shift == 0) {
        return image;
    }

    Image output_image(
          {image.size.row - abs(row_shift), image.size.col - abs(col_shift)});

    int begin_row, begin_col, end_row, end_col;
    if (row_shift > 0) {
        begin_row = row_shift;
        end_row = image.size.row;
    } else {
        begin_row = 0;
        end_row = image.size.row + row_shift;
    }

    if (col_shift > 0) {
        begin_col = col_shift;
        end_col = image.size.col;
    } else {
        begin_col = 0;
        end_col = image.size.col + col_shift;
    }

    int begin_src = begin_row * image.size.col + begin_col;
    int nb_elem = end_col - begin_col;
    int begin = 0;
    for (int i = begin_row; i < end_row; ++i) {
        memcpy(&output_image.data[begin], &image.data[begin_src],
               nb_elem * sizeof(double));
        begin_src += image.size.col;
        begin += nb_elem;
    }

    return output_image;
}

}  // end namespace sirius