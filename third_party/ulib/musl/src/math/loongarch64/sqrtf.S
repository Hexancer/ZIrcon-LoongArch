.text
.global sqrtf
.type   sqrtf,%function
sqrtf:
	fsqrt.s $f0, $f0
	jr      $ra
