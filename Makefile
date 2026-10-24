ifndef PS4SDK
    $(error PS4SDK no está definido. Asegúrate de tener el entorno configurado.)
endif

target := xbox_pad
OutPath := release
TargetFile := $(OutPath)/$(target).prx
ObjectFiles := build/main.o

CC := clang
CXX := clang++
AR := ar
ODIR := build
SDIR := src

CFLAGS := -target x86_64-pc-freebsd-elf -fPIC -funwind-tables -c -O2 -std=gnu11
# AQUI EL CAMBIO: quitamos --script y usamos -shared
LDFLAGS := -m elf_x86_64 -shared --eh-frame-hdr

all: $(TargetFile)

$(TargetFile): $(ObjectFiles)
	mkdir -p $(OutPath)
	ld $(LDFLAGS) -o $(TargetFile) $(ObjectFiles)

$(ODIR)/%.o: $(SDIR)/%.c
	mkdir -p $(ODIR)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(OutPath) $(ODIR) $(ObjectFiles)
