.text
.global sqrt
.type   sqrt,%function
sqrt:
	fsqrt.d $f0, $f0
	jr      $ra
