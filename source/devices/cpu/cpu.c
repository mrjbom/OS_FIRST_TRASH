/*
 * Copyright (c) 2006-2007 -  http://brynet.biz.tm - <brynet@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* You need to include a file with fairly(ish) compliant printf prototype, Decimal and String support like %s and %d and this is truely all you need! */

#include "cpu.h"
#include "../../debug/debug.h"

//my
//char forprint[256];

cpuinfo_t cpu_info;
char stringregs[17];

/* Required Declarations */
int do_intel(void);
int do_amd(void);
void printregs(int eax, int ebx, int ecx, int edx);

/* Simply call this function detect_cpu(); */
void detect_cpu(void) { /* or main() if your trying to port this as an independant application */
	unsigned long ebx, unused;
	cpuidm(0, unused, ebx, unused, unused);
	switch(ebx) {
		case 0x756e6547: /* Intel Magic Code */
		do_intel();
		break;
		case 0x68747541: /* AMD Magic Code */
		do_amd();
		break;
		default:
		write_serial_str("Unknown x86 CPU Detected\n");
		break;
	}
	//return rcpuinfo;
}

/* Intel Specific brand list */
char *Intel[] = {
	"Brand ID Not Supported.", 
	"Intel(R) Celeron(R) processor", 
	"Intel(R) Pentium(R) III processor", 
	"Intel(R) Pentium(R) III Xeon(R) processor", 
	"Intel(R) Pentium(R) III processor", 
	"Reserved", 
	"Mobile Intel(R) Pentium(R) III processor-M", 
	"Mobile Intel(R) Celeron(R) processor", 
	"Intel(R) Pentium(R) 4 processor", 
	"Intel(R) Pentium(R) 4 processor", 
	"Intel(R) Celeron(R) processor", 
	"Intel(R) Xeon(R) Processor", 
	"Intel(R) Xeon(R) processor MP", 
	"Reserved", 
	"Mobile Intel(R) Pentium(R) 4 processor-M", 
	"Mobile Intel(R) Pentium(R) Celeron(R) processor", 
	"Reserved", 
	"Mobile Genuine Intel(R) processor", 
	"Intel(R) Celeron(R) M processor", 
	"Mobile Intel(R) Celeron(R) processor", 
	"Intel(R) Celeron(R) processor", 
	"Mobile Geniune Intel(R) processor", 
	"Intel(R) Pentium(R) M processor", 
	"Mobile Intel(R) Celeron(R) processor"
};

/* This table is for those brand strings that have two values depending on the processor signature. It should have the same number of entries as the above table. */
char *Intel_Other[] = {
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Intel(R) Celeron(R) processor", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Intel(R) Xeon(R) processor MP", 
	"Reserved", 
	"Reserved", 
	"Intel(R) Xeon(R) processor", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved", 
	"Reserved"
};

