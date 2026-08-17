# LinearLayout::sublayout()
```C++
LinearLayout LinearLayout::sublayout(ArrayRef<StringAttr> inDimNames,
                                     ArrayRef<StringAttr> outDimNames) const {
  assertDimsSubsetIgnoringOrder(inDimNames, getInDimNames());
  assertDimsSubsetIgnoringOrder(outDimNames, getOutDimNames());
  SmallDenseSet<StringAttr> inDimSet(inDimNames.begin(), inDimNames.end());
  SmallDenseSet<StringAttr> outDimSet(outDimNames.begin(), outDimNames.end());

  SmallVector<int> outDimIndicesToKeep;
  for (auto [i, outDim] : llvm::enumerate(getOutDimNames())) {
    if (outDimSet.contains(outDim)) {
      outDimIndicesToKeep.push_back(i);
    }
  }

  llvm::outs() << "\noutDimIndicesToKeep: " << outDimIndicesToKeep << "\n\n";

  BasesT newBases;
  for (auto [inDim, inDimBases] : bases) {
    if (!inDimSet.contains(inDim)) {
      continue;
    }
    auto &newInDimBases = newBases[inDim];
    for (auto &basis : inDimBases) {
      auto &newBasis = newInDimBases.emplace_back();
      for (int i : outDimIndicesToKeep) {
        newBasis.push_back(basis[i]);
      }
    }
  }

  SmallVector<std::pair<StringAttr, int32_t>> newOutDims;
  for (auto [outDim, outDimSize] : outDims) {
    if (outDimSet.contains(outDim)) {
      newOutDims.push_back({outDim, outDimSize});
    }
  }
  return LinearLayout(std::move(newBases), std::move(newOutDims),
                      /*requireSurjective=*/false);
}
```

## Foundational Algebra
Framing `sublayout` strictly through the lens of abstract algebra provides the exact theoretical foundation for why Triton's matrix extractions and zero-eliminations are mathematically sound.

In linear algebra over $\mathbb{F}_2$, a "layout" is a linear map between two vector spaces. A "sublayout" is technically the **composition of (1) an inclusion map, (2) the original linear map, and (3) a projection map**.

Here is the strict algebraic breakdown of what `layout.sublayout(inDimNames, outDimName)` actually is.

---

### 1. The Vector Spaces as Direct Sums

First, we define the full input vector space $V$ and the full output vector space $W$.

In Triton, these spaces are constructed by concatenating (or strictly speaking, taking the direct sum of) the bit-vectors of individual dimensions.
Let's use your previous example with input dimensions "msg" and "block", mapping to output dimensions, say, "out1" and "out2".

* **Input Space ($V$):** $V = V_{\text{msg}} \oplus V_{\text{block}}$
* **Output Space ($W$):** $W = W_{\text{out1}} \oplus W_{\text{out2}}$

The full linear layout is the homomorphism $L: V \to W$.

---

### 2. The Inclusion Map (Restricting Inputs)

When the code requests `inDimNames` (e.g., just "msg"), it is isolating a subspace of the input. Algebraically, you cannot feed a vector from the subspace $V_{\text{msg}}$ directly into $L$, because $L$ expects a vector from the full space $V$.

To bridge this, we define an **inclusion map** (or canonical injection) $\phi: V_{\text{msg}} \hookrightarrow V$.

This map takes a vector $\vec{m} \in V_{\text{msg}}$ and maps it into the larger space by padding the missing dimensions with the zero vector $\vec{0} \in V_{\text{block}}$:

$\phi(\vec{m}) = \vec{m} \oplus \vec{0}$

*(Note: This directly mirrors why the compiler can drop the zero-padded constant dimensions—it is applying this exact inclusion map).*

---

### 3. The Projection Map (Restricting Outputs)

When the code specifies `outDimName` (e.g., just "out1"), it does not want the entire output vector in $W$. It only wants the subspace $W_{\text{out1}}$.

To achieve this, we define a **projection map** $\pi: W \twoheadrightarrow W_{\text{out1}}$.

This map takes a fully formed output vector $\vec{w} = \vec{w}_1 \oplus \vec{w}_2 \in W$ and annihilates the unwanted components, projecting it down to just the requested subspace:

$\pi(\vec{w}_1 \oplus \vec{w}_2) = \vec{w}_1$

