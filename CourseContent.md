# CUDA Programming Summary

## Overview
CUDA (Compute Unified Device Architecture) is a parallel computing platform and programming model developed by NVIDIA. It allows developers to leverage the massive parallelism of GPUs (Graphics Processing Units) to accelerate compute-intensive tasks. This summary provides key concepts, examples, and techniques for effective CUDA programming.

---

## 1. CPU vs. GPU
- **CPU (Central Processing Unit):**
  - Optimized for general-purpose tasks and sequential processing, making it highly flexible in running various applications.
  - Equipped with fewer but powerful cores, tailored for logic-heavy operations and decision-making tasks that cannot be parallelized easily.
  - Acts as the central manager, coordinating tasks and delegating computational workloads to other components like the GPU.

- **GPU (Graphics Processing Unit):**
  - Specialized for parallel tasks with thousands of smaller, efficient cores capable of performing massive calculations simultaneously.
  - Ideal for processing large datasets and parallelizable workloads such as rendering graphics, running simulations, and training AI models.
  - Relies on the CPU for initialization, memory allocation, and overall task coordination to achieve optimal performance.

### Analogy: Cargo Ship vs. Airline
- **CPU:** Functions like an airline, designed for rapid and flexible transport of smaller loads, excelling in tasks that require quick responses and adaptability.
- **GPU:** Functions like a cargo ship, optimized for carrying large amounts of data efficiently, though not as suited for tasks that require immediate results or frequent decision-making.

---

## 2. CUDA Basics

### Key Formula:
```cpp
int idx = threadIdx.x + blockIdx.x * blockDim.x;
```
- **threadIdx.x:** Represents the thread's local index within its block.
- **blockIdx.x:** Refers to the block's position within the grid.
- **blockDim.x:** Specifies the number of threads per block.
- **Purpose:** Computes the global index of a thread by combining its local index within the block and the position of the block in the grid.

This formula is foundational in CUDA programming and applies to many scenarios where 1D grids and blocks are used.

---

## 3. Memory Hierarchy
- **Global Memory:**
  - Provides a large memory space shared across all threads but comes with significant latency, making it slower for frequent accesses.
  - Accessible by both the CPU and GPU, used to store large datasets or intermediate results.

- **Shared Memory:**
  - An on-chip memory that is faster than global memory, shared among threads within the same block.
  - Requires explicit management using the `__shared__` keyword and synchronization to avoid race conditions.

- **Registers:**
  - The fastest type of memory, reserved for individual threads.
  - Used to store thread-specific variables with no sharing between threads.

---

## 4. CUDA Thread Organization
- Threads are organized into **blocks**, and blocks are organized into a **grid**, enabling scalable parallelism.
- **1D Thread Grid:**
  - Uses the basic formula to calculate global indices.
  
- **2D/3D Thread Grid:**
  - Supports multidimensional data processing by extending the formula:
  ```cpp
  int x = threadIdx.x + blockIdx.x * blockDim.x;
  int y = threadIdx.y + blockIdx.y * blockDim.y;
  int z = threadIdx.z + blockIdx.z * blockDim.z;
  ```
  - Enables efficient handling of data like matrices or 3D volumes by mapping thread indices to multidimensional coordinates.

- Each thread processes a specific portion of the data, ensuring that computations are distributed across the grid.

---

## 5. Shared Memory Optimization Example
```cpp
__global__ void addArraysShared(int *a, int *b, int *c, int size) {
    __shared__ int shared_a[256];
    __shared__ int shared_b[256];

    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    int localIdx = threadIdx.x;

    if (idx < size) {
        shared_a[localIdx] = a[idx];
        shared_b[localIdx] = b[idx];
        __syncthreads();

        c[idx] = shared_a[localIdx] + shared_b[localIdx];
    }
}
```

### Explanation:
- Shared memory allows threads within a block to share and quickly access data, reducing global memory usage.
- Synchronization (`__syncthreads()`) ensures all threads load data into shared memory before any computations begin.

---

## 6. Multi-Dimensional Grids Example
- **Matrix Addition:**
```cpp
__global__ void addMatrices(int *a, int *b, int *c, int width) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;
    int idx = row * width + col;

    if (row < width && col < width) {
        c[idx] = a[idx] + b[idx];
    }
}
```

### Explanation:
- Uses a 2D grid of threads to map thread indices to matrix coordinates.
- Threads perform addition element-wise, processing one row or column at a time depending on grid dimensions.

---

## 7. Optimization Techniques
1. **Memory Coalescing:**
   - Align memory accesses so that consecutive threads access consecutive memory locations, improving bandwidth utilization.

2. **Using Shared Memory:**
   - Minimize the number of global memory accesses by storing reusable data in shared memory.
   - Example: Tile-based matrix multiplication.

3. **Thread Synchronization:**
   - Use `__syncthreads()` to ensure that all threads complete a task before proceeding, avoiding race conditions.

4. **Minimizing Divergence:**
   - Avoid divergent branches (`if-else`) within warps, as this can slow down execution.

---

## 8. Stream Programming
- CUDA streams allow overlapping computation and memory transfers for better utilization of GPU resources.
```cpp
cudaMemcpyAsync(...);
kernel<<<...>>>(...);
cudaMemcpyAsync(...);
```
- Example:
  - Transfer data while a kernel executes.
  - Process the next set of data while transferring results back to the host.

### Benefits:
- Enables pipelined execution, reducing idle times for the GPU.

---

## 9. Advanced Topics
1. **Matrix Multiplication:**
   - Uses shared memory to store intermediate tiles for reduced global memory reads.
   - Ensures efficient thread-to-data mapping by organizing threads in a 2D grid.