/* Intel-specific information */
int do_intel(void) {
	cpu_info.intel_or_amd = 1;
	//write_serial_str("Intel Specific Features:\n");
	unsigned long eax, ebx, ecx, edx, max_eax, signature, unused;
	int model, family, type, brand, stepping, reserved;
	int extended_family = -1;
	cpu_info.intel_extended_family = -1;
	cpuidm(1, eax, ebx, unused, unused);
	model = (eax >> 4) & 0xf;
	cpu_info.intel_model = model;
	family = (eax >> 8) & 0xf;
	cpu_info.intel_family = family;
	type = (eax >> 12) & 0x3;
	cpu_info.intel_type = type;
	brand = ebx & 0xff;
	cpu_info.intel_brand = brand;
	stepping = eax & 0xf;
	cpu_info.intel_stepping = stepping;
	reserved = eax >> 14;
	cpu_info.intel_reserved = reserved;
	signature = eax;
	cpu_info.intel_signature = signature;
	//printf("Type %d - ", type);
	//sprintf(forprint, "Type %i - ", type);
	//write_serial_str(forprint);
	switch(type) {
		case 0:
		//write_serial_str("Original OEM");
		cpu_info.intel_type_str = "Original OEM";
		break;
		case 1:
		//write_serial_str("Overdrive");
		cpu_info.intel_type_str = "Overdrive";
		break;
		case 2:
		//write_serial_str("Dual-capable");
		cpu_info.intel_type_str = "Dual-capable";
		break;
		case 3:
		//write_serial_str("Reserved");
		cpu_info.intel_type_str = "Reserved";
		break;
	}
	//write_serial_str("\n");
	//printf("Family %d - ", family);
	//sprintf(forprint, "Family %i - ", family);
	switch(family) {
		case 3:
		write_serial_str("i386");
		cpu_info.intel_family_str = "i386";
		break;
		case 4:
		write_serial_str("i486");
		cpu_info.intel_family_str = "i486";
		break;
		case 5:
		write_serial_str("Pentium");
		cpu_info.intel_family_str = "Pentium";
		break;
		case 6:
		//write_serial_str("Pentium Pro");
		cpu_info.intel_family_str = "Pentium Pro";
		break;
		case 15:
		//write_serial_str("Pentium 4");
		cpu_info.intel_family_str = "Pentium 4";
	}
	//write_serial_str("\n");
	if(family == 15) {
		extended_family = (eax >> 20) & 0xff;
		cpu_info.intel_extended_family = extended_family;
		//printf("Extended family %d\n", extended_family);
		//sprintf(forprint, "Extended family %i\n", extended_family);
	}
	//printf("Model %d - ", model);
	//sprintf(forprint, "Model %i - ", model);
	switch(family) {
		case 3:
		break;
		case 4:
		switch(model) {
			case 0:
			case 1:
			//write_serial_str("DX");
			cpu_info.intel_model_str = "DX";
			break;
			case 2:
			//write_serial_str("SX");
			cpu_info.intel_model_str = "SX";
			break;
			case 3:
			//write_serial_str("487/DX2");
			cpu_info.intel_model_str = "487/DX2";
			break;
			case 4:
			//write_serial_str("SL");
			cpu_info.intel_model_str = "SL";
			break;
			case 5:
			//write_serial_str("SX2");
			cpu_info.intel_model_str = "SX2";
			break;
			case 7:
			//write_serial_str("Write-back enhanced DX2");
			cpu_info.intel_model_str = "Write-back enhanced DX2";
			break;
			case 8:
			//write_serial_str("DX4");
			cpu_info.intel_model_str = "DX4";
			break;
		}
		break;
		case 5:
		switch(model) {
			case 1:
			//write_serial_str("60/66");
			cpu_info.intel_model_str = "60/66";
			break;
			case 2:
			//write_serial_str("75-200");
			cpu_info.intel_model_str = "75-200";
			break;
			case 3:
			//write_serial_str("for 486 system");
			cpu_info.intel_model_str = "for 486 system";
			break;
			case 4:
			//write_serial_str("MMX");
			cpu_info.intel_model_str = "MMX";
			break;
		}
		break;
		case 6:
		switch(model) {
			case 1:
			//write_serial_str("Pentium Pro");
			cpu_info.intel_model_str = "Pentium Pro";
			break;
			case 3:
			//write_serial_str("Pentium II Model 3");
			cpu_info.intel_model_str = "Pentium II Model 3";
			break;
			case 5:
			//write_serial_str("Pentium II Model 5/Xeon/Celeron");
			cpu_info.intel_model_str = "Pentium II Model 5/Xeon/Celeron";
			break;
			case 6:
			//write_serial_str("Celeron");
			cpu_info.intel_model_str = "Celeron";
			break;
			case 7:
			//write_serial_str("Pentium III/Pentium III Xeon - external L2 cache");
			cpu_info.intel_model_str = "Pentium III/Pentium III Xeon - external L2 cache";
			break;
			case 8:
			//write_serial_str("Pentium III/Pentium III Xeon - internal L2 cache");
			cpu_info.intel_model_str = "Pentium III/Pentium III Xeon - internal L2 cache";
			break;
		}
		break;
		case 15:
		break;
	}
	//write_serial_str("\n");
	cpuidm(0x80000000, max_eax, unused, unused, unused);
	/* Quok said: If the max extended eax value is high enough to support the processor brand string
	(values 0x80000002 to 0x80000004), then we'll use that information to return the brand information. 
	Otherwise, we'll refer back to the brand tables above for backwards compatibility with older processors. 
	According to the Sept. 2006 Intel Arch Software Developer's Guide, if extended eax values are supported, 
	then all 3 values for the processor brand string are supported, but we'll test just to make sure and be safe. */
	strcpy(cpu_info.intel_brand_str, "");
	if(max_eax >= 0x80000004) {
		//write_serial_str("Brand: ");
		if(max_eax >= 0x80000002) {
			cpuidm(0x80000002, eax, ebx, ecx, edx);
			printregs(eax, ebx, ecx, edx);
			strcat(cpu_info.intel_brand_str, stringregs);
		}
		if(max_eax >= 0x80000003) {
			cpuidm(0x80000003, eax, ebx, ecx, edx);
			printregs(eax, ebx, ecx, edx);
			strcat(cpu_info.intel_brand_str, stringregs);
		}
		if(max_eax >= 0x80000004) {
			cpuidm(0x80000004, eax, ebx, ecx, edx);
			printregs(eax, ebx, ecx, edx);
			strcat(cpu_info.intel_brand_str, stringregs);
		}
		//write_serial_str("\n");
	} else if(brand > 0) {
		//printf("Brand %d - ", brand);
		//sprintf(forprint, "Brand %i - ", brand);
		//write_serial_str(forprint);
		if(brand < 0x18) {
			if(signature == 0x000006B1 || signature == 0x00000F13) {
				//printf("%s\n", Intel_Other[brand]);
				cpu_info.intel_specific_brand_other = Intel_Other[brand];
				//write_serial_str(Intel_Other[brand]);
				//write_serial_ch('\n');
			} else {
				//sprintf(forprint, "%s\n", Intel[brand]);
				cpu_info.intel_specific_brand = Intel[brand];
				//write_serial_str(forprint);
				//write_serial_str(Intel[brand]);
				//write_serial_ch('\n');
			}
		} else {
			cpu_info.intel_brand_str = "Reserved";
			//write_serial_str("Reserved\n");
		}
	}
	//printf("Stepping: %d Reserved: %d\n", stepping, reserved);
	cpu_info.intel_stepping = stepping;
	cpu_info.intel_reserved = reserved;
	//sprintf(forprint, "Stepping: %i Reserved: %i\n", stepping, reserved);
	return 0;
}

