#include <bits/stdc++.h>
using namespace std;
#define endl '\n'


string extendto(string datain, int len, char ch, bool bit = false)
{
  if (bit)
  {
    if (len <= datain.length())
    {
      return datain.substr(0, len);
    }
    else
    {
      for (int i = len - datain.length(); i > 0; i--)
      {
        datain += ch;
      }
    }
  }
  else
  {
    if (len <= datain.length())
    {
      return datain.substr(datain.length() - len, len);
    }
    else
    {
      for (int i = len - datain.length(); i > 0; i--)
      {
        datain = ch + datain;
      }
    }
  }
  return datain;
}


int hexaatoint(string a)
{
  return stoul(a, nullptr, 16);
}


string intToHexString(int y, int a = 5)
{
  stringstream s;
  s << setfill('0') << setw(a) << hex << y;
  string st = s.str();
  st = st.substr(st.length() - a, a);
  transform(st.begin(), st.end(), st.begin(), ::toupper);
  return st;
}


string stringToHexString(const string &input)
{
  static const char *const lut = "0123456789ABCDEF";
  size_t len = input.length();

  string output;
  output.reserve(2 * len);
  for (size_t i = 0; i < len; ++i)
  {
    const unsigned char c = input[i];
    output.push_back(lut[c >> 4]);
    output.push_back(lut[c & 15]);
  }
  return output;
}


bool ifblankspace(char a)
{
  if (a == ' ' || a == '\t')
  {
    return true;
  }
  return false;
}


bool checkComment(string line)
{
  if (line[0] == '.')
  {
    return true;
  }
  return false;
}

bool ifallnum(string a)
{
  bool all_num = true;
  int i = 0;
  while (all_num && (i < a.length()))
  {
    all_num &= isdigit(a[i++]);
  }
  return all_num;
}

string final_op_code(string opcode)
{
  if (opcode[0] == '+' || opcode[0] == '@')
  {
    return opcode.substr(1, opcode.length() - 1);
  }
  return opcode;
}


void firstletter(string line, int &ind, bool &status, string &datain, bool readTillTheEnd = false)
{
  datain = "";
  status = true;
  if (readTillTheEnd)
  {
    datain = line.substr(ind, line.length() - ind);
    if (datain == "")
    {
      status = false;
    }
    return;
  }
  while (ind < line.length() && !ifblankspace(line[ind]))
  { 
    datain += line[ind];
    ind++;
  }

  if (datain == "")
  {
    status = false;
  }

  while (ind < line.length() && ifblankspace(line[ind]))
  {
    ind++;
  }
}


char getFlagformat(string input)
{
  if (input[0] == '#' || input[0] == '+' || input[0] == '@' || input[0] == '=')
  {
    return input[0];
  }
  return ' ';
}


void read_byte_oper(string line, int &ind, bool &status, string &datain)
{
  datain = "";
  status = true;
  if (line[ind] == 'C')
  {
    datain += line[ind++];
    char ident = line[ind++];
    datain += ident;
    while (ind < line.length() && line[ind] != ident)
    {
      datain += line[ind];
      ind++;
    }
    datain += ident;
  }
  else
  {
    while (ind < line.length() && !ifblankspace(line[ind]))
    {
      datain += line[ind];
      ind++;
    }
  }

  if (datain == "")
  {
    status = false;
  }

  while (ind < line.length() && ifblankspace(line[ind]))
  { 
    ind++;
  }
}

void write_in_the_file(ofstream &fileused, string datain, bool newline = true)
{
  if (newline)
  {
    fileused << datain << endl;
  }
  else
  {
    fileused << datain;
  }
}



class string_evaluate
{
public:
  int get_result();
  string_evaluate(string datain);

private:
  string stored_data;
  int ind;
  char peek();
  int factor();
  int num();
  char get();
  int term();
};

string_evaluate::string_evaluate(string datain)
{
  stored_data = datain;
  ind = 0;
}

int string_evaluate::get_result()
{
  int result = term();
  while (peek() == '+' || peek() == '-')
  {
    if (get() == '+')
    {
      result += term();
    }
    else
    {
      result -= term();
    }
  }
  return result;
}

int string_evaluate::term()
{
  int result = factor();
  while (peek() == '*' || peek() == '/')
  {
    if (get() == '*')
    {
      result *= factor();
    }
    else
    {
      result /= factor();
    }
  }
  return result;
}

int string_evaluate::factor()
{
  if (peek() >= '0' && peek() <= '9')
  {
    return num();
  }
  else if (peek() == '(')
  {
    get();
    int result = get_result();
    get();
    return result;
  }
  else if (peek() == '-')
  {
    get();
    return -factor();
  }
  return 0;
}

int string_evaluate::num()
{
  int ans = get() - '0';
  while (peek() >= '0' && peek() <= '9')
  {
    ans = 10 * ans + get() - '0';
  }
  return ans;
}

char string_evaluate::get()
{
  return stored_data[ind++];
}

char string_evaluate::peek()
{
  return stored_data[ind];
}
