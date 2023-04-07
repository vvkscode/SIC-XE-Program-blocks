#include <bits/stdc++.h>
#include "p1.cpp"
#define endl '\n'
using namespace std;

ifstream inter_file;
ofstream error_file, obj_file, list_file, print_table;
string oper1, oper2, stringadd, prog_block, opcode, operand, comment, objectCode, wr_data, currRecord, modify_rec = "M^", e_r, currentSecTitle = "DEFAULT";
int lineNum, blocknum, address, startAddress, secCntr = 0, prog_section_len = 0, p_counter, base_reg_value, currTextRecLength;
bool is_comm, base_not_found;

/************************************************************************************/

string read_until_tab(string datain, int &ind)
{
  string tempbuff = "";

  while (ind < datain.length() && datain[ind] != '\t')
  {
    tempbuff += datain[ind];
    ind++;
  }
  ind++;
  if (tempbuff == " ")
  {
    tempbuff = "-1";
  }
  return tempbuff;
}

bool rFile(ifstream &rd_file, bool &is_comm, int &lineNum, int &address, int &blocknum, string &prog_block, string &opcode, string &operand, string &comment)
{
  string fileLine = "", tempbuff = "";
  bool tempStatus;
  int ind = 0;

  if (!getline(rd_file, fileLine))
  {
    return false;
  }
  lineNum = stoi(read_until_tab(fileLine, ind));

  is_comm = (fileLine[ind] == '.') ? true : false;
  if (is_comm)
  {
    firstletter(fileLine, ind, tempStatus, comment, true);
    return true;
  }

  address = hexaatoint(read_until_tab(fileLine, ind));
  tempbuff = read_until_tab(fileLine, ind);

  if (tempbuff == " ")
  {
    blocknum = -1;
  }
  else
  {
    blocknum = stoi(tempbuff);
  }
  prog_block = read_until_tab(fileLine, ind);
  if (prog_block == "-1")
  {
    prog_block = " ";
  }
  opcode = read_until_tab(fileLine, ind);
  if (opcode == "BYTE")
  {
    read_byte_oper(fileLine, ind, tempStatus, operand);
  }
  else
  {
    operand = read_until_tab(fileLine, ind);
    if (operand == "-1")
    {
      operand = " ";
    }
  }
  firstletter(fileLine, ind, tempStatus, comment, true);
  return true;
}

