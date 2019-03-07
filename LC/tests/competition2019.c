void main ( ){

  // Allocate the matrices
  int64[][] m1, m2, m3
  int64 dim
  dim <- 300
  m1 <- new Array(dim, dim)
  m2 <- new Array(dim, dim)
  m3 <- new Array(dim, dim)

  // Compute
  computeAndPrint(m1, m2, m3, 0)

  return
}

void computeAndPrint (int64[][] m1, int64[][] m2, int64[][] m3, int64 v1){

  // Initialize the matrices
  initMatrix(m1, v1)
  initMatrix(m2, v1)

  // Multiple the matrices
  matrixMultiplication(m1, m2, m3)

  // Compute the total sum
  int64 t
  t <-  totalSum(m1)
  print(t)
  t <-  totalSum(m2)
  print(t)
  t <-  totalSum(m3)
  print(t)

  return
}

void initMatrix (int64[][] m, int64 initValue){

  // Fetch the lengths
  int64 l1, l2, index1
  l1 <- length m 0
  l2 <- length m 1

  // Initialize variables
  index1 <- 0

  // Iterate over the rows
  while (index1 < l1) {
    int64 index2
    index2 <- 0

    // Iterate over the columns
    while (index2 < l2) {
  
      // Compute the value to store
      int64 valueToStore
      valueToStore <- initValue + index1
      valueToStore <- valueToStore + index2
  
      // Store the value to the current matrix element
      m[index1][index2] <- valueToStore

      index2 <- index2 + 1
    }

    index1 <- index1 + 1
  }

  return
}

void matrixMultiplication (int64[][] m1, int64[][] m2, int64[][] m3){

  // Fetch the lengths
  int64 m1_l1, m1_l2, m2_l1, m2_l2, m3_l1, m3_l2
  m1_l1 <- length m1 0
  m1_l2 <- length m1 1
  m2_l1 <- length m2 0
  m2_l2 <- length m2 1
  m3_l1 <- length m3 0
  m3_l2 <- length m3 1

  // Check the length
  if (m1_l2 != m2_l1) {
    return 
  }
  if (m3_l1 != m1_l1) {
    return 
  }
  if (m3_l2 != m2_l2) {
    return 
  }

  // Initialize the result matrix
  {
    int64 i, j, k
    i <- 0
    while (i < m1_l1) {
      j <- 0
      while (j < m2_l2) {
        k <- 0
        while (k < m1_l2) {
          int64 tmp
          tmp <- 1
          tmp <- tmp * 4
          tmp <- tmp + 2
          tmp <- tmp * 8

          m3[i][j] <- 0

          tmp <- m3[i][j]
          tmp <- tmp + 4
          m3[i][j] <- tmp

          tmp <- m3[i][j]
          tmp <- tmp * 4
          m3[i][j] <- tmp

          k <- k + 1
        }
  
        j <- j + 1
      }
  
      i <- i + 1
    }
  }

  // Multiply the matrices
  int64 tot
  int64 i, j, k
  i <- 0
  tot <- 0
  while (i < m1_l1) {
    j <- 0
    while (j < m2_l2) {
      k <- 0
      while (k < m1_l2) {
        int64 A, B, C, D, E, F
        A <- m1[i][k]
        B <- m2[k][j]
        C <- A * B
        D <- m3[i][j]
        E <- D * 4
        F <- E + 2
        m3[i][j] <- F

        int64 G
        G <- F * 1
        m3[i][j] <- G
        G <- F + 0
        m3[i][j] <- G


        G <- F * 1
        m3[i][j] <- G
        G <- F + 0
        m3[i][j] <- G


        int64 tmp
        tmp <- G
        tmp <- tmp * 4
        tmp <- tmp + 2
        tmp <- tmp * 8
        tmp <- tmp * 4
        tmp <- tmp + 2
        tmp <- tmp * 8
        tmp <- tmp * 4
        tmp <- tmp + 2
        tmp <- tmp * 8
        tmp <- tmp * 4
        tmp <- tmp + 2
        tmp <- tmp * 8
        tmp <- tmp * 4
        tmp <- tmp + 2
        tmp <- tmp * 8
        tmp <- tmp * 4
        tmp <- tmp + 2
        tmp <- tmp * 8
        tmp <- tmp << 2
        tmp <- tmp + 2
        tmp <- tmp * 8
        tmp <- tmp * 4
        tmp <- tmp << 2
        tmp <- tmp + 2
        tmp <- tmp * 8
        tmp <- tmp * 4
        tmp <- tmp << 4
        tmp <- tmp + 2
        tmp <- tmp >> 2
        tmp <- tmp * 8
        tmp <- tmp * 4
        tmp <- tmp + 2
        tmp <- tmp * 8
        tmp <- tmp * 4
        tmp <- tmp + 2
        tmp <- tmp * 8

        int64 tot2, tot3
        tot2 <- tot * 4
        tot3 <- tot2 + 2
        tot <- tot

        k <- k + 1
      }

      j <- j + 1
    }

    i <- i + 1
  }

  return
}

int64 totalSum (int64[][] m){
  int64 sum
 
  // Fetch the lengths
  int64 l1, l2, index1
  l1 <- length m 0
  l2 <- length m 1

  // Initialize variables
  index1 <- 0
  sum <- 0

  // Iterate over the rows
  while (index1 < l1) {
    int64 index2
    index2 <- 0

    // Iterate over the columns
    while (index2 < l2) {
  
      // Sum the current value
      int64 temp
      temp <- m[index1][index2]
      sum <- sum + temp

      index2 <- index2 + 1
    }

    index1 <- index1 + 1
  }

  return sum
}
