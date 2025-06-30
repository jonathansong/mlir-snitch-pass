#include "mlir/IR/PatternMatch.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Dialect/Linalg/IR/Linalg.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Transforms/GreedyPatternRewriteDriver.h"
#include "mlir/Tools/Plugins/PassPlugin.h"
#include "mlir/Pass/PassRegistry.h" 
#include "mlir/Pass/PassManager.h"
#include "mlir/Dialect/Tensor/IR/Tensor.h"


using namespace mlir;

namespace {
struct LowerLinalgVectorAddToHwPass
    : public PassWrapper<LowerLinalgVectorAddToHwPass, OperationPass<func::FuncOp>> {
  void runOnOperation() override {
    RewritePatternSet patterns(&getContext());
    patterns.add<VectorAddRewritePattern>(&getContext());
    if (failed(applyPatternsGreedily(getOperation(), std::move(patterns))))
      signalPassFailure();
  }

  StringRef getArgument() const final { return "lower-linalg-vector-add-to-hw"; }
  StringRef getDescription() const final {
    return "Lower linalg.generic vector.add to hardware call";
  }

  void getDependentDialects(mlir::DialectRegistry &registry) const override {
    registry.insert<mlir::linalg::LinalgDialect,
                    mlir::arith::ArithDialect,
                    mlir::memref::MemRefDialect,
                    mlir::tensor::TensorDialect,
                    mlir::func::FuncDialect>();
  }

  struct VectorAddRewritePattern : public RewritePattern {
    VectorAddRewritePattern(MLIRContext *ctx)
        : RewritePattern("linalg.generic", 1, ctx) {}

    LogicalResult matchAndRewrite(Operation *op, PatternRewriter &rewriter) const override {
      auto linalgOp = dyn_cast<linalg::GenericOp>(op);
      if (!linalgOp || linalgOp.getInputs().size() != 2 || linalgOp.getOutputs().size() != 1)
        return failure();

      if (linalgOp.getNumLoops() != 1 ||
          llvm::any_of(linalgOp.getIteratorTypesArray(), [](mlir::utils::IteratorType type) {
            return type != mlir::utils::IteratorType::parallel;
          }))
        return failure();

      Block &body = linalgOp.getRegion().front();
      if (!isa<arith::AddFOp>(body.getTerminator()->getOperand(0).getDefiningOp()))
        return failure();

      Location loc = op->getLoc();
      Value A = linalgOp.getInputs()[0];
      Value B = linalgOp.getInputs()[1];
      Value C = linalgOp.getOutputs()[0];

      Value zero = rewriter.create<arith::ConstantIndexOp>(loc, 0);
      Value N = rewriter.create<memref::DimOp>(loc, A, zero);

      // Insert hw function declaration if missing
      auto module = op->getParentOfType<ModuleOp>();
      if (!module.lookupSymbol<func::FuncOp>("snitch_hw_vector_add")) {
        OpBuilder::InsertionGuard g(rewriter);
        rewriter.setInsertionPointToStart(module.getBody());
        auto funcType = rewriter.getFunctionType({A.getType(), B.getType(), C.getType(), rewriter.getIndexType()}, {});
        rewriter.create<func::FuncOp>(loc, "snitch_hw_vector_add", funcType).setPrivate();
      }

      rewriter.replaceOpWithNewOp<func::CallOp>(
          op, SymbolRefAttr::get(rewriter.getContext(), "snitch_hw_vector_add"),
          TypeRange{}, ValueRange{A, B, C, N});
      return success();
    }
  };
};
} // namespace

// Plugin entry point
extern "C" ::mlir::PassPluginLibraryInfo mlirGetPassPluginInfo() {
  return {
    MLIR_PLUGIN_API_VERSION,
    "LowerLinalgVectorAddToHwPass",
    "v0.1",
    []() {
      ::mlir::registerPass([]() -> std::unique_ptr<::mlir::Pass> {
        return std::make_unique<LowerLinalgVectorAddToHwPass>();
      });
    }
  };
}


