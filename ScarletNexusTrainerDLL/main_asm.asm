.data
	extern DamageFilter: proto
.code

HookFunc proc
	mov ecx, [rdx]

	push rax
	push rcx
	push rdx

	mov rdx, rax
	;;ecx
	call DamageFilter
	
	pop rdx
	pop rcx
	pop rax

	ret
HookFunc endp

end