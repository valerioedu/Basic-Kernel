//
//  here i'll write the bootloader code
//  it will be written with the instructions set of the cortex a76
//  max memory size 8GB
//

.equ UART0_BASE, 0x3F201000
.equ UARTDR, UART0_BASE + 0x00
.equ UARTFR, UART0_BASE + 0x18
.equ UARTIBRD, UART0_BASE + 0x24
.equ UARTFBRD, UART0_BASE + 0x28
.equ UARTLCR_H, UART0_BASE + 0x2C
.equ UARTCR, UART0_BASE + 0x30

.equ KERNEL_SIZE, 0x800000
.equ KERNEL_SRC_ADDR, 0x80000000
.equ KERNEL_DST_ADDR, 0x80000

.section .text

.global _start

_start:
    ldr x0, =0xFFFFFFFF
    mov sp, x0
    bl uart_init
    ldr x0, =msg_start
    bl uart_print
    ldr x1, =KERNEL_SIZE
    ldr x2, =KERNEL_SRC_ADDR
    ldr x3, =KERNEL_DST_ADDR
    bl verify_kernel
    cbnz x0, error_handler
    bl copy
    br x3

msg_start:
    .asciz "Bootloader started\n"

error_handler:
    ldr x0, =msg_error
    bl uart_print
    wfi

msg_error:
    .asciz "Failed to load the Kernel\n"

verify_kernel:
    mov x4, x1
    mov x5, x2
    mov x6, #0

verify_loop:
    cbz x4, verify_end
    ldrb w7, [x5], #1
    add x6, x6, w7
    sub x4, x4, #1
    b verify_loop

verify_end:
    ldr x8, =EXPECTED_SIZE
    cmp x6, x8
    cset x0, ne
    ret

copy:
    mov x4, x1
    mov x5, x2
    mov x6, x3

copy_loop:
    cbz x4, copy_end
    ldrb w7, [x5], #1
    strb w7, [x6], #1
    sub x4, x4, #1
    b copy_loop

copy_end:
    ret

EXPECTED_SIZE:
    .word // kernel size

uart_init:
    ldr x1, =UARTCR
    mov w0, #0
    str w0, [x1]
    ldr x1, =UARTIBRD
    mov w0, #1
    str w0, [x1]
    ldr x1, =UARTFBRD
    mov w0, #40
    str w0, [x1]
    ldr x1, =UARTLCR_H
    mov w0, #0(0x3 << 5)
    str w0, [x1]
    ldr x1, =UARTCR
    mov w0, #(1 << 0 | 1 << 8 | 1 << 9)
    str w0, [x1]
    ret

uart_print:
    mov x1, x0

uart_print_loop:
    ldrb w2, [x1], #1
    cbz w2, uart_print_end

uart_print_wait:
    ldr x3, =UARTFR
    ldr w4, [x3]
    and w4, w4, #(1 << 5)
    cbz w4, uart_print_wait
    ldr x3, =UARTDR
    strb w2, [x3]
    b uart_print_loop
    
uart_print_end:
    ret