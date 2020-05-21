.data
.align 2
x: .space 4
.text
setglobalx: 
addiu $sp, $sp, -4
sw $ra, 0($sp)
la $t1, x
lw $a0, 0($t1)
addiu $sp $sp -4
sw $t1 0($sp)
li $a0 10
lw $t1 0($sp)
addiu $sp $sp 4
sw $a0 0($t1)
.RET2:
lw $ra 0($sp)
addiu $sp $sp 4
jr $ra
printglobalx: 
addiu $sp, $sp, -4
sw $ra, 0($sp)
addiu $sp $sp -4
la $t1, x
lw $a0, 0($t1)
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
.RET3:
lw $ra 0($sp)
addiu $sp $sp 4
jr $ra
foo: 
addiu $sp, $sp, -4
sw $ra, 0($sp)
addiu $sp $sp -4
la $t1, 4($sp)
lw $a0, 0($t1)
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
addiu $sp $sp -4
la $t1, 4($sp)
lw $a0, 0($t1)
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
.RET4:
lw $ra 0($sp)
addiu $sp $sp 4
jr $ra
main: 
addiu $sp, $sp, -8
sw $ra, 0($sp)
li $a0 0
sw $a0, 4($sp)
addiu $sp $sp -0
jal setglobalx
addiu $sp, $sp 0
la $t1, 4($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $t1 0($sp)
li $a0 100
lw $t1 0($sp)
addiu $sp $sp 4
sw $a0 0($t1)
addiu $sp $sp -0
jal printglobalx
addiu $sp, $sp 0
addiu $sp $sp -4
li $a0 1000
sw $a0 0($sp)
jal foo
addiu $sp, $sp 4
addiu $sp $sp -4
la $t1, 4($sp)
lw $a0, 0($t1)
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
addiu $sp $sp -0
jal printglobalx
addiu $sp, $sp 0
.RET5:
lw $ra 0($sp)
addiu $sp $sp 8
jr $ra

.text
.globl __crt_out_of_bounds
.globl output
.globl input

__crt_out_of_bounds: 
li $v0, 10 # exit
syscall
output: 
li $v0, 1 # print_int
syscall
li $a0, 0x0a
li $v0, 11 #print_char
syscall
jr $ra
input: 
li $v0, 5 #read_int
syscall
move $a0, $v0
jr $ra
