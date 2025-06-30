"builtin.module"() ({
 
  func.func @add_memrefs(%arg0: memref<10xf32>, %arg1: memref<10xf32>, %result: memref<10xf32>) {
    linalg.generic {
        indexing_maps = [
          affine_map<(d0) -> (d0)>,
          affine_map<(d0) -> (d0)>,
          affine_map<(d0) -> (d0)>
        ],
        iterator_types = ["parallel"]
      }
      ins(%arg0, %arg1 : memref<10xf32>, memref<10xf32>)
      outs(%result : memref<10xf32>) {
        ^bb0(%a: f32, %b: f32, %c: f32):
          %sum = arith.addf %a, %b : f32
          linalg.yield %sum : f32
      }
    return
  }

}) : () -> ()
