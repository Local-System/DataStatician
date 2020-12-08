#include "GlobalDef.h"
int main();
void flush();
void Hide();
void Show();
void gotoxy(int,int);
bool GetFilesW(wstring, vector<WIN32_FIND_DATAW>&);
bool FilePathExists(wstring);
int WaitForResponse();
void ManageData(vector<wstring>&);
void CreateDataStor(vector<wstring>&);
int GetNumber(int,int,int);
int ScanNumberKey();
void WaitForAllKeyUp();
void WelcomeFrame();
int main() {
	SetConsoleTextAttribute(hConsole, 7);
	wchar_t filePath[1025];
	GetCurrentDirectoryW(1025,filePath);
	szDir=filePath;
	wstring wszTemp;
	szDir+=L"\\DataStorage";
	if(!FilePathExists(szDir)) {
		SetConsoleTextAttribute(hConsole, 12);
		cout<<"Storage directory does not exist or is missing.\r\nCreating...\r";
		do {
			CreateDirectoryW(szDir.c_str(),NULL);
		} while(!FilePathExists(szDir));
		SetConsoleTextAttribute(hConsole, 7);
		cout<<"Storage directory created.\r\n";
		SetCurrentDirectoryW(szDir.c_str());
		wszTemp=szDir;
		wszTemp.assign(L"DataStor.dbcc");
		fsIO.open("DataStor.dbdc",ios::out);
		fsIO.seekg(ios::beg);
		fsIO.close();
		cout<<"Data storage description file (DataStor.dbdc) created.\r\n";
	} else {
		SetCurrentDirectoryW(szDir.c_str());
		wszTemp=szDir;
		wszTemp.assign(L"DataStor.dbdc");
		if(!FilePathExists(wszTemp)) {
			SetConsoleTextAttribute(hConsole, 12);
			cout<<"Data storage description file (DataStor.dbdc) is not available.\r\nCreating...\r";
			SetConsoleTextAttribute(hConsole, 7);
			fsIO.open("DataStor.dbdc",ios::out);
			fsIO.seekg(ios::beg);
			fsIO.close();
			cout<<"Data storage description file (DataStor.dbdc) created.\r\n";
		}
	}
	system("mode con lines=40 cols=128");
	WelcomeFrame();
	return 0;
}
void WelcomeFrame() {
	SetConsoleTextAttribute(hConsole, 10);
	wcout<<L"Welcome!\r\n";
	SetConsoleTextAttribute(hConsole, 7);
	vector<wstring> vCreated;
	vector<wstring> vNames;
	fsIO.open("DataStor.dbdc",ios::in);
	wstring wszTemp;
	wcout<<L"Reading data sub-storage from database...\r\n";
	while(fsIO>>wszTemp) {
		vCreated.push_back(wszTemp);
	}
	fsIO.close();
	bool bWarnOut=false;
	for(int i=0,e=vCreated.size(); i<e; i++) {
		if(!fbIsGlobalUniqueID(vCreated[i])) {
			bWarnOut=true;
			wcout<<L"WARNING: ";
			SetConsoleTextAttribute(hConsole, 14);
			wcout<<L"\""<<vCreated[i]<<L"\" is not a valid GUID.\r\nIt will be ignored."<<endl;
			vCreated.erase(vCreated.begin()+i);
			SetConsoleTextAttribute(hConsole, 7);
			i--;
			e--;
		}
	}
	wszTemp=szDir;
	if(bWarnOut) {
		wcout<<endl;
		bWarnOut=false;
	}
	for(int i=0,e=vCreated.size(); i<e; i++) {
		if(!FilePathExists(wszTemp+L'\\'+vCreated[i])) {
			bWarnOut=true;
			wcout<<L"WARNING: ";
			SetConsoleTextAttribute(hConsole, 14);
			wcout<<L"\""<<vCreated[i]<<L"\" does not exist.\r\nIt will be ignored."<<endl;
			vCreated.erase(vCreated.begin()+i);
			SetConsoleTextAttribute(hConsole, 7);
			i--;
			e--;
		}
	}
	if(bWarnOut) {
		wcout<<endl;
	}
	for(int i=0,e=vCreated.size(); i<e; i++) {
		if(!FilePathExists(wszTemp+L'\\'+vCreated[i]+L"\\BaseConf.cfg")) {
			wcout<<L"WARNING: ";
			SetConsoleTextAttribute(hConsole, 14);
			wcout<<L"Configuration file (BaseConf.cfg) of \""<<vCreated[i]<<L"\" is missing.\r\nIt will be ignored."<<endl;
			vCreated.erase(vCreated.begin()+i);
			SetConsoleTextAttribute(hConsole, 7);
			i--;
			e--;
		}
	}
	Sleep(3000);
	clrscr(hConsole);
	gotoxy(0,1);
	SetConsoleTextAttribute(hConsole, 15);
	wcout<<"Sub-database available:";
	SetConsoleTextAttribute(hConsole, 7);
	int nValid=vCreated.size();
	for(int i=0; i<nValid; i++) {
		if(i>=23) {
			gotoxy(1,25);
			wcout<<L"... ("<<nValid-23<<" more)";
			break;
		}
		gotoxy(1,i+2);
		wszTemp=szDir+L'\\'+vCreated[i];
		SetCurrentDirectoryW(wszTemp.c_str());
		fsIO.open("BaseConf.cfg",ios::in);
		fsIO>>wszTemp;
		fsIO.close();
		wcout<<wszTemp;
		vNames.push_back(wszTemp);
	}
	gotoxy(0,nValid+3);
	SetConsoleTextAttribute(hConsole, 15);
	wcout<<L"What would you like to do?";
	SetConsoleTextAttribute(hConsole, 7);
	wstring wszChoices[]= {L"1.View/Edit the storage...",L"2.Create new datastore area...",L"3.Delete a storage area..."};
	int nMaxCnt=3,nCurrentSection=0;
	while(true) {
		Hide();
		for(int i=0; i<nMaxCnt; i++) {
			gotoxy(0,nValid+4+i);
			if(i==nCurrentSection) {
				SetConsoleTextAttribute(hConsole, 240);
			}
			wcout<<wszChoices[i];
			SetConsoleTextAttribute(hConsole, 7);
		}
		int nKeyPress=WaitForResponse();
		if(nKeyPress==0) {
			switch(nCurrentSection) {
				case 0:
					ManageData(vCreated);
				case 1:
					CreateDataStor(vCreated);
			}
			gotoxy(0,1);
			SetConsoleTextAttribute(hConsole, 15);
			wcout<<"Sub-database available:";
			SetConsoleTextAttribute(hConsole, 7);
			for(int i=0; i<nValid; i++) {
				if(i>=23) {
					gotoxy(1,25);
					wcout<<L"... ("<<nValid-23<<" more)";
					break;
				}
				gotoxy(1,i+2);
				wcout<<vNames[i];
			}
			gotoxy(0,nValid+3);
			SetConsoleTextAttribute(hConsole, 15);
			wcout<<L"What would you like to do?";
			SetConsoleTextAttribute(hConsole, 7);
		}
		if(nKeyPress==1) {
			nCurrentSection--;
		}
		if(nKeyPress==2) {
			nCurrentSection++;
		}
		if(nKeyPress==3) {
			nCurrentSection=GetNumber(nMaxCnt,19,nValid+nMaxCnt+5);
		}
		nCurrentSection+=nMaxCnt;
		nCurrentSection%=nMaxCnt;
	}
}
void CreateDataStor(vector<wstring>& vCreated){
	clrscr(hConsole);
}
void ManageData(vector<wstring>& vCreated) {
	clrscr(hConsole);
	gotoxy(0,1);
	SetConsoleTextAttribute(hConsole, 15);
	wcout<<"Manage/View:";
	gotoxy(0,39);
	SetConsoleTextAttribute(hConsole, 10);
	wcout<<"Press Escape key to go back...";
	SetConsoleTextAttribute(hConsole, 7);
	int nValid=vCreated.size();
	vector<wstring> vNames;
	wstring wszTemp;
	for(int i=0; i<nValid; i++) {
		wszTemp=szDir+L'\\'+vCreated[i];
		SetCurrentDirectoryW(wszTemp.c_str());
		fsIO.open("BaseConf.cfg",ios::in);
		fsIO>>wszTemp;
		fsIO.close();
		vNames.push_back(wszTemp);
	}
	int nMaxCnt=nValid,nCurrentSection=0;
	int nCurX,nCurY;
	while(true) {
		Hide();
		for(int i=0; i<nMaxCnt; i++) {
			gotoxy(1,i+2);
			if(i==nCurrentSection) {
				SetConsoleTextAttribute(hConsole, 240);
			}
			wcout<<vNames[i];
			SetConsoleTextAttribute(hConsole, 7);
		}
		int nKeyPress=WaitForResponse();
		if(nKeyPress==0) {
		}
		if(nKeyPress==1) {
			nCurrentSection--;
		}
		if(nKeyPress==2) {
			nCurrentSection++;
		}
		if(nKeyPress==3) {
			nCurrentSection=GetNumber(nMaxCnt,19,nValid+nMaxCnt+5);
		}
		if(nKeyPress==4) {
			clrscr(hConsole);
			return;
		}
		nCurrentSection+=nMaxCnt;
		nCurrentSection%=nMaxCnt;
	}
}
int GetNumber(int nLimit,int nOutX,int nOutY) {
	Show();
	int nResult=0;
	int nNumkey;
	bool bInputBack;
	bool bReturnBack;
	gotoxy(0,nOutY);
	wcout<<L"Enter a option ID:";
	while(true) {
		while(!((bInputBack=KEY_INPUT(VK_BACK))==true||(bReturnBack=KEY_INPUT(VK_RETURN))==true||(nNumkey=ScanNumberKey())!=-1));
		if(bReturnBack) {
			if(nResult!=0) {
				Hide();
				gotoxy(0,nOutY);
				wcout<<L"                                                                        ";
				return nResult-1;
			}
		}
		if(nNumkey!=-1) {
			nResult*=10;
			nResult+=nNumkey;
		}
		if(bInputBack) {
			nResult/=10;
		}
		if(nResult>nLimit) {
			nResult=nLimit;
		}
		gotoxy(0,nOutY);
		wcout<<L"Enter a option ID:"<<nResult<<"          ";
		gotoxy(0,nOutY);
		wcout<<L"Enter a option ID:"<<nResult;
		Sleep(1);
	}
}
void WaitForAllKeyUp() {
	bool bFlag;
	do {
		bFlag=false;
		if(KEY_DOWN(VK_UP)) {
			bFlag=true;
		}
		if(KEY_DOWN(VK_RETURN)) {
			bFlag=true;
		}
		if(KEY_DOWN(VK_DOWN)) {
			bFlag=true;
		}
		if(KEY_DOWN(VK_TAB)) {
			bFlag=true;
		}
	} while(bFlag);
}
int WaitForResponse() {
	while(true) {
		if(KEY_INPUT(VK_UP)) {
			return 1;
		}
		if(KEY_INPUT(VK_RETURN)) {
			return 0;
		}
		if(KEY_INPUT(VK_DOWN)) {
			return 2;
		}
		if(KEY_INPUT(VK_TAB)) {
			return 3;
		}
		if(KEY_INPUT(VK_ESCAPE)) {
			return 4;
		}
	}
}
int ScanNumberKey() {
	if(KEY_INPUT('0')) {
		return 0;
	}
	if(KEY_INPUT('1')) {
		return 1;
	}
	if(KEY_INPUT('2')) {
		return 2;
	}
	if(KEY_INPUT('3')) {
		return 3;
	}
	if(KEY_INPUT('4')) {
		return 4;
	}
	if(KEY_INPUT('5')) {
		return 5;
	}
	if(KEY_INPUT('6')) {
		return 6;
	}
	if(KEY_INPUT('7')) {
		return 7;
	}
	if(KEY_INPUT('8')) {
		return 8;
	}
	if(KEY_INPUT('9')) {
		return 9;
	}
	return -1;
}
void flush() {
	while(kbhit()) {
		getch();
	}
}