string object_code_format()
{
  string objcode;
  int h_bytes;
  h_bytes = (getFlagformat(opcode) == '+') ? 5 : 3;

  if (getFlagformat(operand) == '#')
  { // ITS AN IMMEDIATE
    if (operand.substr(operand.length() - 2, 2) == ",X")
    { 
      wr_data = "Line: " + to_string(lineNum) + " INDEX BASED ADDRESSING IS NOT SUPPORTED WITH INDIRECT ADDRESSING";
      write_in_the_file(error_file, wr_data);
      objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 1, 2);
      objcode += (h_bytes == 5) ? "100000" : "0000";
      return objcode;
    }

    string temp_opernd = operand.substr(1, operand.length() - 1);

    if (ifallnum(temp_opernd) || ((SYMTAB[temp_opernd].exist == 1) && (SYMTAB[temp_opernd].reltve == 0)))
    { 
      int immediateValue;

      if (ifallnum(temp_opernd))
      {
        immediateValue = stoi(temp_opernd);
      }
      else
      {
        immediateValue = hexaatoint(SYMTAB[temp_opernd].address);
      }

      if (immediateValue >= (1 << 4 * h_bytes))
      {
        wr_data = "Line: " + to_string(lineNum) + " IMMEDIATE VALUE EXCEDES FORMAT LIMIT";
        write_in_the_file(error_file, wr_data);
        objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 1, 2);
        objcode += (h_bytes == 5) ? "100000" : "0000";
      }
      else
      {
        objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 1, 2);
        objcode += (h_bytes == 5) ? '1' : '0';
        objcode += intToHexString(immediateValue, h_bytes);
      }
      return objcode;
    }
    else if (SYMTAB[temp_opernd].exist == 0)
    {

      if (h_bytes == 3)
      {
        wr_data = "Line " + to_string(lineNum);
        if (h_bytes == 3)
        {
          wr_data += " : INVALID FORMAT FOR EXTERNAL REFERENCE " + temp_opernd;
        }
        else
        {
          wr_data += " : SYMBOL NOT FOUND. INSTEAD, FOUND " + temp_opernd;
        }
        write_in_the_file(error_file, wr_data);
        objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 1, 2);
        objcode += (h_bytes == 5) ? "100000" : "0000";
        return objcode;
      }
    }
    else
    {
      int op_addr = hexaatoint(SYMTAB[temp_opernd].address) + hexaatoint(BLOCKS[block_num_name[SYMTAB[temp_opernd].blocknum]].startAddress);

      /*PROCESS IMMEDIATE SINGLE VALUE*/
      if (h_bytes == 5)
      { /*IF FORMAT = 4*/
        objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 1, 2);
        objcode += '1';
        objcode += intToHexString(op_addr, h_bytes);

        /*ADDING THE MODIFICATION RECORD*/
        modify_rec += "M^" + intToHexString(address + 1, 6) + '^';
        modify_rec += (h_bytes == 5) ? "05" : "03";
        modify_rec += '\n';

        return objcode;
      }

      /*FORMAT = 3*/
      p_counter = address + hexaatoint(BLOCKS[block_num_name[blocknum]].startAddress);
      p_counter += (h_bytes == 5) ? 4 : 3;
      int relative_addr = op_addr - p_counter;

      /*PC ADDRESSING*/
      if (relative_addr >= (-2048) && relative_addr <= 2047)
      {
        objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 1, 2);
        objcode += '2';
        objcode += intToHexString(relative_addr, h_bytes);
        return objcode;
      }

      /*BASE ADDRESSING*/
      if (!base_not_found)
      {
        relative_addr = op_addr - base_reg_value;
        if (relative_addr >= 0 && relative_addr <= 4095)
        {
          objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 1, 2);
          objcode += '4';
          objcode += intToHexString(relative_addr, h_bytes);
          return objcode;
        }
      }

      /*DIRECT ADDRESSING(WITHOUT PC OR BASE reltve)*/
      if (op_addr <= 4095)
      {
        objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 1, 2);
        objcode += '0';
        objcode += intToHexString(op_addr, h_bytes);

        //ADD MODIFICATION RECORD HERE
        modify_rec += "M^" + intToHexString(address + 1 + hexaatoint(BLOCKS[block_num_name[blocknum]].startAddress), 6) + '^';
        modify_rec += (h_bytes == 5) ? "05" : "03";
        modify_rec += '\n';

        return objcode;
      }
    }
  }
  else if (getFlagformat(operand) == '@')
  {
    string temp_opernd = operand.substr(1, operand.length() - 1);
    if (temp_opernd.substr(temp_opernd.length() - 2, 2) == ",X" || SYMTAB[temp_opernd].exist == 0)
    { // ERROR HANDLING FOR INDIRECT ADDRESSING.

      if (h_bytes == 3)
      {
        wr_data = "Line " + to_string(lineNum);

        wr_data += " : SYMBOL NOT FOUND. INDIRECT ADDRESSING DOESN'T SUPPORT INDEX ADDRESSING ";

        write_in_the_file(error_file, wr_data);
        objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 2, 2);
        objcode += (h_bytes == 5) ? "100000" : "0000";
        return objcode;
      }
    }
    int op_addr = hexaatoint(SYMTAB[temp_opernd].address) + hexaatoint(BLOCKS[block_num_name[SYMTAB[temp_opernd].blocknum]].startAddress);
    p_counter = address + hexaatoint(BLOCKS[block_num_name[blocknum]].startAddress);
    p_counter += (h_bytes == 5) ? 4 : 3;

    if (h_bytes == 3)
    {
      int relative_addr = op_addr - p_counter;
      if (relative_addr >= (-2048) && relative_addr <= 2047)
      {
        objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 2, 2);
        objcode += '2';
        objcode += intToHexString(relative_addr, h_bytes);
        return objcode;
      }

      if (!base_not_found)
      {
        relative_addr = op_addr - base_reg_value;
        if (relative_addr >= 0 && relative_addr <= 4095)
        {
          objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 2, 2);
          objcode += '4';
          objcode += intToHexString(relative_addr, h_bytes);
          return objcode;
        }
      }

      if (op_addr <= 4095)
      {
        objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 2, 2);
        objcode += '0';
        objcode += intToHexString(op_addr, h_bytes);

        //ADD MODIFICATION RECORD
        modify_rec += "M^" + intToHexString(address + 1 + hexaatoint(BLOCKS[block_num_name[blocknum]].startAddress), 6) + '^';
        modify_rec += (h_bytes == 5) ? "05" : "03";
        modify_rec += '\n';

        return objcode;
      }
    }
    else
    { 
      objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 2, 2);
      objcode += '1';
      objcode += intToHexString(op_addr, h_bytes);

      //ADD MODIFICATION RECORD
      modify_rec += "M^" + intToHexString(address + 1 + hexaatoint(BLOCKS[block_num_name[blocknum]].startAddress), 6) + '^';
      modify_rec += (h_bytes == 5) ? "05" : "03";
      modify_rec += '\n';

      return objcode;
    }

    wr_data = "Line: " + to_string(lineNum);
    wr_data += " CAN'T FIT INTO PC OR BASE REGISTER BASED ADDRESSING.";
    write_in_the_file(error_file, wr_data);
    objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 2, 2);
    objcode += (h_bytes == 5) ? "100000" : "0000";

    return objcode;
  }
  else if (getFlagformat(operand) == '=')
  { //LITERALS FOUND
    string temp_opernd = operand.substr(1, operand.length() - 1);

    if (temp_opernd == "*")
    {
      temp_opernd = "X'" + intToHexString(address, 6) + "'";
      //ADD MODIFICATION RECORD
      modify_rec += "M^" + intToHexString(hexaatoint(LITTAB[temp_opernd].address) + hexaatoint(BLOCKS[block_num_name[LITTAB[temp_opernd].blocknum]].startAddress), 6) + '^';
      modify_rec += intToHexString(6, 2);
      modify_rec += '\n';
    }

    if (LITTAB[temp_opernd].exist == 0)
    {
      wr_data = "Line " + to_string(lineNum) + " : SYMBOL NOT FOUND. INSTEAD, FOUND " + temp_opernd;
      write_in_the_file(error_file, wr_data);

      objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 3, 2);
      objcode += (h_bytes == 5) ? "000" : "0";
      objcode += "000";
      return objcode;
    }

    int op_addr = hexaatoint(LITTAB[temp_opernd].address) + hexaatoint(BLOCKS[block_num_name[LITTAB[temp_opernd].blocknum]].startAddress);
    p_counter = address + hexaatoint(BLOCKS[block_num_name[blocknum]].startAddress);
    p_counter += (h_bytes == 5) ? 4 : 3;

    if (h_bytes == 3)
    {
      int relative_addr = op_addr - p_counter;
      if (relative_addr >= (-2048) && relative_addr <= 2047)
      {
        objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 3, 2);
        objcode += '2';
        objcode += intToHexString(relative_addr, h_bytes);
        return objcode;
      }

      if (!base_not_found)
      {
        relative_addr = op_addr - base_reg_value;
        if (relative_addr >= 0 && relative_addr <= 4095)
        {
          objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 3, 2);
          objcode += '4';
          objcode += intToHexString(relative_addr, h_bytes);
          return objcode;
        }
      }

      if (op_addr <= 4095)
      {
        objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 3, 2);
        objcode += '0';
        objcode += intToHexString(op_addr, h_bytes);

        //ADD MODIFICATION RECORD
        modify_rec += "M^" + intToHexString(address + 1 + hexaatoint(BLOCKS[block_num_name[blocknum]].startAddress), 6) + '^';
        modify_rec += (h_bytes == 5) ? "05" : "03";
        modify_rec += '\n';

        return objcode;
      }
    }
    else
    { 
      objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 3, 2);
      objcode += '1';
      objcode += intToHexString(op_addr, h_bytes);

      //ADD MODIFICATION RECORD
      modify_rec += "M^" + intToHexString(address + 1 + hexaatoint(BLOCKS[block_num_name[blocknum]].startAddress), 6) + '^';
      modify_rec += (h_bytes == 5) ? "05" : "03";
      modify_rec += '\n';

      return objcode;
    }

    wr_data = "Line: " + to_string(lineNum);
    wr_data += " CAN'T FIT INTO PC OR BASE REGISTER BASED ADDRESSING.";
    write_in_the_file(error_file, wr_data);
    objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 3, 2);
    objcode += (h_bytes == 5) ? "100" : "0";
    objcode += "000";

    return objcode;
  }
  else
  { 
    int xbpe = 0;
    string temp_opernd = operand;
    if (operand.substr(operand.length() - 2, 2) == ",X")
    {
      temp_opernd = operand.substr(0, operand.length() - 2);
      xbpe = 8;
    }

    if (SYMTAB[temp_opernd].exist == 0)
    {
      
      if (h_bytes == 3)
      { 

        wr_data = "Line " + to_string(lineNum);

        wr_data += " : SYMBOL NOT FOUND. INSTEAD, FOUND " + temp_opernd;

        write_in_the_file(error_file, wr_data);
        objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 3, 2);
        objcode += (h_bytes == 5) ? (intToHexString(xbpe + 1, 1) + "00") : intToHexString(xbpe, 1);
        objcode += "000";
        return objcode;
      }
    }
    else
    {

      int op_addr = hexaatoint(SYMTAB[temp_opernd].address) + hexaatoint(BLOCKS[block_num_name[SYMTAB[temp_opernd].blocknum]].startAddress);
      p_counter = address + hexaatoint(BLOCKS[block_num_name[blocknum]].startAddress);
      p_counter += (h_bytes == 5) ? 4 : 3;

      if (h_bytes == 3)
      {
        int relative_addr = op_addr - p_counter;
        if (relative_addr >= (-2048) && relative_addr <= 2047)
        {
          objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 3, 2);
          objcode += intToHexString(xbpe + 2, 1);
          objcode += intToHexString(relative_addr, h_bytes);
          return objcode;
        }

        if (!base_not_found)
        {
          relative_addr = op_addr - base_reg_value;
          if (relative_addr >= 0 && relative_addr <= 4095)
          {
            objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 3, 2);
            objcode += intToHexString(xbpe + 4, 1);
            objcode += intToHexString(relative_addr, h_bytes);
            return objcode;
          }
        }

        if (op_addr <= 4095)
        {
          objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 3, 2);
          objcode += intToHexString(xbpe, 1);
          objcode += intToHexString(op_addr, h_bytes);

          //ADD MODIFICATION RECORD HERE
          modify_rec += "M^" + intToHexString(address + 1 + hexaatoint(BLOCKS[block_num_name[blocknum]].startAddress), 6) + '^';
          modify_rec += (h_bytes == 5) ? "05" : "03";
          modify_rec += '\n';

          return objcode;
        }
      }
      else
      { 
        objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 3, 2);
        objcode += intToHexString(xbpe + 1, 1);
        objcode += intToHexString(op_addr, h_bytes);

        //ADD MODIFICATION RECORD HERE
        modify_rec += "M^" + intToHexString(address + 1 + hexaatoint(BLOCKS[block_num_name[blocknum]].startAddress), 6) + '^';
        modify_rec += (h_bytes == 5) ? "05" : "03";
        modify_rec += '\n';

        return objcode;
      }

      wr_data = "Line: " + to_string(lineNum);
      wr_data += " CAN'T FIT INTO PC OR BASE REGISTER BASED ADDRESSING.";
      write_in_the_file(error_file, wr_data);
      objcode = intToHexString(hexaatoint(OPTAB[final_op_code(opcode)].opcode) + 3, 2);
      objcode += (h_bytes == 5) ? (intToHexString(xbpe + 1, 1) + "00") : intToHexString(xbpe, 1);
      objcode += "000";

      return objcode;
    }
  }
  return "AB";
}

