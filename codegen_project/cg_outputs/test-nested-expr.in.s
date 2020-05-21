.data
.align 2
.text
main: 
addiu $sp, $sp, -24
sw $ra, 0($sp)
li $a0 0
sw $a0, 4($sp)
li $a0 0
sw $a0, 8($sp)
li $a0 0
sw $a0, 12($sp)
li $a0 0
sw $a0, 16($sp)
li $a0 0
sw $a0, 20($sp)
la $t1, 20($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $t1 0($sp)
addiu $sp $sp -0
jal input
addiu $sp, $sp 0
lw $t1 0($sp)
addiu $sp $sp 4
sw $a0 0($t1)
la $t1, 16($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $t1 0($sp)
addiu $sp $sp -0
jal input
addiu $sp, $sp 0
lw $t1 0($sp)
addiu $sp $sp 4
sw $a0 0($t1)
la $t1, 12($sp)
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
la $t1, 24($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 20($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 20($sp)
lw $a0, 0($t1)
lw $t1 0($sp)
add $a0 $t1 $a0
addiu $sp $sp 4
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 32($sp)
lw $a0, 0($t1)
lw $t1 0($sp)
sub $a0 $t1 $a0
addiu $sp $sp 4
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 32($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 32($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 28($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 36($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 48($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 40($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 48($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 56($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 64($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 56($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 60($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 72($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 72($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 84($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 84($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 84($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $a0 0($sp)
la $t1, 84($sp)
lw $a0, 0($t1)
lw $t1 0($sp)
mult $t1 $a0
mflo $a0
addiu $sp $sp 4
lw $t1 0($sp)
mult $t1 $a0
mflo $a0
addiu $sp $sp 4
lw $t1 0($sp)
mult $t1 $a0
mflo $a0
addiu $sp $sp 4
lw $t1 0($sp)
add $a0 $t1 $a0
addiu $sp $sp 4
lw $t1 0($sp)
sub $a0 $t1 $a0
addiu $sp $sp 4
lw $t1 0($sp)
mult $t1 $a0
mflo $a0
addiu $sp $sp 4
lw $t1 0($sp)
mult $t1 $a0
mflo $a0
addiu $sp $sp 4
lw $t1 0($sp)
div $t1 $a0
mflo $a0
addiu $sp $sp 4
lw $t1 0($sp)
mult $t1 $a0
mflo $a0
addiu $sp $sp 4
lw $t1 0($sp)
add $a0 $t1 $a0
addiu $sp $sp 4
lw $t1 0($sp)
sub $a0 $t1 $a0
addiu $sp $sp 4
lw $t1 0($sp)
mult $t1 $a0
mflo $a0
addiu $sp $sp 4
lw $t1 0($sp)
add $a0 $t1 $a0
addiu $sp $sp 4
lw $t1 0($sp)
sub $a0 $t1 $a0
addiu $sp $sp 4
lw $t1 0($sp)
div $t1 $a0
mflo $a0
addiu $sp $sp 4
lw $t1 0($sp)
mult $t1 $a0
mflo $a0
addiu $sp $sp 4
lw $t1 0($sp)
add $a0 $t1 $a0
addiu $sp $sp 4
lw $t1 0($sp)
mult $t1 $a0
mflo $a0
addiu $sp $sp 4
lw $t1 0($sp)
addiu $sp $sp 4
sw $a0 0($t1)
addiu $sp $sp -4
la $t1, 4($sp)
lw $a0, 0($t1)
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
.RET1:
lw $ra 0($sp)
addiu $sp $sp 24
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
