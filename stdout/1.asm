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
    addiu $sp, $sp, -20    # tmps: 4, locals: 1

    li $t0, 123
    sw $t0, -4($fp)

    li $t0, 123
    sw $t0, -12($fp)

	lw $t5, -4($fp)
	lw $t6, -12($fp)
	sub $t7, $t5, $t6
	sw $t7, -16($fp)

    lw $t1, -16($fp) # name: Temp_3
    sw $t1, -20($fp) # popo
    lw $t0, -20($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal _println
    addiu $sp, $sp, 4


    move $sp, $fp
    lw $ra, 0($sp)
    lw $fp, 4($sp)
    addiu $sp, $sp, 20
li $v0, 10
syscall
