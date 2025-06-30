builtin.module {
  func.func @add_tensors(%arg0: tensor<10xf32>, %arg1: tensor<10xf32>) -> tensor<10xf32> {
    %init = tensor.empty() : tensor<10xf32>
    %result = linalg.generic {
        indexing_maps = [
          affine_map<(d0) -> (d0)>,
          affine_map<(d0) -> (d0)>,
          affine_map<(d0) -> (d0)>
        ],
        iterator_types = ["parallel"]
      }
      ins(%arg0, %arg1 : tensor<10xf32>, tensor<10xf32>)
      outs(%init : tensor<10xf32>) {
        ^bb0(%a: f32, %b: f32, %c: f32):
          %sum = arith.addf %a, %b : f32
          linalg.yield %sum : f32
      } -> tensor<10xf32>

    return %result : tensor<10xf32>
  }
}
