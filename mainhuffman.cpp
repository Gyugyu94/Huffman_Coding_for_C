#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct node {
	char c;
	int cnt;
	char *code;
	struct node *next;
	struct node *left;
	struct node *right;
};

struct node *head = 0;
struct node *codehead = 0; // code를 가지고 있는 SLL
struct node *root = 0;
void addSLL(struct node *new_one)
{

	if (head == 0)
	{
		head = new_one;
		return;
	}

	struct node *temp = head;

	while (temp->next != 0)
	{
		temp = temp->next;
	}

	temp->next = new_one;

}

void checkFrequency(char c)
{
	struct node *temp = head;

	while (temp != 0)
	{
		if (temp->c == c)
		{
			temp->cnt += 1;
			return;
		}
		temp = temp->next;
	}

	struct node *new_one = (struct node *)malloc(sizeof(struct node));
	new_one->c = c;
	new_one->cnt = 1;
	new_one->left = new_one->next = new_one->right = 0;
	new_one->code = 0;
	addSLL(new_one);


}

void showFrequency(void)
{
	struct node *temp = head;

	while (temp != 0)
	{
		printf("%c : %d \n", temp->c, temp->cnt);
		temp = temp->next;
	}
}

struct node *findSmallestFromSLL()
{
	struct node *smallest = 0;
	int smallestCnt = INT_MAX;

	if (head == 0)
	{
		return 0;
	}

	struct node *temp = head;
	while (temp != 0)
	{
		if (temp->cnt < smallestCnt)
		{
			smallestCnt = temp->cnt;
			smallest = temp;
		}
		temp = temp->next;
	}

	// smallest가 가리키고 있는 것을 SLL에서 제거한다.
	if (head == smallest) // 맨 앞에 있는 경우
	{
		head = head->next;
	}
	else
	{
		// smallest의 앞에 있는 node를 찾는다.
		struct node *prev = head;
		while (1)
		{
			if (prev->next == smallest)
			{
				prev->next = smallest->next;
				break;
			}
			prev = prev->next;
		}
	}

	smallest->next = 0;
	return smallest;
}

struct node *buildHuffmanTree()
{
	while (1)
	{
		struct node *first = findSmallestFromSLL();
		struct node *second = findSmallestFromSLL();

		if (second == 0)
		{
			return first;
		}

		struct node *new_one = (struct node *)malloc(sizeof(struct node));
		new_one->next = 0;
		new_one->cnt = first->cnt + second->cnt;
		new_one->left = first;
		new_one->right = second;
		new_one->code = 0;

		// SLL의 맨 끝에 new_one을 붙인다.
		addSLL(new_one);
	}
}


char stack[100];
int top = -1;

void push(int v)
{
	top += 1;
	stack[top] = v;
}

int pop()
{
	char result = stack[top];
	top = top - 1;
	return result;
}

void insertCode(char c)
{
	char buf[100]= "";


	for (int i = 0; i <= top; i++)
	{
		buf[i] = stack[i];
	}

	
	struct node *new_one = (struct node *)malloc(sizeof(struct node));
	new_one->c = c;
	new_one->cnt = 0;
	new_one->code = (char *)malloc(strlen(buf)+1);
	new_one->code[0] = '\0';
	strcpy(new_one->code, buf);
	new_one->left = new_one->next = new_one->right = 0;

	if (codehead == 0)
	{
		codehead = new_one;
		return;
	}

	struct node *temp = codehead;

	while (temp->next != 0)
	{
		temp = temp->next;
	}
	temp->next = new_one;

	return;
}


void CreateCodeTable(struct node *_node)
{
	if (_node == 0)
	{
		pop();
		return;
	}
	push('0');
	CreateCodeTable(_node->left);
	if (_node->left == 0 && _node->right == 0)
	{
		insertCode(_node->c);
	}
	push('1');
	CreateCodeTable(_node->right);
	pop();
}

char *findCode(char c)
{
	struct node *temp = codehead;

	while (temp != 0)
	{
		if (temp->c == c)
		{
			return temp->code;
		}
		temp = temp->next;
	}
}

