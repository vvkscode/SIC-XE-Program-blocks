#include <bits/stdc++.h>
using namespace std;

struct optab
{
  string opcode;
  int exist;
  int frmt;
  optab()
  {
    opcode = "undefined";
    frmt = 0;
    exist = 0;
  }

  optab(string opc, int format, int dexist)
  {
    opcode = opc;
    frmt = format;
    exist = dexist;
  }
};

struct regtab
{
  char regnum;
  int exist;
  regtab()
  {
    regnum = 'F';
    exist = 0;
  }
  regtab(char num, int dexist)
  {
    regnum = num;
    exist = dexist;
  }
};

struct labeltab
{
  string address;
  string name;
  int reltve;
  int blocknum;
  int exist;
  labeltab()
  {
    name = "undefined";
    address = "0";
    blocknum = 0;
    exist = 0;
    reltve = 0;
  }
};

struct littab
{
  string value;
  string address;
  int exist;
  int blocknum = 0;
  littab()
  {
    value = "";
    address = "?";
    blocknum = 0;
    exist = 0;
  }
};

struct blocktab
{
  string startAddress;
  string name;
  string LOCCTR;
  int number;
  int exist;
  blocktab()
  {
    name = "undefined";
    startAddress = "?";
    exist = 0;
    number = -1;
    LOCCTR = "0";
  }
}; 

typedef map<string, labeltab> SYMBOL_TABLE;
typedef map<string, optab> OPCODE_TABLE;
typedef map<string, regtab> REG_TABLE;
typedef map<string, littab> LIT_TABLE;
typedef map<string, blocktab> BLOCK_TABLE;

SYMBOL_TABLE SYMTAB;
OPCODE_TABLE OPTAB;
REG_TABLE REGTAB;
LIT_TABLE LITTAB;
BLOCK_TABLE BLOCKS;

void loadregtable()
{
  regtab new_reg1('0', 1);
  REGTAB["A"] = new_reg1;
  regtab new_reg2('1', 1);
  REGTAB["X"] = new_reg2;
  regtab new_reg3('2', 1);
  REGTAB["L"] = new_reg3;
  regtab new_reg4('3', 1);
  REGTAB["B"] = new_reg4;
  regtab new_reg5('4', 1);
  REGTAB["S"] = new_reg5;
  regtab new_reg6('5', 1);
  REGTAB["T"] = new_reg6;
  regtab new_reg7('6', 1);
  REGTAB["F"] = new_reg7;
  regtab new_reg8('8', 1);
  REGTAB["PC"] = new_reg8;
  regtab new_reg9('9', 1);
  REGTAB["SW"] = new_reg9;
}

void load_block()
{
  BLOCKS["DEFAULT"].exist = 1;
  BLOCKS["DEFAULT"].name = "DEFAULT";
  BLOCKS["DEFAULT"].startAddress = "00000";
  BLOCKS["DEFAULT"].number = 0;
  BLOCKS["DEFAULT"].LOCCTR = "0";
}

