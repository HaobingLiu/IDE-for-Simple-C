#include "stdafx.h"

#include <iostream>
#include <stdlib.h>
#include <string>
#include <stack>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <vector>
#include "showDlg.h"

using namespace std;

showDlg Dlg;
string floatTOstring(float Num);
string intTOstring(int Num);
///extern.h
int tokennum = 0;
FILE *fp;
const int MAX = 1000;
const float EPSINON = 0.00001;
int n = 0;
stack<string>SEM;//语义栈
string token = "";
/*关键字表，修改时请同时修改下面的关键字长度length_of_keywords。*/
string keywords[MAX] = { "int", "main", "void", "if", "else", "float", "while", "bool", "char" };
int length_of_keywords = 9;
/*界符表。*/
string definitions[MAX] = { ">=", "<=", "==", "!=", "=", ">", "<", "+", "-",
"*", "/", "{", "}", ",", ";", "(", ")",
"[", "]", ".", "!", "&&", "||", "/*", "*/", "//" };

vector<float> cons;  //常数表
//int length_of_cons = 0, length_of_signal = 0;
int arrstr = 0;
int IsDigit(char ch);
enum enumcat { f, c, t, d, v, vn, vf };

int searchcons(string str)
{
	unsigned int i;
	float x;
	if (IsDigit(str[0]))
		for (i = 0; i<cons.size(); i++)
		{
			x = cons[i] - atof(str.c_str());
			if (x <= EPSINON && x >= EPSINON*(-1))
				return i;
		}
	return -1;
}

typedef struct SMBLTABLitem {
	string name;
	int type;//int为-1，非-1指向整形一维数组表
	int addr;
	enumcat cat;
	bool operator == (string i);//重载==
}SMBLTABLi;//标识符表

		   //重载vector中==的函数
bool SMBLTABLi::operator == (string i)
{
	return this->name == i;
}

int SMBLTABLlength = 0;  //标识符表长度
int temp = 0;    //中间变量数
int addr = 0;
vector<SMBLTABLi> SMBLTABL;
SMBLTABLi SMBLTABLitem;

int searchadmdefine(string str)    //返回1表示是标识符，返回2表示是中间变量
{
	unsigned int i;
	for (i = 0; i < SMBLTABL.size(); i++)
	{
		if (str == SMBLTABL[i].name)
		{
			if (i < SMBLTABL.size() - temp) return 1;
			else return 2;
		}
	}
	return -1;
}

class TokenType
{
public:
	int TYPCOD;
	int pp;

	int getpp() { return pp; }
	int getTYPCOD() { return TYPCOD; }

	void mkTYPCOD(int typecode) { TYPCOD = typecode; }

	void mkpp(string itoken)
	{
		unsigned int j;
		if (TYPCOD == 0)
		{
			for (j = 0; j<SMBLTABL.size(); j++)
			{
				if (SMBLTABL[j].name == itoken)
				{
					pp = j;
					return;
				}
			}
			SMBLTABLitem.name = itoken;
			pp = SMBLTABL.size();
			SMBLTABL.push_back(SMBLTABLitem);
			return;
		}
		else if (TYPCOD == 3)
		{
			int x;

			for (j = 0; j< cons.size(); j++)
			{
				x = cons[j] - atof(itoken.c_str());
				if (x <= EPSINON && x >= EPSINON*(-1))
				{
					pp = j;
					return;
				}
			}
			pp = cons.size();
			cons.push_back(atof(itoken.c_str()));
			return;
		}
		else if (itoken == "#")
		{
			pp = -2;
		}
		else
			pp = -1;
	}
}Token[MAX];


class TokenList
{
public:
	int TYPCOD;
	int pp;//关键字、界符为-1，非-1指向符号表或常量表

	int getpp() { return pp; }
	int getTYPCOD() { return TYPCOD; }

	void mkTYPCOD(int code) { TYPCOD = code; }

	void mkpp(string itoken)
	{
		unsigned int j;
		float x;
		if (TYPCOD == 0)
		{
			for (j = 0; j<SMBLTABL.size(); j++)
			{
				if (SMBLTABL[j].name == itoken)
				{
					pp = j;
					return;
				}
			}
			return;
		}
		else if (TYPCOD == 3)
		{
			for (j = 0; j< cons.size(); j++)
			{
				x = atof(itoken.c_str()) - cons[j];
				if (x <= EPSINON && x >= EPSINON*(-1))
				{
					pp = j;
					return;
				}
			}
		}
		else if (itoken == "#")
		{
			pp = -2;
		}
		else
			pp = -1;
	}
}TokenList[MAX];//TOKEN数组
int pToken = 0;

struct Quat {
	string op;      //操作tail[1]="";符
	string op1;     //操作数1
	string op2;     //操作数2
	string result;  //结果
}NEWQT[30], QT[50];
int pQT = 0;          //QT个数
int Num = 0;        //DAG节点个数
int NpQT = 0;       //NEWQT个数

int block1[10];//block1数组用来存放入口四元式标号 
int l1=0;//block1数组存的入口四元式标号的个数，其值等于block2四元式存的出口四元式标号的个数，其值由inBlock();函数返回值赋值
int block2[10];//block2数组用来存放出口四元式标号 

struct Node {
	int NodeNum;					//该节点的编号
	bool Empty;                   //该节点是否为空
	int RChild, LChild;			//左右子节点的编号
	int TailNum;              //副标记个数
	int Num;        //DAG节点个数
	string fun;
	string prim;
	string tail[5];
	string special1;
	string special2;
	string special3;
	Node() {
		fun = "";
		prim = "";
		RChild = -1;
		LChild = -1;
		Empty = true;
		tail[0] = "";
		tail[1] = "";
		tail[2] = "";
		tail[3] = "";
		tail[4] = "";
		TailNum = 0;
		special1 = "_";
		special1 = "_";
		special1 = "_";
	}
}DAG1[30], DAG2[30], DAG3[30], DAG4[30], DAG5[30], DAG6[30], DAG7[30], DAG8[30], DAG9[30];


int IsDigit(char ch)  //判断ch是否为数字
{
	if (ch >= 48 && ch <= 57)
		return 1;
	else
		return 0;
}

int IsLetter(char ch)  //判断ch是否为字母
{
	if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
		return 1;
	else
		return 0;
}



///h1.h
string str_out1 = "";///输出TOKENLIST
void printSMBLTABL()
{
	int j;

	for (j = 0; j<n; j++)
		if (Token[j].getpp() == -1) continue;
		else cout << "·ûºÅ±í\t" << SMBLTABL[Token[j].getpp()].name << endl;
}
int state_to_code(int state_before, string token)    //返回TOKEN的编码
{
	switch (state_before)
	{
	case 2:
		for (int i = 0; i<length_of_keywords; i++)
			if (token == keywords[i]) return i + 4;
		return 0;
		break;
	case 3:	case 7:	return 3; break;
	case 16: return 1;
	case 17: return 2;
	case 4: return 56;
	case 5: return 57;
	case 6: return 55;
	case 13: case 14: case 15: return state_before + 38;
	case 32: return 54;
	case 31: return 72;

	case 43: return 76;
	case 44: return 75;
	default:
		for (int i = 7; i<25; i++)
			if (token == definitions[i]) return i + 51;
	}
	return -1;
}

int ch_to_num(char ch)
{
	int num = 0;
	if (IsLetter(ch))
		num = 1;
	else if (isdigit(ch))
		num = 2;
	else if (ch == '\n') num = 3;
	else if (ch == '\r') num = 4;
	else if (ch == '\"') num = 6;
	else if (ch == '\'') num = 7;
	else if (ch == '=') num = 14;
	else if (ch == '>') num = 15;
	else if (ch == '<') num = 16;
	else if (ch == '+') num = 17;
	else if (ch == '-') num = 18;
	else if (ch == '*') num = 19;
	else if (ch == '/') num = 20;
	else if (ch == '{') num = 21;
	else if (ch == '}') num = 22;
	else if (ch == ',') num = 23;
	else if (ch == ';') num = 24;
	else if (ch == '(') num = 25;
	else if (ch == ')') num = 26;
	else if (ch == '[') num = 27;
	else if (ch == ']') num = 28;
	else if (ch == '.') num = 29;
	else if (ch == '!') num = 30;
	else if (ch == '&') num = 31;
	else if (ch == '|') num = 32;
	else if (ch == ' ') num = 0;
	return num;
}

