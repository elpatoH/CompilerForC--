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
    addiu $sp, $sp, -24    # tmps: 6, locals: 0

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
    li $t0, 3
    sw $t0, -12($fp)

    li $t0, 3
    sw $t0, -16($fp)

	lw $t3, -12($fp)
	lw $t4, -16($fp)
	seq $t5, $t3, $t4
	bnez $t5, label_3
	b label_4

label_3:
    li $t0, 1
    sw $t0, -20($fp)

    lw $t0, -20($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal println
    addiu $sp, $sp, 4

	b label_5

label_4:
    li $t0, 3
    sw $t0, -24($fp)

    lw $t0, -24($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal println
    addiu $sp, $sp, 4


label_5:
	b label_2

label_1:

label_2:

    move $sp, $fp
    lw $ra, 0($sp)
    lw $fp, 4($sp)
    addiu $sp, $sp, 28
li $v0, 10
syscall
