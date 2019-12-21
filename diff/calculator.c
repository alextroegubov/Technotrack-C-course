#include <assert.h>
#include <stdio.h>
#include <math.h>

int get_G(char *str);
int get_T();
int get_P();
int get_N();
int get_E();
int get_W();
int get_F();
int get_V();
int get_D();

int power(int e, int x){
	int res = 1;
	if(x != 0)
		for(int i = x; i > 0; i--)
			res *= e;

	return res;
}

/*
EX+: {17, 127*(9-1), ..}
EX_: {13^2 + 6, 44}

grammatic rules:

G ::= E#

E ::= T {[+ -]T}*

T ::= F {[/ *]F}* 

F ::= cos(W) | sin(W) | W

W :: = P{^P}?

P ::= (E) | N | V

V ::= [A-Z]

/////D ::= N {. N}?

N ::= [0-9]+

*/

char *s = NULL;

int get_G(char *str){
	
	s = str;
	int val = get_E();
	assert(*s == '\0');
	return val;
}

int get_E(){
	int val = get_T();

	while(*s == '+' || *s == '-'){
		char op = *s;
		s++;
		int val2 = get_T();

		if(op == '+')
			val += val2;
		else if(op == '-')
			val -= val2;
		else
			assert(!"+-");
	}
	return val;
}

int get_T(){
	int val = get_F();

	while(*s == '*' || *s == '/'){		
		char op = *s;
		s++;
		int val2 = get_F();

		if(op == '*')
			val *= val2;
		else if(op == '/')
			val /= val2;
		else
			assert(!"*/");
	}
	return val;
}

int get_F(){
	int val1 = 0;
	
	if((*s == 's' && *(s+1) == 'i' && *(s+2) == 'n' && *(s+3) == '(') ||
		(*s == 'c' && *(s+1) == 'o' && *(s+2) == 's' && *(s+3) == '(')){
		char op = *s;
		s += 4;
		val1 = get_W();
		assert(*s == ')');
		s++;
		val1 = (op == 's') ? sin(val1) : cos(val1);
	}
	else
		val1 = get_W();

	return val1;
}

int get_W(){
	int val1 = get_P();

	if(*s == '^'){
		s++;
		int val2 = get_P();		
		val1 = power(val1, val2);
	}
	return val1;
}

int get_P(){
	
	if(*s == '('){		
		s++;
		int val = get_E();		
		assert(*s == ')');
		s++;
		return val;
	}
	else if('0' <= *s && *s <= '9')
		return get_N();
	else 
		return get_V();
}

//all variables are 0
int get_V(){
	char val = 0;
	if('A' <= *s && *s <= 'Z'){
		val = *s;
		s++;
	}
	return 0;
}

int get_N(){
	int val = 0;

	do{		
		val = val * 10 + *s - '0';		
		s++;

	}while('0' <= *s && *s <= '9');

	return val;
}

/*
int get_D(){
	int val = 0;
	char n[100] = {0};
	int i = 0;

	int val1 = get_N();
	
}
*/
int main(){

	char str[100] = {'\0'};
	scanf("%s", str);
	printf("%d\n", get_G(str));

	return 0;
}
