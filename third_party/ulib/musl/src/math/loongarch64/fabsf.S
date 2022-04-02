.text
.global fabsf
.type   fabsf,%function
fabsf:
	break  0
	fabs.s $f0, $f0
	jr     $ra