int state_check(int state, int ch_code)
{
	switch (state)
	{
	case 1:
		switch (ch_code)
		{
		case 0: return 1;
		case 1: return 2;
		case 2: return 3;
		case 3: return 1;
		case 6: return 11;
		case 7: return 9;
		case 14: return 6;					//==
		case 15: return 4;					//>=
		case 16: return 5;                  //<=
		case 17: case 18: case 19: case 20: case 21: case 22:
		case 23: case 24: case 25: case 26: case 27: case 28:
		case 29: case 30:
			return ch_code + 1;
		case 31: case 32:
			return ch_code + 2;
		}
	case 2:
		if (ch_code == 1 || ch_code == 2) return 2;
		else if (ch_code == 0) return 0;
		else return 0;
	case 3:
		if (ch_code == 2) return 3;
		else if (ch_code == 29) return 8;
		else if (ch_code == 0) return 0;
		else return 0;
	case 4:
		if (ch_code == 14) return 13;
		else if (ch_code == 0) return 0;
		else return 0;
	case 5:
		if (ch_code == 14) return 14;
		else if (ch_code == 0) return 0;
		else return 0;
	case 6:
		if (ch_code == 14) return 15;
		else if (ch_code == 0) return 0;
		else return 0;
	case 7:
		if (ch_code == 2) return 7;
		else if (ch_code == 0) return 0;
		else return 0;
	case 8:
		if (ch_code == 2) return 7;
		else if (ch_code == 0) return 0;
	case 9:
		if (ch_code == 7) return 16;
		else if (ch_code == 0) return 0;
		else return 10;
	case 10:
		if (ch_code == 7) return 16;
		else if (ch_code == 0) return 0;
	case 11:
		if (ch_code == 6) return 17;
		else if (ch_code == 0) return 0;
		else return 12;
	case 12:
		if (ch_code == 6) return 17;
		else if (ch_code == 0) return 0;
		else return 12;
	case 21:
		if (ch_code == 19) return 40;    // ×¢ÊÍ /* */
		else if (ch_code == 20) return 42;    //×¢ÊÍ //
		else return 0;
	case 31:    // !
		if (ch_code == 14) return 32;    // !=
		else return 0;
	case 33:    // &
		if (ch_code == 31) return 34;
		else return 0;
	case 35:    // |
		if (ch_code == 32) return 36;
		else return 0;
	case 40:    // ×¢ÊÍ /* */
		if (ch_code == 19) return 41;
		else return 40;
	case 41:
		if (ch_code == 20) return 44;
		else return 0;
	case 42:    //×¢ÊÍ //
		if (ch_code == 3 || ch_code == 4) return 43;
		else return 42;
	default: return 0;
	}
}

int state_change(int state, char ch)
{
	int end_state = 0;
	int ch_code = ch_to_num(ch);
	if (state >= 1 && state <= 100)
		end_state = state_check(state, ch_code);
	return end_state;
}

int scanner()
{
	int state = 1;
	char ch;
	
	int state_before, code;
	int i;


	while ((ch = getc(fp)) != EOF)
	{
		state_before = state;
		state = state_change(state, ch);
		if (state)
		{
			if (ch != ' ' && ch != '\n' && ch != '\r' && ch != '\t')
				token += ch;
		}
		else
		{
			code = state_to_code(state_before, token);
			if (code == -1) { AfxMessageBox("No this state!"); exit(1); }
			if (code != 75 && code != 76)
			{
				Token[n].mkTYPCOD(code);
				Token[n].mkpp(token);
				TokenList[tokennum].mkTYPCOD(code);
				TokenList[tokennum].mkpp(token);
				tokennum++;
				n++;
			}

			token = "";
			if (ch != ' ' && ch != '\n') { fseek(fp, -1L, SEEK_CUR); }
			state = 1;
		}
	}

	state_before = state;
	state = state_change(state, ch);
	if (state)
	{
		if (ch != ' ' && ch != '\n' && ch != '\r')
			token += ch;
	}
	else
	{
		code = state_to_code(state_before, token);
		if (code == -1) { AfxMessageBox("No this state!"); exit(1); }
		if (code != 75 && code != 76)
		{
			Token[n].mkTYPCOD(code);
			Token[n].mkpp(token);
			TokenList[tokennum].mkTYPCOD(code);
			TokenList[tokennum].mkpp(token);
			tokennum++;
			n++;
		}


		token = "";
		if (ch != ' ' && ch != '\n') { fseek(fp, -1L, SEEK_CUR); }
		state = 1;
	}
	TokenList[n].mkTYPCOD(71);
	TokenList[n].mkpp("#");

	for (i = 0; i<tokennum; i++)
		if (TokenList[i].getTYPCOD() == 0)
			str_out1 += intTOstring(TokenList[i].getTYPCOD()) + "\t" + intTOstring(TokenList[i].getpp()) + "\t" + SMBLTABL[TokenList[i].getpp()].name + "\r\n";
		else if (TokenList[i].getTYPCOD() == 3)
			str_out1 += "3\t" + intTOstring(TokenList[i].getpp()) + "\t" + floatTOstring(cons[TokenList[i].getpp()]) + "\r\n";
		else
			str_out1 += intTOstring(TokenList[i].getTYPCOD()) + "\t" + intTOstring(TokenList[i].getpp()) + "\r\n";
		str_out1 += "endscanner.\r\n";


		CString str_1 = str_out1.c_str();
		Dlg.str1 = str_1;
		//Dlg.DoModal();
		return 0;
}



///h2.h
struct AINFLitem {
	int low;
	int up;
	int ctp;
	int clen;
}AINFL[100];//数组表
int pAINFL = 0;//需要我一个个来填，由于没用vector动态数组，只能靠全局变量
void B();

void A();
void B();
void B1();
void definsen();
void definsen1();
void eqsen();
void E();
void T();
void F();
void ifsen();
void whilesen();

void geq(string op);
void send(string op, string op1, string op2, string result);
void ENT(char type, int addr);
void ENT1(int low, int up, int ctp, int clen);

int inBlock(int block[], Quat Qt[], int pQt);
int outBlock(int block[], Quat Qt[], int pQt);


void err(int k)//语法分析出错处理函数
{
	switch (k) {
	case 0:
		AfxMessageBox("请确保tiny c程序有加\"void main(){   }\"!");
		break;
	case 1:
		AfxMessageBox("请确保tiny c程序有写变量定义语句!");
		break;
	case 2:
		AfxMessageBox("请确保tiny c程序最后没有奇怪的东西!");
		break;
	case 6:
		AfxMessageBox("变量定义语句错误!");
		break;
	case 7:
		AfxMessageBox("赋值语句错误!");
		break;
	case 8:
		AfxMessageBox("算术表达式错误!");
		break;
	case 9:
		AfxMessageBox("if语句错误!");
		break;
	case 10:
		AfxMessageBox("while语句错误!");
		break;
	}
	exit(1);
}

string floatTOstring(float Num)
{
	ostringstream oss;
	oss << Num;
	string str(oss.str());
	return str;
}

string intTOstring(int Num)
{
	ostringstream oss;
	oss << Num;
	string str(oss.str());
	return str;
}

void ENT(int type, int addr) {
	SMBLTABL[TokenList[pToken].pp].type = type;
	SMBLTABL[TokenList[pToken].pp].addr = addr;
}

void ENT1(int low, int up, int ctp, int clen) {
	AINFL[pAINFL].low = low;
	AINFL[pAINFL].up = up;
	AINFL[pAINFL].ctp = ctp;
	AINFL[pAINFL].clen = clen;
	pAINFL++;
}

void geq(string op) {
	temp++;//从T1开始
	string strTemp = intTOstring(temp);
	string result = "T" + strTemp;

	SMBLTABLitem.name = result;
	SMBLTABLitem.type = -1;
	SMBLTABLitem.addr = addr;
	SMBLTABL.push_back(SMBLTABLitem);
	int wid = 2;
	addr += wid;

	string a = SEM.top();
	SEM.pop();
	send(op, SEM.top(), a, result);
	SEM.pop();
	SEM.push(result);
}

void send(string op, string op1, string op2, string result) {
	QT[pQT].op = op;
	QT[pQT].op1 = op1;
	QT[pQT].op2 = op2;
	QT[pQT].result = result;
	pQT++;
}

int outBlock(int block[], Quat Qt[], int pQt) {
	int l = 0;
	for (int i = 0; i <= pQt - 1; i++) {
		if (Qt[i].op == "if" || Qt[i].op == "else" || Qt[i].op == "do" || Qt[i].op == "whileend") { block[l] = i; l++; }//转向语句为出口
		else if (Qt[i].op == "ifend"|| Qt[i].op == "while") { block[l] = i - 1; l++; }//转向语句转移到的语句上一个为出口
	}
	if (block[l - 1] != pQt - 1) { block[l] = pQt - 1; l++; }//最后一个语句一定为出口
	return l;
}

int inBlock(int block[], Quat Qt[], int pQt) {
	int l = 0;
	if (Qt[0].op == "ifend"|| Qt[0].op == "while") { block[0] = 0; l++; }//转向语句转移到的语句恰为第一个语句
	else { block[0] = 0; l++; }//第一个语句为入口
	for (int i = 1; i <= pQt - 1; i++) {
		if (Qt[i].op == "if" || Qt[i].op == "else" || Qt[i].op == "do" || Qt[i].op == "whileend") { block[l] = i + 1; l++; }//转向语句的下一个语句为入口
		else if (Qt[i].op == "ifend"|| Qt[i].op == "while") { block[l] = i; l++; }//转向语句转移到的语句为入口
	}
	if (block[l - 1] == pQt) { block[l - 1] = '\0', l--; }//如果入口地址超出了四元式地址，那么就不算了。此种情况do语句可能发生
	return l;
}