void wr_text_rec(bool lastRecord = false)
{
  if (lastRecord)
  {
    if (currRecord.length() > 0)
    { //LAST TEXT RECORD
      wr_data = intToHexString(currRecord.length() / 2, 2) + '^' + currRecord;
      write_in_the_file(obj_file, wr_data);
      currRecord = "";
    }
    return;
  }
  if (objectCode != "")
  {
    if (currRecord.length() == 0)
    {
      wr_data = "T^" + intToHexString(address + hexaatoint(BLOCKS[block_num_name[blocknum]].startAddress), 6) + '^';
      write_in_the_file(obj_file, wr_data, false);
    }
    // ONJCODE LENGTH > 60
    if ((currRecord + objectCode).length() > 60)
    {
      // CURR RECORD
      wr_data = intToHexString(currRecord.length() / 2, 2) + '^' + currRecord;
      write_in_the_file(obj_file, wr_data);

      // NEW TEXT RECORD
      currRecord = "";
      wr_data = "T^" + intToHexString(address + hexaatoint(BLOCKS[block_num_name[blocknum]].startAddress), 6) + '^';
      write_in_the_file(obj_file, wr_data, false);
    }

    currRecord += objectCode;
  }
  else
  {
    // ASSEMBLY DIRECTIVES DOESN'T NEED MEMORY ALLOCATION/ADDRESS ALLOCATION
    if (opcode == "START" || opcode == "END" || opcode == "BASE" || opcode == "base_not_found" || opcode == "LTORG" || opcode == "ORG" || opcode == "EQU")
    {
      //*****************
    }
    else
    {
      // WRITE CURRENT RECORD, IF CURRENT RECORD EXISTS
      if (currRecord.length() > 0)
      {
        wr_data = intToHexString(currRecord.length() / 2, 2) + '^' + currRecord;
        write_in_the_file(obj_file, wr_data);
      }
      currRecord = "";
    }
  }
}

