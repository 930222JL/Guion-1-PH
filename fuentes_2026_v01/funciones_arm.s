	AREA codigo, CODE, READONLY
	EXPORT dense_layer_q12_ARM_C
	IMPORT neuron_q12_C
	PRESERVE8 {TRUE}
		
	
	
dense_layer_q12_ARM_C
	push {lr,fp}
	mov fp,sp
	push{r4-r11}
	mov r4,r3
	ldr r5,[fp,#12]
	ldr r6,[fp,#16]
	ldr r7,[fp,#20]
	push{r7,r6}
	mov r6,r0
	mov r7,r2
	ldr r2,[fp,#8]
	mov r8,#0
	mov r9,#0
	mov r10,r1
	
	; r0=input, r1=weights_o, r2=input_size, r4=output, r5=output_size, r6=input, r7=bias, r8=checksum, r9=o, r10=weights, sp+0=clamp_min, sp+4=clamp_max
	
bucle
	cmp r9,r5
	bge fin_bucle
	ldr r2,[fp,#8]
	mov r0,r6
	lsl r9,#1
	mul r1,r9,r2
	add r1,r1,r10
	ldrsh r3,[r7,r9]
	bl neuron_q12_C
	strh r0,[r4,r9]
	mov r1,#33
	mul r8,r1,r8
	lsl r0,r0,#16
	lsr r0,r0,#16
	add r8,r8,r0
	add r9,r9,#2
	lsr r9,#1
	b bucle
	
fin_bucle
	add sp,sp,#8	;desapilo clamp_min y clamp_max de las llamadas a neuron_q12_C
	mov r0,r8
	pop{r4-r11}
	pop{fp,pc}
	
	
	
neuron_q12_ARM
	mov ip,sp
	stmdb sp!, {r4-r10,fp,ip,lr,pc}
	sub fp,ip,#4
	sub sp, sp,#16
	lsl r4,r3#12	;r4=acc
	mov r5,#0		;r5=i
bucle 
	cmp r5,r2
	bge fin_bucle
	lsl r5,#2
	ldr r6,[r0,r5]
	ldr r7,[r1,r5]
	mul r6,r7,r6
	add r4,r4,r6
	lsr r5,#2
	add r5,r5,#1
	b bucle
	
fin_bucle
	lsr r4,#12
	ldr r5,[fp,#4]	;r5=clamp_min
	ldr r6,[fp,#8]	;r6clamp_max
	mov r0,r4
	cmp r4,r5
	movlt r0,r5
	cmp r4,r6
	movgt r0,r6
	lsl r0,#16
	lsr r0,#16
	
	ldmdb fp, {r4-r10.fp,sp,pc}


dense_layer_q12_ARM

	
	end