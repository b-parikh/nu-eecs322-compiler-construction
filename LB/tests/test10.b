void main ( ){
  int64[][] %m1
  %m1 <- new Array(1, 2)
  call initMatrix(%m1, 0)
  return
}

void initMatrix (int64[][] %m, int64 %initValue){
  int64 %l1, %l2
  %l1 <- length %m 0
  %l2 <- length %m 1
  call print(%l1)
  call print(%l2)
  return
}