void writeEndRecord(bool write = true)
{
  if (write)
  {
    if (e_r.length() > 0)
    {
      write_in_the_file(obj_file, e_r);
    }
    else
    {
      writeEndRecord(false);
    }
  }
  if ((operand == "" || operand == " ") && currentSecTitle == "DEFAULT")
  { 
    e_r = "E^" + intToHexString(startAddress, 6);
  }
  else if (currentSecTitle != "DEFAULT")
  {
    e_r = "E";
  }
  else
  { 
    int firstExecutableAddress;

    firstExecutableAddress = hexaatoint(SYMTAB[first_exe_section].address);

    e_r = "E^" + intToHexString(firstExecutableAddress, 6) + "\n";
  }
}

void p2()
{
  string tempbuff;
  inter_file.open("Intermediate of " + fileName); 
  if (!inter_file)
  {
    cout << "Unable to open file: Intermediate of " << fileName << endl;
    exit(1);
  }
  getline(inter_file, tempbuff); 

  obj_file.open("Object program of " + fileName);
  if (!obj_file)
  {
    cout << "Unable to open file: Object program of" << fileName << endl;
    exit(1);
  }

  list_file.open("listing file of " + fileName);
  if (!list_file)
  {
    cout << "Unable to open file: ListingFile_of_" << fileName << endl;
    exit(1);
  }
  write_in_the_file(list_file, "Line\tAddress\tprog_block\tOPCODE\tOPERAND\tObjectCode\tComment");

  error_file.open("Errors in " + fileName, fstream::app);
  if (!error_file)
  {
    cout << "Unable to open file: Error in " << fileName << endl;
    exit(1);
  }
  write_in_the_file(error_file, "\n\nERRORS OF PASS2:");
  
  objectCode = "";
  currTextRecLength = 0;
  currRecord = "";
  modify_rec = "";
  blocknum = 0;
  base_not_found = true;

  rFile(inter_file, is_comm, lineNum, address, blocknum, prog_block, opcode, operand, comment);
  while (is_comm)
  { 
    wr_data = to_string(lineNum) + "\t" + comment;
    write_in_the_file(list_file, wr_data);
    rFile(inter_file, is_comm, lineNum, address, blocknum, prog_block, opcode, operand, comment);
  }

  if (opcode == "START")
  {
    startAddress = address;
    wr_data = to_string(lineNum) + "\t" + intToHexString(address) + "\t" + to_string(blocknum) + "\t" + prog_block + "\t" + opcode + "\t" + operand + "\t" + objectCode + "\t" + comment;
    write_in_the_file(list_file, wr_data);
  }
  else
  {
    prog_block = "";
    startAddress = 0;
    address = 0;
  }
  prog_section_len = programLength;

  wr_data = "H^" + extendto(prog_block, 6, ' ', true) + '^' + intToHexString(address, 6) + '^' + intToHexString(prog_section_len, 6);
  write_in_the_file(obj_file, wr_data);

  rFile(inter_file, is_comm, lineNum, address, blocknum, prog_block, opcode, operand, comment);
  currTextRecLength = 0;

  while (opcode != "END")
  {
    while (opcode != "END")
    {
      if (!is_comm)
      {
        if (OPTAB[final_op_code(opcode)].exist == 1)
        {
          if (OPTAB[final_op_code(opcode)].frmt == 1)
          {
            objectCode = OPTAB[final_op_code(opcode)].opcode;
          }
          else if (OPTAB[final_op_code(opcode)].frmt == 2)
          {
            oper1 = operand.substr(0, operand.find(','));
            oper2 = operand.substr(operand.find(',') + 1, operand.length() - operand.find(',') - 1);

            if (oper2 == operand)
            { // If not two operand i.e. a
              if (final_op_code(opcode) == "SVC")
              {
                objectCode = OPTAB[final_op_code(opcode)].opcode + intToHexString(stoi(oper1), 1) + '0';
              }
              else if (REGTAB[oper1].exist == 1)
              {
                objectCode = OPTAB[final_op_code(opcode)].opcode + REGTAB[oper1].regnum + '0';
              }
              else
              {
                objectCode = final_op_code(opcode) + '0' + '0';
                wr_data = "Line: " + to_string(lineNum) + " INVALID REGISTER NAME";
                write_in_the_file(error_file, wr_data);
              }
            }
            else
            { // Two operands i.e. a,b
              if (REGTAB[oper1].exist == 0)
              {
                objectCode = OPTAB[final_op_code(opcode)].opcode + "00";
                wr_data = "Line: " + to_string(lineNum) + " INVALID REGISTER NAME";
                write_in_the_file(error_file, wr_data);
              }
              else if (final_op_code(opcode) == "SHIFTR" || final_op_code(opcode) == "SHIFTL")
              {
                objectCode = OPTAB[final_op_code(opcode)].opcode + REGTAB[oper1].regnum + intToHexString(stoi(oper2), 1);
              }
              else if (REGTAB[oper2].exist == 0)
              {
                objectCode = OPTAB[final_op_code(opcode)].opcode + "00";
                wr_data = "Line: " + to_string(lineNum) + " INVALID REGISTER NAME";
                write_in_the_file(error_file, wr_data);
              }
              else
              {
                objectCode = OPTAB[final_op_code(opcode)].opcode + REGTAB[oper1].regnum + REGTAB[oper2].regnum;
              }
            }
          }
          else if (OPTAB[final_op_code(opcode)].frmt == 3)
          {
            if (final_op_code(opcode) == "RSUB")
            {
              objectCode = OPTAB[final_op_code(opcode)].opcode;
              objectCode += (getFlagformat(opcode) == '+') ? "000000" : "0000";
            }
            else
            {
              objectCode = object_code_format();
            }
          }
        } // If opcode in optab
        else if (opcode == "BYTE")
        {
          if (operand[0] == 'X')
          {
            objectCode = operand.substr(2, operand.length() - 3);
          }
          else if (operand[0] == 'C')
          {
            objectCode = stringToHexString(operand.substr(2, operand.length() - 3));
          }
        }
        else if (prog_block == "*")
        {
          if (opcode[1] == 'C')
          {
            objectCode = stringToHexString(opcode.substr(3, opcode.length() - 4));
          }
          else if (opcode[1] == 'X')
          {
            objectCode = opcode.substr(3, opcode.length() - 4);
          }
        }
        else if (opcode == "WORD")
        {
          objectCode = intToHexString(stoi(operand), 6);
        }
        else if (opcode == "BASE")
        {
          if (SYMTAB[operand].exist == 1)
          {
            base_reg_value = hexaatoint(SYMTAB[operand].address) + hexaatoint(BLOCKS[block_num_name[SYMTAB[operand].blocknum]].startAddress);
            base_not_found = false;
          }
          else
          {
            wr_data = "Line " + to_string(lineNum) + " : SYMBOL NOT FOUND. INSTEAD, FOUND " + operand;
            write_in_the_file(error_file, wr_data);
          }
          objectCode = "";
        }
        else if (opcode == "base_not_found")
        {
          if (base_not_found)
          { // check if assembler was using base addressing
            wr_data = "Line " + to_string(lineNum) + ": ASSEMBLER NOT USING BASE ADDRESSING";
            write_in_the_file(error_file, wr_data);
          }
          else
          {
            base_not_found = true;
          }
          objectCode = "";
        }
        else
        {
          objectCode = "";
        }
        
        wr_text_rec();

        if (blocknum == -1 && address != -1)
        {
          wr_data = to_string(lineNum) + "\t" + intToHexString(address) + "\t" + " " + "\t" + prog_block + "\t" + opcode + "\t" + operand + "\t" + objectCode + "\t" + comment;
        }
        else if (address == -1)
        {

          wr_data = to_string(lineNum) + "\t" + " " + "\t" + " " + "\t" + prog_block + "\t" + opcode + "\t" + operand + "\t" + objectCode + "\t" + comment;
        }

        else
        {
          wr_data = to_string(lineNum) + "\t" + intToHexString(address) + "\t" + to_string(blocknum) + "\t" + prog_block + "\t" + opcode + "\t" + operand + "\t" + objectCode + "\t" + comment;
        }
      } 
      else
      {
        wr_data = to_string(lineNum) + "\t" + comment;
      }
      write_in_the_file(list_file, wr_data);                                                           
      rFile(inter_file, is_comm, lineNum, address, blocknum, prog_block, opcode, operand, comment); 
      objectCode = "";
    } 
  }   

    //*****************************************************************************************
    wr_text_rec();
    writeEndRecord(true);
    wr_data =  wr_data = to_string(lineNum) + "\t" + intToHexString(address) + "\t" + " " + prog_block + "\t" + "END" + "    " + "FIRST";
    write_in_the_file(list_file,wr_data);
    
      while (rFile(inter_file, is_comm, lineNum, address, blocknum, prog_block, opcode, operand, comment))
      {
        if (prog_block == "*")
        {
          if (opcode[1] == 'C')
          {
            objectCode = stringToHexString(opcode.substr(3, opcode.length() - 4));
          }
          else if (opcode[1] == 'X')
          {
            objectCode = opcode.substr(3, opcode.length() - 4);
          }
          wr_text_rec();
        }
        wr_data = to_string(lineNum) + "\t" + intToHexString(address) + "\t" + to_string(blocknum) + prog_block + "\t" + opcode + "\t" + operand + "\t" + objectCode + "\t" + comment;
        write_in_the_file(list_file, wr_data);
      }
    

    wr_text_rec(true);
    if (!is_comm)
    {

      write_in_the_file(obj_file, modify_rec, false); // Write modification record
      writeEndRecord(true);                                    // Write end record
      currentSecTitle = prog_block;
      modify_rec = "";
    }
  }
  //***********************************************************************