///<A> -> void main ( ) { <B> }
void A() {
	if (TokenList[pToken].TYPCOD == 6) {
		pToken++;
		if (TokenList[pToken].TYPCOD == 5) {
			pToken++;
			if (TokenList[pToken].TYPCOD == 66) {
				pToken++;
				if (TokenList[pToken].TYPCOD == 67) {
					pToken++;
					if (TokenList[pToken].TYPCOD == 62) {
						pToken++;
						B();
						if (TokenList[pToken].TYPCOD == 63) {
							pToken++;
						}
						else err(0);//没 }
					}
					else err(0);//没 {
				}
				else err(0);//没 )
			}
			else err(0);//没 (
		}
		else err(0);//没 main
	}
	else err(0);//没 void
}

///<B> -> <definsen> <B1> | 空
void B() {
	if (TokenList[pToken].TYPCOD == 4) {
		definsen();
		B1();
	}
	else if (TokenList[pToken].TYPCOD != 63)
		err(1);
}

///<B1> -> <eqsen> <B1> | <ifsen> <B1> | <whilesen> <B1> | 空
void B1() {
	if (TokenList[pToken].TYPCOD == 0) {
		eqsen();
		B1();
	}

	else if (TokenList[pToken].TYPCOD == 7) {
		ifsen();
		B1();
	}

	else if (TokenList[pToken].TYPCOD == 10) {
		whilesen();
		B1();
	}
}

///<definsen> -> int 标识符 <definsen1> | int 标识符 [ 常数 ] <definsen1>
void definsen() {
	int typ = -1;
	int wid = 2;///
	pToken++;
	if (TokenList[pToken].TYPCOD == 0 && TokenList[pToken + 1].TYPCOD != 68) {
		ENT(typ, addr);
		addr += wid;
		pToken++;
		definsen1();
	}

	else if (TokenList[pToken].TYPCOD == 0 && TokenList[pToken + 1].TYPCOD == 68 && TokenList[pToken + 2].TYPCOD == 3 && TokenList[pToken + 3].TYPCOD == 69) {
		typ = pAINFL;
		wid = 2 * (int)cons[TokenList[pToken + 2].pp];///
		ENT(typ, addr);
		addr += wid;

		for (int i = 0; i<(int)cons[TokenList[pToken + 2].pp]; i++) {
			SMBLTABLitem.name = SMBLTABL[TokenList[pToken].pp].name + "+" + intTOstring(2*i);///
			SMBLTABLitem.type = -2;
			SMBLTABLitem.addr = -2;
			SMBLTABL.push_back(SMBLTABLitem);
		}

		ENT1(0, (int)cons[TokenList[pToken + 2].pp] - 1, -1, 2);///填完符号表再填数组表

		pToken += 4;//这次pToken要加4而不是1
		definsen1();
	}
	else err(6);//变量定义语句错误
}

///<definsen1> -> , 标识符 <definsen1> | , 标识符 [ 常数 ] <definsen1> | ;
void definsen1() {
	int typ = -1;
	int wid = 2;///
	if (TokenList[pToken].TYPCOD == 64) {
		pToken++;
		if (TokenList[pToken].TYPCOD == 0 && TokenList[pToken + 1].TYPCOD != 68) {
			ENT(typ, addr);
			addr += wid;
			pToken++;
			definsen1();
		}

		else if (TokenList[pToken].TYPCOD == 0 && TokenList[pToken + 1].TYPCOD == 68 && TokenList[pToken + 2].TYPCOD == 3 && TokenList[pToken + 3].TYPCOD == 69) {
			typ = pAINFL;
			wid = 2 * (int)cons[TokenList[pToken + 2].pp];///
			ENT(typ, addr);
			addr += wid;

			for (int i = 0; i<(int)cons[TokenList[pToken + 2].pp]; i++) {
				SMBLTABLitem.name = SMBLTABL[TokenList[pToken].pp].name + "+" + intTOstring(2*i);///
				SMBLTABLitem.type = -2;
				SMBLTABLitem.addr = -2;
				SMBLTABL.push_back(SMBLTABLitem);
			}

			ENT1(0, (int)cons[TokenList[pToken + 2].pp] - 1, -1, 2);///

			pToken += 4;
			definsen1();
		}
		else err(6);//变量定义语句错误
	}

	else if (TokenList[pToken].TYPCOD == 65) {
		pToken++;
	}
	else err(6);//变量定义语句错误
}

///<eqsen> -> 标识符 = <E> ; | 标识符 [ 常数 ] = <E> ;
void eqsen() {
	if (TokenList[pToken + 1].TYPCOD == 68 && TokenList[pToken + 2].TYPCOD == 3 && TokenList[pToken + 3].TYPCOD == 69) {
		SEM.push(SMBLTABL[TokenList[pToken].pp].name + "+" + floatTOstring(2*cons[TokenList[pToken + 2].pp]));///
		pToken += 4;
	}//属于 标识符 [ 常数 ] 这种情况 //插的语义动作

	else {
		SEM.push(SMBLTABL[TokenList[pToken].pp].name);
		pToken++;
	}
	if (TokenList[pToken].TYPCOD == 55) {
		pToken++;
		E();

		string a = SEM.top();
		SEM.pop();
		send("=", a, "_", SEM.top());
		SEM.pop();//插的语义动作
	}
	else err(7);//赋值语句错误
	if (TokenList[pToken].TYPCOD == 65) {
		pToken++;
	}
	else err(7);//赋值语句错误

}

///<E> -> <T> {加减算符 <T>}//花括号代表0个或多个
void E() {
	T();
jmp1:if (TokenList[pToken].TYPCOD == 58) {
		pToken++;
		T();

		geq("+");//插的语义动作
		goto jmp1;
	}

	else if(TokenList[pToken].TYPCOD == 59){
		pToken++;
		T();
		geq("-");
		goto jmp1;
	}
	//goto jmp1;//不写else就是死循环
}

///<T> -> <F> {乘除算符 <F>}//花括号代表0个或多个
void T() {
	F();

jmp2:if (TokenList[pToken].TYPCOD == 60 || TokenList[pToken].TYPCOD == 61) {
	if (TokenList[pToken].TYPCOD == 60) {
		pToken++;
		F();

		geq("*");//插的语义动作
	}

	else {
		pToken++;
		F();
		geq("/");
	}
	goto jmp2;
}
}

///<F> -> 标识符 | 常数 | 标识符 [ 常数 ] | ( <E> )
void F() {
	if (TokenList[pToken].TYPCOD == 0 || TokenList[pToken].TYPCOD == 3) {
		if (TokenList[pToken].TYPCOD == 0 && TokenList[pToken + 1].TYPCOD == 68 && TokenList[pToken + 2].TYPCOD == 3 && TokenList[pToken + 3].TYPCOD == 69) {
			SEM.push(SMBLTABL[TokenList[pToken].pp].name + "+" + floatTOstring(2*cons[TokenList[pToken + 2].pp]));///
			pToken += 3;
		}//属于 标识符 [ 常数 ] 这种情况 //插的语义动作

		else if (TokenList[pToken].TYPCOD == 0) {
			SEM.push(SMBLTABL[TokenList[pToken].pp].name);
		}

		else {
			SEM.push(floatTOstring(cons[TokenList[pToken].pp]));
		}
	}

	else {
		if (TokenList[pToken].TYPCOD == 66) {
			pToken++;
			E();
			if (TokenList[pToken].TYPCOD == 67) {}
			else err(8);
		}
		else err(8);//算术表达式错误
	}
	pToken++;
}

///<ifsen> -> if ( <E> 关系算符 <E> ) { <B1> } [else { <B1> }]//方括号表示可有可无
void ifsen()
{
	pToken++;
	if (TokenList[pToken].TYPCOD == 66)//"("
	{
		pToken++;
		E();
		if ((TokenList[pToken].TYPCOD <= 57) && (TokenList[pToken].TYPCOD >= 51) && (TokenList[pToken].TYPCOD != 55))
		{//关系运算符
			int code = TokenList[pToken].TYPCOD;
			pToken++;
			E();
			switch (code)
			{
			case 51:
				geq(">=");
				break;
			case 52:
				geq("<=");
				break;
			case 53:
				geq("==");
				break;
			case 54:
				geq("!=");
				break;
			case 56:
				geq(">");
				break;
			case 57:
				geq("<");
				break;
			default:
				err(9);
			}
		}

		if (TokenList[pToken].TYPCOD == 67)//")"
		{
			pToken++;
			send("if", SEM.top(), "_", "_");
			SEM.pop();
			if (TokenList[pToken].TYPCOD == 62) //"{"
			{
				pToken++;
				B1();
				if (TokenList[pToken].TYPCOD == 63) //"}"
					pToken++;
				if (TokenList[pToken].TYPCOD == 8) //else
				{
					pToken++;
					send("else", "_", "_", "_");
					if (TokenList[pToken].TYPCOD == 62) //"{"
					{
						pToken++;
						B1();
						if (TokenList[pToken].TYPCOD == 63) //"}"
						{
							pToken++;
							send("ifend", "_", "_", "_");
						}
						else err(9);
					}
				}
			}
			else err(9);
		}
		else err(9);
	}
	else err(9);
}

