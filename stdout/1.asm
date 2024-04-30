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

.globl main
main:
    addiu $sp, $sp, -8     # 2 slots for fp and ra
    sw $fp, 4($sp)         # Save the old frame pointer
    sw $ra, 0($sp)         # Save the return address
    move $fp, $sp          # Set the new frame pointer
    addiu $sp, $sp, -24    # tmps: 5, locals: 1

    li $t0, 100
    sw $t0, -4($fp)

    lw $t1, -4($fp) # name: Temp_0
    sw $t1, -28($fp) # popo
    li $t0, 1000
    sw $t0, -12($fp)

	lw $t3, -28($fp)
	lw $t4, -12($fp)
	slt $t5, $t3, $t4
	bnez $t5, label_0
	b label_3

label_3:
    li $t0, 10
    sw $t0, -16($fp)

    li $t0, 1
    sw $t0, -20($fp)

	lw $t3, -16($fp)
	lw $t4, -20($fp)
	sgt $t5, $t3, $t4
	bnez $t5, label_0
	b label_1

label_0:
    li $t0, 9
    sw $t0, -24($fp)

    lw $t1, -24($fp) # name: Temp_5
    sw $t1, -28($fp) # popo
	b label_2

label_1:

label_2:
    lw $t0, -28($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal _println
    addiu $sp, $sp, 4


    move $sp, $fp
    lw $ra, 0($sp)
    lw $fp, 4($sp)
    addiu $sp, $sp, 24
li $v0, 10
syscall