int main()
{
  cout << "INPUT FILE NAME:";
  cin >> fileName;

  cout << "\nLoading OPCODE TAB" << endl;
  load_all_tables();

  cout << "\nRUNNING PASS1" << endl;
  cout << "Writing intermediate values to 'Intermediate of " << fileName << "'" << endl;
  cout << "Writing errors to 'Error in" << fileName << "'" << endl;
  p1();

  cout << "Writing SYMBOL TABLE into Tables of " + fileName << endl;
  print_table.open("Tables of " + fileName);
  write_in_the_file(print_table, "SYMBOL TABLE:\n");
  for (auto const &it : SYMTAB)
  {
    stringadd += it.first + ":-\t" + "name:" + it.second.name + "\t|" + "address:" + it.second.address + "\t|" + "reltve:" + intToHexString(it.second.reltve) + " \n";
  }
  write_in_the_file(print_table, stringadd);

  stringadd = "";
  cout << "Writing LITERAL TABLE into Tables of " + fileName << endl;

  write_in_the_file(print_table, "LITERAL TABLE:\n");
  for (auto const &it : LITTAB)
  {
    stringadd += it.first + ":-\t" + "value:" + it.second.value + "\t|" + "address:" + it.second.address + " \n";
  }
  write_in_the_file(print_table, stringadd);

  cout << "\nRUNNING PASS2" << endl;
  cout << "Writing objectcode in the 'Object program of " << fileName << "'" << endl;
  cout << "Listing to 'List of" << fileName << "'\n.\n.\n." << endl;
  p2();
  cout << "THE OUTPUT FILES ARE READY (INCLUDES OBJECT PROGRAM, TABLES)" << endl;
}