void load_op()
{
  optab new_info1("18", 3, 1);
  OPTAB["ADD"] = new_info1;
  optab new_info2("58", 3, 1);
  OPTAB["ADDF"] = new_info2;
  optab new_info3("90", 2, 1);
  OPTAB["ADDR"] = new_info3;
  optab new_info4("40", 3, 1);
  OPTAB["AND"] = new_info4;
  optab new_info5("B4", 2, 1);
  OPTAB["CLEAR"] = new_info5;
  optab new_info6("28", 3, 1);
  OPTAB["COMP"] = new_info6;
  optab new_info7("88", 3, 1);
  OPTAB["COMPF"] = new_info7;
  optab new_info8("A0", 2, 1);
  OPTAB["COMPR"] = new_info8;
  optab new_info9("24", 3, 1);
  OPTAB["DIV"] = new_info9;
  optab new_info10("64", 3, 1);
  OPTAB["DIVF"] = new_info10;
  optab new_info11("9C", 2, 1);
  OPTAB["DIVR"] = new_info11;
  optab new_info12("C4", 1, 1);
  OPTAB["FIX"] = new_info12;
  optab new_info13("C0", 1, 1);
  OPTAB["FLOAT"] = new_info13;
  optab new_info14("F4", 1, 1);
  OPTAB["HIO"] = new_info14;
  optab new_info15("3C", 3, 1);
  OPTAB["J"] = new_info15;
  optab new_info16("30", 3, 1);
  OPTAB["JEQ"] = new_info16;
  optab new_info17("34", 3, 1);
  OPTAB["JGT"] = new_info17;
  optab new_info18("38", 3, 1);
  OPTAB["JLT"] = new_info18;
  optab new_info19("48", 3, 1);
  OPTAB["JSUB"] = new_info19;
  optab new_info20("00", 3, 1);
  OPTAB["LDA"] = new_info20;
  optab new_info21("68", 3, 1);
  OPTAB["LDB"] = new_info21;
  optab new_info22("50", 3, 1);
  OPTAB["LDCH"] = new_info22;
  optab new_info23("70", 3, 1);
  OPTAB["LDF"] = new_info23;
  optab new_info24("08", 3, 1);
  OPTAB["LDL"] = new_info24;
  optab new_info25("6C", 3, 1);
  OPTAB["LDS"] = new_info25;
  optab new_info26("74", 3, 1);
  OPTAB["LDT"] = new_info26;
  optab new_info27("04", 3, 1);
  OPTAB["LDX"] = new_info27;
  optab new_info28("D0", 3, 1);
  OPTAB["LPS"] = new_info28;
  optab new_info29("20", 3, 1);
  OPTAB["MUL"] = new_info29;
  optab new_info30("60", 3, 1);
  OPTAB["MULF"] = new_info30;
  optab new_info31("98", 2, 1);
  OPTAB["MULR"] = new_info31;
  optab new_info32("C8", 1, 1);
  OPTAB["NORM"] = new_info32;
  optab new_info33("44", 3, 1);
  OPTAB["OR"] = new_info33;
  optab new_info34("D8", 3, 1);
  OPTAB["RD"] = new_info34;
  optab new_info35("AC", 2, 1);
  OPTAB["RMO"] = new_info35;
  optab new_info36("4F", 3, 1);
  OPTAB["RSUB"] = new_info36;
  optab new_info37("A4", 2, 1);
  OPTAB["SHIFTL"] = new_info37;
  optab new_info38("A8", 2, 1);
  OPTAB["SHIFTR"] = new_info38;
  optab new_info39("F0", 1, 1);
  OPTAB["SIO"] = new_info39;
  optab new_info40("EC", 3, 1);
  OPTAB["SSK"] = new_info40;
  optab new_info41("0C", 3, 1);
  OPTAB["STA"] = new_info41;
  optab new_info42("78", 3, 1);
  OPTAB["STB"] = new_info42;
  optab new_info43("54", 3, 1);
  OPTAB["STCH"] = new_info43;
  optab new_info44("80", 3, 1);
  OPTAB["STF"] = new_info44;
  optab new_info45("D4", 3, 1);
  OPTAB["STI"] = new_info45;
  optab new_info46("14", 3, 1);
  OPTAB["STL"] = new_info46;
  optab new_info47("7C", 3, 1);
  OPTAB["STS"] = new_info47;
  optab new_info48("E8", 3, 1);
  OPTAB["STSW"] = new_info48;
  optab new_info49("84", 3, 1);
  OPTAB["STT"] = new_info49;
  optab new_info50("10", 3, 1);
  OPTAB["STX"] = new_info50;
  optab new_info51("1C", 3, 1);
  OPTAB["SUB"] = new_info51;
  optab new_info52("5C", 3, 1);
  OPTAB["SUBF"] = new_info52;
  optab new_info53("94", 2, 1);
  OPTAB["SUBR"] = new_info53;
  optab new_info54("B0", 2, 1);
  OPTAB["SVC"] = new_info54;
  optab new_info55("E0", 3, 1);
  OPTAB["TD"] = new_info55;
  optab new_info56("F8", 1, 1);
  OPTAB["TIO"] = new_info56;
  optab new_info57("2C", 3, 1);
  OPTAB["TIX"] = new_info57;
  optab new_info58("B8", 2, 1);
  OPTAB["TIXR"] = new_info58;
  optab new_info59("DC", 3, 1);
  OPTAB["WD"] = new_info59;
}

void load_all_tables()
{
  loadregtable();
  load_block();
  load_op();
}
