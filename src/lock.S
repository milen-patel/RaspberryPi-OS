.global atomic_decrement
atomic_decrement:
  ldxr x0, [x0]
  sub x0, x0, #1
  stlxr w1, x0, [x0]
  cbnz w1, atomic_decrement
  ret