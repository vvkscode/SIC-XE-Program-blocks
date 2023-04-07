#include "methods.cpp"
#include "alltables.cpp"
#include <bits/stdc++.h>
#define endl '\n'
using namespace std;

string fileName, *block_num_name, first_exe_section;
bool errorFlagraised = false;
int programLength;



void LTORGPP(string &p_lit, int &l_num_delta, int l_num, int &LOCCTR, int &lastDeltaLOCCTR, int currBlockNum)
{
  string addr_lit, value_literal;
  p_lit = "";
  for (auto const &it : LITTAB)
  {
    addr_lit = it.second.address;
    value_literal = it.second.value;
    if (addr_lit != "?")
    {
      
    }
    else
    {
      l_num += 5;
      l_num_delta += 5;
      LITTAB[it.first].address = intToHexString(LOCCTR);
      LITTAB[it.first].blocknum = currBlockNum;
      p_lit += "\n" + to_string(l_num) + "\t" + intToHexString(LOCCTR) + "\t" + to_string(currBlockNum) + "\t" + "*" + "\t" + "=" + value_literal + "\t" + " " + "\t" + " ";

      if (value_literal[0] == 'X')
      {
        LOCCTR += (value_literal.length() - 3) / 2;
        lastDeltaLOCCTR += (value_literal.length() - 3) / 2;
      }
      else if (value_literal[0] == 'C')
      {
        LOCCTR += value_literal.length() - 3;
        lastDeltaLOCCTR += value_literal.length() - 3;
      }
    }
  }
}

