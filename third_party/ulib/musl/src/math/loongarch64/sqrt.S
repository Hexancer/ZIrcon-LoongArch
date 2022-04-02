.text
.global sqrt
.type   sqrt,%function
sqrt:
	break   0
	fsqrt.d $f0, $f0
	jr      $ra
