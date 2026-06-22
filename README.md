# LinearLayout Paper
Original paper: https://arxiv.org/pdf/2505.23819

Downloaded paper: [Linear Layouts Robust Code Generation of Efficient Tensor Computation Using F2.pdf](./Linear%20Layouts%20Robust%20Code%20Generation%20of%20Efficient%20Tensor%20Computation%20Using%20F2.pdf)


# Foundational Linear Algebra
## Linear Map
Given two vector spaces $U$, $V$, and a linear map:

$L:\ U \rightarrow V$

Let $N = \text{dim}(U)$

Let $\boldsymbol{e}_i,\ 0 \ll i < N$ denote the **standard basis** of $U$.

With any vector $\boldsymbol{u} = (u_0, \dots, u_{N-1}) \in U$, by definition, it can be represented as a linear combination of $\boldsymbol{e}_i$:

$`\boldsymbol{u} = u_0\boldsymbol{e}_0 + \cdots + u_{N-1}\boldsymbol{e}_{N-1}`$

Then, $L(\boldsymbol{u})$ can be computed as follows:

$`
\begin{aligned}
L(\boldsymbol{u}) &= L(u_0\boldsymbol{e}_0 + \cdots + u_{N-1}\boldsymbol{e}_{N-1}) \\
              &= L(u_0\boldsymbol{e}_0) + \cdots + L(u_{N-1}\boldsymbol{e}_{N-1}) \\
              &= u_0L(\boldsymbol{e}_0) + \cdots + u_{N-1}L(\boldsymbol{e}_{N-1}) 
\end{aligned}
`$

Obviously, if we know $L(\boldsymbol{e}_i)$, we can compute $L(\boldsymbol{u})$ for any $\boldsymbol{u} \in U$.

Therefore, any linear map is completely defined by its action on the basis of the input vector space $L(\boldsymbol{e}_i)$.

## Direct Sum Of Two Linear Maps
### Simple case
Given the following two linear maps:

$L_1:\ F_2^{N_1} \rightarrow F_2^{D_1}$

$L_2:\ F_2^{N_2} \rightarrow F_2^{D_2}$

Direct sum of $L_1$ and $L_2$ is defined as:

$L_1 \times L_2:\ F_2^{N_1} \times F_2^{N_2} \rightarrow F_2^{D_1} \times F_2^{D_2}$
<br/><br/>

Let $\phi$ denote a linear map that flattens a tuple of two vectors $(\boldsymbol{x}, \boldsymbol{y}) \in F_2^{N_1} \times F_2^{N_2}$ to one vector in $F_2^{N_1+N_2}$:

$\phi:\ F_2^{N_1} \times F_2^{N_2} \rightarrow F_2^{N_1+N_2}$

We can easily prove that $\phi$ is a isomorphism (a invertible linear map) and thefore, there exists an inverse $\phi^{-1}$ that splits a vector in $F_2^{N_1+N_2}$ into a tuple of two vectors $(\boldsymbol{x}, \boldsymbol{y}) \in F_2^{N_1} \times F_2^{N_2}$

$\phi^{-1}:\ F_2^{N_1+N_2} \rightarrow F_2^{N_1} \times F_2^{N_2}$
<br/><br/>

Let $L = (L_1 \times L_2) \circ \phi^{-1}$ denote a composition between $(L_1 \times L_2)$ and $\phi^{-1}$, then $L$ is also a linear map:

$L:\ F_2^{N_1+N_2} \rightarrow F_2^{D_1} \times F_2^{D_2}$

Let $\boldsymbol{e}_k,\ 0 \ll k < N_1 + N_2$ denote the **standard basis** in $F_2^{N_1+N_2}$

We can compute $L(\boldsymbol{e}_k),\ 0 \ll k < N_1 + N_2$ via $L_1$, $L_2$ as follows:

$L(\boldsymbol{e}_k) = \big((L_1 \times L_2) \circ \phi^{-1}\big)(\boldsymbol{e}_k) = (L_1 \times L_2)\big(\phi^{-1}(\boldsymbol{e}_k)\big)$

Let $\boldsymbol{u}_i$ denote the standard basis vectors in $F_2^{N_1}$ and $\boldsymbol{v}_j$ denote the standard basis vectors in $F_2^{N_2}$. When we apply $\phi^{-1}$ to the standard basis $\boldsymbol{e}_k$ of $F_2^{N_1+N_2}$, it splits into two distinct cases depending on the index $k$:

