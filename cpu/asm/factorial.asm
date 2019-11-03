in 
pop rcx ; rcx = n


call factorial

out

end

factorial: 
; rax - result, rcx - current number
	push rcx
	push 1
	jae ret1

	push rcx
	dec rcx
	call factorial
	mul
	ret

	ret1:
		push 1
		ret
