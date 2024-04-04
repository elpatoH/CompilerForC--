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

.globl main
main:
    addiu $sp, $sp, -8     # 2 slots for fp and ra
    sw $fp, 4($sp)         # Save the old frame pointer
    sw $ra, 0($sp)         # Save the return address
    move $fp, $sp          # Set the new frame pointer
    addiu $sp, $sp, -112    # tmps: 28, locals: 0

    li $t0, 1
    sw $t0, -4($fp)

    li $t0, 1
    sw $t0, -8($fp)

	lw $t3, -4($fp)
	lw $t4, -8($fp)
	seq $t5, $t3, $t4
	bnez $t5, label_0
	b label_1

label_0:
    li $t0, 1
    sw $t0, -12($fp)

    lw $t0, -12($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal println
    addiu $sp, $sp, 4

    li $t0, 1
    sw $t0, -16($fp)

    li $t0, 1
    sw $t0, -20($fp)

	lw $t3, -16($fp)
	lw $t4, -20($fp)
	seq $t5, $t3, $t4
	bnez $t5, label_3
	b label_4

label_3:
    li $t0, 2
    sw $t0, -24($fp)

    lw $t0, -24($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal println
    addiu $sp, $sp, 4

	b label_5

label_4:

label_5:
    li $t0, 1
    sw $t0, -28($fp)

    li $t0, 2
    sw $t0, -32($fp)

	lw $t3, -28($fp)
	lw $t4, -32($fp)
	seq $t5, $t3, $t4
	bnez $t5, label_6
	b label_7

label_6:
    li $t0, 1
    sw $t0, -36($fp)

    li $t0, 2
    sw $t0, -40($fp)

	lw $t3, -36($fp)
	lw $t4, -40($fp)
	seq $t5, $t3, $t4
	bnez $t5, label_9
	b label_10

label_9:
    li $t0, 0
    sw $t0, -44($fp)

    lw $t0, -44($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal println
    addiu $sp, $sp, 4

	b label_11

label_10:
    li $t0, 1
    sw $t0, -48($fp)

    li $t0, 2
    sw $t0, -52($fp)

	lw $t3, -48($fp)
	lw $t4, -52($fp)
	seq $t5, $t3, $t4
	bnez $t5, label_12
	b label_13

label_12:
    li $t0, 0
    sw $t0, -56($fp)

    lw $t0, -56($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal println
    addiu $sp, $sp, 4

	b label_14

label_13:
    li $t0, 0
    sw $t0, -60($fp)

    lw $t0, -60($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal println
    addiu $sp, $sp, 4


label_14:

label_11:
	b label_8

label_7:
    li $t0, 1
    sw $t0, -64($fp)

    li $t0, 2
    sw $t0, -68($fp)

	lw $t3, -64($fp)
	lw $t4, -68($fp)
	seq $t5, $t3, $t4
	bnez $t5, label_15
	b label_16

label_15:
    li $t0, 0
    sw $t0, -72($fp)

    lw $t0, -72($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal println
    addiu $sp, $sp, 4

	b label_17

label_16:
    li $t0, 1
    sw $t0, -76($fp)

    li $t0, 1
    sw $t0, -80($fp)

	lw $t3, -76($fp)
	lw $t4, -80($fp)
	seq $t5, $t3, $t4
	bnez $t5, label_18
	b label_19

label_18:
    li $t0, 3
    sw $t0, -84($fp)

    lw $t0, -84($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal println
    addiu $sp, $sp, 4

	b label_20

label_19:
    li $t0, 1
    sw $t0, -88($fp)

    li $t0, 0
    sw $t0, -92($fp)

	lw $t3, -88($fp)
	lw $t4, -92($fp)
	seq $t5, $t3, $t4
	bnez $t5, label_21
	b label_22

label_21:
    li $t0, 0
    sw $t0, -96($fp)

    lw $t0, -96($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal println
    addiu $sp, $sp, 4

	b label_23

label_22:
    li $t0, 0
    sw $t0, -100($fp)

    lw $t0, -100($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal println
    addiu $sp, $sp, 4


label_23:

label_20:

label_17:

label_8:
    li $t0, 4
    sw $t0, -104($fp)

    lw $t0, -104($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal println
    addiu $sp, $sp, 4

	b label_2

label_1:
    li $t0, 0
    sw $t0, -108($fp)

    lw $t0, -108($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal println
    addiu $sp, $sp, 4


label_2:
    li $t0, 5
    sw $t0, -112($fp)

    lw $t0, -112($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal println
    addiu $sp, $sp, 4


    move $sp, $fp
    lw $ra, 0($sp)
    lw $fp, 4($sp)
    addiu $sp, $sp, 120
li $v0, 10
syscall
