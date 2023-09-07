//===- ImageContainer.h ---------------------------------------------------===//
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//===----------------------------------------------------------------------===//
//
// Image container descriptor.
//
//===----------------------------------------------------------------------===//

#ifndef FRONTEND_INTERFACES_BUDDY_DIP_IMAGECONTAINER
#define FRONTEND_INTERFACES_BUDDY_DIP_IMAGECONTAINER

#include "buddy/Core/Container.h"
#include "buddy/DIP/imgcodecs/replenishment.h"
#include <cassert>
#include <opencv2/opencv.hpp>

using namespace dip;
// Image container.
// - T represents the type of the elements.
// - N represents the number of dimensions.
template <typename T, size_t N> class Img : public MemRef<T, N> {
public:
  Img();

  /**
   * @brief overload
   * @param sizes Array of integers specifying an n-dimensional array shape.
   * @param data Pointer to the user data.
   * they just initialize the matrix header that points to the specified data.
   */
  Img(intptr_t *sizes, T *data);

  /**
   * @brief overload
   * @param sizes Array of integers specifying an n-dimensional array shape.
   */
  Img(intptr_t *sizes);

  /**
   * @brief overload
   * @param m Array that (as a whole or partly) is assigned to the constructed
   * matrix.
   */
  Img(const Img<T, N> &m);

  /**
   * @brief assignment operators
   * @param m Assigned, right-hand-side matrix.
   * matrix.
   */
  Img &operator=(const Img<T, N> &m);

  // Move constructor.
  Img(Img<T, N> &&m);

  // Move assignment operator.
  Img &operator=(Img<T, N> &&m);

  /**
   * @brief Load image data from OpenCV Mat.
   * @param image represents the OpenCV Mat object.
   * @param norm indicates whether to perform.
   */
  Img(cv::Mat image, intptr_t sizes[N] = nullptr, bool norm = false);

  /**
   * @brief overload
   * @param sizes Array of integers specifying an n-dimensional array shape.
   */
  void create(intptr_t *sizes);

  int channels();
};

// Image Constructor from Img.
template <typename T, size_t N> Img<T, N>::Img() : MemRef<T, N>() {}

/**
 * @brief overload
 * @param sizes Array of integers specifying an n-dimensional array shape.
 */
template <typename T, size_t N>
Img<T, N>::Img(intptr_t *sizes) : MemRef<T, N>() {
  create(sizes);
}

/**
 * @brief overload
 * @param m Array that (as a whole or partly) is assigned to the constructed
 * matrix.
 */
template <typename T, size_t N>
Img<T, N>::Img(const Img<T, N> &m) : MemRef<T, N>() {
  for (size_t i = 0; i < N; i++) {
    this->sizes[i] = m.sizes[i];
    this->strides[i] = m.strides[i];
  }
  this->size = m.size;
  this->allocated = new T[this->size];
  this->aligned = this->allocated;
  for (size_t i = 0; i < this->size; i++) {
    this->aligned[i] = m.aligned[i];
  }
}

// Move Constructor.
// This constructor is used to initialize a MemRef object from a rvalue.
// The move constructor steals the resources of the original object.
// Note that the original object no longer owns the members and spaces.
// Steal members from the original object.
// Assign the NULL pointer to the original aligned and allocated members to
// avoid the double free error.
template <typename T, size_t N> Img<T, N>::Img(Img<T, N> &&m) : MemRef<T, N>() {
  this->aligned = m.aligned;
  this->allocated = m.allocated;
  this->size = m.size;
  std::swap(this->sizes, m.sizes);
  std::swap(this->strides, m.strides);
  // Assign the NULL pointer to the original aligned and allocated members to
  // avoid the double free error.
  m.allocated = m.aligned = nullptr;
}

