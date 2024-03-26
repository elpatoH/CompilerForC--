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

.globl g
g:
    addiu $sp, $sp, -8    # tmps: 0, params: 1, 1
    sw $fp, 4($sp)         # Save the old frame pointer
    sw $ra, 0($sp)         # Save the return address
    move $fp, $sp          # Set the new frame pointer

    lw $t0, 8($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal println
    addiu $sp, $sp 4


    move $sp, $fp
    lw $ra, 0($sp)
    lw $fp, 4($sp)
    addiu $sp, $sp, 12
jr $ra

.globl f
f:
    addiu $sp, $sp, -8    # tmps: 0, params: 1, 1
    sw $fp, 4($sp)         # Save the old frame pointer
    sw $ra, 0($sp)         # Save the return address
    move $fp, $sp          # Set the new frame pointer

    lw $t0, 8($fp)
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal g
    addiu $sp, $sp 4


    move $sp, $fp
    lw $ra, 0($sp)
    lw $fp, 4($sp)
    addiu $sp, $sp, 12
jr $ra

.globl main
main:
    addiu $sp, $sp, -12    # tmps: 1, params: 0, 0
    sw $fp, 4($sp)         # Save the old frame pointer
    sw $ra, 0($sp)         # Save the return address
    move $fp, $sp          # Set the new frame pointer

    li $t0, 34567
    addiu $sp, $sp, -4
    sw $t0, 0($sp)

    jal f
    addiu $sp, $sp 4


    move $sp, $fp
    lw $ra, 0($sp)
    lw $fp, 4($sp)
    addiu $sp, $sp, 12
jr $ra

