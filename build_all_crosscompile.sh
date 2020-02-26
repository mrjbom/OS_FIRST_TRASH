#!/bin/bash

declare -a clearRoutes
clearRoutes=("./bootable.iso" "./o/bootloaderasm.o" "./o/iqrhandlersasm.o" 
#/builded objects
"./o/kmain.o" "./o/more.o"
"./o/cstdlib.o" "./o/string.o"
"./o/lfbmem.o" "./o/textmodemem.o" 
"./o/inlineasm.o" 
"./o/interruptions.o"
#/memory
"./o/memorymemdetect.o"
"./o/memorymemmmu.o"
#\memory
"./o/debug.o" "./o/deviceskeyboard.o"
"./o/devicescpu.o"
#\builded objects
"./kernel-0" "./iso/boot/kernel-0")

clear()
{
	echo -e "\e[36mCleansing...\e[0m"
	clearArraySize=${#clearRoutes[*]}
	for ((i=0; i < clearArraySize; i++))
	do
		if [ -f ./${clearRoutes[i]} ]
		then
			echo "Remove ${clearRoutes[i]}"
			rm ${clearRoutes[i]}
		else
			echo "${clearRoutes[i]} not found"
		fi
	done	
}

echo -e "\e[36mStart kernel building...\e[0m"

clear

#check and compile boot loader
echo -e "\e[36mCompile ./source/bootloader.asm...\n\e[0m"
if [ ! -f ./source/bootloader.asm ]
then
	echo -e "\e[31mERROR!\e[0m"
	echo "./source/bootloader.asm not found"
	echo "Abort..."
	clear
	exit;
fi
nasm -f elf32 ./source/bootloader.asm -o ./o/bootloaderasm.o

#check and compile irqhandlers.asm
echo -e "\e[36mCompile ./source/interruptions/irqhandlers.asm...\n\e[0m"
if [ ! -f ./source/interruptions/irqhandlers.asm ]
then
	echo -e "\e[31mERROR!\e[0m"
	echo "./source/interruptions/irqhandlers.asm not found"
	echo "Abort..."
	clear
	exit;
fi
nasm -f elf32 ./source/interruptions/irqhandlers.asm -o ./o/irqhandlersasm.o

declare -a buildCRoutes
buildCRoutes=("./source/kmain.c" "./source/more/more.c"
"./source/lib/cstdlib.c" "./source/lib/string.c"
"./source/lfbmemory/lfbmemory.c" "./source/textmodememory/textmodememory.c" 
"./source/inlineassembly/inlineassembly.c"
"./source/interruptions/interruptions.c"
#memory
"./source/memory/memdetect/memdetect.c"
"./source/memory/memmmu/memmmu.c"
#memory
"./source/debug/debug.c" "./source/devices/keyboard/keyboard.c"
"./source/devices/cpu/cpu.c")

declare -a buildObjectRoutes
buildObjectRoutes=("./o/kmain.o" "./o/more.o"
"./o/cstdlib.o" "./o/string.o"
"./o/lfbmem.o" "./o/textmodemem.o" 
"./o/inlineasm.o" 
"./o/interruptions.o"
#memory
"./o/memorymemdetect.o"
"./o/memorymemmmu.o"
#memory
"./o/debug.o" "./o/deviceskeyboard.o"
"./o/devicescpu.o")

echo -e "\e[36mCompile .c files...\e[0m"
buildCRoutesSize=${#buildCRoutes[*]}
for ((i=0; i < buildCRoutesSize; i++))
do
	if [ -f ${buildCRoutes[i]} ]
	then
		echo "Build ${buildCRoutes[i]}"
		./i386-elf-4.9.1-Linux-x86_64/bin/i386-elf-gcc --freestanding -m32 -std=c11 -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-but-set-variable -c ${buildCRoutes[i]} -o ${buildObjectRoutes[i]}
	else
		echo -e "\e[31mERROR!\e[0m"
		echo "${buildCRoutes[i]} not found"
		echo "Abort..."
		clear
		exit;
	fi
done

#
#check linker file and start linker
#if [ ! -f ./link.ld ]
#then
#	echo -e "\e[31mERROR!\e[0m"
#	echo "./link.ld not found"
#	echo "Abort..."
#	clear
#	exit;
#fi
#echo -e "\n\e[36mStart linker...\n\e[0m"
#
#./i386-elf-4.9.1-Linux-x86_64/bin/i386-elf-ld -m elf_i386 -T link.ld -o kernel-0 ./o/bootloaderasm.o ./o/iqrhandlersasm.o ${buildObjectRoutes[@]} -nostdlib -lgcc
#

#check linker file and start linker
if [ ! -f ./link.ld ]
then
	echo -e "\e[31mERROR!\e[0m"
	echo "./link.ld not found"
	echo "Abort..."
	clear
	exit;
fi
echo -e "\n\e[36mStart linker...\n\e[0m"

#./i386-elf-4.9.1-Linux-x86_64/bin/i386-elf-ld -m elf_i386 -T link.ld -o kernel-0 ./o/bootloaderasm.o ./o/iqrhandlersasm.o ${buildObjectRoutes[@]} -nostdlib -lgcc
./i386-elf-4.9.1-Linux-x86_64/bin/i386-elf-gcc -T link.ld -o kernel-0 -ffreestanding ./o/bootloaderasm.o ./o/irqhandlersasm.o ${buildObjectRoutes[@]} -nostdlib -lgcc

#check kernel file
if [ ! -f ./kernel-0 ]
then
	echo -e "\e[31mERROR!\e[0m"
	echo "./kernel-0 not found"
	echo "Abort..."
	clear
	exit;
fi

echo -e "\e[36mImage preparation...\e[0m"
#image preparation
cp kernel-0 iso/boot
grub-mkrescue -o bootable.iso iso
if [ ! -f ./bootable.iso ]
then
	echo -e "\e[31mERROR!\e[0m"
	echo "./bootable.iso not found"
	echo "Abort..."
	clear
	exit;
fi

echo -e "\e[36mStart emulation...\e[0m"
#start emulation
if [[ $1 == "-d" ]]
then
	echo -e "\e[32mDEBUG ENABLE\n\e[0m"
	qemu-system-i386 -serial file:serial.log bootable.iso
else
	qemu-system-i386 bootable.iso
fi
