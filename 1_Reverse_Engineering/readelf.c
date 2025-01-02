#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct {
    unsigned char e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
} Elf64_Ehdr;

typedef struct {
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
} Elf64_Phdr;

typedef struct {
    uint32_t sh_name;
    uint32_t sh_type;
    uint64_t sh_flags;
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint64_t sh_addralign;
    uint64_t sh_entsize;
} Elf64_Shdr;

int main() {
    const char *filepath = "/challenge/easy_re";

    FILE *fp = fopen(filepath, "rb");
    if (!fp) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    Elf64_Ehdr ehdr;
    fread(&ehdr, 1, sizeof(Elf64_Ehdr), fp);

    if (memcmp(ehdr.e_ident, "\x7f""ELF", 4) != 0) {
        fprintf(stderr, "This is not a valid ELF file.\n");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    printf("Segment Information:\n");
    for (int i = 0; i < ehdr.e_phnum; i++) {
        Elf64_Phdr phdr;
        fseek(fp, ehdr.e_phoff + i * sizeof(Elf64_Phdr), SEEK_SET);
        fread(&phdr, 1, sizeof(Elf64_Phdr), fp);
        printf("Segment #%d:\n", i);
        printf("  File Offset: 0x%lx\n", phdr.p_offset);
        printf("  Virtual Address: 0x%lx\n", phdr.p_vaddr);
        printf("  File Size: 0x%lx\n", phdr.p_filesz);
        printf("  Memory Size: 0x%lx\n", phdr.p_memsz);
    }

    Elf64_Shdr shstrtab_hdr;
    fseek(fp, ehdr.e_shoff + ehdr.e_shstrndx * sizeof(Elf64_Shdr), SEEK_SET);
    fread(&shstrtab_hdr, 1, sizeof(Elf64_Shdr), fp);

    char *shstrtab = malloc(shstrtab_hdr.sh_size);
    fseek(fp, shstrtab_hdr.sh_offset, SEEK_SET);
    fread(shstrtab, 1, shstrtab_hdr.sh_size, fp);

    printf("\nSection Information:\n");
    for (int i = 0; i < ehdr.e_shnum; i++) {
        Elf64_Shdr shdr;
        fseek(fp, ehdr.e_shoff + i * sizeof(Elf64_Shdr), SEEK_SET);
        fread(&shdr, 1, sizeof(Elf64_Shdr), fp);
        printf("Section #%d: %s\n", i, &shstrtab[shdr.sh_name]);
        printf("  File Offset: 0x%lx\n", shdr.sh_offset);
        printf("  Virtual Address: 0x%lx\n", shdr.sh_addr);
        printf("  Section Size: 0x%lx\n", shdr.sh_size);
    }

    free(shstrtab);
    fclose(fp);

    return 0;
}