///<whilesen> -> while ( <E> 关系算符 <E> ) { <B1> }
void whilesen()
{
	pToken++;
	send("while", "_", "_", "_");
	if (TokenList[pToken].TYPCOD == 66) //"("
	{
		pToken++;
		E();
		if ((TokenList[pToken].TYPCOD <= 57) && (TokenList[pToken].TYPCOD >= 51))
		{
			int code = TokenList[pToken].TYPCOD;
			pToken++;
			E();
			switch (code)
			{
			case 51:
				geq(">=");
				break;
			case 52:
				geq("<=");
				break;
			case 53:
				geq("==");
				break;
			case 54:
				geq("!=");
				break;
			case 56:
				geq(">");
				break;
			case 57:
				geq("<");
				break;
			default:
				err(10);
			}
		}

		if (TokenList[pToken].TYPCOD == 67) //")"
		{
			pToken++;
			send("do", SEM.top(), "_", "_");
			SEM.pop();
			if (TokenList[pToken].TYPCOD == 62) //"{"
			{
				pToken++;
				B1();
				if (TokenList[pToken].TYPCOD == 63) //"}"
				{
					pToken++;
					send("whileend", "_", "_", "_");
				}
				else err(10);
			}
			else err(10);
		}
		else err(10);
	}
	else err(10);
}



///simplify.h
string str_out3 = "";
//struct Node *PDAG ;
//输出DAG节点树
struct Node *q=NULL;
int ShowDAG(struct Node *DAG) {
	str_out3 += "DAG节点如下:\r\n";
	int i = 0;
	for (i = 0; i <= 29; i++)
	{
		if (DAG[i].Empty == false)
		{
			str_out3 += "该节点编号是:" + intTOstring(DAG[i].NodeNum) + " 操作符是:" + DAG[i].fun + " 主标记是:" + DAG[i].prim + "副标记是:";
			for (int j = 0; j <= 4; j++)
			{
				if (DAG[i].tail[j] != "") str_out3 += DAG[i].tail[j] + ", ";
			}
			if (DAG[i].LChild != -1 && DAG[i].RChild != -1)
			{
				str_out3 += "左孩子是:" + DAG[DAG[i].LChild].prim + " 右孩子是:" + DAG[DAG[i].RChild].prim;
			}
			str_out3 += "\r\n";
		}
	}
	return 0;
}

//检查是否重定义过
string VerRedefine(string str, struct Node *DAG) {
	for (int i = 29; i >= 0; i--)
		for (int j = 0; j <= 4; j++)
		{
			if (DAG[i].tail[j] == str&& i != Num - 1) { str = "redefined1"; return str; }
			if (DAG[i].prim == str&& i != Num - 1) { str = "redefined2"; return str; }
		}
	return str;
}

//找到所在的节点并返回NodeNum
int FindNum(string str, struct Node *DAG) {
	for (int i = 29; i >= 0; i--)
		for (int j = 0; j <= 4; j++)
		{
			if (DAG[i].tail[j] == str && i != Num - 1) { str = "redefined1"; return i; }
			if (DAG[i].prim == str&& i != Num - 1) { str = "redefined2"; return i; }
		}
	return 0;
}

string ftos(float i)   // 将float 转换成string
{
	stringstream s;
	s << i;
	return s.str();
}

//赋值语句增加节点
int InsertEqual(int i, struct Node *DAG) {
	//寻找操作数OP1是否存在DAG节点中
	int Mutpart(string str, struct Node *DAG);
	int c1 = Mutpart(QT[i].op1, q);
	if (c1 != 30)
	{
		int TM = DAG[c1].TailNum;
		DAG[c1].tail[TM] = QT[i].result;
		DAG[c1].TailNum++;
		return 0;
	}
	if (isdigit(QT[i].op1[0]))  //若常值表达式 A=C1wC2 或 A=C1;
	{
		//若 C 在 n1 已定义过则跳else 没定义过进if
		if (VerRedefine(QT[i].result, q) != "redefined1" && VerRedefine(QT[i].result, q) != "redefined2")//如果result不是之前定义的主标记或副标记
		{
			Num++;
			DAG[Num].NodeNum = Num;
			DAG[Num].Empty = false;
			DAG[Num].prim = QT[i].op1;
			DAG[Num].tail[DAG[Num].TailNum] = QT[i].result;
			DAG[Num].fun = QT[i].op;
			DAG[Num].TailNum++;
			return 0;
		}
		else //若 C 在 n1 已定义过
		{
			int Mutpart(string str, struct Node *q);
			int n = FindNum(QT[i].op1, q); int c1 = Mutpart(QT[i].op1, q);
			if (c1 != 30) //若操作数一定义过则将result赋予已定义的赋值节点的副标记上
			{
				int TM = DAG[c1].TailNum;
				DAG[c1].tail[TM] = QT[i].result;
				DAG[c1].TailNum++;
				return 0;
			}
			for (int j = 0; j <= 4; j++) //如果已定义过且不是主标记则删掉
			{
				if (DAG[n].tail[j] == QT[i].result)
				{
					DAG[n].tail[j] = "";
					DAG[n].TailNum--;
				}
			}
			return 0;
		}

		if (VerRedefine(QT[i].result, q) == "redefined1") //若 A 在 n2 已定义过且不是主标记就删除
		{
			int n = FindNum(QT[i].result, q);
			for (int j = 0; j <= 4; j++)
			{
				if (DAG[n].tail[j] == QT[i].result)
					DAG[n].tail[j] = ""; DAG[n].TailNum--;
			}
			return 0;
		}

	}
	else //是equal语句但不是常数赋值
	{
		Num++;
		DAG[Num].NodeNum = Num;
		DAG[Num].Empty = false;
		DAG[Num].prim = QT[i].op1;
		DAG[Num].tail[DAG[Num].TailNum] = QT[i].result;
		DAG[Num].TailNum++;
		//赋值A=B情况
		if (VerRedefine(QT[i].result, q) == "redefined1") //若 A 在 n2 已定义过且不是主标记就删除
		{
			int n = FindNum(QT[i].result, q);
			for (int j = 0; j <= 4; j++)
			{
				if (DAG[n].tail[j] == QT[i].result)
					DAG[n].tail[j] = ""; DAG[n].TailNum--;
			}
			return 0;
		}
	}
	return 1;
}

//寻找公共表达式
int MutExp(int i, int c1, int c2, struct Node *DAG)
{
	if (c1 == 30 || c2 == 30)return 30;
	string str2 = DAG[c1].prim + QT[i].op + DAG[c2].prim;

	for (int j = 29; j >= 0; j--)
	{
		if (DAG[j].LChild != -1 && DAG[j].RChild != -1)
		{
			string str1 = DAG[DAG[j].LChild].prim + DAG[j].fun + DAG[DAG[j].RChild].prim; //拼接字符串寻找公共部分
			if (str1 == str2) { return j; }
		}
	}
	return 30;
}


//寻找相同部分
int Mutpart(string str, struct Node *DAG)
{
	for (int j = 29; j >= 0; j--)
		for (int m = 0; m <= 4; m++)
		{
			string str1 = DAG[j].prim;
			string str2 = DAG[j].tail[m];
			if (str == str1)return j;
			if (str == str2)return j;
		}
	return 30;
}

