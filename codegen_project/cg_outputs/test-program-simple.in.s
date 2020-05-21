.data
.align 2
.text
main: 
addiu $sp, $sp, -4
sw $ra, 0($sp)
addiu $sp $sp -4
li $a0 4
addiu $sp $sp -4
sw $a0 0($sp)
li $a0 5
lw $t1 0($sp)
add $a0 $t1 $a0
addiu $sp $sp 4
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
.RET1:
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
