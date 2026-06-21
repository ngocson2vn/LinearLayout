#include <bitset>

#include "mlir/IR/AsmState.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/BuiltinAttributes.h"

#include "LinearLayout.h"

using namespace mlir;
using namespace mlir::triton;

namespace tt = mlir::triton;

LinearLayout buildRegLayout(MLIRContext* ctx) {
  auto reg = StringAttr::get(ctx, "register");
  auto dim1 = StringAttr::get(ctx, "dim1");
  auto dim0 = StringAttr::get(ctx, "dim0");

  auto inner = LinearLayout::identity1D(2, reg, dim1);
  auto outer = LinearLayout::identity1D(2, reg, dim0);
  auto regLayout = inner * outer;

  return regLayout;
}

LinearLayout buildThreadLayout(MLIRContext* ctx) {
  auto thread = StringAttr::get(ctx, "thread");
  auto dim1 = StringAttr::get(ctx, "dim1");
  auto dim0 = StringAttr::get(ctx, "dim0");

  // Bits 0, 1, 2 of thread point to bits 0, 1, 2 of dim1 (j)
  // Bits 3, 4 of thread point to bits 0, 1 of dim0 (i)

  // Map 8 (3 bits) thread IDs: [0, 7] to [0, 7] dim1
  // Layout matrix is identity matrix I_3
  auto inner = LinearLayout::identity1D(8, thread, dim1);

  // Map 4 (2 bits) thread IDs: [0, 3] to [0, 3] dim0
  // Layout matrix is identity matrix I_2
  auto outer = LinearLayout::identity1D(4, thread, dim0);

  // Mathematically, 
  //   inner: F_2^3 -> F_2^3
  //   outer: F_2^2 -> F_2^2
  //   inner X outer: F_2^3 X F_2^2 -> F_2^3 X F_2^2
  //   M1 X M2 =  [M1 0
  //               0 M2]
  // 
  // operator* produces layout matrix I_5
  // because product of 2 layouts is represented by
  // the direct sum of 2 layout matrices
  auto threadLayout = inner * outer;

  return threadLayout;
}

LinearLayout buildLayoutA(MLIRContext* ctx) {
  // For Layout A from the paper (16x16 tensor):
  // - Logical dimensions: "dim0" (i), "dim1" (j)
  // - Physical dimensions: "register", "thread", "warp"

  // 1. Registers (2x2 registers)
  // Bit 0 of register points to bit 0 of dim1 (j)
  // Bit 1 of register points to bit 0 of dim0 (i)
  auto reg = StringAttr::get(ctx, "register");
  auto thread = StringAttr::get(ctx, "thread");
  auto warp = StringAttr::get(ctx, "warp");
  auto dim1 = StringAttr::get(ctx, "dim1");
  auto dim0 = StringAttr::get(ctx, "dim0");
  LinearLayout regLhs = LinearLayout::identity1D(2, reg, dim1);
  // llvm::outs() << "regLhs: " << regLhs << "\n\n";

  LinearLayout regRhs = LinearLayout::identity1D(2, reg, dim0);
  // llvm::outs() << "regRhs: " << regRhs << "\n\n";

  LinearLayout regLayout = regLhs * regRhs;
  // llvm::outs() << "regLayout: " << regLayout << "\n\n";

  // 2. Threads (4x8 threads)
  // Bits 0..2 of thread point to bits 1..3 of dim1 (j)
  // Bits 3..4 of thread point to bits 1..2 of dim0 (i)
  LinearLayout threadLayout = LinearLayout::identity1D(8, thread, dim1) * LinearLayout::identity1D(4, thread, dim0);

  // 3. Warps (2x1 warps)
  // Bit 0 of warp points to bit 3 of dim0 (i)
  LinearLayout warpLayout = LinearLayout::identity1D(2, warp, dim0);

  // 4. Combine them using the Product operation (operator*)
  // Multiplying layouts automatically stacks and concatenates their output bases.
  // For dim1: 1 bit (Reg) + 3 bits (Thread) = 4 bits (size 16)
  // For dim0: 1 bit (Reg) + 2 bits (Thread) + 1 bit (Warp) = 4 bits (size 16)
  LinearLayout layoutA = regLayout * threadLayout * warpLayout;

  return layoutA;
}

// A helper function to simulate the F2 matrix-vector multiplication
// v = l_W|l_T|l_R
// w = i|j = Aw = v0C0 ^ v1C1 ^ v2C2 ^ v3C3 ^ v4C4 ^ v5C5 ^ v6C6 ^ v7C7
// j = v0C0 ^ v2C2 ^ v3C3 ^ v4C4
// i = v1C1 ^ v5C5 ^ v6C6 ^ v7C7