//其他运算语句增加节点
int InsertFun(int i, struct Node *DAG) {
	//如果是前两个操作数是常数 则先算出结果再按赋值语句处理
	if ((searchcons(QT[i].op1) != -1) && (searchcons(QT[i].op2) != -1))
	{
		float op1 = atof(QT[i].op1.c_str());
		float op2 = atof(QT[i].op2.c_str());
		if (QT[i].op == "+") QT[i].op1 = ftos(op1 + op2);
		if (QT[i].op == "-") QT[i].op1 = ftos(op1 - op2);
		if (QT[i].op == "*") QT[i].op1 = ftos(op1 * op2);
		if (QT[i].op == "/") QT[i].op1 = ftos(op1 / op2);
		QT[i].op = "="; QT[i].op2 = "";
		InsertEqual(i, q);
		return 0;
	}

	else  //如果不是常数运算
	{
		int c1 = Mutpart(QT[i].op1, q);
		int c2 = Mutpart(QT[i].op2, q);
		int n = MutExp(i, c1, c2, q);
		int TN = DAG[n].TailNum;
		//若在n1存在公共表达式：BwC则把A附加在n1上
		if (n != 30) { DAG[n].tail[++TN] = QT[i].result; DAG[n].TailNum++; return 0; }//若结果操作数定义过，则直接将该QT结果赋予定义过的节点副标记上
		Num++;
		DAG[Num].NodeNum = Num;
		DAG[Num].Empty = false;
		DAG[Num].prim = QT[i].result;
		//若第一个操作数已定义过
		if (c1 != 30)
		{
			DAG[Num].LChild = c1;
			if (c2 != 30)//若第二个操作数已定义过
			{
				DAG[Num].RChild = c2;
				DAG[Num].fun = QT[i].op;
				return 0;
			}
			//若第一个操作数已定义过但第二个没定义
			Num++;
			DAG[Num].NodeNum = Num;
			DAG[Num].Empty = false;
			DAG[Num].prim = QT[i].op2;
			DAG[Num - 1].RChild = Num;
			DAG[Num - 1].fun = QT[i].op;
			return 0;
		}
		if (n == 30)//如果结果操作数没有定义过则
		{
			Num++;
			DAG[Num].NodeNum = Num;
			DAG[Num].Empty = false;
			DAG[Num].prim = QT[i].op1;
			Num++;
			DAG[Num].NodeNum = Num;
			DAG[Num].Empty = false;
			DAG[Num].prim = QT[i].op2;
			DAG[Num - 2].LChild = Num - 1;
			DAG[Num - 2].RChild = Num;
			DAG[Num - 2].fun = QT[i].op;
			return 0;
		}

		if (VerRedefine(QT[i].result, q) == "redefined1") //若 A 在 n2 已定义过且不是主标记就删除
		{
			int n = FindNum(QT[i].result, q);
			for (int j = 0; j <= 4; j++)
			{
				if (DAG[n].tail[j] == QT[i].result)
					DAG[n].tail[j] = ""; DAG[n].TailNum--;
			}
			return 0;
		}
	}return 1;
}

//讲QT内容插入DAG节点并建立DAG树
int InsertNode(int i, struct Node *DAG) {
	if (QT[i].op == "=") { InsertEqual(i, q); return 0; }
	if (QT[i].op == "+" || QT[i].op == "-" || QT[i].op == "*" || QT[i].op == "/") { InsertFun(i, q); return 0; }
	if (QT[i].op == "else" || QT[i].op == "ifend" || QT[i].op == "while" || QT[i].op == "whileend")
	{

		Num++;
		DAG[Num].NodeNum = Num; DAG[Num].Empty = false;
		DAG[Num].fun = QT[i].op;
		return 0;

	}
	if (QT[i].op == "if" || QT[i].op == "do" || QT[i].op == ">" || QT[i].op == "<")
	{

		Num++;
		DAG[Num].NodeNum = Num; DAG[Num].Empty = false;
		DAG[Num].fun = QT[i].op;
		DAG[Num].special1 = QT[i].op1;      //三个special成员在DAG树中 但不做优化用，最后直接生成四原式到NEWQT
		DAG[Num].special2 = QT[i].op2;
		DAG[Num].special3 = QT[i].result;
		return 0;

	}
	return 1;
}

//按优先级交换主标记和副标记
void ShiftNode(int n, struct Node *DAG) {
	for (int i = 0; i < n; i++)
		for (int j = 0; j <= 4; j++)
		{
			if (searchcons(DAG[i].prim) != -1) break; //如果主标记是常数 则不用交换 break
			if (searchadmdefine(DAG[i].prim) == 2) //如果主标记是中间变量 交换
			{
				for (int m = 0; m <= 4; m++)
				{
					if (searchadmdefine(DAG[i].tail[m]) == 1 || searchcons(DAG[i].tail[m]) != -1)
					{
						string a = DAG[i].tail[m];
						DAG[i].tail[m] = DAG[i].prim;
						DAG[i].prim = a;
						break;
					}
				}
			}
		}
}

//符号表维护 即删除无用中间变量
void Maintain(struct Node *p) {
	for (int i = 0; i < 30; i++) {
		for (int j = 0; j < 5; j++)
		{
			if (searchadmdefine(p[i].tail[j]) == 2) //如果副节点是中间变量
			{
				vector<SMBLTABLi>::iterator iter;
				for (iter = SMBLTABL.begin(); iter != SMBLTABL.end(); iter++)
				{
					if (*iter == p[i].tail[j]) {
						vector<SMBLTABLi>::iterator it;
						for (it = iter; it != SMBLTABL.end(); it++)
						{
							it->addr -= 2;
						}
						SMBLTABL.erase(iter);
						iter = SMBLTABL.begin(); //防止iter被erase后变成野指针
					}
				}
			}
		}
	}
}

//主函数通过调用Simplify函数实现优化 三个形参分别是块入口地址、块出口地址、第几块对应DAG树的指针
void Simplify(int n, int m, struct Node *p) {
	Num = 0;
	int f = n;
	q = p;
	do {
		InsertNode(n, q);
		ShiftNode(m - f + 1, q);
		n++;
	} while (n <= m);
	ShowDAG(q);
	//遍历DAG节点,输出到优化后的四元式数组
	str_out3 += "该块生成的四元式如下:\r\n";
	for (int i = 0; i < 30; i++)
	{
		if (searchcons(q[i].prim) != -1) //常数赋值的赋值语句
		{
			for (int j = 0; j<5; j++)
			{
				if (searchadmdefine(q[i].tail[j]) == 1)
				{
					str_out3 += "(= " + q[i].prim + " _ " + q[i].tail[j] + ")\r\n";
					NEWQT[NpQT].op = "=";
					NEWQT[NpQT].op1 = q[i].prim;
					NEWQT[NpQT].op2 = "_";
					NEWQT[NpQT].result = q[i].tail[j];
					NpQT++;
				}
			}
		}

		if (q[i].fun == "if" || q[i].fun == "do" || q[i].fun == ">" || q[i].fun == "<") //如果操作符是if/do/>/<的语句
		{
			str_out3 += "(" + q[i].fun + " " + q[i].special1 + " " + q[i].special2 + " " + q[i].special3 + ")\r\n";
			NEWQT[NpQT].op = q[i].fun;
			NEWQT[NpQT].op1 = q[i].special1;
			NEWQT[NpQT].op2 = q[i].special2;
			NEWQT[NpQT].result = q[i].special3;
			NpQT++;
		}

		//如果操作符是ifend/while/whileend/else的语句
		if (q[i].fun == "ifend" || q[i].fun == "while" || q[i].fun == "whileend" || q[i].fun == "else")
		{
			str_out3 += "(" + q[i].fun + " _ _ _)\r\n";
			NEWQT[NpQT].op = q[i].fun;
			NEWQT[NpQT].op1 = "_";
			NEWQT[NpQT].op2 = "_";
			NEWQT[NpQT].result = "_";
			NpQT++;
		}

		if (searchcons(q[i].prim) == -1) //不是常数赋值的赋值语句即中间变量赋值
		{
			for (int j = 0; j<5; j++)
			{
				if (searchadmdefine(q[i].tail[j]) == 1)
				{
					str_out3 += "(= " + q[i].prim + " _ " + q[i].tail[j] + ")\r\n";
					NEWQT[NpQT].op = "=";
					NEWQT[NpQT].op1 = q[i].prim;
					NEWQT[NpQT].op2 = "_";
					NEWQT[NpQT].result = q[i].tail[j];
					NpQT++;
				}
			}
		}
		if (q[i].LChild != -1 && q[i].RChild != -1) //左右孩子均非空，即带两个操作数的语句
		{
			if (q[q[i].LChild].prim != q[q[i].RChild].prim)
			{
				str_out3 += "(" + q[i].fun + " " + q[q[i].LChild].prim + " " + q[q[i].RChild].prim + " " + q[i].prim + ")\r\n";
				NEWQT[NpQT].op = q[i].fun;
				NEWQT[NpQT].op1 = q[q[i].LChild].prim;
				NEWQT[NpQT].op2 = q[q[i].RChild].prim;
				NEWQT[NpQT].result = q[i].prim;
				NpQT++;
			}
			else //常数赋值语句
			{
				if (searchcons(q[q[i].LChild].prim))
				{
					float c1 = atof(q[q[i].LChild].prim.c_str());
					float c2 = atof(q[q[i].RChild].prim.c_str());
					float a;
					if (q[i].fun == "+")  a = c1 + c2;
					if (q[i].fun == "-")  a = c1 - c2;
					if (q[i].fun == "*")  a = c1*c2;
					if (q[i].fun == "/")  a = c1 / c2;
					str_out3 += "(= " + floatTOstring(a) + " _ " + q[i].prim + ")\r\n";
					NEWQT[NpQT].op = "=";
					NEWQT[NpQT].op1 = ftos(a);
					NEWQT[NpQT].op2 = "_";
					NEWQT[NpQT].result = q[i].prim;
					NpQT++;
				}
			}
		}
	}
	Maintain(p);
}



