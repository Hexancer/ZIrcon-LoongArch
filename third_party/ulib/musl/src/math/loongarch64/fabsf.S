.text
.global fabsf
.type   fabsf,%function
fabsf:
	fabs.s $f0, $f0
	jr     $ra
