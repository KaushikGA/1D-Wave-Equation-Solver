# 1D Wave Equation Solver (C++)




## Overview
This project is a high-performance C++ solver for the 1D Wave Equation, visualized using Python. I built this as a personal exploration into computational physics and numerical methods, specifically focusing on finite difference schemes, periodic boundary conditions, and memory-efficient C++ programming.

<p align="center">
  <img src="assets/wave_animation.gif" alt="1D wave on periodic boundary conditions" width="400"/>
</p>

## Mathematics & Discretization

The solver approximates the continuous one-dimensional wave equation:

$$
\frac{\partial^2 u}{\partial t^2} = c^2 \frac{\partial^2 u}{\partial x^2}
$$

Where $u$ is the wave amplitude and $c$ is the wave velocity. 

To solve this computationally, I implemented an **Explicit Central Difference Scheme**. By discretizing both time ($\Delta t$) and space ($\Delta x$), using Taylor series expansion, the numerical scheme becomes:

$$
u_i^{t+1} = 2u_i^t - u_i^{t-1} + C^2 \left( u_{i+1}^t - 2u_i^t + u_{i-1}^t \right)
$$

Where $C$ is the **Courant–Friedrichs–Lewy (CFL)** number:
$$
C = c \frac{\Delta t}{\Delta x} \leq 1
$$

An initial Gaussian pulse is used as the starting condition, and the edges of the domain are enforced with periodic boundary conditions to allow the wave to wrap around the domain seamlessly.

## C++ Optimizations

In numerical computing, memory allocation inside hot loops (like a time-stepping simulation) will ruin performance. To address this, the solver utilizes a zero-allocation buffer swapping technique:

```cpp
// O(1) Memory Swapping: Moves data pointers instead of copying arrays
std::swap(uPast, uPresent);
std::swap(uPresent, uFuture);
