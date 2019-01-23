void main ( ){
  int64 %p0, %p1, %p2, %p3, %p4, %p5, %p6, %p7, %p8

  %p0 <- 1
  %p1 <- 2
  %p2 <- 3
  %p3 <- 4
  %p4 <- 5
  %p5 <- 6
  %p6 <- 7
  %p7 <- 8
  %p8 <- 9

  call printValues(%p0, %p1, %p2, %p3, %p4, %p5, %p6, %p7, %p8)

  return
}

void printValues (int64 %p0, int64 %p1, int64 %p2, int64 %p3, int64 %p4, int64 %p5, int64 %p6, int64 %p7, int64 %p8){

  call print(%p0)
  call print(%p1)
  call print(%p2)
  call print(%p3)
  call print(%p4)
  call print(%p5)
  call print(%p6)
  call print(%p7)
  call print(%p8)

  return
}