///Outcode.h
//Num为四元式的总个数，i是四元式序号
//只要QT里存有完整的四元式，再分别对相应的四元式调用相应的函数就行
//赋值语句调用assignment_obj(i)
//算术表达式调用expression_obj(i)
//关系(布尔)表达式调用boolexp_obj(i)
//"if","else","ifend"对应的四元式调用ifsen_obj(i)
//"while","do","whileend"对应的四元式调用whilesen_obj(i)
//if语句中的普通语句也需要调用各自相应的函数来生成目标代码
struct OBJ//OBJ
{
	string obj;
	string obj1;
	string obj2;
	int j_num;//i行的列数默认放在零号单元中
	string sign;
	OBJ() { sign = " "; }
}obj[50][20];
string sign_table[] = { "ABC","DEF","GHI","JKL","MNO","PQR","STU" };
stack<int> stack_P1;
string acc = "0";
int flag[50] = { 0 };//汇编语句是否进行了简化，如当上一句的结果是这一句的操作数，即AX不需要再置数

int asmd_obj(int i, int j, string ops);
void expression_obj(int i);//算术表达式OBJ
void assignment_obj(int i);//赋值语句OBJ
void relation_obj(int i);//关系运算符汇编跳转指令的生成
void boolexp_obj(int i);//布尔表达式OBJ
void ifsen_obj(int i);//IF语句OBJ
void whilesen_obj(int i);//while语句OBJ
void check();//检查由化简引起的可能错误

void err()
{
	AfxMessageBox("目标代码生成错误!");
}

//"+-*/"类四元式-->OBJ
int asmd_obj(int i, int j, string ops)
{
	if (NEWQT[i].op == "+")
	{
		obj[i][j].obj = "ADD"; obj[i][j].obj1 = "AX"; obj[i][j].obj2 = ops;
		return j;
	}
	else if (NEWQT[i].op == "-")
	{
		obj[i][j].obj = "SUB"; obj[i][j].obj1 = "AX"; obj[i][j].obj2 = ops;
		return j;
	}
	else if (NEWQT[i].op == "*")
	{
		obj[i][j].obj = "MOV"; obj[i][j].obj1 = "BX"; obj[i][j].obj2 = ops;
		obj[i][j + 1].obj = "MUL"; obj[i][j + 1].obj1 = "BX";//默认所乘结果还是16位，即结果在AX中
		return j + 1;
	}
	else if (NEWQT[i].op == "/")
	{
		obj[i][j].obj = "MOV"; obj[i][j].obj1 = "BX"; obj[i][j].obj2 = ops;
		obj[i][j + 1].obj = "XOR"; obj[i][j + 1].obj1 = "DX"; obj[i][j + 1].obj2 = "DX";//DX清零
		obj[i][j + 2].obj = "DIV"; obj[i][j + 2].obj1 = "BX";//忽略余数
		return j + 2;
	}
	else
	{
		err();
		return 0;
	}
}

void expression_obj(int i)//算术表达式OBJ
{
	int j = 0;
	if ((NEWQT[i].op == "+" || NEWQT[i].op == "-" || NEWQT[i].op == "*" || NEWQT[i].op == "/"))
	{
		if (acc == "0" || (acc != NEWQT[i].op1) && (acc != NEWQT[i].op2))
		{
			obj[i][j].obj = "MOV"; obj[i][j].obj1 = "AX"; obj[i][j].obj2 = NEWQT[i].op1;
			int jj = asmd_obj(i, j + 1, NEWQT[i].op2);
			obj[i][jj + 1].obj = "MOV"; obj[i][jj + 1].obj1 = NEWQT[i].result; obj[i][jj + 1].obj2 = "AX";
			obj[i][0].j_num = jj + 1;
		}
		else
		{
			if (acc == NEWQT[i].op1)
			{
				flag[i] = 1;
				int jj = asmd_obj(i, j, NEWQT[i].op2);
				obj[i][jj + 1].obj = "MOV"; obj[i][jj + 1].obj1 = NEWQT[i].result; obj[i][jj + 1].obj2 = "AX";
				obj[i][0].j_num = jj + 1;
			}
			else if (acc == NEWQT[i].op2)
			{
				if (NEWQT[i].op == "+" || NEWQT[i].op == "*")
				{
					flag[i] = 2;
					int jj = asmd_obj(i, j, NEWQT[i].op1);
					obj[i][jj + 1].obj = "MOV"; obj[i][jj + 1].obj1 = NEWQT[i].result; obj[i][jj + 1].obj2 = "AX";
					obj[i][0].j_num = jj + 1;
				}
				else if (NEWQT[i].op == "-" || NEWQT[i].op == "/")
				{
					obj[i][j + 1].obj = "MOV"; obj[i][j + 1].obj1 = "AX"; obj[i][j + 1].obj2 = NEWQT[i].op1;
					int jj = asmd_obj(i, j + 2, NEWQT[i].op2);
					obj[i][jj + 1].obj = "MOV"; obj[i][jj + 1].obj1 = NEWQT[i].result; obj[i][jj + 1].obj2 = "AX";
					obj[i][0].j_num = jj + 2;
				}
				else err();
			}
			else err();
		}
	}
	acc = NEWQT[i].result;
}

void assignment_obj(int i)//赋值语句OBJ
{
	int j = 0;
	if (NEWQT[i].op == "=")
	{
		if (acc == "0")
		{
			obj[i][j].obj = "MOV"; obj[i][j].obj1 = "AX"; obj[i][j].obj2 = NEWQT[i].op1;
			obj[i][j + 1].obj = "MOV"; obj[i][j + 1].obj1 = NEWQT[i].result; obj[i][j + 1].obj2 = "AX";
			obj[i][0].j_num = j + 1;
		}
		else if (acc == NEWQT[i].op1)
		{
			flag[i] = 1;
			obj[i][j].obj = "MOV"; obj[i][j].obj1 = NEWQT[i].result; obj[i][j].obj2 = "AX";
			obj[i][0].j_num = j;
		}
		else
		{
			obj[i][j].obj = "MOV"; obj[i][j].obj1 = "AX"; obj[i][j].obj2 = NEWQT[i].op1;
			obj[i][j + 1].obj = "MOV"; obj[i][j + 1].obj1 = NEWQT[i].result; obj[i][j + 1].obj2 = "AX";
			obj[i][0].j_num = j + 1;
		}
	}
	acc = NEWQT[i].result;
}

void relation_obj(int i)//关系运算符汇编跳转指令的生成，也即if四元式所生成的汇编指令
{
	if (NEWQT[i].op == ">=")
		obj[i+1][0].obj = "JB";
	else if (NEWQT[i].op == ">")
		obj[i+1][0].obj = "JBE";
	else if (NEWQT[i].op == "<=")
		obj[i+1][0].obj = "JA";
	else if (NEWQT[i].op == "<")
		obj[i+1][0].obj = "JAE";
	else if (NEWQT[i].op == "==")
		obj[i+1][0].obj = "JNE";
	else if (NEWQT[i].op == "!=")
		obj[i+1][0].obj = "JE";
	else err();
	obj[i+1][0].j_num = 0;
}

void boolexp_obj(int i)//布尔表达式OBJ
{
	int j = 0;
	if ((acc == "0") || (acc == NEWQT[i].op2) || ((acc != NEWQT[i].op1) && (acc != NEWQT[i].op2)))
	{
		obj[i][j].obj = "MOV"; obj[i][j].obj1 = "AX"; obj[i][j].obj2 = NEWQT[i].op1;
		obj[i][j + 1].obj = "CMP"; obj[i][j + 1].obj1 = "AX"; obj[i][j + 1].obj2 = NEWQT[i].op2;
		obj[i][0].j_num = j + 1;
	}
	else if (acc == NEWQT[i].op1)
	{
		flag[i] = 1;
		obj[i][j].obj = "CMP"; obj[i][j].obj1 = "AX"; obj[i][j].obj2 = NEWQT[i].op2;
		obj[i][0].j_num = j;
	}
	else err();
	acc = NEWQT[i].result;
}

void ifsen_obj(int i)//IF语句OBJ
{
	obj[i][0].j_num = 0;
	if (NEWQT[i].op == "if")
	{
		relation_obj(i-1);
		stack_P1.push(i);
	}
	if (NEWQT[i].op == "else")
	{
		obj[i + 1][0].sign = sign_table[0];
		int k = stack_P1.top();
		obj[k][0].obj1 = sign_table[0];//地址回填
		stack_P1.pop();
		obj[i][0].obj = "JMP";
		stack_P1.push(i);
	}
	if (NEWQT[i].op == "ifend")
	{
		if (NEWQT[i + 1].op == "while") //下一句是while时的标号处理
			obj[i + 2][0].sign = sign_table[1];
		else
			obj[i + 1][0].sign = sign_table[1];
		int k = stack_P1.top();
		obj[k][0].obj1 = sign_table[1];
		stack_P1.pop();
	}
}

