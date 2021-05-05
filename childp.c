#include <stdio.h>
#include <windows.h>
#define BUFFER_SIZE 25
int main(VOID)
{
HANDLE ReadHandle; // handle per lexim ne procesin femije
HANDLE WriteHandleO; // handle per shkrim ne procesin femije
CHAR buffer[BUFFER_SIZE]; // ruhet mesazhi qe dergon procesi prind
DWORD read; // pointer per lexim te mesazhit
DWORD written; // pointer per shkrim te mesazhit

/* Procesi merr readHandle-in e trasheguar nga pipe1 (inicializon ate me standardin e hyrjes) */
ReadHandle = GetStdHandle(STD_INPUT_HANDLE);
/* Procesi merr writeHandle-in e trasheguar nga pipe2 (inicializon ate me standardin e daljes) */
WriteHandleO = GetStdHandle(STD_OUTPUT_HANDLE);

// Dergojme tekst ne daljen standarde te procesit edhe permes "printf"
	printf("\n ** mesazhi_i_child_per_parent ** \n");

/* Procesi femije lexon mesazhin e procesit prind nga pipe1 */
if (ReadFile(ReadHandle, buffer, BUFFER_SIZE, &read, NULL))
printf("Procesi femije lexoi: %s",buffer);
else
fprintf(stderr, "Procesi femije deshtoi te lexoje nga pipe");

// procesi femije dergon mesazhin per procesin prind permes pipe2
WriteFile(WriteHandleO,"Hello from child", BUFFER_SIZE, &written, NULL);

return 0;
}