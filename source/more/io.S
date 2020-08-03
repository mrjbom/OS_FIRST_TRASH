.global setl,
.type setl, @function
setl:
  movl 4(%esp), %eax
  movl 8(%esp), %ecx
  movl %ecx, (%eax)
  ret
.size setl,.-setl