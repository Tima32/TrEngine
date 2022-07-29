option casemap:none
.686
.model flat, c

EXTERN HPFunc :PROC ; HPFunc
EXTERN DamageFunc :PROC ; DamageFunc
EXTERN CheckHPFunc :PROC ; CheckHPFunc
EXTERN ManaFunc :PROC ; ManaFunc

.data


.code

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
HPHookF proc
	
	pusha
	push esi
	call HPFunc
	pop esi
	popa

	fld dword ptr [esi+0000042Ch]

	ret
HPHookF endp
HPHookFEnd proc
ret
HPHookFEnd endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
DamageHookF proc
	
	push eax

	mov eax, [esi+000000E4h]
	fstp dword ptr [esi+000000E4h]

	pusha
	push eax
	push esi
	call DamageFunc
	pop esi
	pop eax
	popa

	pop eax

	ret
DamageHookF endp
DamageHookFEnd proc
ret
DamageHookFEnd endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
CheckHPHookF proc
	
	pusha
	push ecx
	call CheckHPFunc
	pop ecx
	popa

	fcom dword ptr [ecx+00000E4h]

	ret
CheckHPHookF endp
CheckHPHookFEnd proc
ret
CheckHPHookFEnd endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ManaHookF proc
	
	push eax

	mov eax, [ecx+00000470h]
	fstp dword ptr [ecx+00000470h]

	pusha
	push eax
	push ecx
	call ManaFunc
	pop ecx
	pop eax
	popa

	pop eax

	ret
ManaHookF endp
ManaHookFEnd proc
ret
ManaHookFEnd endp
;;;;;;;;;;;;;;;;;;;;;;;;;;;

END