.data
.align 2
pad1: .space 0
arr: .space 0
pad2: .space 0
.text
main: 
addiu $sp, $sp, -4
sw $ra, 0($sp)
addiu $sp $sp -4
li $a0 0
addiu $sp $sp -4
sw $a0 0($sp)
li $a0 1
lw $t1 0($sp)
sub $a0 $t1 $a0
addiu $sp $sp 4
bgezal $a0, .ERR_1
j __crt_out_of_bounds
.ERR_1:
sll $a0, $a0, 2
la $t1, arr
add $t1, $t1, $a0
lw $a0, 0($t1)
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
.RET4:
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
