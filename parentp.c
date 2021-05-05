#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#define BUFFER_SIZE 25
int main(VOID)
{
   HANDLE ReadHandle, WriteHandle; // per hyrje teksti ne child
   HANDLE ReadHandleO, WriteHandleO; // per dalje teksti nga child
   STARTUPINFO si; // krijimi i instances se STARTUPINFO (specifikon vetite e sistemit)
   PROCESS_INFORMATION pi; // strukture qe identifikon procesin e ri te krijuar
   char message[BUFFER_SIZE] = "Pershendetje"; // mesazhi qe do te dergohet ne procesin femije
   DWORD written; // pointer per shkrim ne pipe
 
   /* percaktimi i atributeve te sigurise qe i mundeson pipe(tubave) te trashegohen */
   SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
   
   /* alokimi i memories */
   ZeroMemory(&pi, sizeof(pi));
   
   /* krijojme njerin pipe (pipe1) */
   if (!CreatePipe(&ReadHandle, &WriteHandle, &sa, 0))
   {
      fprintf(stderr, "Krijimi i pipet te input-it per child deshtoi");
      return 1;
   }

   /* Sigurohemi qe te mos trashegojme tek procesi femije writeHandle te pipe1 */
   SetHandleInformation(WriteHandle, HANDLE_FLAG_INHERIT, 0);

   /* krijojme pipe-in tjeter per lexim nga child procesi (pipe2) */
   if (!CreatePipe(&ReadHandleO, &WriteHandleO, &sa, 0))
   {
      fprintf(stderr, "Krijimi i pipet te output-it per child deshtoi");
      return 1;
   }

   /* Sigurohemi qe te mos trashegojme tek procesi femije readHandle te pipe2 */
   SetHandleInformation(ReadHandleO, HANDLE_FLAG_INHERIT, 0);

   // Percaktojme anetaret e struktures STARTUPINFO (specifikon vetite e procesit te krijuar) 
   ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO); // cb – madhësia (size) e strukturës, në bajt
	si.hStdError = WriteHandleO;
   // ridrejton daljen standarde per shkrim nga skaji i pipe1  
	si.hStdOutput = WriteHandleO;
   // ridrejton hyrjen standarde per lexim nga skaji i pipe2
	si.hStdInput = ReadHandle;
	si.dwFlags = STARTF_USESTDHANDLES;
   
   /* krijimi i procesit femije */
   if (!CreateProcess(NULL, "childp.exe", NULL, NULL, TRUE, /* inherit handles */ 0, NULL, NULL, &si, &pi))
   {
      printf("Deshton krijimi i procesit\n");
   }
   
   /* mbyllen skajet e paperdorura te pipes */ 
   CloseHandle(ReadHandle);
   CloseHandle(WriteHandleO);
   
   /* procesi prind shkruan ne pipe */
   if (!WriteFile(WriteHandle, message, BUFFER_SIZE, &written, NULL))
      fprintf(stderr, "Mesazhi nuk u shkrua ne pipe.");

   /* mbyllet writeEnd i pipe1 */ 
   CloseHandle(WriteHandle);

   /* procesi prind lexon mesazh nga procesi femije dhe ndalon kur nuk ka me mesazhe */
   DWORD dwRead;
	CHAR chBuf[BUFFER_SIZE];
	BOOL bSuccess = FALSE;
	HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	for (;;)
	{
      // Lexon mesazhin e procesit femije nga pipe2
		bSuccess = ReadFile(ReadHandleO, chBuf, BUFFER_SIZE, &dwRead, NULL);
		if (!bSuccess || dwRead == 0) break;
      
      // shkruan ne handle te tij per te shfaqur tekstin ne ekran
		bSuccess = WriteFile(hParentStdOut, chBuf,
			dwRead, &written, NULL);
		if (!bSuccess) break;
	}
   
   /* Procesi prind pret derisa te perfundoje se ekzekutuari procesi femije */
   WaitForSingleObject(pi.hProcess, INFINITE);
   CloseHandle(pi.hProcess);
   CloseHandle(pi.hThread);

   return 0;
}