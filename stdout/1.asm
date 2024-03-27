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


.text

.globl main
main:
    addiu $sp, $sp, -8     # 2 slots for fp and ra
    sw $fp, 4($sp)         # Save the old frame pointer
    sw $ra, 0($sp)         # Save the return address
    move $fp, $sp          # Set the new frame pointer
    addiu $sp, $sp, -4    # tmps: 1, locals: 0

    li $t0, 3456
    sw $t0, -4($fp)

    lw $t0, -4($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal println
    addiu $sp, $sp 4


    move $sp, $fp
    lw $ra, 0($sp)
    lw $fp, 4($sp)
    addiu $sp, $sp, 12
jr $ra

