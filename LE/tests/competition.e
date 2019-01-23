void main ( ){

  ; Allocate the matrices
  int64 dim <- 300
  int64[][] m1 <- new Array(dim, dim)
  int64[][] m2 <- new Array(dim, dim)
  int64[][] m3 <- new Array(dim, dim)

  ; Compute
  int64 max <- 4

  for (int64 index1 <- 0; index1 < max; index1 <- index1 + 1){
    for (int64 index2 <- 0; index2 < max; index2 <- index2 + 1) {
      computeAndPrint(m1, m2, m3, index1, index2);
    }
  }

  return
}

void computeAndPrint (int64[][] m1, int64[][] m2, int64[][] m3, int64 v1, int64 v2){

  ; Initialize the matrices
  initMatrix(m1, v1)
  initMatrix(m2, v2)

  ; Multiple the matrices
  matrixMultiplication(m1, m2, m3)

  ; Compute the total sum
  int64 t <- totalSum(m1)
  print(t)
  t <- totalSum(m2)
  print(t)
  t <- totalSum(m3)
  print(t)

  return
}

void initMatrix (int64[][] m, int64 initValue){
  for (int64 index1 <- 0; index1 < length m 0; index1 <- index1 + 1) {
    for (int64 index2 <- 0; index2 < length m 1; index2 <- index2 + 1) {
      m[index1][index2] <- initValue + index1 + index2
    }
  }
  return
}

void matrixMultiplication (int64[][] m1, int64[][] m2, int64[][] m3){

  ; Check the length
  if (length m1 1 != length m2 0) {
    return 
  }
  if (length m3 0 != length m1 0) {
    return
  }
  if (length m3 1 != length m2 1) {
    return 
  }

  ; Initialize the result matrix
  for (int64 i <- 0; i < length m1 0; i <- i + 1) {
    for (int64 j <- 0; j < length m2 1; j <- j + 1) {
      for (int64 k <- 0; k < length m1 1; k <- k + 1) {
        m3[i][j] <- 0
      }
    }
  }

  ; Multiply the matrices
  for (int64 i <- 0; i < length m1 0; i <- i + 1) {
    for (int64 j <- 0; j < length m2 1; j <- j + 1) {
      for (int64 k <- 0; k < length m1 1; k <- k + 1) {
        m3[i][j] <- m3[i][j] + (m1[i][k] * m2[k][j])
      }
    }
  }

  return
}

int64 totalSum (int64[][] m){
  int64 sum <- 0
  for (int64 index1 <- 0; index1 < length m 0; index1 <- index1 + 1) {
    for (int64 index2 <- 0; index2 < length m 1; index2 <- index2 + 1) {
      sum <- sum + m[index1][index2]
    }    
  }
  return sum
}