// Move Assignment Operator.
// Note that the original object no longer owns the members and spaces.
// Check if they are the same object.
// Free the data space of this object to avoid memory leaks.
// Steal members from the original object.
// Assign the NULL pointer to the original aligned and allocated members to
// avoid the double free error.
template <typename T, size_t N> Img<T, N> &Img<T, N>::operator=(Img<T, N> &&m) {
  if (this != &m) {
    // Free the original aligned and allocated space.
    delete[] this->allocated;
    // Steal members of the original object.
    std::swap(this->size, m.size);
    std::swap(this->allocated, m.allocated);
    std::swap(this->aligned, m.aligned);
    std::swap(this->sizes, m.sizes);
    std::swap(this->strides, m.strides);
    // Assign the NULL pointer to the original aligned and allocated members to
    // avoid the double free error.
    m.allocated = m.aligned = nullptr;
  }
  return *this;
}

/**
 * @brief overload
 * @param sizes Array of integers specifying an n-dimensional array shape.
 */
template <typename T, size_t N> void Img<T, N>::create(intptr_t *sizes) {
  for (size_t i = 0; i < N; i++) {
    this->sizes[i] = sizes[i];
  }
  this->setStrides();
  this->size = this->product(this->sizes);
  if (this->size > 0) {
    this->allocated = new T[this->size];
    this->aligned = this->allocated;
  }
}

/**
 * @brief assignment operators
 * @param m Assigned, right-hand-side matrix.
 * matrix.
 */
template <typename T, size_t N>
Img<T, N> &Img<T, N>::operator=(const Img<T, N> &m) {
  if (this == &m) {
    return *this;
  } else {
    for (size_t i = 0; i < N; i++) {
      this->sizes[i] = m.sizes[i];
      this->strides[i] = m.strides[i];
    }
    this->size = m.size;
    // Allocate new space and deep copy.
    T *ptr = new T[this->size];
    for (size_t i = 0; i < this->size; i++) {
      ptr[i] = m.aligned[i];
    }
    this->allocated = ptr;
    this->aligned = ptr;
  }
  return *this;
}

/**
 * @brief overload
 * @param sizes Array of integers specifying an n-dimensional array shape.
 * @param data Pointer to the user data.
 * they just initialize the matrix header that points to the specified data.
 */
template <typename T, size_t N>
Img<T, N>::Img(intptr_t *sizes, T *data) : MemRef<T, N>() {
  for (size_t i = 0; i < N; i++) {
    this->sizes[i] = sizes[i];
  }
  this->size = this->product(this->sizes);
  this->setStrides();
  this->aligned = data;
}

// Image Constructor from OpenCV Mat.
template <typename T, size_t N>
Img<T, N>::Img(cv::Mat image, intptr_t sizes[N], bool norm) : MemRef<T, N>() {
  if (image.channels() == 1) {
    assert((N == 2) && "For gray images, the number of dimensions must be 2.");
  } else if (image.channels() == 3) {
    assert((N == 4) && "For RGB images, the number of dimensions must be 4, "
                       "either in NHWC or NCHW layout.");
  } else {
    std::cerr << "Only 2-channel gray images and 3-channel RGB images are "
                 "supported, but got images' channel equal to "
              << image.channels() << "." << std::endl;
  }
  // Use default layout setting.
  if (sizes == nullptr) {
    // The size of the gray image is represented by height and width by default.
    if (N == 2) {
      this->sizes[0] = image.rows;
      this->sizes[1] = image.cols;
    }
    this->size = this->product(this->sizes);
    this->setStrides();
    this->allocated = new T[this->size];
    this->aligned = this->allocated;
    // Load gray image data from OpenCV Mat.
    if (N == 2) {
      size_t k = 0;
      for (int i = 0; i < this->sizes[0]; i++) {
        for (int j = 0; j < this->sizes[1]; j++) {
          if (norm) {
            this->aligned[k] = (T)image.at<uchar>(i, j) / 255;
          } else {
            this->aligned[k] = (T)image.at<uchar>(i, j);
          }
          k++;
        }
      }
    }
  }
}

template <typename T, size_t N> int Img<T, N>::channels() {
  if (N == 2) {
    return 1;
  }
  return this->sizes[2];
}

#endif // FRONTEND_INTERFACES_BUDDY_DIP_IMAGECONTAINER