---

### 4. The Sublayout as a Composition

The `sublayout` operation is simply the algebraic composition of these three maps:

$L_{\text{sub}} = \pi \circ L \circ \phi$

This means $L_{\text{sub}}$ is a strictly defined, entirely new linear map $L_{\text{sub}}: V_{\text{msg}} \to W_{\text{out1}}$. Because the composition of linear maps is always a linear map, $L_{\text{sub}}$ retains all the properties of superposition over $\mathbb{F}_2$.

---

## Mapping Algebra with C++ code
The fundamental property of linear maps: **a linear map is uniquely and completely determined by its action on a basis.**

Here is the strict formulation of the C++ code relying entirely on vector spaces, basis vectors, and linear mappings.

### 1. The Vector Spaces as Direct Sums

Let $I$ be the set of all input dimension names and $O$ be the set of all output dimension names.
The total input space $V$ and total output space $W$ are the direct sums of the spaces for each dimension:


$V = \bigoplus_{x \in I} V_x$

$W = \bigoplus_{y \in O} W_y$

Let $S \subseteq I$ be the requested input dimensions (`inDimSet`) and $T \subseteq O$ be the requested output dimensions (`outDimSet`).
The subspaces we are mapping between are:


$V_S = \bigoplus_{x \in S} V_x$

$W_T = \bigoplus_{y \in T} W_y$

### 2. The Original Map and the Basis

The original layout is a linear map $L: V \to W$.

Let $E_x$ be the standard basis for the subspace $V_x$. The union $\bigcup_{x \in I} E_x$ forms a complete basis for the total input space $V$.

**Mapping to C++:** The `bases` variable does not store a matrix; it stores the *images* of the basis vectors under $L$.
When the code iterates `for (auto &basis : inDimBases)`, `basis` represents the vector $w = L(e) \in W$ for some specific basis vector $e \in E_x$.

### 3. The Inclusion Map ($\phi$)

The inclusion map $\phi: V_S \to V$ takes a tuple from the subspace and expands it into a tuple in the full space $V = \bigoplus_{y \in I} V_y$ by padding the missing dimensions with zeros.

For any tuple $(v_x)_{x \in S} \in V_S$, the inclusion map produces a tuple $(u_y)_{y \in I} \in V$ such that:

* $u_y = v_y$ if $y \in S$
* $u_y = 0_y$ if $y \in I \setminus S$


**Mapping to C++:** To evaluate the composed map $L_{\text{sub}}$ on the domain $V_S$, we only need to know its action on the basis vectors of $V_S$. The code explicitly ignores any basis vectors that are not in the domain of $\phi$ by skipping dimensions $x \notin S$:

```cpp
if (!inDimSet.contains(inDim)) {
  continue; 
}

```

### 4. The Projection Map ($\pi$)

We define the projection map $\pi: W \to W_T$. For any vector $w \in W$, $\pi(w)$ discards the components belonging to $\bigoplus_{y \notin T} W_y$.

**Mapping to C++:** For a basis vector $e_i^S \in V_S$, the inclusion map $\phi(e_i^S)$ produces a corresponding basis vector $e_k$ in $V$, the code has the image $w = L(\phi(e_i^S)) = L(e_k)$ stored in the `basis` variable. It then applies the projection map $\pi$ to this image by iterating over the coordinates that belong to $T$ (`outDimIndicesToKeep`) and discarding the rest:

```cpp
for (int i : outDimIndicesToKeep) {
  newBasis.push_back(basis[i]);
}

```

This physically constructs the vector $\pi(L(\phi(e_i^S))) \in W_T$.

### 5. The Composition ($L_{\text{sub}}$)

Because a linear map is entirely defined by where it sends the basis vectors of its domain, the C++ code constructs the new linear map $L_{\text{sub}}: V_S \to W_T$ by explicitly computing:


$L_{\text{sub}}(e_i^S) = \pi(L(\phi(e_i^S)))$


for every basis vector $e_i^S \in \bigcup_{x \in S} E_x$.

By storing these newly computed image vectors in `newBases` and passing them to the `LinearLayout` constructor, the code fully instantiates the composed map $L_{\text{sub}} = \pi \circ L \circ \phi$ without ever needing to reference matrices or their structural blocks.