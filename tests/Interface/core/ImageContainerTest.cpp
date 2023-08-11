//===- ImageContainerTest.cpp ---------------------------------------------===//
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
// This is the image container test file.
//
//===----------------------------------------------------------------------===//

// RUN: buddy-image-container-test 2>&1 | FileCheck %s
#include <buddy/Core/Container.h>
#include <buddy/DIP/ImageContainer.h>
#include<buddy/DIP/imgcodecs/loadsave.hpp>
int main() {

  Img<float, 2> grayimage=imread<float,2>("../../../../tests/Interface/core/TestGrayImage_8.bmp", IMREAD_GRAYSCALE);
  //===--------------------------------------------------------------------===//
  // Test copy constructor.
  //===--------------------------------------------------------------------===//
  
  Img<float, 2> testOpenCVConstructor(grayimage);
  // CHECK: 15.0
  fprintf(stderr, "%f\n", testOpenCVConstructor.getData()[0]);
  // CHECK: 4, 4
  fprintf(stderr, "%ld, %ld\n", testOpenCVConstructor.getSizes()[0],
          testOpenCVConstructor.getSizes()[1]);
  // CHECK: 4, 1
  fprintf(stderr, "%ld, %ld\n", testOpenCVConstructor.getStrides()[0],
          testOpenCVConstructor.getStrides()[1]);
  // CHECK: 2
  fprintf(stderr, "%ld\n", testOpenCVConstructor.getRank());
  // CHECK: 16
  fprintf(stderr, "%ld\n", testOpenCVConstructor.getSize());
  // CHECK: 60.0
  fprintf(stderr, "%f\n", testOpenCVConstructor[3]);

  //===--------------------------------------------------------------------===//
  // Test copy constructor.
  //===--------------------------------------------------------------------===//
  // TODO: Add copy assignment operator test.
  Img<float, 2> testCopyConstructor1(testOpenCVConstructor);
  // CHECK: 15.0
  fprintf(stderr, "%f\n", testCopyConstructor1[0]);
  Img<float, 2> testCopyConstructor2 = testOpenCVConstructor;
  // CHECK: 15.0
  fprintf(stderr, "%f\n", testCopyConstructor2[0]);
  Img<float, 2> testCopyConstructor3 = Img<float, 2>(testOpenCVConstructor);
  // CHECK: 15.0
  fprintf(stderr, "%f\n", testCopyConstructor3[0]);
  Img<float, 2> *testCopyConstructor4 =
      new Img<float, 2>(testOpenCVConstructor);
  // CHECK: 5.0
  fprintf(stderr, "%f\n", testCopyConstructor4->getData()[0]);
  delete testCopyConstructor4;

  //===--------------------------------------------------------------------===//
  // Test move constructor.
  //===--------------------------------------------------------------------===//
  // TODO: Add copy assignment operator test.
  Img<float, 2> testMoveConstructor1(std::move(testCopyConstructor1));
  // CHECK: 15.0
  fprintf(stderr, "%f\n", testMoveConstructor1[0]);
  Img<float, 2> testMoveConstructor2 = std::move(testMoveConstructor1);
  // CHECK: 15.0
  fprintf(stderr, "%f\n", testMoveConstructor2[0]);

  //===--------------------------------------------------------------------===//
  // Test overloading bracket operator.
  //===--------------------------------------------------------------------===//
  Img<float, 2> testBracketOperator1(grayimage);
  // CHECK: 240.0
  fprintf(stderr, "%f\n", testBracketOperator1[15]);
  testBracketOperator1[15] = 90.0;
  // CHECK: 90.0
  fprintf(stderr, "%f\n", testBracketOperator1[15]);
  const Img<float, 2> testBracketOperator2(grayimage);
  // CHECK: 240.0
  fprintf(stderr, "%f\n", testBracketOperator2[15]);
}
