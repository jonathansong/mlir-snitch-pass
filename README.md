# mlir-snitch-pass
lowering linalg.generic to a embedded hardware function "snitch_hw_vector_add", support memref, not tensor.
when working with tensor types, linalg.generic must produce a result (i.e., it must return a tensor value).
1:
build LLVM:
cmake -G Ninja ../llvm \
  -DLLVM_ENABLE_PROJECTS="mlir" \
  -DLLVM_TARGETS_TO_BUILD="host" \
  -DLLVM_ENABLE_ASSERTIONS=ON \
  -DCMAKE_BUILD_TYPE=Release \
  -DLLVM_ENABLE_RTTI=ON \
  -DLLVM_ENABLE_PLUGINS=ON \
  -DBUILD_SHARED_LIBS=ON \
  -DMLIR_ENABLE_BINDINGS_PYTHON=ON \
  -DPython3_EXECUTABLE=$(which python)
2:
build libLinalgVectorAddPass.so by cmake

mlir-opt --load-pass-plugin=./libLinalgVectorAddPass.so --pass-pipeline="builtin.module(func.func(lower-linalg-vector-add-to-hw))" ../vector_add_memref.mlir -o ../lowered_memref.mlir
