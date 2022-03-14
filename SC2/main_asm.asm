.data


.code
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ShieldsHookF proc
	push rax
	mov eax, [rsi+0190h]
	cmp eax, 0
	pop rax
	jne @code

	mov eax, 0
	mov [rsi+019Ch],eax

	jmp @return
	@code:
	mov [rsi+019Ch],eax
	@return:
	ret
ShieldsHookF endp
ShieldsHookFEnd proc
ret
ShieldsHookFEnd endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
HPHookF proc
	push rax
	mov eax, [rbp+00000190h]
	cmp eax, 0
	pop rax
	jne @code
	mov esi, 0
	@code:
	mov [rbp+00000198h],esi
	ret
HPHookF endp
HPHookFEnd proc
ret
HPHookFEnd endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
HPSubjugationHookF proc
	push rax
	mov eax, [rbp+00000190h]
	cmp eax, 0
	pop rax
	jne @code
	mov esi, 0
	@code:

	mov esi, 10000h
	mov [rbp+0000034h],esi
	mov esi, 0

	mov [rbp+00000198h],esi
	ret
HPSubjugationHookF endp
HPSubjugationHookFEnd proc
ret
HPSubjugationHookFEnd endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;
EnergyHookF proc
	push rax
	mov eax, [rbp+00000190h]
	cmp eax, 0
	pop rax
	jne @code

	cmp [rbp+000001A0h],ebx
	jg @return
	@code:
	mov [rbp+000001A0h],ebx

	@return:
	ret
EnergyHookF endp
EnergyHookFEnd proc
ret
EnergyHookFEnd endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;;

END