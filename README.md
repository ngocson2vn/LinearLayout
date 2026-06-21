# LinearLayout Paper
Original paper: https://arxiv.org/pdf/2505.23819

Downloaded paper: [Linear Layouts Robust Code Generation of Efficient Tensor Computation Using F2.pdf](./Linear%20Layouts%20Robust%20Code%20Generation%20of%20Efficient%20Tensor%20Computation%20Using%20F2.pdf)


# Foundational Linear Algebra
## Linear Map
Given two vector spaces $U$, $V$, and a linear map:

$L:\; U \rightarrow V$

Let $N = \text{dim}(U)$

Let $\mathbf{e}_i,\; 0 \ll i < N$ denote the **standard basis** of $U$.

With any vector $\mathbf{u} = (u_0, \dots, u_{N-1}) \in U$, by definition, it can be represented as a linear combination of $\mathbf{e}_i$:

$\mathbf{u} = u_0\mathbf{e}_0 + \cdots + u_{N-1}\mathbf{e}_{N-1}$

Then, $L(\mathbf{u})$ can be computed as follows:

$
\begin{aligned}
L(\mathbf{u}) &= L(u_0\mathbf{e}_0 + \cdots + u_{N-1}\mathbf{e}_{N-1}) \\
              &= L(u_0\mathbf{e}_0) + \cdots + L(u_{N-1}\mathbf{e}_{N-1}) \\
              &= u_0L(\mathbf{e}_0) + \cdots + u_{N-1}L(\mathbf{e}_{N-1}) 
\end{aligned}
$

Obviously, if we know $L(\mathbf{e}_i)$, we can compute $L(\mathbf{u})$ for any $\mathbf{u} \in U$.

Therefore, any linear map is completely defined by its action on the basis of the input vector space $L(\mathbf{e}_i)$.

## Direct Sum Of Two Linear Maps
Given the following two linear maps:

$L_1:\; F_2^{N_1} \rightarrow F_2^{D_1}$

$L_2:\; F_2^{N_2} \rightarrow F_2^{D_2}$

Direct sum of $L_1$ and $L_2$ is defined as:

$L_1 \times L_2:\; F_2^{N_1} \times F_2^{N_2} \rightarrow F_2^{D_1} \times F_2^{D_2}$
<br/><br/>

Let $\phi$ denote a linear map that flattens a tuple of two vectors $(\mathbf{x}, \mathbf{y}) \in F_2^{N_1} \times F_2^{N_2}$ to one vector in $F_2^{N_1+N_2}$:

$\phi:\; F_2^{N_1} \times F_2^{N_2} \rightarrow F_2^{N_1+N_2}$

We can easily prove that $\phi$ is a isomorphism (a invertible linear map) and thefore, there exists an inverse $\phi^{-1}$ that splits a vector in $F_2^{N_1+N_2}$ into a tuple of two vectors $(\mathbf{x}, \mathbf{y}) \in F_2^{N_1} \times F_2^{N_2}$

$\phi^{-1}:\; F_2^{N_1+N_2} \rightarrow F_2^{N_1} \times F_2^{N_2}$
<br/><br/>

Let $L = (L_1 \times L_2) \circ \phi^{-1}$ denote a composition between $(L_1 \times L_2)$ and $\phi^{-1}$, then $L$ is also a linear map:

$L:\; F_2^{N_1+N_2} \rightarrow F_2^{D_1} \times F_2^{D_2}$

Let $\mathbf{e}_k,\; 0 \ll k < N_1 + N_2$ denote **standard basis** in $F_2^{N_1+N_2}$

We can compute $L(\mathbf{e}_k),\; 0 \ll k < N_1 + N_2$ via $L_1$, $L_2$ as follows:

$L(\mathbf{e}_k) = \big((L_1 \times L_2) \circ \phi^{-1}\big)(\mathbf{e}_k) = (L_1 \times L_2)\big(\phi^{-1}(\mathbf{e}_k)\big)$

Let $\mathbf{u}_i$ denote the standard basis vectors in $F_2^{N_1}$ and $\mathbf{v}_j$ denote the standard basis vectors in $F_2^{N_2}$. When we apply $\phi^{-1}$ to the standard basis $\mathbf{e}_k$ of $F_2^{N_1+N_2}$, it splits into two distinct cases depending on the index $k$:

For $0 \leq k < N_1$: 
- The basis vector maps entirely to the first component.

  $\phi^{-1}(\mathbf{e}_k) = \big(\mathbf{u}_k, \mathbf{0}^{N_2}\big)$, where $\mathbf{0}^{N_2}$ is the zero vector in $F_2^{N_2}$

For $N_1 \leq k < N_1 + N_2$: 
- The basis vector maps entirely to the second component.

  $\phi^{-1}(\mathbf{e}_k) = \big(\mathbf{0}^{N_1}, \mathbf{v}_{k - N_1}\big)$, where $\mathbf{0}^{N_1}$ is the zero vector in $F_2^{N_1}$

Thus,

For $0 \leq k < N_1$: 
- $L(\mathbf{e}_k) = (L_1 \times L_2)\big(\mathbf{u}_k, \mathbf{0}^{N_2}\big) = \big(L_1(\mathbf{u}_k), L_2(\mathbf{0}^{N_2})\big) = \big(L_1(\mathbf{u}_k), \mathbf{0}^{D_2}\big)$, where $\mathbf{0}^{D_2}$ is zero vector in $F_2^{D_2}$

For $N_1 \leq k < N_1 + N_2$: 
- $L(\mathbf{e}_k) = (L_1 \times L_2)\big(\mathbf{0}^{N_1}, \mathbf{v}_{k - N_1}\big) = \big(L_1(\mathbf{0}^{N_1}), L_2(\mathbf{v}_{k - N_1})\big) = \big(\mathbf{0}^{D_1}, L_2(\mathbf{v}_{k - N_1})\big)$, where $\mathbf{0}^{D_1}$ is zero vector in $F_2^{D_1}$