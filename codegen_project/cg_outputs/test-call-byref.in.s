.data
.align 2
g: .space 4
garr: .space 0
.text
bar: 
addiu $sp, $sp, -4
sw $ra, 0($sp)
addiu $sp $sp -4
li $a0 1
bgezal $a0, .ERR_1
j __crt_out_of_bounds
.ERR_1:
sll $a0, $a0, 2
la $t1, 16($sp)
add $t1, $t1, $a0
lw $a0, 0($t1)
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
li $a0 2
bgezal $a0, .ERR_2
j __crt_out_of_bounds
.ERR_2:
sll $a0, $a0, 2
la $t1, 16($sp)
add $t1, $t1, $a0
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
addiu $sp $sp -0
jal input
addiu $sp, $sp 0
lw $t1 0($sp)
addiu $sp $sp 4
sw $a0 0($t1)
.RET3:
lw $ra 0($sp)
addiu $sp $sp 4
jr $ra
foo: 
addiu $sp, $sp, -4
sw $ra, 0($sp)
addiu $sp $sp -4
li $a0 1
bgezal $a0, .ERR_3
j __crt_out_of_bounds
.ERR_3:
sll $a0, $a0, 2
la $t1, 12($sp)
add $t1, $t1, $a0
lw $a0, 0($t1)
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
li $a0 0
bgezal $a0, .ERR_4
j __crt_out_of_bounds
.ERR_4:
sll $a0, $a0, 2
la $t1, 12($sp)
add $t1, $t1, $a0
lw $a0, 0($t1)
addiu $sp $sp -4
sw $t1 0($sp)
addiu $sp $sp -0
jal input
addiu $sp, $sp 0
lw $t1 0($sp)
addiu $sp $sp 4
sw $a0 0($t1)
li $a0 1
bgezal $a0, .ERR_5
j __crt_out_of_bounds
.ERR_5:
sll $a0, $a0, 2
la $t1, 12($sp)
add $t1, $t1, $a0
lw $a0, 0($t1)
addiu $sp $sp -4
sw $t1 0($sp)
addiu $sp $sp -0
jal input
addiu $sp, $sp 0
lw $t1 0($sp)
addiu $sp $sp 4
sw $a0 0($t1)
addiu $sp $sp -16
la $t1, 12($sp)
lw $a0, 0($t1)
sw $a0 0($sp)
li $a0 1
bgezal $a0, .ERR_6
j __crt_out_of_bounds
.ERR_6:
sll $a0, $a0, 2
la $t1, 12($sp)
add $t1, $t1, $a0
lw $a0, 0($t1)
sw $a0 4($sp)
la $t1, 8($sp)
lw $a0, 0($t1)
sw $a0 8($sp)
la $t1, 4($sp)
lw $a0, 0($t1)
sw $a0 12($sp)
jal bar
addiu $sp, $sp 16
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
.RET4:
lw $ra 0($sp)
addiu $sp $sp 4
jr $ra
main: 
addiu $sp, $sp, -12
sw $ra, 0($sp)
li $a0 0
sw $a0, 4($sp)
li $a0, 16
li $v0, 9 #syscall 9 (sbrk)
syscall
add $a0, $v0, $0
sw $a0, 8($sp)
la $t1, 8($sp)
lw $a0, 0($t1)
addiu $sp $sp -4
sw $t1 0($sp)
li $a0 101
lw $t1 0($sp)
addiu $sp $sp 4
sw $a0 0($t1)
li $a0 0
bgezal $a0, .ERR_7
j __crt_out_of_bounds
.ERR_7:
sll $a0, $a0, 2
la $t1, 4($sp)
add $t1, $t1, $a0
lw $a0, 0($t1)
addiu $sp $sp -4
sw $t1 0($sp)
li $a0 102
lw $t1 0($sp)
addiu $sp $sp 4
sw $a0 0($t1)
li $a0 1
bgezal $a0, .ERR_8
j __crt_out_of_bounds
.ERR_8:
sll $a0, $a0, 2
la $t1, 4($sp)
add $t1, $t1, $a0
lw $a0, 0($t1)
addiu $sp $sp -4
sw $t1 0($sp)
li $a0 103
lw $t1 0($sp)
addiu $sp $sp 4
sw $a0 0($t1)
li $a0 2
bgezal $a0, .ERR_9
j __crt_out_of_bounds
.ERR_9:
sll $a0, $a0, 2
la $t1, 4($sp)
add $t1, $t1, $a0
lw $a0, 0($t1)
addiu $sp $sp -4
sw $t1 0($sp)
li $a0 104
lw $t1 0($sp)
addiu $sp $sp 4
sw $a0 0($t1)
li $a0 3
bgezal $a0, .ERR_10
j __crt_out_of_bounds
.ERR_10:
sll $a0, $a0, 2
la $t1, 4($sp)
add $t1, $t1, $a0
lw $a0, 0($t1)
addiu $sp $sp -4
sw $t1 0($sp)
li $a0 105
lw $t1 0($sp)
addiu $sp $sp 4
sw $a0 0($t1)
addiu $sp $sp -12
la $t1, 4($sp)
add $a0 $t1 $0
sw $a0 0($sp)
la $t1, 8($sp)
lw $a0, 0($t1)
sw $a0 4($sp)
li $a0 3
bgezal $a0, .ERR_11
j __crt_out_of_bounds
.ERR_11:
sll $a0, $a0, 2
la $t1, 4($sp)
add $t1, $t1, $a0
lw $a0, 0($t1)
sw $a0 8($sp)
jal foo
addiu $sp, $sp 12
addiu $sp $sp -4
li $a0 0
bgezal $a0, .ERR_12
j __crt_out_of_bounds
.ERR_12:
sll $a0, $a0, 2
la $t1, 4($sp)
add $t1, $t1, $a0
lw $a0, 0($t1)
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
addiu $sp $sp -4
li $a0 1
bgezal $a0, .ERR_13
j __crt_out_of_bounds
.ERR_13:
sll $a0, $a0, 2
la $t1, 4($sp)
add $t1, $t1, $a0
lw $a0, 0($t1)
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
addiu $sp $sp -4
li $a0 2
bgezal $a0, .ERR_14
j __crt_out_of_bounds
.ERR_14:
sll $a0, $a0, 2
la $t1, 4($sp)
add $t1, $t1, $a0
lw $a0, 0($t1)
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
addiu $sp $sp -4
li $a0 3
bgezal $a0, .ERR_15
j __crt_out_of_bounds
.ERR_15:
sll $a0, $a0, 2
la $t1, 4($sp)
add $t1, $t1, $a0
lw $a0, 0($t1)
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
addiu $sp $sp -4
la $t1, 8($sp)
lw $a0, 0($t1)
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
la $t1, g
lw $a0, 0($t1)
addiu $sp $sp -4
sw $t1 0($sp)
li $a0 201
lw $t1 0($sp)
addiu $sp $sp 4
sw $a0 0($t1)
li $a0 0
bgezal $a0, .ERR_16
j __crt_out_of_bounds
.ERR_16:
sll $a0, $a0, 2
la $t1, garr
add $t1, $t1, $a0
lw $a0, 0($t1)
addiu $sp $sp -4
sw $t1 0($sp)
li $a0 202
lw $t1 0($sp)
addiu $sp $sp 4
sw $a0 0($t1)
li $a0 1
bgezal $a0, .ERR_17
j __crt_out_of_bounds
.ERR_17:
sll $a0, $a0, 2
la $t1, garr
add $t1, $t1, $a0
lw $a0, 0($t1)
addiu $sp $sp -4
sw $t1 0($sp)
li $a0 203
lw $t1 0($sp)
addiu $sp $sp 4
sw $a0 0($t1)
li $a0 2
bgezal $a0, .ERR_18
j __crt_out_of_bounds
.ERR_18:
sll $a0, $a0, 2
la $t1, garr
add $t1, $t1, $a0
lw $a0, 0($t1)
addiu $sp $sp -4
sw $t1 0($sp)
li $a0 204
lw $t1 0($sp)
addiu $sp $sp 4
sw $a0 0($t1)
li $a0 3
bgezal $a0, .ERR_19
j __crt_out_of_bounds
.ERR_19:
sll $a0, $a0, 2
la $t1, garr
add $t1, $t1, $a0
lw $a0, 0($t1)
addiu $sp $sp -4
sw $t1 0($sp)
li $a0 205
lw $t1 0($sp)
addiu $sp $sp 4
sw $a0 0($t1)
addiu $sp $sp -12
la $t1, garr
add $a0 $t1 $0
sw $a0 0($sp)
la $t1, g
lw $a0, 0($t1)
sw $a0 4($sp)
li $a0 3
bgezal $a0, .ERR_20
j __crt_out_of_bounds
.ERR_20:
sll $a0, $a0, 2
la $t1, garr
add $t1, $t1, $a0
lw $a0, 0($t1)
sw $a0 8($sp)
jal foo
addiu $sp, $sp 12
addiu $sp $sp -4
li $a0 0
bgezal $a0, .ERR_21
j __crt_out_of_bounds
.ERR_21:
sll $a0, $a0, 2
la $t1, garr
add $t1, $t1, $a0
lw $a0, 0($t1)
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
addiu $sp $sp -4
li $a0 1
bgezal $a0, .ERR_22
j __crt_out_of_bounds
.ERR_22:
sll $a0, $a0, 2
la $t1, garr
add $t1, $t1, $a0
lw $a0, 0($t1)
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
addiu $sp $sp -4
li $a0 2
bgezal $a0, .ERR_23
j __crt_out_of_bounds
.ERR_23:
sll $a0, $a0, 2
la $t1, garr
add $t1, $t1, $a0
lw $a0, 0($t1)
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
addiu $sp $sp -4
li $a0 3
bgezal $a0, .ERR_24
j __crt_out_of_bounds
.ERR_24:
sll $a0, $a0, 2
la $t1, garr
add $t1, $t1, $a0
lw $a0, 0($t1)
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
addiu $sp $sp -4
la $t1, g
lw $a0, 0($t1)
sw $a0 0($sp)
jal output
addiu $sp, $sp 4
.RET5:
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