void performEncoding(void)
{
	struct node *temp = codehead;
	int numOfchar = 0;
	while (temp != 0)
	{

		printf("%c : %s \n", temp->c, temp->code);
		numOfchar += 1;
		temp = temp->next;

	}
	FILE *fin = fopen("alice.txt", "rt");

	FILE *fout = fopen("encode.bin", "wb");

	fwrite(&numOfchar, sizeof(char), 1, fout);

	temp = codehead;
	char len = 0;
	while (temp != 0)
	{
		fwrite(&temp->c, sizeof(char), 1, fout);
		len = strlen(temp->code);
		fwrite(&len, sizeof(char), 1, fout);

		fwrite(temp->code, sizeof(char)*len, 1, fout);
		temp = temp->next;
	}


	int idx_for_numofbit = ftell(fout);

	fseek(fout, 4, SEEK_CUR);

	char encodedBuf = 0; // 실제로 인코딩된 내용을 가지고
						 // 파일에 기록하는 바이트 
	int shiftCount = 7;
	int numOfBits = 0;

	while (!feof(fin))
	{
		char buf[256] = "";
		fgets(buf, 256, fin);

		for (int i = 0; i < strlen(buf); i++)
		{
			char code_idx_char = buf[i];
			char *huffcode = findCode(code_idx_char);

			int code_len = strlen(huffcode);

			for (int i = 0; i < code_len; i++)
			{
				int codeChar = huffcode[i];
				if (codeChar == '1')
				{
					char v = 0x01;
					v = v << shiftCount;
					encodedBuf |= v;

				}
				shiftCount -= 1;  // codeChar가 0 일 때도 포함.
				numOfBits += 1;

				if (shiftCount < 0)  // 1byte가 완성되어, 파일에 기록
				{
					fwrite(&encodedBuf, sizeof(char), 1, fout);
					shiftCount = 7;
					encodedBuf = 0;
				}
			}
		}
	}

	if (shiftCount != 7)
	{
		fwrite(&encodedBuf, sizeof(char), 1, fout);
	}

	// numOfBits를 파일의 맨 처음에 기록한다.
	fseek(fout, idx_for_numofbit, SEEK_SET);
	fwrite(&numOfBits, sizeof(int), 1, fout);

	fclose(fout);
	fclose(fin);

}

void performDecoding(void)
{
	struct node *root = (struct node *)malloc(sizeof(struct node));
	root->left = root->right = 0;

	struct node *temp = root;

	FILE *fin = fopen("encode.bin", "rb");
	FILE *fout = fopen("decoding.txt","wt");

	char NumOfChar = 0;
	fread(&NumOfChar, 1, 1, fin);

	printf("Number of Char = %d \n", NumOfChar);

	//첫번째, 허프만 트리 복구

	for (int i = 0; i < NumOfChar; i++)
	{
		char c = 0;
		char code_len = 0;
		char code_idx = 0;
		fread(&c, 1, 1, fin); // 문자
		fread(&code_len, 1, 1, fin);
		temp = root;
		for (int i = 0; i < code_len; i++)
		{
			fread(&code_idx, 1, 1, fin);

			if (code_idx == '0')
			{
				if (temp->left == 0)
				{
					temp->left = (struct node *)malloc(sizeof(struct node));
					temp->left->left = temp->left->right = 0;
				}
				temp = temp->left;

			}
			else
			{
				if (temp->right == 0)
				{
					temp->right = (struct node *)malloc(sizeof(struct node));
					temp->right->left = temp->right->right = 0;
				}
				temp = temp->right;
			}
		}
		temp->c = c;
	}


	int NumOfBits = 0;
	fread(&NumOfBits, 4, 1, fin);

	printf("number of bits  = %d \n", NumOfBits);


	temp = root;

	while (1)
	{
		
		char v;
		int n = fread(&v, sizeof(char), 1, fin);
		if (n <= 0) // 더 이상 읽을 것이 없으면....
		{
			break;
		}

		char andMask = 0x80;  // 1000_0000

		while (1)  // 1바이트를 decoding....
		{
			char result = v & andMask;

			if (result == 0)
			{
				temp = temp->left;
			}
			else
			{
				temp = temp->right;
			}

			if (temp->left == 0 && temp->right == 0)
			{
				fprintf(fout, "%c", temp->c);
				temp = root;
			}

			NumOfBits -= 1;  // 1비트를 풀었음.

			andMask = (andMask >> 1) & 0x7f;

			if (andMask == 0 || NumOfBits == 0)
			{
				break;
			}
		}

	}

}
int main(void)
{
	FILE *f = fopen("alice.txt", "rt");
	char charOfFile = 0;

	while (!feof(f))
	{
		charOfFile = fgetc(f);
		if (charOfFile == -1)
		{
			break;
		}
		checkFrequency(charOfFile);
	}
	fclose(f);

	showFrequency();

	struct node *huffmantree = buildHuffmanTree();
	
	CreateCodeTable(huffmantree);

	printf("\n");

	//performEncoding();
	performDecoding();


	return 0;
}