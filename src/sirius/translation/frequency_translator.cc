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

#include "sirius/translation/frequency_translator.h"

#include <cstring>

#include "sirius/exception.h"

#include "sirius/utils/gsl.h"
#include "sirius/utils/log.h"
#include "sirius/utils/numeric.h"

#include "sirius/fftw/wrapper.h"

namespace sirius {
namespace translation {

/**
  \brief remove borders according to given shifts.
  \param row_shift shift on y axis
  \param col_shift shift on x axis
  \return cropped image
 */
Image RemoveBorders(const Image& image, int row_shift, int col_shift);

FrequencyTranslation::FrequencyTranslation(float row_shift, float col_shift)
    : row_shift_(row_shift), col_shift_(col_shift) {}

Image FrequencyTranslation::Process(const Image& image) {
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

    // LOG("sirius", trace, "Translation x:{}, y:{}", col_shift_, row_shift_);

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

TranslationInterpolator2D::TranslationInterpolator2D(float row_shift,
                                                     float col_shift)
    : row_shift_(row_shift), col_shift_(col_shift) {}

Image TranslationInterpolator2D::Interpolate2D(const Image& image) {
    LOG("interpolated2D", trace, "input image size {}x{}", image.size.row,
        image.size.col);
    Image interpolated_im(image.size);
    interpolated_im = image;

    float beta = row_shift_ - static_cast<int>(row_shift_);
    float alpha = col_shift_ - static_cast<int>(col_shift_);
    LOG("TranslationInterpolator2D", trace, "alpha = {}, beta={}", alpha, beta);
    std::vector<double> BLN_kernel(4, 0);
    BLN_kernel[0] = (1 - alpha) * (1 - beta);
    BLN_kernel[1] = (1 - beta) * alpha;
    BLN_kernel[2] = beta * (1 - alpha);
    BLN_kernel[3] = alpha * beta;

    Size img_mirror_size(image.size.row + 1, image.size.col + 1);

    std::vector<double> img_mirror(img_mirror_size.CellCount(), 0);
    auto img_mirror_span = gsl::as_multi_span(img_mirror);
    for (int i = 0; i < image.size.row; i++) {
        for (int j = 0; j < image.size.col; j++) {
            img_mirror_span[i * (image.size.col + 1) + j] = image.Get(i, j);
        }
    }

    // duplicate last row
    for (int j = 0; j < image.size.col + 1; j++) {
        img_mirror_span[image.size.row * (image.size.col + 1) + j] =
              img_mirror_span[(image.size.row - 1) * (image.size.col + 1) + j];
    }

    // duplicate last col
    for (int i = 0; i < image.size.row + 1; i++) {
        img_mirror_span[(i + 1) * (image.size.col + 1) - 1] =
              img_mirror_span[(i + 1) * (image.size.col + 1) - 2];
    }

    // convolve. BLN_kernel is already flipped
    for (int i = 0; i < image.size.row; ++i) {
        for (int j = 0; j < image.size.col; ++j) {
            interpolated_im.Set(
                  i, j,
                  img_mirror_span[i * img_mirror_size.col + j] * BLN_kernel[0] +
                        img_mirror_span[i * img_mirror_size.col + j + 1] *
                              BLN_kernel[1] +
                        img_mirror_span[(i + 1) * img_mirror_size.col + j] *
                              BLN_kernel[2] +
                        img_mirror_span[(i + 1) * img_mirror_size.col + j + 1] *
                              BLN_kernel[3]);
        }
    }

    // use "- shift" to remove borders that should have been replicated on the
    // opposite side of the translation if we had passed the entire shift
    return RemoveBorders(interpolated_im, std::ceil(-row_shift_),
                         std::ceil(-col_shift_));
    ;
}

Image RemoveBorders(const Image& image, int row_shift, int col_shift) {
    LOG("RemoveBorders", trace, "removed size {}x{}", row_shift, col_shift);
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

    int nb_elem = end_col - begin_col;
    auto src_it = image.data.begin() + (begin_row * image.size.col + begin_col);
    auto output_it = output_image.data.begin();
    for (int i = begin_row; i < end_row; ++i) {
        std::copy(src_it, src_it + nb_elem, output_it);
        src_it += image.size.col;
        output_it += nb_elem;
    }

    return output_image;
}

}  // namespace translation
}  // namespace sirius