#!/bin/bash

declare -a buildCRoutes
buildCRoutes=("./source/kmain.c" "./source/more/more.c"
"./source/lib/string.c" "./source/lib/math.c"
"./source/lib/printf.c"
"./source/lfbmemory/lfbmemory.c" "./source/textmodememory/textmodememory.c" 
"./source/inlineassembly/inlineassembly.c"
"./source/interruptions/interruptions.c"
"./source/interruptions/exceptions/exceptions.c"
"./source/memory/memdetect/memdetect.c"
"./source/memory/mmu/mmu.c"
"./source/memory/mmu/paging.S"
"./source/task/task.c"
"./source/task/switch.S"
"./source/debug/debug.c"
"./source/devices/keyboard/keyboard.c"
"./source/devices/pit/pit.c"
"./source/devices/cpu/cpu.c")

declare -a buildObjectRoutes
buildObjectRoutes=("./o/kmain.o" "./o/more.o"
"./o/string.o" "./o/math.o"
"./o/printf.o"
"./o/lfbmem.o" "./o/textmodemem.o" 
"./o/inlineasm.o" 
"./o/interruptions.o"
"./o/interrexceptions.o"
"./o/memorymemdetect.o"
"./o/memorymmu.o"
"./o/memorypagingS.o"
"./o/task.o"
"./o/switchS.o"
"./o/debug.o"
"./o/deviceskeyboard.o"
"./o/devicespit.o"
"./o/devicescpu.o")

declare -a clearRoutes
clearRoutes=("./bootable.iso" "./o/bootloaderasm.o" "./o/irqhandlersasm.o"
"./o/kmain.o" "./o/more.o"
"./o/string.o" "./o/math.o"
"./o/printf.o"
"./o/lfbmem.o" "./o/textmodemem.o" 
"./o/inlineasm.o" 
"./o/interruptions.o"
"./o/interrexceptions.o"
"./o/memorymemdetect.o"
"./o/memorymmu.o"
"./o/memorypagingS.o"
"./o/task.o"
"./o/switchS.o"
"./o/debug.o" 
"./o/deviceskeyboard.o"
"./o/devicespit.o"
"./o/devicescpu.o"
"./kernel-0" "./iso/boot/kernel-0")

#default flags
gccCompilerDefaultFlagsString="--freestanding -m32 -std=c99 -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-but-set-variable"
gccLinkerDefaultFlagsString="-nostdlib -lgcc"

#debug flags
gccCompilerDebugFlagsString="--freestanding -m32 -std=c99 -Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-but-set-variable -g3"
gccLinkerDebugFlagsString="-nostdlib -lgcc"

