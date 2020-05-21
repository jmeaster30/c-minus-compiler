.data
.align 2
.text
foo: 
addiu $sp, $sp, -12
sw $ra, 0($sp)
li $a0 0
sw $a0, 4($sp)
li $a0 0
sw $a0, 8($sp)
la $t1, 16($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
li $a0 0
lw $t1 0($sp)
seq $a0 $t1 $a0
addiu $sp $sp 4
bne $a0 $0 true_branch_1
b end_if_1
true_branch_1:
li $a0 42
j .RET1
end_if_1:
la $t1, 8($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $t1 0($sp)
la $t1, 20($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 20($sp)
lw $a0, 0($t1)
lw $t1 0($sp)
mult $t1 $a0
mflo $a0
addiu $sp $sp 4
lw $t1 0($sp)
addiu $sp $sp 4
sw $a0 0($t1)
la $t1, 4($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $t1 0($sp)
la $t1, 20($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 20($sp)
lw $a0, 0($t1)
lw $t1 0($sp)
add $a0 $t1 $a0
addiu $sp $sp 4
lw $t1 0($sp)
addiu $sp $sp 4
sw $a0 0($t1)
la $t1, 4($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 12($sp)
lw $a0, 0($t1)
lw $t1 0($sp)
mult $t1 $a0
mflo $a0
addiu $sp $sp 4
j .RET1
.RET1:
lw $ra 0($sp)
addiu $sp $sp 12
jr $ra
main: 
addiu $sp, $sp, -4
sw $ra, 0($sp)
addiu $sp $sp -4
addiu $sp $sp -8
li $a0 0
sw $a0 0($sp)
li $a0 20
sw $a0 4($sp)
jal foo
addiu $sp, $sp 8
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
addiu $sp $sp -4
addiu $sp $sp -8
li $a0 10
sw $a0 0($sp)
li $a0 20
sw $a0 4($sp)
jal foo
addiu $sp, $sp 8
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
.RET2:
lw $ra 0($sp)
addiu $sp $sp 4
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
