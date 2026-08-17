# LinearLayout::strided1D
```C++
/*static*/ LinearLayout LinearLayout::strided1D(int32_t size, int32_t stride,
                                                StringAttr inDimName,
                                                StringAttr outDimName) {
  if (size == 0)
    return LinearLayout::empty();

  assert(llvm::isPowerOf2_32(size));
  std::vector<std::vector<int32_t>> bases;
  for (int32_t i = 1; i < size; i *= 2) {
    bases.emplace_back(std::vector<int32_t>{i * stride});
  }
  bool requiresSurjective = (stride == 1);
  return LinearLayout({{inDimName, std::move(bases)}},
                      {{outDimName, stride * size}}, requiresSurjective);
}
```
