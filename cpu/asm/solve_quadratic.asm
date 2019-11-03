const_string no_roots 'no roots'
const_string one_root 'one root'
const_string inf_root 'infinite number of roots'
const_string two_real 'two real roots'
const_string two_complex 'two complex'
const_string matching 'matching roots'

in 
pop rax ; rax = a

in
pop rbx ; rbx = b

in
pop rcx ; rcx = c

push rax
push 0
jne solve_quadratic

;solve_linear:
push 0
push rbx
je inf_or_no
push rbx
push rcx
push 0
sub
div
print one_root
out
end

inf_of_no:
	push 0
	push rcx
	je inf_roots
	print no_roots
	end

	inf_roots:
		print inf_roots
		end

solve_quadratic:
	push 4
	push rax
	push rcx
	mul
	mul

	push rbx
	push rbx
	mul

	sub
	pop rdx ; D = rdx

	push epsilon
	push rdx
	abs
	jb matching ; D = 0

	push rdx
	push 0

	jb two_real	; D >0
	
	push -1
	mul rdx ; D -> -D

	print two complex
	;;;;;;;;;
	;;;;;;;;;
	;;;;;;;;;
	;;;;;;;;;
	end



two_real:
	print two_real
	push 2
	push a
	mul
	
	push rdx
	sqrt
	push rbx
	push 0
	sub

	add
	div
	out ;first root

	push 2
	push a
	mul
	
	push rdx
	sqrt
	push rbx
	push 0
	sub

	sub
	div
	out ;second root

	end

matching:

	push 2
	push rax
	mul

	push rbx
	push 0
	sub

	div
	print matching
	out 
	end
