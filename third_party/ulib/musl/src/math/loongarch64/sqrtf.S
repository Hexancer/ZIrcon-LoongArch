.text
.global sqrtf
.type   sqrtf,%function
sqrtf:
	break   0
	fsqrt.s $f0, $f0
	jr      $ra
