;MIT License

;Copyright (c) 2025 Nguyen Hoang Minh Triet

;Permission is hereby granted, free of charge, to any person obtaining a copy
;of this software and associated documentation files (the "Software"), to deal
;in the Software without restriction, including without limitation the rights
;to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
;of the Software, and to permit persons to whom the Software is furnished to do so,
;subject to the following conditions:

;The above copyright notice and this permission notice shall be included in all
;copies or substantial portions of the Software.

;THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
;INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
;PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
;HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
;OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
;SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

;-----------------------------------------------------------------------
;Author : Nguyen Hoang Minh Triet 
;Email  : 23trietminh23@gmail.com  
;GitHub : https://github.com/nguyentriet0810  
;YouTube: https://www.youtube.com/@hoangtriet9999
;-----------------------------------------------------------------------

				; Start of code section (read-only, aligned to 2 bytes)
                AREA |.TEXT|, CODE, READONLY, ALIGN=2

                ; Indicate use of Thumb instruction set
                THUMB

                ; Ensure 8-byte stack alignment as per ARM AAPCS
                PRESERVE8

                ; External variable declaration (defined in C code)
                EXTERN currentPt

                ; Export symbols to make functions visible to C code
                EXPORT osSchedulerLaunch
                EXPORT PendSV_Handler

                ; External function call (defined in C code)
                IMPORT osSchedulerRoundRobin

;------------------------------------------------------------------------------
; Function: osSchedulerLaunch
; Description:
;   Restores the first task context from its stack and starts execution.
;   Used to launch the scheduler.
;------------------------------------------------------------------------------
osSchedulerLaunch
                ; Load address of currentPt (pointer to current task control block)
                LDR     R0, =currentPt

                ; Dereference currentPt to get the current task's stack pointer
                LDR     R2, [R0]

                ; Load saved process stack pointer
                LDR     SP, [R2]

                ; Restore R4-R11 (callee-saved registers)
                POP     {R4-R11}

                ; Restore R0-R3 (caller-saved registers)
                POP     {R0-R3}

                ; Restore R12
                POP     {R12}

                ; Skip saved LR (reserved space)
                ADD     SP, SP, #4

                ; Restore LR (return address)
                POP     {LR}

                ; Skip saved xPSR (not needed here)
                ADD     SP, SP, #4

                ; Enable interrupts (clear PRIMASK)
                CPSIE   I

                ; Return from function (branch to address in LR)
                BX      LR

;------------------------------------------------------------------------------
; Function: PendSV_Handler
; Description:
;   PendSV exception handler for context switching.
;   Saves current task context, calls scheduler, restores next task context.
;------------------------------------------------------------------------------
PendSV_Handler
                ; Disable interrupts (set PRIMASK)
                CPSID   I

                ; Save R4-R11 (callee-saved registers)
                PUSH    {R4-R11}

                ; Load address of currentPt
                LDR     R0, =currentPt

                ; Load pointer to current task's control block
                LDR     R1, [R0]

                ; Save current SP into currentPt
                STR     SP, [R1]

                ; Save LR and currentPt before scheduler call
                PUSH    {R0, LR}

                ; Call round-robin scheduler to update currentPt
                BL      osSchedulerRoundRobin

                ; Restore R0 and LR after scheduler call
                POP     {R0, LR}

                ; Load new task's stack pointer
                LDR     R1, [R0]

                ; Restore SP from new task
                LDR     SP, [R1]

                ; Restore R4-R11
                POP     {R4-R11}

                ; Enable interrupts
                CPSIE   I

                ; Return from handler
                BX      LR

                ; End of file
                END
