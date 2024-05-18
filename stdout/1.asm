.data
_nl: .asciiz "\n"
.text
.globl _println
_println:
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

.globl _factorial
_factorial:
    addiu $sp, $sp, -8     # 2 slots for fp and ra
    sw $fp, 4($sp)         # Save the old frame pointer
    sw $ra, 0($sp)         # Save the return address
    move $fp, $sp          # Set the new frame pointer
    addiu $sp, $sp, -28    # tmps: 6, locals: 1

    lw $t0, 8($fp) #useless?
    li $t0, 0
    sw $t0, -4($fp)

	lw $t3, 8($fp) #useless?
	lw $t4, -4($fp)
	sle $t5, $t3, $t4
	bnez $t5, label_0
	b label_1

label_0:
    li $t0, 1
    sw $t0, -8($fp)

	lw $v0, -8($fp)

    move $sp, $fp
    lw $ra, 0($sp)
    lw $fp, 4($sp)
    addiu $sp, $sp, 32
jr $ra

	b label_2

label_1:
    lw $t0, 8($fp) #useless?
	lw $t5, 8($fp) #useless?
	lw $t6, -12($fp)
	sub $t7, $t5, $t6
	sw $t7, -16($fp)

	lw $t0, -16($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    li $t0, 1
    sw $t0, -12($fp)

    lw $t0, 8($fp) #useless?
    jal _factorial
    addiu $sp, $sp, 4

	add $t0, $zero, $v0
	sw $t0 -20($fp)
	lw $t5, 8($fp) #useless?
	lw $t6, -20($fp)
	mulo $t7, $t5, $t6
	sw $t7, -24($fp)

	lw $v0, -24($fp)

    move $sp, $fp
    lw $ra, 0($sp)
    lw $fp, 4($sp)
    addiu $sp, $sp, 32
jr $ra


label_2:

    move $sp, $fp
    lw $ra, 0($sp)
    lw $fp, 4($sp)
    addiu $sp, $sp, 32
jr $ra

.globl main
main:
    addiu $sp, $sp, -8     # 2 slots for fp and ra
    sw $fp, 4($sp)         # Save the old frame pointer
    sw $ra, 0($sp)         # Save the return address
    move $fp, $sp          # Set the new frame pointer
    addiu $sp, $sp, -36    # tmps: 6, locals: 3

    li $t0, 7
    sw $t0, -4($fp)

    lw $t1, -4($fp) # name: Temp_0
    sw $t1, -8($fp) # popo
    li $t0, 0
    sw $t0, -12($fp)

    lw $t1, -12($fp) # name: Temp_2
    sw $t1, -40($fp) # popo

label_3:
	lw $t3, -40($fp)
	lw $t4, -8($fp)
	sle $t5, $t3, $t4
	bnez $t5, label_4
	b label_5

label_4:
	lw $t0, -40($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal _factorial
    addiu $sp, $sp, 4

	add $t0, $zero, $v0
	sw $t0 -20($fp)
    lw $t1, -20($fp) # name: Temp_4
    sw $t1, -24($fp) # popo
	lw $t0, -24($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal _println
    addiu $sp, $sp, 4

	add $t0, $zero, $v0
	sw $t0 -28($fp)
    li $t0, 1
    sw $t0, -32($fp)

	lw $t5, -40($fp)
	lw $t6, -32($fp)
	add $t7, $t5, $t6
	sw $t7, -36($fp)

    lw $t1, -36($fp) # name: Temp_8
    sw $t1, -40($fp) # popo
	b label_3

label_5:

    move $sp, $fp
    lw $ra, 0($sp)
    lw $fp, 4($sp)
    addiu $sp, $sp, 28
li $v0, 10
syscall
