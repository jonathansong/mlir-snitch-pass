# mlir-snitch-pass
lowering linalg.generic to a embedded hardware function "snitch_hw_vector_add", support memref, not tensor.
when working with tensor types, linalg.generic must produce a result (i.e., it must return a tensor value).

build libLinalgVectorAddPass.so by cmake

mlir-opt --load-pass-plugin=./libLinalgVectorAddPass.so --pass-pipeline="builtin.module(func.func(lower-linalg-vector-add-to-hw))" ../vector_add_memref.mlir -o ../lowered_memref.mlir
