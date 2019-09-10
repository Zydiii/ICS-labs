#/* $begin ncopy-ys */
##################################################################
# ncopy.ys - Copy a src block of len words to dst.
# Return the number of positive words (>0) contained in src.
#
# Include your name and ID here.
#
# Describe how and why you modified the baseline code.
#
##################################################################
# Do not modify this portion
# Function prologue.
# %rdi = src, %rsi = dst, %rdx = len
ncopy:
##################################################################
# You can modify this portion
# Loop header

# Best CPE: 6.80
# Average CPE: 6.85
# Best Score: 60.0
# Bytes: 967

	iaddq $-8, %rdx
	jl Remain

# 8 members
Loop:
	mrmovq (%rdi), %r8
	mrmovq 8(%rdi),	%r9
	mrmovq 16(%rdi), %r10
	mrmovq 24(%rdi), %r11
	mrmovq 32(%rdi), %r12
	mrmovq 40(%rdi), %r13
	mrmovq 48(%rdi), %r14
	mrmovq 56(%rdi), %rcx

	andq %r8, %r8
	jg Bin_1
Bin_0:
    andq %r9, %r9
    jg Bin_10
Bin_00:
    andq %r10, %r10
    jg Bin_100
Bin_000:
    andq %r11, %r11
    jg Bin_1000
Bin_0000:
    andq %r12, %r12
    jg Bin_10000
Bin_00000:
    andq %r13, %r13
    jg Bin_100000
Bin_000000:
    andq %r14, %r14 
	jg Bin_1000000
Bin_0000000:
	andq %rcx, %rcx
    jg Add_1
	jmp Npos
####################

Bin_1:
    andq %r9, %r9
    jg Bin_11
Bin_10:
    andq %r10, %r10
    jg Bin_110
Bin_100:
    andq %r11, %r11
    jg Bin_1100
Bin_1000:
    andq %r12, %r12
    jg Bin_11000
Bin_10000:
    andq %r13, %r13
    jg Bin_110000
Bin_100000:
    andq %r14, %r14
	jg Bin_1100000	
Bin_1000000:
	andq %rcx, %rcx
    jg Add_2
Add_1:
    iaddq $1, %rax
    jmp Npos
####################

Bin_11:
    andq %r10, %r10
    jg Bin_111
Bin_110:
    andq %r11, %r11
    jg Bin_1110
Bin_1100:
    andq %r12, %r12
    jg Bin_11100
Bin_11000:
    andq %r13, %r13
    jg Bin_111000
Bin_110000:
	andq %r14, %r14
    jg Bin_1110000
Bin_1100000:
    andq %rcx, %rcx
	jg Add_3
Add_2:
    iaddq $2, %rax
    jmp Npos
####################

Bin_111:
    andq %r11, %r11
    jg Bin_1111
Bin_1110:
    andq %r12, %r12
    jg Bin_11110
Bin_11100:
    andq %r13, %r13
    jg Bin_111100
Bin_111000:
	andq %r14, %r14
    jg Bin_1111000
Bin_1110000:
    andq %rcx, %rcx
	jg Add_4	
Add_3:
    iaddq $3, %rax
    jmp Npos
####################

Bin_1111:
    andq %r12, %r12
    jg Bin_11111
Bin_11110:
	andq %r13, %r13
	jg Bin_111110
Bin_111100:
    andq %r14, %r14
    jg Bin_1111100
Bin_1111000:
    andq %rcx, %rcx
    jg Add_5
Add_4:
    iaddq $4, %rax
    jmp Npos
#####################

Bin_11111:
    andq %r13, %r13
    jg Bin_111111
Bin_111110:
    andq %r14, %r14
    jg Bin_1111110
Bin_1111100:
    andq %rcx, %rcx
    jg Add_6
Add_5:
    iaddq $5, %rax
    jmp Npos
####################

Bin_111111:
    andq %r14, %r14
    jg Bin_1111111
Bin_1111110:
    andq %rcx, %rcx
    jg Add_7
Add_6:
    iaddq $6, %rax
    jmp Npos
###################

Bin_1111111:
	andq %rcx, %rcx
	jg Add_8
Add_7:
    iaddq $7, %rax
	jmp Npos

Add_8:
	iaddq $8, %rax

Npos:
	rmmovq %r8, (%rsi)
	rmmovq %r9, 8(%rsi)
	rmmovq %r10, 16(%rsi)
	rmmovq %r11, 24(%rsi)
	rmmovq %r12, 32(%rsi)
	rmmovq %r13, 40(%rsi)
	rmmovq %r14, 48(%rsi)
	rmmovq %rcx, 56(%rsi)

	iaddq $64, %rdi
	iaddq $64, %rsi
	iaddq $-8, %rdx
	jge Loop

# %rdx = 01234567
Remain:
	iaddq $6, %rdx
	jl Final

Remain_Loop:
	mrmovq (%rdi), %r8
	mrmovq 8(%rdi), %r9

	andq %r8, %r8
	jle R2
	iaddq $1, %rax
R2:
	andq %r9, %r9
	jle Remain_Npos
	iaddq $1, %rax

Remain_Npos:
	rmmovq %r8, (%rsi)
	rmmovq %r9, 8(%rsi)
	iaddq $16, %rdi
	iaddq $16, %rsi
	iaddq $-2, %rdx
	jge Remain_Loop

# %rdx = 01
Final:
	iaddq $1, %rdx
	jl Done

    mrmovq (%rdi), %r10
	andq %r10, %r10
    rmmovq %r10, (%rsi)
    jle Done
    iaddq $1, %rax

##################################################################
# Do not modify the following section of code
# Function epilogue.
Done:
	ret
##################################################################
# Keep the following label at the end of your function
End:
#/* $end ncopy-ys */