clear()
{
	echo -e "\e[36mCleansing...\e[0m"
	clearArraySize=${#clearRoutes[*]}
	for ((i=0; i < clearArraySize; i++))
	do
		if [ -f ./"${clearRoutes[i]}" ]
		then
			echo "Remove ${clearRoutes[i]}"
			rm "${clearRoutes[i]}"
		else
			echo "${clearRoutes[i]} not found"
		fi
	done	
}

#show info
if [[ $1 == "info" ]]
then
	echo "-d compiles the kernel and prepares it for debugging using Visual Studio Code"
	echo "-d -s \"qemu parameters\" compiles the kernel and prepares it for debugging,"
	echo "adds parameters for QEMU(Only works with the \"-d\" flag), example:"
	echo "-d -s \"-m G1\""
	echo "without parameters it just compiles the kernel and run in qemu"
fi
#DEBUGING
#Compilation with writing debug symbols for gdb and preparing for debugging,
#runs qemu and gdb cross-compiler
if [[ $1 == "-d" ]]
then
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

	echo -e "\e[36m(debug) Compile .c files...\e[0m"
	buildCRoutesSize=${#buildCRoutes[*]}
	for ((i=0; i < buildCRoutesSize; i++))
	do
		if [ -f "${buildCRoutes[i]}" ]
		then
			echo "(debug) Build ${buildCRoutes[i]}"
			./i386-elf-4.9.1-Linux-x86_64/bin/i386-elf-gcc $gccCompilerDebugFlagsString -c "${buildCRoutes[i]}" -o "${buildObjectRoutes[i]}"
		else
			echo -e "\e[31mERROR!\e[0m"
			echo "${buildCRoutes[i]} not found"
			echo "Abort..."
			clear
			exit;
		fi
	done

	#check linker file and start linker
	if [ ! -f ./link.ld ]
	then
		echo -e "\e[31mERROR!\e[0m"
		echo "./link.ld not found"
		echo "Abort..."
		clear
		exit;
	fi
	echo -e "\n\e[36m(debug) Start linker...\n\e[0m"

	./i386-elf-4.9.1-Linux-x86_64/bin/i386-elf-gcc -T link.ld -o kernel-0 -ffreestanding ./o/bootloaderasm.o ./o/irqhandlersasm.o ./FreeSans.o ./UbuntuBold.o "${buildObjectRoutes[@]}" $gccLinkerDebugFlagsString

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

	#echo -e "\e[36mStart emulation...\e[0m"
	#start emulation
	echo -e "\e[32mGDB DEBUG ENABLE\n\e[0m"
	if [[ $2 == "" ]]
	then
		./i386-elf-4.9.1-Linux-x86_64/bin/i386-elf-objcopy --only-keep-debug kernel-0 kernel.sym
		qemu-system-i386 -serial file:serial.log -s -monitor stdio bootable.iso
	#fi
	#-s - special arguments for qemu
	#example (starts the VM in debug mode and 1 GB of RAM)
	#-d -s -m G1
	elif [[ $2 == "-s" ]]
	then
		specialQEMUParameters=""
		while [[ -n "$2" ]]
		do
			shift
			specialQEMUParameters+=" $2"
		done
		echo -e "\e[32mStart QEMU with special parameters \"$specialQEMUParameters\"\e[0m"
		./i386-elf-4.9.1-Linux-x86_64/bin/i386-elf-objcopy --only-keep-debug kernel-0 kernel.sym
		qemu-system-i386 -serial file:serial.log -s -monitor stdio $specialQEMUParameters bootable.iso
		exit;
	else
		echo -e "\e[31mERROR!\e[0m"
		echo -e "\e[31mThe argument \"$2\" was not recognized!\e[0m"
		echo "Abort..."
		clear
		exit;
	fi
fi
################################################################################
################################################################################
################################################################################

#DEFAULT
if [[ $1 == "" ]]
then
	#Normal compilation with running qemu
	#writing debugging information in serial.log
	
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

	echo -e "\e[36mCompile .c files...\e[0m"
	buildCRoutesSize=${#buildCRoutes[*]}
	for ((i=0; i < buildCRoutesSize; i++))
	do
		if [ -f "${buildCRoutes[i]}" ]
		then
			echo "Build ${buildCRoutes[i]}"
			./i386-elf-4.9.1-Linux-x86_64/bin/i386-elf-gcc $gccCompilerDefaultFlagsString -c "${buildCRoutes[i]}" -o "${buildObjectRoutes[i]}"
		else
			echo -e "\e[31mERROR!\e[0m"
			echo "${buildCRoutes[i]} not found"
			echo "Abort..."
			clear
			exit;
		fi
	done

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

	./i386-elf-4.9.1-Linux-x86_64/bin/i386-elf-gcc -T link.ld -o kernel-0 -ffreestanding ./o/bootloaderasm.o ./o/irqhandlersasm.o ./FreeSans.o ./UbuntuBold.o "${buildObjectRoutes[@]}" $gccLinkerDefaultFlagsString

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
	echo -e "\e[32mSERIAL DEBUG ENABLE\n\e[0m"
	qemu-system-i386 -serial file:serial.log bootable.iso
fi