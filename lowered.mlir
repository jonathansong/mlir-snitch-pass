module {
  func.func private @snitch_hw_vector_add(memref<10xf32>, memref<10xf32>, memref<10xf32>, index)
  func.func @add_memrefs(%arg0: memref<10xf32>, %arg1: memref<10xf32>, %arg2: memref<10xf32>) {
    %c10 = arith.constant 10 : index
    call @snitch_hw_vector_add(%arg0, %arg1, %arg2, %c10) : (memref<10xf32>, memref<10xf32>, memref<10xf32>, index) -> ()
    return
  }
}