void whilesen_obj(int i)//while语句OBJ
{
	obj[i][0].j_num = 0;
	if (NEWQT[i].op == "while")
	{
		stack_P1.push(i);
	}
	if (NEWQT[i].op == "do")
	{
		relation_obj(i - 1);
		stack_P1.push(i);
	}
	if (NEWQT[i].op == "whileend")
	{
		int k = stack_P1.top();
		obj[i + 1][0].sign = sign_table[2];
		k = stack_P1.top();
		obj[k][0].obj1 = sign_table[2];//地址回填
		stack_P1.pop();

		k = stack_P1.top()+1;
		if (obj[k][0].sign == " ")
		{
			obj[k][0].sign = sign_table[3];
			obj[i][0].obj1 = sign_table[3];
		}
		else
			obj[i][0].obj1 = obj[k][0].sign;
		obj[i][0].obj = "JMP";
		stack_P1.pop();
	}
}

void check()//检查可能由优化引起的错误
{
	int p, q, r;
	for (p = 0; p<NpQT; p++)
	{
		q = obj[p][0].j_num;
		r = q;
		if (obj[p][0].sign != " ")
		{
			if (flag[p] == 1)
			{
				for (; r >= 0; r--)
				{
					obj[p][r + 1].obj = obj[p][r].obj; obj[p][r + 1].obj1 = obj[p][r].obj1; obj[p][r + 1].obj2 = obj[p][r].obj2;
				}

				obj[p][0].obj = "MOV"; obj[p][0].obj1 = "AX"; obj[p][0].obj2 = NEWQT[p].op1;
				obj[p][0].j_num = q + 1;
			}
			if (flag[p] == 2)
			{
				for (r = q; r >= 0; r--)
				{
					obj[p][r + 1].obj = obj[p][r].obj; obj[p][r + 1].obj1 = obj[p][r].obj1; obj[p][r + 1].obj2 = obj[p][r].obj2;
				}
				obj[p][0].obj = "MOV"; obj[p][0].obj1 = "AX"; obj[p][0].obj2 = NEWQT[p].op2;
				obj[p][0].j_num = q + 1;
			}
		}
	}
}

string str_code = "";

int Outcode()
{

	str_code += "DSEG\tSEGMENT\n";
	for (int t = 0; t<SMBLTABL.size(); t++)
	{
		if (SMBLTABL[t].type == -1)
		{
			str_code += SMBLTABL[t].name + "\tDW\t0\n";
		}
		else if (SMBLTABL[t].type == -2) {}
		else
		{
			string str = intTOstring(AINFL[SMBLTABL[t].type].up + 1);
			str_code += SMBLTABL[t].name + "\tDW\t" + str + " DUP(0)\n";
		}

	}
	str_code += "DSEG\tENDS\nCSEG\tSEGMENT\n\tASSUME\tCS:CSEG,DS:DSEG\n";
	str_code += "START:\tMOV\tAX,DSEG\n\tMOV\tDS,AX";
	for (int k = 0; k<NpQT; k++)//生成OBJ
	{
		if (NEWQT[k].op == "+" || NEWQT[k].op == "-" || NEWQT[k].op == "*" || NEWQT[k].op == "/")
			expression_obj(k);
		else if (NEWQT[k].op == "=")
			assignment_obj(k);
		else if (NEWQT[k].op == "if" || NEWQT[k].op == "else" || NEWQT[k].op == "ifend")
			ifsen_obj(k);
		else if (NEWQT[k].op == "while" || NEWQT[k].op == "do" || NEWQT[k].op == "whileend")
			whilesen_obj(k);
		else if (NEWQT[k].op == ">" || NEWQT[k].op == ">=" || NEWQT[k].op == "<" || NEWQT[k].op == "<=" || NEWQT[k].op == "==" || NEWQT[k].op == "!=")
			boolexp_obj(k);
	}
	obj[NpQT][0].obj = "MOV"; obj[NpQT][0].obj1 = "AX"; obj[NpQT][0].obj2 = "4C00H";
	obj[NpQT][1].obj = "INT"; obj[NpQT][1].obj1 = "21H";
	obj[NpQT][0].j_num = 1;

	check();

	for (int i = 0; i <= NpQT; i++)//输出OBJ
		for (int j = 0; j <= obj[i][0].j_num; j++)
		{
			if (obj[i][j].obj == "JA" || obj[i][j].obj == "JB" || obj[i][j].obj == "JAE" || obj[i][j].obj == "JBE"
				|| obj[i][j].obj == "JE" || obj[i][j].obj == "JNE" || obj[i][j].obj == "JMP" || obj[i][j].obj == "MUL"
				|| obj[i][j].obj == "DIV" || obj[i][j].obj == "INT")
			{
				if (obj[i][j].sign == " ")
					str_code += obj[i][j].sign + "\t" + obj[i][j].obj + "\t" + obj[i][j].obj1 + "\n";
				else
					str_code += obj[i][j].sign + ":" + "\t" + obj[i][j].obj + "\t" + obj[i][j].obj1 + "\n";
			}
			else if ((NEWQT[i].op == "while") || (NEWQT[i].op == "ifend")) {}
			else
			{
				if (obj[i][j].sign == " ")
					str_code += obj[i][j].sign + "\t" + obj[i][j].obj + "\t" + obj[i][j].obj1 + "," + obj[i][j].obj2 + "\n";
				else
					str_code += obj[i][j].sign + ":" + "\t" + obj[i][j].obj + "\t" + obj[i][j].obj1 + "," + obj[i][j].obj2 + "\n";
			}
		}
	str_code += "CSEG\tENDS\n\tEND\tSTART\n";
	return 0;
}



