#Tommy Prusak
#CMPE220-02

.data
.word
.text
sltiu $s0, $zero, 4
bne $s0, $s1, $s2
addi $s1, $s1, 1
sub $s1, $s1, $s0
jr $ra
sltu $t6, $s3, $a2
sw $t5, 7($t2)
nor $s1, $t4, $t0
