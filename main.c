#include<stdio.h>
#include<stdlib.h>
#include<string.h>
typedef struct ShaMes{
    unsigned char* mes;
    int len;//单位是字节
}ShaMes;
unsigned int A = 0x67452301;
unsigned int B = 0xEFCDAB89;
unsigned int C = 0x98BADCFE;
unsigned int D = 0x10325476;
unsigned int K[] = { 
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501, 
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, 
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a, 
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, 
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1, 
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};
unsigned int S[] = { 
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 
    5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 };
//num循环左移x位
unsigned int RL(unsigned int num,unsigned int x){
    return (num << x) | (num >> (32 - x));
}
//反向unsigned int
unsigned int ReverseUnsignedInt(unsigned int x){
    unsigned char a[] = {x,x>>8,x>>16,x>>24};
    unsigned int y = 0;
    for(int i=0;i<4;i++){
        y += a[i];
        if(i<3){
            y = y<<8;
        }
    }
    return y;
}

//消息预处理——补位
ShaMes* HandleMessage(char *message){
    
    ShaMes *s = (ShaMes *)malloc(sizeof(ShaMes));
    int mlen = strlen(message);
    int last = (mlen*8) % 512;
    if (last < 448){
        last = 448 - last;
    }else if (last >= 448){
        last = 512 - (last - 448);
    }
    message = (char*)realloc(message,mlen+last/8+8);
    int len = mlen;
    message[len] = 0x80;
    //printf("%s\n",message);
    for (int i=1;i<last/8;i++){
        message[++len] = 0x00;
    }
    for (int i=0;i<4;i++){
        message[++len] = (mlen*8)>>(i*8);
    }
    for (int i=0;i<4;i++){
        message[++len] = 0x00;
    }
    s->mes = message;
    s->len = len+1;
    return s;
}

unsigned int* SplitMessage(ShaMes *s){
    unsigned int* m = (unsigned int*)malloc(s->len/4*sizeof(unsigned int));
    memset(m,0,s->len/4*sizeof(unsigned int));
    for (int i=0;i<s->len/4;i++){
        for (int j=3;j>=0;j--){ 
            m[i] += (unsigned char)s->mes[i*4+j];
            if (j>0){
                m[i] = m[i] << 8;
            }
        }
        printf("i=%x,mes=%08X\n",i,m[i]);
    }
    // for (int i=16;i<s->len/4;i++){
    //     for (int j=0;j<4;j++){ 
    //         m[i] += (unsigned char)s->mes[i*4+j];
    //         if (j<3){
    //             m[i] = m[i] << 8;
    //         }
    //     }
    //     printf("i=%x,mes=%08X\n",i,m[i]);
    // }
    return m;
}
void Hash(unsigned int* W,int len){
    for (int k=0;k<len/16;k++){
        unsigned int a = A;
        unsigned int b = B;
        unsigned int c = C;
        unsigned int d = D;
        unsigned int f,g;
        for (int i=0;i<64;i++){
            if (i>=0 && i<=15){
                f = (b&c) | (~b&d);
                g = i;
            }else if (i>=16 && i<=31){
                f = (d&b) | (~d&c);
                g = (5*i+1)%16;
            }else if (i>=32 && i<=47){
                f = b^c^d;
                g = (3*i+5)%16;
            }else{
                f = c ^ (b|~d);
                g = (7*i)%16;
            }
            unsigned int temp = d;
            d = c;
            c = b;
            b = RL(a+f+K[i]+W[k*16+g],S[i]) + b;
            a = temp;
        }
        A += a;
        B += b;
        C += c;
        D += d;
    }
}
void Md5(char* message){
    ShaMes *shaMes = HandleMessage(message);//得到补位后的message
    unsigned int *m = SplitMessage(shaMes);//拆分message
    Hash(m,shaMes->len/4);
}
int main(){
    char *str = "hello,world!";
    char *message = (char*)malloc(strlen(str));
    strcpy(message,str);
    printf("raw message is %s\n",message);
    Md5(message);
    A = ReverseUnsignedInt(A);
    B = ReverseUnsignedInt(B);
    C = ReverseUnsignedInt(C);
    D = ReverseUnsignedInt(D);
    printf("%08x%08x%08x%08x\n",A,B,C,D);
    return 0;
}