/* Print Registers */
void printregs(int eax, int ebx, int ecx, int edx) {
	int j;
	stringregs[16] = '\0';
	for(j = 0; j < 4; j++) {
		stringregs[j] = eax >> (8 * j);
		stringregs[j + 4] = ebx >> (8 * j);
		stringregs[j + 8] = ecx >> (8 * j);
		stringregs[j + 12] = edx >> (8 * j);
	}
	//dprintf("%s", stringregs);
	//printf("%s", string);
}

/* AMD-specific information */
int do_amd(void) {
	//write_serial_str("AMD Specific Features:\n");
	unsigned long extended, eax, ebx, ecx, edx, unused;
	int family, model, stepping, reserved;
	cpuidm(1, eax, unused, unused, unused);
	model = (eax >> 4) & 0xf;
	cpu_info.amd_model = model;
	family = (eax >> 8) & 0xf;
	cpu_info.amd_family = family;
	stepping = eax & 0xf;
	cpu_info.amd_stepping = stepping;
	reserved = eax >> 12;
	cpu_info.amd_reserved = reserved;
	//printf("Family: %d Model: %d [", family, model);
	//sprintf(forprint, "Family: %i Model: %i [", family, model);
	switch(family) {
		case 4:
		//printf("486 Model %d", model);
		//sprintf(forprint, "486 Model %i", model);
		cpu_info.amd_family_str = "Am486";
		break;
		case 5:
		switch(model) {
			case 0:
			case 1:
			case 2:
			case 3:
			case 6:
			case 7:
			//printf("K6 Model %d", model);
			//sprintf(forprint, "K6 Model %i", model);
			cpu_info.amd_model_str = "K6 Model";
			break;
			case 8:
			//write_serial_str("K6-2 Model 8");
			cpu_info.amd_model_str = "K6-2 Model 8";
			break;
			case 9:
			//write_serial_str("K6-III Model 9");
			cpu_info.amd_model_str = "K6-III Model 9";
			break;
			default:
			//printf("K5/K6 Model %d", model);
			//sprintf(forprint, "K5/K6 Model %i", model);
			cpu_info.amd_model_str = "K5/K6 Model";
			break;
		}
		break;
		case 6:
		switch(model) {
			case 1:
			case 2:
			case 4:
			//printf("Athlon Model %d", model);
			//sprintf(forprint, "Athlon Model %i", model);
			cpu_info.amd_model_str = "Athlon Model";
			break;
			case 3:
			//write_serial_str("Duron Model 3");
			cpu_info.amd_model_str = "Duron Model 3";
			break;
			case 6:
			//write_serial_str("Athlon MP/Mobile Athlon Model 6");
			cpu_info.amd_model_str = "Athlon MP/Mobile Athlon Model 6";
			break;
			case 7:
			//write_serial_str("Mobile Duron Model 7");
			cpu_info.amd_model_str = "Mobile Duron Model 7";
			break;
			default:
			//printf("Duron/Athlon Model %d", model);
			//sprintf(forprint, "Duron/Athlon Model %i", model);
			cpu_info.amd_model_str = "Duron/Athlon Model";
			break;
		}
		break;
	}
	//write_serial_str("]\n");
	cpuidm(0x80000000, extended, unused, unused, unused);
	if(extended == 0) {
		return 0;
	}
	strcpy(cpu_info.amd_brand_str, "");
	if(extended >= 0x80000002) {
		unsigned int j;
		//write_serial_str("Detected Processor Name: ");
		for(j = 0x80000002; j <= 0x80000004; j++) {
			cpuidm(j, eax, ebx, ecx, edx);
			printregs(eax, ebx, ecx, edx);
			strcat(cpu_info.amd_brand_str, stringregs);
		}
		//write_serial_str("\n");
	}
	if(extended >= 0x80000007) {
		cpuidm(0x80000007, unused, unused, unused, edx);
		if(edx & 1) {
			//write_serial_str("Temperature Sensing Diode Detected!\n");
		}
	}
	//printf("Stepping: %d Reserved: %d\n", stepping, reserved);
	//sprintf(forprint, "Stepping: %i Reserved: %i\n", stepping, reserved);
	return 0;
}
