.data
.align 2
.text
main: 
addiu $sp, $sp, -12
sw $ra, 0($sp)
li $a0 0
sw $a0, 4($sp)
li $a0 0
sw $a0, 8($sp)
la $t1, 8($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $t1 0($sp)
addiu $sp $sp -0
jal input
addiu $sp, $sp 0
lw $t1 0($sp)
addiu $sp $sp 4
sw $a0 0($t1)
la $t1, 4($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $t1 0($sp)
addiu $sp $sp -0
jal input
addiu $sp, $sp 0
lw $t1 0($sp)
addiu $sp $sp 4
sw $a0 0($t1)
la $t1, 8($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 8($sp)
lw $a0, 0($t1)
lw $t1 0($sp)
seq $a0 $t1 $a0
addiu $sp $sp 4
bne $a0 $0 true_branch_1
addiu $sp $sp -4
li $a0 2
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
b end_if_1
true_branch_1:
addiu $sp $sp -4
li $a0 1
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
end_if_1:
addiu $sp $sp -4
li $a0 3
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
la $t1, 8($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 8($sp)
lw $a0, 0($t1)
lw $t1 0($sp)
sne $a0 $t1 $a0
addiu $sp $sp 4
bne $a0 $0 true_branch_2
addiu $sp $sp -4
li $a0 5
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
b end_if_2
true_branch_2:
addiu $sp $sp -4
li $a0 4
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
end_if_2:
addiu $sp $sp -4
li $a0 6
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
la $t1, 8($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 8($sp)
lw $a0, 0($t1)
lw $t1 0($sp)
seq $a0 $t1 $a0
addiu $sp $sp 4
bne $a0 $0 true_branch_3
b end_if_3
true_branch_3:
addiu $sp $sp -4
li $a0 7
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
end_if_3:
addiu $sp $sp -4
li $a0 8
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
la $t1, 8($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 8($sp)
lw $a0, 0($t1)
lw $t1 0($sp)
sne $a0 $t1 $a0
addiu $sp $sp 4
bne $a0 $0 true_branch_4
b end_if_4
true_branch_4:
addiu $sp $sp -4
li $a0 9
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
end_if_4:
addiu $sp $sp -4
li $a0 10
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
.RET1:
lw $ra 0($sp)
addiu $sp $sp 12
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
