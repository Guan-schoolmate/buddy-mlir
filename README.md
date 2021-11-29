# BUDDY MLIR

An MLIR-Based Ideas Landing Project.

## Getting Started

### LLVM/MLIR Dependencies

This project uses LLVM/MLIR as an external library. Please make sure [the dependencies](https://mlir.llvm.org/getting_started/) are available
on your machine.

### Clone and Initialize


```
$ git clone git@github.com:buddy-compiler/buddy-mlir.git
$ cd buddy-mlir
$ git submodule update --init
```

### Build and Test LLVM/MLIR

```
$ cd buddy-mlir
$ mkdir llvm/build
$ cd llvm/build
$ cmake -G Ninja ../llvm \
    -DLLVM_ENABLE_PROJECTS="mlir" \
    -DLLVM_TARGETS_TO_BUILD="host" \
    -DLLVM_ENABLE_ASSERTIONS=ON \
    -DCMAKE_BUILD_TYPE=RELEASE
$ ninja
$ ninja check-mlir
```

### Build buddy-mlir

```
$ cd buddy-mlir
$ mkdir build
$ cd build
$ cmake -G Ninja .. \
    -DMLIR_DIR=$PWD/../llvm/build/lib/cmake/mlir \
    -DLLVM_DIR=$PWD/../llvm/build/lib/cmake/llvm \
    -DLLVM_ENABLE_ASSERTIONS=ON \
    -DCMAKE_BUILD_TYPE=RELEASE
$ ninja
```

## Dialects

### Bud Dialect

Bud dialect is designed for testing and demonstrating.

### DIP Dialect

DIP dialect is designed for digital image processing abstraction.

## Tools

### buddy-opt

The buddy-opt is the driver for dialects and optimization in buddy-mlir project. 

**Convolution Optimization**

So far, we provide the 2D convolution vectorization pass `conv-vectorization`. The pass implements the Coefficients Broadcasting algorithm with Strip Mining strategy, and the strip mining size is configurable. Take the size of 256 as an example, you can use the tool with the following configuration.

```
$ buddy-opt <input> -conv-vectorization="strip-mining=256"
```

- Conversion example

We provide a function with `linalg.conv_2d` operation. You can use the following commands to print the conversion result.

```
$ cd buddy-mlir/build/bin
$ ./buddy-opt ../../examples/ConvOpt/conv2d.mlir -conv-vectorization="strip-mining=256"
```

- Edge detection example

We also provide an edge detection example to show the optimization.
The `conv-vectorization` pass is responsible for lowering the `linalg.conv_2d` with our algorithm.
And then we use `mlir-translate` and `llc` tools to generate the object file.
At last, we call the MLIR convolution function in a C++ program.

Please use a Linux machine with OpenCV installed to play around.
You should specify the `<strip mining size>` (e.g. 256) and `<ISA vector extension>` (e.g. avx512f).

```
$ cd buddy-mlir/build
$ cmake -G Ninja .. \
    -DBUDDY_EXAMPLES=ON \
    -DBUDDY_CONV_OPT_STRIP_MINING=<strip mining size> \
    -DBUDDY_CONV_OPT_ATTR=<ISA vector extension>
$ ninja edge-detection
```

We provide an image at `buddy-mlir/examples/ConvOpt/images/YuTu.png`, which is the robotic lunar rover that formed part of the Chinese Chang'e 3 mission.
You can detect the edge of the image with `edge-detection`.

```
$ cd bin
$ ./edge-detection ../../examples/ConvOpt/images/YuTu.png result.png
```

Note: In the edge detection example, images size needs to be an integer multiple of the strip mining size.

We also provide the performance comparison between our conv-opt tool and other state-of-the-art approaches. 
For more details, please see [convolution comparison](./examples/ConvOpt/comparison/README.md).

**Lowering DIP Dialect**

```
$ buddy-opt <input> -lower-dip
```

- Conversion example:

```
$ cd buddy-mlir/build/bin
$ ./buddy-opt ../../examples/DIPDialect/corr2d.mlir --lower-dip
```

- Edge detection example:

Build and run the example.

*Note: Please use a Linux machine with OpenCV installed to play around.*

```
$ cd buddy-mlir/build
$ cmake -G Ninja .. -DBUDDY_EXAMPLES=ON
$ ninja correlation2D
$ cd bin
$ ./correlation2D ../../examples/ConvOpt/images/YuTu.png result-dip.png
```

**Lowering Bud Dialect**

```
$ buddy-opt <input> -lower-bud
```

Example:

```
$ cd buddy-mlir/build/bin
$ ./buddy-opt ../../examples/BudDialect/TestConstant.mlir --lower-bud
```

*NOTE: The benchmark directory is out-of-date. Please see [buddy-benchmark](https://github.com/buddy-compiler/buddy-benchmark) for the current state.*
