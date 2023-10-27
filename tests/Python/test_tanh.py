# RUN: %PYTHON %s 2>&1 | FileCheck %s

import torch
import torch._dynamo as dynamo
from torch._inductor.decomposition import decompositions as inductor_decomp

from buddy.compiler.frontend import DynamoCompiler
from buddy.compiler.ops import tosa


def foo(x):
    return torch.ops.aten.tanh(x)


x = torch.randn(10)

# Initialize the dynamo compiler.
dynamo_compiler = DynamoCompiler(
    primary_registry=tosa.ops_registry,
    aot_autograd_decomposition=inductor_decomp,
)

foo_mlir = dynamo.optimize(dynamo_compiler)(foo)
foo_mlir(x)

# CHECK: module {
# CHECK-LABEL: func.func @forward
# CHECK: %{{.*}} = "tosa.tanh"
# CHECK: return %{{.*}}
# CHECK: }
# CHECK: }
print(dynamo_compiler.imported_module)
