.data
_nl: .asciiz "\n"
.text
.globl println
println:
	addiu $sp, $sp, -8
	sw $fp, 4($sp)
	sw $ra, 0($sp)
	move $fp, $sp
	lw   $a0, 8($fp)
	li   $v0, 1
	syscall
	la   $a0, _nl
	li   $v0, 4
	syscall
	move $sp, $fp
	lw $ra, 0($sp)
	lw $fp, 4($sp)
	addiu $sp, $sp, 8
jr   $ra

.data
u : .word 0
x : .word 0

.text

.globl main
main:
    addiu $sp, $sp, -8     # 2 slots for fp and ra
    sw $fp, 4($sp)         # Save the old frame pointer
    sw $ra, 0($sp)         # Save the return address
    move $fp, $sp          # Set the new frame pointer
    addiu $sp, $sp, -32    # tmps: 6, locals: 2

    li $t0, 1
    sw $t0, -4($fp)

    lw $t1, -4($fp)
    sw $t1, x  # : x :

    li $t0, 0
    sw $t0, -8($fp)

    lw $t1, -8($fp)
    sw $t1, -12($fp)  # : y :

    li $t0, 123
    sw $t0, -16($fp)

    lw $t1, -16($fp)
    sw $t1, u  # : u :

    li $t0, 456
    sw $t0, -20($fp)

    lw $t1, -20($fp)
    sw $t1, -24($fp)  # : v :

	lw $t3, x
	lw $t4, -12($fp)
	sgt $t5, $t3, $t4
	bnez $t5, label_0
	b label_1

label_0:
    lw $t0, u
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal println
    addiu $sp, $sp, 4

	b label_2

label_1:
    lw $t0, -24($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal println
    addiu $sp, $sp, 4


label_2:

label_3:
    li $t0, 0
    sw $t0, -28($fp)

	lw $t3, x
	lw $t4, -28($fp)
	sgt $t5, $t3, $t4
	bnez $t5, label_4
	b label_5

label_4:
    li $t0, 2581104
    sw $t0, -32($fp)

    lw $t0, -32($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal println
    addiu $sp, $sp, 4

	b label_3

label_5:
    lw $t0, x
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal println
    addiu $sp, $sp, 4

    lw $t0, -12($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal println
    addiu $sp, $sp, 4


    move $sp, $fp
    lw $ra, 0($sp)
    lw $fp, 4($sp)
    addiu $sp, $sp, 32
li $v0, 10
syscall