///main.cpp
int tcc_main(CString filename, CString filepath) {
	if ((fp = fopen(filename, "r")) == NULL)
	{
		AfxMessageBox("Failed to open the file!");
		exit(1);
	}

	scanner();


	string str_out2 = "";///输出四元式和符号表
	A();
	if (TokenList[pToken].TYPCOD == 71) {
		str_out2 += "语法分析通过.\r\n";
	}
	else err(2);
	str_out2 += "四元式未优化前总计" + intTOstring(pQT) + "个.分别为:\r\n";///pQT为实际数
	for (int i = 0; i <= pQT - 1; i++) {
		str_out2 += intTOstring(i) + ":\t" + QT[i].op + "\t" + QT[i].op1 + "\t" + QT[i].op2 + "\t" + QT[i].result + "\r\n";
	}
	str_out2 += "未优化前符号表项总计" + intTOstring(SMBLTABL.size()) + "项.分别为:\r\n";///SMBLTABLlength为实际数
	for (unsigned int i = 0; i <= SMBLTABL.size() - 1; i++) {
		str_out2 += intTOstring(i) + ":\t" + SMBLTABL[i].name + "\t" + intTOstring(SMBLTABL[i].type) + "\t" + intTOstring(SMBLTABL[i].addr) + "\r\n";
	}
	str_out2 += "数组表项总计" + intTOstring(pAINFL) + "项.分别为:\r\n";///pAINFL为实际数
	for (int i = 0; i <= pAINFL - 1; i++) {
		str_out2 += intTOstring(i) + ":\t" + intTOstring(AINFL[i].low) + "\t" + intTOstring(AINFL[i].up) + "\t" + intTOstring(AINFL[i].ctp) + "\t" + intTOstring(AINFL[i].clen) + "\r\n";
	}

	l1 = inBlock(block1, QT, pQT);
	outBlock(block2, QT, pQT);
	str_out2 += "入口四元式的标号为:\r\n";
	for (int k = 0; k<l1; k++) { str_out2 += intTOstring(block1[k]) + "\t"; }
	str_out2 += "\r\n出口四元式的标号为:\r\n";
	for (int k = 0; k<l1; k++) { str_out2 += intTOstring(block2[k]) + "\t"; }
	CString str_2 = str_out2.c_str();
	Dlg.str2 = str_2;


	for (int k = 0; k<l1; k++) //分块读入不同DAG树进行优化
	{
		str_out3 += "块" + intTOstring(k) + "信息如下:\r\n";
		switch (k) {
		case 0: Simplify(block1[k], block2[k], DAG1); break;
		case 1: Simplify(block1[k], block2[k], DAG2); break;
		case 2: Simplify(block1[k], block2[k], DAG3); break;
		case 3: Simplify(block1[k], block2[k], DAG4); break;
		case 4: Simplify(block1[k], block2[k], DAG5); break;
		case 5: Simplify(block1[k], block2[k], DAG6); break;
		case 6: Simplify(block1[k], block2[k], DAG7); break;
		case 7: Simplify(block1[k], block2[k], DAG8); break;
		case 8: Simplify(block1[k], block2[k], DAG9); break;
		}
		str_out3 += "\r\n";
	}
	str_out3 += "优化后NEWQT中有" + intTOstring(NpQT) + "个四元式\r\nNEWQT内容如下:\r\n";
	for (int n = 0; n < NpQT; n++) { str_out3 += "(" + NEWQT[n].op + " " + NEWQT[n].op1 + " " + NEWQT[n].op2 + " " + NEWQT[n].result + ")\r\n"; }
	str_out3 += "优化后符号表项总计" + intTOstring(SMBLTABL.size()) + "项.分别为:\r\n";///SMBLTABLlength为实际数
	for (unsigned int i = 0; i <= SMBLTABL.size() - 1; i++) {
		str_out3 += intTOstring(i) + ":\t" + SMBLTABL[i].name + "\t" + intTOstring(SMBLTABL[i].type) + "\t" + intTOstring(SMBLTABL[i].addr) + "\r\n";
	}
	CString str_3 = str_out3.c_str();
	Dlg.str3 = str_3;
	Dlg.DoModal();
	//MessageBox(NULL, str_3, "DAG优化:", MB_OK);

	Outcode();



	filepath += "asm";


	CFile cf;

	if (!cf.Open(filepath, CFile::modeCreate | CFile::modeWrite))
	{
		AfxMessageBox(filepath + _T(" Open failed when write."));
		return FALSE;
	}
	CString cstr_code = str_code.c_str();
	int len = cstr_code.GetLength();
	cf.Write(cstr_code.GetBuffer(len), len);
	cf.Close();

	cstr_code.ReleaseBuffer();
	MessageBox(NULL, "目标代码已经生成到原路径!", "目标代码生成:", MB_OK);
	fclose(fp);






	tokennum = 0;
	n = 0;
	token = "";
	arrstr = 0;
	cons.clear();
	SMBLTABL.clear();
	for (int i = 0; i<n; i++)
	{
		Token[i].TYPCOD = 0;
		Token[i].pp = 0;
	}
	for (int i = 0; i<tokennum; i++)
	{
		TokenList[i].TYPCOD = 0;
		TokenList[i].pp = 0;
	}

	temp = 0;
	addr = 0;
	pToken = 0;
	while (!SEM.empty())
		SEM.pop();
	for (int i = 0; i<pAINFL; i++){
		AINFL[i].low = 0;
		AINFL[i].up = 0;
		AINFL[i].ctp = 0;
		AINFL[i].clen = 0;
	}
	pAINFL = 0;
	for (int i = 0; i<pQT; i++){
		QT[i].op = "";
		QT[i].op1 = "";
		QT[i].op2 = "";
		QT[i].result = "";
	}
	pQT = 0;
	for (int i = 0; i < l1;i++) {
		block1[i] = 0;
		block2[i] = 0;
	}
	l1 = 0;
	
	Num = 0;
	for (int i = 0; i<NpQT; i++) {
		NEWQT[i].op = "";
		NEWQT[i].op1 = "";
		NEWQT[i].op2 = "";
		NEWQT[i].result = "";
	}
	NpQT = 0;
	Node *q = NULL;
	for (int i = 0; i<30; i++) {
		DAG1[i].NodeNum = 0;
		DAG1[i].Empty = true;
		DAG1[i].RChild = -1;
		DAG1[i].LChild = -1;
		DAG1[i].TailNum = 0;
		DAG1[i].Num = 0;
		DAG1[i].fun = "";
		DAG1[i].prim = "";
		DAG1[i].tail[0] = "";
		DAG1[i].tail[1] = "";
		DAG1[i].tail[2] = "";
		DAG1[i].tail[3] = "";
		DAG1[i].tail[4] = "";
		DAG1[i].special1 = "";
		DAG1[i].special2 = "";
		DAG1[i].special3 = "";
		DAG2[i].NodeNum = 0;
		DAG2[i].Empty = true;
		DAG2[i].RChild = -1;
		DAG2[i].LChild = -1;
		DAG2[i].TailNum = 0;
		DAG2[i].Num = 0;
		DAG2[i].fun = "";
		DAG2[i].prim = "";
		DAG2[i].tail[0] = "";
		DAG2[i].tail[1] = "";
		DAG2[i].tail[2] = "";
		DAG2[i].tail[3] = "";
		DAG2[i].tail[4] = "";
		DAG2[i].special1 = "";
		DAG2[i].special2 = "";
		DAG2[i].special3 = "";
		DAG3[i].NodeNum = 0;
		DAG3[i].Empty = true;
		DAG3[i].RChild = -1;
		DAG3[i].LChild = -1;
		DAG3[i].TailNum = 0;
		DAG3[i].Num = 0;
		DAG3[i].fun = "";
		DAG3[i].prim = "";
		DAG3[i].tail[0] = "";
		DAG3[i].tail[1] = "";
		DAG3[i].tail[2] = "";
		DAG3[i].tail[3] = "";
		DAG3[i].tail[4] = "";
		DAG3[i].special1 = "";
		DAG3[i].special2 = "";
		DAG3[i].special3 = "";
		DAG4[i].NodeNum = 0;
		DAG4[i].Empty = true;
		DAG4[i].RChild = -1;
		DAG4[i].LChild = -1;
		DAG4[i].TailNum = 0;
		DAG4[i].Num = 0;
		DAG4[i].fun = "";
		DAG4[i].prim = "";
		DAG4[i].tail[0] = "";
		DAG4[i].tail[1] = "";
		DAG4[i].tail[2] = "";
		DAG4[i].tail[3] = "";
		DAG4[i].tail[4] = "";
		DAG4[i].special1 = "";
		DAG4[i].special2 = "";
		DAG4[i].special3 = "";
		DAG5[i].NodeNum = 0;
		DAG5[i].Empty = true;
		DAG5[i].RChild = -1;
		DAG5[i].LChild = -1;
		DAG5[i].TailNum = 0;
		DAG5[i].Num = 0;
		DAG5[i].fun = "";
		DAG5[i].prim = "";
		DAG5[i].tail[0] = "";
		DAG5[i].tail[1] = "";
		DAG5[i].tail[2] = "";
		DAG5[i].tail[3] = "";
		DAG5[i].tail[4] = "";
		DAG5[i].special1 = "";
		DAG5[i].special2 = "";
		DAG5[i].special3 = "";
		DAG6[i].NodeNum = 0;
		DAG6[i].Empty = true;
		DAG6[i].RChild = -1;
		DAG6[i].LChild = -1;
		DAG6[i].TailNum = 0;
		DAG6[i].Num = 0;
		DAG6[i].fun = "";
		DAG6[i].prim = "";
		DAG6[i].tail[0] = "";
		DAG6[i].tail[1] = "";
		DAG6[i].tail[2] = "";
		DAG6[i].tail[3] = "";
		DAG6[i].tail[4] = "";
		DAG6[i].special1 = "";
		DAG6[i].special2 = "";
		DAG6[i].special3 = "";
		DAG7[i].NodeNum = 0;
		DAG7[i].Empty = true;
		DAG7[i].RChild = -1;
		DAG7[i].LChild = -1;
		DAG7[i].TailNum = 0;
		DAG7[i].Num = 0;
		DAG7[i].fun = "";
		DAG7[i].prim = "";
		DAG7[i].tail[0] = "";
		DAG7[i].tail[1] = "";
		DAG7[i].tail[2] = "";
		DAG7[i].tail[3] = "";
		DAG7[i].tail[4] = "";
		DAG7[i].special1 = "";
		DAG7[i].special2 = "";
		DAG7[i].special3 = "";
		DAG8[i].NodeNum = 0;
		DAG8[i].Empty = true;
		DAG8[i].RChild = -1;
		DAG8[i].LChild = -1;
		DAG8[i].TailNum = 0;
		DAG8[i].Num = 0;
		DAG8[i].fun = "";
		DAG8[i].prim = "";
		DAG8[i].tail[0] = "";
		DAG8[i].tail[1] = "";
		DAG8[i].tail[2] = "";
		DAG8[i].tail[3] = "";
		DAG8[i].tail[4] = "";
		DAG8[i].special1 = "";
		DAG8[i].special2 = "";
		DAG8[i].special3 = "";
		DAG9[i].NodeNum = 0;
		DAG9[i].Empty = true;
		DAG9[i].RChild = -1;
		DAG9[i].LChild = -1;
		DAG9[i].TailNum = 0;
		DAG9[i].Num = 0;
		DAG9[i].fun = "";
		DAG9[i].prim = "";
		DAG9[i].tail[0] = "";
		DAG9[i].tail[1] = "";
		DAG9[i].tail[2] = "";
		DAG9[i].tail[3] = "";
		DAG9[i].tail[4] = "";
		DAG9[i].special1 = "";
		DAG9[i].special2 = "";
		DAG9[i].special3 = "";
	}

	acc = "0";
	int flag[50] = { 0 };
	for (int i=0; i<50; i++)
		for (int j=0; j<20; j++)
		{
			obj[i][j].obj = "";
			obj[i][j].obj1 = "";
			obj[i][j].obj2 = "";
			obj[i][j].j_num = 0;
			obj[i][j].sign = "";
		}
	while (!stack_P1.empty())
		stack_P1.pop();
	
	str_out1 = "";
	str_out2 = "";
	str_out3 = "";
	str_code = "";
	//tcc_main(filename, filepath);
	return 0;
}