For $0 \leq k < N_1$: 
- The basis vector maps entirely to the first component.

  $`\phi^{-1}(\boldsymbol{e}_k) = \big(\boldsymbol{u}_k, \boldsymbol{0}^{N_2}\big)`$, where $\boldsymbol{0}^{N_2}$ is the zero vector in $F_2^{N_2}$

For $N_1 \leq k < N_1 + N_2$: 
- The basis vector maps entirely to the second component.

  $`\phi^{-1}(\boldsymbol{e}_k) = \big(\boldsymbol{0}^{N_1}, \boldsymbol{v}_{k - N_1}\big)`$, where $\boldsymbol{0}^{N_1}$ is the zero vector in $F_2^{N_1}$

Thus,

For $0 \leq k < N_1$: 
- $`L(\boldsymbol{e}_k) = (L_1 \times L_2)\big(\boldsymbol{u}_k, \boldsymbol{0}^{N_2}\big) = \big(L_1(\boldsymbol{u}_k), L_2(\boldsymbol{0}^{N_2})\big) = \big(L_1(\boldsymbol{u}_k), \boldsymbol{0}^{D_2}\big)`$, where $\boldsymbol{0}^{D_2}$ is zero vector in $F_2^{D_2}$

  Since $`L_1(\boldsymbol{u}_k)`$ is a binary vector of $N_1$ elements, we can represent it by an integer $t_{1k}$

  So $`L(\boldsymbol{e}_k) = (t_{1k}, 0)`$
<br/><br/>

For $N_1 \leq k < N_1 + N_2$: 
- $`L(\boldsymbol{e}_k) = (L_1 \times L_2)\big(\boldsymbol{0}^{N_1}, \boldsymbol{v}_{k - N_1}\big) = \big(L_1(\boldsymbol{0}^{N_1}), L_2(\boldsymbol{v}_{k - N_1})\big) = \big(\boldsymbol{0}^{D_1}, L_2(\boldsymbol{v}_{k - N_1})\big)`$, where $\boldsymbol{0}^{D_1}$ is zero vector in $F_2^{D_1}$

  Similarly, we can represent $`L_2(\boldsymbol{v}_{k - N_1})`$ by an integer $t_{2k}$.

  So $`L(\boldsymbol{e}_k) = (0, t_{2k})`$

<br/><br/>
### Merging output space case
Given the following linear maps:

$`L_1: \ F_2^{N_1} \rightarrow F_2^{Q_{dim1}} \times F_2^{Q_{dim0}}`$

$`L_2: \ F_2^{N_2} \rightarrow F_2^{P_{dim1}} \times F_2^{P_{dim0}}`$
<br/><br/>

$`L_1 \times L_2:\ F_2^{N_1} \times F_2^{N_2} \rightarrow (F_2^{Q_{dim1}} \times F_2^{Q_{dim0}}) \times (F_2^{P_{dim1}} \times F_2^{P_{dim0}})`$

$`\phi:\ F_2^{N_1} \times F_2^{N_2} \rightarrow F_2^{N_1+N_2}`$

$`\phi^{-1}:\ F_2^{N_1+N_2} \rightarrow F_2^{N_1} \times F_2^{N_2}`$

$`L:\ F_2^{N_1+N_2} \rightarrow (F_2^{Q_{dim1}} \times F_2^{Q_{dim0}}) \times (F_2^{P_{dim1}} \times F_2^{P_{dim0}})`$
<br/><br/>

Similarly, we can calculate $L(e_k)$ as follows:

For $0 \leq k < N_1$: 
- $`L(\boldsymbol{e}_k) = (L_1 \times L_2)\big(\boldsymbol{u}_k, \boldsymbol{0}^{N_2}\big) = \big(L_1(\boldsymbol{u}_k), L_2(\boldsymbol{0}^{N_2})\big) = \big(L_1(\boldsymbol{u}_k), \boldsymbol{0}^{P}\big)`$, where $\boldsymbol{0}^{P}$ is zero vector in $`(F_2^{P_{dim1}} \times F_2^{P_{dim0}})`$

<br/>