void evaluate(string exprsn, bool &reltve, string &tempOpnd, int l_num, ofstream &error_file, bool &errorFlagraised)
{
  string singleOpnd = "?", singleOptr = "?", stringval = "", tempVal = "", wr_data = "";
  int lastOperand = 0, lastOperator = 0, numPairs = 0;
  char prevByte = ' ';
  bool illegal_exp = false;

  for (int i = 0; i < exprsn.length();)
  {
    singleOpnd = "";

    prevByte = exprsn[i];
    while ((prevByte != '+' && prevByte != '-' && prevByte != '/' && prevByte != '*') && i < exprsn.length())
    {
      singleOpnd += prevByte;
      prevByte = exprsn[++i];
    }

    if (SYMTAB[singleOpnd].exist == 1)
    { 
      lastOperand = SYMTAB[singleOpnd].reltve;
      tempVal = to_string(hexaatoint(SYMTAB[singleOpnd].address));
    }
    else if ((singleOpnd != "" || singleOpnd != "?") && ifallnum(singleOpnd))
    {
      lastOperand = 0;
      tempVal = singleOpnd;
    }
    else
    {
      wr_data = "Line: " + to_string(l_num) + " : Can't find symbol. Instead found " + singleOpnd;
      write_in_the_file(error_file, wr_data);
      illegal_exp = true;
      break;
    }

    if (lastOperand * lastOperator == 1)
    {
      wr_data = "Line: " + to_string(l_num) + " : illegal expression";
      write_in_the_file(error_file, wr_data);
      errorFlagraised = true;
      illegal_exp = true;
      break;
    }
    else if ((singleOptr == "-" || singleOptr == "+" || singleOptr == "?") && lastOperand == 1)
    {
      if (singleOptr == "-")
      {
        numPairs--;
      }
      else
      {
        numPairs++;
      }
    }

    stringval += tempVal;

    singleOptr = "";
    while (i < exprsn.length() && (prevByte == '+' || prevByte == '-' || prevByte == '/' || prevByte == '*'))
    {
      singleOptr += prevByte;
      prevByte = exprsn[++i];
    }

    if (singleOptr.length() > 1)
    {
      wr_data = "Line: " + to_string(l_num) + " : ILLEGAL OPERATOR IN EXPRESSION. FOUND " + singleOptr;
      write_in_the_file(error_file, wr_data);
      errorFlagraised = true;
      illegal_exp = true;
      break;
    }

    if (singleOptr == "*" || singleOptr == "/")
    {
      lastOperator = 1;
    }
    else
    {
      lastOperator = 0;
    }

    stringval += singleOptr;
  }

  if (!illegal_exp)
  {
    if (numPairs == 1)
    { 
      reltve = 1;
      string_evaluate tempOBJ(stringval);
      tempOpnd = intToHexString(tempOBJ.get_result());
    }
    else if (numPairs == 0)
    { 
      reltve = 0;
      cout << stringval << endl;
      string_evaluate tempOBJ(stringval);
      tempOpnd = intToHexString(tempOBJ.get_result());
    }
    else
    {
      wr_data = "Line: " + to_string(l_num) + " : ILLEGAL EXPRESSION";
      write_in_the_file(error_file, wr_data);
      errorFlagraised = true;
      tempOpnd = "00000";
      reltve = 0;
    }
  }
  else
  {
    tempOpnd = "00000";
    errorFlagraised = true;
    reltve = 0;
  }
}
void p1()
{
  ifstream sourceFile;
  ofstream inter_file, error_file;

  sourceFile.open(fileName);
  if (!sourceFile)
  {
    cout << "Can't find file : " << fileName << endl;
    exit(1);
  }

  inter_file.open("Intermediate of " + fileName);
  if (!inter_file)
  {
    cout << "Unable to open file: Intermediate of " << fileName << endl;
    exit(1);
  }
  write_in_the_file(inter_file, "Line\tAddress\tprog_block\tOPCODE\tOPERAND\tComment");
  error_file.open("Errors in " + fileName);
  if (!error_file)
  {
    cout << "Unable to open file: Errors in " << fileName << endl;
    exit(1);
  }
  write_in_the_file(error_file, "ERRORS OF PASS1:");

  string fileLine;
  string wr_data, wr_dataSuffix = "", wr_dataPrefix = "";
  int ind = 0;

  string currentBlockName = "DEFAULT";
  int currBlockNum = 0;
  int t_blocks = 1;

  bool statusCode;
  string prog_block, opcode, operand, comment;
  string tempOpnd;

  int startAddress, LOCCTR, saveLOCCTR, l_num, lastDeltaLOCCTR, l_num_delta = 0;
  l_num = 0;
  lastDeltaLOCCTR = 0;

  getline(sourceFile, fileLine);
  l_num += 5;
  while (checkComment(fileLine))
  {
    wr_data = to_string(l_num) + "\t" + fileLine;
    write_in_the_file(inter_file, wr_data);
    getline(sourceFile, fileLine); 
    l_num += 5;
    ind = 0;
  }

  firstletter(fileLine, ind, statusCode, prog_block);
  firstletter(fileLine, ind, statusCode, opcode);

  if (opcode == "START")
  {
    firstletter(fileLine, ind, statusCode, operand);
    firstletter(fileLine, ind, statusCode, comment, true);
    startAddress = hexaatoint(operand);
    // cout<<startAddress<<endl;
    LOCCTR = startAddress;
    wr_data = to_string(l_num) + "\t" + intToHexString(LOCCTR - lastDeltaLOCCTR) + "\t" + to_string(currBlockNum) + "\t" + prog_block + "\t" + opcode + "\t" + operand + "\t" + comment;
    write_in_the_file(inter_file, wr_data); 

    getline(sourceFile, fileLine); 
    l_num += 5;
    ind = 0;
    firstletter(fileLine, ind, statusCode, prog_block);  // Parse prog_block
    firstletter(fileLine, ind, statusCode, opcode); 
  }
  else
  {
    startAddress = 0;
    LOCCTR = 0;
  }
 
  string currentSecTitle = "DEFAULT";
  int secCntr = 0;

  while (opcode != "END")
  {
 

    while (opcode != "END")
    {

    
      if (!checkComment(fileLine))
      {
        if (prog_block != "")
        { // prog_block found
          if (SYMTAB[prog_block].exist == 0)
          {
            SYMTAB[prog_block].name = prog_block;
            SYMTAB[prog_block].address = intToHexString(LOCCTR);
            SYMTAB[prog_block].reltve = 1;
            SYMTAB[prog_block].exist = 1;
            SYMTAB[prog_block].blocknum = currBlockNum;
          }
          else
          {
            wr_data = "Line: " + to_string(l_num) + " : Duplicate symbol for '" + prog_block + "'. Previously defined at " + SYMTAB[prog_block].address;
            write_in_the_file(error_file, wr_data);
            errorFlagraised = true;
          }
        }
        if (OPTAB[final_op_code(opcode)].exist == 1)
        { 
          if (OPTAB[final_op_code(opcode)].frmt == 3)
          {
            LOCCTR += 3;
            lastDeltaLOCCTR += 3;
            if (getFlagformat(opcode) == '+')
            {
              LOCCTR += 1;
              lastDeltaLOCCTR += 1;
            }
            if (final_op_code(opcode) == "RSUB")
            {
              operand = " ";
            }
            else
            {
              firstletter(fileLine, ind, statusCode, operand);
              if (operand[operand.length() - 1] == ',')
              {
                firstletter(fileLine, ind, statusCode, tempOpnd);
                operand += tempOpnd;
              }
            }

            if (getFlagformat(operand) == '=')
            {
              tempOpnd = operand.substr(1, operand.length() - 1);
              if (tempOpnd == "*")
              {
                tempOpnd = "X'" + intToHexString(LOCCTR - lastDeltaLOCCTR, 6) + "'";
              }
              if (LITTAB[tempOpnd].exist == 0)
              {
                LITTAB[tempOpnd].value = tempOpnd;
                LITTAB[tempOpnd].exist = 1;
                LITTAB[tempOpnd].address = "?";
                LITTAB[tempOpnd].blocknum = -1;
              }
            }
          }
          else if (OPTAB[final_op_code(opcode)].frmt == 1)
          {
            operand = " ";
            LOCCTR += OPTAB[final_op_code(opcode)].frmt;
            lastDeltaLOCCTR += OPTAB[final_op_code(opcode)].frmt;
          }
          else
          {
            LOCCTR += OPTAB[final_op_code(opcode)].frmt;
            lastDeltaLOCCTR += OPTAB[final_op_code(opcode)].frmt;
            firstletter(fileLine, ind, statusCode, operand);
            if (operand[operand.length() - 1] == ',')
            {
              firstletter(fileLine, ind, statusCode, tempOpnd);
              operand += tempOpnd;
            }
          }
        }

        else if (opcode == "WORD")
        {
          firstletter(fileLine, ind, statusCode, operand);
          LOCCTR += 3;
          lastDeltaLOCCTR += 3;
        }
        else if (opcode == "RESW")
        {
          firstletter(fileLine, ind, statusCode, operand);
          LOCCTR += 3 * stoi(operand);
          lastDeltaLOCCTR += 3 * stoi(operand);
        }
        else if (opcode == "RESB")
        {
          firstletter(fileLine, ind, statusCode, operand);
          LOCCTR += stoi(operand);
          lastDeltaLOCCTR += stoi(operand);
        }
        else if (opcode == "BYTE")
        {
          read_byte_oper(fileLine, ind, statusCode, operand);
          if (operand[0] == 'X')
          {
            LOCCTR += (operand.length() - 3) / 2;
            lastDeltaLOCCTR += (operand.length() - 3) / 2;
          }
          else if (operand[0] == 'C')
          {
            LOCCTR += operand.length() - 3;
            lastDeltaLOCCTR += operand.length() - 3;
          }
        }
        else if (opcode == "BASE")
        {
          firstletter(fileLine, ind, statusCode, operand);
        }
        else if (opcode == "LTORG")
        {
          operand = " ";
          LTORGPP(wr_dataSuffix, l_num_delta, l_num, LOCCTR, lastDeltaLOCCTR, currBlockNum);
        }
        else if (opcode == "ORG")
        {
          firstletter(fileLine, ind, statusCode, operand);

          char prevByte = operand[operand.length() - 1];
          while (prevByte == '+' || prevByte == '-' || prevByte == '/' || prevByte == '*')
          {
            firstletter(fileLine, ind, statusCode, tempOpnd);
            operand += tempOpnd;
            prevByte = operand[operand.length() - 1];
          }

          int tempVariable;
          tempVariable = saveLOCCTR;
          saveLOCCTR = LOCCTR;
          LOCCTR = tempVariable;

          if (SYMTAB[operand].exist == 1)
          {
            LOCCTR = hexaatoint(SYMTAB[operand].address);
          }
          else
          {
            bool reltve;
            errorFlagraised = false;
            evaluate(operand, reltve, tempOpnd, l_num, error_file, errorFlagraised);
            if (!errorFlagraised)
            {
              LOCCTR = hexaatoint(tempOpnd);
            }
            errorFlagraised = false;
          }
        }
        else if (opcode == "USE")
        {

          firstletter(fileLine, ind, statusCode, operand);
          if(statusCode==false){
            operand = "DEFAULT";
          }
          BLOCKS[currentBlockName].LOCCTR = intToHexString(LOCCTR);
        
          if (BLOCKS[operand].exist == 0)
          {
            BLOCKS[operand].exist = 1;
            BLOCKS[operand].name = operand;
            BLOCKS[operand].number = t_blocks++;
            BLOCKS[operand].LOCCTR = "0";
          }
          currBlockNum = BLOCKS[operand].number;
          currentBlockName = BLOCKS[operand].name;
          LOCCTR = hexaatoint(BLOCKS[operand].LOCCTR);
        }
        else if (opcode == "EQU")
        {
          firstletter(fileLine, ind, statusCode, operand);
          tempOpnd = "";
          bool reltve;

          if (operand == "*")
          {
            tempOpnd = intToHexString(LOCCTR - lastDeltaLOCCTR, 6);
            reltve = 1;
          }
          else if (ifallnum(operand))
          {
            tempOpnd = intToHexString(stoi(operand), 6);
            reltve = 0;
          }
          else
          {
            char prevByte = operand[operand.length() - 1];

            while (prevByte == '+' || prevByte == '-' || prevByte == '/' || prevByte == '*')
            {
              firstletter(fileLine, ind, statusCode, tempOpnd);
              operand += tempOpnd;
              prevByte = operand[operand.length() - 1];
            }
           
            evaluate(operand, reltve, tempOpnd, l_num, error_file, errorFlagraised);
          }

          SYMTAB[prog_block].name = prog_block;
          SYMTAB[prog_block].address = tempOpnd;
          SYMTAB[prog_block].reltve = reltve;
          SYMTAB[prog_block].blocknum = currBlockNum;
          lastDeltaLOCCTR = LOCCTR - hexaatoint(tempOpnd);
        }
        else
        {
          firstletter(fileLine, ind, statusCode, operand);
          wr_data = "Line: " + to_string(l_num) + " : Invalid OPCODE. Found " + opcode;
          write_in_the_file(error_file, wr_data);
          errorFlagraised = true;
        }
        firstletter(fileLine, ind, statusCode, comment, true);
        if (opcode == "EQU" && SYMTAB[prog_block].reltve == 0)
        {
          wr_data = wr_dataPrefix + to_string(l_num) + "\t" + intToHexString(LOCCTR - lastDeltaLOCCTR) + "\t" + " " + "\t" + prog_block + "\t" + opcode + "\t" + operand + "\t" + comment + wr_dataSuffix;
        }
        else
        {
          wr_data = wr_dataPrefix + to_string(l_num) + "\t" + intToHexString(LOCCTR - lastDeltaLOCCTR) + "\t" + to_string(currBlockNum) + "\t" + prog_block + "\t" + opcode + "\t" + operand + "\t" + comment + wr_dataSuffix;
        }
        wr_dataPrefix = "";
        wr_dataSuffix = "";
      }
      else
      {
        wr_data = to_string(l_num) + "\t" + fileLine;
      }
      write_in_the_file(inter_file, wr_data);

      BLOCKS[currentBlockName].LOCCTR = intToHexString(LOCCTR);  
      getline(sourceFile, fileLine);                          
      l_num += 5 + l_num_delta;
      l_num_delta = 0;
      ind = 0;
      lastDeltaLOCCTR = 0;
      firstletter(fileLine, ind, statusCode, prog_block);  
      firstletter(fileLine, ind, statusCode, opcode); 
    }
  

    if (opcode != "END")
    {

      if (SYMTAB[prog_block].exist == 0)
      {
        SYMTAB[prog_block].name = prog_block;
        SYMTAB[prog_block].address = intToHexString(LOCCTR);
        SYMTAB[prog_block].reltve = 1;
        SYMTAB[prog_block].exist = 1;
        SYMTAB[prog_block].blocknum = currBlockNum;
      }

      write_in_the_file(inter_file, wr_dataPrefix + to_string(l_num) + "\t" + intToHexString(LOCCTR - lastDeltaLOCCTR) + "\t" + to_string(currBlockNum) + "\t" + prog_block + "\t" + opcode);

      getline(sourceFile, fileLine); 
      l_num += 5;

      firstletter(fileLine, ind, statusCode, prog_block);  // Parse prog_block
      firstletter(fileLine, ind, statusCode, opcode); 
    }
  }

  if (opcode == "END")
  {
    first_exe_section = SYMTAB[prog_block].address;
    SYMTAB[first_exe_section].name = prog_block;
    SYMTAB[first_exe_section].address = first_exe_section;
  }

  firstletter(fileLine, ind, statusCode, operand);
  firstletter(fileLine, ind, statusCode, comment, true);


  currentBlockName = "DEFAULT";
  currBlockNum = 0;
  LOCCTR = hexaatoint(BLOCKS[currentBlockName].LOCCTR);

  LTORGPP(wr_dataSuffix, l_num_delta, l_num, LOCCTR, lastDeltaLOCCTR, currBlockNum);

  wr_data = to_string(l_num) + "\t" + intToHexString(LOCCTR - lastDeltaLOCCTR) + "\t" + " " + "\t" + prog_block + "\t" + opcode + "\t" + operand + "\t" + comment + wr_dataSuffix;
  write_in_the_file(inter_file, wr_data);

  int LOCCTR_array[t_blocks];
  block_num_name = new string[t_blocks];
  for (auto const &it : BLOCKS)
  {
    LOCCTR_array[it.second.number] = hexaatoint(it.second.LOCCTR);
    block_num_name[it.second.number] = it.first;
  }

  for (int i = 1; i < t_blocks; i++)
  {
    LOCCTR_array[i] += LOCCTR_array[i - 1];
  }

  for (auto const &it : BLOCKS)
  {
    if (it.second.startAddress == "?")
    {
      BLOCKS[it.first].startAddress = intToHexString(LOCCTR_array[it.second.number - 1]);
    }
  }

  programLength = LOCCTR_array[t_blocks - 1] - startAddress;

  sourceFile.close();
  inter_file.close();
  error_file.close();
}
