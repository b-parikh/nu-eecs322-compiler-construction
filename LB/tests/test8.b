void main ( ){
  int64 %v
  %v <- 1

  call print(%v)

  {
    int64 %v
    %v <- 2
    call print(%v)
  }
  
  call print(%v)

  return

}