For $N_1 \le k < N_1 + N_2$, denote $h = k - N_1$
- $`L(\boldsymbol{e}_k) = (L_1 \times L_2)\big(\boldsymbol{0}^{N_1}, \boldsymbol{v}_{h}\big) = \big(L_1(\boldsymbol{0}^{N_1}), L_2(\boldsymbol{v}_{h})\big) = \big(\boldsymbol{0}^{Q}, L_2(\boldsymbol{v}_{h})\big)`$, where $\boldsymbol{0}^{Q}$ is zero vector in $`(F_2^{Q_{dim1}} \times F_2^{Q_{dim0}})`$
<br/><br/>

Let $R$ denote the following **Canonical Isomorphism** (Regrouping):

$R: \ (F_2^{Q_{dim1}} \times F_2^{Q_{dim0}}) \times (F_2^{P_{dim1}} \times F_2^{P_{dim0}}) \rightarrow (F_2^{Q_{dim1}} \times F_2^{P_{dim1}}) \times (F_2^{Q_{dim0}} \times F_2^{P_{dim0}})$

Let $\Omega$ denote the following **Canonical Isomorphism** (Flattening):

$\Omega: \ (F_2^{Q_{dim1}} \times F_2^{P_{dim1}}) \times (F_2^{Q_{dim0}} \times F_2^{P_{dim0}}) \rightarrow F_2^{Q_{dim1} + P_{dim1}} \times F_2^{Q_{dim0} + P_{dim0}}$

Let $T = \Omega \circ R \circ L$ denote a composition of $\Omega$ and $R$ and $L$. Then, $T$ is also a linear map:

$T: \ F_2^{N_1+N_2} \rightarrow F_2^{Q_{dim1} + P_{dim1}} \times F_2^{Q_{dim0} + P_{dim0}}$
<br/><br/>

Now let's calculate $T(e_k)$:

$T(e_k) = (\Omega \circ R \circ L)(e_k) = \Omega(R(L(e_k)))$

For $0 \le k < N_1$
- $`T(e_k) = \Omega \big( R\big((L_1(\boldsymbol{u}_k)^{Q_{dim1}}, L_1(\boldsymbol{u}_k)^{Q_{dim0}}), (\boldsymbol{0}^{P_{dim1}}, \boldsymbol{0}^{P_{dim0}})\big)\big) = \Omega\big((L_1(\boldsymbol{u}_k)^{Q_{dim1}}, \boldsymbol{0}^{P_{dim1}}), (L_1(\boldsymbol{u}_k)^{Q_{dim0}}, \boldsymbol{0}^{P_{dim0}})\big)`$

  Since $`L_1(\boldsymbol{u}_k)^{Q_{dim1}}`$ and $`L_1(\boldsymbol{u}_k)^{Q_{dim0}}`$ are binary vectors, we can represent them by the integers $t_{k,dim1}$ and $t_{k,dim0}$, respectively.

  The flattened forms of $`(L_1(\boldsymbol{u}_k)^{Q_{dim1}}, \boldsymbol{0}^{P_{dim1}})`$ and $`(L_1(\boldsymbol{u}_k)^{Q_{dim0}}, \boldsymbol{0}^{P_{dim0}})`$ are binary vectors whose higher-order bits are zero. 
  
  Obviously, they can be reduced exactly to the integers $t_{k,dim1}$ and $t_{k,dim0}$, respectively.

  $T(e_k) = (t_{k,dim1}$, $t_{k,dim0})$  
<br/>

For $N_1 \le k < N_1 + N_2$
- $`T(e_k) = \Omega\big(R\big((\boldsymbol{0}^{Q_{dim1}}, \boldsymbol{0}^{Q_{dim0}}), (L_2(\boldsymbol{v}_h)^{P_{dim1}}, L_2(\boldsymbol{v}_h)^{P_{dim0}})\big)\big) = \Omega\big((\boldsymbol{0}^{Q_{dim1}}, L_2(\boldsymbol{v}_h)^{P_{dim1}}), (\boldsymbol{0}^{Q_{dim0}}, L_2(\boldsymbol{v}_h)^{P_{dim0}})\big)`$

  Similarly, we can represent $`L_2(\boldsymbol{v}_h)^{P_{dim1}}`$ and $`L_2(\boldsymbol{v}_h)^{P_{dim0}}`$ by the integers $w_{k,dim1}$ and $w_{k,dim0}$, respectively.

  $T(e_k) = (w_{k,dim1} \ll Q_{dim1}, w_{k,dim0} \ll Q_{dim0})$
<br/><br/>