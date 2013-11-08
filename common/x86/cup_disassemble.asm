
cpu-a.o:     file format elf32-i386


Disassembly of section .text:

00000000 <x265_cpu_cpuid>:
   0:	53                   	push   %ebx
   1:	56                   	push   %esi
   2:	57                   	push   %edi
   3:	55                   	push   %ebp
   4:	8b 44 24 14          	mov    0x14(%esp),%eax
   8:	8b 4c 24 18          	mov    0x18(%esp),%ecx
   c:	8b 54 24 1c          	mov    0x1c(%esp),%edx
  10:	8b 5c 24 20          	mov    0x20(%esp),%ebx
  14:	8b 74 24 24          	mov    0x24(%esp),%esi
  18:	53                   	push   %ebx
  19:	56                   	push   %esi
  1a:	53                   	push   %ebx
  1b:	52                   	push   %edx
  1c:	51                   	push   %ecx
  1d:	89 c0                	mov    %eax,%eax
  1f:	31 c9                	xor    %ecx,%ecx
  21:	0f a2                	cpuid  
  23:	5e                   	pop    %esi
  24:	89 06                	mov    %eax,(%esi)
  26:	5e                   	pop    %esi
  27:	89 1e                	mov    %ebx,(%esi)
  29:	5e                   	pop    %esi
  2a:	89 0e                	mov    %ecx,(%esi)
  2c:	5e                   	pop    %esi
  2d:	89 16                	mov    %edx,(%esi)
  2f:	5b                   	pop    %ebx
  30:	5d                   	pop    %ebp
  31:	5f                   	pop    %edi
  32:	5e                   	pop    %esi
  33:	5b                   	pop    %ebx
  34:	c3                   	ret    
  35:	0f 1f 44 00 00       	nopl   0x0(%eax,%eax,1)
  3a:	66 0f 1f 44 00 00    	nopw   0x0(%eax,%eax,1)

00000040 <x265_cpu_xgetbv>:
  40:	53                   	push   %ebx
  41:	56                   	push   %esi
  42:	57                   	push   %edi
  43:	55                   	push   %ebp
  44:	8b 44 24 14          	mov    0x14(%esp),%eax
  48:	8b 4c 24 18          	mov    0x18(%esp),%ecx
  4c:	8b 54 24 1c          	mov    0x1c(%esp),%edx
  50:	52                   	push   %edx
  51:	51                   	push   %ecx
  52:	89 c1                	mov    %eax,%ecx
  54:	0f 01 d0             	xgetbv 
  57:	5e                   	pop    %esi
  58:	89 06                	mov    %eax,(%esi)
  5a:	5e                   	pop    %esi
  5b:	89 16                	mov    %edx,(%esi)
  5d:	5d                   	pop    %ebp
  5e:	5f                   	pop    %edi
  5f:	5e                   	pop    %esi
  60:	5b                   	pop    %ebx
  61:	c3                   	ret    
  62:	0f 1f 80 00 00 00 00 	nopl   0x0(%eax)
  69:	0f 1f 80 00 00 00 00 	nopl   0x0(%eax)

00000070 <x265_cpu_cpuid_test>:
  70:	9c                   	pushf  
  71:	53                   	push   %ebx
  72:	55                   	push   %ebp
  73:	56                   	push   %esi
  74:	57                   	push   %edi
  75:	9c                   	pushf  
  76:	58                   	pop    %eax
  77:	89 c3                	mov    %eax,%ebx
  79:	35 00 00 20 00       	xor    $0x200000,%eax
  7e:	50                   	push   %eax
  7f:	9d                   	popf   
  80:	9c                   	pushf  
  81:	58                   	pop    %eax
  82:	31 d8                	xor    %ebx,%eax
  84:	5f                   	pop    %edi
  85:	5e                   	pop    %esi
  86:	5d                   	pop    %ebp
  87:	5b                   	pop    %ebx
  88:	9d                   	popf   
  89:	c3                   	ret    
  8a:	66 0f 1f 44 00 00    	nopw   0x0(%eax,%eax,1)

00000090 <x265_stack_align>:
  90:	55                   	push   %ebp
  91:	89 e5                	mov    %esp,%ebp
  93:	83 ec 0c             	sub    $0xc,%esp
  96:	83 e4 f0             	and    $0xfffffff0,%esp
  99:	8b 4d 08             	mov    0x8(%ebp),%ecx
  9c:	8b 55 0c             	mov    0xc(%ebp),%edx
  9f:	89 14 24             	mov    %edx,(%esp)
  a2:	8b 55 10             	mov    0x10(%ebp),%edx
  a5:	89 54 24 04          	mov    %edx,0x4(%esp)
  a9:	8b 55 14             	mov    0x14(%ebp),%edx
  ac:	89 54 24 08          	mov    %edx,0x8(%esp)
  b0:	ff d1                	call   *%ecx
  b2:	c9                   	leave  
  b3:	c3                   	ret    
  b4:	66 0f 1f 44 00 00    	nopw   0x0(%eax,%eax,1)
  ba:	66 0f 1f 44 00 00    	nopw   0x0(%eax,%eax,1)

000000c0 <x265_cpu_emms>:
  c0:	0f 77                	emms   
  c2:	c3                   	ret    
  c3:	66 0f 1f 44 00 00    	nopw   0x0(%eax,%eax,1)
  c9:	0f 1f 80 00 00 00 00 	nopl   0x0(%eax)

000000d0 <x265_cpu_sfence>:
  d0:	0f ae f8             	sfence 
  d3:	c3                   	ret    
  d4:	66 0f 1f 44 00 00    	nopw   0x0(%eax,%eax,1)
  da:	66 0f 1f 44 00 00    	nopw   0x0(%eax,%eax,1)

000000e0 <x265_cpu_mask_misalign_sse>:
  e0:	83 ec 04             	sub    $0x4,%esp
  e3:	0f ae 1c 24          	stmxcsr (%esp)
  e7:	81 0c 24 00 00 02 00 	orl    $0x20000,(%esp)
  ee:	0f ae 14 24          	ldmxcsr (%esp)
  f2:	83 c4 04             	add    $0x4,%esp
  f5:	c3                   	ret    
