#include <iostream>
#include "TCPKernel.h"
using namespace std;

int main(){
	TCPKernel *pKernel = new TCPKernel;
	if(pKernel->Open())
		cout<<"�����������ɹ�����������....."<<endl;

	system("pause");
	return 0;
}