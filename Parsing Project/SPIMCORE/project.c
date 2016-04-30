//CDA Project 2015
//Jose Ocasio

#include "spimcore.h"

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    // Execute The 8 ALU Operations
      if(ALUControl == 0)
      {
            *ALUresult = (A + B);
      }

      else if(ALUControl == 1)
      {
            *ALUresult = (A - B);
      }

      else if(ALUControl == 2)
      {
            if((int)A < (int)B)
            {
                  *ALUresult = 1;
            }

            else
            {
                  *ALUresult = 0;
            }
      }

      else if(ALUControl == 3)
      {
            if(A < B)
            {
                  *ALUresult = 1;
            }

            else
            {
                  *ALUresult = 0;
            }
      }

      else if(ALUControl == 4)
      {
            *ALUresult = (A & B);
      }

      else if(ALUControl == 5)
      {
            *ALUresult = (A | B);
      }

      else if(ALUControl == 6)
      {
            *ALUresult = B << 16;
      }

      else
      {
            *ALUresult = !A;
      }

      // If Result is 0, Zero is set to 1. Else, Zero is set to 0
      if(*ALUresult == 0)
      {
            *Zero = 1;
      }

      else
      {
            *Zero = 0;
      }
}



/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
    if (PC % 4 != 0)   //check if PC is word aligned
      return 1;   //halt

    else
    {
        *instruction = Mem[PC<<2];  //obtain instructions from memory address location at PC<<2
        if(*instruction == 0)
            return 1;   //halt
        else
            return 0;   //success
    }
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    // shift to the right 26 bits to obtain bits 26-31 for opcode
   *op = (instruction >> 26);

    //shift to the right 21 then and it with 0x0000001F for bits 21-25
   *r1 = ((instruction >> 21) & 0x0000001F);

    //shift to the right 16 then and it with 0x0000001F for bits 16-20
   *r2 = ((instruction >> 16) & 0x0000001F);

    //shift to the right 11 then and it with 0x0000001F for bits 11-15
   *r3 = ((instruction >> 11) & 0x0000001F);

    //and with 0x0000001F for the first 5 bits
   *funct = (instruction & 0x0000001F);

    //and with 0x00007FFF for the first 15 bits
   *offset = (instruction & 0x00007FFF);

    //and with 0x01FFFFFF in binary, for the first 25 bits
   *jsec = (instruction & 0x01FFFFFF);
}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    // R-Type
    if((op == 0))
    {
        controls->ALUOp = 7;
        controls->ALUSrc = 0;
        controls->Branch = 0;
        controls->Jump = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->MemWrite = 0;
        controls->RegDst = 1;
        controls->RegWrite = 1;
    }

    // Addi
    else if(op == 8)
    {
        controls->ALUOp = 0;
        controls->ALUSrc = 1;
        controls->Branch = 0;
        controls->Jump = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->MemWrite = 0;
        controls->RegDst = 0;
        controls->RegWrite = 1;
    }

    // beq / subtraction
    else if(op == 4)
    {
        controls->ALUOp = 1;
        controls->ALUSrc = 0;
        controls->Branch = 1;
        controls->Jump = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 2;
        controls->MemWrite = 0;
        controls->RegDst = 2;
        controls->RegWrite = 0;
    }

    // Shift
    else if(op == 15)
    {
        controls->ALUOp = 6;
        controls->ALUSrc = 1;
        controls->Branch = 0;
        controls->Jump = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->MemWrite = 0;
        controls->RegDst = 0;
        controls->RegWrite = 1;
    }

    // lw / load word
    else if(op == 35)
    {
        controls->ALUOp = 0;
        controls->ALUSrc = 1;
        controls->Branch = 0;
        controls->Jump = 0;
        controls->MemRead = 1;
        controls->MemtoReg = 1;
        controls->MemWrite = 0;
        controls->RegDst = 0;
        controls->RegWrite = 1;
    }

    // slti / set less than
    else if(op == 10)
    {
        controls->ALUOp = 2;
        controls->ALUSrc = 1;
        controls->Branch = 0;
        controls->Jump = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->MemWrite = 0;
        controls->RegDst = 0;
        controls->RegWrite = 1;
    }

    // sltiu / set less than unsigned
    else if(op == 11)
    {
        controls->ALUOp = 3;
        controls->ALUSrc = 1;
        controls->Branch = 0;
        controls->Jump = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->MemWrite = 0;
        controls->RegDst = 0;
        controls->RegWrite = 1;
    }

    // sw / save word
    else if(op == 43)
    {
        controls->ALUOp = 0;
        controls->ALUSrc = 1;
        controls->Branch = 0;
        controls->Jump = 0;
        controls->MemRead = 0;
        controls->MemtoReg = 2;
        controls->MemWrite = 1;
        controls->RegDst = 2;
        controls->RegWrite = 0;
    }

    // jump
    else if(op == 2)
    {
        controls->ALUOp = 0;
        controls->ALUSrc = 0;
        controls->Branch = 0;
        controls->Jump = 1;
        controls->MemRead = 0;
        controls->MemtoReg = 0;
        controls->MemWrite = 0;
        controls->RegDst = 0;
        controls->RegWrite = 0;
    }

   else
      return 1;

   return 0;
}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
    *data1 = Reg[r1];
    *data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{
    if(offset >> 15 == 1)   //negative offset
      *extended_value = offset + 0xFFFF0000;

    else   //positive offset
      *extended_value = offset;
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    //check the second ALU operand, is it 2nd reg file or sign-extended instruction?
    if (ALUSrc == '1')
    {
        data2 = extended_value;
    }

    //Signifies an R type instruction
    //Now to decide which ALU operation to path
    if(ALUOp == 7)
    {
		switch(funct)
		{
		    //addition
			case 0x20:
				ALUOp = 0;
				break;
            //subtraction
			case 0x22:
				ALUOp = 1;
				break;
            //slt
			case 0x2a:
				ALUOp = 2;
				break;
            //stlu
			case 0x2b:
				ALUOp = 3;
				break;
            //AND
			case 0x24:
				ALUOp = 4;
				break;
            //OR
			case 0x25:
				ALUOp = 5;
				break;
            //sll
			case 0x6:
				ALUOp = 6;
				break;
            //NOR
			case 0x27:
				ALUOp = 7;
				break;
            //otherwise halt
			default:
				return 1;
		}

        //Path data to the ALU
		ALU(data1,data2,ALUOp,ALUresult,Zero);

	}
	//non-funct ALU path
	else
		ALU(data1,data2,ALUOp,ALUresult,Zero);


    return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
    //reading in from memory
    if (MemRead == 1)
    {
        //check to see if word is properly aligned
        if ((ALUresult % 4) == 0)
            *memdata = Mem[ALUresult >> 2]; //shift to get correct data
        else
            return 1;
    }
    //writing to memory
    if (MemWrite == 1)
    {
        if((ALUresult % 4) == 0)
            Mem[ALUresult >> 2] = data2;
        else
            return 1;
    }

    return 0;
}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
    //source is ALUResult
    if( (MemtoReg == 0) && (RegWrite == 1) )
   {
      if (RegDst == 0)
         Reg[r3] = ALUresult;

      else if (RegDst == 1)
         Reg[r2] = ALUresult;
   }

   //source is Memory
   else if( (MemtoReg == 1) && (RegWrite == 1) )
   {
      if (RegDst == 1)
         Reg[r3] = memdata;

      else if (RegDst == 0)
         Reg[r2] = memdata;
   }
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
    //program counter - go to next word
    *PC += 4;

    //if a jump occurred, shift left
    if(Jump == '1')
    {
        *PC = (jsec << 2) | (*PC & 0xf0000000);
    }

    //if a branch occurred, add extended value
    if (Branch == '1' && Zero == '1')
        *PC += extended_value << 2;
}