std::pair<int, int> applyLayoutA(MLIRContext* ctx, LinearLayout& layoutA, int v) {
  // Linear index of Register
  int v_reg    = v & 0b00000011; // Take 2 bits, its value is in [0, 1, 2, 3]

  // Linear index of Thread.
  // Must truncate 2 bits of Register to get the correct linear index of Thread
  int v_thread = (v & 0b01111100) >> 2; 

  // Linear index of warp
  // Must truncate 2 bits of Register and 5 bits of Thread
  int v_warp   = (v & 0b10000000) >> 7;

  int w_dim1 = 0; // This will hold our column (j)
  int w_dim0 = 0; // This will hold our row (i)

  auto reg = StringAttr::get(ctx, "register");
  auto thread = StringAttr::get(ctx, "thread");
  auto warp = StringAttr::get(ctx, "warp");
  auto dim1 = StringAttr::get(ctx, "dim1");
  auto dim0 = StringAttr::get(ctx, "dim0");

  // 1. Process Register bits
  for (int bit = 0; bit < layoutA.getInDimSizeLog2(reg); ++bit) {
    if ((v_reg >> bit) & 1) { // If this physical bit is ON
      w_dim1 ^= layoutA.getBasis(reg, bit, dim1);
      w_dim0 ^= layoutA.getBasis(reg, bit, dim0);
    }
  }

  // 2. Process Thread bits
  for (int bit = 0; bit < layoutA.getInDimSizeLog2(thread); ++bit) {
    if ((v_thread >> bit) & 1) { // If this physical bit is ON
      w_dim1 ^= layoutA.getBasis(thread, bit, dim1);
      w_dim0 ^= layoutA.getBasis(thread, bit, dim0);
    }
  }

  w_dim0 ^= layoutA.getBasis(warp, 0, dim0);

  return {w_dim0, w_dim1}; // Return (i, j)
}

int main(int argc, char** argv) {
  mlir::DialectRegistry registry;
  MLIRContext context(registry);
  MLIRContext* ctx = &context;


  llvm::outs() << "========================================================================================\n";
  llvm::outs() << "LinearLayout\n";
  llvm::outs() << "========================================================================================\n";

  auto threadLayout = buildThreadLayout(ctx);
  llvm::outs() << "threadLayout: " << threadLayout << "\n\n";

  auto regLayout = buildRegLayout(ctx);
  llvm::outs() << "regLayout: " << regLayout << "\n\n";

  auto layout = regLayout * threadLayout;
  llvm::outs() << "regLayout * threadLayout: " << layout << "\n\n";

  // auto ptr = tt::getMatrix(threadLayout);
  // auto m = ptr.get();
  // int numRows = threadLayout.getTotalOutDimSizeLog2();
  // int numCols = threadLayout.getTotalInDimSizeLog2();
  // for (int i = 0; i < numRows; i++) {
  //   auto tmp_bits = std::bitset<64>(m[i]).to_string();
  //   auto row_bits = tmp_bits.substr(64 - numCols, numCols);
  //   std::reverse(row_bits.begin(), row_bits.end());
  //   llvm::outs() << "Row " << i << ": " << row_bits << "\n";
  // }

  // auto layoutA = buildLayoutA(ctx);
  // llvm::outs() << "layoutA: " << layoutA << "\n\n";
/*
layoutA: 
 - register=1 -> (1, 0)
   register=2 -> (0, 1)
 - thread=1 -> (2, 0)
   thread=2 -> (4, 0)
   thread=4 -> (8, 0)
   thread=8 -> (0, 2)
   thread=16 -> (0, 4)
 - warp=1 -> (0, 8)
where out dims are: [dim1 (size 16), dim0 (size 16)]

Physical bits = l_W|l_T|l_R = i_W|j_W|i_T|j_T|i_R|j_R
where,
  - i_W is 1 bit
  - j_W is 0 bit
  - i_T is 2 bits
  - j_T is 3 bits
  - i_R is 1 bit
  - j_R is 1 bit

Since ID maps will map 
  - i_* -> i_W|i_T|i_R
  - j_* -> j_W|j_T|j_R

thread=1 -> l_T = b00001 -> j_T = b001, i_T = b00. Because j_R occupies 1 bit slot before j_T, we must shift j_T 1 bit to the left.
So the final coordinate becomes (b010, b00) = (2, 0).
thread=2 -> l_T = b00010 -> j_T = b010, i_T = b00 -> (b100, b00) = (4, 0)
thread=4 -> l_T = b00100 -> j_T = b100, i_T = b00 -> (b1000, b00) = (8, 0)

thread=8 -> l_T = b01000 -> j_T = b000, i_T = b01. Because i_R occupies 1 bit slot before i_T, we must shift i_T 1 bit to the left.
So the final coordinate becomes (b000, b10) = (0, 2).
thread=16 -> l_t = b10000 -> j_T = b000, i_T = b10 -> (b000, b100) = (0, 4)
*/


  // // Sublayout
  // auto thread = StringAttr::get(ctx, "thread");
  // SmallVector<StringAttr> outDimNames = {
  //   // StringAttr::get(ctx, "dim1"),
  //   StringAttr::get(ctx, "dim0")
  // };
  // auto sublayout = layoutA.sublayout(thread, outDimNames);
  // llvm::outs() << "sublayout: " << sublayout << "\n\n";

  // // Test layoutA
  // int v = 0b11010101; // l_W|l_T|l_R
  // auto w = applyLayoutA(ctx, layoutA, v);
  // llvm::outs() << "v = " << v << "\n";
  // llvm::outs() << "i = " << w.first << "\n";
  // llvm::outs() << "j = " << w.second << "\n";

  llvm::outs() << "\nAll done\n";
  return 0;
}
