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
    addiu $sp, $sp, -28    # tmps: 7, locals: 0

    li $t0, 7
    sw $t0, -4($fp)

    lw $t1, -4($fp)
    sw $t1, x  # : x :


label_0:
    li $t0, 1
    sw $t0, -8($fp)

	lw $t3, x
	lw $t4, -8($fp)
	sgt $t5, $t3, $t4
	bnez $t5, label_1
	b label_2

label_1:
    li $t0, 2581104
    sw $t0, -12($fp)

    lw $t0, -12($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal println
    addiu $sp, $sp, 4

    li $t0, 9
    sw $t0, -16($fp)

	lw $t3, x
	lw $t4, -16($fp)
	sgt $t5, $t3, $t4
	bnez $t5, label_3
	b label_4

label_3:
    li $t0, 9
    sw $t0, -20($fp)

    lw $t1, -20($fp)
    sw $t1, x  # : x :

	b label_5

label_4:
    li $t0, 8
    sw $t0, -24($fp)

	lw $t3, x
	lw $t4, -24($fp)
	sgt $t5, $t3, $t4
	bnez $t5, label_6
	b label_7

label_6:
    li $t0, 0
    sw $t0, -28($fp)

    lw $t1, -28($fp)
    sw $t1, x  # : x :

	b label_8

label_7:

    move $sp, $fp
    lw $ra, 0($sp)
    lw $fp, 4($sp)
    addiu $sp, $sp, 36
li $v0, 10
syscall

label_8:

label_5:
	b label_0

label_2:
    lw $t0, x
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal println
    addiu $sp, $sp, 4


    move $sp, $fp
    lw $ra, 0($sp)
    lw $fp, 4($sp)
    addiu $sp, $sp, 36
li $v0, 10
syscall
