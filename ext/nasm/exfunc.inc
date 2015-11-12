bshl:
	mov eax,[esp+4]
	mov ecx,[esp+8]
	shl eax,cl
	ret

bshr:
	mov eax,[esp+4]
	mov ecx,[esp+8]
	shr eax,cl
	ret
	
bsar:
	mov eax,[esp+4]
	mov ecx,[esp+8]
	sar eax,cl
	ret

faddl:
	mov esi,[esp+4]
	movsd xmm0,[esi]
	mov edi,[esp+8]
	addsd xmm0,[edi]
	movsd [esi],xmm0
	ret
	
fsubl:
	mov esi,[esp+4]
	movsd xmm0,[esi]
	mov edi,[esp+8]
	subsd xmm0,[edi]
	movsd [esi],xmm0
	ret
	
fmull:
	mov esi,[esp+4]
	movsd xmm0,[esi]
	mov edi,[esp+8]
	mulsd xmm0,[edi]
	movsd [esi],xmm0
	ret
	
fdivl:
	mov esi,[esp+4]
	movsd xmm0,[esi]
	mov edi,[esp+8]
	divsd xmm0,[edi]
	movsd [esi],xmm0
	ret
	
fclsbl:
	mov esi,[esp+4]
	mov edi,[esp+8]
	movsd xmm0,[esi]
	comisd xmm0,[edi]
	mov ebx,0
	setb bl
	mov eax,ebx
	ret
	
addl:
	mov esi,[esp+4]
	mov eax,[esi]
	mov edi,[esp+8]
	add eax,[edi]
	mov ecx,[esi+4]
	adc ecx,[edi+4]
	mov [esi],eax
	mov [esi+4],ecx
	ret
	
subl:
	mov esi,[esp+4]
	mov eax,[esi]
	mov edi,[esp+8]
	sub eax,[edi]
	mov ecx,[esi+4]
	sbb ecx,[edi+4]
	mov [esi],eax
	mov [esi+4],ecx
	ret
	
	