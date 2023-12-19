[bits 16]
[org 0x7C00]

jmp short bootloader_start
nop

; BIOS PARAMETER BLOCK
OEM_IDENTIFIER      db "LWDOS1.0"
BytesPerSector      dw 0x0200
SectorsPerCluster   db 0x08
ReservedSectors     dw 0x0020
NumberOfFats        db 0x02
RootDirEntries      dw 0x0000
TotalSectors16      dw 0x0000
Media               db 0xF8
FATSize16           dw 0x0000
SectorsPerTrack     dw 0x003D
NumberOfHeads       dw 0x0002
HiddenSectors       dd 0x00000000
TotalSectors32      dd 0x00FE3B1F

; EXTENDED BOOT RECORD
FATSize32           dd 0x00000778
Flags               dw 0x0000
FATVersion          dw 0x0000
RootDirStart        dd 0x00000002
FATInfoSect         dw 0x0001
BackupBootSect      dw 0x0006

times 12 db 0

DriveNumber         db 0x80
ReservedBytes       db 0x00
Signature           db 0x28
VolumeNumber        dw 0xef02
VolumeLabel         db "LWD-OS 1.0 "
SystemIdentifier    db "FAT32   "

bootloader_start:
    cli
    xor ax, ax
    mov es, ax
    mov ds, ax
    mov ss, ax
    mov ax, 0x7c00
    mov sp, ax
    sti

    cmp dl, 0x80
    jne floppy_error

    mov byte [DriveNumber], dl 

    mov ah, 0x01
    mov cx, 0x0007
    int 0x10

    mov ah, 0x06
    mov al, 0x00
    mov bh, 0x07
    mov ch, 0x00
    mov cl, 0x00
    mov dh, 0x27
    mov dl, 0x50

    int 0x10

    mov bh, 0x1f
    mov dh, 0x02
    mov dl, 0x50

    int 0x10

    mov ah, 0x02
    mov bh, 0x00

    mov dh, 0x00
    mov dl, 0x00

    int 0x10

    mov cx, 240

.fill:
    mov ah, 0x0e
    mov al, '-'
    int 0x10

    dec cx
    cmp cx, 0
    je .done

    jmp .fill

.done:
    mov ah, 0x02
    mov bh, 0x00
    mov dh, 0x01
    mov dl, 0x1A
    int 0x10
    mov si, bootloader_msg
    call bootloader_print

    mov bh, 0x00
    mov dh, 0x03
    mov dl, 0x00
    int 0x10

    mov si, bootloader_loading_msg
    call bootloader_print

    pusha
    push dx

.check_for_extensions:
    mov ah, 0x41
    mov bx, 0x55AA
    mov dl, [DriveNumber]
    int 0x13
    jc no_extensions

.load_second_stage:
    stc
    mov si, Disk_Address_Packet
    mov ah, 0x42
    mov dl, [DriveNumber]
    int 0x13
    jc short disk_error

    hlt

.halt:
    jmp .halt

bootloader_print:
    pusha
    mov ah, 0x0e
.loop:
    lodsb
    or al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    popa
    ret

disk_error:
    mov si, bootloader_error_msg
    call bootloader_print
    hlt
.hlt:
    jmp .hlt

floppy_error:
    mov si, bootloader_floppy_msg
    call bootloader_print
    hlt
.halt:
    jmp .halt

no_extensions:
    mov si, bootloader_ext_error_msg
    call bootloader_print
    hlt
.halt:
    jmp .halt

; Bootloader strings
bootloader_msg db "LWD_OS LEGACY BIOS BOOTLOADER", 0x0D, 0x0A, 0
bootloader_loading_msg db "Loading Stage 2...", 0x0D, 0x0A, 0
bootloader_error_msg db "ERROR READING FROM DISK!!!", 0x0D, 0x0A, 0
bootloader_ext_error_msg db "YOUR BIOS DOES NOT SUPPORT EXTENSIONS!!! LWD-OS CANNOT LOAD!!!", 0x0D, 0x0A, 0
bootloader_floppy_msg db "LWD-OS DOES NOT SUPPORT FLOPPY DRIVES!!!", 0x0D, 0x0A, 0

Disk_Address_Packet:
        db 0x10
        db 0
        dw 16
        dw 0x7e00
        dw 0
        dw 1
        dd 0


times 510-($-$$) db 0
db 0x55
db 0xAA