.text
.global fabs
.type   fabs,%function
fabs:
	break  0
	fabs.d $f0, $f0
	jr     $ra