2. **Profiling:**
   - Tools like NVIDIA Nsight and `nvprof` help identify bottlenecks and optimize memory usage, thread efficiency, and kernel performance.

3. **Distributed GPU Programming:**
   - Leverage multiple GPUs using MPI or NCCL for scalable solutions.
   - Ideal for tasks like deep learning training and large-scale simulations.

---

Here’s a full markdown text with all the CUDA code examples we’ve discussed:

```markdown
# Full CUDA Code Examples

## 1. Simple Global Addition
This example performs addition of two arrays on the GPU using global memory.

```cpp
__global__ void addArrays(int *a, int *b, int *c, int size) {
    int idx = threadIdx.x + blockIdx.x * blockDim.x; // Calculate global thread index
    if (idx < size) {
        c[idx] = a[idx] + b[idx]; // Perform addition
    }
}
```

### Host Code to Run the Kernel
```cpp
int main() {
    const int size = 1024;               // Number of elements in arrays
    const int bytes = size * sizeof(int); // Size in bytes

    // Allocate host memory
    int *h_a = (int *)malloc(bytes);
    int *h_b = (int *)malloc(bytes);
    int *h_c = (int *)malloc(bytes);

    // Initialize input arrays
    for (int i = 0; i < size; i++) {
        h_a[i] = i;
        h_b[i] = i * 2;
    }

    // Allocate device memory
    int *d_a, *d_b, *d_c;
    cudaMalloc((void **)&d_a, bytes);
    cudaMalloc((void **)&d_b, bytes);
    cudaMalloc((void **)&d_c, bytes);

    // Copy input arrays from host to device
    cudaMemcpy(d_a, h_a, bytes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, h_b, bytes, cudaMemcpyHostToDevice);

    // Define block and grid size
    int blockSize = 256;  // Threads per block
    int gridSize = (size + blockSize - 1) / blockSize; // Total number of blocks

    // Launch kernel
    addArrays<<<gridSize, blockSize>>>(d_a, d_b, d_c, size);

    // Copy result back to host
    cudaMemcpy(h_c, d_c, bytes, cudaMemcpyDeviceToHost);

    // Verify results
    for (int i = 0; i < size; i++) {
        if (h_c[i] != h_a[i] + h_b[i]) {
            std::cout << "Error at index " << i << std::endl;
            break;
        }
    }

    std::cout << "Addition completed successfully!" << std::endl;

    // Free device and host memory
    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_c);
    free(h_a);
    free(h_b);
    free(h_c);

    return 0;
}
```

---

## 2. Shared Memory Optimized Addition
This version uses shared memory to reduce global memory access latency.

```cpp
__global__ void addArraysShared(int *a, int *b, int *c, int size) {
    __shared__ int shared_a[256];
    __shared__ int shared_b[256];

    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    int localIdx = threadIdx.x;

    if (idx < size) {
        shared_a[localIdx] = a[idx];
        shared_b[localIdx] = b[idx];
        __syncthreads();

        c[idx] = shared_a[localIdx] + shared_b[localIdx];
    }
}
```

---

## 3. Matrix Addition (2D Grids and Blocks)
This example demonstrates matrix addition using a 2D thread and block grid.

```cpp
__global__ void addMatrices(int *a, int *b, int *c, int width) {
    int row = blockIdx.y * blockDim.y + threadIdx.y; // Calculate global row index
    int col = blockIdx.x * blockDim.x + threadIdx.x; // Calculate global column index
    int idx = row * width + col;

    if (row < width && col < width) {
        c[idx] = a[idx] + b[idx];
    }
}
```

### Host Code to Run the Matrix Addition
```cpp
int main() {
    const int width = 1024;
    const int size = width * width;
    const int bytes = size * sizeof(int);

    // Allocate host memory
    int *h_a = (int *)malloc(bytes);
    int *h_b = (int *)malloc(bytes);
    int *h_c = (int *)malloc(bytes);

    // Initialize input matrices
    for (int i = 0; i < size; i++) {
        h_a[i] = i;
        h_b[i] = i * 2;
    }

    // Allocate device memory
    int *d_a, *d_b, *d_c;
    cudaMalloc((void **)&d_a, bytes);
    cudaMalloc((void **)&d_b, bytes);
    cudaMalloc((void **)&d_c, bytes);

    // Copy input matrices from host to device
    cudaMemcpy(d_a, h_a, bytes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, h_b, bytes, cudaMemcpyHostToDevice);

    // Define block and grid size
    dim3 blockSize(16, 16);
    dim3 gridSize((width + blockSize.x - 1) / blockSize.x,
                  (width + blockSize.y - 1) / blockSize.y);

    // Launch kernel
    addMatrices<<<gridSize, blockSize>>>(d_a, d_b, d_c, width);

    // Copy result back to host
    cudaMemcpy(h_c, d_c, bytes, cudaMemcpyDeviceToHost);

    // Verify results
    for (int i = 0; i < size; i++) {
        if (h_c[i] != h_a[i] + h_b[i]) {
            std::cout << "Error at index " << i << std::endl;
            break;
        }
    }

    std::cout << "Matrix addition completed successfully!" << std::endl;

    // Free device and host memory
    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_c);
    free(h_a);
    free(h_b);
    free(h_c);

    return 0;
}
```

---

## Final Note
These examples highlight foundational CUDA programming concepts. Experiment with different block and grid configurations and optimization techniques like memory coalescing to improve performance. Let me know if you'd like additional examples or assistance!
```

## Final Thoughts
CUDA is a powerful tool for leveraging the parallelism of GPUs to solve complex computational problems efficiently. By understanding thread organization, memory hierarchy, and optimization techniques, developers can harness the full potential of modern GPUs for a wide range of applications.

