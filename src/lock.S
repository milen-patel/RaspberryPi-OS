.global TestAndSet
TestAndSet:
    // Let x0 - memory address
loop:
  ldxr x0, [x1]
  cmp w0, #0
  bne end_loop
  mov w2, #1
  stxr w3, w2, [x1]
  cmp w3, #0
  bne loop

end_loop: