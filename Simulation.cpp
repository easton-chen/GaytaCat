#include "Simulation.h"
using namespace std;

extern void read_elf();
extern unsigned long long cadr; //代码段在解释文件中的偏移地址
extern unsigned long long csize; //代码段的长度
extern unsigned long long vcadr; //代码段在内存中的虚拟地址	

extern unsigned long long dadr; //数据段在解释文件中的偏移地址
extern unsigned long long dsize; //数据段的长度
extern unsigned long long bsize; //.bss和.sbss段的长度
extern unsigned long long vdadr; //数据段在内存中的虚拟地址	
extern unsigned long long gp; //全局数据段在内存的地址

extern unsigned long long madr; //main函数在内存中地址
extern unsigned long long msize;
extern unsigned long long endPC; //程序结束时的PC
extern unsigned long long entry; //程序的入口地址
extern FILE *file;


//指令运行数
long long inst_num=0;

//系统调用退出指示
int exit_flag=0;

//加载代码段
//初始化PC
void load_memory()
{
	memset(memory, 0, sizeof(memory));

	//映射.text段，cadr和csize在Read_Elf.cpp中维护
	fseek(file,cadr,SEEK_SET);
	fread(&memory[vcadr],1,csize,file);
	
	//for debugging
	printf("cadr = %x\n", cadr);
	printf("csize = %x\n", csize);
	printf("vcadr = %x\n", vcadr);
	for(int i = 0; i < csize; i++)
	{
		printf("%02x",memory[i+vcadr]);
		if((i+1)%4 == 0) printf(" ");
		if((i+1)%16 == 0) printf("\n");
	}
	printf("\n.text finished!\n\n\n");
	
	//映射.data段
	fseek(file,dadr,SEEK_SET);
	//因为.bss段在elf文件中不占用空间，所以实际映射的是.data段的大小
	//由于之前的初始化，映射后内存中数据区的后bsize个字节仍是0
	fread(&memory[vdadr],1,dsize-bsize,file);	
	
	//for debugging
	printf("dadr = %x\n", dadr);
	printf("dsize = %x\n", dsize);
	printf("vdadr = %x\n", vdadr);
	for(int i = 0; i < dsize; i++)
	{
		printf("%02x",memory[i+vdadr]);
		if((i+1)%4 == 0) printf(" ");
		if((i+1)%16==0) printf("\n");
	}
	printf("\n.data finished!\n\n\n");

	//维护一些值。
	entry = vcadr;
	endPC=madr+msize+1;
	//for debugging
	printf("entry = %x\n", entry);
	printf("gp = %x\n", gp);
	printf("madr = %x\n", madr);


	fclose(file);
}

int main()
{
	//解析elf文件
	read_elf();
	
	//加载内存
	load_memory();

	//设置入口地址
	PC=madr;
	
	//设置全局数据段地址寄存器
	reg[3]=gp;
	
	reg[2]=MAX/2;//栈基址 （sp寄存器）

	simulate();

	cout <<"simulate over!"<<endl;

	return 0;
}

void simulate()
{
	//结束PC的设置
	//int end=(int)endPC/4-1;
	while(PC!=endPC)
	{
		//Decoder* decoder = (Decoder*) malloc(sizeof(Decoder));
		//memset(decoder, 0, sizeof(Decoder));
		//运行
		IF();
		ID();
		EX();
		MEM();
		WB();

		//更新中间寄存器
		IF_ID=IF_ID_old;
		ID_EX=ID_EX_old;
		EX_MEM=EX_MEM_old;
		MEM_WB=MEM_WB_old;

        if(exit_flag==1)
            break;

        reg[0]=0;//一直为零

        //free(decoder);
	}
}


//取指令
void IF()
{
	//write IF_ID_old
	//IF_ID_old.inst=memory[PC];
	memcpy(&IF_ID_old.inst,memory+PC,4);
	PC=PC+4;
	IF_ID_old.PC=PC;
}

//译码
void ID()
{
	//Read IF_ID
	unsigned int inst=IF_ID.inst;

	int EXTop=0;
	unsigned int EXTsrc=0;

	char RegDst,ALUop,ALUSrc;
	char Branch,MemRead,MemWrite;
	char RegWrite,MemtoReg;

	unsigned int OP=getbit(inst,25,31);
	unsigned int rs;
	unsigned int rt;
	unsigned int rd;
	unsigned int fuc3;
	unsigned int fuc7;
	unsigned int Imm=0;

	//rd=getbit(inst,7,11);
	//fuc3=getbit(inst,0,0);
	//....


	if(OP==OP_R)//R 0x33
	{
		fuc7=getbit(inst,0,6);
		rt=getbit(inst,7,11);
		rs=getbit(inst,12,16);
		fuc3=getbit(inst,17,19);
		rd=getbit(inst,20,24);

		EXTop=0;
		RegDst=1;
		ALUop=0;
		ALUSrc=0;
		Branch=0;
		MemRead=0;
		MemWrite=0;
		RegWrite=1;
		MemtoReg=0;

		switch(fuc3){
			case 0:
				switch(fuc7){
					case 0x00:
						ALUop=1;// R[rd] ← R[rs1] + R[rs2]
						break;
					case 0x01:
						ALUop=2;//R[rd] ← (R[rs1] * R[rs2])[31:0]
						break;
					case 0x20:
						ALUop=3;//R[rd] ← R[rs1] - R[rs2]
						break;
					default: printf("Illegal Instruction\n"); break;
				}
				break;
			case 1:
				if(fuc7 == 0x00) ALUop=4;//R[rd] ← R[rs1] << R[rs2]
				else if(fuc7==0x01) ALUop=5;//R[rd] ← (R[rs1] * R[rs2])[63:32]
				else printf("Illegal Instruction\n");
				break;
			case 2:
				if(fuc7==0x00) ALUop=6;//R[rd] ← (R[rs1] < R[rs2]) ? 1 : 0
				else printf("Illegal Instruction\n");
				break;
			case 4:
				if(fuc7 == 0x00) ALUop=7;//R[rd] ← R[rs1] ^ R[rs2]
				else if(fuc7==0x01) ALUop=8;//R[rd] ← R[rs1] / R[rs2]
				else printf("Illegal Instruction\n");
				break;
			case 5:
				if(fuc7 == 0x00||fuc7==0x02) ALUop=9;//R[rd] ← R[rs1] >> R[rs2]
				else printf("Illegal Instruction\n");
				break;
			case 6:
				if(fuc7 == 0x00) ALUop=10;//R[rd] ← R[rs1] | R[rs2]
				else if(fuc7==0x01) ALUop=11;//R[rd] ← R[rs1] % R[rs2]
				else printf("Illegal Instruction\n");
				break;
			case 7:
				if(fuc7==0x00) ALUop=12;//R[rd] ← R[rs1] & R[rs2]
				else printf("Illegal Instruction\n");
				break;
			default: 
				printf("Illegal Instruction\n");
				break;
		}
	}
	else if(OP==OP_I)//I 0x13
    {
    	Imm=getbit(inst,0,11);
    	rs=getbit(inst,12,16);
    	fuc3=getbit(inst,17,19);
    	rd=getbit(inst,20,24);

    		EXTop=0;
		RegDst=1;
		ALUop=0;
		ALUSrc=1;
		Branch=0;
		MemRead=0;
		MemWrite=0;
		RegWrite=1;
		MemtoReg=0;

		switch(fuc3){
			case 0:
				ALUop=17;//R[rd] ← R[rs1] + imm
				break;
			case 1:
				if(fuc7==0x00)
				ALUop=18;//R[rd] ← R[rs1] << imm
				else printf("Illegal Instruction\n");
				break;
			case 2:
				ALUop=19;//R[rd] ← (R[rs1] < imm) ? 1 : 0
				break;
			case 4:
				ALUop=20;//R[rd] ← R[rs1] ^ imm
				break;
			case 5:
				if(fuc7 == 0x00||fuc7==0x20) ALUop=21;//R[rd] ← R[rs1] >> imm
				else printf("Illegal Instruction\n");
				break;
			case 6:
				
				ALUop=22;//R[rd] ← R[rs1] | imm
				break;
			case 7:		
				ALUop=23;//R[rd] ← R[rs1] & imm
				break;
			default:
				printf("Illegal Instruction\n"); 
				break;
		}
    }
    else if(OP==OP_SW)//S 0x23 
    {
        Imm=getbit(inst,0,6)<<5 + getbit(inst,20,24);
        rt=getbit(inst,7,11);
        rs=getbit(inst,12,16);
        fuc3=getbit(inst,17,19);

    		EXTop=1;
		RegDst=0;
		ALUop=0;
		ALUSrc=1;
		Branch=0;
		MemRead=0;
		MemWrite=1;
		RegWrite=0;
		MemtoReg=0;

		switch(fuc3){
			case 0:
				ALUop=30;//Mem(R[rs1] + offset) ← R[rs2][7:0]
				break;
			case 1:
				ALUop=31;//Mem(R[rs1] + offset) ← R[rs2][15:0]
				break;
			case 2:
				ALUop=32;//Mem(R[rs1] + offset) ← R[rs2][31:0]
				break;
			case 3:
				ALUop=33;//Mem(R[rs1] + offset) ← R[rs2][63:0]
				break;
			default: printf("Illegal Instruction\n"); break;
		}
    }
    else if(OP==OP_LW)//I 0x03  The LW instruction loads a 32-bit value from memory and sign-extends this to 64 bits before storing
it in register rd for RV64I.
    {
        Imm=getbit(inst,0,11);
    	rs=getbit(inst,12,16);
    	fuc3=getbit(inst,17,19);
    	rd=getbit(inst,20,24);

    	EXTop=1;//sign-extend
		RegDst=1;
		ALUop=0;
		ALUSrc=1;
		Branch=0;
		MemRead=1;
		MemWrite=0;
		RegWrite=1;
		MemtoReg=1;

		switch(fuc3){
			case 0:
				ALUop=13;
				break;
			case 1:
				ALUop=14;
				break;
			case 2:
				ALUop=15;
				break;
			case 3:
				ALUop=16;
				break;
			default: break;
		}
    }
    else if(OP==OP_BEQ)
    {
        Imm=getbit(inst,0,0)<<12 + getbit(inst,24,24)<<11 + getbit(inst,1,6)<<5 + getbit(inst,20,23)<<1;
        rt=getbit(inst,7,11);
        rs=getbit(inst,12,16);
        fuc3=getbit(inst,17,19);

        EXTop=1;
		RegDst=0;
		ALUop=0;
		ALUSrc=1;
		Branch=1;
		MemRead=0;
		MemWrite=0;
		RegWrite=0;
		MemtoReg=0;

		switch(fuc3){
			case 0:
				ALUop=33;
				break;
			case 1:
				ALUop=34;
				break;
			case 4:
				ALUop=35;
				break;
			case 5:
				ALUop=36;
				break;
			default: break;
		}
    }
    else if(OP==OP_JAL)
    {
        Imm=getbit(inst,0,0)<<20 + getbit(inst,12,19)<<12 + getbit(inst,11,11)<<11 + getbit(inst,1,10)<<1;
        rd=getbit(inst,20,24);

        EXTop=1;
		RegDst=1;
		ALUop=39;
		ALUSrc=1;
		Branch=1;
		MemRead=0;
		MemWrite=0;
		RegWrite=1;
		MemtoReg=0;
    }
    else if(OP==OP_IW)
    {
		Imm=getbit(inst,0,11);
    	rs=getbit(inst,12,16);
    	fuc3=getbit(inst,17,19);
    	rd=getbit(inst,20,24);

    	EXTop=1;
		RegDst=1;
		ALUop=26;
		ALUSrc=1;
		Branch=0;
		MemRead=0;
		MemWrite=0;
		RegWrite=1;
		MemtoReg=0;
    }
    else if(OP==OP_JALR)
    {
    	Imm=getbit(inst,0,11);
    	rs=getbit(inst,12,16);
    	fuc3=getbit(inst,17,19);
    	rd=getbit(inst,20,24);

    	EXTop=1;
		RegDst=1;
		ALUop=27;
		ALUSrc=1;
		Branch=1;
		MemRead=0;
		MemWrite=0;
		RegWrite=1;
		MemtoReg=0;
    }
    else if(OP==OP_SCALL)
    {
    	Imm=getbit(inst,0,11);
    	rs=getbit(inst,12,16);
    	fuc3=getbit(inst,17,19);
    	rd=getbit(inst,20,24);

    	EXTop=0;
		RegDst=0;
		ALUop=28;
		ALUSrc=0;
		Branch=0;
		MemRead=0;
		MemWrite=0;
		RegWrite=0;
		MemtoReg=0;
    }
    else if(OP==OP_AUIPC)
    {
    	Imm=getbit(inst,0,19)<<12;
    	rd=getbit(inst,20,24);

    	EXTop=1;
		RegDst=1;
		ALUop=37;
		ALUSrc=1;
		Branch=0;
		MemRead=0;
		MemWrite=0;
		RegWrite=1;
		MemtoReg=0;
    }
    else if(OP==OP_LUI)
    {
    	Imm=getbit(inst,0,19)<<12;
    	rd=getbit(inst,20,24);
    	
    	EXTop=1;
		RegDst=1;
		ALUop=38;
		ALUSrc=1;
		Branch=0;
		MemRead=0;
		MemWrite=0;
		RegWrite=1;
		MemtoReg=0;
    }
    else{}

	//write ID_EX_old
	ID_EX_old.Rd=rd;
	ID_EX_old.Rt=rt;
	ID_EX_old.Imm=ext_signed(EXTsrc,EXTop);
	//...

	ID_EX_old.Ctrl_EX_ALUSrc=ALUSrc;
	ID_EX_old.Ctrl_EX_ALUOp=ALUop;
	ID_EX_old.Ctrl_EX_RegDst=RegDst;

	ID_EX_old.Ctrl_M_Branch=Branch;
	ID_EX_old.Ctrl_M_MemWrite=MemWrite;
	ID_EX_old.Ctrl_M_MemRead=MemRead;

	ID_EX_old.Ctrl_WB_RegWrite=RegWrite;
	ID_EX_old.Ctrl_WB_MemtoReg=MemtoReg;

}

//执行
void EX()
{
	//read ID_EX
	int temp_PC=ID_EX.PC;
	char RegDst=ID_EX.Ctrl_EX_RegDst;
	char ALUOp=ID_EX.Ctrl_EX_ALUOp;

	//Branch PC calulate
	//...

	//choose ALU input number
	//...

	//alu calculate
	int Zero;
	REG ALUout;
	switch(ALUOp){
	default:;
	}

	//choose reg dst address
	int Reg_Dst;
	if(RegDst)
	{

	}
	else
	{

	}



	//write EX_MEM_old
	EX_MEM_old.ALU_out=ALUout;
	EX_MEM_old.PC=temp_PC;
    //.....
}

//访问存储器
void MEM()
{
	//read EX_MEM

	//complete Branch instruction PC change

	//read / write memory

	//write MEM_WB_old
}


//写回
void WB()
{
	//read MEM_WB

	//write reg
}
