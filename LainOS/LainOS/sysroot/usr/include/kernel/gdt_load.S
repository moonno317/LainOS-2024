.intel_syntax noprefix
.section .text
	.global gdt_load

gdt_load:
	# Copy the first argument in esp + 4 into eax
	mov eax, [esp + 4] # EAX is a pointer to a 48 bit struct
	
	# Load the new GDT by dereferencing the pointer contained
	# in register eax, which points to the 48 bit struct.
	lgdt [eax]
	
	# Set the data segment registers
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	jmp 0x08:.flush
	# mov eax, 0 # Handle the return code
	ret

.flush:
	ret
